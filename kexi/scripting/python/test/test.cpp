/***************************************************************************
 * main.cpp
 * copyright (C)2003 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 ***************************************************************************/

#include <Python.h>

//#include <stdlib.h>
//#include <cstdlib>
//#include <string>
//#include <sstream>
#include <iostream>

//#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>
//#include <qmap.h>
#include <qfile.h>

#include <kinstance.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include "kexidb/pythonkexidb.h"
#include "kexidb/pythonkexidbdriver.h"

#include "CXX/Objects.hxx"

KApplication *app = 0;
KInstance *instance = 0;

bool test_simple(int argc, char **argv)
{
    PyEval_AcquireLock();
    PyThreadState *tstate = Py_NewInterpreter();
    if(! tstate) {
        std::cerr << "Py_NewInterpreter() failed" << std::endl;
        exit(1);
    }
    PySys_SetArgv(argc, argv) ;

    Kross::PythonKexiDB* database = new Kross::PythonKexiDB();
    Py::Module module = database->module();
    //std::cout << "module->as_string() = " << module.as_string() << std::endl;

    QFile f( QFile::encodeName("test/test.py") );
    if(! f.open(IO_ReadOnly)) return false;
    QString data = f.readAll();
    f.close();

    PyRun_SimpleString(data);

    delete database;

    Py_EndInterpreter(tstate);
    PyEval_ReleaseLock();

    return true;
}

int main(int argc, char **argv)
{
    KCmdLineArgs::init(argc, argv,
        new KAboutData("test", "KrossTest",
            "0.1", "", KAboutData::License_GPL,
            "(c) 2004, Sebastian Sauer (mail@dipe.org)\n"
            "http://www.koffice.org/kexi\n"
            "http://www.dipe.org/kross",
            "kross@dipe.org"
        )
    );
    app = new KApplication(true, true);
    instance = new KInstance("test");

    Py_SetProgramName("Kross");

    Py_Initialize();
    PyEval_InitThreads();
    PyThreadState *gtstate = PyEval_SaveThread();
    if(! gtstate) {
        std::cerr << "PyEval_SaveThread() failed" << std::endl;
        exit(1);
    }

    std::cout << "##############################################" << std::endl;
    test_simple(argc, argv);
    //test_simple(argc, argv);
    std::cout << "##############################################" << std::endl;

    PyEval_AcquireThread(gtstate);
    Py_Finalize();

    delete instance;
    delete app;
    return 0;
}
