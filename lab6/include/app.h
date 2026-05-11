#pragma once

#include "hash_dictionary.h"
#include "rb_tree_dictionary.h"

class app {
  public:
    static void run();

  private:
    static bool _running;
    static bool _back;
    static RedBlackTreeDictionary _tree_dictionary;
    static HashDictionary _hash_dictionary;

    static void handle_main_menu(int choice);
    static void work_with_tree_dictionary();
    static void work_with_hash_dictionary();
    static void handle_tree_menu(int choice);
    static void handle_hash_menu(int choice);
    static void load_file_to_both();
    static void clear_both();
    static void show_all_stats();
};
