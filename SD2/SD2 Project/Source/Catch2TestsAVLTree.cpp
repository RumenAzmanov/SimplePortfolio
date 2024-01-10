#define CATCH_CONFIG_MAIN
#include <sstream>

#include "../Headers/AVLTree.hpp"
#include "../catch2/catch.hpp"

TEST_CASE("AVL tree testing") {
    SECTION("Create and get tree data") {
        AVLTree<int> myAVLTree;
        REQUIRE(myAVLTree.getTreeWeight() == 0);
        REQUIRE(myAVLTree.getTreeHeight() == 0);
    }

    SECTION("Insert") {
        AVLTree<int> myAVLTree;
        myAVLTree.insert(1);
        myAVLTree.insert(2);
        myAVLTree.insert(3);

        REQUIRE(myAVLTree.getTreeWeight() == 3);
        REQUIRE(myAVLTree.getTreeHeight() == 2);
    }

    SECTION("Find") {
        AVLTree<int> myAVLTree;
        for (int i = 0; i < 10; ++i) {
            myAVLTree.insert(i);
        }

        //      3
        //     / \
        //    /   \
        //   1     7
        //  / \   / \
        // 0   2 5   8
        //      / \   \
        //     4   6   9

        REQUIRE(myAVLTree.getTreeWeight() == 10);
        REQUIRE(myAVLTree.getTreeHeight() == 4);
        REQUIRE(myAVLTree.find(0) == 1);
        REQUIRE(myAVLTree.find(1) == 1);
        REQUIRE(myAVLTree.find(2) == 1);
        REQUIRE(myAVLTree.find(3) == 1);
        REQUIRE(myAVLTree.find(4) == 1);
        REQUIRE(myAVLTree.find(5) == 1);
        REQUIRE(myAVLTree.find(6) == 1);
        REQUIRE(myAVLTree.find(7) == 1);
        REQUIRE(myAVLTree.find(8) == 1);
        REQUIRE(myAVLTree.find(9) == 1);
    }

    SECTION("Insert identical values") {
        AVLTree<int> myAVLTree;
        myAVLTree.insert(1);
        myAVLTree.insert(1);
        myAVLTree.insert(1);
        myAVLTree.insert(1);
        myAVLTree.insert(1);

        REQUIRE(myAVLTree.getTreeWeight() == 5);
        REQUIRE(myAVLTree.getTreeHeight() == 3);
        REQUIRE(myAVLTree.find(1) == 1);
    }

    SECTION("Find non-existent values") {
        AVLTree<int> myAVLTree;
        for (int i = 0; i < 10; ++i) {
            myAVLTree.insert(i);
        }

        REQUIRE(myAVLTree.getTreeWeight() == 10);
        REQUIRE(myAVLTree.getTreeHeight() == 4);
        REQUIRE(myAVLTree.find(-1) == 0);
        REQUIRE(myAVLTree.find(10) == 0);
    }

    SECTION("Erase node with 2 children") {
        AVLTree<int> myAVLTree;
        for (int i = 0; i < 10; ++i) {
            myAVLTree.insert(i);
        }

        REQUIRE(myAVLTree.erase(5));

        REQUIRE(myAVLTree.getTreeWeight() == 9);
        REQUIRE(myAVLTree.getTreeHeight() == 4);
        REQUIRE(myAVLTree.find(5) == 0);
        REQUIRE(myAVLTree.find(1) == 1);

        REQUIRE(myAVLTree.erase(1));

        REQUIRE(myAVLTree.getTreeWeight() == 8);
        REQUIRE(myAVLTree.getTreeHeight() == 4);
        REQUIRE(myAVLTree.find(5) == 0);
        REQUIRE(myAVLTree.find(1) == 0);
    }

    SECTION("Erase node with 1 child") {
        AVLTree<int> myAVLTree;
        for (int i = 0; i < 10; ++i) {
            myAVLTree.insert(i);
        }

        REQUIRE(myAVLTree.erase(8));

        REQUIRE(myAVLTree.getTreeWeight() == 9);
        REQUIRE(myAVLTree.getTreeHeight() == 4);
        REQUIRE(myAVLTree.find(8) == 0);
    }

    SECTION("Erase node with no children") {
        AVLTree<int> myAVLTree;
        for (int i = 0; i < 10; ++i) {
            myAVLTree.insert(i);
        }

        REQUIRE(myAVLTree.erase(0));

        REQUIRE(myAVLTree.getTreeWeight() == 9);
        REQUIRE(myAVLTree.getTreeHeight() == 4);
        REQUIRE(myAVLTree.find(0) == 0);
        REQUIRE(myAVLTree.find(9) == 1);

        REQUIRE(myAVLTree.erase(9));

        REQUIRE(myAVLTree.getTreeWeight() == 8);
        REQUIRE(myAVLTree.getTreeHeight() == 4);
        REQUIRE(myAVLTree.find(0) == 0);
        REQUIRE(myAVLTree.find(9) == 0);
    }

    SECTION("Erase non-existent node") {
        AVLTree<int> myAVLTree;
        for (int i = 0; i < 10; ++i) {
            myAVLTree.insert(i);
        }

        REQUIRE(myAVLTree.find(-1) == 0);
        REQUIRE(myAVLTree.find(10) == 0);

        REQUIRE(myAVLTree.erase(-1) == false);
        REQUIRE(myAVLTree.erase(10) == false);

        REQUIRE(myAVLTree.getTreeWeight() == 10);
        REQUIRE(myAVLTree.getTreeHeight() == 4);
        REQUIRE(myAVLTree.find(-1) == 0);
        REQUIRE(myAVLTree.find(10) == 0);
    }

    SECTION("Clear") {
        AVLTree<int> myAVLTree;
        AVLTree<int> myAVLTree0;
        for (int i = 0; i < 10; ++i) {
            myAVLTree.insert(i);
        }

        REQUIRE(myAVLTree.getTreeWeight() == 10);
        REQUIRE(myAVLTree.getTreeHeight() == 4);

        REQUIRE(myAVLTree0.getTreeWeight() == 0);
        REQUIRE(myAVLTree0.getTreeHeight() == 0);

        myAVLTree.clear();
        myAVLTree0.clear();

        REQUIRE(myAVLTree.getTreeWeight() == 0);
        REQUIRE(myAVLTree.getTreeHeight() == 0);

        REQUIRE(myAVLTree0.getTreeWeight() == 0);
        REQUIRE(myAVLTree0.getTreeHeight() == 0);
    }

    SECTION("Copy") {
        AVLTree<int> myAVLTree1;
        for (int i = 0; i < 5; ++i) {
            myAVLTree1.insert(i);
        }

        AVLTree<int> myAVLTree2(myAVLTree1);
        AVLTree<int> myAVLTree3;
        myAVLTree3 = myAVLTree1;

        myAVLTree1.clear();

        REQUIRE(myAVLTree2.getTreeWeight() == 5);
        REQUIRE(myAVLTree2.getTreeHeight() == 3);
        REQUIRE(myAVLTree2.find(0) == 1);
        REQUIRE(myAVLTree2.find(1) == 1);
        REQUIRE(myAVLTree2.find(2) == 1);
        REQUIRE(myAVLTree2.find(3) == 1);
        REQUIRE(myAVLTree2.find(4) == 1);

        REQUIRE(myAVLTree3.getTreeWeight() == 5);
        REQUIRE(myAVLTree3.getTreeHeight() == 3);
        REQUIRE(myAVLTree3.find(0) == 1);
        REQUIRE(myAVLTree3.find(1) == 1);
        REQUIRE(myAVLTree3.find(2) == 1);
        REQUIRE(myAVLTree3.find(3) == 1);
        REQUIRE(myAVLTree3.find(4) == 1);
    }

    SECTION("Empty") {
        AVLTree<int> myAVLTree;
        AVLTree<int> myAVLTree0;
        for (int i = 0; i < 10; ++i) {
            myAVLTree.insert(i);
        }

        REQUIRE(myAVLTree.empty() == false);

        myAVLTree.clear();

        REQUIRE(myAVLTree.empty() == true);
        REQUIRE(myAVLTree0.empty() == true);
    }

    SECTION("Print") {
        std::stringstream treeInorder;
        std::stringstream treeStructure;
        std::stringstream outputInorder;
        std::stringstream outputStructure;
        AVLTree<int> myAVLTree;
        for (int i = 0; i < 10; ++i) {
            myAVLTree.insert(i);
        }

        outputInorder << "0 1 2 3 4 5 6 7 8 9 \n";
        outputStructure << "(3)\n(1,7)\n(0,2)(5,8)\n(,)(,)(4,6)(,9)\n(,)(,)(,)\n";

        myAVLTree.printInorder(treeInorder);
        REQUIRE(treeInorder.str() == outputInorder.str());
        myAVLTree.printStructure(treeStructure);
        REQUIRE(treeStructure.str() == outputStructure.str());
    }

    SECTION("Example with type char") {
        AVLTree<char> myAVLTree;
        char arrayInput[] = {"Hello world!"};
        int size = sizeof(arrayInput) / sizeof(arrayInput[0]);

        for (int i = 0; i < size; ++i) {
            myAVLTree.insert(arrayInput[i]);
        }

        REQUIRE(myAVLTree.getTreeWeight() == 13);
        REQUIRE(myAVLTree.getTreeHeight() == 4);
    }

    SECTION("Memory") {
        AVLTree<int> myAVLTree;
        AVLTree<int> myAVLTree0;
        for (int i = 0; i < 10; ++i) {
            myAVLTree.insert(i);
        }

        INFO("Pointer to root and variable int(size of tree)");
        REQUIRE(myAVLTree0.checkMemory() == 16);

        INFO("16 + 10 * sizeof(struct Node) = 16 + 10 * 24 = 256");
        REQUIRE(myAVLTree.checkMemory() == 256);
    }

    SECTION("Perform right rotation") {
        AVLTree<int> myAVLTree;
        std::stringstream treeInorder;
        std::stringstream treeStructure;
        std::stringstream outputInorder;
        std::stringstream outputStructure;
        myAVLTree.insert(5);
        myAVLTree.insert(4);
        myAVLTree.insert(3);

        REQUIRE(myAVLTree.getTreeWeight() == 3);
        REQUIRE(myAVLTree.getTreeHeight() == 2);

        outputInorder << "3 4 5 \n";
        outputStructure << "(4)\n(3,5)\n(,)(,)\n";

        myAVLTree.printInorder(treeInorder);
        REQUIRE(treeInorder.str() == outputInorder.str());
        myAVLTree.printStructure(treeStructure);
        REQUIRE(treeStructure.str() == outputStructure.str());
    }

    SECTION("Perform left and right rotation") {
        AVLTree<int> myAVLTree;
        std::stringstream treeInorder;
        std::stringstream treeStructure;
        std::stringstream outputInorder;
        std::stringstream outputStructure;
        myAVLTree.insert(5);
        myAVLTree.insert(3);
        myAVLTree.insert(4);

        REQUIRE(myAVLTree.getTreeWeight() == 3);
        REQUIRE(myAVLTree.getTreeHeight() == 2);

        outputInorder << "3 4 5 \n";
        outputStructure << "(4)\n(3,5)\n(,)(,)\n";

        myAVLTree.printInorder(treeInorder);
        REQUIRE(treeInorder.str() == outputInorder.str());
        myAVLTree.printStructure(treeStructure);
        REQUIRE(treeStructure.str() == outputStructure.str());
    }

    SECTION("Perform right and left rotation") {
        AVLTree<int> myAVLTree;
        std::stringstream treeInorder;
        std::stringstream treeStructure;
        std::stringstream outputInorder;
        std::stringstream outputStructure;
        myAVLTree.insert(5);
        myAVLTree.insert(7);
        myAVLTree.insert(6);

        REQUIRE(myAVLTree.getTreeWeight() == 3);
        REQUIRE(myAVLTree.getTreeHeight() == 2);

        outputInorder << "5 6 7 \n";
        outputStructure << "(6)\n(5,7)\n(,)(,)\n";

        myAVLTree.printInorder(treeInorder);
        REQUIRE(treeInorder.str() == outputInorder.str());
        myAVLTree.printStructure(treeStructure);
        REQUIRE(treeStructure.str() == outputStructure.str());
    }

    SECTION("Perform left rotation") {
        AVLTree<int> myAVLTree;
        std::stringstream treeInorder;
        std::stringstream treeStructure;
        std::stringstream outputInorder;
        std::stringstream outputStructure;
        myAVLTree.insert(5);
        myAVLTree.insert(6);
        myAVLTree.insert(7);

        REQUIRE(myAVLTree.getTreeWeight() == 3);
        REQUIRE(myAVLTree.getTreeHeight() == 2);

        outputInorder << "5 6 7 \n";
        outputStructure << "(6)\n(5,7)\n(,)(,)\n";

        myAVLTree.printInorder(treeInorder);
        REQUIRE(treeInorder.str() == outputInorder.str());
        myAVLTree.printStructure(treeStructure);
        REQUIRE(treeStructure.str() == outputStructure.str());
    }
}