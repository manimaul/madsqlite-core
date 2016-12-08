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

    bool moveToFirst();

    bool moveToNext();

    bool isAfterLast();

    const std::string getString(int columnIndex) const;

    const std::vector<byte> getBlob(int columnIndex) const;

    sqlite3_int64 getInt(int columnIndex);

    double getReal(int columnIndex);

//endregion

//region Private Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

private:

//endregion

};

}

#endif //PROJECT_CURSOR_H
