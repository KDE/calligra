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
    virtual bool drv_connect();
    virtual bool drv_disconnect();
    virtual bool drv_readTableSchema(const QString& originalName,
                     KexiDB::TableSchema& tableSchema);
    virtual bool drv_tableNames(QStringList& tablenames);
    virtual bool drv_copyTable(const QString& srcTable,
                                    KexiDB::Connection *destConn, KexiDB::TableSchema* dstTable);

private:
    JNIEnv* create_vm(JavaVM **jvm);
    JNIEnv* env;
    JavaVM* jvm;


};
}

#endif // ODBMIGRATE_H

