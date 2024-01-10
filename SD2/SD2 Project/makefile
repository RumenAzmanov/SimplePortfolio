all: TestStructuresAndCreateResults

MainTask:
	g++ Source/main.cpp -o .\Executables\SD2
	.\Executables\SD2.exe

Catch2Tests:
	g++ Source/Catch2TestsAVLTree.cpp -o .\Executables\Catch2TestsAVLTree
	g++ Source/Catch2TestsSkipList.cpp -o .\Executables\Catch2TestsSkipList
	.\Executables\Catch2TestsAVLTree.exe
	.\Executables\Catch2TestsSkipList.exe

TestStructures:
	g++ Source/AVLTreeTests.cpp -O3 -o .\Executables\AVLTreeTests
	g++ Source/SkipListTests.cpp -O3 -o .\Executables\SkipListTests
	.\Executables\AVLTreeTests.exe
	.\Executables\SkipListTests.exe

TestStructuresAndCreateResults:
	g++ Source/AVLTreeTests.cpp -O3 -o .\Executables\AVLTreeTests
	g++ Source/SkipListTests.cpp -O3 -o .\Executables\SkipListTests
	.\Executables\AVLTreeTests.exe >> Result.csv
	.\Executables\SkipListTests.exe >> Result.csv