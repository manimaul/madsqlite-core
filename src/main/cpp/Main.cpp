

#include <string>
#include <iostream>
#include "Database.hpp"

static void print(std::string const &msg, double &value) {
    std::cout << msg << " " << value << std::endl;
}

static void print(std::string const &msg, std::string &value) {
    std::cout << msg << " " << value << std::endl;
}

static void print(std::string const &msg, int &value) {
    std::cout << msg << " " << value << std::endl;
}

static void print(std::string const &msg, uint64_t &value) {
    std::cout << msg << " " << value << std::endl;
}

static void print(std::string const &msg, std::vector<byte> &blob) {
    std::cout << msg << " ";
    for (byte &b : blob) {
        std::cout << b;
    }
    std::cout << std::endl;
}

int main() {
    // Create an in-memory database
    auto db = Database();
    db.exec("CREATE TABLE test(x INTEGER, "
                    "y TEXT, "
                    "z BLOB);");

    // Insert in database
    auto cv = ContentValues();
    char *blob = "i'm a blob";
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
        uint64_t storedX = cursor.getInt(0);
        print("x:", storedX);
        std::string storedY = cursor.getString(1);
        print("y:", storedY);
        std::vector<byte> storedBlob = cursor.getBlob(2);
        print("z:", storedBlob);
        cursor.moveToNext();
    }
}
