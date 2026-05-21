#include "hash_dictionary.h"
#include "text_utils.h"

#include <iostream>

using namespace std;

HashDictionary::Entry::Entry(const string &value)
    : word(value), count(1), next(nullptr) {}

HashDictionary::HashDictionary()
    : _cells(nullptr), _capacity(0), _unique_words(0), _total_words(0) {
    init_cells(HASH_INITIAL_CAPACITY);
}

HashDictionary::~HashDictionary() { destroy_cells(); }

unsigned long long HashDictionary::hash_value(const string &word) {
    unsigned long long hash = 0;
    for (unsigned char byte : word) {
        hash = hash * HASH_MULTIPLIER + byte;
    }
    return hash;
}

size_t HashDictionary::cell_index(const string &word) const {
    return static_cast<size_t>(hash_value(word) % _capacity);
}

void HashDictionary::init_cells(size_t capacity) {
    _capacity = capacity;
    _cells = new Entry *[_capacity];
    for (size_t i = 0; i < _capacity; ++i)
        _cells[i] = nullptr;
}

void HashDictionary::destroy_cells() {
    if (_cells == nullptr)
        return;

    for (size_t i = 0; i < _capacity; ++i) {
        Entry *current = _cells[i];
        while (current != nullptr) {
            Entry *next = current->next;
            delete current;
            current = next;
        }
    }

    delete[] _cells;
    _cells = nullptr;
    _capacity = 0;
    _unique_words = 0;
    _total_words = 0;
}

void HashDictionary::rehash(size_t new_capacity) {
    Entry **old_cells = _cells;
    size_t old_capacity = _capacity;

    init_cells(new_capacity);
    _unique_words = 0;

    for (size_t i = 0; i < old_capacity; ++i) {
        Entry *current = old_cells[i];
        while (current != nullptr) {
            Entry *next = current->next;
            size_t index = cell_index(current->word);
            current->next = _cells[index];
            _cells[index] = current;
            ++_unique_words;
            current = next;
        }
    }

    delete[] old_cells;
}

bool HashDictionary::insert(const string &raw_word) {
    string word = text_utils::normalize_word(raw_word);
    if (word.empty())
        return false;

    size_t index = cell_index(word);
    Entry *current = _cells[index];
    while (current != nullptr) {
        if (current->word == word) {
            ++current->count;
            ++_total_words;
            return false;
        }
        current = current->next;
    }

    Entry *entry = new Entry(word);
    entry->next = _cells[index];
    _cells[index] = entry;
    ++_unique_words;
    ++_total_words;

    if (_unique_words * HASH_LOAD_LIMIT_D >
        _capacity * HASH_LOAD_LIMIT_N)
        rehash(_capacity * HASH_GROWTH_FACTOR);

    return true;
}

bool HashDictionary::erase(const string &raw_word) {
    string word = text_utils::normalize_word(raw_word);
    size_t index = cell_index(word);
    Entry *current = _cells[index];
    Entry *previous = nullptr;

    while (current != nullptr) {
        if (current->word == word) {
            if (previous == nullptr)
                _cells[index] = current->next;
            else
                previous->next = current->next;

            _total_words -= current->count;
            --_unique_words;
            delete current;
            return true;
        }
        previous = current;
        current = current->next;
    }

    return false;
}

bool HashDictionary::contains(const string &word) const {
    return count(word) > 0;
}

int HashDictionary::count(const string &raw_word) const {
    string word = text_utils::normalize_word(raw_word);
    size_t index = cell_index(word);
    Entry *current = _cells[index];

    while (current != nullptr) {
        if (current->word == word)
            return current->count;
        current = current->next;
    }

    return 0;
}

void HashDictionary::clear() {
    destroy_cells();
    init_cells(HASH_INITIAL_CAPACITY);
}

namespace {
void hash_insert_callback(void *context, const string &word) {
    static_cast<HashDictionary *>(context)->insert(word);
}
} // namespace

size_t HashDictionary::load_from_file(const string &path, string &error) {
    return text_utils::load_words_from_file(path, this, hash_insert_callback,
                                            error);
}

void HashDictionary::print(ostream &out, int limit) const {
    if (_unique_words == 0) {
        out << "Словарь пуст\n";
        return;
    }

    int printed = 0;
    for (size_t i = 0; i < _capacity && printed < limit; ++i) {
        Entry *current = _cells[i];
        while (current != nullptr && printed < limit) {
            out << current->word << " (" << current->count << ")\n";
            ++printed;
            current = current->next;
        }
    }
}

void HashDictionary::print_table(ostream &out) const {
    for (size_t i = 0; i < _capacity; ++i) {
        out << "[" << i << "] ";

        Entry *current = _cells[i];
        if (current == nullptr) {
            out << "пусто\n";
            continue;
        }

        while (current != nullptr) {
            out << current->word << "(" << current->count << ")";
            if (current->next != nullptr)
                out << " -> ";
            current = current->next;
        }
        out << "\n";
    }
}

size_t HashDictionary::unique_words() const { return _unique_words; }

size_t HashDictionary::total_words() const { return _total_words; }

size_t HashDictionary::capacity() const { return _capacity; }

size_t HashDictionary::collision_cells() const {
    size_t collisions = 0;
    for (size_t i = 0; i < _capacity; ++i) {
        if (_cells[i] != nullptr && _cells[i]->next != nullptr)
            ++collisions;
    }
    return collisions;
}

size_t HashDictionary::max_chain_length() const {
    size_t max_length = 0;
    for (size_t i = 0; i < _capacity; ++i) {
        size_t length = 0;
        Entry *current = _cells[i];
        while (current != nullptr) {
            ++length;
            current = current->next;
        }
        if (length > max_length)
            max_length = length;
    }
    return max_length;
}
