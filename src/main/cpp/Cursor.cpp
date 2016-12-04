//
// Created by William Kamp on 10/10/16.
//

#include "Cursor.hpp"

#define CURSOR_STEP_UNKNOWN -1

//region Class Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//endregion

//region Constructor ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Cursor::Cursor(sqlite3_stmt *statement) : statement(statement) {
}

Cursor::Cursor(Cursor &&curs) {
    statement = curs.statement;
    position = curs.position;
    curs.statement = nullptr;
    curs.position = 0;
}

Cursor::~Cursor() {
    if (statement) {
        sqlite3_finalize(statement);
    }
}

//endregion

//region Public Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool Cursor::moveToFirst() {
    if (sqlite3_reset(statement) == SQLITE_OK) {
        stepResult = sqlite3_step(statement);
        position = 0;
        if (stepResult == SQLITE_ROW || stepResult == SQLITE_DONE) {
            return true;
        }
    }
    return false;
}

bool Cursor::moveToNext() {
    if (!isAfterLast()) {
        stepResult = sqlite3_step(statement);
        if (stepResult == SQLITE_ROW || SQLITE_DONE) {
            ++position;
            return true;
        }
    }
    return false;
}

bool Cursor::isAfterLast() {
    return stepResult > CURSOR_STEP_UNKNOWN && stepResult != SQLITE_ROW;
}

const std::string Cursor::getString(int columnIndex) const {
    const unsigned char* text = sqlite3_column_text(statement, columnIndex);
    if (text) {
        return std::string(reinterpret_cast<const char*>(text));
    }
    return "";
}

const std::vector<byte> Cursor::getBlob(int columnIndex) const {
    const void *blob = sqlite3_column_blob(statement, columnIndex);
    int sz = sqlite3_column_bytes(statement, columnIndex);
    const byte *charBuf = reinterpret_cast<const byte*>(blob);
    std::vector<byte> value(charBuf, charBuf + sz);
    return value;
}

sqlite3_int64 Cursor::getInt(int columnIndex) {
    return (sqlite3_int64) sqlite3_column_int64(statement, columnIndex);
}

double Cursor::getReal(int columnIndex) {
    return sqlite3_column_double(statement, columnIndex);
}

//endregion

//region Private Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//endregion
