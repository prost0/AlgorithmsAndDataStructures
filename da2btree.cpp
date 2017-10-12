#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <cctype>
//#include "btree.h"

using namespace std;



#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>

using namespace std;


typedef unsigned long long ull;
const size_t STR_LENTH = 258;
const char SAVE[] = "save";
const char LOAD[] = "load";
const unsigned int t = 3;

class TData
{
public:
    char *key;
    size_t keyLen;
    ull value;
    
    TData();
};


class TBTreeNode
{
public:
    bool isLeaf;
    TBTreeNode *father;
    TData *dataElements;
    size_t dataElementsNumber;
    TBTreeNode **children;
    size_t childrenNumber;
    
    TBTreeNode(TBTreeNode *parent, bool is_leaf);
    ~TBTreeNode();
    
    void InsertElementToLeaf(TData dataElement);
    void DeleteElementFromLeaf(size_t pos);
    size_t GetInsertPosition(TBTreeNode *child);
    void Print(size_t lvl);
    void PrintDataElements();
    void Save(FILE *out);
};


class TSearchRes
{
public:
    TData dataElement;
    bool isFound;
    TBTreeNode *foundNode;
    size_t position;
};


class TBTree
{
private:
    TBTreeNode *root;
    size_t factorT;
    
    size_t Split(TBTreeNode *node);
    void Unite(TBTreeNode *node, size_t pos);
    void Balance(TBTreeNode *CurrentNode, size_t pos);
    void RotateLeft(TBTreeNode *CurrentNode);
    void RotateRight(TBTreeNode *CurrentNode);
    void TreeDestroy(TBTreeNode *node);
    TBTreeNode *LoadNodes(FILE *in, TBTreeNode *parent);
    
public:
    TBTree(size_t factor);
    ~TBTree();
    
    TSearchRes Search(char *key);
    void Push(TData dataElement);
    TData Pop(char *key);
    void Print();
    bool Save(char *path);
    bool Load(char *path);
};


void strtolower(char *str)
{
    for (int i = 0; str[i] != '\0'; ++i) {
        if (str[i] >= 'A' && str[i] <= 'Z') {
            str[i] += 'a' - 'A';
        }
    }
}


int main()
{
    ios::sync_with_stdio(false);
    TBTree *btree = new TBTree(t);
    char input[STR_LENTH];
    char filePath[STR_LENTH];
    
    while (!cin.eof()) {
        cin >> input;
        strtolower(input);
        if (cin.eof()) {
            break;
        }
        TData inputData;
        TSearchRes searchRes;
        char inputStr[STR_LENTH];
        
        switch (input[0]) {
            case '\n':
                break;
            case '+':
                cin >> inputStr >> inputData.value;
                strtolower(inputStr);
                inputData.keyLen = 1;
                for (int i = 0; inputStr[i] != '\0' && i < STR_LENTH - 1; i++) {
                    inputData.keyLen++;
                }
                inputData.key = new char[inputData.keyLen];
                inputData.key[inputData.keyLen - 1] = '\0';
                for (int i = 0; i < inputData.keyLen; i++) {
                    inputData.key[i] = inputStr[i];
                }
                btree->Push(inputData);
                break;
            case '-':
                cin >> inputStr;
                strtolower(inputStr);
                inputData = btree->Pop(inputStr);
                delete [] inputData.key;
                break;
            case '!':
                cin >> inputStr;
                strtolower(inputStr);
                cin >> filePath;
                if (strcmp(inputStr, SAVE) == 0) {
                    btree->Save(filePath);
                    break;
                }
                if (strcmp(inputStr, LOAD) == 0) {
                    btree->Load(filePath);
                    break;
                }
                break;
            default:
                TSearchRes searchRes = btree->Search(input);
                if (searchRes.isFound) {
                    cout << "OK: " << searchRes.dataElement.value << "\n";
                } else {
                    cout << "NoSuchWord" << "\n";
                }
                break;
        }
    }
    delete btree;
    
    return 0;
}


TData::TData()
{
    value = 0;
}


TBTreeNode::TBTreeNode(TBTreeNode *parent, bool leaf)
{
    father = parent;
    dataElements = NULL;
    dataElementsNumber = 0;
    children = NULL;
    childrenNumber = 0;
    isLeaf = leaf;
}


TBTreeNode::~TBTreeNode()
{
    free(dataElements);
    free(children);
}


void TBTreeNode::InsertElementToLeaf(TData dataElement)
{
    //Binnary search
    size_t left = 0;
    size_t right = dataElementsNumber - 1;
    if (dataElementsNumber == 0) {
        right = 0;
    }
    while (right > left) {
        size_t tmpSize = right - left + 1;
        size_t middle = tmpSize / 2 + left;
        if (strcmp(dataElement.key, dataElements[middle].key) < 0) {
            right = middle - 1;
        } else {
            left = middle;
        }
    }
    
    //Insert dataElement
    size_t insert_pos = left;
    if (dataElementsNumber != 0) {
        int cmp = strcmp(dataElement.key, dataElements[insert_pos].key);
        if (insert_pos != 0 || cmp >= 0) {
            insert_pos++;
        }
    }
    dataElementsNumber++;
    dataElements = (TData *)realloc(dataElements, dataElementsNumber * sizeof(TData));
    for (size_t i = dataElementsNumber - 1; i > insert_pos; i--) {
        dataElements[i] = dataElements[i - 1];
    }
    dataElements[insert_pos] = dataElement;
}


void TBTreeNode::DeleteElementFromLeaf(size_t pos)
{
    dataElementsNumber--;
    for (size_t i = pos; i < dataElementsNumber; i++) {
        dataElements[i] = dataElements[i + 1];
    }
    dataElements = (TData *)realloc(dataElements, dataElementsNumber * sizeof(TData));
}


size_t TBTreeNode::GetInsertPosition(TBTreeNode *child)
{
    for (size_t i = 0; i < childrenNumber; i++) {
        if (children[i] == child) {
            return i;
        }
    }
    return 0;
}


void TBTreeNode::Print(size_t lvl)
{
    for (size_t i = 0; i < dataElementsNumber; i++) {
        for (size_t j = 0; j < lvl; j++) {
            cout << "  ";
        }
        cout << dataElements[i].key << ":" << dataElements[i].value << "\n";
    }
    cout << "----------" << "\n";
    for (size_t i = 0; i < childrenNumber; i++) {
        children[i]->Print(lvl + 1);
    }
}


void TBTreeNode::PrintDataElements()
{
    cout << "NodeContent::";
    for (size_t i = 0; i < dataElementsNumber; i++) {
        cout << dataElements[i].key << " ";
    }
    cout << "\n";
}


TBTree::TBTree(size_t factor)
{
    root = NULL;
    factorT = factor;
}


TBTree::~TBTree()
{
    if (root != NULL) {
        TreeDestroy(root);
    }
}


void TBTree::TreeDestroy(TBTreeNode *node)
{
    for (size_t i = 0; i < node->childrenNumber; i++) {
        if (i != node->childrenNumber - 1) {
            delete[] node->dataElements[i].key;
        }
        TreeDestroy(node->children[i]);
    }
    delete node;
}


size_t TBTree::Split(TBTreeNode *node)
{
    //Separate arrays
    size_t middle = node->dataElementsNumber / 2;
    TData middleDataElement = node->dataElements[middle];
    
    size_t arr1dataElementsLen = middle;
    size_t arr2dataElementsLen = node->dataElementsNumber - middle - 1;
    
    TData *arr2dataElements = (TData *)calloc(arr2dataElementsLen, sizeof(TData));
    for (size_t i = middle + 1; i < node->dataElementsNumber; i++) {
        arr2dataElements[i - middle - 1] = node->dataElements[i];
    }
    TData *arr1dataElements = (TData *)realloc(node->dataElements, arr1dataElementsLen * sizeof(TData));
    size_t arr1childrenLen = 0;
    size_t arr2childrenLen = 0;
    
    TBTreeNode **arr1children = NULL;
    TBTreeNode **arr2children = NULL;
    if (!node->isLeaf) {
        arr1childrenLen = arr1dataElementsLen + 1;
        arr2childrenLen = arr2dataElementsLen + 1;
        
        arr2children = (TBTreeNode **)calloc(arr2childrenLen, sizeof(TBTreeNode *));
        for (size_t i = middle + 1; i < node->childrenNumber; i++) {
            arr2children[i - middle - 1] = node->children[i];
        }
        arr1children = (TBTreeNode **)realloc(node->children, arr1childrenLen * sizeof(TBTreeNode *));
    }
    
    if (node->father == NULL) {
        node->father = new TBTreeNode(NULL, false);
        node->father->childrenNumber = 1;
        node->father->children = (TBTreeNode **)calloc(node->father->childrenNumber, sizeof(TBTreeNode *));
        node->father->children[node->father->childrenNumber - 1] = node;
        root = node->father;
    }
    
    if (node->father->dataElementsNumber >= factorT * 2 - 1) {
        Split(node->father);
    }
    
    size_t insertPosDataElement = node->father->GetInsertPosition(node);
    size_t insertPosChildren = insertPosDataElement + 1;
    
    //Space
    node->father->dataElementsNumber++;
    node->father->dataElements = (TData *)realloc(node->father->dataElements, node->father->dataElementsNumber * sizeof(TData));
    for (size_t i = node->father->dataElementsNumber - 1; i > insertPosDataElement; i--) {
        node->father->dataElements[i] = node->father->dataElements[i - 1];
    }
    
    node->father->childrenNumber++;
    node->father->children = (TBTreeNode **)realloc(node->father->children, node->father->childrenNumber * sizeof(TBTreeNode *));
    for (size_t i = node->father->childrenNumber - 1; i > insertPosDataElement; i--) {
        node->father->children[i] = node->father->children[i - 1];
    }
    
    //Inserting
    node->father->dataElements[insertPosDataElement] = middleDataElement;
    
    node->father->children[insertPosChildren] = new TBTreeNode(node->father, node->isLeaf);
    node->father->children[insertPosChildren]->isLeaf = node->isLeaf;
    
    node->father->children[insertPosChildren]->dataElements = arr2dataElements;
    node->father->children[insertPosChildren]->dataElementsNumber = arr2dataElementsLen;
    
    if (!node->isLeaf) {
        TBTreeNode *thsTmp = node->father->children[insertPosChildren];
        thsTmp->children = arr2children;
        thsTmp->childrenNumber = arr2childrenLen;
        for (size_t i = 0; i < thsTmp->childrenNumber; i++) {
            thsTmp->children[i]->father = thsTmp;
        }
    }
    
    node->dataElements = arr1dataElements;
    node->dataElementsNumber = arr1dataElementsLen;
    
    if (!node->isLeaf) {
        node->children = arr1children;
        node->childrenNumber = arr1childrenLen;
    }
    
    return insertPosDataElement;
}


void TBTree::Unite(TBTreeNode *node, size_t pos)
{
    if (node->isLeaf) {
        cout << "Unite error: Node is leaf" << "\n";
        return;
    }
    if (pos >= node->dataElementsNumber) {
        cout << "Unite error: Unable position" << "\n";
        return;
    }
    
    //Creating united arrays
    size_t newArrDataElementsLen = 1;
    newArrDataElementsLen += node->children[pos]->dataElementsNumber;
    newArrDataElementsLen += node->children[pos + 1]->dataElementsNumber;
    
    TData *new_arr_dataElements = (TData *)realloc(node->children[pos]->dataElements, newArrDataElementsLen * sizeof(TData));
    new_arr_dataElements[node->children[pos]->dataElementsNumber] = node->dataElements[pos];
    for (size_t i = node->children[pos]->dataElementsNumber + 1; i < newArrDataElementsLen; i++) {
        size_t tmpPos = i - node->children[pos]->dataElementsNumber - 1;
        new_arr_dataElements[i] = node->children[pos + 1]->dataElements[tmpPos];
    }
    
    size_t newArrChildrenLen = 0;
    TBTreeNode **newArrChildren = NULL;
    if (!node->children[pos]->isLeaf) {
        newArrChildrenLen = node->children[pos]->childrenNumber;
        newArrChildrenLen += node->children[pos + 1]->childrenNumber;
        newArrChildren = (TBTreeNode **)realloc(node->children[pos]->children, newArrChildrenLen * sizeof(TBTreeNode *));
        
        for (size_t i = node->children[pos]->childrenNumber; i < newArrChildrenLen; i++) {
            size_t tmpPos = i - node->children[pos]->childrenNumber;
            newArrChildren[i] = node->children[pos + 1]->children[tmpPos];
        }
    }
    
    //Deleting second branch and creating united child in first branch
    delete node->children[pos + 1];
    
    node->children[pos]->dataElements = new_arr_dataElements;
    node->children[pos]->dataElementsNumber = newArrDataElementsLen;
    node->children[pos]->children = newArrChildren;
    node->children[pos]->childrenNumber = newArrChildrenLen;
    
    //Creating new parent arrays
    size_t newParentArrDataElementsLen = node->dataElementsNumber - 1;
    size_t newParentArrChildrenLen = node->childrenNumber - 1;
    
    for (size_t i = pos + 1; i < node->dataElementsNumber; i++) {
        node->dataElements[i - 1] = node->dataElements[i];
    }
    for (size_t i = pos + 2; i < node->childrenNumber; i++) {
        node->children[i - 1] = node->children[i];
    }
    
    node->dataElements = (TData *)realloc(node->dataElements, newParentArrDataElementsLen * sizeof(TData));
    node->children = (TBTreeNode **)realloc(node->children, newParentArrChildrenLen * sizeof(TBTreeNode *));
    
    //Refreshing arrays
    node->dataElementsNumber = newParentArrDataElementsLen;
    node->childrenNumber = newParentArrChildrenLen;
    for (size_t i = 0; i < node->children[pos]->childrenNumber; i++) {
    
        node->children[pos]->children[i]->father = node->children[pos];
    }
    
    size_t tmpPos = 0;
    if (node->father != NULL) {
        for (size_t i = 0; i < node->father->childrenNumber; i++) {
            if (node == node->father->children[i]) {
                tmpPos = i;
                break;
            }
        }
    }
    
    Balance(node, tmpPos);
}


void TBTree::Balance(TBTreeNode *CurrentNode, size_t pos)
{
    if (CurrentNode == NULL) {
        return;
    }
    if (CurrentNode->dataElementsNumber == 0 && CurrentNode->father == NULL) {
        CurrentNode = CurrentNode->children[0];
        delete root;
        root = CurrentNode;
        root->father = NULL;
        return;
    }
    if (CurrentNode->dataElementsNumber >= factorT - 1 || CurrentNode->father == NULL) {
        return;
    }
    
    bool isLeftLarger = false;
    bool isRightLarger = false;
    
    if (pos > 0) {
        if (CurrentNode->father->children[pos - 1]->dataElementsNumber > factorT - 1) {
            isLeftLarger = true;
        }
    }
    if (pos < CurrentNode->father->childrenNumber - 1) {
        if (CurrentNode->father->children[pos + 1]->dataElementsNumber > factorT - 1) {
            isRightLarger = true;
        }
    }
    
    if (!isLeftLarger && !isRightLarger) {
        if (pos > 0) {
            Unite(CurrentNode->father, pos - 1);
        } else {
            Unite(CurrentNode->father, pos);
        }
        return;
    }
    if (isLeftLarger) {
        RotateRight(CurrentNode);
    } else {
        RotateLeft(CurrentNode);
    }
}


void TBTree::RotateLeft(TBTreeNode *CurrentNode)
{
    size_t pos = 0;
    for (size_t i = 0; i < CurrentNode->father->childrenNumber; i++) {
        if (CurrentNode == CurrentNode->father->children[i]) {
            pos = i;
            break;
        }
    }
    
    size_t rightFirst = 0;
    TData k1 = CurrentNode->father->children[pos + 1]->dataElements[rightFirst];
    TData k2 = CurrentNode->father->dataElements[pos];
    CurrentNode->father->children[pos + 1]->DeleteElementFromLeaf(rightFirst);
    CurrentNode->father->dataElements[pos] = k1;
    
    CurrentNode->dataElementsNumber++;
    CurrentNode->dataElements = (TData *)realloc(CurrentNode->dataElements, CurrentNode->dataElementsNumber * sizeof(TData));
    CurrentNode->dataElements[CurrentNode->dataElementsNumber - 1] = k2;
    
    if (!CurrentNode->isLeaf) {
        TBTreeNode *tRight = CurrentNode->father->children[pos + 1];
        TBTreeNode *tmp = tRight->children[rightFirst];
        
        for (size_t i = 1; i < tRight->childrenNumber; i++) {
            tRight->children[i - 1] = tRight->children[i];
        }
        tRight->childrenNumber--;
        tRight->children = (TBTreeNode **)realloc(tRight->children, tRight->childrenNumber * sizeof(TBTreeNode *));
        
        tmp->father = CurrentNode;
        CurrentNode->childrenNumber++;
        CurrentNode->children = (TBTreeNode **)realloc(CurrentNode->children, CurrentNode->childrenNumber * sizeof(TBTreeNode *));
        CurrentNode->children[CurrentNode->childrenNumber - 1] = tmp;
    }
}


void TBTree::RotateRight(TBTreeNode *CurrentNode)
{
    size_t pos = 0;
    for (size_t i = 0; i < CurrentNode->father->childrenNumber; i++) {
        if (CurrentNode == CurrentNode->father->children[i]) {
            pos = i;
            break;
        }
    }
    //cout << "Rotate::Point1" << "\n";
    size_t leftLast = CurrentNode->father->children[pos - 1]->dataElementsNumber - 1;
    TData k1 = CurrentNode->father->children[pos - 1]->dataElements[leftLast];
    TData k2 = CurrentNode->father->dataElements[pos - 1];
    CurrentNode->father->children[pos - 1]->DeleteElementFromLeaf(leftLast);
    CurrentNode->father->dataElements[pos - 1] = k1;
    
    CurrentNode->dataElementsNumber++;
    CurrentNode->dataElements = (TData *)realloc(CurrentNode->dataElements, CurrentNode->dataElementsNumber * sizeof(TData));
    for (size_t i = CurrentNode->dataElementsNumber - 1; i > 0; i--) {
        CurrentNode->dataElements[i] = CurrentNode->dataElements[i - 1];
    }
    CurrentNode->dataElements[0] = k2;
    
    if (!CurrentNode->isLeaf) {
        TBTreeNode *tLeft = CurrentNode->father->children[pos - 1];
        TBTreeNode *tmp = tLeft->children[leftLast + 1];
        
        tLeft->childrenNumber--;
        tLeft->children = (TBTreeNode **)realloc(tLeft->children, tLeft->childrenNumber * sizeof(TBTreeNode *));
        
        tmp->father = CurrentNode;
        CurrentNode->childrenNumber++;
        CurrentNode->children = (TBTreeNode **)realloc(CurrentNode->children, CurrentNode->childrenNumber * sizeof(TBTreeNode *));
        for (size_t i = CurrentNode->childrenNumber - 1; i > 0; i--) {
            CurrentNode->children[i] = CurrentNode->children[i - 1];
        }
        CurrentNode->children[0] = tmp;
    }
}


TSearchRes TBTree::Search(char *searchKey)
{
    TSearchRes res;
    TBTreeNode *node = root;
    res.foundNode = node;
    res.position = 0;
    res.isFound = false;
    
    while (node != NULL) {
    
        //Binnary search
        size_t left = 0;
        size_t right = node->dataElementsNumber - 1;
        if (node->dataElementsNumber == 0) {
            right = 0;
        }
        while (right > left) {
            size_t tmpSize = right - left + 1;
            size_t middle = tmpSize / 2 + left;
            int cmp = strcmp(searchKey, node->dataElements[middle].key);
            if (cmp == 0) {
                res.dataElement = node->dataElements[middle];
                res.position = middle;
                res.isFound = true;
                res.foundNode = node;
                return res;
            }
            if (cmp < 0) {
                right = middle - 1;
            } else {
                left = middle;
            }
        }
        int cmp = strcmp(searchKey, node->dataElements[left].key);
        if (cmp == 0) {
            res.dataElement = node->dataElements[left];
            res.position = left;
            res.isFound = true;
            return res;
        }
        if (!node->isLeaf) {
            if (cmp < 0) {
                node = node->children[left];
            } else {
                node = node->children[left + 1];
            }
        } else {
            res.foundNode = node;
            break;
        }
        if (node != NULL) {
            res.foundNode = node;
        }
    }
    res.isFound = false;
    return res;
}


void TBTree::Push(TData dataElement)
{

    TSearchRes searchRes = Search(dataElement.key);
    TBTreeNode *CurrentNode = searchRes.foundNode;
    
    if (searchRes.isFound) {
        cout << "Exist" << "\n";
        return;
    }
    
    if (CurrentNode == NULL) {
        CurrentNode = new TBTreeNode(NULL, true);
        root = CurrentNode;
    }
    
    if (CurrentNode->dataElementsNumber >= factorT * 2 - 1) {
        size_t insert_pos = Split(CurrentNode);
        if (strcmp(dataElement.key, CurrentNode->father->dataElements[insert_pos].key) >= 0) {
            CurrentNode = CurrentNode->father->children[insert_pos + 1];
        }
    }
    
    CurrentNode->InsertElementToLeaf(dataElement);
    cout << "OK" << "\n";
}


TData TBTree::Pop(char *searchKey)
{
    TData res;
    res.value = 0;
    res.key = NULL;
    TSearchRes searchRes = Search(searchKey);
    if (!searchRes.isFound) {
        cout << "NoSuchWord" << "\n";
        return res;
    }
    TBTreeNode *CurrentNode = searchRes.foundNode;
    size_t pos = searchRes.position;
    
    res = CurrentNode->dataElements[pos];
    while (!CurrentNode->isLeaf) {
        TBTreeNode *left = CurrentNode->children[pos];
        TBTreeNode *right = CurrentNode->children[pos + 1];
        
        bool isLeftLarger = false;
        bool isRightLarger = false;
        
        if (left->dataElementsNumber > factorT - 1) {
            isLeftLarger = true;
        }
        
        if (right->dataElementsNumber > factorT - 1) {
            isRightLarger = true;
        }
        
        if (!isLeftLarger && !isRightLarger) {
            Unite(CurrentNode, pos);
            CurrentNode = left;
            pos = CurrentNode->dataElementsNumber / 2;
            continue;
        }
        
        if (isLeftLarger) {
            RotateRight(right);
            CurrentNode = right;
            pos = 0;
        } else {
            RotateLeft(left);
            CurrentNode = left;
            pos = CurrentNode->dataElementsNumber - 1;
            
        }
    }
    
    if (CurrentNode == root && CurrentNode->dataElementsNumber == 1) {
        TreeDestroy(root);
        root = NULL;
        cout << "OK" << "\n";
        return res;
    }
    
    CurrentNode->DeleteElementFromLeaf(pos);
    
    size_t parent_pos = 0;
    if (CurrentNode->father != NULL) {
        for (size_t i = 0; i < CurrentNode->father->childrenNumber; i++) {
            if (CurrentNode == CurrentNode->father->children[i]) {
                parent_pos = i;
                break;
            }
        }
    }
    
    Balance(CurrentNode, parent_pos);
    
    cout << "OK" << "\n";
    return res;
}


void TBTree::Print()
{
    if (root != NULL) {
        root->Print(0);
    } else {
        cout << "Tree is empty" << "\n";
    }
}


bool TBTree::Save(char *path)
{
    FILE *out = fopen(path, "wb");
    if (out == NULL) {
        cout << "ERROR: Couldn't create file" << "\n";
        return false;
    }
    
    //Writing prefix
    char is_empty = (char)false;
    if (root == NULL) {
        is_empty = (char)true;
        fwrite(&is_empty, 1, 1, out);
        fclose(out);
        cout << "OK" << "\n";
        return true;
    }
    fwrite(&is_empty, 1, 1, out);
    root->Save(out);
    fclose(out);
    cout << "OK" << "\n";
    return true;
}


bool TBTree::Load(char *path)
{
    FILE *in = fopen(path, "rb");
    
    if (in == NULL) {
        cout << "ERROR: Couldn't open file" << "\n";
        return false;
    }
    
    if (root != NULL) {
        TreeDestroy(root);
    }
    root = NULL;
    
    char is_empty;
    fread(&is_empty, 1, 1, in);
    if ((bool)is_empty) {
        root = NULL;
        fclose(in);
        cout << "OK" << "\n";
        return true;
    }
    root = LoadNodes(in, NULL);
    fclose(in);
    
    cout << "OK" << "\n";
    return true;
}


void TBTreeNode::Save(FILE *out)
{
    if (out == NULL) {
        return;
    }
    char is_leaf = (char)isLeaf;
    fwrite(&is_leaf, 1, 1, out);
    fwrite(&dataElementsNumber, sizeof(size_t), 1, out);
    for (size_t i = 0; i < dataElementsNumber; i++) {
        fwrite(&dataElements[i].keyLen, sizeof(size_t), 1, out);
        for (size_t j = 0; j < dataElements[i].keyLen; j++) {
            fwrite(dataElements[i].key + j, sizeof(char), 1, out);
        }
        fwrite(&dataElements[i].value, sizeof(ull), 1, out);
    }
    for (size_t i = 0; i < childrenNumber; i++) {
        children[i]->Save(out);
    }
}


TBTreeNode *TBTree::LoadNodes(FILE *in, TBTreeNode *parent)
{
    if (feof(in)) {
        return NULL;
    }
    char is_leaf;
    fread(&is_leaf, 1, 1, in);
    TBTreeNode *node = new TBTreeNode(parent, (bool)is_leaf);
    
    fread(&node->dataElementsNumber, sizeof(size_t), 1, in);
    node->dataElements = (TData *)calloc(node->dataElementsNumber, sizeof(TData));
    for (size_t i = 0; i < node->dataElementsNumber; i++) {
        fread(&node->dataElements[i].keyLen, sizeof(size_t), 1, in);
        node->dataElements[i].key = new char[node->dataElements[i].keyLen];
        for (size_t j = 0; j < node->dataElements[i].keyLen; j++) {
            fread(node->dataElements[i].key + j, sizeof(char), 1, in);
        }
        fread(&node->dataElements[i].value, sizeof(ull), 1, in);
    }
    if (!node->isLeaf) {
        node->childrenNumber = node->dataElementsNumber + 1;
        node->children = (TBTreeNode **)calloc(node->childrenNumber, sizeof(TBTreeNode *));
        for (size_t i = 0; i < node->childrenNumber; i++) {
            node->children[i] = LoadNodes(in, node);
        }
    }
    return node;
}