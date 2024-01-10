all: MainTask TaskTests

MainTask:
	g++ Source/main.cpp Source/command.cpp Source/utilityhelper.cpp Source/avltree.cpp Source/table.cpp -o SDPDatabase
	.\SDPDatabase.exe

TaskTests:
	g++ Source/command.cpp Source/utilityhelper.cpp Source/avltree.cpp Source/table.cpp Source/test.cpp -o SDPDatabaseTests
	.\SDPDatabaseTests.exe
