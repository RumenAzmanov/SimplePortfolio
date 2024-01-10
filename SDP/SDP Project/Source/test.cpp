#define CATCH_CONFIG_MAIN
#include "../catch2/catch.hpp"
#include "../Headers/command.hpp"
#include "../Headers/database.hpp"
#include "../Headers/field.hpp"
#include "../Headers/table.hpp"
#include "../Headers/utilityhelper.hpp"

TEST_CASE("Utility testing") {
    SECTION("Operator types") {
        REQUIRE(getOperator("<") == (int)OperatorType::LT);
        REQUIRE(getOperator(">") == (int)OperatorType::GT);
        REQUIRE(getOperator("AND") == (int)OperatorType::AND);
        REQUIRE(getOperator("NOT") == (int)OperatorType::NOT);

        REQUIRE(getOperator("==") == (int)OperatorType::UNDEF);
        REQUIRE(getOperator("=!") == (int)OperatorType::UNDEF);
        REQUIRE(getOperator("=>") == (int)OperatorType::UNDEF);
        REQUIRE(getOperator("") == (int)OperatorType::UNDEF);
    }

    SECTION("Operator order") {
        REQUIRE(higherOrder("OR", "<="));
        REQUIRE(higherOrder("AND", "NOT"));
        REQUIRE(higherOrder("AND", "AND"));
        REQUIRE(higherOrder("!=", "!="));

        REQUIRE(higherOrder("=", "NOT") == false);
        REQUIRE(higherOrder(">=", "OR") == false);
    }

    SECTION("Number verification") {
        REQUIRE(isNumber("123"));
        REQUIRE(isNumber("500"));
        REQUIRE(isNumber("005"));

        REQUIRE(isNumber("12.34") == false);
        REQUIRE(isNumber("12*34") == false);
        REQUIRE(isNumber("123abc456") == false);
        REQUIRE(isNumber("123_456") == false);
    }

    SECTION("String verification") {
        REQUIRE(isString("\"Hello world!\""));
        REQUIRE(isString("\"Name\""));
        REQUIRE(isString("\"123\""));
        REQUIRE(isString("\"+-*/\""));

        REQUIRE(isString("\"string") == false);
        REQUIRE(isString("string\"") == false);
        REQUIRE(isString("string") == false);
    }

    SECTION("Date verification") {
        REQUIRE(isDate("01/01/2000"));
        REQUIRE(isDate("31/12/2022"));

        REQUIRE(isDate("01.01.2000") == false);
        REQUIRE(isDate("01 01 2000") == false);
        REQUIRE(isDate("01012000") == false);
        REQUIRE(isDate("01/012000") == false);

        REQUIRE(isDate("30/02/2022") == false);
        REQUIRE(isDate("31/04/2022") == false);
    }

    SECTION("Bool string verification") {
        REQUIRE(isTrueOrFalse("#TRUE"));
        REQUIRE(isTrueOrFalse("#FALSE"));

        REQUIRE(isTrueOrFalse("TRUE") == false);
        REQUIRE(isTrueOrFalse("#TRU") == false);
        REQUIRE(isTrueOrFalse("") == false);
    }

    SECTION("Letter symbol verification") {
        REQUIRE(isLetter('A'));
        REQUIRE(isLetter('a'));
        REQUIRE(isLetter('Z'));
        REQUIRE(isLetter('z'));
        REQUIRE((isLetter('S') && isLetter('D') && isLetter('P')));

        REQUIRE(isLetter('5') == false);
        REQUIRE(isLetter('*') == false);
        REQUIRE(isLetter('#') == false);
    }

    SECTION("Data symbol verification") {
        REQUIRE(datumPiece('A'));
        REQUIRE(datumPiece('a'));
        REQUIRE(datumPiece('Z'));
        REQUIRE(datumPiece('z'));
        REQUIRE(datumPiece('0'));
        REQUIRE(datumPiece('5'));
        REQUIRE(datumPiece('('));
        REQUIRE(datumPiece('\"'));
        REQUIRE(datumPiece('='));

        REQUIRE(datumPiece(' ') == false);
        REQUIRE(datumPiece('\n') == false);
    }

    SECTION("Uppercase string") {
        REQUIRE(toUpperString("Hello world!") == "HELLO WORLD!");
        REQUIRE(toUpperString("iNt StRiNg DaTe") == "INT STRING DATE");
        REQUIRE(toUpperString("123") == "123");
    }
}

TEST_CASE("Table tests") {
    Table myTable("MyTable");

    SECTION("Empty new table") {
        REQUIRE(myTable.getTableName() == "MyTable");
        REQUIRE(myTable.tableSizeBytes() == 0);
        REQUIRE(myTable.getTableColumnsSize() == 0);
        REQUIRE(myTable.getTableNumberOfRecords() == 0);
    }

    myTable.addField("ID", FieldType::INT, false, true, "");
    myTable.addField("Name", FieldType::STRING, false, false, "");
    myTable.addField("Date", FieldType::DATE, false, false, "");

    SECTION("Table with structure") {
        REQUIRE(myTable.tableSizeBytes() == 0);
        REQUIRE(myTable.getTableColumnsSize() == 3);
        REQUIRE(myTable.getTableNumberOfRecords() == 0);
        REQUIRE(myTable.tableDefinition() == "CREATETABLE MyTable (ID:INT INDEX, Name:STRING, Date:DATE)\n");
    }

    std::vector<std::string> singleRecord;

    singleRecord.clear();
    singleRecord.push_back("400");
    singleRecord.push_back("\"Name1\"");
    singleRecord.push_back("02/01/2022");
    myTable.addRecord(singleRecord);

    singleRecord.clear();
    singleRecord.push_back("100");
    singleRecord.push_back("\"Name2\"");
    singleRecord.push_back("01/01/2022");
    myTable.addRecord(singleRecord);

    singleRecord.clear();
    singleRecord.push_back("500");
    singleRecord.push_back("\"Name3\"");
    singleRecord.push_back("03/01/2022");
    myTable.addRecord(singleRecord);

    singleRecord.clear();
    singleRecord.push_back("300");
    singleRecord.push_back("\"Name4\"");
    singleRecord.push_back("03/01/2022");
    myTable.addRecord(singleRecord);

    singleRecord.clear();
    singleRecord.push_back("200");
    singleRecord.push_back("\"Name5\"");
    singleRecord.push_back("02/01/2022");
    myTable.addRecord(singleRecord);

    SECTION("Table with records") {
        REQUIRE(myTable.getTableColumnsSize() == 3);
        REQUIRE(myTable.getTableNumberOfRecords() == 5);
    }

    // SELECT * FROM MyTable
    std::vector<std::string> distinctColumns;
    std::vector<std::string> orderByColumns;
    std::vector<std::string> whereParameters;

    SECTION("SELECT * FROM MyTable") {
        std::list<Record*> selection = myTable.selectRecords(distinctColumns, orderByColumns, whereParameters);
        REQUIRE(selection.size() == 5);
        REQUIRE(selection.front()->getData(0)->getValueUniform() == "400");
        REQUIRE(selection.front()->getData(1)->getValueUniform() == "\"Name1\"");
        REQUIRE(selection.front()->getData(2)->getValueUniform() == "02/01/2022");
        REQUIRE(selection.back()->getData(0)->getValueUniform() == "200");
        REQUIRE(selection.back()->getData(1)->getValueUniform() == "\"Name5\"");
        REQUIRE(selection.back()->getData(2)->getValueUniform() == "02/01/2022");
    }

    SECTION("SELECT DISTINCT Date FROM MyTable") {
        distinctColumns.push_back("Date");
        std::list<Record*> selection = myTable.selectRecords(distinctColumns, orderByColumns, whereParameters);
        REQUIRE(selection.size() == 3);
    }

    SECTION("SELECT ORDERBY ID FROM MyTable") {
        orderByColumns.push_back("ID");
        std::list<Record*> selection = myTable.selectRecords(distinctColumns, orderByColumns, whereParameters);
        REQUIRE(selection.size() == 5);
        REQUIRE(selection.front()->getData(0)->getValueUniform() == "100");
        REQUIRE(selection.front()->getData(1)->getValueUniform() == "\"Name2\"");
        REQUIRE(selection.front()->getData(2)->getValueUniform() == "01/01/2022");
        REQUIRE(selection.back()->getData(0)->getValueUniform() == "500");
        REQUIRE(selection.back()->getData(1)->getValueUniform() == "\"Name3\"");
        REQUIRE(selection.back()->getData(2)->getValueUniform() == "03/01/2022");
    }

    SECTION("SELECT * FROM MyTable WHERE Name = \"Name3\"") {
        whereParameters.push_back("Name");
        whereParameters.push_back("=");
        whereParameters.push_back("\"Name3\"");
        std::list<Record*> selection = myTable.selectRecords(distinctColumns, orderByColumns, whereParameters);
        REQUIRE(selection.size() == 1);
        REQUIRE(selection.front()->getData(0)->getValueUniform() == "500");
        REQUIRE(selection.front()->getData(1)->getValueUniform() == "\"Name3\"");
        REQUIRE(selection.front()->getData(2)->getValueUniform() == "03/01/2022");
    }

    SECTION("Index test -> SELECT * FROM MyTable WHERE ID < 300") {
        whereParameters.push_back("ID");
        whereParameters.push_back("<");
        whereParameters.push_back("300");
        std::list<Record*> selection = myTable.selectRecords(distinctColumns, orderByColumns, whereParameters);
        REQUIRE(selection.size() == 2);
    }

    SECTION("REMOVE FROM MyTable WHERE Date = 02/01/2022") {
        whereParameters.push_back("Date");
        whereParameters.push_back("=");
        whereParameters.push_back("02/01/2022");
        myTable.removeRecords(whereParameters);
        REQUIRE(myTable.getTableNumberOfRecords() == 3);
    }
}