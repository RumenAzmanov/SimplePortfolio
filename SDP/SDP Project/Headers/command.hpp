#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "database.hpp"
#include "utilityhelper.hpp"

const unsigned int SIZE_OPERATION = 12;  //!< Size of operation keyword

//! Оperations with corresponding integers
enum class OperationType {
    UNDEFINED = 0,
    QUIT = 1,
    HELP = 2,

    OPEN = 3,
    CLOSE = 4,

    CREATETABLE = 5,
    DROPTABLE = 6,
    LISTTABLES = 7,
    TABLEINFO = 8,

    INSERT = 9,
    REMOVE = 10,
    SELECT = 11,

    COUNTOPERATIONS  // Number of operations
};

//! Operations with corresponding strings
const char OperationList[(int)(OperationType::COUNTOPERATIONS)][SIZE_OPERATION] = {
    "UNDEFINED",
    "QUIT",
    "HELP",
    "OPEN",
    "CLOSE",
    "CREATETABLE",
    "DROPTABLE",
    "LISTTABLES",
    "TABLEINFO",
    "INSERT",
    "REMOVE",
    "SELECT"};

class CommandLine {
   private:
    bool commandExit = false;      //!< Quit the program flag
    std::string commandLineInput;  //!< The input command
    Database database;             //!< The database data
    bool loading = false;

   public:
    CommandLine() {}
    ~CommandLine() { database.~Database(); }

    //! Main commandline interface cycle
    void CommandLineInterface();

    //! Console input command
    void CommandLineInput();

    //! Direct console input
    void CommandLineDirectInput(const char* commandLineDirectInput);

    //! Command analysis and execution
    void CommandLineExecute();

    //! Splash screen
    void splashScreen();

    //! Help information
    void helpInfo() const;

    //! Get table by its name
    Table* getTableByName(std::string tableName);

    //! Create table in the database command
    void createTable();

    //! Remove table from the database command
    void dropTable();

    //! Display information about the tables in the database command
    void listTables();

    //! Display information about a single table command
    void tableInfo();

    //! Insert record/s in a table command
    void insertRecord();

    //! Select records from the table command
    void selectRecords();

    //! Dizplay database records
    void printDatabase(Table* selectedTable, std::list<Record*> selectedRecords, std::vector<std::string> selectedColumns);

    //! Store information directly on the disk as a file command
    void closeInfile();

    //! Load information in the computer memory command
    void loadFromFile();

    //! Remove selected records from the database command
    void removeRecords();
};

#endif