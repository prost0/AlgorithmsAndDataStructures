#include <cstddef>
#include <iostream>
#include <vector>

#include "suffix_tree.h"

int main()
{
    std::ios::sync_with_stdio(false);
    
    char spec_char = '$';
    std::vector<char> alphabet('z' - 'a' + 1u);
    for (int i = 'a'; i <= 'z'; ++i) {
        alphabet[i - 'a'] = i;
    }
    
    TSuffixTree tree(alphabet, spec_char);
    
    std::string text;
    std::cin >> text;
    
    for (std::size_t i = 0u, end = text.size(); i < end; ++i) {
        tree.PushBack(text[i]);
    }
    tree.PushBack('$');
    
    while (std::cin >> text) {
        TMatchingStatistics statistic(tree, text);
        std::vector<std::size_t> result = statistic.Find();
        for (std::vector<std::size_t>::const_iterator it = result.cbegin(); it != result.cend(); ++it) {
            std::cout << *it + 1u << "\n";
        }
    }
    
    return 0;
}