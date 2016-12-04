//
// Created by William Kamp on 10/9/16.
//

#ifndef MADSQLITE_DATABASE_H
#define MADSQLITE_DATABASE_H


#include "sqlite3.h"
#include "MadQuery.hpp"
#include "MadConstants.hpp"
#include "MadContentValues.hpp"
#include <string>
#include <unordered_map>

namespace madsqlite {

    class MadDatabase {

//region Members ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    private:

        sqlite3 *db;
        bool isInTransaction = false;

        int execInternal(std::string const &sql);

        const std::unordered_set<std::string> transactionKeyWords = {"BEGIN", "COMMIT", "ROLLBACK"};

//endregion

//region Constructor ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    public:

        /**
         * Creates an in memory database an in memory database.
         */
        MadDatabase();

        /**
         * Creates a database.
         * @param dbPath the absolute path of the database to open / create.
         */
        MadDatabase(std::string const &dbPath);

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
        bool insert(std::string const &table, MadContentValues &values);

        /**
         * Execute a sql query
         *
         * @param sql the query
         * @return a MadQuery to retrive query results
         */
        MadQuery query(std::string const &sql);

        /**
         * Execute a sql query
         *
         * @param sql the query
         * @param args query arguments
         * @return a MadQuery to retrive query results
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

        void beginTransaction();

        void rollbackTransaction();

        void commitTransaction();

//endregion

//region Private Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    private:

//endregion

    };
}
#endif //MADSQLITE_DATABASE_H
