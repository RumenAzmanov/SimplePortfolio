#include "../Headers/command.hpp"

void CommandLine::CommandLineInterface() {
    splashScreen();
    while (!commandExit) {
        try {
            CommandLineInput();
        } catch (const std::invalid_argument& e) {
            std::cout << e.what() << '\n';
        } catch (const std::runtime_error& e) {
            std::cout << e.what() << '\n';
        } catch (...) {
            std::cout << "[!]Error!" << '\n';
        }
    }
}

void CommandLine::CommandLineInput() {
    commandLineInput.clear();
    char inputChar = ' ';
    bool inQuotes = false;
    std::cout << " > ";
    while (inputChar != '\n') {
        inputChar = std::cin.get();
        if (inputChar == ' ' && (commandLineInput.size() == 0 || commandLineInput.back() == ' ') && !inQuotes) {
            continue;
        }
        if (inputChar == '"') {
            inQuotes = !inQuotes;
        }
        commandLineInput.push_back(inputChar);
    }
    CommandLineExecute();
}

void CommandLine::CommandLineDirectInput(const char* commandLineDirectInput) {
    commandLineInput.clear();
    commandLineInput = commandLineDirectInput;
    commandLineInput.push_back('\n');
    CommandLineExecute();
}

void CommandLine::CommandLineExecute() {
    std::string operationName;
    int countOperations = (int)(OperationType::COUNTOPERATIONS);
    OperationType operationType = OperationType::UNDEFINED;

    for (unsigned int i = 0; !isEmptySpace(commandLineInput[i]) && i < commandLineInput.size(); ++i) {
        operationName.push_back(commandLineInput[i]);
    }

    for (unsigned short int i = 0; i < operationName.size(); ++i) {
        if (operationName[i] >= 'a' && operationName[i] <= 'z') {
            operationName[i] -= 32;
        }
        if (operationName[i] < 'A' || operationName[i] > 'Z') {
            throw std::invalid_argument("[!] Invalid operation name!");
        }
    }

    for (unsigned int i = 0; i < countOperations; ++i) {
        if (operationName == OperationList[i]) {
            operationType = (OperationType)i;
        }
    }
    if (operationType != OperationType::UNDEFINED) {
        commandLineInput.erase(commandLineInput.begin(), 1 + commandLineInput.begin() + strlen(OperationList[(int)operationType]));
    }

    switch (operationType) {
        case OperationType::QUIT:
            commandExit = true;
            break;
        case OperationType::HELP:
            helpInfo();
            break;
        case OperationType::OPEN:
            loadFromFile();
            break;
        case OperationType::CLOSE:
            closeInfile();
            break;
        case OperationType::CREATETABLE:
            createTable();
            break;
        case OperationType::DROPTABLE:
            dropTable();
            break;
        case OperationType::LISTTABLES:
            listTables();
            break;
        case OperationType::TABLEINFO:
            tableInfo();
            break;
        case OperationType::INSERT:
            insertRecord();
            break;
        case OperationType::REMOVE:
            removeRecords();
            break;
        case OperationType::SELECT:
            selectRecords();
            break;
        default:
            break;
    }
}

void CommandLine::splashScreen() {
    // SDP PROJECT
    // Database
    std::cout << " ________________________________________________ \n"
              << "|      _   _   _     _   _   _       _  _ ___    |\n"
              << "|     |_  | | |_|   |_| |_| | |   | |_ |   |     |\n"
              << "|      _| |/  |     |   | \\ |_| |_| |_ |_  |     |\n"
              << "|           _                                    |\n"
              << "|          | \\  _  |_  _  |_   _   _  _          |\n"
              << "|   ###    |  |/ \\ |  / \\ | \\ / \\ (_ /_)    ###  |\n"
              << "|          |_/ \\_/\\\\/ \\_/\\|_/ \\_/\\ _)\\_          |\n"
              << "|                                                |\n"
              << "|________________________________________________|\n";
}

void CommandLine::helpInfo() const {
    std::cout << "               # HELP INFORMATION #               \n"
              << " #  OPERATIONS\n"
              << " > QUIT\n"
              << "   Close the application\n"
              << " > CREATETABLE <name> (<fields>)\n"
              << "   Create a table with selected name and fields\n"
              << " --> INT\n"
              << "     Integer field type\n"
              << " --> STRING\n"
              << "     String field type\n"
              << " --> DATE\n"
              << "     Date field type\n"
              << " --> NOTNULL\n"
              << "     A field type that must have a value\n"
              << " --> INDEX\n"
              << "     Indexed field for faster search options\n"
              << " --> DEFAULT <value>\n"
              << "     Default value for a field\n"
              << " --> DISTINCT\n"
              << "     Definition for a record to have distinct\n"
              << "     field value\n"
              << " > DROPTABLE <name>\n"
              << "   Remove a table with selected name\n"
              << " > LISTTABLES\n"
              << "   Display a list of all tables\n"
              << " > TABLEINFO <name>\n"
              << "   Display information about a single table\n"
              << " > SELECT <fields> FROM <name>\n"
              << "   Display selected columns from a table\n"
              << " > SELECT DISTINCT <fields> FROM <name>\n"
              << "   Display selected columns from a table that have\n"
              << "   distinct value for a field\n"
              << " --> WHERE <condition>\n"
              << "     Display selected records based on the\n"
              << "     condition\n"
              << " --> ORDERBY <fields>\n"
              << "     Order selected records by field\n"
              << " > REMOVE FROM <name> WHERE <condition>\n"
              << "   Remove selected columns from a table\n"
              << " > INSERT <name> (<records>)\n"
              << "   Insert a single or multiple records in the\n"
              << "   database\n"
              << " > OPEN <name>\n"
              << "   Load a table from its file\n"
              << " > CLOSE <name>\n"
              << "   Save a table in its file\n"
              << " #  MESSAGES\n"
              << "(V) Information message\n"
              << "    Successfully performed an operation\n"
              << "[!] Error message\n"
              << "    An error occured and the operation can not be\n"
              << "    performed\n"
              << " #  VERSION\n"
              << "    Course: SDP 2022\n"
              << "    Project: Database\n"
              << "    Version: 1.0\n"
              << " ________________________________________________ \n";
}

Table* CommandLine::getTableByName(std::string tableName) {
    for (std::vector<Table*>::iterator it = database.tables.begin(); it != database.tables.end(); ++it) {
        if ((*it)->getTableName() == tableName) return (*it);
    }
    return nullptr;
}

void CommandLine::createTable() {
    std::string tableName;
    for (unsigned int i = 0; !isEmptySpace(commandLineInput[i]) && i < commandLineInput.size(); ++i)
        tableName.push_back(commandLineInput[i]);

    Table* selectedTable = getTableByName(tableName);
    if (!selectedTable) {
        database.tables.push_back(new Table(tableName));
        selectedTable = database.tables.back();
    }

    std::string fieldName;
    std::string datum;
    std::vector<std::string> fieldParameters;
    bool flagName = true;

    for (unsigned int i = 0 + tableName.size(); i < commandLineInput.size(); ++i) {
        if (commandLineInput[i] == '(') {
            fieldName.clear();
            datum.clear();
            fieldParameters.clear();
            continue;
        }

        if (commandLineInput[i] == ':') {
            flagName = false;
            continue;
        }

        if (commandLineInput[i] == ',' || commandLineInput[i] == ')') {
            fieldParameters.push_back(datum);

            int fieldType = 0;
            bool fieldNotNull = false;
            bool fieldIsIndex = false;
            std::string fieldDefault = "";

            for (unsigned int j = 0; j < (int)(FieldType::COUNTFIELDTYPE); ++j) {
                if (toUpperString(fieldParameters[0]) == FieldTypeList[j]) fieldType = j;
            }
            if (fieldType == -1) throw std::invalid_argument("[!] Invalid field type!");
            for (unsigned int j = 0; j < fieldParameters.size(); ++j) {
                if (toUpperString(fieldParameters[j]) == "NOTNULL") fieldNotNull = true;
                if (toUpperString(fieldParameters[j]) == "INDEX") fieldIsIndex = true;
                if (toUpperString(fieldParameters[j]) == "DEFAULT" && j + 1 < fieldParameters.size()) fieldDefault = fieldParameters[j + 1];
            }

            selectedTable->addField(fieldName, (FieldType)fieldType, fieldNotNull, fieldIsIndex, fieldDefault);

            fieldName.clear();
            datum.clear();
            fieldParameters.clear();
            flagName = true;
            continue;
        }

        if (!isEmptySpace(commandLineInput[i])) {
            if (flagName)
                fieldName.push_back(commandLineInput[i]);
            else
                datum.push_back(commandLineInput[i]);
        } else {
            if (!flagName) {
                fieldParameters.push_back(datum);
                datum.clear();
            }
        }
    }
    std::cout << "(V) New table created!" << std::endl;
}

void CommandLine::dropTable() {
    std::string tableName;
    for (unsigned int i = 0; !isEmptySpace(commandLineInput[i]) && i < commandLineInput.size(); ++i)
        tableName.push_back(commandLineInput[i]);

    bool flagDeleted = false;
    for (unsigned int i = 0; i < database.tables.size(); ++i) {
        if (database.tables[i]->getTableName() == tableName) {
            database.tables.erase(database.tables.begin() + i);
            flagDeleted = true;
            break;
        }
    }

    if (!flagDeleted) throw std::invalid_argument("[!] Unable to delete the table!");

    std::cout << "(V) A table is removed from the database!" << std::endl;
}

void CommandLine::listTables() {
    std::cout << "Number of tables in the database: " << database.tables.size() << std::endl;
    for (unsigned int i = 0; i < database.tables.size(); ++i) {
        std::cout << "  " << database.tables[i]->getTableName() << std::endl;
    }
}

void CommandLine::tableInfo() {
    std::string tableName;
    for (unsigned int i = 0; !isEmptySpace(commandLineInput[i]) && i < commandLineInput.size(); ++i)
        tableName.push_back(commandLineInput[i]);

    Table* selectedTable = getTableByName(tableName);
    if (!selectedTable) throw std::invalid_argument("[!] Unable to find selected table!");

    std::cout << selectedTable->tableInfo();
}

void CommandLine::insertRecord() {
    std::string tableName;
    std::string datum;
    std::vector<std::string> recordData;
    bool flagQuotes = false;

    for (unsigned int i = 0; !isEmptySpace(commandLineInput[i]) && i < commandLineInput.size(); ++i)
        tableName.push_back(commandLineInput[i]);

    Table* selectedTable = getTableByName(tableName);
    if (!selectedTable) throw std::invalid_argument("[!] Unable to find selected table!");

    for (unsigned int i = 0; i < commandLineInput.size(); ++i) {
        if (commandLineInput[i] == ',') {
            recordData.push_back(datum);
            datum.clear();
            continue;
        }
        if (commandLineInput[i] == '(') {
            recordData.clear();
            datum.clear();
            continue;
        }
        if (commandLineInput[i] == ')') {
            if (!datum.empty()) recordData.push_back(datum);
            if (!recordData.empty()) selectedTable->addRecord(recordData);
            recordData.clear();
            datum.clear();
            continue;
        }
        if (!isEmptySpace(commandLineInput[i])) datum.push_back(commandLineInput[i]);
    }

    if (!loading) std::cout << "(V) Inserted record in the database!" << std::endl;
}

void CommandLine::selectRecords() {
    std::string datum;
    std::vector<std::string> selectParameters;
    std::vector<std::string> selectedColumns;  // 0
    std::vector<std::string> distinctColumns;  // 1
    std::vector<std::string> orderByColumns;   // 2
    std::string tableName;
    std::vector<std::string> whereParameters;  // 3

    for (unsigned int i = 0; i < commandLineInput.size(); ++i) {
        if (commandLineInput[i] == ',' || commandLineInput[i] == ' ' || commandLineInput[i] == '\n') {
            if (!datum.empty()) selectParameters.push_back(datum);
            datum.clear();
            continue;
        }
        if (!isEmptySpace(commandLineInput[i])) datum.push_back(commandLineInput[i]);
    }

    int flagColumns = 0;
    for (unsigned int i = 0; i < selectParameters.size(); ++i) {
        if (toUpperString(selectParameters[i]) == "DISTINCT") {
            flagColumns = 1;
            continue;
        }
        if (toUpperString(selectParameters[i]) == "ORDERBY") {
            flagColumns = 2;
            continue;
        }
        if (toUpperString(selectParameters[i]) == "FROM") {
            tableName = selectParameters[++i];
            continue;
        }
        if (toUpperString(selectParameters[i]) == "WHERE") {
            flagColumns = 3;
            continue;
        }

        if (flagColumns == 0) selectedColumns.push_back(selectParameters[i]);
        if (flagColumns == 1) distinctColumns.push_back(selectParameters[i]);
        if (flagColumns == 2) orderByColumns.push_back(selectParameters[i]);
        if (flagColumns == 3) whereParameters.push_back(selectParameters[i]);
    }

    Table* selectedTable = getTableByName(tableName);
    if (!selectedTable) throw std::invalid_argument("[!] Unable to find selected table!");

    std::list<Record*> selectedRecords = selectedTable->selectRecords(distinctColumns, orderByColumns, whereParameters);
    if (distinctColumns.empty()) {
        printDatabase(selectedTable, selectedRecords, selectedColumns);
    } else {
        printDatabase(selectedTable, selectedRecords, distinctColumns);
    }
}

void CommandLine::printDatabase(Table* selectedTable, std::list<Record*> selectedRecords, std::vector<std::string> selectedColumns) {
    std::vector<int> columnsIndex;
    int colIndex = 0;

    if (selectedColumns.size() == 1 && selectedColumns[0] == "*") {
        for (unsigned int i = 0; i < selectedTable->getTableColumnsSize(); ++i) {
            columnsIndex.push_back(i);
        }
    } else {
        for (unsigned int i = 0; i < selectedColumns.size(); ++i) {
            colIndex = selectedTable->getFieldIndexByName(selectedColumns[i]);
            if (colIndex >= 0) columnsIndex.push_back(colIndex);
        }
    }

    std::vector<int> printSizes;
    int fullSize = 0;

    for (unsigned int i = 0; i < columnsIndex.size(); ++i) {
        printSizes.push_back(selectedTable->fieldName(columnsIndex[i]).size());
    }

    for (std::list<Record*>::iterator it = selectedRecords.begin(); it != selectedRecords.end(); ++it) {
        for (unsigned int i = 0; i < columnsIndex.size(); ++i) {
            int strSize = (*it)->getData(columnsIndex[i])->getValueUniform().size();
            if (printSizes[i] < strSize) {
                printSizes[i] = strSize;
            }
        }
    }

    for (unsigned int i = 0; i < columnsIndex.size(); ++i) {
        std::cout << "|";
        std::cout.width(printSizes[i]);
        std::cout << selectedTable->fieldName(columnsIndex[i]);
        fullSize += printSizes[i];
    }
    std::cout << "|\n";

    fullSize += printSizes.size() + 1;

    for (unsigned int i = 0; i < fullSize; ++i) std::cout << "-";
    std::cout << std::endl;

    for (std::list<Record*>::iterator it = selectedRecords.begin(); it != selectedRecords.end(); ++it) {
        for (unsigned int i = 0; i < columnsIndex.size(); ++i) {
            std::cout << "|";
            std::cout.width(printSizes[i]);
            std::cout << (*it)->getData(columnsIndex[i])->getValueUniform();
        }
        std::cout << "|" << std::endl;
    }
}

void CommandLine::closeInfile() {
    std::string tableName;
    for (unsigned int i = 0; !isEmptySpace(commandLineInput[i]) && i < commandLineInput.size(); ++i)
        tableName.push_back(commandLineInput[i]);

    Table* selectedTable = getTableByName(tableName);
    if (!selectedTable) throw std::invalid_argument("[!] Unable to find selected table!");

    std::string fileName = tableName + ".txt";
    std::ofstream fileOutput;
    fileOutput.open(fileName, std::ios::out);
    if (!fileOutput.is_open()) {
        throw std::invalid_argument("[!] Unable to open the file!");
    }
    fileOutput << selectedTable->tableDefinition();
    fileOutput << selectedTable->recordDefinition();
    fileOutput.close();
    selectedTable->cleanTable();
    selectedTable->setSaved();

    std::cout << "(V) Table colsed!" << std::endl;
}

void CommandLine::loadFromFile() {
    std::string tableName;
    for (unsigned int i = 0; !isEmptySpace(commandLineInput[i]) && i < commandLineInput.size(); ++i)
        tableName.push_back(commandLineInput[i]);

    Table* selectedTable = getTableByName(tableName);
    if (!selectedTable) throw std::invalid_argument("[!] Unable to find selected table!");
    std::string fileName = tableName + ".txt";
    std::ifstream fileInput;
    fileInput.open(fileName, std::ios::in);
    if (!fileInput.is_open()) {
        throw std::invalid_argument("[!] Unable to open the file!");
    }

    loading = true;
    std::string singleCommand;
    std::getline(fileInput, singleCommand, '\n');
    while (!fileInput.eof()) {
        std::getline(fileInput, singleCommand, '\n');
        CommandLineDirectInput(singleCommand.c_str());
    }

    fileInput.close();
    selectedTable->setLoaded();
    loading = false;
    std::cout << "(V) Table loaded!" << std::endl;
}

void CommandLine::removeRecords() {
    std::string datum;
    std::vector<std::string> selectParameters;
    std::string tableName;
    std::vector<std::string> whereParameters;

    for (unsigned int i = 0; i < commandLineInput.size(); ++i) {
        if (commandLineInput[i] == ',' || commandLineInput[i] == ' ' || commandLineInput[i] == '\n') {
            if (!datum.empty()) selectParameters.push_back(datum);
            datum.clear();
            continue;
        }
        if (!isEmptySpace(commandLineInput[i])) datum.push_back(commandLineInput[i]);
    }

    int flagWhere = 0;
    for (unsigned int i = 0; i < selectParameters.size(); ++i) {
        if (toUpperString(selectParameters[i]) == "FROM") {
            tableName = selectParameters[++i];
            continue;
        }
        if (toUpperString(selectParameters[i]) == "WHERE") {
            flagWhere = 1;
            continue;
        }
        if (flagWhere == 1) whereParameters.push_back(selectParameters[i]);
    }

    Table* selectedTable = getTableByName(tableName);
    if (!selectedTable) throw std::invalid_argument("[!] Unable to find selected table!");

    selectedTable->removeRecords(whereParameters);

    std::cout << "(V) Records removed!" << std::endl;
}