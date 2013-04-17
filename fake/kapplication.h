#ifndef FAKE_KAPPLICATION_H
#define FAKE_KAPPLICATION_H

#include <QApplication>
#include <QDebug>
#include <kcmdlineargs.h>
#include <kglobal.h>

#include "kofake_export.h"


class KApplication : public QApplication
{
public:
    KApplication(bool isGui = true) : QApplication(KCmdLineArgs::qtArgc(), KCmdLineArgs::qtArgv()) {
        FAKE_KAPPLICATION_CTOR
    }
    KApplication(int &argc, char **argv) : QApplication(argc, argv) {
        FAKE_KAPPLICATION_CTOR
    }

    static QApplication* kApplication() { return qApp; }
//     TODO if needed turn into a decorator for QApplication...
//     KApplication() {}
//     static KApplication* instance() {
//         static KApplication* s_instance;
//         if (!s_instance)
//             s_instance = new KApplication();
//         return s_instance;
//     }
};

#define kapp qApp

#endif
 
