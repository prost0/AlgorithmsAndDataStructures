#include <cstddef>
#include <map>
#include <vector>
#include <string>

class TNode
{
public:
    std::map<char, TNode *> edges;
    TNode *parent;
    TNode *suffix_link;
    std::size_t begin;
    std::size_t length;
    
    TNode();
    TNode(const std::string &, TNode *const &);
    TNode(const std::vector<char> &, const char &);
    TNode(const std::vector<char> &, const char &, TNode *const &);
    TNode(const std::string &, TNode *const &, const std::size_t &);
    
    std::size_t GetBegin() const;
    void SetBegin(const std::size_t &);
    std::size_t GetLength() const;
    void SetLength(const std::size_t &);
    TNode *GetParent() const;
    void SetParent(TNode *const &);
    TNode *GetSuffixLink() const;
    void SetSuffixLink(TNode *const &);
    
    std::map<char, TNode *> &Edges();
    
    ~TNode();
};

class TSuffixTree;
class TMatchingStatistics;

class TMatchingStatistics
{
    std::vector<std::size_t> statistic;
    std::size_t pattern_size;
    
public:
    TMatchingStatistics(const TSuffixTree &, const std::string &);
    
    std::vector<std::size_t> Find() const;
    
    ~TMatchingStatistics();
};

class TSuffixTree
{
    std::string text;
    TNode *root;
    TNode *active_vertex;
    std::size_t active_length;
    std::size_t active_char_idx;
    
    void DeepFirstSearch(TNode *const &, std::vector<std::size_t> &, const std::size_t &) const;
    friend TMatchingStatistics::TMatchingStatistics(const TSuffixTree &, const std::string &);
    
public:
    TSuffixTree(const std::vector<char> &, const char &);
    
    void PushBack(const char &ch);
    std::vector<std::size_t> Find(const std::string &pattern) const;
    
    ~TSuffixTree();
};