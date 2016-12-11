//
// Created by William Kamp on 10/10/16.
//

#ifndef PROJECT_CURSOR_H
#define PROJECT_CURSOR_H

#include <string>
#include <vector>

namespace madsqlite {

/**
 * Provides read access to the result set returned by a sqlite database query.
 */
class MadQuery {

private:

    friend class MadDatabase;

    class Impl;

    std::unique_ptr<Impl> impl;

public:

    /**
     * For internal use.
     */
    MadQuery(std::unique_ptr<Impl> impl);

    /**
     * Copying a query is disallowed.
     */
    MadQuery(MadQuery &other) = delete;

    /**
     * Moving a query is allowed.
     *
     * @param other the query to move.
     */
    MadQuery(MadQuery &&other);

    virtual ~MadQuery();

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
    const std::vector<unsigned char> getBlob(int columnIndex) const;

    /**
     * @param columnIndex the zero-based index of the target column.
     * @return the value of that column a long long integer.
     */
    long long int getInt(int columnIndex);

    /**
     * @param columnIndex the zero-based index of the target column.
     * @return the value of that column as a double.
     */
    double getReal(int columnIndex);

};
}

#endif //PROJECT_CURSOR_H
