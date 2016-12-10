//
// Created by William Kamp on 10/9/16.
//

#ifndef MADSQLITE_DATABASE_H
#define MADSQLITE_DATABASE_H

#include "MadQuery.hpp"
#include "MadContentValues.hpp"
#include <string>
#include <vector>
#include <memory>

namespace madsqlite {

class MadDatabaseImpl;

/**
 * Opens, creates and provides access to a sqlite database.
 */
class MadDatabase {

private:

    class Impl;

    std::unique_ptr<Impl> impl;

public:

    /**
     * Opens a database.
     * @param dbPath the absolute path of the database to open / create.
     */
    static std::shared_ptr<MadDatabase> openDatabase(std::string const &dbPath);

    /**
     * Opens an in memory database an in memory database.
     */
    static std::unique_ptr<MadDatabase> openInMemoryDatabase();

    /**
     * For internal use.
     * See MadDatabase::openDatabase
     */
    MadDatabase(std::unique_ptr<Impl> impl);

    virtual ~MadDatabase();

//endregion

//region Public Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

public:

    /**
     * Insert values into a table
     *
     * @param table the table to insert into
     * @param values the values to insert
     * @return if the insertion was successful
     */
    bool insert(std::string const &table, MadContentValues &contentValues);

    /**
     * Execute a sql query
     *
     * @param sql the query
     * @return a MadQuery to retrieve query results
     */
    MadQuery query(std::string const &sql);

    /**
     * Execute a sql query
     *
     * @param sql the query
     * @param args query arguments
     * @return a MadQuery to retrieve query results
     */
    MadQuery query(std::string const &sql, std::vector<std::string> const &args);

    /**
     * Execute a sql statement.
     *
     * @param sql a statement to execute
     * @return the number of rows modified by those SQL statements (INSERT, UPDATE or DELETE only)
     */
    int exec(std::string const &sql);

    /**
     * @return the most recent database API call error message.
     */
    std::string getError();

    /**
     * Begins a transaction. The changes will be rolled back if any transaction performed without being commited.
     */
    void beginTransaction();

    /**
     * Rolls back a begun transaction.
     */
    void rollbackTransaction();

    /**
     * Commits a previously begun transaction.
     */
    void commitTransaction();

};
}
#endif //MADSQLITE_DATABASE_H
