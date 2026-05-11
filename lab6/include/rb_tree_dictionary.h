#pragma once

#include <cstddef>
#include <iosfwd>
#include <string>

using namespace std;

class RedBlackTreeDictionary {
  private:
    enum Color { RED, BLACK };

    struct Node {
        string word;
        int count;
        Color color;
        Node *left;
        Node *right;
        Node *parent;

        explicit Node(const string &value);
    };

    Node *_root;
    size_t _unique_words;
    size_t _total_words;

    static Color color_of(Node *node);
    static void set_color(Node *node, Color color);
    static Node *minimum(Node *node);
    static int height(Node *node);

    Node *find_node(const string &word) const;
    void rotate_left(Node *node);
    void rotate_right(Node *node);
    void fix_insert(Node *node);
    void transplant(Node *old_node, Node *new_node);
    void fix_delete(Node *node, Node *parent);
    void clear(Node *node);
    void print_in_order(Node *node, ostream &out, int &printed,
                        int limit) const;
    void print_tree(Node *node, ostream &out, const string &prefix,
                    bool is_left, bool is_root) const;

  public:
    RedBlackTreeDictionary();
    ~RedBlackTreeDictionary();

    RedBlackTreeDictionary(const RedBlackTreeDictionary &) = delete;
    RedBlackTreeDictionary &operator=(const RedBlackTreeDictionary &) = delete;

    bool insert(const string &word);
    bool erase(const string &word);
    bool contains(const string &word) const;
    int count(const string &word) const;
    void clear();
    size_t load_from_file(const string &path, string &error);
    void print(ostream &out, int limit) const;
    void print_tree(ostream &out) const;

    size_t unique_words() const;
    size_t total_words() const;
    int height() const;
};
