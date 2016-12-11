//
// Created by William Kamp on 12/8/16.
//

#ifndef PROJECT_MADCONTENTVALUEP_H
#define PROJECT_MADCONTENTVALUEP_H

#include <string>
#include <vector>

namespace madsqlite {

/**
 * Key values container useful for the insertion of data into a MadDatabase.
 */
class MadContentValues {

private:

    friend class MadDatabase;

    class Impl;

    std::shared_ptr<Impl> impl;

public:

    /**
     * Create a new content value container instance useful for the insertion of data into a MadDatabase.
     */
    MadContentValues();

    virtual ~MadContentValues();

    /**
     * Adds a value to the set.
     *
     * @param key the name of the value to put.
     * @param value the data for the value to put.
     */
    void putInteger(std::string const &key, long long int value);

    /**
     * Adds a value to the set.
     *
     * @param key the name of the value to put.
     * @param value the data for the value to put.
     */
    void putReal(std::string const &key, double value);

    /**
     * Adds a value to the set.
     *
     * @param key the name of the value to put.
     * @param value the data for the value to put.
     */
    void putString(std::string const &key, std::string const &value);

    /**
     * Adds a value to the set.
     *
     * @param key the name of the value to put.
     * @param value the data for the value to put.
     */
    void putBlob(std::string const &key, std::vector<unsigned char> &value);

    /**
     * Adds a value to the set.
     *
     * @param key the name of the value to put.
     * @param value the data for the value to put.
     * @param sz the size of the value.
     */
    void putBlob(std::string const &key, const void *value, size_t sz);

    /**
     * Removes all values.
     */
    void clear();
};

}

#endif //PROJECT_MADCONTENTVALUEP_H
