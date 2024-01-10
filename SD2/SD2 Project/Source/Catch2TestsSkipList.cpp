#define CATCH_CONFIG_MAIN
#include <sstream>

#include "../Headers/SkipList.hpp"
#include "../catch2/catch.hpp"

TEST_CASE("Skip-list testing") {
    SECTION("Create and get tree data") {
        SkipList<int> mySkipList;
        REQUIRE(mySkipList.getListLength() == 0);
    }

    SECTION("Insert") {
        SkipList<int> mySkipList;
        mySkipList.insert(1);
        mySkipList.insert(2);
        mySkipList.insert(3);

        REQUIRE(mySkipList.getListLength() == 3);
    }

    SECTION("Find") {
        SkipList<int> mySkipList;
        for (int i = 0; i < 10; ++i) {
            mySkipList.insert(i);
        }

        REQUIRE(mySkipList.getListLength() == 10);
        REQUIRE(mySkipList.find(0) == 1);
        REQUIRE(mySkipList.find(1) == 1);
        REQUIRE(mySkipList.find(2) == 1);
        REQUIRE(mySkipList.find(3) == 1);
        REQUIRE(mySkipList.find(4) == 1);
        REQUIRE(mySkipList.find(5) == 1);
        REQUIRE(mySkipList.find(6) == 1);
        REQUIRE(mySkipList.find(7) == 1);
        REQUIRE(mySkipList.find(8) == 1);
        REQUIRE(mySkipList.find(9) == 1);
    }

    SECTION("Insert identical values") {
        SkipList<int> mySkipList;
        mySkipList.insert(1);
        mySkipList.insert(1);
        mySkipList.insert(1);
        mySkipList.insert(1);
        mySkipList.insert(1);

        REQUIRE(mySkipList.getListLength() == 5);
        REQUIRE(mySkipList.find(1) == 1);
    }

    SECTION("Find non-existent values") {
        SkipList<int> mySkipList;
        for (int i = 0; i < 10; ++i) {
            mySkipList.insert(i);
        }

        REQUIRE(mySkipList.getListLength() == 10);
        REQUIRE(mySkipList.find(-1) == 0);
        REQUIRE(mySkipList.find(10) == 0);
    }

    SECTION("Erase first element") {
        SkipList<int> mySkipList;
        for (int i = 0; i < 10; ++i) {
            mySkipList.insert(i);
        }

        REQUIRE(mySkipList.erase(0));

        REQUIRE(mySkipList.getListLength() == 9);
        REQUIRE(mySkipList.find(0) == 0);

        REQUIRE(mySkipList.erase(1));

        REQUIRE(mySkipList.getListLength() == 8);
        REQUIRE(mySkipList.find(1) == 0);
    }

    SECTION("Erase last element") {
        SkipList<int> mySkipList;
        for (int i = 0; i < 10; ++i) {
            mySkipList.insert(i);
        }

        REQUIRE(mySkipList.erase(9));

        REQUIRE(mySkipList.getListLength() == 9);
        REQUIRE(mySkipList.find(9) == 0);

        REQUIRE(mySkipList.erase(8));

        REQUIRE(mySkipList.getListLength() == 8);
        REQUIRE(mySkipList.find(8) == 0);
    }

    SECTION("Erase middle elements") {
        SkipList<int> mySkipList;
        for (int i = 0; i < 10; ++i) {
            mySkipList.insert(i);
        }

        REQUIRE(mySkipList.erase(4));

        REQUIRE(mySkipList.getListLength() == 9);
        REQUIRE(mySkipList.find(4) == 0);

        REQUIRE(mySkipList.erase(6));

        REQUIRE(mySkipList.getListLength() == 8);
        REQUIRE(mySkipList.find(6) == 0);

        REQUIRE(mySkipList.erase(5));

        REQUIRE(mySkipList.getListLength() == 7);
        REQUIRE(mySkipList.find(5) == 0);
    }

    SECTION("Erase non-existent element") {
        SkipList<int> mySkipList;
        for (int i = 0; i < 10; ++i) {
            mySkipList.insert(i);
        }

        REQUIRE(mySkipList.find(-1) == 0);
        REQUIRE(mySkipList.find(10) == 0);

        REQUIRE(mySkipList.erase(-1) == false);
        REQUIRE(mySkipList.erase(10) == false);

        REQUIRE(mySkipList.getListLength() == 10);
        REQUIRE(mySkipList.find(-1) == 0);
        REQUIRE(mySkipList.find(10) == 0);
    }

    SECTION("Clear") {
        SkipList<int> mySkipList;
        SkipList<int> mySkipList0;
        for (int i = 0; i < 10; ++i) {
            mySkipList.insert(i);
        }

        mySkipList.clear();
        mySkipList0.clear();

        REQUIRE(mySkipList.getListLength() == 0);
        REQUIRE(mySkipList0.getListLength() == 0);
    }

    SECTION("Copy") {
        SkipList<int> mySkipList1;
        for (int i = 0; i < 5; ++i) {
            mySkipList1.insert(i);
        }

        SkipList<int> mySkipList2(mySkipList1);
        SkipList<int> mySkipList3;
        mySkipList3 = mySkipList2;

        mySkipList1.clear();

        REQUIRE(mySkipList2.getListLength() == 5);
        REQUIRE(mySkipList2.find(0) == 1);
        REQUIRE(mySkipList2.find(1) == 1);
        REQUIRE(mySkipList2.find(2) == 1);
        REQUIRE(mySkipList2.find(3) == 1);
        REQUIRE(mySkipList2.find(4) == 1);

        REQUIRE(mySkipList3.getListLength() == 5);
        REQUIRE(mySkipList3.find(0) == 1);
        REQUIRE(mySkipList3.find(1) == 1);
        REQUIRE(mySkipList3.find(2) == 1);
        REQUIRE(mySkipList3.find(3) == 1);
        REQUIRE(mySkipList3.find(4) == 1);
    }

    SECTION("Empty") {
        SkipList<int> mySkipList;
        SkipList<int> mySkipList0;
        for (int i = 0; i < 10; ++i) {
            mySkipList.insert(i);
        }

        REQUIRE(mySkipList.empty() == false);

        mySkipList.clear();

        REQUIRE(mySkipList.empty() == true);
        REQUIRE(mySkipList0.empty() == true);
    }

    SECTION("Print") {
        std::stringstream listInorder;
        std::stringstream listStructure;
        std::stringstream outputInorder;
        std::stringstream outputStructure;
        INFO("The only probability that will create the same structure");
        SkipList<int> mySkipList(1, 1);
        for (int i = 0; i < 5; ++i) {
            mySkipList.insert(i);
        }

        outputInorder << "0 0 1 2 3 4 \n";
        outputStructure << "0 ~ 1\n 0 -> 0\n 1 -> 0\n0 ~ 1\n 0 -> 1\n 1 -> 1\n1 ~ 1\n 0 -> 2\n 1 -> 2\n2 ~ 1\n 0 -> 3\n 1 -> 3\n3 ~ 1\n 0 -> 4\n 1 -> 4\n4 ~ 1\n 0 -> NUL\n 1 -> NUL\n";

        mySkipList.printInorder(listInorder);
        REQUIRE(listInorder.str() == outputInorder.str());
        mySkipList.printStructure(listStructure);
        REQUIRE(listStructure.str() == outputStructure.str());
    }

    SECTION("Example with type char") {
        SkipList<int> mySkipList;
        char arrayInput[] = {"Hello world!"};
        int size = sizeof(arrayInput) / sizeof(arrayInput[0]);

        for (int i = 0; i < size; ++i) {
            mySkipList.insert(arrayInput[i]);
        }

        REQUIRE(mySkipList.getListLength() == 13);
    }

    SECTION("Memory") {
        INFO("The only probability that will create the same structure");
        SkipList<int> mySkipList(1, 1);
        SkipList<int> mySkipList0(3, 2);
        for (int i = 0; i < 10; ++i) {
            mySkipList.insert(i);
        }

        INFO("Pointer to head and int*, variables int, int, long long int and head node data (sizeof(struct Node) + 4 * sizeof(struct Node*))");
        INFO("8 + 8 + 4 + 4 + 8 + 16 + 4 * 8");
        // 8 + 8 + 4 + 4 + 8 + 16 + 4 * 8
        REQUIRE(mySkipList0.checkMemory() == 80);

        INFO("List data (8 + 8 + 4 + 4 + 8) + first node (16 + 2 * 8) + 10 nodes with 1 additional pointer (10 * (16 + 8 + 8))");
        REQUIRE(mySkipList.checkMemory() == 384);
    }
}