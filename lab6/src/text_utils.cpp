#include "text_utils.h"

#include <fstream>
#include <iterator>

using namespace std;

namespace {

void append_byte(string &word, unsigned char byte) {
    word.push_back(static_cast<char>(byte));
}

void append_russian_letter(string &word, unsigned char first,
                           unsigned char second) {
    append_byte(word, first);
    append_byte(word, second);
}

bool append_letter(const string &text, size_t &pos, string &word) {
    unsigned char c = static_cast<unsigned char>(text[pos]);

    if (c >= 'A' && c <= 'Z') {
        append_byte(word, c - 'A' + 'a');
        ++pos;
        return true;
    }

    if (c >= 'a' && c <= 'z') {
        append_byte(word, c);
        ++pos;
        return true;
    }

    if (pos + 1 >= text.size())
        return false;

    unsigned char first = static_cast<unsigned char>(text[pos]);
    unsigned char second = static_cast<unsigned char>(text[pos + 1]);

    // Ё -> ё
    if (first == 0xD0 && second == 0x81) {
        append_russian_letter(word, 0xD1, 0x91);
        pos += 2;
        return true;
    }

    // А-П -> а-п
    if (first == 0xD0 && second >= 0x90 && second <= 0x9F) {
        append_russian_letter(word, 0xD0, second + 0x20);
        pos += 2;
        return true;
    }

    // Р-Я -> р-я
    if (first == 0xD0 && second >= 0xA0 && second <= 0xAF) {
        append_russian_letter(word, 0xD1, second - 0x20);
        pos += 2;
        return true;
    }

    // а-п
    if (first == 0xD0 && second >= 0xB0 && second <= 0xBF) {
        append_russian_letter(word, first, second);
        pos += 2;
        return true;
    }

    // р-я и ё
    if (first == 0xD1 &&
        ((second >= 0x80 && second <= 0x8F) || second == 0x91)) {
        append_russian_letter(word, first, second);
        pos += 2;
        return true;
    }

    return false;
}

} // namespace

namespace text_utils {

string normalize_word(const string &word) {
    string result;

    for (size_t pos = 0; pos < word.size();) {
        if (!append_letter(word, pos, result))
            ++pos;
    }

    return result;
}

size_t load_words_from_file(const string &path, void *context,
                            WordHandler handler, string &error) {
    ifstream file(path, ios::binary);
    if (!file) {
        error = "Не удалось открыть файл: " + path;
        return 0;
    }

    string text((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

    size_t loaded = 0;
    string word;

    for (size_t pos = 0; pos < text.size();) {
        if (append_letter(text, pos, word))
            continue;

        if (!word.empty()) {
            handler(context, word);
            ++loaded;
            word.clear();
        }

        ++pos;
    }

    if (!word.empty()) {
        handler(context, word);
        ++loaded;
    }

    error.clear();
    return loaded;
}

} // namespace text_utils
