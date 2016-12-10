//
// Created by William Kamp on 12/10/16.
//

#ifndef PROJECT_MADDATABASEIMPL_HPP
#define PROJECT_MADDATABASEIMPL_HPP

#include "sqlite3.h"
#include "MadQuery.hpp"
#include "MadContentValues.hpp"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace madsqlite {

class MadDatabase::Impl {

//region Constructor

public:

    friend class MadDatabase;

    Impl();

    Impl(std::string const &dbPath);

    virtual ~Impl();

//endregion

//region Members

private:
    sqlite3 *db;
    bool isInTransaction = false;
    const std::unordered_set<std::string> transactionKeyWords = {"BEGIN", "COMMIT", "ROLLBACK"};

//endregion

//region Methods

public:

    std::string getError(bool doLock);

private:

    int execInternal(std::string const &sql);

    bool insert(std::string const &table, MadContentValues &contentValues);

    MadQuery query(std::string const &sql, std::vector<std::string> const &args);

    int exec(std::string const &sql);

    void beginTransaction();

    void rollbackTransaction();

    void commitTransaction();

//endregion

};
}
#endif //PROJECT_MADDATABASEIMPL_HPP
