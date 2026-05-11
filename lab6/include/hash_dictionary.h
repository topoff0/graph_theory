#pragma once

#include "config.h"

#include <cstddef>
#include <iosfwd>
#include <string>

using namespace std;

class HashDictionary {
  private:
    struct Entry {
        string word;
        int count;
        Entry *next;

        explicit Entry(const string &value);
    };

    Entry **_buckets;
    size_t _capacity;
    size_t _unique_words;
    size_t _total_words;

    static unsigned long long hash_value(const string &word);

    size_t bucket_index(const string &word) const;
    void init_buckets(size_t capacity);
    void destroy_buckets();
    void rehash(size_t new_capacity);

  public:
    HashDictionary();
    ~HashDictionary();

    HashDictionary(const HashDictionary &) = delete;
    HashDictionary &operator=(const HashDictionary &) = delete;

    bool insert(const string &word);
    bool erase(const string &word);
    bool contains(const string &word) const;
    int count(const string &word) const;
    void clear();
    size_t load_from_file(const string &path, string &error);
    void print(ostream &out, int limit) const;
    void print_table(ostream &out) const;

    size_t unique_words() const;
    size_t total_words() const;
    size_t capacity() const;
    size_t collision_buckets() const;
    size_t max_chain_length() const;
};
