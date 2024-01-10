#include <immintrin.h>
#include <smmintrin.h>

#include <cassert>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

// Basic functions -----------------------------------------
template <typename T>
void swapInt(T& a, T& b) {
	T c = a;
	a = b;
	b = c;
}

template <typename T>
void comparator(T& a, T& b) {
	if (a > b) {
		T c = a;
		a = b;
		b = c;
	}
}

template <typename T>
void printArray(std::vector<T>& dataVector, const int size) {
	for (int i = 0; i < size; ++i) {
		std::cout << dataVector[i] << '\n';
	}
}

template <typename T>
bool isSorted(std::vector<T>& dataVector, const int size) {
	for (int i = 0; i < size - 1; ++i) {
		if (dataVector[i] > dataVector[i + 1]) return false;
	}
	return true;
}

// Odd-even adaptive sort ----------------------------------
template <typename T>
void oddEvenSort(std::vector<T>& dataVector, int size) {
	bool sorted = false;
	while (!sorted) {
		sorted = true;
		for (int i = 1; i < size - 1; i += 2) {
			if (dataVector[i] > dataVector[i + 1]) {
				swapInt(dataVector[i], dataVector[i + 1]);
				sorted = false;
			}
		}
		for (int i = 0; i < size - 1; i += 2) {
			if (dataVector[i] > dataVector[i + 1]) {
				swapInt(dataVector[i], dataVector[i + 1]);
				sorted = false;
			}
		}
	}
}

// Odd-even merge sort -------------------------------------
template <typename T>
void oddEvenMerge(std::vector<T>& dataVector, int begin, int size, int step) {
	int power = step * 2;
	if (power < size) {
		oddEvenMerge(dataVector, begin, size, power);
		oddEvenMerge(dataVector, begin + step, size, power);
		for (int i = begin + step; i + step < begin + size; i += power)
			comparator(dataVector[i], dataVector[i + step]);
	} else {
		comparator(dataVector[begin], dataVector[begin + step]);
	}
}

template <typename T>
void oddEvenMergeSort(std::vector<T>& dataVector, int begin, int size, int threads) {
	int half = size / 2;
	if (threads >= 2) {
		if (size > 1) {
			std::thread thread1(oddEvenMergeSort<T>, std::ref(dataVector), begin, half, threads / 2 - 1);
			std::thread thread2(oddEvenMergeSort<T>, std::ref(dataVector), begin + half, half, threads / 2 - 1);
			thread1.join();
			thread2.join();
			oddEvenMerge(dataVector, begin, size, 1);
		}
	} else {
		if (size > 1) {
			oddEvenMergeSort(dataVector, begin, half, 0);
			oddEvenMergeSort(dataVector, begin + half, half, 0);
			oddEvenMerge(dataVector, begin, size, 1);
		}
	}
}

// Odd-even multithread sort -------------------------------
template <typename T>
void oddEvenParallelIteration(std::vector<T>& dataVector,
	int size, int begin, int sortSize, int phases[], int threadID, int numberOfThreads) {
	int end = begin + sortSize - 1;
	int len = size / 2 + 1;
	for (int step = 0; step < len; ++step) {
		for (int i = begin; i < end; i += 2) {
			comparator(dataVector[i], dataVector[i + 1]);
		}
		++phases[threadID];
		for (int i = begin + 1; i < end; i += 2) {
			comparator(dataVector[i], dataVector[i + 1]);
		}
		if (threadID < numberOfThreads - 1) {
			while (phases[threadID + 1] < phases[threadID]) {
				std::this_thread::yield();
			}
			comparator(dataVector[end], dataVector[end + 1]);
		}
		if (threadID > 0) {
			while (phases[threadID - 1] <= phases[threadID]) {
				std::this_thread::yield();
			}
		}
		++phases[threadID];
	}
}

template <typename T>
void oddEvenParallelSort(std::vector<T>& dataVector, const int size, int numberOfThreads) {
	if (numberOfThreads > 64) return;
	int phases[64] = { 0 };
	std::thread threads[64];
	int split = size / numberOfThreads;
	if (split % 2 != 0) ++split;
	for (int i = 0; i < numberOfThreads; ++i) {
		if (i != numberOfThreads - 1) {
			threads[i] = std::thread(oddEvenParallelIteration<T>,
				std::ref(dataVector), size, (i * split), split, phases, i, numberOfThreads);
		} else {
			threads[i] = std::thread(oddEvenParallelIteration<T>,
				std::ref(dataVector), size, (i * split), (size - i * split), phases, i, numberOfThreads);
		}
	}
	for (int i = 0; i < numberOfThreads; ++i) threads[i].join();
}

// Odd-even multithread sort -------------------------------
template <typename T>
void oddEvenParallelIterationJump(std::vector<T>& dataVector, int size, int begin, int numberOfThreads) {
	for (int i = begin; i < size - 1; i += numberOfThreads * 2) {
		comparator(dataVector[i], dataVector[i + 1]);
	}
}

template <typename T>
void oddEvenParallelSortJump(std::vector<T>& dataVector, const int size, int numberOfThreads) {
	if (numberOfThreads > 64) return;
	int phases[64] = { 0 };
	std::thread threads[64];
	for (int j = 0; j < size / 2; ++j) {
		for (int i = 0; i < numberOfThreads; ++i) {
			threads[i] = std::thread(oddEvenParallelIterationJump<T>, std::ref(dataVector), size, (i * 2), numberOfThreads);
		}
		for (int i = 0; i < numberOfThreads; ++i) threads[i].join();
		for (int i = 0; i < numberOfThreads; ++i) {
			threads[i] = std::thread(oddEvenParallelIterationJump<T>, std::ref(dataVector), size, (i * 2 + 1), numberOfThreads);
		}
		for (int i = 0; i < numberOfThreads; ++i) threads[i].join();
	}
}

// Odd-even SIMD sort --------------------------------------
void oddEvenSIMDSort (std::vector<int>& dataVector, int size) {
	int i = 0;
	for (int step = 0; step < size / 2 + 2; ++step) {
		for (int j = 0, i = 0; j < 2; ++j) {
			for (i = j; i + 4 <= size; i += 4) {
				// Standart compare and swap
				__m128i data = _mm_load_si128((__m128i*) & dataVector[i]);
				__m128i shuffle = _mm_shuffle_epi32(data, _MM_SHUFFLE(2, 3, 0, 1));
				__m128i compare = _mm_cmplt_epi32(data, shuffle);
				compare = _mm_xor_si128(compare, _mm_set_epi32(-1, 0, -1, 0));
				data = _mm_blendv_epi8(shuffle, data, compare);
				_mm_store_si128((__m128i*) & dataVector[i], data);
			}
			for (; i < size - 1; ++i) comparator(dataVector[i], dataVector[i + 1]);
		}
	}
}

void testing() {
	int dataSizes[] = { 1024, 2048, 4096, 16384, 65536/*, 262144*/ };
	int dataSizesSize = sizeof(dataSizes) / sizeof(int);
	int numThreads[] = { 1, 2, 4, 8, 16, 32 };
	int numThreadsSize = sizeof(numThreads) / sizeof(int);

	int size = 262144;
	srand(82176);
	std::chrono::steady_clock::time_point begin;
	std::chrono::steady_clock::time_point end;
	std::chrono::duration<long long, std::nano> time;

	std::cout << "Testing for type: INT = " << sizeof(int) << std::endl;
	std::vector<int> dataVectorINT(size);
	for (int i = 0; i < size; ++i) dataVectorINT[i] = rand();
	for (int i_size = 0; i_size < dataSizesSize; ++i_size) {
		std::cout << "Testing for size of data: " << dataSizes[i_size] << std::endl;
		std::cout << "Testing for block partition" << std::endl;
		for (int i_threads = 0; i_threads < numThreadsSize; ++i_threads) {
			std::cout << "Testing for number of threads: " << numThreads[i_threads] << std::endl;
			for (int i = 0; i < 3; ++i) {
				std::vector<int> testDataVector(dataVectorINT);
				begin = std::chrono::steady_clock::now();
				oddEvenParallelSort(testDataVector, dataSizes[i_size], numThreads[i_threads]);
				end = std::chrono::steady_clock::now();
				time = end - begin;
				std::cout << "\t" << "# (" << (isSorted(testDataVector, dataSizes[i_size]) ? "V" : "X") << ") " << time.count() << std::endl;
			}
		}
		//std::cout << "Testing for index partition" << std::endl;
		//for (int i_threads = 0; i_threads < numThreadsSize; ++i_threads) {
		//	std::cout << "Testing for number of threads: " << numThreads[i_threads] << std::endl;
		//	for (int i = 0; i < 3; ++i) {
		//		std::vector<int> testDataVector(dataVectorINT);
		//		begin = std::chrono::steady_clock::now();
		//		oddEvenParallelSortJump(testDataVector, dataSizes[i_size], numThreads[i_threads]);
		//		end = std::chrono::steady_clock::now();
		//		time = end - begin;
		//		std::cout << "\t" << "# (" << (isSorted(testDataVector, dataSizes[i_size]) ? "V" : "X") << ") " << time.count() << std::endl;
		//	}
		//}
		std::cout << "Testing for parallel merge" << std::endl;
		for (int i_threads = 0; i_threads < numThreadsSize; ++i_threads) {
			std::cout << "Testing for number of threads: " << numThreads[i_threads] << std::endl;
			for (int i = 0; i < 3; ++i) {
				std::vector<int> testDataVector(dataVectorINT);
				begin = std::chrono::steady_clock::now();
				oddEvenMergeSort(testDataVector, 0, dataSizes[i_size], numThreads[i_threads]);
				end = std::chrono::steady_clock::now();
				time = end - begin;
				std::cout << "\t" << "# (" << (isSorted(testDataVector, dataSizes[i_size]) ? "V" : "X") << ") " << time.count() << std::endl;
			}
		}
		std::cout << "Testing for adaptive algorithm: " << std::endl;
		for (int i = 0; i < 3; ++i) {
			std::vector<int> testDataVector(dataVectorINT);
			begin = std::chrono::steady_clock::now();
			oddEvenSort(testDataVector, dataSizes[i_size]);
			end = std::chrono::steady_clock::now();
			time = end - begin;
			std::cout << "\t" << "# (" << (isSorted(testDataVector, dataSizes[i_size]) ? "V" : "X") << ") " << time.count() << std::endl;
		}
		std::cout << "Testing for SIMD operations: " << std::endl;
		for (int i = 0; i < 3; ++i) {
			std::vector<int> testDataVector(dataVectorINT);
			begin = std::chrono::steady_clock::now();
			oddEvenSIMDSort(testDataVector, dataSizes[i_size]);
			end = std::chrono::steady_clock::now();
			time = end - begin;
			std::cout << "\t" << "# (" << (isSorted(testDataVector, dataSizes[i_size]) ? "V" : "X") << ") " << time.count() << std::endl;
		}
	}

	std::cout << "Testing for type: CHAR = " << sizeof(char) << std::endl;
	std::vector<char> dataVectorCHAR(size);
	for (int i = 0; i < size; ++i) dataVectorCHAR[i] = '!' + (rand() % ('Z' - '!'));
	for (int i_size = 0; i_size < dataSizesSize; ++i_size) {
		std::cout << "Testing for size of data: " << dataSizes[i_size] << std::endl;
		std::cout << "Testing for block partition" << std::endl;
		for (int i_threads = 0; i_threads < numThreadsSize; ++i_threads) {
			std::cout << "Testing for number of threads: " << numThreads[i_threads] << std::endl;
			for (int i = 0; i < 3; ++i) {
				std::vector<char> testDataVector(dataVectorCHAR);
				begin = std::chrono::steady_clock::now();
				oddEvenParallelSort(testDataVector, dataSizes[i_size], numThreads[i_threads]);
				end = std::chrono::steady_clock::now();
				time = end - begin;
				std::cout << "\t" << "# (" << (isSorted(testDataVector, dataSizes[i_size]) ? "V" : "X") << ") " << time.count() << std::endl;
			}
		}
		//std::cout << "Testing for index partition" << std::endl;
		//for (int i_threads = 0; i_threads < numThreadsSize; ++i_threads) {
		//	std::cout << "Testing for number of threads: " << numThreads[i_threads] << std::endl;
		//	for (int i = 0; i < 3; ++i) {
		//		std::vector<char> testDataVector(dataVectorCHAR);
		//		begin = std::chrono::steady_clock::now();
		//		oddEvenParallelSortJump(testDataVector, dataSizes[i_size], numThreads[i_threads]);
		//		end = std::chrono::steady_clock::now();
		//		time = end - begin;
		//		std::cout << "\t" << "# (" << (isSorted(testDataVector, dataSizes[i_size]) ? "V" : "X") << ") " << time.count() << std::endl;
		//	}
		//}
		std::cout << "Testing for parallel merge" << std::endl;
		for (int i_threads = 0; i_threads < numThreadsSize; ++i_threads) {
			std::cout << "Testing for number of threads: " << numThreads[i_threads] << std::endl;
			for (int i = 0; i < 3; ++i) {
				std::vector<char> testDataVector(dataVectorCHAR);
				begin = std::chrono::steady_clock::now();
				oddEvenMergeSort(testDataVector, 0, dataSizes[i_size], numThreads[i_threads]);
				end = std::chrono::steady_clock::now();
				time = end - begin;
				std::cout << "\t" << "# (" << (isSorted(testDataVector, dataSizes[i_size]) ? "V" : "X") << ") " << time.count() << std::endl;
			}
		}
		std::cout << "Testing for adaptive algorithm: " << std::endl;
		for (int i = 0; i < 3; ++i) {
			std::vector<char> testDataVector(dataVectorCHAR);
			begin = std::chrono::steady_clock::now();
			oddEvenSort(testDataVector, dataSizes[i_size]);
			end = std::chrono::steady_clock::now();
			time = end - begin;
			std::cout << "\t" << "# (" << (isSorted(testDataVector, dataSizes[i_size]) ? "V" : "X") << ") " << time.count() << std::endl;
		}
	}

	std::cout << "Testing for type: DOUBLE = " << sizeof(double) << std::endl;
	std::vector<double> dataVectorDOUBLE(size);
	for (int i = 0; i < size; ++i) dataVectorDOUBLE[i] = ((double)(rand()) / (double)(rand()));
	for (int i_size = 0; i_size < dataSizesSize; ++i_size) {
		std::cout << "Testing for size of data: " << dataSizes[i_size] << std::endl;
		std::cout << "Testing for block partition" << std::endl;
		for (int i_threads = 0; i_threads < numThreadsSize; ++i_threads) {
			std::cout << "Testing for number of threads: " << numThreads[i_threads] << std::endl;
			for (int i = 0; i < 3; ++i) {
				std::vector<double> testDataVector(dataVectorDOUBLE);
				begin = std::chrono::steady_clock::now();
				oddEvenParallelSort(testDataVector, dataSizes[i_size], numThreads[i_threads]);
				end = std::chrono::steady_clock::now();
				time = end - begin;
				std::cout << "\t" << "# (" << (isSorted(testDataVector, dataSizes[i_size]) ? "V" : "X") << ") " << time.count() << std::endl;
			}
		}
		//std::cout << "Testing for index partition" << std::endl;
		//for (int i_threads = 0; i_threads < numThreadsSize; ++i_threads) {
		//	std::cout << "Testing for number of threads: " << numThreads[i_threads] << std::endl;
		//	for (int i = 0; i < 3; ++i) {
		//		std::vector<double> testDataVector(dataVectorDOUBLE);
		//		begin = std::chrono::steady_clock::now();
		//		oddEvenParallelSortJump(testDataVector, dataSizes[i_size], numThreads[i_threads]);
		//		end = std::chrono::steady_clock::now();
		//		time = end - begin;
		//		std::cout << "\t" << "# (" << (isSorted(testDataVector, dataSizes[i_size]) ? "V" : "X") << ") " << time.count() << std::endl;
		//	}
		//}
		std::cout << "Testing for parallel merge" << std::endl;
		for (int i_threads = 0; i_threads < numThreadsSize; ++i_threads) {
			std::cout << "Testing for number of threads: " << numThreads[i_threads] << std::endl;
			for (int i = 0; i < 3; ++i) {
				std::vector<double> testDataVector(dataVectorDOUBLE);
				begin = std::chrono::steady_clock::now();
				oddEvenMergeSort(testDataVector, 0, dataSizes[i_size], numThreads[i_threads]);
				end = std::chrono::steady_clock::now();
				time = end - begin;
				std::cout << "\t" << "# (" << (isSorted(testDataVector, dataSizes[i_size]) ? "V" : "X") << ") " << time.count() << std::endl;
			}
		}
		std::cout << "Testing for adaptive algorithm: " << std::endl;
		for (int i = 0; i < 3; ++i) {
			std::vector<double> testDataVector(dataVectorDOUBLE);
			begin = std::chrono::steady_clock::now();
			oddEvenSort(testDataVector, dataSizes[i_size]);
			end = std::chrono::steady_clock::now();
			time = end - begin;
			std::cout << "\t" << "# (" << (isSorted(testDataVector, dataSizes[i_size]) ? "V" : "X") << ") " << time.count() << std::endl;
		}
	}
}

int main() {
	testing();
	return 0;
	// 2^14 = 16384
	// 2^16 = 65536
	// 2^18 = 262144
}
