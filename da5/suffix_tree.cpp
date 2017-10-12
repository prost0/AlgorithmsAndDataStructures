#include <algorithm>
#include <limits>
#include <utility>
#include <iostream>

#include "suffix_tree.h"

TNode::TNode(const std::string &text, TNode *const &node) :
    edges(), parent(node), suffix_link(nullptr), begin(text.size() - 1u), length(std::numeric_limits<std::size_t>::max())
{
    node->Edges().insert(std::pair<char, TNode *>(text.back(), this));
}

TNode::TNode(const std::vector<char> &alpha, const char &spec) :
    edges(), parent(nullptr), suffix_link(nullptr), begin(0u), length(0u)
{
    suffix_link = new TNode(alpha, spec, this);
}

TNode::TNode(const std::vector<char> &alpha, const char &spec, TNode *const &root) :
    edges(), parent(nullptr), suffix_link(nullptr), begin(0u), length(0u)
{
    for (std::vector<char>::const_iterator it = alpha.begin(); it != alpha.end(); ++it) {
        edges.insert(std::pair<char, TNode *>(*it, root));
    }
    edges.insert(std::pair<char, TNode *>(spec, root));
}

TNode::TNode(const std::string &text, TNode *const &node, const std::size_t &pos) :
    edges(
{
    std::pair<char, TNode *>(text[node->GetBegin() + pos], node)
}), parent(node->GetParent()), suffix_link(nullptr), begin(node->GetBegin()), length(pos)
{
    new TNode(text, this);
    node->GetParent()->Edges().at(text[node->GetBegin()]) = this;
    node->SetParent(this);
    node->SetBegin(node->GetBegin() + pos);
    if (node->GetLength() != std::numeric_limits<std::size_t>::max()) {
        node->SetLength(node->GetLength() - pos);
    }
}

std::size_t TNode::GetBegin() const
{
    return begin;
}

void TNode::SetBegin(const std::size_t &pos)
{
    begin = pos;
}

std::size_t TNode::GetLength() const
{
    return length;
}

void TNode::SetLength(const std::size_t &len)
{
    length = len;
}

TNode *TNode::GetParent() const
{
    return parent;
}

void TNode::SetParent(TNode *const &p)
{
    parent = p;
}

TNode *TNode::GetSuffixLink() const
{
    return suffix_link;
}

void TNode::SetSuffixLink(TNode *const &link)
{
    suffix_link = link;
}

std::map<char, TNode *> &TNode::Edges()
{
    return edges;
}

TNode::~TNode()
{
    for (std::map<char, TNode *>::iterator it = edges.begin(); it != edges.end(); ++it) {
        delete it->second;
    }
}

TSuffixTree::TSuffixTree(const std::vector<char> &alpha, const char &spec) :
    text(), active_length(0u), active_char_idx(0u)
{
    active_vertex = root = new TNode(alpha, spec);
}

void TSuffixTree::PushBack(const char &new_ch)
{
    text.push_back(new_ch);
    std::map<char, TNode *>::iterator it = active_vertex->Edges().end();
    TNode *last_visit = nullptr;
    while ((active_length < active_vertex->GetLength() && text[active_vertex->GetBegin() + active_length] != new_ch) || (it = active_vertex->Edges().find(new_ch), active_length >= active_vertex->GetLength() && it == active_vertex->Edges().end())) {
        if (active_length < active_vertex->GetLength() && text[active_vertex->GetBegin() + active_length] != new_ch) {
            TNode *temp = new TNode(text, active_vertex, active_length);
            if (last_visit != nullptr) {
                last_visit->SetSuffixLink(temp);
            }
            last_visit = active_vertex->GetParent();
            active_vertex = last_visit->GetParent()->GetSuffixLink()->Edges().at(text[active_char_idx]);
            if (active_vertex == root) {
                --active_length;
                ++active_char_idx;
            }
            for (std::size_t length; active_length > active_vertex->GetLength(); active_length -= length) {
                length = active_vertex->GetLength();
                active_char_idx += length;
                active_vertex = active_vertex->Edges().at(text[active_char_idx]);
            }
        } else {
            new TNode(text, active_vertex);
            if (last_visit != nullptr) {
                last_visit->SetSuffixLink(active_vertex);
                last_visit = nullptr;
            }
            if (active_vertex != root) {
                active_vertex = active_vertex->GetSuffixLink();
                active_length = active_vertex->GetLength();
            }
        }
    }
    if (last_visit != nullptr) {
        last_visit->SetSuffixLink(active_vertex);
    }
    if (active_length >= active_vertex->GetLength() && it != active_vertex->Edges().end() && it->second->GetBegin() < text.size() - 1u) {
        active_length = 1u;
        active_vertex = it->second;
        active_char_idx = active_vertex->GetBegin();
    } else {
        ++active_length;
    }
}

std::vector<std::size_t> TSuffixTree::Find(const std::string &pattern) const
{
    std::vector<std::size_t> result;
    if (pattern.size() >= text.size()) {
        return result;
    }
    
    TNode *curr = root;
    std::size_t summary = 0u;
    for (std::size_t edge_idx = 0u, p_idx = 0u; p_idx < pattern.size(); ++edge_idx, ++p_idx) {
        if (edge_idx >= curr->GetLength()) {
            std::map<char, TNode *>::iterator it = curr->Edges().find(pattern[p_idx]);
            if (it == curr->Edges().end()) {
                return result;
            }
            
            curr = it->second;
            if (curr->GetLength() == std::numeric_limits<std::size_t>::max()) {
                summary += text.size() - curr->GetBegin();
            } else {
                summary += curr->GetLength();
            }
            edge_idx = 0u;
        }
        if (pattern[p_idx] != text[curr->GetBegin() + edge_idx]) {
            return result;
        }
    }
    
    DeepFirstSearch(curr, result, summary);
    
    if (result.size() > 1u) {
        std::sort(result.begin(), result.end());
    }
    
    return result;
}

void TSuffixTree::DeepFirstSearch(TNode *const &curr, std::vector<std::size_t> &result, const std::size_t &summary) const
{
    if (curr->Edges().empty()) {
        result.push_back(text.size() - summary);
        
        return;
    }
    
    for (std::map<char, TNode *>::const_iterator it = curr->Edges().begin(); it != curr->Edges().end(); ++it) {
        std::size_t temp = summary;
        if (it->second->GetLength() == std::numeric_limits<std::size_t>::max()) {
            temp += text.size() - it->second->GetBegin();
        } else {
            temp += it->second->GetLength();
        }
        DeepFirstSearch(it->second, result, temp);
    }
}

TSuffixTree::~TSuffixTree()
{
    root->GetSuffixLink()->Edges().clear();
    delete root->GetSuffixLink();
    delete root;
}
#include <cassert>
TMatchingStatistics::TMatchingStatistics(const TSuffixTree &tree, const std::string &text) :
    statistic(text.size()), pattern_size(tree.text.size() - 1u)
{
    std::size_t active_length = 0u, active_char_idx = 0u, summary = 0u;
    TNode *active_vertex = tree.root;
    
    for (std::string::const_iterator it = text.cbegin(), pos = text.cbegin(); it != text.cend(); ++it) {
        //      assert(summary < text.size() && summary < tree.text.size());
        std::map<char, TNode *>::const_iterator link;
        while (pos != text.cend() && ((active_length < active_vertex->GetLength() && *pos == tree.text[active_vertex->GetBegin() + active_length]) || (active_length >= active_vertex->GetLength() && (link = active_vertex->Edges().find(*pos), link != active_vertex->Edges().cend())))) {
            if (active_length >= active_vertex->GetLength()) {
                summary += active_vertex->GetLength();
                active_vertex = link->second;
                active_char_idx = active_vertex->GetBegin();
                active_length = 0u;
            }
            ++active_length;
            ++pos;
        }
        statistic[it - text.cbegin()] = summary + active_length;
        if (active_vertex == tree.root) {
            //          assert(!summary);
            ++pos;
            continue;
        }
        if (active_length < active_vertex->GetLength()) {
            active_vertex = active_vertex->GetParent()->GetSuffixLink()->Edges().at(tree.text[active_char_idx]);
            if (active_vertex == tree.root) {
                --active_length;
                ++active_char_idx;
                ++summary;
            }
            for (std::size_t length; active_length > active_vertex->GetLength(); active_length -= length, summary += length) {
                length = active_vertex->GetLength();
                active_char_idx += length;
                active_vertex = active_vertex->Edges().at(tree.text[active_char_idx]);
            }
        } else {
            active_vertex = active_vertex->GetSuffixLink();
            active_length = active_vertex->GetLength();
            if (active_vertex == tree.root) {
                ++summary;
            }
        }
        --summary;
    }
}

std::vector<std::size_t> TMatchingStatistics::Find() const
{
    std::vector<std::size_t> result;
    for (std::vector<std::size_t>::const_iterator it = statistic.cbegin(); it != statistic.cend(); ++it) {
        if (*it == pattern_size) {
            result.push_back(it - statistic.cbegin());
        }
    }
    
    return result;
}

TMatchingStatistics::~TMatchingStatistics()
{
}