#include "../Headers/table.hpp"

std::string Table::fieldName(int indexField) {
    return tableFields[indexField]->fieldName;
}

int Table::tableSizeBytes() const {
    int sizeBytes = 0;
    for (std::list<Record*>::const_iterator it = records.begin(); it != records.end(); ++it) {
        for (unsigned int i = 0; i < (*it)->fields.size(); ++i) {
            if ((*it)->fields[i]->getType() == FieldType::INT) sizeBytes += 4;
            if ((*it)->fields[i]->getType() == FieldType::STRING) sizeBytes += (*it)->fields[i]->getValueUniform().size();
            if ((*it)->fields[i]->getType() == FieldType::DATE) sizeBytes += 10;
        }
    }
    return sizeBytes;
}

void Table::clearTable() {
    while (!records.empty()) records.pop_back();
    while (!tableFields.empty()) tableFields.pop_back();
    indexedColumns.clear();
}

void Table::cleanTable() {
    while (!records.empty()) records.pop_back();
    indexedColumns.clear();
}

std::string Table::getTableName() const {
    return tableName;
}

int Table::getTableColumnsSize() const {
    return tableFields.size();
}

int Table::getTableNumberOfRecords() const {
    return records.size();
}

void Table::setSaved() {
    savedAsFile = true;
}

void Table::setLoaded() {
    savedAsFile = false;
}

bool Table::getSaved() const {
    return savedAsFile;
}

std::string Table::tableInfo() const {
    std::string information;
    information += "Name: " + tableName + "\n";
    for (unsigned int i = 0; i < tableFields.size(); ++i) {
        information += tableFields[i]->fieldName + ":" + FieldTypeList[(int)tableFields[i]->fieldType] +
                       (tableFields[i]->fieldNotNull ? ", NOTNULL" : "") +
                       (tableFields[i]->fieldIsIndex ? ", INDEX" : "") +
                       (!tableFields[i]->fieldDefault.empty() ? (", DEFAULT " + tableFields[i]->fieldDefault) : "") + "\n";
    }
    information += "Records: " + std::to_string(records.size()) + " \n";
    information += "Data: " + std::to_string(tableSizeBytes()) + "bytes \n";
    return information;
}

std::string Table::tableDefinition() const {
    std::string information;
    information += "CREATETABLE " + tableName + " (";
    for (unsigned int i = 0; i < tableFields.size(); ++i) {
        information += tableFields[i]->fieldName + ":" + FieldTypeList[(int)tableFields[i]->fieldType] +
                       (tableFields[i]->fieldNotNull ? " NOTNULL" : "") +
                       (tableFields[i]->fieldIsIndex ? " INDEX" : "") +
                       (!tableFields[i]->fieldDefault.empty() ? (" DEFAULT " + tableFields[i]->fieldDefault) : "");
        if (i + 1 < tableFields.size()) information += ", ";
    }
    information += ")\n";
    return information;
}

std::string Table::recordDefinition() const {
    std::string information;
    for (std::list<Record*>::const_iterator it = records.begin(); it != records.end(); ++it) {
        information += "INSERT " + tableName + " (";
        for (unsigned int i = 0; i < (*it)->fields.size(); ++i) {
            information += (*it)->fields[i]->getValueUniform();
            if (i + 1 < (*it)->fields.size()) information += ", ";
        }
        information += ")\n";
    }
    return information;
}

void Table::addRecord(std::vector<std::string> recordValuesInput) {
    int recordValuesSize = recordValuesInput.size();
    if (tableFields.size() != recordValuesSize) {
        throw std::invalid_argument("[!] Invalid number of input arguments for the record!");
    } else {
        Record* newRecord = new Record;
        for (unsigned int i = 0; i < recordValuesSize; ++i) {
            if (!(tableFields[i]->fieldNotNull && recordValuesInput[i] == "")) {
                switch (tableFields[i]->fieldType) {
                    case FieldType::INT:
                        if (isNumber(recordValuesInput[i])) {
                            if (tableFields[i]->fieldDefault != "" && recordValuesInput[i] == "") {
                                newRecord->fields.push_back(new DataFieldInt(stoi(tableFields[i]->fieldDefault)));  // Use default value
                            } else {
                                newRecord->fields.push_back(new DataFieldInt(stoi(recordValuesInput[i])));  // Use input value
                            }
                        } else {
                            throw std::invalid_argument("[!] Invalid input parameter for the record! INT is required for the field!");
                        }
                        break;
                    case FieldType::STRING:
                        if (isString(recordValuesInput[i])) {
                            if (tableFields[i]->fieldDefault != "" && recordValuesInput[i] == "") {
                                newRecord->fields.push_back(new DataFieldString(tableFields[i]->fieldDefault));  // Use default value
                            } else {
                                newRecord->fields.push_back(new DataFieldString(recordValuesInput[i]));  // Use input value
                            }
                        } else {
                            throw std::invalid_argument("[!] Invalid input parameter for the record! STRING is required for the field!");
                        }
                        break;

                    case FieldType::DATE:
                        if (isDate(recordValuesInput[i])) {
                            if (tableFields[i]->fieldDefault != "" && recordValuesInput[i] == "") {
                                newRecord->fields.push_back(new DataFieldDate(tableFields[i]->fieldDefault));  // Use default value
                            } else {
                                newRecord->fields.push_back(new DataFieldDate(recordValuesInput[i]));  // Use input value
                            }
                        } else {
                            throw std::invalid_argument("[!] Invalid input parameter for the record! DATE is required for the field!");
                        }
                        break;
                    default:
                        throw std::runtime_error("[!] Invalid field type!");
                }

                if (tableFields[i]->fieldIsIndex) {  // Add to the index
                    indexedColumns[i]->indexedStructure->insert(newRecord);
                }
            } else {
                throw std::invalid_argument("[!] Invalid input parameter for the record! The value cannot be empty!");
            }
        }

        records.push_back(newRecord);
    }
}

int Table::getFieldIndexByName(const std::string& selectedColumn) const {
    for (unsigned int i = 0; i < tableFields.size(); ++i) {
        if (tableFields[i]->fieldName == selectedColumn) return i;
    }
    return -1;
}

std::string Table::calculateWhere(Record* currentRecord, std::string operand1, std::string operand2, std::string operator0) {
    OperatorType operatorType = (OperatorType)getOperator(operator0);
    if (getFieldIndexByName(operand1) == -1 && getFieldIndexByName(operand2) >= 0) {
        std::string swap = operand1;
        operand1 = operand2;
        operand2 = swap;
    }
    if (!(isTrueOrFalse(operand1) || isTrueOrFalse(operand2))) {
        if (getFieldIndexByName(operand1) >= 0 && getFieldIndexByName(operand2) == -1) {
            if (!(isNumber(operand2) || isString(operand2) || isDate(operand2))) {
                throw std::invalid_argument("[!] Invalid selection structure! The operands should be literals!");
            }
        } else {
            throw std::invalid_argument("[!] Invalid selection structure! The operands should be field names!");
        }
        if (OperatorTypePriorityList[(int)operatorType] == 1) {
            operand1 = currentRecord->fields[getFieldIndexByName(operand1)]->getValueUniform();
        }
    }

    if (isNumber(operand1)) {
        int operand1int = stoi(operand1);
        int operand2int = stoi(operand2);
        switch (operatorType) {
            case OperatorType::EQ:
                return (operand1int == operand2int) ? "#TRUE" : "#FALSE";
            case OperatorType::NE:
                return (operand1int != operand2int) ? "#TRUE" : "#FALSE";
            case OperatorType::GT:
                return (operand1int > operand2int) ? "#TRUE" : "#FALSE";
            case OperatorType::LT:
                return (operand1int < operand2int) ? "#TRUE" : "#FALSE";
            case OperatorType::GE:
                return (operand1int >= operand2int) ? "#TRUE" : "#FALSE";
            case OperatorType::LE:
                return (operand1int <= operand2int) ? "#TRUE" : "#FALSE";
            default:
                return "#FALSE";
        }
    } else {
        switch (operatorType) {
            case OperatorType::EQ:
                return (operand1 == operand2) ? "#TRUE" : "#FALSE";
            case OperatorType::NE:
                return (operand1 != operand2) ? "#TRUE" : "#FALSE";
            case OperatorType::GT:
                return (operand1 > operand2) ? "#TRUE" : "#FALSE";
            case OperatorType::LT:
                return (operand1 < operand2) ? "#TRUE" : "#FALSE";
            case OperatorType::GE:
                return (operand1 >= operand2) ? "#TRUE" : "#FALSE";
            case OperatorType::LE:
                return (operand1 <= operand2) ? "#TRUE" : "#FALSE";
            case OperatorType::AND:
                return (operand1 == "#TRUE" && operand2 == "#TRUE") ? "#TRUE" : "#FALSE";
            case OperatorType::OR:
                return (operand1 == "#TRUE" || operand2 == "#TRUE") ? "#TRUE" : "#FALSE";
            default:
                return "#FALSE";
        }
    }
}

bool Table::isInSelection(Record* currentRecord, std::queue<std::string> operands) {
    std::stack<std::string> solution;
    while (!operands.empty()) {
        if (operands.front() == "NOT") {  // Boolean operators directly access the solution
            if (solution.top() == "#FALSE") {
                solution.push("#FALSE");
            } else {
                solution.push("#TRUE");
            }
            operands.pop();
        } else {
            std::string operand1, operand2;
            if (getOperator(operands.front()) == 0) {
                operand1 = operands.front();
                operands.pop();
                operand2 = operands.front();
                operands.pop();
            }
            if (operands.front() == "AND" || operands.front() == "OR") {  // Boolean operators access the solution
                operand1 = solution.top();
                solution.pop();
                operand2 = solution.top();
                solution.pop();
            }
            // std::cout << "#CALCULATE " << operand1 << " " << operand2 << " " << operands.front() << std::endl;
            solution.push(calculateWhere(currentRecord, operand1, operand2, operands.front()));
            operands.pop();
        }
    }

    if (solution.size() > 1) throw std::invalid_argument("[!] Invalid expression! Invalid number of arguments!");

    return (solution.top() == "#TRUE");
}

std::queue<std::string> Table::transformConditions(std::vector<std::string> whereConditions) {
    std::stack<std::string> operators;
    std::queue<std::string> operands;

    for (unsigned int i = 0; i < whereConditions.size(); ++i) {
        if (whereConditions[i] == "(") {
            operators.push(whereConditions[i]);
        }
        if (whereConditions[i] == ")") {
            while (!operators.empty() && operators.top() != "(") {
                operands.push(operators.top());
                operators.pop();
            }
        }
        if (getOperator(whereConditions[i]) > 0) {
            while (!operators.empty() && !higherOrder(operators.top(), whereConditions[i])) {
                operands.push(operators.top());
                operators.pop();
            }
            operators.push(whereConditions[i]);
        } else {
            operands.push(whereConditions[i]);
        }
    }

    while (!operators.empty()) {
        operands.push(operators.top());
        operators.pop();
    }

    return operands;
}

std::list<Record*> Table::selectRecordsWhere(std::vector<std::string> whereConditions) {
    std::list<Record*> selectedRecords;
    selectedRecords.clear();
    if (records.size() == 0) return selectedRecords;
    if (whereConditions.empty()) {
        return records;
    } else {
        if (whereConditions.size() == 3) {
            OperatorType smartOperator = (OperatorType)getOperator(whereConditions[1]);
            int selectedColumn = getFieldIndexByName(whereConditions[0]);
            if (selectedColumn >= 0 && tableFields[selectedColumn]->fieldIsIndex) {
                return indexedColumns[selectedColumn]->indexedStructure->searchNode(whereConditions[2], smartOperator);
            }
        }

        std::queue<std::string> operands = transformConditions(whereConditions);
        for (std::list<Record*>::iterator it = records.begin(); it != records.end(); ++it) {
            if (isInSelection((*it), operands)) {
                selectedRecords.push_back(*it);
            }
        }
    }
    return selectedRecords;
}

bool Table::isDistinctSelection(Record* currentRecord, std::list<Record*> alreadySelected, std::vector<int> columnsIndex) {
    for (std::list<Record*>::iterator it = alreadySelected.begin(); it != alreadySelected.end(); ++it) {
        bool flagUnique = false;
        for (unsigned int i = 0; i < columnsIndex.size(); ++i) {
            if (currentRecord->fields[columnsIndex[i]]->getValueUniform() != (*it)->fields[columnsIndex[i]]->getValueUniform()) {
                flagUnique = true;
                break;
            }
        }
        if (!flagUnique) return false;
    }
    return true;
}

std::list<Record*> Table::selectRecordsDistinct(std::list<Record*> selectedRecords, std::vector<std::string> distinctColumns) {
    std::list<Record*> selected;
    std::vector<int> columnsIndex;
    for (unsigned int i = 0; i < distinctColumns.size(); ++i) {
        int columnIndex = getFieldIndexByName(distinctColumns[i]);
        if (columnIndex >= 0) columnsIndex.push_back(columnIndex);
    }

    for (std::list<Record*>::iterator it = selectedRecords.begin(); it != selectedRecords.end(); ++it) {
        if (isDistinctSelection(*it, selected, columnsIndex)) {
            selected.push_back(*it);
        }
    }
    return selected;
}

std::list<Record*> Table::selectRecords(std::vector<std::string> distinctColumns,
                                        std::vector<std::string> orderByColumns,
                                        std::vector<std::string> whereParameters) {
    std::list<Record*> selectedRecordsWhere = selectRecordsWhere(whereParameters);
    if (!distinctColumns.empty()) {
        std::list<Record*> selectedRecordsDistinctWhere = selectRecordsDistinct(selectedRecordsWhere, distinctColumns);
        return selectedRecordsDistinctWhere;
    }
    for (unsigned int i = 0; i < orderByColumns.size(); ++i) {
        selectedRecordsWhere = sortRecordsWhere(selectedRecordsWhere, getFieldIndexByName(orderByColumns[i]));
    }
    return selectedRecordsWhere;
}

void Table::removeRecords(std::vector<std::string> whereParameters) {
    std::list<Record*> selectedRecordsWhere = selectRecordsWhere(whereParameters);
    for (std::list<Record*>::iterator it = selectedRecordsWhere.begin(); it != selectedRecordsWhere.end(); ++it) {
        for (unsigned int i = 0; i < tableFields.size(); ++i) {
            if (tableFields[i]->fieldIsIndex) {
                indexedColumns[i]->indexedStructure->removeData(*it);
            }
        }
        records.remove(*it);
    }
}

bool Table::compareRecords(Record* record1, Record* record2, int columnIndex) {  // True if the second is bigger
    if (tableFields[columnIndex]->fieldType == FieldType::INT) {
        return (record1->fields[columnIndex]->getValueInt() < record2->fields[columnIndex]->getValueInt());
    }
    if (tableFields[columnIndex]->fieldType == FieldType::STRING) {
        return (record1->fields[columnIndex]->getValueUniform().compare(record2->fields[columnIndex]->getValueUniform()) > 1);
    }
}

std::list<Record*> Table::sortRecordsWhere(std::list<Record*>& selectedRecords, int columnIndex) {
    std::list<Record*> selected;
    Record* minElement = selectedRecords.front();
    while (!selectedRecords.empty()) {
        minElement = selectedRecords.front();
        for (std::list<Record*>::iterator it = selectedRecords.begin(); it != selectedRecords.end(); ++it) {
            if (compareRecords((*it), minElement, columnIndex)) minElement = (*it);
        }
        selected.push_back(minElement);
        selectedRecords.remove(minElement);
    }
    return selected;
}