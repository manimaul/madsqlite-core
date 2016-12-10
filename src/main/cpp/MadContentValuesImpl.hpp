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
#include "MadContentValues.hpp"
#include "MadSqlType.hpp"
#include "sqlite3.h"

namespace madsqlite {

class MadContentValuesImpl {

public:

    struct Data {
        SqlDataType dataType;
        std::vector<madsqlite::byte> dataBlob;
        double dataReal;
        long long int dataInt;
        std::string dataText;

        Data() {
        };

        Data(const std::vector<madsqlite::byte> &dataBlob) : dataBlob(dataBlob) {
            dataType = SqlDataType::BLOB;
        };

        Data(const double dataReal) : dataReal(dataReal) {
            dataType = SqlDataType::REAL;
        };

        Data(const sqlite3_int64 dataInt) : dataInt(dataInt) {
            dataType = SqlDataType::INT;
        };

        Data(const std::string &dataText) : dataText(dataText) {
            dataType = SqlDataType::TEXT;
        };
    };

//region Members

public:

    std::unordered_set<std::string> _keys;
    std::vector<Data> _values;
    std::unordered_map<std::string, long> _dataMap;

//endregion

//region Constructor

public:

    MadContentValuesImpl();

    virtual ~MadContentValuesImpl();

//endregion

//region Methods

public:

    const std::vector<std::string> keys() const;

    bool isEmpty();

    bool containsKey(std::string const &key);

    SqlDataType typeForKey(std::string const &key);

    void clear();

    long long int getAsInteger(std::string const &key);

    double getAsReal(std::string const &key);

    std::string getAsText(std::string const &key);

    std::vector<madsqlite::byte> getAsBlob(std::string const &key);

    void putData(std::string const &key, Data &data);

    Data getData(std::string const &key);

    double stringToDouble(std::string const &str);

    long long int stringToInt(std::string const &str);

    template<typename T>
    std::string numberToString(T number);

//endregion

};

}

#endif //PROJECT_CONTENT_VALUES_H