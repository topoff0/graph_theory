#include "rb_tree_dictionary.h"
#include "config.h"
#include "text_utils.h"

#include <algorithm>
#include <iostream>

using namespace std;

RedBlackTreeDictionary::Node::Node(const string &value)
    : word(value), count(1), color(RED), left(nullptr), right(nullptr),
      parent(nullptr) {}

RedBlackTreeDictionary::RedBlackTreeDictionary()
    : _root(nullptr), _unique_words(0), _total_words(0) {}

RedBlackTreeDictionary::~RedBlackTreeDictionary() { clear(); }

RedBlackTreeDictionary::Color RedBlackTreeDictionary::color_of(Node *node) {
    return node == nullptr ? BLACK : node->color;
}

void RedBlackTreeDictionary::set_color(Node *node, Color color) {
    if (node != nullptr)
        node->color = color;
}

RedBlackTreeDictionary::Node *RedBlackTreeDictionary::minimum(Node *node) {
    while (node != nullptr && node->left != nullptr)
        node = node->left;
    return node;
}

int RedBlackTreeDictionary::height(Node *node) {
    if (node == nullptr)
        return 0;
    return 1 + max(height(node->left), height(node->right));
}

RedBlackTreeDictionary::Node *
RedBlackTreeDictionary::find_node(const string &word) const {
    Node *current = _root;
    while (current != nullptr) {
        if (word == current->word)
            return current;
        current = word < current->word ? current->left : current->right;
    }
    return nullptr;
}

void RedBlackTreeDictionary::rotate_left(Node *node) {
    Node *right = node->right;
    node->right = right->left;
    if (right->left != nullptr)
        right->left->parent = node;

    right->parent = node->parent;
    if (node->parent == nullptr)
        _root = right;
    else if (node == node->parent->left)
        node->parent->left = right;
    else
        node->parent->right = right;

    right->left = node;
    node->parent = right;
}

void RedBlackTreeDictionary::rotate_right(Node *node) {
    Node *left = node->left;
    node->left = left->right;
    if (left->right != nullptr)
        left->right->parent = node;

    left->parent = node->parent;
    if (node->parent == nullptr)
        _root = left;
    else if (node == node->parent->right)
        node->parent->right = left;
    else
        node->parent->left = left;

    left->right = node;
    node->parent = left;
}

void RedBlackTreeDictionary::fix_insert(Node *node) {
    while (node->parent != nullptr && node->parent->color == RED) {
        Node *parent = node->parent;
        Node *grandparent = parent->parent;

        if (parent == grandparent->left) {
            Node *uncle = grandparent->right;
            if (color_of(uncle) == RED) {
                parent->color = BLACK;
                uncle->color = BLACK;
                grandparent->color = RED;
                node = grandparent;
            } else {
                if (node == parent->right) {
                    node = parent;
                    rotate_left(node);
                    parent = node->parent;
                    grandparent = parent->parent;
                }
                parent->color = BLACK;
                grandparent->color = RED;
                rotate_right(grandparent);
            }
        } else {
            Node *uncle = grandparent->left;
            if (color_of(uncle) == RED) {
                parent->color = BLACK;
                uncle->color = BLACK;
                grandparent->color = RED;
                node = grandparent;
            } else {
                if (node == parent->left) {
                    node = parent;
                    rotate_right(node);
                    parent = node->parent;
                    grandparent = parent->parent;
                }
                parent->color = BLACK;
                grandparent->color = RED;
                rotate_left(grandparent);
            }
        }
    }
    _root->color = BLACK;
}

void RedBlackTreeDictionary::transplant(Node *old_node, Node *new_node) {
    if (old_node->parent == nullptr)
        _root = new_node;
    else if (old_node == old_node->parent->left)
        old_node->parent->left = new_node;
    else
        old_node->parent->right = new_node;

    if (new_node != nullptr)
        new_node->parent = old_node->parent;
}

void RedBlackTreeDictionary::fix_delete(Node *node, Node *parent) {
    while (node != _root && color_of(node) == BLACK) {
        if (parent == nullptr)
            break;

        if (node == parent->left) {
            Node *brother = parent->right;
            if (color_of(brother) == RED) {
                brother->color = BLACK;
                parent->color = RED;
                rotate_left(parent);
                brother = parent->right;
            }

            if (color_of(brother == nullptr ? nullptr : brother->left) ==
                    BLACK &&
                color_of(brother == nullptr ? nullptr : brother->right) ==
                    BLACK) {
                set_color(brother, RED);
                node = parent;
                parent = node->parent;
            } else {
                if (color_of(brother == nullptr ? nullptr : brother->right) ==
                    BLACK) {
                    set_color(brother == nullptr ? nullptr : brother->left,
                              BLACK);
                    set_color(brother, RED);
                    if (brother != nullptr)
                        rotate_right(brother);
                    brother = parent->right;
                }
                set_color(brother, color_of(parent));
                parent->color = BLACK;
                set_color(brother == nullptr ? nullptr : brother->right, BLACK);
                rotate_left(parent);
                node = _root;
                parent = nullptr;
            }
        } else {
            Node *brother = parent->left;
            if (color_of(brother) == RED) {
                brother->color = BLACK;
                parent->color = RED;
                rotate_right(parent);
                brother = parent->left;
            }

            if (color_of(brother == nullptr ? nullptr : brother->right) ==
                    BLACK &&
                color_of(brother == nullptr ? nullptr : brother->left) ==
                    BLACK) {
                set_color(brother, RED);
                node = parent;
                parent = node->parent;
            } else {
                if (color_of(brother == nullptr ? nullptr : brother->left) ==
                    BLACK) {
                    set_color(brother == nullptr ? nullptr : brother->right,
                              BLACK);
                    set_color(brother, RED);
                    if (brother != nullptr)
                        rotate_left(brother);
                    brother = parent->left;
                }
                set_color(brother, color_of(parent));
                parent->color = BLACK;
                set_color(brother == nullptr ? nullptr : brother->left, BLACK);
                rotate_right(parent);
                node = _root;
                parent = nullptr;
            }
        }
    }
    set_color(node, BLACK);
}

void RedBlackTreeDictionary::clear(Node *node) {
    if (node == nullptr)
        return;
    clear(node->left);
    clear(node->right);
    delete node;
}

void RedBlackTreeDictionary::print_in_order(Node *node, ostream &out,
                                            int &printed, int limit) const {
    if (node == nullptr || printed >= limit)
        return;
    print_in_order(node->left, out, printed, limit);
    if (printed < limit) {
        out << node->word << " (" << node->count << ")\n";
        ++printed;
    }
    print_in_order(node->right, out, printed, limit);
}

void RedBlackTreeDictionary::print_tree(Node *node, ostream &out,
                                        const string &prefix, bool is_left,
                                        bool is_root) const {
    if (node == nullptr)
        return;

    if (node->right != nullptr) {
        string next_prefix =
            is_root ? "" : prefix + (is_left ? "│   " : "    ");
        print_tree(node->right, out, next_prefix, false, false);
    }

    out << prefix;
    if (is_root)
        out << "Корень: ";
    else
        out << (is_left ? "└── " : "┌── ");

    const char *node_color =
        node->color == RED ? TREE_RED_COLOR : TREE_BLACK_COLOR;
    out << node_color << node->word << " [" << node->count << ", "
        << (node->color == RED ? "R" : "B") << "]" << TREE_RESET_COLOR << "\n";

    if (node->left != nullptr) {
        string next_prefix =
            is_root ? "" : prefix + (is_left ? "    " : "│   ");
        print_tree(node->left, out, next_prefix, true, false);
    }
}

bool RedBlackTreeDictionary::insert(const string &raw_word) {
    string word = text_utils::normalize_word(raw_word);
    if (word.empty())
        return false;

    Node *parent = nullptr;
    Node *current = _root;
    while (current != nullptr) {
        parent = current;
        if (word == current->word) {
            ++current->count;
            ++_total_words;
            return false;
        }
        current = word < current->word ? current->left : current->right;
    }

    Node *node = new Node(word);
    node->parent = parent;

    if (parent == nullptr)
        _root = node;
    else if (word < parent->word)
        parent->left = node;
    else
        parent->right = node;

    ++_unique_words;
    ++_total_words;
    fix_insert(node);
    return true;
}

bool RedBlackTreeDictionary::erase(const string &raw_word) {
    string word = text_utils::normalize_word(raw_word);
    Node *target = find_node(word);
    if (target == nullptr)
        return false;

    Node *moved = target;
    Node *fix_node = nullptr;
    Node *fix_parent = nullptr;
    Color moved_original_color = moved->color;

    if (target->left == nullptr) {
        fix_node = target->right;
        fix_parent = target->parent;
        transplant(target, target->right);
    } else if (target->right == nullptr) {
        fix_node = target->left;
        fix_parent = target->parent;
        transplant(target, target->left);
    } else {
        moved = minimum(target->right);
        moved_original_color = moved->color;
        fix_node = moved->right;

        if (moved->parent == target) {
            fix_parent = moved;
            if (fix_node != nullptr)
                fix_node->parent = moved;
        } else {
            fix_parent = moved->parent;
            transplant(moved, moved->right);
            moved->right = target->right;
            moved->right->parent = moved;
        }

        transplant(target, moved);
        moved->left = target->left;
        moved->left->parent = moved;
        moved->color = target->color;
    }

    _total_words -= target->count;
    --_unique_words;
    delete target;

    if (moved_original_color == BLACK)
        fix_delete(fix_node, fix_parent);
    return true;
}

bool RedBlackTreeDictionary::contains(const string &word) const {
    return find_node(text_utils::normalize_word(word)) != nullptr;
}

int RedBlackTreeDictionary::count(const string &word) const {
    Node *node = find_node(text_utils::normalize_word(word));
    return node == nullptr ? 0 : node->count;
}

void RedBlackTreeDictionary::clear() {
    clear(_root);
    _root = nullptr;
    _unique_words = 0;
    _total_words = 0;
}

namespace {
void tree_insert_callback(void *context, const string &word) {
    static_cast<RedBlackTreeDictionary *>(context)->insert(word);
}
} // namespace

size_t RedBlackTreeDictionary::load_from_file(const string &path,
                                              string &error) {
    return text_utils::load_words_from_file(path, this, tree_insert_callback,
                                            error);
}

void RedBlackTreeDictionary::print(ostream &out, int limit) const {
    if (_root == nullptr) {
        out << "Словарь пуст\n";
        return;
    }
    int printed = 0;
    print_in_order(_root, out, printed, limit);
}

void RedBlackTreeDictionary::print_tree(ostream &out) const {
    if (_root == nullptr) {
        out << "Дерево пусто\n";
        return;
    }

    print_tree(_root, out, "", false, true);
}

size_t RedBlackTreeDictionary::unique_words() const { return _unique_words; }

size_t RedBlackTreeDictionary::total_words() const { return _total_words; }

int RedBlackTreeDictionary::height() const { return height(_root); }
