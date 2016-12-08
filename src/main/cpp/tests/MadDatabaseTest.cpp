//
// Created by William Kamp on 12/7/16.
//
#include <iostream>
#include "gtest/gtest.h"
#include "MadDatabase.hpp"

using namespace madsqlite;
using namespace std;


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

TEST(MadDatabseTests, InsertLong) {
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

TEST(MadDatabseTests, InsertFloat) {
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

TEST(MadDatabseTests, InsertDouble) {
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

TEST(MadDatabseTests, InsertBlob) {
    auto db = MadDatabase::openInMemoryDatabase();
    db->exec("CREATE TABLE test(keyBlob BLOB);");
    EXPECT_EQ("", db->getError());

    auto cv = MadContentValues();
    vector<byte> data = {'d', 'a', 't', 'a'};
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

TEST(MadDatabseTests, InsertText) {
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
