/**
 * \brief SD2
 * \author 82176 Rumen Azmanov - Computer Science
 * \details Data Structures 2 Project Homework
 * \date 06.2022 - 07.2022
 */

#include <iostream>

#include "../Headers/AVLTree.hpp"
#include "../Headers/SkipList.hpp"

int main() {
    std::cout << "Data Structures 2 Project Homework\n";
    std::cout << "To perform unit testing use \"Catch2Tests\" task in the makefile.\n";
    std::cout << "To perform the tests use \"TestStructures\" task in the makefile.\n";
    std::cout << "To perform the tests and create Results.csv file use \"TestStructuresAndCreateResults\" task in the makefile.\n";

    AVLTree<int> tree;
    SkipList<int> list(2, 2);

    tree.insert(40);
    tree.insert(20);
    tree.insert(10);
    tree.insert(30);
    tree.insert(60);
    tree.insert(50);
    tree.insert(70);

    list.insert(10);
    list.insert(20);
    list.insert(30);

    std::cout << "---------------\n";
    tree.printStructure(std ::cout);
    std::cout << "---------------\n";
    list.printStructure(std ::cout);
    std::cout << "---------------\n";

    // std::cout << "Hello world!" << '\n';
    return 0;
}