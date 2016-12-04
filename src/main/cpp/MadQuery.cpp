//
// Created by William Kamp on 10/10/16.
//

#include "MadQuery.hpp"

#define CURSOR_STEP_UNKNOWN -1

using namespace madsqlite;
using namespace std;

//region Class Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//endregion

//region Constructor ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

MadQuery::MadQuery(sqlite3_stmt *statement) : statement(statement) {
}

MadQuery::MadQuery(MadQuery &&query) {
    statement = query.statement;
    position = query.position;
    query.statement = nullptr;
    query.position = 0;
}

MadQuery::~MadQuery() {
    if (statement) {
        sqlite3_finalize(statement);
    }
}

//endregion

//region Public Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool MadQuery::moveToFirst() {
    if (sqlite3_reset(statement) == SQLITE_OK) {
        stepResult = sqlite3_step(statement);
        position = 0;
        if (stepResult == SQLITE_ROW || stepResult == SQLITE_DONE) {
            return true;
        }
    }
    return false;
}

bool MadQuery::moveToNext() {
    if (!isAfterLast()) {
        stepResult = sqlite3_step(statement);
        if (stepResult == SQLITE_ROW || SQLITE_DONE) {
            ++position;
            return true;
        }
    }
    return false;
}

bool MadQuery::isAfterLast() {
    return stepResult > CURSOR_STEP_UNKNOWN && stepResult != SQLITE_ROW;
}

const string MadQuery::getString(int columnIndex) const {
    const unsigned char* text = sqlite3_column_text(statement, columnIndex);
    if (text) {
        return string(reinterpret_cast<const char*>(text));
    }
    return "";
}

const vector<byte> MadQuery::getBlob(int columnIndex) const {
    const void *blob = sqlite3_column_blob(statement, columnIndex);
    int sz = sqlite3_column_bytes(statement, columnIndex);
    const byte *charBuf = reinterpret_cast<const byte*>(blob);
    vector<byte> value(charBuf, charBuf + sz);
    return value;
}

sqlite3_int64 MadQuery::getInt(int columnIndex) {
    return (sqlite3_int64) sqlite3_column_int64(statement, columnIndex);
}

double MadQuery::getReal(int columnIndex) {
    return sqlite3_column_double(statement, columnIndex);
}

//endregion

//region Private Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//endregion
