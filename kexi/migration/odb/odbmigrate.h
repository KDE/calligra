/*
 *This is a header file of the odbmigrate.cpp program
*/
#ifndef ODBMIGRATE_H
#define ODBMIGRATE_H
#include <jni.h>

#include <QByteArray>
#include <QString>
#include <QStringList>

#include <migration/keximigrate.h>
#include <migration/keximigrate_p.h>

namespace KexiMigration
{
class OdbMigrate : public KexiMigrate
{
    Q_OBJECT
    KEXIMIGRATION_DRIVER
public:
    OdbMigrate(QObject *parent, const QVariantList& args = QVariantList());
    virtual ~OdbMigrate();


protected:
    //! Driver specific connection implementation
    virtual bool drv_connect();
    virtual bool drv_disconnect();
    //! Driver specific function to read a table schema
    virtual bool drv_readTableSchema(const QString& originalName,
				     KexiDB::TableSchema& tableSchema);
    //! Driver specific function to return table names
    virtual bool drv_tableNames(QStringList& tablenames);
    /*! Copy ODB table to KexiDB database */
    virtual bool drv_copyTable(const QString& srcTable,
                                    KexiDB::Connection *destConn, KexiDB::TableSchema* dstTable);

private:
    JNIEnv* create_vm(JavaVM **jvm);
    KexiDB::Field::Type type(QString type);
    QVariant toQVariant(const char*, unsigned int, QString);
    JNIEnv* env;
    jclass clsH;
    jobject java_class_object;
};
}

#endif // ODBMIGRATE_H


