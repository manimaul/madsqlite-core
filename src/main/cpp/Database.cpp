//
// Created by William Kamp on 10/9/16.
//

#include <iostream>
#include "Database.hpp"
#include "Util.cpp"

//region Class Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//endregion

//region Constructor ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Database::Database() {
    sqlite3_open(":memory:", &db);
}

Database::Database(std::string const &dbPath) {
    sqlite3_open(dbPath.c_str(), &db);
}

Database::~Database() {
    sqlite3_close(db);
}

//endregion

//region Public Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void Database::beginTransaction() {
    if (!isInTransaction) {
        execInternal("BEGIN");
        isInTransaction = true;
    }
}

void Database::rollbackTransaction() {
    if (isInTransaction) {
        execInternal("ROLLBACK");
        isInTransaction = false;
    }
}

void Database::commitTransaction() {
    if (isInTransaction) {
        execInternal("COMMIT");
        isInTransaction = false;
    }
}

int Database::exec(std::string const &sql) {
    std::string upper = upperCaseString(sql);
    if (transactionKeyWords.find(upper) != transactionKeyWords.end()) {
        return 0;
    }
    return execInternal(sql);
}

std::string Database::getError() {
    auto err = std::string(sqlite3_errmsg(db));
    if (err.compare("not an error") == 0 || err.compare("unknown error") == 0) {
        return "";
    } else {
        return err;
    }
}

int Database::execInternal(std::string const &sql) {
    char *errorMessage = 0;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMessage);
    if (rc == SQLITE_OK) {
        std::cout << "exec: " << sql << " " << rc << std::endl;
    } else {
        std::cout << "exec: " << sql << " " << rc << " " << errorMessage << std::endl;
        sqlite3_free(errorMessage);
    }
    return sqlite3_changes(db);
}

bool Database::insert(std::string const &table, ContentValues &values) {
    if (values.isEmpty()) {
        return false;
    }
    /*
     * INSERT INTO [table] ([row1], [row2]) VALUES (0,"value");
     * INSERT INTO [table] ([?], [?]) VALUES (?,?);
     */
    std::string sql = "INSERT INTO [" + table + "] (";
    std::string bindings = " VALUES (";
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
        std::cout << "Could not prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    for (int i = 0; i < keys.size(); ++i) {
        std::string key = keys.at((unsigned long) i);
        switch (values.typeForKey(key)) {
            case ContentValues::NONE: {
                break;
            }
            case ContentValues::INT: {
                if (sqlite3_bind_int64(stmt, i + 1, values.getAsInteger(key)) != SQLITE_OK) {
                    std::cout << "Could not bind statement." << std::endl;
                    return -1;
                };
                break;
            }
            case ContentValues::REAL: {
                if (sqlite3_bind_double(stmt, i + 1, values.getAsReal(key)) != SQLITE_OK) {
                    std::cout << "Could not bind statement." << std::endl;
                    return -1;
                };
                break;
            }
            case ContentValues::TEXT: {
                const std::string &text = values.getAsText(key);
                if (sqlite3_bind_text(stmt, i + 1, text.c_str(), (int) text.length(), SQLITE_TRANSIENT) != SQLITE_OK) {
                    std::cout << "Could not bind statement." << std::endl;
                    return -1;
                };
                break;
            }
            case ContentValues::BLOB: {
                const std::vector<byte> vector = values.getAsBlob(key);
                if (sqlite3_bind_blob(stmt, i + 1, vector.data(), (int) vector.size(), SQLITE_TRANSIENT) != SQLITE_OK) {
                    std::cout << "Could not bind statement." << std::endl;
                    return -1;
                };
                break;
            }
        }
    }

    // sqlite3_reset(stmt);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cout << "Could not step (execute) stmt." << std::endl;
        return false;
    }

    return true;
}

Cursor Database::query(std::string const &sql, std::vector<std::string> const &args) {
    sqlite3_stmt *stmt = nullptr;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        std::cout << "Could not prepare statement: " << sqlite3_errmsg(db) << std::endl;
    }
    for (int i = 0; i < args.size(); ++i) {
        const std::string &str = args.at((unsigned long) i);
        rc = sqlite3_bind_text(stmt, i + 1, str.c_str(), (int) str.length(), SQLITE_TRANSIENT);
        if (rc != SQLITE_OK) {
            std::cout << "Could not bind text: " << str << std::endl;
        }
    }
    return Cursor(stmt);
}

Cursor Database::query(std::string const &sql) {
    return query(sql, {});
}

//endregion

//region Private Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//endregion
