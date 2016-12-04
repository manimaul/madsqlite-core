

#include <string>
#include <iostream>
#include "MadDatabase.hpp"

using namespace madsqlite;
using namespace std;

static void print(string const &msg, double &value) {
    cout << msg << " " << value << endl;
}

static void print(string const &msg, string &value) {
    cout << msg << " " << value << endl;
}

static void print(string const &msg, int &value) {
    cout << msg << " " << value << endl;
}

static void print(string const &msg, sqlite3_int64 &value) {
    cout << msg << " " << value << endl;
}

static void print(string const &msg, vector<byte> &blob) {
    cout << msg << " ";
    for (byte &b : blob) {
        cout << b;
    }
    cout << endl;
}

int main() {
    // Create an in-memory database
    auto db = MadDatabase();
    db.exec("CREATE TABLE test(x INTEGER, "
                    "y TEXT, "
                    "z BLOB);");

    // Insert in database
    auto cv = MadContentValues();
    const char *blob = "i'm a blob";
    cv.putBlob("z", blob, strlen(blob));
    cv.putInteger("y", 7070);
    cv.putInteger("x", 1970);
    db.insert("test", cv);

    cv.clear();
    db.insert("test", cv);

    cv.clear();
    cv.putInteger("y", 61);
    db.insert("test", cv);

    auto cursor = db.query("SELECT * FROM test");
    cursor.moveToFirst();
    while (!cursor.isAfterLast()) {
        sqlite3_int64 storedX = cursor.getInt(0);
        print("x:", storedX);
        string storedY = cursor.getString(1);
        print("y:", storedY);
        vector<byte> storedBlob = cursor.getBlob(2);
        print("z:", storedBlob);
        cursor.moveToNext();
    }
}
