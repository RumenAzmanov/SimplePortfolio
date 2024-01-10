#include "../Headers/utilityhelper.hpp"

int getOperator(const std::string& singleOperator) {
    int size = (int)(OperatorType::COUNTOPERATORTYPE);
    for (unsigned int i = 0; i < size; ++i) {
        if (singleOperator == OperatorTypeList[i]) return i;
    }
    return 0;
}

bool higherOrder(const std::string& operator1, const std::string& operator2) {
    int operator1order = OperatorTypePriorityList[getOperator(operator1)];
    int operator2order = OperatorTypePriorityList[getOperator(operator2)];
    return (operator2order <= operator1order);
}

bool isNumber(const std::string& numStr) {
    for (unsigned int i = 0; i < numStr.size(); ++i) {
        if (numStr[i] < '0' || numStr[i] > '9') return false;
    }
    return true;
}

bool isString(const std::string& strStr) {
    return (strStr[0] == '\"' && strStr[strStr.size() - 1] == '\"');
}

bool isDate(const std::string& datStr) {                                // Date format 31/12/2000
    if (datStr.size() == 10 && datStr[2] == '/' && datStr[5] == '/') {  // Size and format
        for (unsigned int i = 0; i < 10; ++i) {                         // Consists of numbers
            if (i != 2 && i != 5) {
                if (datStr[i] < '0' || datStr[i] > '9') return false;
            }
        }
        int day = 0, month = 0, year = 0;  // Is a correct date
        day += ((int)(datStr[0]) - (int)'0') * 10 + ((int)(datStr[1]) - (int)'0');
        month += ((int)(datStr[3]) - (int)'0') * 10 + ((int)(datStr[4]) - (int)'0');
        year += ((int)(datStr[6]) - (int)'0') * 1000 + ((int)(datStr[7]) - (int)'0') * 100 + ((int)(datStr[8]) - (int)'0') * 10 + ((int)(datStr[9]) - (int)'0');
        if (day >= 1 && day <= 31 && month >= 1 && month <= 12 && year >= 0 && year <= 9999) {
            int maxMonthDays = 30;
            switch (month) {
                case 2:
                    if (year % 4 == 0 && year % 100 != 0 || year % 400 == 0)
                        maxMonthDays = 29;
                    else
                        maxMonthDays = 28;
                    break;
                case 1:
                case 3:
                case 5:
                case 7:
                case 8:
                case 10:
                case 12:
                    maxMonthDays = 31;
                    break;

                default:
                    break;
            }
            if (day <= maxMonthDays) return true;
        }
    }
    return false;
}

bool isTrueOrFalse(const std::string& boolStr) {
    return (boolStr == "#TRUE" || boolStr == "#FALSE");
}

bool isEmptySpace(char c) {
    return (c == ' ' || c == '\n' || c == '\t');
}

bool isLetter(char c) {
    return (c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z');
}

bool datumPiece(char c) {
    return (c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c >= '0' && c <= '9' || c == '\"' || c == '<' || c == '>' || c == '=' || c == '!' || c == '(' || c == ')');
}

std::string toUpperString(std::string lowerCaseString) {
    for (unsigned int i = 0; i < lowerCaseString.size(); ++i) {
        if (lowerCaseString[i] >= 'a' && lowerCaseString[i] <= 'z') {
            lowerCaseString[i] = (char)(lowerCaseString[i] - 32);
        }
    }
    return lowerCaseString;
}
