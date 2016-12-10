//
// Created by William Kamp on 12/10/16.
//

#ifndef PROJECT_MADQUERYIMPL_HPP
#define PROJECT_MADQUERYIMPL_HPP

#include "sqlite3.h"
#include "MadQuery.hpp"

namespace madsqlite {

class MadQuery::Impl {

//region Members

private:

    sqlite3_stmt *statement;
    int position = -1;
    int stepResult = -1;

//endregion

//region Constructor

public:

    Impl(sqlite3_stmt *statement);

    Impl(Impl &&other);

    Impl(Impl &other) = delete; // disallow copy

    virtual ~Impl();

//endregion

//region Methods

    bool moveToFirst();

    bool moveToNext();

    bool isAfterLast();

    const std::string getString(int columnIndex) const;

    const std::vector<unsigned char> getBlob(int columnIndex) const;

    long long int getInt(int columnIndex);

    double getReal(int columnIndex);

//endregion

};
}
#endif //PROJECT_MADQUERYIMPL_HPP
