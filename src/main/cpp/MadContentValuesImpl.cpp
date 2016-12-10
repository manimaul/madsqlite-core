//
// Created by William Kamp on 10/11/16.
//

#include <iostream>
#include <sstream>
#include <MadContentValuesImpl.hpp>

#ifdef ANDROID
#include <arpa/inet.h>
#endif

using namespace madsqlite;
using namespace std;

//region MadContentValues Constructor

MadContentValues::MadContentValues() : impl(new MadContentValuesImpl()) {}

//endregion

//region MadContentValuesImpl Constructor

MadContentValuesImpl::MadContentValuesImpl() {}

MadContentValuesImpl::~MadContentValuesImpl() {}

//endregion

//region MadContentValues Methods

void MadContentValues::clear() {
    impl->clear();
}

void MadContentValues::putInteger(string const &key, sqlite3_int64 value) {
    MadContentValuesImpl::Data d = {value};
    impl->putData(key, d);
}

void MadContentValues::putReal(string const &key, double value) {
    MadContentValuesImpl::Data d = {value};
    impl->putData(key, d);
}

void MadContentValues::putString(string const &key, string const &value) {
    MadContentValuesImpl::Data d = {value};
    impl->putData(key, d);
}

void MadContentValues::putBlob(string const &key, vector<byte> &value) {
    MadContentValuesImpl::Data d = {value};
    impl->putData(key, d);
}

void MadContentValues::putBlob(string const &key, const void *blob, size_t sz) {
    byte *charBuf = (byte *) blob;
    vector<byte> value(charBuf, charBuf + sz);
    MadContentValuesImpl::Data d = {value};
    impl->putData(key, d);
}

//endregion

//region MadContentValuesImpl Methods

const vector<string> MadContentValuesImpl::keys() const {
    return vector<string>(_keys.begin(), _keys.end());
}

bool MadContentValuesImpl::isEmpty() {
    return _keys.size() <= 0;
}

bool MadContentValuesImpl::containsKey(string const &key) {
    return _keys.find(key) != _keys.end();
}

SqlDataType MadContentValuesImpl::typeForKey(string const &key) {
    if (containsKey(key)) {
        return getData(key).dataType;
    }
    return SqlDataType::NONE;
}

void MadContentValuesImpl::clear() {
    _keys.clear();
    _values.clear();
    _dataMap.clear();
}

long long int MadContentValuesImpl::getAsInteger(string const &key) {
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

double MadContentValuesImpl::getAsReal(string const &key) {
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

string MadContentValuesImpl::getAsText(string const &key) {
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

vector<byte> MadContentValuesImpl::getAsBlob(string const &key) {
    if (containsKey(key)) {
        return getData(key).dataBlob;
    }
    return vector<byte>();
}

void MadContentValuesImpl::putData(string const &key, MadContentValuesImpl::Data &data) {
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

MadContentValuesImpl::Data MadContentValuesImpl::getData(string const &key) {
    long i = _dataMap.at(key);
    return _values[i];
}

double MadContentValuesImpl::stringToDouble(string const &str) {
    stringstream ss(str);
    double result;
    return ss >> result ? result : 0;
}

long long int MadContentValuesImpl::stringToInt(string const &str) {
    stringstream ss(str);
    long long int result;
    return ss >> result ? result : 0;
}

template<typename T>
string
MadContentValuesImpl::numberToString(T number) {
    stringstream ss;
    ss << number;
    return ss.str();
}

//endregion
