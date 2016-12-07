//
// Created by William Kamp on 10/9/16.
//

#include "MadDatabase.hpp"
#include "MadUtil.hpp"
#include <iostream>

using namespace madsqlite;
using namespace std;

static mutex databaseMutex;
static unordered_map<string, weak_ptr<MadDatabase>> databaseSet = unordered_map<string, weak_ptr<MadDatabase>>();


class MadDatabase::Impl {

public:
    Impl(string const &dbPath);
    virtual ~Impl();

    // Members
    sqlite3 *db;
    bool isInTransaction = false;
    const std::unordered_set<std::string> transactionKeyWords = {"BEGIN", "COMMIT", "ROLLBACK"};

    // Methods
    int execInternal(std::string const &sql);

};

//region Class Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//endregion

//region Constructor ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

MadDatabase::Impl::Impl(string const &dbPath) {
    sqlite3_open(dbPath.c_str(), &db);
}

MadDatabase::Impl::~Impl() {
    sqlite3_close(db);
}

MadDatabase::MadDatabase() : internals(new Impl(":memory:")) {}

MadDatabase::MadDatabase(string const &dbPath) : internals(new Impl(dbPath)) {}

MadDatabase::~MadDatabase() {
    lock_guard<mutex> guard(databaseMutex);
    for (auto itr = databaseSet.begin(); itr != databaseSet.end(); ++itr) {
        if (itr->second.expired()) {
            databaseSet.erase(itr);
        }
    }
}

std::shared_ptr<MadDatabase> MadDatabase::openInMemoryDatabase() {
    return make_shared<MadDatabase>();
}

std::shared_ptr<MadDatabase> MadDatabase::openDatabase(string const &dbPath) {
    lock_guard<mutex> guard(databaseMutex);

    const string absPath = getAbsoluteFilePath(dbPath);
    auto itr = databaseSet.find(absPath);
    if (itr != databaseSet.end()) {
        if (itr->second.expired()) {
            databaseSet.erase(itr);
        } else {
            return itr->second.lock();
        }
    }

    auto ptr = make_shared<MadDatabase>(absPath);
    databaseSet.emplace(absPath, ptr);
    return move(ptr);
}

//endregion

//region Public Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void MadDatabase::beginTransaction() {
    if (!internals->isInTransaction) {
        internals->execInternal("BEGIN");
        internals->isInTransaction = true;
    }
}

void MadDatabase::rollbackTransaction() {
    if (internals->isInTransaction) {
        internals->execInternal("ROLLBACK");
        internals->isInTransaction = false;
    }
}

void MadDatabase::commitTransaction() {
    if (internals->isInTransaction) {
        internals->execInternal("COMMIT");
        internals->isInTransaction = false;
    }
}

int MadDatabase::exec(string const &sql) {
    string upper = upperCaseString(sql);
    if (internals->transactionKeyWords.find(upper) != internals->transactionKeyWords.end()) {
        return 0;
    }
    return internals->execInternal(sql);
}

string MadDatabase::getError() {
    lock_guard<mutex> guard(databaseMutex);
    auto err = string(sqlite3_errmsg(internals->db));
    if (err.compare("not an error") == 0 || err.compare("unknown error") == 0) {
        return "";
    } else {
        return err;
    }
}

int MadDatabase::Impl::execInternal(std::string const &sql) {
    lock_guard<mutex> guard(databaseMutex);
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

    lock_guard<mutex> guard(databaseMutex);
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
    int rc = sqlite3_prepare_v2(internals->db, sql.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        cout << "Could not prepare statement: " << sqlite3_errmsg(internals->db) << endl;
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
    lock_guard<mutex> guard(databaseMutex);
    sqlite3_stmt *stmt = nullptr;
    int rc = sqlite3_prepare_v2(internals->db, sql.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        cout << "Could not prepare statement: " << sqlite3_errmsg(internals->db) << endl;
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
