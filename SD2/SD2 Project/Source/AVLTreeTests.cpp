/**
 * AVL tree data structure testing
 */

#include <chrono>
#include <iostream>

#include "../Headers/AVLTree.hpp"

//! Test: Input unique sorted integer values
void inputUniqueSortedIntegerValues(int numberOfElements) {
    std::cout << "Input " << numberOfElements << " unique sorted integer values, ";
    AVLTree<int> object;

    //! Experiment
    auto start = std::chrono::steady_clock::now();  //!< Timer start

    for (int i = 0; i < numberOfElements; ++i) {
        object.insert(i);
    }

    auto end = std::chrono::steady_clock::now();  //!< Timer end

    //! Result
    std::chrono::duration<long long, std::nano> timer = end - start;
    std::cout << timer.count() << '\n';
}

//! Test: Input unique reverse sorted integer values
void inputUniqueReverseSortedIntegerValues(int numberOfElements) {
    std::cout << "Input " << numberOfElements << " unique reverse sorted integer values, ";
    AVLTree<int> object;

    //! Experiment
    auto start = std::chrono::steady_clock::now();  //!< Timer start

    for (int i = numberOfElements; i > 0; --i) {
        object.insert(i);
    }

    auto end = std::chrono::steady_clock::now();  //!< Timer end

    //! Result
    std::chrono::duration<long long, std::nano> timer = end - start;
    std::cout << timer.count() << '\n';
}

//! Test: Input unique integer values
void inputUniqueIntegerValues(int numberOfElements) {
    std::cout << "Input " << numberOfElements << " unique integer values, ";
    AVLTree<int> object;
    int* input = new (std::nothrow) int[numberOfElements];
    if (!input) return;
    bool* appear = new (std::nothrow) bool[numberOfElements];
    if (!appear) return;
    int last = 0, value = 0;
    for (int i = 0; i < numberOfElements; ++i) {
        appear[i] = false;
    }

    srand(time(0));
    for (int i = 0; i < numberOfElements; ++i) {
        value = rand() % numberOfElements;
        while (appear[value % numberOfElements] != 0) {
            value = (value + 1) % numberOfElements;
        }
        appear[value] = true;
        input[last++] = value;
    }

    //! Experiment
    auto start = std::chrono::steady_clock::now();  //!< Timer start

    for (int i = 0; i < numberOfElements; ++i) {
        object.insert(input[i]);
    }

    auto end = std::chrono::steady_clock::now();  //!< Timer end

    delete[] input;
    input = nullptr;
    delete[] appear;
    appear = nullptr;

    //! Result
    std::chrono::duration<long long, std::nano> timer = end - start;
    std::cout << timer.count() << '\n';
}

//! Test: Input integer values
void inputIntegerValues(int numberOfElements) {
    std::cout << "Input " << numberOfElements << " integer values, ";
    AVLTree<int> object;
    int* input = new (std::nothrow) int[numberOfElements];
    if (!input) return;
    int last = 0, value = 0;

    srand(time(0));
    for (int i = 0; i < numberOfElements; ++i) {
        value = rand() % numberOfElements;
        input[last++] = value;
    }

    //! Experiment
    auto start = std::chrono::steady_clock::now();  //!< Timer start

    for (int i = 0; i < numberOfElements; ++i) {
        object.insert(input[i]);
    }

    auto end = std::chrono::steady_clock::now();  //!< Timer end

    delete[] input;
    input = nullptr;

    //! Result
    std::chrono::duration<long long, std::nano> timer = end - start;
    std::cout << timer.count() << '\n';
}

//! Test: Input integer values
void inputRepeatingIntegerValues(int numberOfElements) {
    std::cout << "Input " << numberOfElements << " repeating integer values, ";
    AVLTree<int> object;
    int* input = new (std::nothrow) int[numberOfElements];
    if (!input) return;
    int last = 0, value = 0;

    srand(time(0));
    for (int i = 0; i < numberOfElements; ++i) {
        value = rand() % numberOfElements;
        input[last++] = value % 10;
    }

    //! Experiment
    auto start = std::chrono::steady_clock::now();  //!< Timer start

    for (int i = 0; i < numberOfElements; ++i) {
        object.insert(input[i]);
    }

    auto end = std::chrono::steady_clock::now();  //!< Timer end

    delete[] input;
    input = nullptr;

    //! Result
    std::chrono::duration<long long, std::nano> timer = end - start;
    std::cout << timer.count() << '\n';
}

//! Test: Find random integer values
void findRandomIntegerValues(int numberOfElements) {
    std::cout << "Find " << numberOfElements << " random integer values, ";
    AVLTree<int> object;
    int* input = new (std::nothrow) int[numberOfElements];
    if (!input) return;
    int last = 0;

    srand(time(0));

    for (int i = 0; i < numberOfElements; ++i) {
        object.insert(rand() % numberOfElements);
        input[last++] = (rand() % numberOfElements);
    }

    //! Experiment
    auto start = std::chrono::steady_clock::now();  //!< Timer start

    for (int i = 0; i < numberOfElements; ++i) {
        object.find(input[i]);
    }

    auto end = std::chrono::steady_clock::now();  //!< Timer end

    delete[] input;
    input = nullptr;

    //! Result
    std::chrono::duration<long long, std::nano> timer = end - start;
    std::cout << timer.count() << '\n';
}

//! Test: Find random existing integer values
void findRandomExistingIntegerValues(int numberOfElements) {
    std::cout << "Find " << numberOfElements << " random existing integer values, ";
    AVLTree<int> object;
    int* input = new (std::nothrow) int[numberOfElements];
    if (!input) return;
    int last = 0, value;

    srand(time(0));
    for (int i = 0; i < numberOfElements; ++i) {
        value = rand() % numberOfElements;
        object.insert(value);
        input[last++] = value;
    }

    //! Experiment
    auto start = std::chrono::steady_clock::now();  //!< Timer start

    for (int i = 0; i < numberOfElements; ++i) {
        object.find(input[i]);
    }

    auto end = std::chrono::steady_clock::now();  //!< Timer end

    delete[] input;
    input = nullptr;

    //! Result
    std::chrono::duration<long long, std::nano> timer = end - start;
    std::cout << timer.count() << '\n';
}

//! Test: Find random non-existing integer values
void findRandomNonExistingIntegerValues(int numberOfElements) {
    std::cout << "Find " << numberOfElements << " random non-existing integer values, ";
    AVLTree<int> object;
    int* input = new (std::nothrow) int[numberOfElements];
    if (!input) return;
    int last = 0, value;

    srand(time(0));
    for (int i = 0; i < numberOfElements; ++i) {
        value = rand() % numberOfElements;
        value += value % 2;
        object.insert(value);
    }
    for (int i = 0; i < numberOfElements; ++i) {
        value = rand() % numberOfElements;
        value += 1 - (value % 2);
        input[last++] = value;
    }

    //! Experiment
    auto start = std::chrono::steady_clock::now();  //!< Timer start

    for (int i = 0; i < numberOfElements; ++i) {
        object.find(input[i]);
    }

    auto end = std::chrono::steady_clock::now();  //!< Timer end

    delete[] input;
    input = nullptr;

    //! Result
    std::chrono::duration<long long, std::nano> timer = end - start;
    std::cout << timer.count() << '\n';
}

//! Test: Erase random integer values
void eraseRandomIntegerValues(int numberOfElements) {
    std::cout << "Erase " << numberOfElements << " random integer values, ";
    AVLTree<int> object;
    int* input = new (std::nothrow) int[numberOfElements];
    if (!input) return;
    int last = 0;

    srand(time(0));

    for (int i = 0; i < numberOfElements; ++i) {
        object.insert(rand() % numberOfElements);
        input[last++] = (rand() % numberOfElements);
    }

    //! Experiment
    auto start = std::chrono::steady_clock::now();  //!< Timer start

    for (int i = 0; i < numberOfElements; ++i) {
        object.erase(input[i]);
    }

    auto end = std::chrono::steady_clock::now();  //!< Timer end

    delete[] input;
    input = nullptr;

    //! Result
    std::chrono::duration<long long, std::nano> timer = end - start;
    std::cout << timer.count() << '\n';
}

//! Test: Erase random existing integer values
void eraseRandomExistingIntegerValues(int numberOfElements) {
    std::cout << "Erase " << numberOfElements << " random existing integer values, ";
    AVLTree<int> object;
    int* input = new (std::nothrow) int[numberOfElements];
    if (!input) return;
    int last = 0, value;

    srand(time(0));
    for (int i = 0; i < numberOfElements; ++i) {
        value = rand() % numberOfElements;
        object.insert(value);
        input[last++] = value;
    }

    //! Experiment
    auto start = std::chrono::steady_clock::now();  //!< Timer start

    for (int i = 0; i < numberOfElements; ++i) {
        object.erase(input[i]);
    }

    auto end = std::chrono::steady_clock::now();  //!< Timer end

    delete[] input;
    input = nullptr;

    //! Result
    std::chrono::duration<long long, std::nano> timer = end - start;
    std::cout << timer.count() << '\n';
}

//! Test: Erase random non-existing integer values
void eraseRandomNonExistingIntegerValues(int numberOfElements) {
    std::cout << "Erase " << numberOfElements << " random non-existing integer values, ";
    AVLTree<int> object;
    int* input = new (std::nothrow) int[numberOfElements];
    if (!input) return;
    int last = 0, value;

    srand(time(0));
    for (int i = 0; i < numberOfElements; ++i) {
        value = rand() % numberOfElements;
        value += value % 2;
        object.insert(value);
    }
    for (int i = 0; i < numberOfElements; ++i) {
        value = rand() % numberOfElements;
        value += 1 - (value % 2);
        input[last++] = value;
    }

    //! Experiment
    auto start = std::chrono::steady_clock::now();  //!< Timer start

    for (int i = 0; i < numberOfElements; ++i) {
        object.erase(input[i]);
    }

    auto end = std::chrono::steady_clock::now();  //!< Timer end

    delete[] input;
    input = nullptr;

    //! Result
    std::chrono::duration<long long, std::nano> timer = end - start;
    std::cout << timer.count() << '\n';
}

//! Test: Clear structure with random values
void clearStructure(int numberOfElements) {
    std::cout << "Clear structure with " << numberOfElements << " random values, ";
    AVLTree<int> object;
    int value;

    srand(time(0));
    for (int i = 0; i < numberOfElements; ++i) {
        value = rand() % numberOfElements;
        object.insert(value);
    }

    //! Experiment
    auto start = std::chrono::steady_clock::now();  //!< Timer start

    object.clear();

    auto end = std::chrono::steady_clock::now();  //!< Timer end

    //! Result
    std::chrono::duration<long long, std::nano> timer = end - start;
    std::cout << timer.count() << '\n';
}

//! Test: Copy structure with random values
void copyStructure(int numberOfElements) {
    std::cout << "Copy structure with " << numberOfElements << " random values, ";
    AVLTree<int> object1;
    AVLTree<int> object2;
    int value;

    srand(time(0));
    for (int i = 0; i < numberOfElements; ++i) {
        value = rand() % numberOfElements;
        object1.insert(value);
    }

    //! Experiment
    auto start = std::chrono::steady_clock::now();  //!< Timer start

    object2 = object1;

    auto end = std::chrono::steady_clock::now();  //!< Timer end

    //! Result
    std::chrono::duration<long long, std::nano> timer = end - start;
    std::cout << timer.count() << '\n';
}

//! Test: Memory for random integer values
void memoryForRandomIntegerValues(int numberOfElements) {
    std::cout << "Memory for " << numberOfElements << " random integer values, ";
    AVLTree<int> object;

    //! Experiment
    for (int i = 0; i < numberOfElements; ++i) {
        object.insert(rand() % numberOfElements);
    }

    //! Result
    std::cout << object.checkMemory() << '\n';
}

int main(int argc, char* argv[]) {
    std::cout << "AVLTree\n";
    std::cout << "Test, Time in nanoseconds\n";
    inputUniqueSortedIntegerValues(300);
    inputUniqueReverseSortedIntegerValues(300);
    inputUniqueIntegerValues(300);
    inputIntegerValues(300);
    inputRepeatingIntegerValues(300);
    findRandomIntegerValues(300);
    findRandomExistingIntegerValues(300);
    findRandomNonExistingIntegerValues(300);
    eraseRandomIntegerValues(300);
    eraseRandomExistingIntegerValues(300);
    eraseRandomNonExistingIntegerValues(300);
    clearStructure(300);
    copyStructure(300);

    inputUniqueSortedIntegerValues(30000);
    inputUniqueReverseSortedIntegerValues(30000);
    inputUniqueIntegerValues(30000);
    inputIntegerValues(30000);
    inputRepeatingIntegerValues(30000);
    findRandomIntegerValues(30000);
    findRandomExistingIntegerValues(30000);
    findRandomNonExistingIntegerValues(30000);
    eraseRandomIntegerValues(30000);
    eraseRandomExistingIntegerValues(30000);
    eraseRandomNonExistingIntegerValues(30000);
    clearStructure(30000);
    copyStructure(30000);

    std::cout << "Test, Memory in bytes\n";
    memoryForRandomIntegerValues(300);
    memoryForRandomIntegerValues(30000);

    return 0;
}
