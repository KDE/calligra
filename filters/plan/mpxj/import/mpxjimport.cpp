/* This file is part of the KDE project
   Copyright (C) 2012 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "mpxjimport.h"

#include <KoFilterChain.h>
#include <KoFilterManager.h>
#include <KoDocument.h>

#include <ktempdir.h>
#include <kdebug.h>

// JNI not used ATM, see "NOTE:" below
// #include <jni.h>

#include "mpxjconfig.h"
#include <kmessagebox.h>
#include <kprocess.h>

#ifdef Q_OS_WIN
#define CLASSPATH_SEPARATOR ";"
#else
#define CLASSPATH_SEPARATOR ":"
#endif

int planMpxjDbg() {
    static int s_area = KDebug::registerArea( "plan(MPXJ import)" );
    return s_area;
}

K_PLUGIN_FACTORY(MpxjImportFactory, registerPlugin<MpxjImport>();)
K_EXPORT_PLUGIN(MpxjImportFactory("calligrafilters"))

MpxjImport::MpxjImport(QObject* parent, const QVariantList &)
    : KoFilter(parent)
{
}

QStringList MpxjImport::mimeTypes()
{
    return QStringList()
        << QLatin1String("application/vnd.ms-project")
        << QLatin1String("application/x-project")
        << QLatin1String("application/x-planner");
}

KoFilter::ConversionStatus MpxjImport::convert(const QByteArray& from, const QByteArray& to)
{
    kDebug(planMpxjDbg()) << from << to;
    if ( to != "application/x-vnd.kde.plan" || ! mimeTypes().contains( from ) ) {
        kDebug(planMpxjDbg())<<"Bad mime types:"<<from<<"->"<<to;
        return KoFilter::BadMimeType;
    }
    bool batch = false;
    if ( m_chain->manager() ) {
        batch = m_chain->manager()->getBatchMode();
    }
    if (batch) {
        //TODO
        kDebug(planMpxjDbg()) << "batch mode not implemented";
        return KoFilter::NotImplemented;
    }
    KoDocument *part = m_chain->outputDocument();
    if ( ! part ) {
        kDebug(planMpxjDbg()) << "could not open document";
        return KoFilter::InternalError;
    }
    QString inputFile = m_chain->inputFile();
    kDebug(planMpxjDbg())<<"Import from:"<<inputFile;
    KTempDir *tmp = new KTempDir();
    QString outFile( tmp->name() + "maindoc.xml" );
    kDebug(planMpxjDbg())<<"Temp file:"<<outFile;
    KoFilter::ConversionStatus sts = doImport( inputFile.toUtf8(), outFile.toUtf8() );
    kDebug(planMpxjDbg())<<"doImport returned:"<<(sts == KoFilter::OK);
    if ( sts == KoFilter::OK ) {
        QFile file( outFile );
        KoXmlDocument doc;
        if ( ! doc.setContent( &file ) ) {
            kDebug(planMpxjDbg()) << "could not read maindoc.xml";
            sts = KoFilter::InternalError;
        } else if ( ! part->loadXML( doc, 0 ) ) {
            kDebug(planMpxjDbg()) << "failed to load maindoc.xml";
            sts = KoFilter::InternalError;
        }
    }
    delete tmp;
    return sts;
}

/*-------------------
NOTE:
There is an old bug in java VM that has the effect that
one can call createJavaVm() only once pr process, even
when destroyJavaVM() has been called in between.

This means that using a JavaVM + JNI, the user can only
use the filter once for each plan process.

We work around this by running java in a separate process until a better solution is found.
---------------------*/

KoFilter::ConversionStatus MpxjImport::doImport( QByteArray inFile, QByteArray outFile )
{
    QString normalizedInFile;
    QString normalizedOutFile;

    // Need to convert to "\" on Windows
    normalizedInFile = QDir::toNativeSeparators(inFile);
    normalizedOutFile = QDir::toNativeSeparators(outFile);

    kDebug(planMpxjDbg()) << normalizedInFile << normalizedOutFile;
#if 1
    QString cp = qgetenv( "PLAN_CLASSPATH" );
    QString x = QDir::toNativeSeparators(PLANCONVERT_JAR_FILE);
    if ( ! x.isEmpty() ) {
        if ( ! cp.isEmpty() ) {
            cp += CLASSPATH_SEPARATOR;
        }
        cp += x;
    }
    QString exe = "java";
    QStringList args;
    args <<  "-cp";
    args << cp;
    args << "plan.PlanConvert";
    args << normalizedInFile << normalizedOutFile;
    int res = KProcess::execute( exe, args );
    kDebug(planMpxjDbg())<<res;
    return res == 0 ? KoFilter::OK : KoFilter::InternalError;

#else
    JavaVM *jvm = 0;       /* denotes a Java VM */
    JNIEnv *env = 0;       /* pointer to native method interface */
    JavaVMInitArgs vm_args; /* JDK/JRE 6 VM initialization arguments */
    JavaVMOption* options = new JavaVMOption[1];
    QByteArray cp = qgetenv( "PLAN_CLASSPATH" );
    if ( ! cp.isEmpty() ) {
        cp = QByteArray( "-Djava.class.path=" ) + cp;
    }
    QByteArray x = PLANCONVERT_JAR_FILE;
    if ( ! x.isEmpty() ) {
        if ( ! cp.isEmpty() ) {
            cp += CLASSPATH_SEPARATOR;
        }
        cp += x;
    }
    options[0].optionString = cp.data();
    kDebug(planMpxjDbg())<<"env:"<<cp;
    vm_args.version = JNI_VERSION_1_6;
    vm_args.nOptions = 1;
    vm_args.options = options;
    vm_args.ignoreUnrecognized = false;
    /* load and initialize a Java VM, return a JNI interface pointer in env */
    JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);
    if ( ! env ) {
        kDebug(planMpxjDbg())<<"Failed to create jvm environment";
        // TODO: proper message, and restore cursor (or enhance filter manager error handling)
        KMessageBox::error( 0, "Failed to create JavaVM, check your installation" );
        return KoFilter::UserCancelled;
    }
    delete options;

    jclass cls = env->FindClass("plan/PlanConvert");
    if ( cls == 0 ) {
        kDebug(planMpxjDbg())<<"Failed to find class";
        return KoFilter::InternalError;
    }
    kDebug(planMpxjDbg())<<"Found class";
    jstring in = env->NewStringUTF( normalizedInFile );
    jstring out = env->NewStringUTF( normalizedOutFile );
    jobjectArray args = env->NewObjectArray( 2, env->FindClass("java/lang/String"), in );
//     env->setObjectArrayElement( args, 0, in );
    env->SetObjectArrayElement( args, 1, out );
    if (args == 0) {
        kDebug(planMpxjDbg())<<"Out of memory";
        return KoFilter::OutOfMemory;
    }
    jmethodID mid = env->GetStaticMethodID(cls, "main", "([Ljava/lang/String;)V" );
    if ( mid == 0 ) {
        kDebug(planMpxjDbg())<<"Failed to find main method";
        return KoFilter::InternalError;
    }
    env->CallStaticVoidMethod( cls, mid, args );
    if ( env->ExceptionCheck() ) {
        kError()<<"Exception:";
        env->ExceptionDescribe();
        return KoFilter::InternalError;
    }
    /* We are done. */
    jvm->DestroyJavaVM();
    if ( ! QFile::exists( normalizedOutFile ) ) {
        kDebug(planMpxjDbg())<<"No output file created:"<<outFile;
        return KoFilter::StorageCreationError;
    }
    return KoFilter::OK;
#endif
}

#include "mpxjimport.moc"
