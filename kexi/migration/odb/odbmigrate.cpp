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
    JavaVM** jvm;
    create_vm(jvm);
    drv_connect();
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
    clsH = env->FindClass("OdbReader");
    if(clsH) 
        kDebug() << "found class";
    else 
        return 0;
    java_class_object = env->NewObject(clsH, NULL);

    kDebug() << "object constructed";

    QString filePath = this->data()->source->fileName();

    return true;
}

bool OdbMigrate::drv_readTableSchema(
    const QString& originalName, KexiDB::TableSchema& tableSchema)
{
    char* tableName=originalName.toAscii().data();
    jmethodID getTableNames = env->GetMethodID(clsH,"getTableSchema","(Ljava/lang/String;)Ljava/lang/String;");
    jstring returnString = (jstring) env->CallObjectMethod(java_class_object,getTableNames,tableName);
    const char* tablesstring = env->GetStringUTFChars(returnString, NULL);
    QString jsonString(tablesstring);
    QStringList list = jsonString.split(",");

    for(int i=0;i<list.size();i+=2)
    {
        QString fldID(KexiUtils::stringToIdentifier(list.at(i+1)));
        KexiDB::Field *fld =
            new KexiDB::Field(fldID, type(list.at(i+1)));
        fld->setCaption(list.at(i));
        tableSchema.addField(fld);
    }

    return false;
}

bool OdbMigrate::drv_disconnect()
{
    return false;
}

bool OdbMigrate::drv_tableNames(QStringList& tableNames)
{
    jmethodID getTableNames = env->GetMethodID(clsH,"getTableNames","()Ljava/lang/String;");
    jstring returnString = (jstring) env->CallObjectMethod(java_class_object,getTableNames,NULL);
    const char* tablesstring = env->GetStringUTFChars(returnString, NULL);
    QString temp(tablesstring);
    QStringList list = temp.split(",");
    for(int i=0;i<list.length();i++){
        QString s=list.at(i);
        if(!s.isEmpty())
        {
            tableNames << s;
            kDebug() << s;
        }
    }
    return false;
}

bool OdbMigrate::drv_copyTable(const QString& srcTable, KexiDB::Connection *destConn,
                                 KexiDB::TableSchema* dstTable)
{
    return false;
}

KexiDB::Field::Type OdbMigrate::type(QString type)
{
    KexiDB::Field::Type kexiType;

    if(type.compare("BOOLEAN")==0 || type.compare("BIT")==0)
        kexiType = KexiDB::Field::Boolean;
    else if(type.compare("CHARACTER")==0||type.compare("CHAR")==0)
        kexiType = KexiDB::Field::Byte;
    else if(type.compare("TINYINT")==0||type.compare("SHORTINT")==0)
        kexiType = KexiDB::Field::ShortInteger;
    else if(type.compare("INTEGER")==0||type.compare("INT")==0)
        kexiType = KexiDB::Field::Integer;
    else if(type.compare("BIGINT")==0)
        kexiType = KexiDB::Field::BigInteger;
    else if(type.compare("DECIMAL")==0||type.compare("NUMERIC")==0||type.compare("REAL")==0||type.compare("DOUBLE")==0)
        kexiType = KexiDB::Field::Double;
    else if(type.compare("FLOAT")==0)
        kexiType = KexiDB::Field::Float;
    else if(type.compare("DATE")==0)
        kexiType = KexiDB::Field::Date;
    else if(type.compare("TIME")==0)
        kexiType = KexiDB::Field::Time;
    else if(type.compare("DATETIME")==0||type.compare("TIMESTAMP")==0)
        kexiType = KexiDB::Field::DateTime;
    else if(type.compare("VARCHAR")==0||type.compare("VARCHAR_IGNORECASE")==0||type.compare("LONGVARCHAR")==0||type.compare("CLOB")==0)
        kexiType = KexiDB::Field::LongText;
    else if(type.compare("BINARY")==0||type.compare("VARBINARY")==0||type.compare("LONGVARBINARY")==0||type.compare("OTHER")==0||type.compare("OBJECT")==0)
        kexiType = KexiDB::Field::BLOB;
    else
        kexiType = KexiDB::Field::InvalidType;

    // If we don't know what it is, hope it's text. :o)
    if (kexiType == KexiDB::Field::InvalidType) {
        return KexiDB::Field::LongText;
    }
    return kexiType;
}
