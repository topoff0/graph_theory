#include "io.h"
#include "config.h"
#include "format.h"

#include <iostream>

using std::cin;
using std::cout;
using std::endl;
using std::max;

const string io::reset_color = "\033[0m";

string io::get_border(const int width, const string &s) {
    string border;
    for (int i = 0; i < width; ++i) {
        border += s;
    }
    return border;
}

const string io::get_color_code(COLOR clr) {
    switch (clr) {
    case COLOR::GREEN:
        return "\033[32m";
    case COLOR::YELLOW:
        return "\033[33m";
    case COLOR::BLUE:
        return "\033[34m";
    case COLOR::RED:
        return "\033[31m";
    case COLOR::PURPLE:
        return "\033[35m";
    case COLOR::CYAN:
        return "\033[36m";

    default:
        return "\033[0m";
        break;
    }
}

void io::print_simple_header(const string &s, const string &main_color) {
    int len = format::count_visible_chars(s);
    int width = max(UI_HEADER_LENGTH, len + 12);
    int padding_left = (width - len) / 2;
    int padding_right = width - len - padding_left;

    string border = get_border(width, "─");

    cout << main_color;
    cout << "◀" << border << "▶" << endl;
    cout << string(padding_left, ' ') << s << string(padding_right, ' ')
         << endl;
    cout << "◀" << border << "▶";
    cout << reset_color << endl;
}

void io::print_boxed_header(const string &s, const string &main_color) {
    int len = format::count_visible_chars(s);
    int width = max(UI_HEADER_LENGTH, len + 12);
    int padding_left = (width - len) / 2;
    int padding_right = width - len - padding_left;

    string border = get_border(width, "─");

    cout << main_color;
    cout << "╭" << border << "╮" << endl;
    cout << "│" << string(padding_left, ' ') << s << string(padding_right, ' ')
         << "│" << endl;
    cout << "╰" << border << "╯";
    cout << reset_color << endl;
}

void io::print_bold_header(const string &s, const string &main_color) {
    int len = format::count_visible_chars(s);
    int width = max(UI_HEADER_LENGTH, len + 12);
    int padding_left = (width - len) / 2;
    int padding_right = width - len - padding_left;

    string border = get_border(width, "═");

    cout << main_color;
    cout << "╔" << border << "╗" << endl;
    cout << "║" << string(padding_left, ' ') << s << string(padding_right, ' ')
         << "║" << endl;
    cout << "╚" << border << "╝";
    cout << reset_color << endl;
}

// ================ PUBLIC ==================

void io::v_space(int times) {
    for (int i = 0; i < times; i++) {
        cout << '\n';
    }
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

    default:
        print_simple_header(s, color);
        break;
    }
}

void io::print_list(const vector<pair<string, COLOR>> &items,
                    const string &header) {
    int len = format::count_visible_chars(header);
    int width = UI_MENU_LENGTH > len ? UI_MENU_LENGTH : len + 2;

    for (const auto &item : items) {
        int visible_len = format::count_visible_chars(item.first) + 10;
        if (visible_len > width) {
            width = visible_len;
        }
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

    // Items
    for (size_t i = 0; i < items.size(); ++i) {
        string menu_item = std::to_string(i + 1) +
                           (i + 1 >= 10 ? ". " : ".  ") + items[i].first;

        int visible_item_len = format::count_visible_chars(menu_item);
        int item_padding_left = UI_LIST_LEFT_PADDING;
        int item_padding_right =
            width - visible_item_len - UI_LIST_LEFT_PADDING - 1;

        cout << outer_padding << "│" << get_color_code(BLUE)
             << string(item_padding_left, ' ')
             << get_color_code(items[i].second) << menu_item
             << string(item_padding_right, ' ') << get_color_code(CYAN) << " │"
             << endl;
    }
    cout << outer_padding << "╰" << border << "╯" << endl;
    cout << reset_color;
}

void io::print_command_menu(const vector<menu_item> &items,
                            const string &header) {
    int width = UI_MENU_LENGTH;
    for (const auto &item : items) {
        int visible_len = format::count_visible_chars(item.title) + 10;
        if (visible_len > width) {
            width = visible_len;
        }
    }

    vector<const menu_item *> top_items;
    vector<const menu_item *> bottom_items;
    top_items.reserve(items.size());
    bottom_items.reserve(items.size());

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

    // Menu items
    for (size_t i = 0; i < top_items.size(); ++i) {
        string menu_item = std::to_string(top_items[i]->id) +
                           (i + 1 >= 10 ? ". " : ".  ") + top_items[i]->title;

        int visible_item_len = format::count_visible_chars(menu_item);
        int item_padding_left = UI_LIST_LEFT_PADDING;
        int item_padding_right =
            width - visible_item_len - UI_LIST_LEFT_PADDING - 1;

        cout << outer_padding << "│" << get_color_code(top_items[i]->clr)
             << string(item_padding_left, ' ') << menu_item
             << string(item_padding_right, ' ') << get_color_code(CYAN) << " │"
             << endl;
    }
    // Exit items
    if (bottom_items.size() > 0) {
        cout << outer_padding << "├" << border << "┤" << endl;
        for (size_t i = 0; i < bottom_items.size(); ++i) {
            int id = bottom_items[i]->id;
            string menu_item = std::to_string(id) +
                               (i + 1 >= 10 ? ". " : ".  ") +
                               bottom_items[i]->title;

            int visible_item_len = format::count_visible_chars(menu_item);
            int item_padding_left =
                id >= 0 ? UI_LIST_LEFT_PADDING : UI_LIST_LEFT_PADDING - 1;
            int item_padding_right =
                width - visible_item_len - item_padding_left - 1;

            cout << outer_padding << "│" << get_color_code(bottom_items[i]->clr)
                 << string(item_padding_left, ' ') << menu_item
                 << string(item_padding_right, ' ') << get_color_code(CYAN)
                 << " │" << endl;
        }
    }

    cout << outer_padding << "╰" << border << "╯" << endl;
    cout << reset_color;
}

void io::print_retry(const string &s, COLOR clr) {
    int len = format::count_visible_chars(s);
    int width = max(UI_HEADER_LENGTH, len + 12);
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

    vector<string> lines = format::split_text(text, width - 4);

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

        cout << "│" << string(left_padding, ' ') << line
             << string(right_padding, ' ') << "│" << endl;
    }
    if (!link.empty()) {
        cout << "├" << border << "┤" << endl;
        string total_link = "Подробнее: " + link;
        int total_link_len = format::count_visible_chars(total_link);
        cout << "│" << string(4, ' ') << total_link
             << string(width - 4 - total_link_len, ' ') << "│" << endl;
    }

    cout << "╰" << border << "╯" << endl;
    cout << reset_color;
}

void io::print_matrix(const matrix &mat, const string &header, COLOR clr) {
    size_t rows = mat.get_rows();
    size_t cols = mat.get_cols();
    if (rows == 0 || cols == 0)
        return;

    const string padding = "  ";

    size_t cell_width = 1;
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            double val = mat.at(i, j);
            string s;
            if (val == static_cast<int>(val))
                s = std::to_string(static_cast<int>(val));
            else {
                char buffer[32];
                snprintf(buffer, sizeof(buffer), "%.1f", val);
                s = buffer;
            }
            if (s.length() > cell_width)
                cell_width = s.length();
        }
    }

    size_t table_width = cols * (cell_width + 3) + 1;
    size_t margin_size = 0;

    if (UI_HEADER_LENGTH > table_width)
        margin_size = (UI_HEADER_LENGTH - table_width) / 2;

    string margin(margin_size, ' ');

    io::print_header(header, BOXED, clr);

    cout << margin << get_color_code(clr) << "╭";
    for (size_t j = 0; j < cols; ++j) {
        cout << get_border(cell_width + 2, "─");
        cout << (j == cols - 1 ? "╮" : "┬");
    }
    cout << "\n";

    for (size_t i = 0; i < rows; ++i) {
        cout << margin << "│";
        for (size_t j = 0; j < cols; ++j) {
            double val = mat.at(i, j);
            string s;
            if (val == static_cast<int>(val))
                s = std::to_string(static_cast<int>(val));
            else {
                char buffer[32];
                snprintf(buffer, sizeof(buffer), "%.1f", val);
                s = buffer;
            }

            size_t padding = cell_width - s.length();
            size_t pad_left = padding / 2;
            size_t pad_right = padding - pad_left;

            cout << " " << string(pad_left, ' ') << get_color_code(BASE) << s
                 << get_color_code(clr) << string(pad_right, ' ') << " │";
        }
        cout << "\n";

        if (i != rows - 1) {
            cout << margin << "├";
            for (size_t j = 0; j < cols; ++j) {
                cout << get_border(cell_width + 2, "─");
                cout << (j == cols - 1 ? "┤" : "┼");
            }
            cout << "\n";
        }
    }

    cout << margin << "╰";
    for (size_t j = 0; j < cols; ++j) {
        cout << get_border(cell_width + 2, "─");
        cout << (j == cols - 1 ? "╯" : "┴");
    }
    cout << "\n" << reset_color;
}

void io::print_error(const string &message, COLOR clr) {
    // Header
    int len = format::count_visible_chars(ERROR_WORD);
    int width = max(UI_HEADER_LENGTH, len + 12) - 2;
    int padding_left = (width - len) / 2;
    int padding_right = width - len - padding_left;

    string left_line = get_border(padding_left, "─") + "▶ ";
    string right_line = " ◀" + get_border(padding_right, "─");
    cout << get_color_code(RED);
    cout << left_line << ERROR_WORD << right_line << endl;

    // Message
    int msg_len = format::count_visible_chars(message);
    int msg_width = max(UI_HEADER_LENGTH, msg_len + 8);
    int msg_padding_left = (msg_width - msg_len) / 2;
    int msg_padding_right = msg_width - msg_len - msg_padding_left;

    string msg_left_line = get_border(msg_padding_left - 2, "─");
    string msg_right_line = get_border(msg_padding_right - 2, "─");

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
        throw std::invalid_argument("Неверный диапазон значений для чтения");

    while (true) {
        int len = format::count_visible_chars(header);
        int width = max(UI_HEADER_LENGTH, len + 12) - 2;
        int padding_left = (width - len) / 2;
        int padding_right = width - len - padding_left;

        string left_line = get_border(padding_left, "─") + "▶" + ' ';
        string right_line = " ◀" + get_border(padding_right, "─");
        cout << left_line << header << right_line << endl;
        cout << left_line;

        string input;
        getline(cin, input);

        if (input.empty()) {
            print_error("Пустой ввод");
            continue;
        }

        try {
            int value = std::stoi(input);

            if (value >= min_max_id.first && value <= min_max_id.second) {
                return value;
            } else {
                string error_message =
                    "Число должно быть от " + std::to_string(min_max_id.first) +
                    " до " + std::to_string(min_max_id.second);
                print_error(error_message);
            }
        } catch (const std::invalid_argument &) {
            string error_message = "'" + input + "'" + " не является числом";
            print_error(error_message);
        } catch (const std::out_of_range &) {
            string error_message("Число слишком большое");
            print_error(error_message);
        }

        print_retry("Попробуйте снова", GREEN);
    }
}

string io::read_string(const string &header) {
    while (true) {
        int len = format::count_visible_chars(header);
        int width = max(UI_HEADER_LENGTH, len + 12) - 2;
        int padding_left = (width - len) / 2;
        int padding_right = width - len - padding_left;

        string left_line = get_border(padding_left, "─") + "▶" + ' ';
        string right_line = " ◀" + get_border(padding_right, "─");
        cout << left_line << header << right_line << endl;
        cout << left_line;

        string input;
        getline(cin, input);

        if (input.empty()) {
            print_error("Пустой ввод");
            continue;
        }

        return input;

        print_retry("Попробуйте снова", GREEN);
    }
}

void io::wait_enter() {
    io::print("Нажмите Enter для продолжения...", CYAN);

    char c = getchar();
    while (c != '\n') {
        c = getchar();
    }
}
