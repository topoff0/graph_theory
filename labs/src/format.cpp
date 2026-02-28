#include "format.h"
#include "io.h"

int format::count_visible_chars(const string &s) {
    int count = 0;
    for (unsigned char c : s) {
        if ((c & 0xC0) != 0x80)
            count++;
    }
    return count;
}

vector<string> format::split_text(const string &text, int max_line_length,
                                  ALIGN align) {
    vector<string> lines;

    vector<string> paragraphs;
    {
        size_t start = 0;
        while (true) {
            size_t pos = text.find('\n', start);
            if (pos == string::npos) {
                paragraphs.push_back(text.substr(start));
                break;
            }
            paragraphs.push_back(text.substr(start, pos - start));
            start = pos + 1;
        }
    }

    for (size_t p = 0; p < paragraphs.size(); ++p) {
        const string &para = paragraphs[p];

        string current_line;
        size_t byte_pos = 0;
        size_t len = para.length();

        while (byte_pos < len) {
            while (byte_pos < len && para[byte_pos] == ' ')
                byte_pos++;

            if (byte_pos >= len)
                break;

            size_t word_end = para.find(' ', byte_pos);
            if (word_end == string::npos)
                word_end = len;

            string word = para.substr(byte_pos, word_end - byte_pos);

            int cur = count_visible_chars(current_line);
            int w = count_visible_chars(word);
            int space = current_line.empty() ? 0 : 1;

            if (cur + w + space > max_line_length) {
                if (!current_line.empty()) {
                    if (align == ALIGN_CENTER)
                        lines.push_back(
                            align_center(current_line, max_line_length));
                    else if (align == ALIGN_JUSTIFY)
                        lines.push_back(
                            align_justify(current_line, max_line_length));
                    else
                        lines.push_back(current_line);

                    current_line.clear();
                }
            }

            if (!current_line.empty())
                current_line += " ";
            current_line += word;

            byte_pos = word_end + 1;
        }

        {
            if (align == ALIGN_CENTER)
                lines.push_back(align_center(current_line, max_line_length));
            else
                lines.push_back(current_line);
        }

        if (p + 1 < paragraphs.size())
            lines.emplace_back("");
    }

    return lines;
}

string format::align_center(const string &line, int width) {
    int visible = count_visible_chars(line);
    if (visible >= width)
        return line;

    int total_pad = width - visible;
    int left = total_pad / 2;
    int right = total_pad - left;

    return string(left, ' ') + line + string(right, ' ');
}

string format::align_justify(const string &line, int width) {
    int visible = count_visible_chars(line);

    if (visible >= width)
        return line;

    if (line.find(' ') == string::npos)
        return line;

    vector<string> words;
    {
        size_t start = 0;
        while (true) {
            size_t pos = line.find(' ', start);
            if (pos == string::npos) {
                words.push_back(line.substr(start));
                break;
            }
            words.push_back(line.substr(start, pos - start));
            start = pos + 1;
        }
    }

    int total_chars = 0;
    for (auto &w : words)
        total_chars += format::count_visible_chars(w);

    int spaces_needed = width - total_chars;
    int gaps = words.size() - 1;

    if (spaces_needed < gaps)
        return line;

    int base_spaces = spaces_needed / gaps;
    int extra = spaces_needed % gaps;

    string result;
    for (int i = 0; i < words.size(); i++) {
        result += words[i];
        if (i < gaps) {
            result.append(base_spaces, ' ');
            if (extra > 0) {
                result.push_back(' ');
                --extra;
            }
        }
    }
    return result;
}
