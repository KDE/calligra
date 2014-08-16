#include "odbmigrate.h"
#include <jni.h>
#include <QRegExp>
#include <QFileInfo>
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
K_EXPORT_KEXIMIGRATE_DRIVER(OdbMigrate, odb)

static QString javaStringToQString(JNIEnv *env, jstring s)
{
    const char* cstr = env->GetStringUTFChars(s, 0);
    QString qs(QString::fromUtf8(cstr));
    env->ReleaseStringUTFChars(s, cstr);
    return qs;
}

OdbMigrate::OdbMigrate(QObject *parent, const QVariantList &args)
        : KexiMigrate(parent, args), m_jvm(0), env(0)
{
}

OdbMigrate::~OdbMigrate()
{
}

bool OdbMigrate::create_vm()
{
    JavaVMInitArgs vm_args;
    JavaVMOption options[2];

    QString odbreader_path = KStandardDirs::locate("lib","OdbReader.jar");
    kDebug() << odbreader_path;

    vm_args.nOptions = 0;
    QByteArray option0 = "-Djava.class.path=" + odbreader_path.toUtf8() + ":" + HSQLDB_LIBRARY;
    options[vm_args.nOptions++].optionString = option0.data();
    QByteArray option1 = "-verbose:jni"; //"-Xcheck:jni:all"
    options[vm_args.nOptions++].optionString = option1.data();
    vm_args.version = JNI_VERSION_1_6; //JDK version. This indicates version 1.6
    vm_args.options = options;
    vm_args.ignoreUnrecognized = false;
    kDebug() << vm_args.options[0].optionString << vm_args.options[1].optionString;

    int ret = JNI_CreateJavaVM(&m_jvm, (void**)&env, &vm_args);
    kDebug() << "JNI_CreateJavaVM():" << ret;
    if(ret != JNI_OK) {
        kWarning() << "Unable to Launch JVM";
        return false;
    }
    return true;
}

bool OdbMigrate::drv_connect()
{
    if (!create_vm()) {
        return false;
    }
    clsH = env->FindClass("OdbReader");
    if(clsH) 
        kDebug() << "found class";
    else 
        return false;

    jmethodID OdbReaderCtorId = env->GetMethodID(clsH, "<init>", "(Ljava/lang/String;)V");
    if (!OdbReaderCtorId) {
        kWarning() << "!OdbReaderCtorId";
        return false;
    }

    jstring filePath = env->NewStringUTF(data()->source->fileName().toUtf8().constData());
    java_class_object = env->NewObject(clsH, OdbReaderCtorId, filePath);
    kDebug() << filePath;
    if (!java_class_object) {
        kWarning() << "!java_class_object";
        return false;
    }
    kDebug() << "object constructed";
    return true;
}

bool OdbMigrate::drv_readTableSchema(
    const QString& originalName, KexiDB::TableSchema& tableSchema)
{
    char* tableName=originalName.toAscii().data();
    jmethodID getTableSchema = env->GetMethodID(clsH,"getTableSchema","(Ljava/lang/String;)Ljava/lang/String;");
    if (!getTableSchema) {
        kWarning() << "!getTableSchema";
        return false;
    }
    jstring returnString = (jstring) env->CallObjectMethod(java_class_object,getTableSchema,tableName);
    QString jsonString = javaStringToQString(env, returnString);
    QStringList list = jsonString.split(",");

    for(int i=0;i<list.size();i+=2)
    {
        QString fldID(KexiUtils::stringToIdentifier(list.at(i)));
        KexiDB::Field *fld =
            new KexiDB::Field(fldID, type(list.at(i+1)));
        fld->setCaption(list.at(i));
        tableSchema.addField(fld);
    }
    return true;
}

bool OdbMigrate::drv_disconnect()
{
    return true;
}

bool OdbMigrate::drv_tableNames(QStringList& tableNames)
{
    jmethodID getTableNames = env->GetMethodID(clsH,"getTableNames","()Ljava/lang/String;");
    if (!getTableNames) {
        kWarning() << "!getTableNames";
        return false;
    }
    jstring returnString = (jstring) env->CallObjectMethod(java_class_object,getTableNames,NULL);
    QString temp = javaStringToQString(env, returnString);
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


bool OdbMigrate::drv_copyTable(const QString& srcTable, KexiDB::Connection *destConn,
                                 KexiDB::TableSchema* dstTable)
{
    bool ok = true;
    char* tableName=srcTable.toAscii().data();

    jmethodID getTableNames = env->GetMethodID(clsH,"getTableSchema","(Ljava/lang/String;)Ljava/lang/String;");
    jstring returnString = (jstring) env->CallObjectMethod(java_class_object,getTableNames,tableName);
    QString jsonString = javaStringToQString(env, returnString);
    QStringList list = jsonString.split(",");

    jmethodID getTableSize = env->GetMethodID(clsH,"getTableSize","(Ljava/lang/String;)Ljava/lang/String;");
    if (!getTableSize) {
        kWarning() << "!getTableSize";
        return false;
    }
    jmethodID getCellValue = env->GetMethodID(clsH,"getCellValue","((II)Ljava/lang/String;Ljava/lang/String;");
    if (!getCellValue) {
        kWarning() << "!getCellValue";
        return false;
    }
    returnString = (jstring) env->CallObjectMethod(java_class_object,getTableSize,tableName);
    QString jsonString2 = javaStringToQString(env, returnString);
    list = jsonString2.split(",");
    int columns=list.at(0).toInt();
    int rows=list.at(1).toInt();
    int i=0;
    for(i=0;i<rows;i++)
    {
        QList<QVariant> vals;
        int j=0;
        for(j=0;j<columns;j++)
        {
            returnString = (jstring) env->CallObjectMethod(java_class_object,getCellValue,j+1,i+1,tableName);
            const char* tablesstring = env->GetStringUTFChars(returnString, NULL);
            QVariant var = toQVariant(tablesstring,QString(tablesstring).length(), list.at(2*j + 1));
            vals << var;
        }
        if (!destConn->insertRecord(*dstTable, vals)) {
            ok = false;
            break;
        }
    }
    return ok;
}


QVariant OdbMigrate::toQVariant(const char* data, unsigned int len, QString type)
{
    if(len==0)
        return QVariant();

    if(type.compare("BOOLEAN")==0 || type.compare("BIT")==0)
        return QString::fromUtf8(data, len).toShort();
    else if(type.compare("CHARACTER")==0||type.compare("CHAR")==0)
        return QString::fromUtf8(data, len).toShort();
    else if(type.compare("TINYINT")==0||type.compare("SHORTINT")==0)
        return QString::fromUtf8(data, len).toLongLong();
    else if(type.compare("INTEGER")==0||type.compare("INT")==0)
        return QString::fromUtf8(data, len).toLongLong();
    else if(type.compare("BIGINT")==0)
        return QString::fromUtf8(data, len).toLongLong();
    else if(type.compare("DECIMAL")==0||type.compare("NUMERIC")==0||type.compare("REAL")==0||type.compare("DOUBLE")==0)
        return QString::fromUtf8(data, len).toDouble();
    else if(type.compare("FLOAT")==0)
        return QString::fromUtf8(data, len).toFloat();
    else if(type.compare("DATE")==0)
        return QDate::fromString(data, Qt::ISODate);
    else if(type.compare("TIME")==0)
        return QTime::fromString(data, Qt::ISODate);
    else if(type.compare("DATETIME")==0||type.compare("TIMESTAMP")==0)
        return QDateTime::fromString(data, Qt::ISODate);
    else if(type.compare("VARCHAR")==0||type.compare("VARCHAR_IGNORECASE")==0||type.compare("LONGVARCHAR")==0||type.compare("CLOB")==0)
        return QVariant(QString::fromUtf8(data, len));
    else if(type.compare("BINARY")==0||type.compare("VARBINARY")==0||type.compare("LONGVARBINARY")==0||type.compare("OTHER")==0||type.compare("OBJECT")==0)
        return QVariant(QString::fromUtf8(data, len));
    else
        return QVariant(QString::fromUtf8(data, len));

}
