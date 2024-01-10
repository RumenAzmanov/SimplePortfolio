
#ifndef UTILITYHELPER_HPP
#define UTILITYHELPER_HPP

#include <string>

//! Enum of all operators
enum class OperatorType {
    UNDEF = 0,
    // Priority level 1
    EQ = 1,
    NE = 2,
    GT = 3,
    LT = 4,
    GE = 5,
    LE = 6,
    // Priority level 2
    NOT = 7,
    // Priority level 3
    AND = 8,
    // Priority level 4
    OR = 9,
    COUNTOPERATORTYPE
};

//! List of all operators and their names
const char OperatorTypeList[(int)(OperatorType::COUNTOPERATORTYPE)][6] = {"UNDEF", "=", "!=", ">", "<", ">=", "<=", "NOT", "AND", "OR"};

//! List of all operators and their priority
const int OperatorTypePriorityList[(int)(OperatorType::COUNTOPERATORTYPE)] = {0, 1, 1, 1, 1, 1, 1, 2, 3, 4};

//! Get operator by its name
int getOperator(const std::string& singleOperator);

//! Verify if the second operator has higher order
bool higherOrder(const std::string& operator1, const std::string& operator2);

//! Verify if a string is a number value
bool isNumber(const std::string& numStr);

//! Verify if a string is a string value
bool isString(const std::string& strStr);

//! Verify if a string is a date value
bool isDate(const std::string& datStr);

//! Verify if a string is a boolean starting with #
bool isTrueOrFalse(const std::string& boolStr);

//! Verify if a character is empty space
bool isEmptySpace(char c);

//! Verify if a character is a letter
bool isLetter(char c);

//! Verify if a character is a piece of data
bool datumPiece(char c);

//! Make string uppercase
std::string toUpperString(std::string lowerCaseString);

#endif