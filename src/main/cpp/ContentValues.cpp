//
// Created by William Kamp on 10/11/16.
//

#include <iostream>
#include <sstream>
#include "ContentValues.hpp"

#ifdef ANDROID
#include <arpa/inet.h>
#endif

const std::unordered_set<std::string> &ContentValues::keySet() const {
    return _keys;
}

const std::vector<std::string> ContentValues::keys() const {
    return std::vector<std::string>(_keys.begin(), _keys.end());
}

bool ContentValues::containsKey(std::string const &key) {
    return _keys.find(key) != _keys.end();
}

void ContentValues::putData(std::string const &key, ContentValues::Data &data) {
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

ContentValues::Data ContentValues::getData(std::string const &key) {
    long i = _dataMap.at(key);
    return _values[i];
}

sqlite3_int64 ContentValues::getAsInteger(std::string const &key) {
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

double ContentValues::getAsReal(std::string const &key) {
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

std::string ContentValues::getAsText(std::string const &key) {
    if (containsKey(key)) {
        const Data &data = getData(key);
        switch (data.dataType) {
            case INT:
#ifdef ANDROID
                return numberToString(data.dataInt);
#else
                return std::to_string(data.dataInt);
#endif
            case REAL:
#ifdef ANDROID
                return numberToString(data.dataReal);
#else
                return std::to_string(data.dataReal);
#endif
            case TEXT:
                return data.dataText;
            case BLOB:
                return std::string(data.dataBlob.begin(), data.dataBlob.end());
            case NONE:
            default:
                break;
        }
    }
    return std::string();
}

std::vector<byte> ContentValues::getAsBlob(std::string const &key) {
    if (containsKey(key)) {
        return getData(key).dataBlob;
    }
    return std::vector<byte>();
}

void ContentValues::putInteger(std::string const &key, sqlite3_int64 value) {
    Data d = {value};
    putData(key, d);
}

void ContentValues::putReal(std::string const &key, double value) {
    Data d = {value};
    putData(key, d);
}

void ContentValues::putString(std::string const &key, std::string const &value) {
    Data d = {value};
    putData(key, d);
}

void ContentValues::putBlob(std::string const &key, std::vector<byte> &value) {
    Data d = {value};
    putData(key, d);
}

bool ContentValues::isEmpty() {
    return _keys.size() <= 0;
}

ContentValues::DataType ContentValues::typeForKey(std::string const &key) {
    if (containsKey(key)) {
        return getData(key).dataType;
    }
    return NONE;
}

void ContentValues::clear() {
    _keys.clear();
    _values.clear();
    _dataMap.clear();
}

void ContentValues::putBlob(std::string const &key, void *blob, size_t sz) {
    byte *charBuf = (byte*)blob;
    std::vector<byte> value(charBuf, charBuf + sz);
    Data d = {value};
    putData(key, d);
}

double ContentValues::stringToDouble(std::string const &str){
    std::stringstream ss(str);
    double result;
    return ss >> result ? result : 0;
}

sqlite3_int64 ContentValues::stringToInt(std::string const &str){
    std::stringstream ss(str);
    sqlite3_int64 result;
    return ss >> result ? result : 0;
}

template <typename T> std::string
ContentValues::numberToString(T number) {
    std::stringstream ss;
    ss << number;
    return ss.str();
}




