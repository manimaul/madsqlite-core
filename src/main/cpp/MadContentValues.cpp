//
// Created by William Kamp on 10/11/16.
//

#include <iostream>
#include <sstream>
#include "../api/MadContentValues.hpp"

#ifdef ANDROID
#include <arpa/inet.h>
#endif

using namespace madsqlite;
using namespace std;

//region Class Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//endregion

//region Constructor ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


MadContentValues::MadContentValues() {}

MadContentValues::~MadContentValues() {}

//endregion

//region Public Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const unordered_set<string> &MadContentValues::keySet() const {
    return _keys;
}

const vector<string> MadContentValues::keys() const {
    return vector<string>(_keys.begin(), _keys.end());
}

bool MadContentValues::isEmpty() {
    return _keys.size() <= 0;
}

bool MadContentValues::containsKey(string const &key) {
    return _keys.find(key) != _keys.end();
}

MadContentValues::DataType MadContentValues::typeForKey(string const &key) {
    if (containsKey(key)) {
        return getData(key).dataType;
    }
    return NONE;
}

void MadContentValues::clear() {
    _keys.clear();
    _values.clear();
    _dataMap.clear();
}

sqlite3_int64 MadContentValues::getAsInteger(string const &key) {
    if (containsKey(key)) {
        const Data &data = getData(key);
        switch (data.dataType) {
            case INT:
                return data.dataInt;
            case REAL:
                return (sqlite3_int64) data.dataReal;
            case TEXT: {
#ifdef ANDROID
                return stringToInt(data.dataText);
#else
                return stoi(data.dataText);
#endif
            }
            case BLOB:
                return ntohs(*reinterpret_cast<const sqlite3_int64 *>(&data.dataBlob[0]));
            case NONE:
            default:
                break;
        }
    }
    return 0;
}

double MadContentValues::getAsReal(string const &key) {
    if (containsKey(key)) {
        const Data &data = getData(key);
        switch (data.dataType) {
            case INT:
                return data.dataInt;
            case REAL:
                return data.dataReal;
            case TEXT: {
#ifdef ANDROID
                return stringToDouble(data.dataText);
#else
                return stod(data.dataText);
#endif
            }
            case BLOB:
                return ntohs(*reinterpret_cast<const double *>(&data.dataBlob[0]));
            case NONE:
            default:
                break;
        }
    }
    return 0;
}

string MadContentValues::getAsText(string const &key) {
    if (containsKey(key)) {
        const Data &data = getData(key);
        switch (data.dataType) {
            case INT:
#ifdef ANDROID
                return numberToString(data.dataInt);
#else
                return to_string(data.dataInt);
#endif
            case REAL:
#ifdef ANDROID
                return numberToString(data.dataReal);
#else
                return to_string(data.dataReal);
#endif
            case TEXT:
                return data.dataText;
            case BLOB:
                return string(data.dataBlob.begin(), data.dataBlob.end());
            case NONE:
            default:
                break;
        }
    }
    return string();
}

vector<byte> MadContentValues::getAsBlob(string const &key) {
    if (containsKey(key)) {
        return getData(key).dataBlob;
    }
    return vector<byte>();
}

void MadContentValues::putInteger(string const &key, sqlite3_int64 value) {
    Data d = {value};
    putData(key, d);
}

void MadContentValues::putReal(string const &key, double value) {
    Data d = {value};
    putData(key, d);
}

void MadContentValues::putString(string const &key, string const &value) {
    Data d = {value};
    putData(key, d);
}

void MadContentValues::putBlob(string const &key, vector<byte> &value) {
    Data d = {value};
    putData(key, d);
}

void MadContentValues::putBlob(string const &key, const void *blob, size_t sz) {
    byte *charBuf = (byte *) blob;
    vector<byte> value(charBuf, charBuf + sz);
    Data d = {value};
    putData(key, d);
}

//endregion

//region Private Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


void MadContentValues::putData(string const &key, MadContentValues::Data &data) {
    if (containsKey(key)) {
        long i = _dataMap.at(key);
        _values[i] = data;
    } else {
        long i = _values.size();
        _values.emplace_back(data);
        _dataMap.emplace(key, i);
    }
    _keys.emplace(key);
}

MadContentValues::Data MadContentValues::getData(string const &key) {
    long i = _dataMap.at(key);
    return _values[i];
}

double MadContentValues::stringToDouble(string const &str) {
    stringstream ss(str);
    double result;
    return ss >> result ? result : 0;
}

sqlite3_int64 MadContentValues::stringToInt(string const &str) {
    stringstream ss(str);
    sqlite3_int64 result;
    return ss >> result ? result : 0;
}

template<typename T>
string
MadContentValues::numberToString(T number) {
    stringstream ss;
    ss << number;
    return ss.str();
}

//endregion
