//
// Created by William Kamp on 12/7/16.
//
#include <iostream>
#include "gtest/gtest.h"
#include "MadDatabase.hpp"
#include <math.h>
#include <cstdio>
#include <thread>
#include <cstdlib>

using namespace madsqlite;
using namespace std;

class TestData {
public:
    int const size = 100;
    vector<string> data;

    TestData() {
        for (int i = 0; i < size; ++i) {
            data.push_back(randomString());
        }
    }

    string dataAt(int index) {
        return data.at((unsigned long) index);
    }

    string randomString() {
        static const char alphanum[] = "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz";
        int len = 10;
        char strArr[len + 1];
        for (int i = 0; i < len - 1; ++i) {
            strArr[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
        }
        strArr[len - 1] = '\n';
        return string(strArr);
    }
};

static TestData testData = TestData();

TEST(MadDatabaseTests, Async) {
    string dbFileName = "test_db.s3db";
    remove(dbFileName.c_str());

    auto db = MadDatabase::openDatabase(dbFileName);
    db->exec("CREATE TABLE test (keyText TEXT, keyIdx INTEGER);");
    EXPECT_EQ("", db->getError());

    auto threads = vector<thread>();

    for (int i = 0; i < testData.size; ++i) {
        threads.push_back(thread([=]() {
            auto dbt = MadDatabase::openDatabase(dbFileName);
            EXPECT_EQ("", db->getError());

            auto cv = MadContentValues();
            auto s = testData.dataAt(i);
            cv.putString("keyText", s);
            cv.putInteger("keyIdx", i);
            dbt->insert("test", cv);
            EXPECT_EQ("", db->getError());

            auto qry = dbt->query("SELECT oid, keyText, keyIdx FROM test");
            EXPECT_EQ("", db->getError());

            EXPECT_TRUE(qry.moveToFirst());
            while (!qry.isAfterLast()) {
                auto id = qry.getInt(0);
                auto val = qry.getString(1);
                auto index = qry.getInt(2);
                EXPECT_EQ(testData.dataAt(index), val);
                qry.moveToNext();
            }
        }));
    }

    for (auto &&t : threads) {
        t.join();
    }

}

TEST(MadDatabaseTests, TransactionRollBack) {
    string dbFileName = "test_trans_db.s3db";
    remove(dbFileName.c_str());

    auto db = MadDatabase::openDatabase(dbFileName);
    db->exec("CREATE TABLE location_table(name TEXT, latitude REAL, longitude REAL, image BLOB);");
    EXPECT_EQ("", db->getError());

    db->beginTransaction();

    auto cv = MadContentValues();

    cv.putString("name", "Cheshire Cat");
    cv.putReal("latitude", 51.2414945);
    cv.putReal("longitude", -0.6354629);
    db->insert("location_table", cv);

    cv.clear();
    cv.putString("name", "Alice");
    cv.putReal("latitude", 51.2414945);
    cv.putReal("longitude", -0.6354629);
    db->insert("location_table", cv);

    db->rollbackTransaction();

    auto query = db->query("SELECT * FROM location_table;");
    EXPECT_EQ("", db->getError());
    EXPECT_FALSE(query.moveToFirst());
    EXPECT_TRUE(query.isAfterLast());
}

TEST(MadDatabaseTests, TransactionCommit) {
    string dbFileName = "test_trans_db.s3db";
    remove(dbFileName.c_str());

    auto db = MadDatabase::openDatabase(dbFileName);
    db->exec("CREATE TABLE location_table(name TEXT, latitude REAL, longitude REAL, image BLOB);");
    EXPECT_EQ("", db->getError());

    db->beginTransaction();

    auto cv = MadContentValues();
    cv.putString("name", "Cheshire Cat");
    cv.putReal("latitude", 51.2414945);
    cv.putReal("longitude", -0.6354629);
    db->insert("location_table", cv);

    db->commitTransaction();

    auto query = db->query("SELECT * FROM location_table;");
    EXPECT_EQ("", db->getError());
    EXPECT_TRUE(query.moveToFirst());
    EXPECT_FALSE(query.isAfterLast());
}

TEST(MadDatabaseTests, Empty) {
    auto db = MadDatabase::openInMemoryDatabase();
    db->exec("CREATE TABLE test(keyInt INTEGER);");
    EXPECT_EQ("", db->getError());

    auto query = db->query("SELECT * FROM test;");
    EXPECT_EQ("", db->getError());
    EXPECT_FALSE(query.moveToFirst());
    EXPECT_TRUE(query.isAfterLast());
    EXPECT_FALSE(query.moveToNext());
}

TEST(MadDatabaseTests, InsertInteger) {
    auto db = MadDatabase::openInMemoryDatabase();
    db->exec("CREATE TABLE test(keyInt INTEGER);");
    EXPECT_EQ("", db->getError());

    auto cv = MadContentValues();
    cv.putInteger("keyInt", INT_MIN);
    EXPECT_TRUE(db->insert("test", cv));
    EXPECT_EQ("", db->getError());

    cv.clear();
    cv.putInteger("keyInt", INT_MAX);
    EXPECT_TRUE(db->insert("test", cv));
    EXPECT_EQ("", db->getError());

    auto query = db->query("SELECT keyInt FROM test;");
    EXPECT_EQ("", db->getError());
    EXPECT_TRUE(query.moveToFirst());
    EXPECT_FALSE(query.isAfterLast());

    auto firstResult = query.getInt(0);
    EXPECT_TRUE(query.moveToNext());
    EXPECT_FALSE(query.isAfterLast());

    auto secondResult = query.getInt(0);
    EXPECT_TRUE(query.moveToNext());
    EXPECT_TRUE(query.isAfterLast());

    EXPECT_EQ(INT_MIN, firstResult);
    EXPECT_EQ(INT_MAX, secondResult);
}

TEST(MadDatabaseTests, InsertLong) {
    auto db = MadDatabase::openInMemoryDatabase();
    db->exec("CREATE TABLE test(keyInt INTEGER);");
    EXPECT_EQ("", db->getError());

    auto cv = MadContentValues();
    cv.putInteger("keyInt", LONG_MIN);
    EXPECT_TRUE(db->insert("test", cv));
    EXPECT_EQ("", db->getError());

    cv.clear();
    cv.putInteger("keyInt", LONG_MAX);
    EXPECT_TRUE(db->insert("test", cv));
    EXPECT_EQ("", db->getError());

    auto query = db->query("SELECT keyInt FROM test;");
    EXPECT_EQ("", db->getError());
    EXPECT_TRUE(query.moveToFirst());
    EXPECT_FALSE(query.isAfterLast());

    auto firstResult = query.getInt(0);
    EXPECT_TRUE(query.moveToNext());
    EXPECT_FALSE(query.isAfterLast());

    auto secondResult = query.getInt(0);
    EXPECT_TRUE(query.moveToNext());
    EXPECT_TRUE(query.isAfterLast());

    EXPECT_EQ(LONG_MIN, firstResult);
    EXPECT_EQ(LONG_MAX, secondResult);
}

TEST(MadDatabaseTests, InsertFloat) {
    auto db = MadDatabase::openInMemoryDatabase();
    db->exec("CREATE TABLE test(keyReal REAL);");
    EXPECT_EQ("", db->getError());

    auto cv = MadContentValues();
    cv.putReal("keyReal", FLT_MIN);
    EXPECT_TRUE(db->insert("test", cv));
    EXPECT_EQ("", db->getError());

    cv.clear();
    cv.putReal("keyReal", FLT_MAX);
    EXPECT_TRUE(db->insert("test", cv));
    EXPECT_EQ("", db->getError());

    auto query = db->query("SELECT keyReal FROM test;");
    EXPECT_EQ("", db->getError());
    EXPECT_TRUE(query.moveToFirst());
    EXPECT_FALSE(query.isAfterLast());

    auto firstResult = query.getReal(0);
    EXPECT_TRUE(query.moveToNext());
    EXPECT_FALSE(query.isAfterLast());

    auto secondResult = query.getReal(0);
    EXPECT_TRUE(query.moveToNext());
    EXPECT_TRUE(query.isAfterLast());

    EXPECT_EQ(FLT_MIN, firstResult);
    EXPECT_EQ(FLT_MAX, secondResult);
}

TEST(MadDatabaseTests, InsertDouble) {
    auto db = MadDatabase::openInMemoryDatabase();
    db->exec("CREATE TABLE test(keyReal REAL);");
    EXPECT_EQ("", db->getError());

    auto cv = MadContentValues();
    cv.putReal("keyReal", DBL_MIN);
    EXPECT_TRUE(db->insert("test", cv));
    EXPECT_EQ("", db->getError());

    cv.clear();
    cv.putReal("keyReal", DBL_MAX);
    EXPECT_TRUE(db->insert("test", cv));
    EXPECT_EQ("", db->getError());

    auto query = db->query("SELECT keyReal FROM test;");
    EXPECT_EQ("", db->getError());
    EXPECT_TRUE(query.moveToFirst());
    EXPECT_FALSE(query.isAfterLast());

    auto firstResult = query.getReal(0);
    EXPECT_TRUE(query.moveToNext());
    EXPECT_FALSE(query.isAfterLast());

    auto secondResult = query.getReal(0);
    EXPECT_TRUE(query.moveToNext());
    EXPECT_TRUE(query.isAfterLast());

    EXPECT_EQ(DBL_MIN, firstResult);
    EXPECT_EQ(DBL_MAX, secondResult);
}

TEST(MadDatabaseTests, InsertBlob) {
    auto db = MadDatabase::openInMemoryDatabase();
    db->exec("CREATE TABLE test(keyBlob BLOB);");
    EXPECT_EQ("", db->getError());

    auto cv = MadContentValues();
    vector<unsigned char> data = {'d', 'a', 't', 'a'};
    cv.putBlob("keyBlob", data);
    EXPECT_TRUE(db->insert("test", cv));
    EXPECT_EQ("", db->getError());

    auto query = db->query("SELECT keyBlob FROM test;");
    EXPECT_EQ("", db->getError());
    EXPECT_TRUE(query.moveToFirst());
    EXPECT_FALSE(query.isAfterLast());
    auto dat = query.getBlob(0);
    auto dataStr = query.getString(0);
    EXPECT_TRUE(query.moveToNext());
    EXPECT_TRUE(query.isAfterLast());

    EXPECT_EQ(data, dat);
    EXPECT_EQ("data", dataStr);
}

TEST(MadDatabaseTests, InsertText) {
    auto db = MadDatabase::openInMemoryDatabase();
    db->exec("CREATE TABLE test(keyText TEXT);");
    EXPECT_EQ("", db->getError());

    auto cv = MadContentValues();
    cv.putString("keyText", "data");
    EXPECT_TRUE(db->insert("test", cv));
    EXPECT_EQ("", db->getError());

    auto query = db->query("SELECT keyText FROM test;");
    EXPECT_EQ("", db->getError());
    EXPECT_TRUE(query.moveToFirst());
    EXPECT_FALSE(query.isAfterLast());
    auto dataStr = query.getString(0);
    EXPECT_TRUE(query.moveToNext());
    EXPECT_TRUE(query.isAfterLast());

    EXPECT_EQ("data", dataStr);
}

TEST(MadDatabaseTests, QueryArgs) {
    auto db = MadDatabase::openInMemoryDatabase();
    db->exec("CREATE TABLE test(keyInt INTEGER, keyText TEXT);");
    EXPECT_EQ("", db->getError());

    auto cv = MadContentValues();
    cv.putString("keyText", "the quick brown fox");
    cv.putInteger("keyInt", 99);
    EXPECT_TRUE(db->insert("test", cv));
    EXPECT_EQ("", db->getError());

    cv.clear();
    cv.putString("keyText", "the slow white tortoise");
    cv.putInteger("keyInt", 34);
    EXPECT_TRUE(db->insert("test", cv));

    long number;
    string value;
    {
        auto query = db->query("SELECT keyText,keyInt FROM test WHERE keyInt is ?;", {"99"});
        EXPECT_EQ("", db->getError());
        EXPECT_TRUE(query.moveToFirst());
        EXPECT_FALSE(query.isAfterLast());
        value = query.getString(0);
        number = query.getInt(1);
        EXPECT_TRUE(query.moveToNext());
        EXPECT_TRUE(query.isAfterLast());
    }

    EXPECT_EQ(99, number);
    EXPECT_EQ("the quick brown fox", value);

    {
        auto query = db->query("SELECT keyText,keyInt FROM test WHERE keyInt is ?;", {"34"});
        EXPECT_EQ("", db->getError());
        EXPECT_TRUE(query.moveToFirst());
        EXPECT_FALSE(query.isAfterLast());
        value = query.getString(0);
        number = query.getInt(1);
        EXPECT_TRUE(query.moveToNext());
        EXPECT_TRUE(query.isAfterLast());
    }

    EXPECT_EQ(34, number);
    EXPECT_EQ("the slow white tortoise", value);
}

TEST(MadDatabaseTests, MultiIndexCursor) {
    auto db = MadDatabase::openInMemoryDatabase();
    db->exec("CREATE TABLE test(keyInt INTEGER, keyReal REAL, keyText TEXT);");

    auto cv = MadContentValues();
    cv.putString("keyText", "the quick brown fox");
    cv.putInteger("keyInt", 99);
    cv.putReal("keyReal", M_PI);

    EXPECT_TRUE(db->insert("test", cv));
    EXPECT_EQ("", db->getError());

    cv.clear();
    cv.putString("keyText", "the slow red tortoise");
    cv.putInteger("keyInt", 42);
    cv.putReal("keyReal", M_E);

    EXPECT_TRUE(db->insert("test", cv));
    EXPECT_EQ("", db->getError());

    auto query = db->query("SELECT * FROM test;");
    EXPECT_EQ("", db->getError());

    EXPECT_TRUE(query.moveToFirst());
    EXPECT_EQ(M_PI, query.getReal(1));
    EXPECT_EQ("the quick brown fox", query.getString(2));
    EXPECT_EQ(99, query.getReal(0));
    EXPECT_FALSE(query.isAfterLast());

    EXPECT_TRUE(query.moveToNext());
    EXPECT_FALSE(query.isAfterLast());
    EXPECT_EQ(M_E, query.getReal(1));
    EXPECT_EQ("the slow red tortoise", query.getString(2));
    EXPECT_EQ(42, query.getReal(0));
    EXPECT_FALSE(query.isAfterLast());

    EXPECT_TRUE(query.moveToNext());
    EXPECT_FALSE(query.moveToNext());
    EXPECT_TRUE(query.isAfterLast());
}