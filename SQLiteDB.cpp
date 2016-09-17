/*
 * SQLiteDB.cpp
 *
 *  Created on: Sep 5, 2016
 *      Author: matt
 */

#include "SQLiteDB.h"

SQLiteDB::SQLiteDB(std::string fileName)
    : d_sqlHandle(0),
      d_errStr(0),
      d_isOpen(false)
{
    int rc = sqlite3_open(fileName.c_str(), &d_sqlHandle);
    d_isOpen = (rc == 0);

    if(rc) {
        d_errStr = sqlite3_errmsg(d_sqlHandle);
        std::ostringstream ss;
        ss << "SQLite totally whiffed on opening: " << fileName << ", rc = " << rc;
        if(d_errStr) {
            ss << ", message = " << d_errStr;
        }

        std::cerr << ss.str() << std::endl;
        throw std::runtime_error(ss.str());
    }
}

SQLiteDB::~SQLiteDB()
{
    sqlite3_free((void*)d_errStr);
    sqlite3_close(d_sqlHandle);
}

bool SQLiteDB::isOpen()
{
    return d_isOpen;
}

int SQLiteDB::executeSql(std::string query, std::vector<SQLiteRow*> *rows)
{
    sqlite3_stmt *sqlStmt;
    int rc = sqlite3_prepare_v2(d_sqlHandle, query.c_str(), query.length(), &sqlStmt, 0);
    if(rc) {
        // failed to compile statement
        std::cerr << "Failed to prepare: " << query << ", rc = " << rc;
        sqlite3_free((void*)d_errStr);
        d_errStr = sqlite3_errmsg(d_sqlHandle);
        if(d_errStr) {
            std::cerr << ", message = " << d_errStr;
        }
        std::cerr << std::endl;
        return rc;
    }

    rc = sqlite3_step(sqlStmt);
    while(SQLITE_ROW == rc) {
        if(!rows) { // result vector wasn't supplied, blow an exception
            throw std::runtime_error("rows is null!");
        }

        int colCount = sqlite3_data_count(sqlStmt);
        SQLiteRow *row = new SQLiteRow(colCount);
        for(int idx = 0; idx < colCount; idx++) {
            row->names()[idx] = std::string(sqlite3_column_name(sqlStmt, idx));
            row->values()[idx] = SQLiteDB::SQLiteValue(sqlite3_column_value(sqlStmt, idx));
        }

        rows->push_back(row);
        rc = sqlite3_step(sqlStmt);
    }

    rc = sqlite3_finalize(sqlStmt);

    return rc;
}

std::string SQLiteDB::error()
{
    if(d_errStr) {
        return std::string(d_errStr);
    }

    return std::string("");
}

SQLiteDB::SQLiteValue::SQLiteValueType SQLiteDB::SQLiteValue::getType() { return d_valueType; }
int SQLiteDB::SQLiteValue::getValueAsInt() { return d_intValue; }
double SQLiteDB::SQLiteValue::getValueAsDouble() { return d_doubleValue; }
void* SQLiteDB::SQLiteValue::getValueAsBlob() { return d_blobValue; }
std::string SQLiteDB::SQLiteValue::getValueAsString() { return d_stringValue; }

SQLiteDB::SQLiteValue::SQLiteValue()
    : d_valueType(SQLiteDB::SQLiteValue::SQLiteValueTypeNull),
      d_length(0)
{

}

SQLiteDB::SQLiteValue::SQLiteValue(int value)
    : d_valueType(SQLiteDB::SQLiteValue::SQLiteValueTypeInteger),
      d_intValue(value),
      d_length(sizeof(int))
{

}

SQLiteDB::SQLiteValue::SQLiteValue(double value)
    : d_valueType(SQLiteDB::SQLiteValue::SQLiteValueTypeDouble),
      d_doubleValue(value),
      d_length(sizeof(float))
{

}

SQLiteDB::SQLiteValue::SQLiteValue(void *value, int len)
    : d_valueType(SQLiteDB::SQLiteValue::SQLiteValueTypeBlob),
      d_blobValue(value),
      d_length(len)
{

}

SQLiteDB::SQLiteValue::SQLiteValue(const char *value, int len)
    : d_valueType(SQLiteDB::SQLiteValue::SQLiteValueTypeText),
      d_stringValue(value),
      d_length(d_stringValue.length())
{

}

SQLiteDB::SQLiteValue::SQLiteValue(std::string value)
    : d_valueType(SQLiteDB::SQLiteValue::SQLiteValueTypeText),
      d_stringValue(value),
      d_length(value.length())
{

}

SQLiteDB::SQLiteValue::SQLiteValue(sqlite3_value *value)
    : d_valueType(static_cast<SQLiteDB::SQLiteValue::SQLiteValueType>(sqlite3_value_type(value))),
      d_length(sqlite3_value_bytes(value))
{
    std::stringstream ss;
    switch(d_valueType) {
        case SQLiteDB::SQLiteValue::SQLiteValueTypeInteger: {
            d_intValue = sqlite3_value_int(value);
            ss << d_intValue;
        } break;

        case SQLiteDB::SQLiteValue::SQLiteValueTypeDouble: {
            d_doubleValue = sqlite3_value_double(value);
            ss << d_doubleValue;
        } break;

        case SQLiteDB::SQLiteValue::SQLiteValueTypeBlob: {
            d_blobValue = (void*)(sqlite3_value_blob(value));
            ss << "[blob " << d_length << " bytes]";
        } break;

        case SQLiteDB::SQLiteValue::SQLiteValueTypeNull: {
             ss << "[null]";
        } break;

        case SQLiteDB::SQLiteValue::SQLiteValueTypeText: {
            ss << ((const char *)sqlite3_value_text(value));
        } break;
    }

    d_stringValue = ss.str();
}

SQLiteDB::SQLiteRow::~SQLiteRow()
{
    delete[] d_colValues;
    delete[] d_colNames;
}
