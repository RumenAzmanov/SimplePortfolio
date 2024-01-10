#include <iostream>
#include <array>
#include <vector>
#include <assert.h>
#include <thread>
#include <barrier>
#include <numeric>
#include <cstdlib>
#include <stdexcept>

#if __linux__ != 0
#include <time.h>

static uint64_t timer_nsec() {
#if defined(CLOCK_MONOTONIC_RAW)
	const clockid_t clockid = CLOCK_MONOTONIC_RAW;

#else
	const clockid_t clockid = CLOCK_MONOTONIC;

#endif

	timespec t;
	clock_gettime(clockid, &t);

	return t.tv_sec * 1000000000UL + t.tv_nsec;
}

#elif _WIN64 != 0
#define NOMINMAX
#include <Windows.h>

static struct TimerBase {
	LARGE_INTEGER freq;
	TimerBase() {
		QueryPerformanceFrequency(&freq);
	}
} timerBase;

// the order of global initialisaitons is non-deterministic, do
// not use this routine in the ctors of globally-scoped objects
static uint64_t timer_nsec() {
	LARGE_INTEGER t;
	QueryPerformanceCounter(&t);

	return 1000000000ULL * t.QuadPart / timerBase.freq.QuadPart;
}

#elif __APPLE__ != 0
#include <mach/mach_time.h>

static struct TimerBase {
	mach_timebase_info_data_t tb;
	TimerBase() {
		mach_timebase_info(&tb);
	}
} timerBase;

// the order of global initialisaitons is non-deterministic, do
// not use this routine in the ctors of globally-scoped objects
static uint64_t timer_nsec() {
	const uint64_t t = mach_absolute_time();
	return t * timerBase.tb.numer / timerBase.tb.denom;
}

#endif

////////////////////
// Interfaces
////////////////////

struct MemoryBlock {
	MemoryBlock() {}
	MemoryBlock(void* ptr, size_t len) : ptr(ptr), len(len) {}
	inline operator bool() const
	{
		return len && ptr;
	}
	void* ptr = nullptr;
	size_t len = 0;
};

class AllocatorBase {
public:
	inline MemoryBlock alloc(size_t size) {
		return MemoryBlock();
	}
	inline void free(MemoryBlock& block) {}

	size_t getMaxUsedMemory() const {
		return maxUsedMemory;
	}
protected:
	size_t usedMemory = 0;
	size_t maxUsedMemory = 0;

	void* malloc(size_t size) {
		usedMemory += size;
		if (usedMemory > maxUsedMemory) {
			maxUsedMemory = usedMemory;
		}
		return std::malloc(size);
	}

	void free(void* ptr, size_t size) {
		usedMemory -= size;
		return std::free(ptr);
	}
};


class DefaultAllocator : public AllocatorBase {
public:
	MemoryBlock alloc(size_t size) {
		return MemoryBlock(AllocatorBase::malloc(size), size);
	}

	void free(MemoryBlock& block) {
		AllocatorBase::free(block.ptr, block.len);
	}
};

////////////////////
// Implementation
////////////////////

#define ListSize(X) ((size_t*)X)[0]
#define ListNextList(X) ((void**)X)[1]
#define ListFirstFreeBlock(X) ((void**)X)[2]
#define BlockSize(Y) ((size_t*)Y)[0]
#define BlockNext(Y) ((void**)Y)[1]

class MyAllocator : public AllocatorBase {
	static thread_local void* memLists; // Pointer to first list of lists (linked list of elements called "lists")
	static thread_local void* lastList; // Pointer to last list

	const int startSize = 2048; // Starting size of memory lists
	int lastSize = 2048; // Block increase size
	const int divider = 40000000; // Largest size for single memory list

	/// Allocate a new empty list and its first element
	void* allocNewList(size_t listSize) {
		void* newList = AllocatorBase::malloc(listSize + 3 * sizeof(void*));
		ListSize(newList) = listSize;
		ListNextList(newList) = nullptr;
		ListFirstFreeBlock(newList) = &((void**)newList)[3];
		((size_t*)newList)[3] = listSize - 2 * sizeof(void*); // Initialize first free block size
		((void**)newList)[4] = nullptr; // Initialize first free block pointer to next
		return newList;
	}

	/// Free allocated list using pointer to the previous
	void freeWithPrevious(void* listPtr) {
		void* newNext = nullptr;
		if (listPtr == nullptr) { // Remove the first list
			newNext = memLists;
			memLists = ListNextList(memLists);
		} else { // Detach a list from the chain
			newNext = ListNextList(listPtr);
			ListNextList(listPtr) = ListNextList(newNext);
		}
		AllocatorBase::free(newNext, ListSize(newNext));
	}

	/// Mark a piece of a list as taken and return its address to the user as block
	void markBlockAsTaken(void*& indexList, void*& bestMatch, void*& bestPrev, const size_t size) {
		if (bestMatch != nullptr) { // If a suitable place is found
			assert(BlockSize(bestMatch) >= size);
			if (BlockSize(bestMatch) == size) { // If it is a perfect match
				if (bestMatch == ListFirstFreeBlock(indexList)) { // If it is the first free block in the list
					ListFirstFreeBlock(indexList) = BlockNext(bestMatch);
				} else {
					BlockNext(bestPrev) = BlockNext(bestMatch);
				}
			} else { // Normal match with new block
				void* newEmptyBlock = static_cast<char*>(bestMatch) + sizeof(void*) * 2 + size;
				BlockSize(newEmptyBlock) = BlockSize(bestMatch) - size - sizeof(void*) * 2;
				BlockNext(newEmptyBlock) = BlockNext(bestMatch);
				if (bestMatch == ListFirstFreeBlock(indexList)) {
					ListFirstFreeBlock(indexList) = newEmptyBlock;
				} else {
					BlockNext(bestPrev) = newEmptyBlock;
				}
			}
			BlockSize(bestMatch) = size;
			BlockNext(bestMatch) = nullptr;
		}
	}

	/// Search through all the lists and find the first suitable space for a new block
	void findPlace(void*& indexList, void*& bestMatch, void*& bestPrev, const size_t size) {
		void* listsIter = memLists;
		if (indexList != nullptr) listsIter = indexList;
		void* memFreeIter = nullptr;
		void* memFreeIterPrev = nullptr;
		bool perfect = false;
		while (listsIter != nullptr) {
			// Impossible to fit
			if (size > ListSize(listsIter)) {
				listsIter = ListNextList(listsIter);
				continue;
			}
			memFreeIterPrev = memFreeIter;
			memFreeIter = ListFirstFreeBlock(listsIter);
			while (memFreeIter != nullptr) {
				// Perfect match, exact the same size
				if (BlockSize(memFreeIter) == size) {
					bestMatch = memFreeIter;
					bestPrev = memFreeIterPrev;
					indexList = listsIter;
					return;
				}
				// Better match
				if (BlockSize(memFreeIter) > size + 2 * sizeof(void*) &&
					(bestMatch == nullptr ||
						(bestMatch != nullptr && BlockSize(memFreeIter) < BlockSize(bestMatch)))) {
					bestMatch = memFreeIter;
					bestPrev = memFreeIterPrev;
					indexList = listsIter;
				}
				// Move next
				memFreeIterPrev = memFreeIter;
				memFreeIter = BlockNext(memFreeIter);
			}
			if (bestMatch != nullptr) break;
			listsIter = ListNextList(listsIter);
		}
	}

	/// Return block and merge with the next/previous
	void returnMatchMerge(void*& indexList, void*& ptrToReturn, void*& ptrIterFree, const size_t size) {
		if (ptrIterFree >= ptrToReturn || ptrIterFree == nullptr) { // Return block to front of list
			BlockNext(ptrToReturn) = ptrIterFree;
			ptrIterFree = ptrToReturn;
			ListFirstFreeBlock(indexList) = ptrToReturn;
		} else { // Return in the middle of the list
			while (ptrIterFree != nullptr &&
				BlockNext(ptrIterFree) != nullptr &&
				BlockNext(ptrIterFree) < (void**)ptrToReturn) {
				ptrIterFree = BlockNext(ptrIterFree);
			} // Find where exactly to return
			BlockNext(ptrToReturn) = BlockNext(ptrIterFree);
			BlockNext(ptrIterFree) = ptrToReturn;
		}
		// Return block and merge with the next free block
		if (static_cast<char*>(ptrToReturn) + 2 * sizeof(void*) + size == static_cast<char*>(BlockNext(ptrToReturn))) {
			void* next = BlockNext(ptrToReturn);
			BlockNext(ptrToReturn) = BlockNext(next);
			BlockSize(ptrToReturn) = BlockSize(ptrToReturn) + 2 * sizeof(void*) + BlockSize(next);
		}
		// Return block and merge with the previous free block
		if (static_cast<char*>(ptrIterFree) + 2 * sizeof(void*) + BlockSize(ptrIterFree) == static_cast<char*>(BlockNext(ptrIterFree))) {
			void* next = BlockNext(ptrIterFree);
			BlockNext(ptrIterFree) = BlockNext(next);
			BlockSize(ptrIterFree) = BlockSize(ptrIterFree) + 2 * sizeof(void*) + BlockSize(next);
		}
	}

public:
	// Constructor
	MyAllocator() {
		memLists = nullptr;
		lastList = nullptr;
		lastSize = startSize;
	}

	/// Allocate memory
	MemoryBlock alloc(size_t size) {
		if (memLists == nullptr) { // No memory lists allocated
			memLists = allocNewList(startSize);
			lastList = memLists;
			lastSize = startSize;
		}

		void* indexList = nullptr;
		void* bestMatch = nullptr;
		void* bestPrev = nullptr;
		if (size >= divider) {
			ListNextList(lastList) = allocNewList(size + 2 * sizeof(void*));
			lastList = ListNextList(lastList);
		}

		findPlace(indexList, bestMatch, bestPrev, size);

		if (bestMatch == nullptr) { // We need a new block of memory
			do {
				if (lastSize <= divider) lastSize *= 2;
				else break;
			} while (((size_t)lastSize) < size + 2 * sizeof(void*));
			ListNextList(lastList) = allocNewList(lastSize);
			lastList = ListNextList(lastList);
			indexList = lastList;
			findPlace(indexList, bestMatch, bestPrev, size);
		}

		markBlockAsTaken(indexList, bestMatch, bestPrev, size);

		void* memory = static_cast<char*>(bestMatch) + 2 * sizeof(void*);
		return MemoryBlock(memory, size);
	}

	/// Free allocated memory
	void free(MemoryBlock& block) {
		void* ptrToReturn = static_cast<char*>(block.ptr) - sizeof(void*) * 2;
		void* ptrIterFree = nullptr;
		void* indexList = memLists;
		void* indexPrevList = nullptr;
		while (indexList != nullptr) { // Iterate through all lists
			// If the block is from this list
			if (indexList <= ptrToReturn &&
				static_cast<char*>(ptrToReturn) < static_cast<char*>(indexList) + ListSize(indexList)) {
				ptrIterFree = ((void**)indexList)[2];
				break;
			}
			indexPrevList = indexList;
			indexList = ((void**)indexList)[1];
		}

		returnMatchMerge(indexList, ptrToReturn, ptrIterFree, block.len);

		// Clean whole list
		if (((size_t*)((void**)indexList)[2])[0] == ListSize(indexList) - 2 * sizeof(void*) &&
			((void**)((void**)indexList)[2])[1] == nullptr) {
			if (indexList == lastList) lastList = indexPrevList;
			freeWithPrevious(indexPrevList);
		}
	}
};

thread_local void* MyAllocator::memLists = nullptr;
thread_local void* MyAllocator::lastList = nullptr;

void myTest01() {
	MyAllocator myAlloc;
	MemoryBlock memoBlock1 = myAlloc.alloc(16);
	MemoryBlock memoBlock2 = myAlloc.alloc(8);
	myAlloc.free(memoBlock1);
	myAlloc.free(memoBlock2);
}

void myTest02() {
	MyAllocator myAlloc;
	MemoryBlock memoBlock1 = myAlloc.alloc(8);
	MemoryBlock memoBlock2 = myAlloc.alloc(8);
	myAlloc.free(memoBlock1);
	MemoryBlock memoBlock3 = myAlloc.alloc(16);
	myAlloc.free(memoBlock2);
	myAlloc.free(memoBlock3);
}

void myTest03() {
	MyAllocator myAlloc;
	MemoryBlock memoBlock1 = myAlloc.alloc(8);
	MemoryBlock memoBlock2 = myAlloc.alloc(24);
	MemoryBlock memoBlock3 = myAlloc.alloc(8);
	MemoryBlock memoBlock4 = myAlloc.alloc(24);
	MemoryBlock memoBlock5 = myAlloc.alloc(8);
	myAlloc.free(memoBlock1);
	myAlloc.free(memoBlock2);
	myAlloc.free(memoBlock3);
	myAlloc.free(memoBlock4);
	MemoryBlock memoBlock6 = myAlloc.alloc(24);
	myAlloc.free(memoBlock6);
	myAlloc.free(memoBlock5);
}

void myTest04() {
	MyAllocator myAlloc;
	MemoryBlock memoBlock2 = myAlloc.alloc(1024 * 4096);
	myAlloc.free(memoBlock2);
}

////////////////////
// Tests
////////////////////

template <typename Allocator>
struct Test1 {
	static void test(Allocator& allocator, size_t testSize = 1000) {
		std::vector<MemoryBlock> results;
		results.reserve(testSize);

		for (size_t i = 0; i < testSize; ++i) {
			results.push_back(allocator.alloc(128 * (1 + i % 4)));
		}

		for (auto& block : results) {
			allocator.free(block);
		}
	}
};

template <typename Allocator>
struct Test2 {
	static void test(Allocator& allocator, size_t testSize = 1000) {
		std::vector<MemoryBlock> results;
		results.reserve(testSize * 1.1f);

		for (size_t i = 0; i < testSize; ++i) {
			results.push_back(allocator.alloc(i % 2 ? 32 : 64));

			if (i % 10 == 0) {
				results.push_back(allocator.alloc(4096 * 1024));
			}
		}
		for (auto& block : results) {
			allocator.free(block);
		}
	}
};

template <typename Allocator>
struct Test3 {
	static void test(Allocator& allocator, size_t testSize = 1000) {
		std::vector<MemoryBlock> results;
		results.reserve(testSize);

		for (size_t i = 0; i < testSize; ++i) {
			results.push_back(allocator.alloc(i % 2 ? 32 : 2000));

			// for every 10 blocks free first 5 of them
			if (i % 10 == 4) {
				for (int j = 0; j < 5; ++j) {
					allocator.free(results[i - j]);
					results[i - j].ptr = nullptr;
				}
			}
		}
		for (auto& block : results) {
			if (block) {
				allocator.free(block);
			}
		}
	}
};

template <typename Allocator>
struct Test4 {
	static void test(Allocator& allocator, size_t testSize = 1000) {
		std::vector<MemoryBlock> results;
		results.reserve(testSize * 1.1f);

		for (size_t i = 0; i < testSize; ++i) {
			results.push_back(allocator.alloc(i % 2 ? 32 : 64));

			if (i % 10 == 0) {
				results.push_back(allocator.alloc(4096 * 1024));
			}
			// for every 10 blocks free first 5 of them
			if (i % 10 == 4) {
				for (int j = 0; j < 5; ++j) {
					allocator.free(results[i - j]);
					results[i - j].ptr = nullptr;
				}
			}
		}
		for (auto& block : results) {
			if (block) {
				allocator.free(block);
			}
		}
	}
};

constexpr int threadsCount = 4;

template <template <typename> typename Tester, typename Allocator>
void executeSingleThreadTest(Allocator& allocator) {
	DefaultAllocator defAllocator;

	auto time1 = timer_nsec();
	Tester<DefaultAllocator>::test(defAllocator);
	auto time2 = timer_nsec();
	const auto defTime = time2 - time1;

	time1 = timer_nsec();
	Tester<Allocator>::test(allocator);
	time2 = timer_nsec();
	const auto yoursTime = time2 - time1;

	std::cout << "Time: " << defTime << "/" << yoursTime << "(" << double(defTime) / double(yoursTime) << ")";
	std::cout << " max mem: " << defAllocator.getMaxUsedMemory() << "/" << allocator.getMaxUsedMemory();
	std::cout << "(" << double(defAllocator.getMaxUsedMemory()) / double(allocator.getMaxUsedMemory()) << ")" << std::endl;
}


template <template <typename> typename Tester, typename Allocator>
uint64_t executeMultiThreadSingle(Allocator& allocator) {
	std::array<uint64_t, threadsCount> results = { 0,0,0,0 };
	std::barrier startingBarrier(threadsCount);
	std::thread threads[threadsCount];

	auto testDefault = [&startingBarrier, &allocator, &results](int index) {
		auto time1 = timer_nsec();
		Tester<Allocator>::test(allocator);
		auto time2 = timer_nsec();
		results[index] = time2 - time1;
	};
	for (int i = 0; i < threadsCount; i++) {
		threads[i] = std::thread(testDefault, i);
	}
	for (int i = 0; i < threadsCount; i++) {
		threads[i].join();
	}

	return std::accumulate(std::begin(results), std::end(results), 0u);
}


template <template <typename> typename Tester, typename Allocator>
void executeMultiThreadTest(Allocator& allocator) {
	assert(std::thread::hardware_concurrency() >= threadsCount);

	DefaultAllocator defAllocator;

	const auto defTime = executeMultiThreadSingle<Tester, DefaultAllocator>(defAllocator);
	const auto yoursTime = executeMultiThreadSingle<Tester, Allocator>(allocator);

	std::cout << "Time: " << defTime << "/" << yoursTime << "(" << double(defTime) / double(yoursTime) << ")";
	std::cout << " max mem: " << defAllocator.getMaxUsedMemory() << "/" << allocator.getMaxUsedMemory();
	std::cout << "(" << double(defAllocator.getMaxUsedMemory()) / double(allocator.getMaxUsedMemory()) << ")" << std::endl;
}

int main() {
	{
		MyAllocator custom;
		executeSingleThreadTest<Test1, MyAllocator>(custom);
	}

	{
		MyAllocator custom;
		executeSingleThreadTest<Test2, MyAllocator>(custom);
	}

	{
		MyAllocator custom;
		executeSingleThreadTest<Test3, MyAllocator>(custom);
	}

	{
		MyAllocator custom;
		executeSingleThreadTest<Test4, MyAllocator>(custom);
	}



	{
		MyAllocator custom;
		executeMultiThreadTest<Test1, MyAllocator>(custom);
	}

	{
		MyAllocator custom;
		executeMultiThreadTest<Test2, MyAllocator>(custom);
	}

	{
		MyAllocator custom;
		executeMultiThreadTest<Test3, MyAllocator>(custom);
	}

	{
		MyAllocator custom;
		executeMultiThreadTest<Test4, MyAllocator>(custom);
	}


	std::cout << "==========" << std::endl;
	int pool[4][2] = { {128, 200}, {256, 200}, {384, 200}, {512, 200} };
	int size = 4;


	myTest01();
	myTest02();
	myTest03();
	myTest04();
	std::cout << "==========" << std::endl;
	std::cout << "----------" << std::endl;
	for (int i = 0; i < 10; ++i) {
		MyAllocator custom;
		executeSingleThreadTest<Test1, MyAllocator>(custom);
	}
	std::cout << "----------" << std::endl;
	for (int i = 0; i < 10; ++i) {
		MyAllocator custom;
		executeSingleThreadTest<Test2, MyAllocator>(custom);
	}
	std::cout << "----------" << std::endl;
	for (int i = 0; i < 10; ++i) {
		MyAllocator custom;
		executeSingleThreadTest<Test3, MyAllocator>(custom);
	}
	std::cout << "----------" << std::endl;
	for (int i = 0; i < 10; ++i) {
		MyAllocator custom;
		executeSingleThreadTest<Test4, MyAllocator>(custom);
	}
	std::cout << "==========" << std::endl;
	std::cout << "----------" << std::endl;
	for (int i = 0; i < 10; ++i) {
		MyAllocator custom;
		executeMultiThreadTest<Test1, MyAllocator>(custom);
	}
	std::cout << "----------" << std::endl;
	for (int i = 0; i < 10; ++i) {
		MyAllocator custom;
		executeMultiThreadTest<Test2, MyAllocator>(custom);
	}
	std::cout << "----------" << std::endl;
	for (int i = 0; i < 10; ++i) {
		MyAllocator custom;
		executeMultiThreadTest<Test3, MyAllocator>(custom);
	}
	std::cout << "----------" << std::endl;
	for (int i = 0; i < 10; ++i) {
		MyAllocator custom;
		executeMultiThreadTest<Test4, MyAllocator>(custom);
	}
	return 0;
}