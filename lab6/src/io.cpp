#include "io.h"
#include "config.h"
#include "format.h"

#include <iostream>
#include <stdexcept>

using namespace std;

const string io::reset_color = ANSI_RESET_COLOR;

namespace {

int count_visible_chars_with_ansi(const string &s) {
    int count = 0;
    bool in_escape = false;

    for (size_t i = 0; i < s.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(s[i]);

        if (in_escape) {
            if (c >= ANSI_SEQUENCE_FINAL_MIN && c <= ANSI_SEQUENCE_FINAL_MAX)
                in_escape = false;
            continue;
        }

        if (c == ANSI_ESCAPE_CHAR && i + 1 < s.size() &&
            s[i + 1] == ANSI_ESCAPE_START) {
            in_escape = true;
            ++i;
            continue;
        }

        if ((c & 0xC0) != 0x80)
            ++count;
    }

    return count;
}

} // namespace

string io::get_border(const int width, const string &s) {
    string border;
    for (int i = 0; i < width; ++i)
        border += s;
    return border;
}

const string io::get_color_code(COLOR clr) {
    switch (clr) {
    case COLOR::GREEN:
        return ANSI_GREEN_COLOR;
    case COLOR::YELLOW:
        return ANSI_YELLOW_COLOR;
    case COLOR::BLUE:
        return ANSI_BLUE_COLOR;
    case COLOR::RED:
        return ANSI_RED_COLOR;
    case COLOR::PURPLE:
        return ANSI_PURPLE_COLOR;
    case COLOR::CYAN:
        return ANSI_CYAN_COLOR;
    default:
        return ANSI_RESET_COLOR;
    }
}

void io::print_simple_header(const string &s, const string &main_color) {
    int len = format::count_visible_chars(s);
    int width = max(UI_HEADER_LENGTH, len + UI_HEADER_EXTRA_WIDTH);
    int padding_left = (width - len) / 2;
    int padding_right = width - len - padding_left;
    string border = get_border(width, "─");

    cout << main_color;
    cout << "◀" << border << "▶" << endl;
    cout << string(padding_left, ' ') << s << string(padding_right, ' ')
         << endl;
    cout << "◀" << border << "▶" << reset_color << endl;
}

void io::print_boxed_header(const string &s, const string &main_color) {
    int len = format::count_visible_chars(s);
    int width = max(UI_HEADER_LENGTH, len + UI_HEADER_EXTRA_WIDTH);
    int padding_left = (width - len) / 2;
    int padding_right = width - len - padding_left;
    string border = get_border(width, "─");

    cout << main_color;
    cout << "╭" << border << "╮" << endl;
    cout << "│" << string(padding_left, ' ') << s << string(padding_right, ' ')
         << "│" << endl;
    cout << "╰" << border << "╯" << reset_color << endl;
}

void io::print_bold_header(const string &s, const string &main_color) {
    int len = format::count_visible_chars(s);
    int width = max(UI_HEADER_LENGTH, len + UI_HEADER_EXTRA_WIDTH);
    int padding_left = (width - len) / 2;
    int padding_right = width - len - padding_left;
    string border = get_border(width, "═");

    cout << main_color;
    cout << "╔" << border << "╗" << endl;
    cout << "║" << string(padding_left, ' ') << s << string(padding_right, ' ')
         << "║" << endl;
    cout << "╚" << border << "╝" << reset_color << endl;
}

void io::v_space(int times) {
    for (int i = 0; i < times; i++)
        cout << '\n';
}

void io::print(const string &s, COLOR clr) {
    if (clr != BASE)
        cout << get_color_code(clr) << s << reset_color << endl;
    else
        cout << s << endl;
}

void io::print_header(const string &s, HEADER_STYLE style, COLOR clr) {
    string color = get_color_code(clr);
    switch (style) {
    case HEADER_STYLE::SIMPLE:
        print_simple_header(s, color);
        break;
    case HEADER_STYLE::BOXED:
        print_boxed_header(s, color);
        break;
    case HEADER_STYLE::BOLD:
        print_bold_header(s, color);
        break;
    }
}

void io::print_list(const vector<pair<string, COLOR>> &items,
                    const string &header) {
    int len = format::count_visible_chars(header);
    int width = UI_MENU_LENGTH > len ? UI_MENU_LENGTH : len + 2;

    for (const auto &item : items) {
        int visible_len =
            format::count_visible_chars(item.first) + UI_MENU_ITEM_EXTRA_WIDTH;
        if (visible_len > width)
            width = visible_len;
    }

    int padding_left = (width - len) / 2;
    int padding_right = width - len - padding_left;
    string outer_padding((UI_HEADER_LENGTH - width) / 2, ' ');
    string border = get_border(width, "─");

    cout << get_color_code(CYAN);
    cout << outer_padding << "╭" << border << "╮" << endl;
    cout << outer_padding << "│" << string(padding_left, ' ') << header
         << string(padding_right, ' ') << "│" << endl;
    cout << outer_padding << "├" << border << "┤" << endl;

    for (size_t i = 0; i < items.size(); ++i) {
        string menu_item = to_string(i + 1) +
                           (i + 1 >= 10 ? ". " : ".  ") + items[i].first;
        int visible_item_len = format::count_visible_chars(menu_item);
        int item_padding_right =
            width - visible_item_len - UI_LIST_LEFT_PADDING - 1;

        cout << outer_padding << "│" << get_color_code(items[i].second)
             << string(UI_LIST_LEFT_PADDING, ' ') << menu_item
             << string(item_padding_right, ' ') << get_color_code(CYAN) << " │"
             << endl;
    }
    cout << outer_padding << "╰" << border << "╯" << reset_color << endl;
}

void io::print_command_menu(const vector<menu_item> &items,
                            const string &header) {
    int width = UI_MENU_LENGTH;
    for (const auto &item : items) {
        int visible_len =
            format::count_visible_chars(item.title) + UI_MENU_ITEM_EXTRA_WIDTH;
        if (visible_len > width)
            width = visible_len;
    }

    vector<const menu_item *> top_items;
    vector<const menu_item *> bottom_items;

    for (const auto &item : items) {
        if (item.id > 0)
            top_items.push_back(&item);
        else
            bottom_items.push_back(&item);
    }

    int len = format::count_visible_chars(header);
    int padding_left = (width - len) / 2;
    int padding_right = width - len - padding_left;
    string outer_padding((UI_HEADER_LENGTH - width) / 2, ' ');
    string border = get_border(width, "─");

    cout << get_color_code(CYAN);
    cout << outer_padding << "╭" << border << "╮" << endl;
    cout << outer_padding << "│" << string(padding_left, ' ') << header
         << string(padding_right, ' ') << "│" << endl;
    cout << outer_padding << "├" << border << "┤" << endl;

    for (size_t i = 0; i < top_items.size(); ++i) {
        string menu_item = to_string(top_items[i]->id) +
                           (top_items[i]->id >= 10 ? ". " : ".  ") +
                           top_items[i]->title;
        int visible_item_len = format::count_visible_chars(menu_item);
        int item_padding_right =
            width - visible_item_len - UI_LIST_LEFT_PADDING - 1;

        cout << outer_padding << "│" << get_color_code(top_items[i]->clr)
             << string(UI_LIST_LEFT_PADDING, ' ') << menu_item
             << string(item_padding_right, ' ') << get_color_code(CYAN) << " │"
             << endl;
    }

    if (!bottom_items.empty()) {
        cout << outer_padding << "├" << border << "┤" << endl;
        for (const auto *item : bottom_items) {
            string menu_item = to_string(item->id) +
                               (item->id >= 10 ? ". " : ".  ") + item->title;
            int visible_item_len = format::count_visible_chars(menu_item);
            int item_padding_left =
                item->id >= 0 ? UI_LIST_LEFT_PADDING : UI_LIST_LEFT_PADDING - 1;
            int item_padding_right =
                width - visible_item_len - item_padding_left - 1;

            cout << outer_padding << "│" << get_color_code(item->clr)
                 << string(item_padding_left, ' ') << menu_item
                 << string(item_padding_right, ' ') << get_color_code(CYAN)
                 << " │" << endl;
        }
    }

    cout << outer_padding << "╰" << border << "╯" << reset_color << endl;
}

void io::print_retry(const string &s, COLOR clr) {
    int len = format::count_visible_chars(s);
    int width = max(UI_HEADER_LENGTH, len + UI_HEADER_EXTRA_WIDTH);
    int padding_left = (width - len) / 2;
    int padding_right = width - len - padding_left;

    string left_line = get_border(padding_left, ".") + ' ';
    string right_line = ' ' + get_border(padding_right, ".");
    cout << get_color_code(clr);
    cout << left_line << s << right_line << reset_color << endl;
    v_space(1);
}

void io::print_text_with_header(const string &text, const string &header,
                                const string &link, HEADER_STYLE style,
                                COLOR clr) {
    int width = UI_HEADER_LENGTH;
    auto cyan = get_color_code(CYAN);
    auto text_color = get_color_code(clr);

    vector<string> lines =
        format::split_text(text, width - UI_TEXT_HORIZONTAL_PADDING);

    string border = get_border(width, "─");

    cout << cyan;

    int header_len = format::count_visible_chars(header);
    int header_padding = (width - header_len) / 2;

    cout << "╭" << border << "╮" << endl;
    cout << "│" << string(header_padding, ' ') << text_color << header << cyan
         << string(width - header_len - header_padding, ' ') << "│" << endl;
    cout << "├" << border << "┤" << endl;

    for (const auto &line : lines) {
        int line_len = format::count_visible_chars(line);
        int total_padding = width - line_len;
        int left_padding = total_padding / 2;
        int right_padding = total_padding - left_padding;

        cout << "│" << string(left_padding, ' ') << text_color << line << cyan
             << string(right_padding, ' ') << "│" << endl;
    }
    if (!link.empty()) {
        cout << "├" << border << "┤" << endl;
        string total_link = "Подробнее: " + link;
        int total_link_len = format::count_visible_chars(total_link);
        cout << "│" << string(UI_LINK_LEFT_PADDING, ' ') << total_link
             << string(width - UI_LINK_LEFT_PADDING - total_link_len, ' ')
             << "│" << endl;
    }

    cout << "╰" << border << "╯" << endl;
    cout << reset_color;
}

void io::print_preformatted_with_header(const string &text,
                                        const string &header, COLOR clr) {
    vector<string> lines;
    size_t start = 0;
    while (true) {
        size_t pos = text.find('\n', start);
        if (pos == string::npos) {
            lines.push_back(text.substr(start));
            break;
        }
        lines.push_back(text.substr(start, pos - start));
        start = pos + 1;
    }

    int width = max(UI_HEADER_LENGTH,
                    format::count_visible_chars(header) + UI_HEADER_EXTRA_WIDTH);
    for (const auto &line : lines) {
        int line_width =
            count_visible_chars_with_ansi(line) + UI_TEXT_HORIZONTAL_PADDING;
        if (line_width > width)
            width = line_width;
    }

    string border = get_border(width, "─");
    int header_len = format::count_visible_chars(header);
    int header_left = (width - header_len) / 2;
    int header_right = width - header_len - header_left;

    cout << get_color_code(CYAN);
    cout << "╭" << border << "╮" << endl;
    cout << "│" << string(header_left, ' ') << get_color_code(clr) << header
         << get_color_code(CYAN) << string(header_right, ' ') << "│" << endl;
    cout << "├" << border << "┤" << endl;

    for (const auto &line : lines) {
        int line_len = count_visible_chars_with_ansi(line);
        int right_padding = width - line_len - 2;
        cout << "│ " << get_color_code(clr) << line << get_color_code(CYAN)
             << string(right_padding, ' ') << "│" << endl;
    }

    cout << "╰" << border << "╯" << endl;
    cout << reset_color;
}

void io::print_error(const string &message, COLOR clr) {
    int len = format::count_visible_chars(ERROR_WORD);
    int width = max(UI_HEADER_LENGTH, len + UI_HEADER_EXTRA_WIDTH) -
                UI_BORDER_WIDTH_ADJUSTMENT;
    int padding_left = (width - len) / 2;
    int padding_right = width - len - padding_left;

    string left_line = get_border(padding_left, "─") + "▶ ";
    string right_line = " ◀" + get_border(padding_right, "─");
    cout << get_color_code(RED);
    cout << left_line << ERROR_WORD << right_line << endl;

    int msg_len = format::count_visible_chars(message);
    int msg_width = max(UI_HEADER_LENGTH, msg_len + UI_MESSAGE_EXTRA_WIDTH);
    int msg_padding_left = (msg_width - msg_len) / 2;
    int msg_padding_right = msg_width - msg_len - msg_padding_left;

    string msg_left_line =
        get_border(msg_padding_left - UI_BORDER_WIDTH_ADJUSTMENT, "─");
    string msg_right_line =
        get_border(msg_padding_right - UI_BORDER_WIDTH_ADJUSTMENT, "─");

    cout << get_color_code(clr);
    string border = get_border(msg_width, "─");

    cout << "╭" << border << "╮" << endl;
    cout << "│ " << msg_left_line << " " << message << " " << msg_right_line
         << " │" << endl;
    cout << "╰" << border << "╯" << endl;

    cout << reset_color;
}

int io::read_number(const pair<int, int> &min_max_id, const string &header) {
    if (min_max_id.first > min_max_id.second)
        throw invalid_argument("Неверный диапазон значений для чтения");

    while (true) {
        int len = format::count_visible_chars(header);
        int width = max(UI_HEADER_LENGTH, len + UI_HEADER_EXTRA_WIDTH) -
                    UI_BORDER_WIDTH_ADJUSTMENT;
        int padding_left = (width - len) / 2;
        int padding_right = width - len - padding_left;

        string left_line = get_border(padding_left, "─") + "▶ ";
        string right_line = " ◀" + get_border(padding_right, "─");
        cout << left_line << header << right_line << endl;
        cout << left_line;

        string input;
        getline(cin, input);

        if (input.empty()) {
            print_error("Пустой ввод");
            print_retry("Попробуйте снова", GREEN);
            continue;
        }

        try {
            int value = stoi(input);
            if (value >= min_max_id.first && value <= min_max_id.second)
                return value;

            string error_message =
                "Число должно быть от " + to_string(min_max_id.first) +
                " до " + to_string(min_max_id.second);
            print_error(error_message);
        } catch (const invalid_argument &) {
            string error_message = "'" + input + "'" + " не является числом";
            print_error(error_message);
        } catch (const out_of_range &) {
            print_error("Число слишком большое");
        }

        print_retry("Попробуйте снова", GREEN);
    }
}

string io::read_string(const string &header) {
    while (true) {
        int len = format::count_visible_chars(header);
        int width = max(UI_HEADER_LENGTH, len + UI_HEADER_EXTRA_WIDTH) -
                    UI_BORDER_WIDTH_ADJUSTMENT;
        int padding_left = (width - len) / 2;
        int padding_right = width - len - padding_left;

        string left_line = get_border(padding_left, "─") + "▶ ";
        string right_line = " ◀" + get_border(padding_right, "─");
        cout << left_line << header << right_line << endl;
        cout << left_line;

        string input;
        getline(cin, input);

        if (input.empty()) {
            print_error("Пустой ввод");
            print_retry("Попробуйте снова", GREEN);
            continue;
        }

        return input;
    }
}

void io::wait_enter() {
    print("Нажмите Enter для продолжения...", CYAN);
    string line;
    getline(cin, line);
}
