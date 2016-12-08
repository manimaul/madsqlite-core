//
// Created by William Kamp on 10/10/16.
//

#ifndef PROJECT_CURSOR_H
#define PROJECT_CURSOR_H

#include <string>
#include <vector>
#include "MadConstants.hpp"
#include "sqlite3.h"

namespace madsqlite {

class MadQuery {

//region Members ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

private:

    sqlite3_stmt *statement;
    int position = -1;
    int stepResult = -1;

//endregion

//region Constructor ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

public:

    MadQuery(sqlite3_stmt *statement);

    MadQuery(MadQuery &&query);

    MadQuery(MadQuery &query) = delete; // disallow copy
    virtual ~MadQuery();

//endregion

//region Public Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

public:

    /**
     * Move the query to the first row.
     * @return false if the query is empty.
     */
    bool moveToFirst();

    /**
     * Move the query to the next row.
     * @return false if the query is already past the last entry in the result set.
     */
    bool moveToNext();

    /**
     * @return Returns whether the query is pointing to the position after the last row.
     */
    bool isAfterLast();

    /**
     * @param columnIndex the zero-based index of the target column.
     * @return the value of that column as a String.
     */
    const std::string getString(int columnIndex) const;

    /**
     * @param columnIndex the zero-based index of the target column.
     * @return the value of that column as data.
     */
    const std::vector<byte> getBlob(int columnIndex) const;

    /**
     * @param columnIndex the zero-based index of the target column.
     * @return the value of that column a long long integer.
     */
    sqlite3_int64 getInt(int columnIndex);

    /**
     * @param columnIndex the zero-based index of the target column.
     * @return the value of that column as a double.
     */
    double getReal(int columnIndex);

//endregion

//region Private Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

private:

//endregion

};

}

#endif //PROJECT_CURSOR_H
