#ifndef TABLE_HPP
#define TABLE_HPP

#include <iostream>
#include <list>
#include <queue>
#include <stack>
#include <string>
#include <vector>

#include "avltree.hpp"
#include "field.hpp"
#include "record.hpp"
#include "utilityhelper.hpp"

/** Table class
 *  The table has a linked list of all records.
 *  Each record has pointers to the single data value.
 */
class Table {
   private:
    //! The field blueprint
    struct TableFieldData {
        std::string fieldName;          //!< Name of the field
        FieldType fieldType;            //!< Type of the data in the field
        bool fieldNotNull = false;      //!< Flag value if the data could be empty
        bool fieldIsIndex = false;      //!< Flag value if the field should be indexed
        std::string fieldDefault = "";  //!< Default value for the field

        TableFieldData(std::string fieldName, FieldType fieldType, bool fieldNotNull, bool fieldIsIndex, std::string fieldDefault)
            : fieldName(fieldName), fieldType(fieldType), fieldNotNull(fieldNotNull), fieldIsIndex(fieldIsIndex), fieldDefault(fieldDefault) {}
    };

    //! Storage for indexed columns
    struct IndexedColumn {
        int columnIndex;            //!< Index of the order in the table
        AVLTree* indexedStructure;  //!< AVL Tree structure for indexing

        IndexedColumn(int columnIndex, FieldType type)
            : columnIndex(columnIndex) {
            indexedStructure = new AVLTree(columnIndex, type);
        }

        ~IndexedColumn() {
            indexedStructure->~AVLTree();
        }
    };

    std::string tableName;                       //!< Name of the table
    std::vector<TableFieldData*> tableFields;    //!< Blueprint of the table
    std::list<Record*> records;                  //!< Collection of all records
    std::vector<IndexedColumn*> indexedColumns;  //!< Collection of all indexed fields
    bool savedAsFile = false;                    //!< Flag if the table is now stored as file

   public:
    Table(std::string tableNameInput)
        : tableName(tableNameInput) {}

    ~Table() {
        clearTable();
    }

    //! Get field name
    std::string fieldName(int indexField);

    //! Get tablesize data in bytes
    int tableSizeBytes() const;

    //! Clear all data from the table
    void clearTable();

    //! Clear all records
    void cleanTable();

    //! Get the name of the table
    std::string getTableName() const;

    //! Get the number of fields - columns
    int getTableColumnsSize() const;

    //! Get the number of records - rows
    int getTableNumberOfRecords() const;

    //! Set the table as "saved in a file"
    void setSaved();

    //! Set the table as "loaded from a file"
    void setLoaded();

    //! Return the file status of the table
    bool getSaved() const;

    //! Return the table info as a string
    std::string tableInfo() const;

    //! Return the table structure as a command
    std::string tableDefinition() const;

    //! Return all records as commands
    std::string recordDefinition() const;

    //! Add field to the structure of the table
    void addField(std::string const& fieldName, FieldType fieldType, bool fieldNotNull = false, bool fieldIsIndex = false, std::string fieldDefault = "") {
        tableFields.push_back(new TableFieldData(fieldName, fieldType, fieldNotNull, fieldIsIndex, fieldDefault));
        if (fieldIsIndex) {  // Create an indexed structure
            int columnIndex = tableFields.size();
            indexedColumns.resize(columnIndex);
            indexedColumns[columnIndex - 1] = new IndexedColumn((columnIndex - 1), fieldType);
        }
    }

    //! Add record to the table
    void addRecord(std::vector<std::string> recordValuesInput);

    //! Get the order index of a field by its name
    int getFieldIndexByName(const std::string& selectedColumn) const;

    //! Calculate the operation
    std::string calculateWhere(Record* currentRecord, std::string operand1, std::string operand2, std::string operator0);

    //! Verify if a record is part of the selestion
    bool isInSelection(Record* currentRecord, std::queue<std::string> operands);

    //! Transform the conditions in reverse polish notation using sunting-yard algorithm
    std::queue<std::string> transformConditions(std::vector<std::string> whereConditions);

    //! Select all records using WHERE conditions
    std::list<Record*> selectRecordsWhere(std::vector<std::string> whereConditions);

    //! Verify if a record is distinct
    bool isDistinctSelection(Record* currentRecord, std::list<Record*> alreadySelected, std::vector<int> columnsIndex);

    //! Reduce only the distinct records based on selected columns
    std::list<Record*> selectRecordsDistinct(std::list<Record*> selectedRecords, std::vector<std::string> distinctColumns);

    //! Selection - main function
    std::list<Record*> selectRecords(std::vector<std::string> distinctColumns,
                                     std::vector<std::string> orderByColumns,
                                     std::vector<std::string> whereParameters);

    //! Remove selected records
    void removeRecords(std::vector<std::string> whereParameters);

    //! Record comparator
    bool compareRecords(Record* record1, Record* record2, int columnIndex);

    //! Sort list of records
    std::list<Record*> sortRecordsWhere(std::list<Record*>& selectedRecords, int columnIndex);
};

#endif