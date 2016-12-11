

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

static void print(string const &msg, long long int &value) {
    cout << msg << " " << value << endl;
}

static void print(string const &msg, vector<unsigned char> &blob) {
    cout << msg << " ";
    for (unsigned char &b : blob) {
        cout << b;
    }
    cout << endl;
}

std::vector<unsigned char> getImage(std::string name) {
    return std::vector<unsigned char>();
}

void db() {

    // Open / create database
    auto db = MadDatabase::openDatabase("my_database.s3db");

    // Execute sql statement
    db->exec("CREATE TABLE location_table(name TEXT, latitude REAL, longitude REAL, image BLOB);");

    // Insert in database
    auto cv = MadContentValues();
    cv.putString("name", "Cheshire Cat");
    cv.putReal("latitude", 51.2414945);
    cv.putReal("longitude", -0.6354629);
    cv.putBlob("image", getImage("Cheshire Cat"));
    db->insert("location_table", cv);

    // Query database
    auto qry = db->query("SELECT name, latitude, longitude FROM location_table WHERE name=?", {"Cheshire Cat"});
    qry.moveToFirst();
    while (!qry.isAfterLast()) {
        auto name = qry.getString(0);
        double latitude = qry.getReal(1);
        double longitude = qry.getReal(2);
        std::cout << name << " latitude:" << latitude << " longitude:" << longitude << std::endl;
        qry.moveToNext();
    }
}

int main() {
    db();
    return 0;
}
