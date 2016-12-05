//
// Created by William Kamp on 10/9/16.
//

#include <iostream>
#include "MadDatabase.hpp"
#include "MadUtil.hpp"

using namespace madsqlite;
using namespace std;

//region Class Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//endregion

//region Constructor ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

MadDatabase::MadDatabase() {
    sqlite3_open(":memory:", &db);
}

MadDatabase::MadDatabase(string const &dbPath) {
    sqlite3_open(dbPath.c_str(), &db);
    string abs = getAbsoluteFilePath(dbPath);
}

MadDatabase::~MadDatabase() {
    sqlite3_close(db);
}

//endregion

//region Public Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void MadDatabase::beginTransaction() {
    if (!isInTransaction) {
        execInternal("BEGIN");
        isInTransaction = true;
    }
}

void MadDatabase::rollbackTransaction() {
    if (isInTransaction) {
        execInternal("ROLLBACK");
        isInTransaction = false;
    }
}

void MadDatabase::commitTransaction() {
    if (isInTransaction) {
        execInternal("COMMIT");
        isInTransaction = false;
    }
}

int MadDatabase::exec(string const &sql) {
    string upper = upperCaseString(sql);
    if (transactionKeyWords.find(upper) != transactionKeyWords.end()) {
        return 0;
    }
    return execInternal(sql);
}

string MadDatabase::getError() {
    auto err = string(sqlite3_errmsg(db));
    if (err.compare("not an error") == 0 || err.compare("unknown error") == 0) {
        return "";
    } else {
        return err;
    }
}

int MadDatabase::execInternal(string const &sql) {
    char *errorMessage = 0;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMessage);
    if (rc == SQLITE_OK) {
        cout << "exec: " << sql << " " << rc << endl;
    } else {
        cout << "exec: " << sql << " " << rc << " " << errorMessage << endl;
        sqlite3_free(errorMessage);
    }
    return sqlite3_changes(db);
}

bool MadDatabase::insert(string const &table, MadContentValues &values) {
    if (values.isEmpty()) {
        return false;
    }
    /*
     * INSERT INTO [table] ([row1], [row2]) VALUES (0,"value");
     * INSERT INTO [table] ([?], [?]) VALUES (?,?);
     */
    string sql = "INSERT INTO [" + table + "] (";
    string bindings = " VALUES (";
    auto keys = values.keys();
    for (auto key: keys) {
        sql += "[" + key + "] ";

        if (key == keys.back()) {
            sql += ")";
            bindings += "?);";
        } else {
            sql += ",";
            bindings += "?,";
        }
    }
    sql = sql + bindings;
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        cout << "Could not prepare statement: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    for (int i = 0; i < keys.size(); ++i) {
        string key = keys.at((unsigned long) i);
        switch (values.typeForKey(key)) {
            case MadContentValues::NONE: {
                break;
            }
            case MadContentValues::INT: {
                if (sqlite3_bind_int64(stmt, i + 1, values.getAsInteger(key)) != SQLITE_OK) {
                    cout << "Could not bind statement." << endl;
                    return -1;
                };
                break;
            }
            case MadContentValues::REAL: {
                if (sqlite3_bind_double(stmt, i + 1, values.getAsReal(key)) != SQLITE_OK) {
                    cout << "Could not bind statement." << endl;
                    return -1;
                };
                break;
            }
            case MadContentValues::TEXT: {
                const string &text = values.getAsText(key);
                if (sqlite3_bind_text(stmt, i + 1, text.c_str(), (int) text.length(), SQLITE_TRANSIENT) != SQLITE_OK) {
                    cout << "Could not bind statement." << endl;
                    return -1;
                };
                break;
            }
            case MadContentValues::BLOB: {
                const vector<byte> vector = values.getAsBlob(key);
                if (sqlite3_bind_blob(stmt, i + 1, vector.data(), (int) vector.size(), SQLITE_TRANSIENT) != SQLITE_OK) {
                    cout << "Could not bind statement." << endl;
                    return -1;
                };
                break;
            }
        }
    }

    // sqlite3_reset(stmt);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        cout << "Could not step (execute) stmt." << endl;
        return false;
    }

    return true;
}

MadQuery MadDatabase::query(string const &sql, vector<string> const &args) {
    sqlite3_stmt *stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        cout << "Could not prepare statement: " << sqlite3_errmsg(db) << endl;
    }
    for (int i = 0; i < args.size(); ++i) {
        const string &str = args.at((unsigned long) i);
        rc = sqlite3_bind_text(stmt, i + 1, str.c_str(), (int) str.length(), SQLITE_TRANSIENT);
        if (rc != SQLITE_OK) {
            cout << "Could not bind text: " << str << endl;
        }
    }
    return MadQuery(stmt);
}

MadQuery MadDatabase::query(string const &sql) {
    return query(sql, {});
}

//endregion

//region Private Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//endregion
