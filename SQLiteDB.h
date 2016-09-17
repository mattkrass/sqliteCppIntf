/*
 * SQLiteDB.h
 *
 *  Created on: Sep 5, 2016
 *      Author: matt
 */

#ifndef SQLITEDB_H_
#define SQLITEDB_H_

#include <sqlite3.h>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>

class SQLiteDB
{
private:
    sqlite3        *d_sqlHandle;
    const char     *d_errStr;
    bool            d_isOpen;

public:
    class SQLiteValue
    {
    public:
        enum SQLiteValueType {
            SQLiteValueTypeInteger  = SQLITE_INTEGER,
            SQLiteValueTypeDouble   = SQLITE_FLOAT,
            SQLiteValueTypeBlob     = SQLITE_BLOB,
            SQLiteValueTypeNull     = SQLITE_NULL,
            SQLiteValueTypeText     = SQLITE_TEXT
        };

        SQLiteValueType getType();
        int getValueAsInt();
        double getValueAsDouble();
        void* getValueAsBlob();
        std::string getValueAsString();

        SQLiteValue();
        SQLiteValue(int value);
        SQLiteValue(double value);
        SQLiteValue(void *value, int len);
        SQLiteValue(const char *value, int len);
        SQLiteValue(std::string value);
        SQLiteValue(sqlite3_value *value);

    private:
        SQLiteValueType     d_valueType;
        union {
            int             d_intValue;
            double          d_doubleValue;
            void*           d_blobValue;
        };

        std::string         d_stringValue;
        int                 d_length;
    };

    class SQLiteRow
    {
    private:
        int                 d_colCount;
        SQLiteValue*        d_colValues;
        std::string*        d_colNames;

    public:
        SQLiteRow(int colCount) : d_colCount(colCount), d_colValues(new SQLiteValue[colCount]), d_colNames(new std::string[colCount]) {}
        ~SQLiteRow();

        int count() { return d_colCount; }
        SQLiteValue* values() { return d_colValues; }
        std::string* names() { return d_colNames; }
    };

    SQLiteDB(std::string fileName);
    virtual ~SQLiteDB();

    bool isOpen();
    int executeSql(std::string query, std::vector<SQLiteRow*> *rows = 0);
    std::string error();
};

#endif /* SQLITEDB_H_ */
