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

#include <qstring.h>
#include <qstringlist.h>
#include <qfile.h>

#include <kdebug.h>
#include <kinstance.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include "../CXX/Objects.hxx"
#include "../kexidb/pythonkexidb.h"
#include "../kexidb/pythonkexidbdriver.h"
#include "../main/pythonmanager.h"

KApplication *app = 0;

static KCmdLineOptions options[] =
{
    { "file <filename>", I18N_NOOP("Pythonfile to execute."), "test.py" },
    { 0, 0, 0}
};

int main(int argc, char **argv)
{
    int result = 0;

    KCmdLineArgs::init(argc, argv,
        new KAboutData("KrossPythonTest", "KrossPythonTest",
            "0.1", "", KAboutData::License_GPL,
            "(c) 2004, Sebastian Sauer (mail@dipe.org)\n"
            "http://www.koffice.org/kexi\n"
            "http://www.dipe.org/kross",
            "kross@dipe.org"
        )
    );
    KCmdLineArgs::addCmdLineOptions(options);
    app = new KApplication(true, true);
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    Kross::PythonManager* pymanager = new Kross::PythonManager("Kross");

    QFile f(QFile::encodeName( args->getOption("file") ));
    if(f.exists() && f.open(IO_ReadOnly)) {
        QString data = f.readAll();
        f.close();

        kdDebug() << "##############################################" << endl;
        pymanager->execute(data, QStringList() << "kexidb");
        kdDebug() << "##############################################" << endl;
    }
    else {
        kdWarning() << "Failed to load Python scriptfile: " << args->getOption("file") << endl;
        result = -1;
    }

    delete pymanager;
    delete app;
    return result;
}
