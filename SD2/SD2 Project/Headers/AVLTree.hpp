#ifndef AVLTREE_HPP
#define AVLTREE_HPP

#include <iostream>
#include <queue>

template <class T>
class AVLTree {
   private:
    //! Tree node
    struct Node {
       public:
        T value;             //!< Node data
        int height;          //!< Node height
        Node *left, *right;  //!< Left and right subtrees

        Node(const T inputValue,
             Node* left = nullptr,
             Node* right = nullptr,
             int height = 1)
            : value(inputValue),
              left(left),
              right(right),
              height(height) {}

        ~Node() {
            left = nullptr;
            right = nullptr;
        }
    };

    Node* root;                //!< First node
    long long int treeWeight;  //!< Number of elements in the tree

   public:
    AVLTree();
    AVLTree(const AVLTree& otherTree);
    AVLTree& operator=(const AVLTree& otherTree);
    ~AVLTree();

    //! Insert a value in the structure - wrapper method
    void insert(const T value);
    //! Find if a value is in the structure - wrapper method
    bool find(const T key) const;
    //! Remove a value from the structure - wrapper method
    bool erase(T value);
    //! Clear all data from the structure - wrapper method
    void clear();

    //! Get the number of elements in the tree
    long long int getTreeWeight() const;
    //! Get the height of the tree
    long long int getTreeHeight() const;
    //! Check if there are any elements in the tree
    bool empty() const;
    //! Print all elements in sorted order - wrapper method
    void printInorder(std::ostream& out) const;
    //! Print the structure of the elements - wrapper method
    void printStructure(std::ostream& out) const;
    //! Calculate the required memory for the structure
    long long int checkMemory() const;

   private:
    //! Clear the entire subtree recursively
    void clear(Node*& node);
    //! Copy the entire subtree recursively
    Node* copy(Node* copyNode);

    //! Directly get the height of the node
    int getNodeHeight(const Node* node) const;
    //! Calculate the height of the node based on the
    //! heights of the children nodes
    int calculateHeight(const Node* node) const;
    //! Calculate the balance factor of the node
    int calculateBalanceFactor(const Node* node) const;

    //! Perform a right rotation on the node
    void rotateRight(Node*& node);
    //! Perform a left rotation on the node
    void rotateLeft(Node*& node);
    //! Perform a set of rotations to balance the node
    void updateNode(Node*& node);

    //! Insert a value in the structure
    void insert(Node*& node, T value);
    //! Find if a value is in the structure recursively
    bool find(const Node* node, const T value) const;

    //! Get the min node in the subtree
    Node* getMinNode(Node*& node);
    //! Erase the min node in the subtree
    void eraseMinNode(Node*& node);
    //! Remove a value from the structure
    bool erase(Node*& node, T value);

    //! Print all elements in sorted order
    void printInorder(const Node* node, std::ostream& out) const;
    //! Print the structure of the elements
    void printStructure(const Node* node, std::ostream& out) const;
};

#include "..\Source\AVLTree.imp"

#endif