#include <jni.h>
#include <string>
#include "Database.hpp"

/*
 * How to find java class,field and method signatures:
 * $unzip /Library/Java/JavaVirtualMachines/jdk1.8.0_74.jdk/Contents/Home/jre/lib/rt.jar
 * $javap -s ./java/lang/Object.class
 *
 * http://docs.oracle.com/javase/7/docs/technotes/guides/jni/spec/types.html
 */
static jclass integerClass;
static jclass longClass;
static jclass floatClass;
static jclass doubleClass;
static jclass stringClass;
static jclass byteArrayClass;
static jmethodID intValueMethodId;
static jmethodID longValueMethodId;
static jmethodID doubleValueMethodId;
static jmethodID floatValueMethodId;

enum JTYPE {
    UNKNOWN,
    JINT,
    JLONG,
    JFLOAT,
    JDOUBLE,
    JSTRING,
    JBYTEARRAY,
};

JTYPE typeOf(JNIEnv *env, jobject &object) {

    // INTEGER
    if (env->IsInstanceOf(object, integerClass)) {
        return JINT;
    }
    if (env->IsInstanceOf(object, longClass)) {
        return JLONG;
    }

    // REAL
    if (env->IsInstanceOf(object, floatClass)) {
        return JFLOAT;
    }
    if (env->IsInstanceOf(object, doubleClass)) {
        return JDOUBLE;
    }

    // TEXT
    if (env->IsInstanceOf(object, stringClass)) {
        return JSTRING;
    }

    // BLOB
    if (env->IsInstanceOf(object, byteArrayClass)) {
        return JBYTEARRAY;
    }

    return UNKNOWN;
}

int jobjectToInteger(JNIEnv *env, jobject &value) {
    return env->CallIntMethod(value, intValueMethodId);
}

sqlite3_int64 jobjectToLong(JNIEnv *env, jobject &value) {
    return env->CallLongMethod(value, longValueMethodId);
}

double jobjectToDouble(JNIEnv *env, jobject &value) {
    return env->CallDoubleMethod(value, doubleValueMethodId);
}

float jobjectToFloat(JNIEnv *env, jobject &value) {
    return env->CallFloatMethod(value, floatValueMethodId);
}

std::string jobjectToString(JNIEnv *env, jobject &value) {
    const char *strValue = env->GetStringUTFChars((jstring) value, 0);
    std::string retVal(strValue);
    env->ReleaseStringUTFChars((jstring) value, strValue);
    return retVal;
}


extern "C" {

JNIEXPORT jboolean JNICALL
Java_io_madrona_madsqlite_JniBridge_moveToFirst(JNIEnv,
                                                jclass,
                                                jlong nativePtr) {

    Cursor *cursor = reinterpret_cast<Cursor *>(nativePtr);
    return (jboolean) cursor->moveToFirst();
}

JNIEXPORT jboolean JNICALL
Java_io_madrona_madsqlite_JniBridge_moveToNext(JNIEnv,
                                               jclass,
                                               jlong nativePtr) {
    Cursor *cursor = reinterpret_cast<Cursor *>(nativePtr);
    return (jboolean) cursor->moveToNext();
}

JNIEXPORT jboolean JNICALL
Java_io_madrona_madsqlite_JniBridge_isAfterLast(JNIEnv,
                                                jclass,
                                                jlong nativePtr) {
    Cursor *cursor = reinterpret_cast<Cursor *>(nativePtr);
    return (jboolean) cursor->isAfterLast();
}

JNIEXPORT jstring JNICALL
Java_io_madrona_madsqlite_JniBridge_getString(JNIEnv *env,
                                              jclass,
                                              jlong nativePtr,
                                              jint columnIndex) {
    Cursor *cursor = reinterpret_cast<Cursor *>(nativePtr);
    auto str = cursor->getString(columnIndex);
    return env->NewStringUTF(str.c_str());
}

JNIEXPORT jbyteArray JNICALL
Java_io_madrona_madsqlite_JniBridge_getBlob(JNIEnv *env,
                                            jclass,
                                            jlong nativePtr,
                                            jint columnIndex) {
    Cursor *cursor = reinterpret_cast<Cursor *>(nativePtr);
    auto vec = cursor->getBlob(columnIndex);
    jbyteArray retVal = env->NewByteArray((jsize) vec.size());
    jbyte *data = reinterpret_cast<jbyte *>(vec.data());
    env->SetByteArrayRegion(retVal, 0, (jsize) vec.size(), data);
    return retVal;
}

JNIEXPORT jlong JNICALL
Java_io_madrona_madsqlite_JniBridge_getLong(JNIEnv,
                                           jclass,
                                           jlong nativePtr,
                                           jint columnIndex) {

    Cursor *cursor = reinterpret_cast<Cursor *>(nativePtr);
    return (jlong) cursor->getInt(columnIndex);
}

JNIEXPORT jdouble JNICALL
Java_io_madrona_madsqlite_JniBridge_getReal(JNIEnv,
                                            jclass,
                                            jlong nativePtr,
                                            jint columnIndex) {
    Cursor *cursor = reinterpret_cast<Cursor *>(nativePtr);
    return cursor->getReal(columnIndex);
}

JNIEXPORT jstring JNICALL
Java_io_madrona_madsqlite_JniBridge_getError(JNIEnv *env,
                                             jclass,
                                             jlong dbPtr) {
    Database *db = reinterpret_cast<Database *>(dbPtr);
    return env->NewStringUTF(db->getError().c_str());
}

JNIEXPORT void JNICALL
Java_io_madrona_madsqlite_JniBridge_beginTransaction(JNIEnv,
                                                     jclass,
                                                     jlong dbPtr) {
    Database *db = reinterpret_cast<Database *>(dbPtr);
    db->beginTransaction();
}

JNIEXPORT void JNICALL
Java_io_madrona_madsqlite_JniBridge_rollbackTransaction(JNIEnv,
                                                        jclass,
                                                        jlong dbPtr) {
    Database *db = reinterpret_cast<Database *>(dbPtr);
    db->rollbackTransaction();
}

JNIEXPORT void JNICALL
Java_io_madrona_madsqlite_JniBridge_commitTransaction(JNIEnv,
                                                   jclass,
                                                   jlong dbPtr) {
    Database *db = reinterpret_cast<Database *>(dbPtr);
    db->commitTransaction();
}

JNIEXPORT jboolean JNICALL
Java_io_madrona_madsqlite_JniBridge_insert(JNIEnv *env,
                                           jclass,
                                           jlong dbPtr,
                                           jstring table,
                                           jobjectArray keys,
                                           jobjectArray values) {
    const char *tableStr = env->GetStringUTFChars(table, 0);
    int keyCount = env->GetArrayLength(keys);
    int valueCount = env->GetArrayLength(keys);
    ContentValues contentValues = {};
    if (keyCount == valueCount) {
        for (int i = 0; i < keyCount; i++) {
            jstring key = (jstring) (env->GetObjectArrayElement(keys, i));
            const char *keyStr = env->GetStringUTFChars(key, 0);
            jobject value = env->GetObjectArrayElement(values, i);
            auto dataType = typeOf(env, value);
            switch (dataType) {
                case JINT: {
                    contentValues.putInteger(keyStr, jobjectToInteger(env, value));
                    break;
                }
                case JLONG: {
                    contentValues.putInteger(keyStr, jobjectToLong(env, value));
                    break;
                }
                case JFLOAT: {
                    contentValues.putReal(keyStr, jobjectToFloat(env, value));
                    break;
                }
                case JDOUBLE: {
                    contentValues.putReal(keyStr, jobjectToDouble(env, value));
                    break;
                }
                case JSTRING: {
                    contentValues.putString(keyStr, jobjectToString(env, value));
                    break;
                }
                case JBYTEARRAY: {
                    jbyteArray array = (jbyteArray) value;
                    jboolean isCopy;
                    jbyte* elements = env->GetByteArrayElements(array, &isCopy);
                    jsize size = env->GetArrayLength(array);
                    contentValues.putBlob(keyStr, elements, (size_t) size);
                    env->ReleaseByteArrayElements(array, elements, 0);
                    break;
                }
                case UNKNOWN: {
                    break;
                }
            }
            env->ReleaseStringUTFChars(key, keyStr);
        }
        Database *db = reinterpret_cast<Database *>(dbPtr);
        bool retVal = db->insert(tableStr, contentValues);
        env->ReleaseStringUTFChars(table, tableStr);
        return (jboolean) retVal;
    } else {
        return (jboolean) false;
    }
}

JNIEXPORT jint JNICALL
Java_io_madrona_madsqlite_JniBridge_exec(JNIEnv *env,
                                         jclass,
                                         jlong dbPtr,
                                         jstring sql) {
    const char *sqlStr = env->GetStringUTFChars(sql, 0);
    Database *db = reinterpret_cast<Database *>(dbPtr);
    int changes = db->exec(sqlStr);
    env->ReleaseStringUTFChars(sql, sqlStr);
    return reinterpret_cast<jint>(changes);
}

JNIEXPORT jlong JNICALL
Java_io_madrona_madsqlite_JniBridge_query(JNIEnv *env,
                                          jclass,
                                          jlong dbPtr,
                                          jstring query,
                                          jobjectArray args) {
    Database *db = reinterpret_cast<Database *>(dbPtr);
    const char *queryStr = env->GetStringUTFChars(query, 0);
    Cursor *cursor;
    if (args) {
        int count = env->GetArrayLength(args);
        auto argsVector = std::vector<std::string>();
        for (int i = 0; i < count; i++) {
            jstring str = (jstring) (env->GetObjectArrayElement(args, i));
            const char *rawString = env->GetStringUTFChars(str, 0);
            argsVector.push_back(rawString);
            env->ReleaseStringUTFChars(str, rawString);
        }
        auto c = db->query(queryStr, argsVector);
        cursor = new Cursor(std::move(c));
    } else {
        auto c = db->query(queryStr);
        cursor = new Cursor(std::move(c));
    }
    env->ReleaseStringUTFChars(query, queryStr);
    return reinterpret_cast<jlong>(cursor);
}

JNIEXPORT jlong JNICALL
Java_io_madrona_madsqlite_JniBridge_openDatabase(JNIEnv *env,
                                                 jclass,
                                                 jstring absPath) {
    if (absPath) {
        const char *path = env->GetStringUTFChars(absPath, 0);
        jlong retVal = reinterpret_cast<jlong>(new Database(path));
        env->ReleaseStringUTFChars(absPath, path);
        return retVal;
    } else {
        return reinterpret_cast<jlong>(new Database());
    }
}

JNIEXPORT void JNICALL
Java_io_madrona_madsqlite_JniBridge_closeDatabase(JNIEnv,
                                                  jclass,
                                                  jlong nativePtr) {
    void *db = reinterpret_cast<void *>(nativePtr);
    delete (db);
}

JNIEXPORT void JNICALL
Java_io_madrona_madsqlite_JniBridge_closeCursor(JNIEnv,
                                                jclass,
                                                jlong nativePtr) {
    void *db = reinterpret_cast<void *>(nativePtr);
    delete (db);
}

jclass FindClass(JNIEnv *env, const char* name) {
    jclass localRef = env->FindClass(name);
    jclass globalRef = (jclass) env->NewGlobalRef(localRef);
    env->DeleteLocalRef(localRef);
    return globalRef;
}

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    vm->GetEnv((void **) &env, JNI_VERSION_1_6);

    integerClass = FindClass(env, "java/lang/Integer");
    longClass = FindClass(env, "java/lang/Long");
    floatClass = FindClass(env, "java/lang/Float");
    doubleClass = FindClass(env, "java/lang/Double");
    stringClass = FindClass(env, "java/lang/String");
    byteArrayClass = FindClass(env, "[B");

    intValueMethodId = env->GetMethodID(integerClass, "intValue", "()I");
    longValueMethodId = env->GetMethodID(longClass, "longValue", "()J");
    doubleValueMethodId = env->GetMethodID(doubleClass, "doubleValue", "()D");
    floatValueMethodId = env->GetMethodID(floatClass, "floatValue", "()F");

    return JNI_VERSION_1_6;
}

}
