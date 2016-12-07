//
// Created by William Kamp on 10/11/16.
//

#ifndef PROJECT_CONTENT_VALUES_H
#define PROJECT_CONTENT_VALUES_H


#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "MadConstants.hpp"
#include "sqlite3.h"

namespace madsqlite {

    class MadContentValues {

//region Structures ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    public:
        enum DataType {
            NONE,
            INT,
            REAL,
            TEXT,
            BLOB,
        };

    private:

        struct Data {
            DataType dataType;
            std::vector<madsqlite::byte> dataBlob;
            double dataReal;
            sqlite3_int64 dataInt;
            std::string dataText;

            Data() {};

            Data(const std::vector<madsqlite::byte> &dataBlob) : dataBlob(dataBlob) {
                dataType = BLOB;
            };

            Data(const double dataReal) : dataReal(dataReal) {
                dataType = REAL;
            };

            Data(const sqlite3_int64 dataInt) : dataInt(dataInt) {
                dataType = INT;
            };

            Data(const std::string &dataText) : dataText(dataText) {
                dataType = TEXT;
            };
        };

//endregion

//region Members ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    private:
        std::unordered_set<std::string> _keys;
        std::vector<Data> _values;
        std::unordered_map<std::string, long> _dataMap;

//endregion

//region Constructor ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    public:

        MadContentValues();

        virtual ~MadContentValues();

//endregion

//region Public Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    public:

        const std::unordered_set<std::string> &keySet() const;

        const std::vector<std::string> keys() const;

        bool isEmpty();

        bool containsKey(std::string const &key);

        DataType typeForKey(std::string const &key);

        void clear();

        sqlite3_int64 getAsInteger(std::string const &key);

        double getAsReal(std::string const &key);

        std::string getAsText(std::string const &key);

        std::vector<madsqlite::byte> getAsBlob(std::string const &key);

        void putInteger(std::string const &key, sqlite3_int64 value);

        void putReal(std::string const &key, double value);

        void putString(std::string const &key, std::string const &value);

        void putBlob(std::string const &key, std::vector<madsqlite::byte> &blob);

        void putBlob(std::string const &key, const void *blob, size_t sz);

//endregion

//region Private Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    private:

        void putData(std::string const &key, Data &data);

        Data getData(std::string const &key);

        double stringToDouble(std::string const &str);

        sqlite3_int64 stringToInt(std::string const &str);

        template<typename T>
        std::string numberToString(T number);

//endregion

    };

}

#endif //PROJECT_CONTENT_VALUES_H
