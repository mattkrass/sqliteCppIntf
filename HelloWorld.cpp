#include <iostream>
#include "SQLiteDB.h"

const std::string sqlStatements[] = {
    "CREATE TABLE trades(trd_id int PRIMARY KEY, instr_id TEXT, instr_desc TEXT, amount FLOAT)",
    "INSERT INTO trades VALUES (1000, 'IBM US Equity', 'International Business Machine', 5.0)",
    "INSERT INTO trades VALUES (1001, 'IBM US Equity', 'International Business Machine', 6.0)",
    "INSERT INTO trades VALUES (1002, 'IBM US Equity', 'International Business Machine', 7.0)",
    "INSERT INTO trades VALUES (1003, 'IBM US Equity', 'International Business Machine', 8.0)",
    "SELECT * FROM trades WHERE trd_id > 1000",
};

int main()
{
    std::cout << "Hello World!" << std::endl;

    // create virtual db
    SQLiteDB sqlHandle(":memory:");
    std::cout << "Creating virtual database, isOpen = "
            << sqlHandle.isOpen() << std::endl;

    int rc = sqlHandle.executeSql(sqlStatements[0]);
    if(rc) {
        std::cerr << "Failed to create table, rc = " << rc << std::endl;
        return rc;
    }

    std::cout << "Executed: " << sqlStatements[0] << ", rc = " << rc << std::endl;

    // create fake records
    for(int idx = 1; idx < 5; idx++) {
        rc = sqlHandle.executeSql(sqlStatements[idx]);
        std::cout << "Executed: " << sqlStatements[idx] << ", rc = " << rc << std::endl;
        if(rc) {
            std::cerr << "Failed to insert row, rc = " << rc << std::endl;
            return rc;
        }
    }

    // select some records
    std::vector<SQLiteDB::SQLiteRow*> results;
    rc = sqlHandle.executeSql(sqlStatements[5], &results);
    std::cout << "Executed: " << sqlStatements[5] << ", rc = " << rc << std::endl;
    if(rc) {
        std::cerr << "Failed to select rows, rc = " << rc << std::endl;
    }

    std::cout << "Got " << results.size() << " rows back." << std::endl;
    for(std::vector<SQLiteDB::SQLiteRow*>::iterator it = results.begin(); it != results.end(); it++) {
        SQLiteDB::SQLiteRow *row = *it;
        std::cout << "Row: (" << row->count() << " columns)" << std::endl;
        for(int idx = 0; idx < row->count(); idx++) {
            std::cout << "\t" << row->names()[idx] << " = " << row->values()[idx].getValueAsString() << std::endl;
        }

        delete row;
    }

    return rc;
}
