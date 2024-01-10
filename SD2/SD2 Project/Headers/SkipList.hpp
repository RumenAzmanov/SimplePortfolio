#ifndef SKIPLIST_HPP
#define SKIPLIST_HPP

#include <time.h>

#include <cstdlib>
#include <iostream>

template <class T>
class SkipList {
   private:
    //! List node
    struct Node {
       public:
        T value;           //!< Node data
        int level;         //!< Number of pointers
        Node** nextNodes;  //!< Next nodes

        Node(const T inputValue,
             const int inputLevel = 0)
            : value(inputValue),
              nextNodes(nullptr),
              level(inputLevel) {
            try {
                nextNodes = new Node*[inputLevel + 1];
            } catch (const std::exception& exc) {
                delete nextNodes;
                nextNodes = nullptr;
                throw exc;
            }
            for (int i = 0; i < inputLevel + 1; ++i)
                nextNodes[i] = nullptr;
        }

        ~Node() {
            delete[] nextNodes;
        }
    };

    Node* head;                //!< First node
    int maxLevel;              //!< Max number of additional pointers
    int levelProbability;      //!< Probability for additional pointers in (1/x)% format
    long long int listLength;  //!< Number of elements in the list
    int* countSizes;           //!< Array, that counts the number of nodes of a certain level

   public:
    SkipList(const int maxLevelInput = 3,
             const int levelProbabilityInput = 2);
    SkipList(const SkipList& otherList);
    SkipList& operator=(const SkipList& otherList);
    ~SkipList();

    //! Insert a value in the structure
    void insert(const T value);
    //! Find if a value is in the structure
    bool find(const T value) const;
    //! Remove a value from the structure
    bool erase(T value);
    //! Clear all data from the structure - wrapper method
    void clear();

    //! Get the number of elements in the list
    long long int getListLength() const;
    //! Check if there are any elements in the list
    bool empty() const;
    //! Print all elements in sorted order
    void printInorder(std::ostream& out) const;
    //! Print the structure of the elements
    void printStructure(std::ostream& out) const;
    //! Calculate the required memory for the structure
    long long int checkMemory() const;

   private:
    //! Clear the entire sublist recursively
    void clear(Node*& node);
    //! Copy the entire sublist recursively
    void copy(Node* copyNode);

    //! Generate a random level number with the required
    //! probability
    int generateRandomLevel() const;
};

#include "..\Source\SkipList.imp"

#endif