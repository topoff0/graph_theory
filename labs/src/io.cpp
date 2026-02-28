#include "io.h"
#include "config.h"
#include "helpers.h"

#include <iostream>

using std::cin;
using std::cout;
using std::endl;
using std::max;

const string io::reset_color = "\033[0m";

string io::get_line(const int width, const string &s)
{
    string border;
    for (int i = 0; i < width; ++i)
    {
        border += s;
    }
    return border;
}

const string io::get_color_code(COLOR clr)
{
    switch (clr)
    {
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

void io::print_simple_header(const string &s, const string &main_color)
{
    int len = helpers::count_visible_chars(s);
    int width = max(UI_HEADER_LENGTH, len + 12);
    int padding_left = (width - len) / 2;
    int padding_right = width - len - padding_left;

    string border = get_line(width, "─");

    cout << main_color;
    cout << "◀" << border << "▶" << endl;
    cout << string(padding_left, ' ') << s << string(padding_right, ' ') << endl;
    cout << "◀" << border << "▶";
    cout << reset_color << endl;
}

void io::print_boxed_header(const string &s, const string &main_color)
{
    int len = helpers::count_visible_chars(s);
    int width = max(UI_HEADER_LENGTH, len + 12);
    int padding_left = (width - len) / 2;
    int padding_right = width - len - padding_left;

    string border = get_line(width, "─");

    cout << main_color;
    cout << "╭" << border << "╮" << endl;
    cout << "│" << string(padding_left, ' ') << s << string(padding_right, ' ') << "│" << endl;
    cout << "╰" << border << "╯";
    cout << reset_color << endl;
}

void io::print_bold_header(const string &s, const string &main_color)
{
    int len = helpers::count_visible_chars(s);
    int width = max(UI_HEADER_LENGTH, len + 12);
    int padding_left = (width - len) / 2;
    int padding_right = width - len - padding_left;

    string border = get_line(width, "═");

    cout << main_color;
    cout << "╔" << border << "╗" << endl;
    cout << "║" << string(padding_left, ' ') << s << string(padding_right, ' ') << "║" << endl;
    cout << "╚" << border << "╝";
    cout << reset_color << endl;
}

// ================ PUBLIC ==================

void io::v_space(int times)
{
    for (int i = 0; i < times; i++)
    {
        cout << '\n';
    }
}

void io::print(const string &s, COLOR clr)
{
    if (clr != BASE)
        cout << get_color_code(clr) << s << reset_color << endl;
    else
        cout << s << endl;
}

void io::print_header(const string &s, HEADER_STYLE style, COLOR clr)
{
    string color = get_color_code(clr);
    switch (style)
    {
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

void io::print_list(const vector<pair<string, COLOR>> &items, const string &header)
{
    int len = helpers::count_visible_chars(header);
    int width = UI_MENU_LENGTH > len ? UI_MENU_LENGTH : len + 2;

    for (const auto &item : items)
    {
        int visible_len = helpers::count_visible_chars(item.first) + 10;
        if (visible_len > width)
        {
            width = visible_len;
        }
    }

    int padding_left = (width - len) / 2;
    int padding_right = width - len - padding_left;
    string outer_padding((UI_HEADER_LENGTH - width) / 2, ' ');
    string border = get_line(width, "─");

    cout << get_color_code(CYAN);
    cout << outer_padding << "╭" << border << "╮" << endl;
    cout << outer_padding << "│" << string(padding_left, ' ') << header << string(padding_right, ' ') << "│" << endl;
    cout << outer_padding << "├" << border << "┤" << endl;

    // Items
    for (size_t i = 0; i < items.size(); ++i)
    {
        string menu_item = std::to_string(i + 1) + (i + 1 >= 10 ? ". " : ".  ") + items[i].first;

        int visible_item_len = helpers::count_visible_chars(menu_item);
        int item_padding_left = UI_LIST_LEFT_PADDING;
        int item_padding_right = width - visible_item_len - UI_LIST_LEFT_PADDING - 1;

        cout << outer_padding << "│" << get_color_code(BLUE) << string(item_padding_left, ' ') << get_color_code(items[i].second)
             << menu_item << string(item_padding_right, ' ') << get_color_code(CYAN) << " │" << endl;
    }
    cout << outer_padding << "╰" << border << "╯" << endl;
    cout << reset_color;
}

void io::print_command_menu(const vector<pair<string, COLOR>> &items, const vector<pair<string, COLOR>> &exit_items, const string &header)
{

    int width = UI_MENU_LENGTH;
    for (const auto &item : items)
    {
        int visible_len = helpers::count_visible_chars(item.first) + 10;
        if (visible_len > width)
        {
            width = visible_len;
        }
    }

    int len = helpers::count_visible_chars(header);
    int padding_left = (width - len) / 2;
    int padding_right = width - len - padding_left;
    string outer_padding((UI_HEADER_LENGTH - width) / 2, ' ');
    string border = get_line(width, "─");

    cout << get_color_code(CYAN);
    cout << outer_padding << "╭" << border << "╮" << endl;
    cout << outer_padding << "│" << string(padding_left, ' ') << header << string(padding_right, ' ') << "│" << endl;
    cout << outer_padding << "├" << border << "┤" << endl;

    // Menu items
    for (size_t i = 0; i < items.size(); ++i)
    {
        string menu_item = std::to_string(i + 1) + (i + 1 >= 10 ? ". " : ".  ") + items[i].first;

        int visible_item_len = helpers::count_visible_chars(menu_item);
        int item_padding_left = UI_LIST_LEFT_PADDING;
        int item_padding_right = width - visible_item_len - UI_LIST_LEFT_PADDING - 1;

        cout << outer_padding << "│" << get_color_code(items[i].second) << string(item_padding_left, ' ') << get_color_code(items[i].second)
             << menu_item << string(item_padding_right, ' ') << get_color_code(CYAN) << " │" << endl;
    }
    // Exit items
    cout << outer_padding << "├" << border << "┤" << endl;
    int size = exit_items.size();
    for (int i = 0; i < size; i++)
    {
        string menu_item = (i - size + 1 < 0 ? "" : " ") + std::to_string(i - size + 1) + ". " + exit_items[i].first;
        int visible_item_len = helpers::count_visible_chars(menu_item);
        int item_padding_left = UI_LIST_LEFT_PADDING;
        int item_padding_right = width - visible_item_len - UI_LIST_LEFT_PADDING - 1;

        cout << outer_padding << "│" << string(item_padding_left, ' ') << get_color_code(exit_items[i].second)
             << menu_item << string(item_padding_right, ' ') << get_color_code(CYAN) << " │" << endl;
    }
    cout << outer_padding << "╰" << border << "╯" << endl;
    cout << reset_color;
}

void io::print_retry(const string &s, COLOR clr)
{
    int len = helpers::count_visible_chars(s);
    int width = max(UI_HEADER_LENGTH, len + 12);
    int padding_left = (width - len) / 2;
    int padding_right = width - len - padding_left;

    string left_line = get_line(padding_left, ".") + ' ';
    string right_line = ' ' + get_line(padding_right, ".");
    cout << get_color_code(clr);
    cout << left_line << s << right_line << reset_color << endl;
    v_space(1);
}

void io::print_text_with_header(const string &text, const string &header, const string &link, HEADER_STYLE style, COLOR clr)
{
    int width = UI_HEADER_LENGTH;

    vector<string> lines = helpers::split_text(text, width - 4);

    string border = get_line(width, "─");

    cout << get_color_code(clr);

    int header_len = helpers::count_visible_chars(header);
    int header_padding = (width - header_len) / 2;

    cout << "╭" << border << "╮" << endl;
    cout << "│" << string(header_padding, ' ') << header
         << string(width - header_len - header_padding, ' ') << "│" << endl;
    cout << "├" << border << "┤" << endl;

    for (const auto &line : lines)
    {
        int line_len = helpers::count_visible_chars(line);
        int total_padding = width - line_len;
        int left_padding = total_padding / 2;
        int right_padding = total_padding - left_padding;

        cout << "│" << string(left_padding, ' ') << line
             << string(right_padding, ' ') << "│" << endl;
    }
    if (!link.empty())
    {
        cout << "├" << border << "┤" << endl;
        string total_link = "Подробнее: " + link;
        int total_link_len = helpers::count_visible_chars(total_link);
        cout << "│" << string(4, ' ') << total_link << string(width - 4 - total_link_len, ' ') << "│" << endl;
    }

    cout
        << "╰" << border << "╯" << endl;
    cout << reset_color;
}

void io::print_error(const string &message, COLOR clr)
{
    // Header
    int len = helpers::count_visible_chars(ERROR_WORD);
    int width = max(UI_HEADER_LENGTH, len + 12) - 2;
    int padding_left = (width - len) / 2;
    int padding_right = width - len - padding_left;

    string left_line = get_line(padding_left, "─") + "▶ ";
    string right_line = " ◀" + get_line(padding_right, "─");
    cout << get_color_code(RED);
    cout << left_line << ERROR_WORD << right_line << endl;

    // Message
    int msg_len = helpers::count_visible_chars(message);
    int msg_width = max(UI_HEADER_LENGTH, msg_len + 8);
    int msg_padding_left = (msg_width - msg_len) / 2;
    int msg_padding_right = msg_width - msg_len - msg_padding_left;

    string msg_left_line = get_line(msg_padding_left - 2, "─");
    string msg_right_line = get_line(msg_padding_right - 2, "─");

    cout << get_color_code(clr);
    string border = get_line(msg_width, "─");

    cout << "╭" << border << "╮" << endl;
    cout << "│ " << msg_left_line << " " << message << " " << msg_right_line << " │" << endl;
    cout << "╰" << border << "╯" << endl;

    cout << reset_color;
}

int io::read_number(int min_value, int max_value, const string &header)
{
    if (min_value > max_value)
        throw std::invalid_argument("Неверный диапазон значений для чтения");

    while (true)
    {
        int len = helpers::count_visible_chars(header);
        int width = max(UI_HEADER_LENGTH, len + 12) - 2;
        int padding_left = (width - len) / 2;
        int padding_right = width - len - padding_left;

        string left_line = get_line(padding_left, "─") + "▶" + ' ';
        string right_line = " ◀" + get_line(padding_right, "─");
        cout << left_line << header << right_line << endl;
        cout << left_line;

        string input;
        getline(cin, input);

        if (input.empty())
        {
            print_error("Пустой ввод");
            continue;
        }

        try
        {
            int value = std::stoi(input);

            if (value >= min_value && value <= max_value)
            {
                return value;
            }
            else
            {
                string error_message =
                    "Число должно быть от " + std::to_string(min_value) + " до " + std::to_string(max_value);
                print_error(error_message);
            }
        }
        catch (const std::invalid_argument &)
        {
            string error_message = "'" + input + "'" + " не является числом";
            print_error(error_message);
        }
        catch (const std::out_of_range &)
        {
            string error_message("Число слишком большое");
            print_error(error_message);
        }

        print_retry("Попробуйте снова", GREEN);
    }
}

void io::wait_enter()
{
    io::print("Нажмите Enter для продолжения...", CYAN);

    char c = getchar();
    while (c != '\n')
    {
        c = getchar();
    }
}
