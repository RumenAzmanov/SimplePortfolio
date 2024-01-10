#ifndef AVLTREE_HPP
#define AVLTREE_HPP

#include <string>

#include "field.hpp"
#include "record.hpp"
#include "utilityhelper.hpp"

class AVLTree {
   private:
    struct RecordNode {
        RecordNode* leftNode;
        RecordNode* rightNode;
        int heightOfSubtree;
        std::list<Record*> records;
        int valueint;

        RecordNode() {}
        RecordNode(Record* value) : heightOfSubtree(0), leftNode(nullptr), rightNode(nullptr) { records.push_back(value); }
    };
    RecordNode* avlRoot = nullptr;
    int columnIndex = -1;
    FieldType dataType = FieldType::UNDEFINED;

   public:
    std::list<Record*> searchResult;

    AVLTree(int column, FieldType type) : columnIndex(column), dataType(type) {}
    ~AVLTree() {
        clearTree(avlRoot);
    }

    //! Clear all data from the tree
    void clearTree(RecordNode* root);

    //! Compare a node and a record by column
    int compareTreeNodeAndRecord(RecordNode* value1, Record* value2);

    //! Get node height
    int treeHeight(RecordNode* root) const;

    //! Recalculate the height of a node
    int recalculateHeight(RecordNode* root) const;

    //! Right node rotation
    void rotateRight(RecordNode*& root);

    //! Left node rotation
    void rotateLeft(RecordNode*& root);

    //! Get balance factor of a node
    int balanceFactor(RecordNode* root) const;

    //! Insert node wrapper function
    void insert(Record* value);

    //! Insert node under the root
    void insertNode(RecordNode*& root, Record* value);

    //! Get the minimal node from a tree
    RecordNode* getMinNode(RecordNode* root) const;

    //! Remove node wrapper function
    void removeData(Record* value);

    //! Remove node from the root
    void deleteNode(RecordNode*& root, Record* value);

    //! Search a node by value
    std::list<Record*> searchNode(std::string value, OperatorType operatorCompare);

    //! Search an equal node by value
    RecordNode* searchEqualNode(RecordNode*& root, std::string value);

    //! Search a node by value wit comparison operators
    RecordNode* searchNodeCompare(RecordNode*& root, std::string value, OperatorType operatorCompare);

    //! Helper function used under development
    // void printall() {
    //     print(avlRoot, 1);
    // }

    //! Helper function used under development
    // void print(const RecordNode* root, int level) const {
    //     if (root) {
    //         std::cout << "Level " << level << " " << root->records.front()->getData(2)->getValueUniform() << "\n";
    //         print(root->leftNode, level + 1);
    //         print(root->rightNode, level + 1);
    //     } else {
    //         std::cout << "Level " << level << " \n";
    //     }
    // }
};

#endif