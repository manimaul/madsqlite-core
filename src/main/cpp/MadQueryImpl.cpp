//
// Created by William Kamp on 10/10/16.
//

#include "MadQuery.hpp"
#include "MadQueryImpl.hpp"
#include <memory>

#define CURSOR_STEP_UNKNOWN -1

using namespace madsqlite;
using namespace std;

//region Class Methods 
//endregion

MadQuery::MadQuery(unique_ptr<Impl> impl) : impl(move(impl)) {}

MadQuery::MadQuery(MadQuery &&other) {
    impl = move(other.impl);
    other.impl = nullptr;
}

// Note: PIMPL with unique_ptr requires a declared deconstructor otherwise the compiler generates a default one which
// needs a complete declaration
MadQuery::~MadQuery() {}

//region Constructor 

MadQuery::Impl::Impl(sqlite3_stmt *statement) : statement(statement) {}

MadQuery::Impl::Impl(Impl &&other) {
    statement = other.statement;
    position = other.position;
    other.statement = nullptr;
    other.position = 0;
}

MadQuery::Impl::~Impl() {
    if (NULL != statement) {
        sqlite3_finalize(statement);
    }
}

//endregion

//region MadQuery

bool MadQuery::moveToFirst() {
    return impl->moveToFirst();
}

bool MadQuery::moveToNext() {
    return impl->moveToNext();
}

bool MadQuery::isAfterLast() {
    return impl->isAfterLast();
}

const std::string MadQuery::getString(int columnIndex) const {
    return impl->getString(columnIndex);
}

const std::vector<unsigned char> MadQuery::getBlob(int columnIndex) const {
    return impl->getBlob(columnIndex);
}

long long int MadQuery::getInt(int columnIndex) {
    return impl->getInt(columnIndex);
}

double MadQuery::getReal(int columnIndex) {
    return impl->getReal(columnIndex);
}

//endregion

//region MadQuery::Impl

bool MadQuery::Impl::moveToFirst() {
    if (sqlite3_reset(statement) == SQLITE_OK) {
        stepResult = sqlite3_step(statement);
        position = 0;
        if (stepResult == SQLITE_ROW) {
            return true;
        }
    }
    return false;
}

bool MadQuery::Impl::moveToNext() {
    if (!isAfterLast()) {
        stepResult = sqlite3_step(statement);
        if (stepResult == SQLITE_ROW || SQLITE_DONE) {
            ++position;
            return true;
        }
    }
    return false;
}

bool MadQuery::Impl::isAfterLast() {
    return stepResult > CURSOR_STEP_UNKNOWN && stepResult != SQLITE_ROW;
}

const string MadQuery::Impl::getString(int columnIndex) const {
    const unsigned char* text = sqlite3_column_text(statement, columnIndex);
    if (text) {
        return string(reinterpret_cast<const char*>(text));
    }
    return "";
}

const vector<unsigned char> MadQuery::Impl::getBlob(int columnIndex) const {
    const void *blob = sqlite3_column_blob(statement, columnIndex);
    int sz = sqlite3_column_bytes(statement, columnIndex);
    const unsigned char *charBuf = reinterpret_cast<const unsigned char*>(blob);
    vector<unsigned char> value(charBuf, charBuf + sz);
    return value;
}

long long int MadQuery::Impl::getInt(int columnIndex) {
    return (long long int) sqlite3_column_int64(statement, columnIndex);
}

double MadQuery::Impl::getReal(int columnIndex) {
    return sqlite3_column_double(statement, columnIndex);
}

//endregion
