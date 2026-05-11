#pragma once

#include <cstddef>
#include <string>

using namespace std;

namespace text_utils {

using WordHandler = void (*)(void *context, const string &word);

string normalize_word(const string &word);
size_t load_words_from_file(const string &path, void *context,
                            WordHandler handler, string &error);

} // namespace text_utils
