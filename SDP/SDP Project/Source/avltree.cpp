#include "../Headers/avltree.hpp"

void AVLTree::clearTree(RecordNode* root) {
    if (root != nullptr) {
        clearTree(root->leftNode);
        clearTree(root->rightNode);
        delete root;
    }
}

int AVLTree::compareTreeNodeAndRecord(RecordNode* value1, Record* value2) {
    if (dataType == FieldType::INT) {
        int value1int = 0, value2int = 0;
        value1int = stoi(value1->records.front()->getData(columnIndex)->getValueUniform());
        value2int = stoi(value2->getData(columnIndex)->getValueUniform());
        if (value1int == value2int) {
            return 0;
        } else {
            if (value1int < value2int)
                return -1;
            else
                return 1;
        }
    } else {
        std::string value1int, value2int;
        value1int = value1->records.front()->getData(columnIndex)->getValueUniform();
        value2int = value2->getData(columnIndex)->getValueUniform();
        (value1int.compare(value2int) > 1);
        if (value1int.compare(value2int) == 0) {
            return 0;
        } else {
            if (value1int.compare(value2int) > 0)
                return -1;
            else
                return 1;
        }
    }
}

int AVLTree::treeHeight(RecordNode* root) const {
    if (root != nullptr) return root->heightOfSubtree;
    return 0;
}

int AVLTree::recalculateHeight(RecordNode* root) const {
    int leftTreeHeight = treeHeight(root->leftNode);
    int rightTreeHeight = treeHeight(root->rightNode);
    if (leftTreeHeight > rightTreeHeight) {
        return leftTreeHeight + 1;
    } else {
        return rightTreeHeight + 1;
    }
    return 0;
}

void AVLTree::rotateRight(RecordNode*& root) {
    RecordNode* rootSubtreeLeft = root->leftNode;
    RecordNode* leftSubtreeRight = rootSubtreeLeft->rightNode;

    rootSubtreeLeft->rightNode = root;
    root->leftNode = leftSubtreeRight;

    root->heightOfSubtree = recalculateHeight(root);
    rootSubtreeLeft->heightOfSubtree = recalculateHeight(rootSubtreeLeft);

    root = rootSubtreeLeft;
}

void AVLTree::rotateLeft(RecordNode*& root) {
    RecordNode* rootSubtreeRight = root->rightNode;
    RecordNode* rightSubtreeLeft = rootSubtreeRight->leftNode;

    rootSubtreeRight->leftNode = root;
    root->rightNode = rightSubtreeLeft;

    root->heightOfSubtree = recalculateHeight(root);
    rootSubtreeRight->heightOfSubtree = recalculateHeight(rootSubtreeRight);

    root = rootSubtreeRight;
}

int AVLTree::balanceFactor(RecordNode* root) const {
    if (root == nullptr) return 0;
    return treeHeight(root->rightNode) - treeHeight(root->leftNode);
}

void AVLTree::insert(Record* value) {
    insertNode(avlRoot, value);
}

void AVLTree::insertNode(RecordNode*& root, Record* value) {
    if (root == nullptr) {
        root = new RecordNode(value);
    } else {
        if (compareTreeNodeAndRecord(root, value) == 1) insertNode(root->leftNode, value);
        if (compareTreeNodeAndRecord(root, value) == -1) insertNode(root->rightNode, value);
        if (compareTreeNodeAndRecord(root, value) == 0) root->records.push_back(value);
        // (compareTreeNodeAndRecord(root, value) ==  1)  (root->value > value)
        // (compareTreeNodeAndRecord(root, value) == -1)  (root->value < value)
    }

    root->heightOfSubtree = recalculateHeight(root);

    if (balanceFactor(root) < -1) {
        if (root->leftNode != nullptr && (compareTreeNodeAndRecord(root->leftNode, value) == 1)) {
            rotateRight(root);
        }
        if (root->leftNode != nullptr && (compareTreeNodeAndRecord(root->leftNode, value) == -1)) {
            rotateLeft(root->leftNode);
            rotateRight(root);
        }
    }
    if (balanceFactor(root) > 1) {
        if (root->rightNode != nullptr && (compareTreeNodeAndRecord(root->rightNode, value) == -1)) {
            rotateLeft(root);
        }
        if (root->rightNode != nullptr && (compareTreeNodeAndRecord(root->rightNode, value) == 1)) {
            rotateRight(root->rightNode);
            rotateLeft(root);
        }
    }
}

AVLTree::RecordNode* AVLTree::getMinNode(RecordNode* root) const {
    if (root->leftNode != nullptr) return getMinNode(root->leftNode);
    RecordNode* newRoot = root;
    root = root->rightNode;
    return newRoot;
}

void AVLTree::removeData(Record* value) {
    deleteNode(avlRoot, value);
}

void AVLTree::deleteNode(RecordNode*& root, Record* value) {
    if (!root) return;

    if (compareTreeNodeAndRecord(root, value) == 0) {
        RecordNode* nodeToDelete = root;
        if (root->records.size() > 1) {
            root->records.remove(value);
        } else {
            if (root->leftNode == nullptr && root->rightNode != nullptr) {
                root = root->rightNode;
            }
            if (root->leftNode != nullptr && root->rightNode == nullptr) {
                root = root->leftNode;
            }
            if (root->leftNode != nullptr && root->rightNode != nullptr) {
                RecordNode* minNodeFromRight = getMinNode(root->rightNode);
                minNodeFromRight->leftNode = root->leftNode;
                minNodeFromRight->rightNode = root->rightNode;
                root = minNodeFromRight;
            }
            if (root->leftNode == nullptr && root->rightNode == nullptr) {
                root = nullptr;
            }
            delete nodeToDelete;
        }
        return;
    } else {
        if (compareTreeNodeAndRecord(root, value) == 1) deleteNode(root->leftNode, value);
        if (compareTreeNodeAndRecord(root, value) == -1) deleteNode(root->rightNode, value);
    }

    if (!root) return;

    root->heightOfSubtree = recalculateHeight(root);

    if (balanceFactor(root) < -1) {
        if (root->leftNode) {
            if (balanceFactor(root->leftNode) >= 0) {
                rotateRight(root);
            } else {
                rotateLeft(root->leftNode);
                rotateRight(root);
            }
        }
    }
    if (balanceFactor(root) > 1) {
        if (root->rightNode) {
            if (balanceFactor(root->rightNode) >= 0) {
                rotateLeft(root);
            } else {
                rotateRight(root->rightNode);
                rotateLeft(root);
            }
        }
    }
}

std::list<Record*> AVLTree::searchNode(std::string value, OperatorType operatorCompare) {
    searchResult.clear();
    if (operatorCompare == OperatorType::EQ) {
        RecordNode* searchEqual = searchEqualNode(avlRoot, value);
        return searchEqual->records;
    } else {
        searchNodeCompare(avlRoot, value, operatorCompare);
    }
    return searchResult;
}

AVLTree::RecordNode* AVLTree::searchEqualNode(RecordNode*& root, std::string value) {
    if (root != nullptr) {
        if (dataType == FieldType::INT) {
            int currentValue = 0;
            currentValue = stoi(root->records.front()->getData(columnIndex)->getValueUniform());  // IS NUMBER?
            int searchingValueToInt = stoi(value);
            if (currentValue == searchingValueToInt) {
                return root;
            } else if (currentValue < searchingValueToInt) {
                return searchEqualNode(root->leftNode, value);
            } else {
                return searchEqualNode(root->rightNode, value);
            }
        }
    } else {
        return nullptr;
    }
}

AVLTree::RecordNode* AVLTree::searchNodeCompare(RecordNode*& root, std::string value, OperatorType operatorCompare) {
    if (root != nullptr) {
        if (dataType == FieldType::INT) {
            int currentValue = 0;
            currentValue = stoi(root->records.front()->getData(columnIndex)->getValueUniform());  // IS NUMBER?
            int searchingValueToInt = stoi(value);
            switch (operatorCompare) {
                case OperatorType::GT:
                    if (currentValue > searchingValueToInt && currentValue != searchingValueToInt) {
                        searchResult.insert(searchResult.end(), root->records.begin(), root->records.end());
                        searchNodeCompare(root->leftNode, value, operatorCompare);
                    }
                    searchNodeCompare(root->rightNode, value, operatorCompare);
                    break;
                case OperatorType::LT:
                    if (currentValue < searchingValueToInt && currentValue != searchingValueToInt) {
                        searchResult.insert(searchResult.end(), root->records.begin(), root->records.end());
                        searchNodeCompare(root->rightNode, value, operatorCompare);
                    }
                    searchNodeCompare(root->leftNode, value, operatorCompare);
                    break;
                case OperatorType::GE:
                    if (currentValue >= searchingValueToInt) {
                        searchResult.insert(searchResult.end(), root->records.begin(), root->records.end());
                        if (currentValue != searchingValueToInt) searchNodeCompare(root->leftNode, value, operatorCompare);
                    }
                    searchNodeCompare(root->rightNode, value, operatorCompare);
                    break;
                case OperatorType::LE:
                    if (currentValue <= searchingValueToInt) {
                        searchResult.insert(searchResult.end(), root->records.begin(), root->records.end());
                        if (currentValue != searchingValueToInt) searchNodeCompare(root->rightNode, value, operatorCompare);
                    }
                    searchNodeCompare(root->leftNode, value, operatorCompare);
                    break;
                default:
                    break;
            }
        } else {
            return nullptr;
        }
    }
}
