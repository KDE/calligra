#include "odbmigrate.h"
#include <jni.h>
#include <QRegExp>
#include <QFile>
#include <QVariant>
#include <QDateTime>
#include <QList>
#include <QString>
#include <QStringList>

#include <KDebug>
#include <KLocale>
#include <KStandardDirs>

#include <kexiutils/identifier.h>

using namespace KexiMigration;
K_EXPORT_KEXIMIGRATE_DRIVER(OdbMigrate, "odb")



OdbMigrate::OdbMigrate(QObject *parent, const QVariantList &args)
        : KexiMigrate(parent, args)
{

}

OdbMigrate::~OdbMigrate()
{


}


JNIEnv* OdbMigrate::create_vm(JavaVM ** jvm) {

    JavaVMInitArgs vm_args;
    JavaVMOption options;

    QString odbreader_path=KStandardDirs::locate("lib","OdbReader.jar");
    QString hsqldb_path=KStandardDirs::locate("lib","hsqldb.jar");
    kDebug() << odbreader_path;
    kDebug() << hsqldb_path;

    options.optionString = QString("-Djava.class.path="+odbreader_path+":"+hsqldb_path).toAscii().data();
    vm_args.version = JNI_VERSION_1_6; //JDK version. This indicates version 1.6
    vm_args.nOptions = 1;
    vm_args.options = &options;
    vm_args.ignoreUnrecognized = 0;

    int ret = JNI_CreateJavaVM(jvm, (void**)&env, &vm_args);
    if(ret != JNI_OK)
        kDebug() << "Unable to Launch JVM";
    return env;
}


bool OdbMigrate::drv_connect()
{
    jclass clsH = env->FindClass("OdbReader");
    if(clsH)
        kDebug() << "found class";
    else
        return 0;
    jobject object = env->NewObject(clsH, NULL);


    kDebug() << "object constructed";
    return true;
}

bool OdbMigrate::drv_readTableSchema(
    const QString& originalName, KexiDB::TableSchema& tableSchema)
{
    return false;
}

bool OdbMigrate::drv_disconnect()
{
    jvm->DestroyJavaVM();
    kDebug() << "jvm destroyed";
    return true;
}

bool OdbMigrate::drv_tableNames(QStringList& tableNames)
{
    return false;
}

bool OdbMigrate::drv_copyTable(const QString& srcTable, KexiDB::Connection *destConn,
                                 KexiDB::TableSchema* dstTable)
{
    return false;
}
