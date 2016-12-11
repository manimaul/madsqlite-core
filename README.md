#MadSqlite Core

 * A simple C++14 [Sqlite](https://sqlite.org) abstraction
 * [FTS5](https://sqlite.org/fts5.html) and [RTree](https://www.sqlite.org/rtree.html) extension modules enabled
 * [BSD License](LICENSE.md)

| Platform                                      | Languages                                | Build status                                   |
| --------------------------------------------- | ---------------------------------------- | ---------------------------------------------- |
| [MadSqlite Core](https://github.com/manimaul/madsqlite-core) | C++14 |[![Build Status](https://travis-ci.org/manimaul/madsqlite-core.svg?branch=master)](https://travis-ci.org/manimaul/madsqlite-core) |
| [MadSqlite Android](https://github.com/manimaul/madsqlite-android) | Java | [![Build Status](https://travis-ci.org/manimaul/madsqlite-android.svg?branch=master)](https://travis-ci.org/manimaul/madsqlite-android) |
| [MadSqlite iOS Obj-c](https://github.com/manimaul/madsqlite-ios-objc) | Objective-C  | [![Build Status](https://travis-ci.org/manimaul/madsqlite-ios-objc.svg?branch=master)](https://travis-ci.org/manimaul/madsqlite-ios-objc) |
| [MadSqlite iOS Swift](https://github.com/manimaul/madsqlite-ios-swift) | Swift

##Documentation
[API (Doxygen) Class List](https://manimaul.github.io/madsqlite-core/annotated.html)

####Example
```cpp

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

```