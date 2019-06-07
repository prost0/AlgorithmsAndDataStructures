#include <iostream>
#include <cctype>
#include <string>
#include <vector>
#include <cstdio>
#include <queue>
#include <sstream>
#include <map>
#include <utility>

//#include "trie.h"

class TTrieNode
{
public:
    friend class TTrie;
    TTrieNode();
private:
    std::map <std::string, TTrieNode *> children;
    bool isLeaf;
    int patternNumber;
    TTrieNode *linkFail;
    TTrieNode *linkExit;
};

class TTrie
{
public:
    TTrie();
    bool InsertPattern(int);
    void CreatingLink();
    void Search();
    void DeleteTry(TTrieNode *node);
    ~TTrie();
private:
    TTrieNode *root;
    std::vector <int> lengthPatterns;
    int numberOfPatterns;
};



TTrieNode::TTrieNode()
{
    isLeaf = true;
    patternNumber = 0;
    linkExit = nullptr;
}

TTrie::TTrie()
{
    root = new TTrieNode;
    numberOfPatterns = 0;
    lengthPatterns.push_back(0);
}

bool TTrie::InsertPattern(int patternNumber)
{
    int sizePattern = 0;
    bool inserted = false;
    TTrieNode *curNode = root;
    std::string token, line;
    getline(std::cin, line);
    std::stringstream streamline(line);
    
    while (streamline >> token) {
        sizePattern++;
        inserted = true;
        
        for (int i = 0; i < token.size(); i++) {
            token[i] = (char) tolower(token[i]);
        }
        
        if (curNode->children.find(token) == curNode->children.end()) {
            TTrieNode *newSon = new TTrieNode;
            curNode->children.insert(make_pair(token, newSon));
            curNode->isLeaf = false;
            curNode = newSon;
        } else {
            curNode = curNode->children.find(token)->second;
        }
    }
    if (inserted) {
        curNode->patternNumber = patternNumber;
        numberOfPatterns++;
        lengthPatterns.push_back(sizePattern);
        if (lengthPatterns[0] < sizePattern) {
            lengthPatterns[0] = sizePattern;
        }
    }
    return inserted;
}

void TTrie::CreatingLink()
{
    for (auto i : root->children) {
        i.second->linkFail = root;
    }
    root->linkFail = root;
    std::queue<TTrieNode *> qChild;
    qChild.push(root);
    while (!qChild.empty()) {
        TTrieNode *node = qChild.front();
        if (node != root && !node->isLeaf) {
            for (auto i : node->children) {
                TTrieNode *w = node->linkFail,
                           *childNode = i.second;
                std::string x = i.first;
                while (w->children.find(x) == w->children.end() && w != root) {
                    w = w->linkFail;
                }
                if (w->children.find(x) != w->children.end()) {
                    childNode->linkFail = w->children.find(x)->second;
                } else {
                    childNode->linkFail = root;
                }
                if (childNode->linkFail->patternNumber) {
                    childNode->linkExit = childNode->linkFail;
                } else {
                    childNode->linkExit = childNode->linkFail->linkExit;
                }
            }
        }
        qChild.pop();
        if (!node->isLeaf) {
            for (auto i : node->children) {
                qChild.push(i.second);
            }
        }
    }
}



void TTrie::Search()
{
    std::string token, line;
    std::stringstream streamline;
    std::pair <int, int> curPos;
    curPos.first = 1;
    curPos.second = 1;
    std::vector<std::pair <int, int>> vPositions(lengthPatterns[0]);
    int indexInPositions = 0;
    TTrieNode *father = root,
               *son;
               
    getline(std::cin, line);
    streamline.str(line);
    bool notEnd = true;
    streamline >> token;
    for (int i = 0; i < token.size(); i++) {
        token[i] = (char) tolower(token[i]);
    }
    vPositions[indexInPositions] = curPos;
    
    do {
        while (father->children.find(token) != father->children.end()) {
            son = father->children.find(token)->second;
            TTrieNode *nodeExit = son->linkExit;
            if (son->patternNumber) {
                int index = indexInPositions - lengthPatterns[son->patternNumber] + 1;
                if (index < 0) {
                    index = lengthPatterns[0] + index;
                }
                std::cout << vPositions[index].second << ", " <<  vPositions[index].first << ", " << son->patternNumber << "\n";
            }
            while (nodeExit) {
                int index = indexInPositions - lengthPatterns[nodeExit->patternNumber] + 1;
                if (index < 0) {
                    index = lengthPatterns[0] + index;
                }
                std::cout << vPositions[index].second << ", " <<  vPositions[index].first << ", " << nodeExit->patternNumber << "\n";
                nodeExit = nodeExit->linkExit;
            }
            father = son;
            token.clear();
            if (streamline >> token) {
                curPos.first++;
                indexInPositions++;
                if (indexInPositions == lengthPatterns[0]) {
                    indexInPositions = 0;
                }
            } else  {
                curPos.first = 1;
                curPos.second++;
                indexInPositions++;
                if (indexInPositions == lengthPatterns[0]) {
                    indexInPositions = 0;
                }
                line.clear();
                if (!getline(std::cin, line)) {
                    notEnd = false;
                }
                if (line.size() < 1) {
                    while (line.size() < 1 && notEnd) {
                        curPos.second++;
                        if (!getline(std::cin, line)) {
                            notEnd = false;
                        }
                    }
                    streamline.clear();
                    streamline.str(line);
                    streamline >> token;
                } else {
                    streamline.clear();
                    streamline.str(line);
                    streamline >> token;
                }
            }
            
            for (int i = 0; i < token.size(); i++) {
                token[i] = (char) tolower(token[i]);
            }
            
            vPositions[indexInPositions] = curPos;
        }
        
        if (father == root) {
            if (streamline >> token) {
                curPos.first++;
                indexInPositions++;
                if (indexInPositions == lengthPatterns[0]) {
                    indexInPositions = 0;
                }
            } else  {
                curPos.first = 1;
                curPos.second++;
                indexInPositions++;
                if (indexInPositions == lengthPatterns[0]) {
                    indexInPositions = 0;
                }
                line.clear();
                if (!getline(std::cin, line)) {
                    notEnd = false;
                }
                if (line.size() < 1) {
                    while (line.size() < 1 && notEnd) {
                        curPos.second++;
                        if (!getline(std::cin, line)) {
                            notEnd = false;
                        }
                    }
                    streamline.clear();
                    streamline.str(line);
                    streamline >> token;
                } else {
                    streamline.clear();
                    streamline.str(line);
                    streamline >> token;
                }
            }
            
            for (int i = 0; i < token.size(); i++) {
                token[i] = (char) tolower(token[i]);
            }
            
            vPositions[indexInPositions] = curPos;
        } else {
            father = father->linkFail;
        }
        
    } while (notEnd);
    
}

void TTrie::DeleteTry(TTrieNode *node)
{
    for (auto it : node->children) {
        DeleteTry(it.second);
    }
    delete node;
}

TTrie::~TTrie()
{
    DeleteTry(root);
    root = nullptr;
    lengthPatterns.clear();
}


int main()
{
    TTrie keysTree;
    int numPat = 1;
    while (keysTree.InsertPattern(numPat)) {
        numPat++;
    }
    keysTree.CreatingLink();
    keysTree.Search();
}
