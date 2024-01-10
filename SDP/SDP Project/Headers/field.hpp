#ifndef FIELD_HPP
#define FIELD_HPP

#include <string>

//! Enum of all field types
enum class FieldType {
    UNDEFINED = 0,
    INT = 1,
    STRING = 2,
    DATE = 3,
    COUNTFIELDTYPE
};

//! List of all field type names
const char FieldTypeList[(int)(FieldType::COUNTFIELDTYPE)][10] = {
    "UNDEFINED",
    "INT",
    "STRING",
    "DATE"};

//! The main field class
class DataField {
   private:
   public:
    virtual FieldType getType() { return FieldType::UNDEFINED; }
    virtual int getValueInt() { return 0; }
    virtual std::string getValueString() { return ""; }
    virtual std::string getValueDate() { return ""; }
    virtual std::string getValueUniform() { return ""; }
};

//! Field type INT
class DataFieldInt : public DataField {
   private:
    int intValue;

   public:
    DataFieldInt(int intValue) : intValue(intValue) {}
    FieldType getType() override { return FieldType::INT; }
    int getValueInt() override { return intValue; }
    std::string getValueUniform() override { return std::to_string(intValue); }
};

//! Field type STRING
class DataFieldString : public DataField {
   private:
    std::string stringValue;

   public:
    DataFieldString(std::string stringValue) : stringValue(stringValue) {}
    FieldType getType() override { return FieldType::STRING; }
    std::string getValueString() override { return stringValue; }
    std::string getValueUniform() override { return stringValue; }
};

//! Field type DATE
class DataFieldDate : public DataField {
   private:
    std::string stringValue;

   public:
    DataFieldDate(std::string stringValue) : stringValue(stringValue) {}
    FieldType getType() override { return FieldType::DATE; }
    std::string getValueDate() override { return stringValue; }
    std::string getValueUniform() override { return stringValue; }
};

#endif