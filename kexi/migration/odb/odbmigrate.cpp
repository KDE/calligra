#include "odbmigrate.h"
#include <jni.h>
#include <QRegExp>
#include <QFile>
#include <QVariant>
#include <QDateTime>
#include <QList>

#include <KDebug>
#include <KLocale>

#include <kexiutils/identifier.h>

using namespace KexiMigration;
//K_EXPORT_KEXIMIGRATE_DRIVER(OdbMigrate, "odb")


OdbMigrate::OdbMigrate(QObject *parent, const QVariantList &args)
        : KexiMigrate(parent, args)
{

}

OdbMigrate::~OdbMigrate()
{


}


JNIEnv* create_vm(JavaVM ** jvm) {

    JNIEnv *env;
    JavaVMInitArgs vm_args;
    JavaVMOption options;
    options.optionString = "-Djava.class.path=/home/dhruv/kde4/src/calligra/kexi/migration/odb/reader:/home/dhruv/Downloads/hsqldb-2.3.2/hsqldb/lib/hsqldb.jar"; //Path to the java source code

    vm_args.version = JNI_VERSION_1_6; //JDK version. This indicates version 1.6
    vm_args.nOptions = 1;
    vm_args.options = &options;
    vm_args.ignoreUnrecognized = 0;

    int ret = JNI_CreateJavaVM(jvm, (void**)&env, &vm_args);
    if(ret < 0)
        
        kDebug() << "\nUnable to Launch JVM\n";
    return env;
}


bool OdbMigrate::drv_connect()
{
    JNIEnv *env;
    jclass clsH = env->FindClass("OdbReader");
    if(clsH) 
      kDebug() << "found class\n";
    else return 0;
    jobject object = env->NewObject(clsH, NULL);

   
    kDebug() << "object constructed\n";
    return true;
}


