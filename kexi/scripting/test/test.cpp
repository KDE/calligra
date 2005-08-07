/***************************************************************************
 * test.cpp
 * This file is part of the KDE project
 * copyright (C)2004-2005 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 ***************************************************************************/

#include "../main/manager.h"

#include "../api/object.h"
#include "../api/class.h"
#include "../api/module.h"
//#include "../api/script.h"
#include "../api/interpreter.h"

#include "../main/scriptcontainer.h"

#include "../kexidb/kexidbmodule.h"

#include "testobject.h"

// Qt
#include <qstring.h>
#include <qfile.h>

// KDE
#include <kdebug.h>
#include <kinstance.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <ksharedptr.h>

// for std namespace
#include <string>
#include <iostream>

KApplication *app = 0;

static KCmdLineOptions options[] =
{
    { "interpreter <interpretername>", I18N_NOOP("Name of the used interpreter"), "python" },
    { "scriptfile <filename>", I18N_NOOP("Scriptfile to execute with the defined interpreter"), "test.py" },
    //{ "functionname <functioname>", I18N_NOOP("Execute the function in the defined scriptfile."), "" },
    //{ "functionargs <functioarguments>", I18N_NOOP("List of arguments to pass to the function on execution."), "" },
    { 0, 0, 0 }
};

void runInterpreter(const QString& interpretername, const QString& scriptcode)
{
    // Return the scriptingmanager instance. The manager is used as main
    // entry point to work with Kross.
    Kross::Api::Manager* manager = Kross::Api::Manager::scriptManager();
    if(! manager) {
        kdWarning() << "Failed to get Kross::Api::Manager instance!" << endl;
        return;
    }

    //TESTCASE
    TestObject* testobject = new TestObject(app);
    manager->addQObject( testobject );

    // Add modules that should be accessible by scripting. Those
    // modules are wrappers around functionality you want to be
    // able to access from within scripts. You don't need to take
    // care of freeing them cause that will be done by Kross.
    // Modules are shared between the ScriptContainer instances.
    Kross::KexiDB::KexiDBModule* kdbm = new Kross::KexiDB::KexiDBModule();
    manager->addChild(kdbm);
    manager->addChild( new Kross::KexiDB::TestModule() ); //testcase

    // To represent a script that should be executed Kross uses
    // the Script container class. You are able to fill them with
    // what is needed and just execute them.
    Kross::Api::ScriptContainer::Ptr scriptcontainer = manager->getScriptContainer("MyScriptName");

    //scriptcontainer->enableModule("KexiDB");

    scriptcontainer->setInterpreterName(interpretername);
    scriptcontainer->setCode(scriptcode);

    try {
        //scriptcontainer->addSlot("stdout", testobject, SLOT(stdoutSlot(const QString&)));
        scriptcontainer->addSignal("stdout", testobject, SIGNAL(stdoutSignal(const QString&)));
        //scriptcontainer->addSlot("stderr", testobject, SLOT(stderrSlot(const QString&)));

        scriptcontainer->addQObject( testobject );
        //scriptcontainer->addSignal("myTestSignal", testobject, SIGNAL(testSignal()));
        //scriptcontainer->addSlot("myTestSlot", testobject, SLOT(testSlot()));

        /*Kross::Api::Object* o =*/ scriptcontainer->execute();

        // Call a function.
        kdDebug()<<"--------------------------"<<endl;
        scriptcontainer->callFunction("testobjectCallback" /*, Kross::Api::List* functionarguments */);

        // Call a class.
        /*
        kdDebug()<<"--------------------------"<<endl;
        Kross::Api::Object* testclassinstance = scriptcontainer->classInstance("testClass");
        if(testclassinstance) {
            QValueList<Kross::Api::Object*> ll;
            Kross::Api::Object* instancecallresult = testclassinstance->call("testClassFunction1", Kross::Api::List::create(ll));
            //kdDebug() << QString("testClass.testClassFunction1 returnvalue => '%1'").arg( instancecallresult.toString() ) << endl;
        }
        kdDebug()<<"--------------------------"<<endl;
        */


/*
        // Connect QObject signal with scriptfunction.
        scriptcontainer->connect(testobject, SIGNAL(testSignal()), "testobjectCallback");
        scriptcontainer->connect(testobject, SIGNAL(testSignalString(const QString&)), "testobjectCallbackWithParams");
        // Call the testSlot to emit the testSignal.
        testobject->testSlot();
*/
std::string s; std::cin >> s; // just wait.

    }
    catch(Kross::Api::Exception& e) {
        std::cout << QString("EXCEPTION type=%1 description=%2").arg(e.type()).arg(e.description()).latin1() << std::endl;
    }

/*TESTCASE
    Kross::Api::ScriptContainer* sc2 = manager->getScriptContainer("MyScriptName222");
    sc2->setInterpreterName(interpretername);
    sc2->setCode(scriptcode);
    try {
        sc2->execute();
    }
    catch(Kross::Api::Exception& e) {
        kdDebug() << QString("EXCEPTION type='%1' description='%2'").arg(e.type()).arg(e.description()) << endl;
    }
    //delete sc2;
*/
}

int main(int argc, char **argv)
{
    int result = 0;

    KCmdLineArgs::init(argc, argv,
        new KAboutData("KrossTest", "KrossTest",
            "0.1", "", KAboutData::License_LGPL,
            "(c)2004-2005 Sebastian Sauer\n"
            "http://www.koffice.org/kexi\n"
            "http://www.dipe.org/kross",
            "kross@dipe.org"
        )
    );
    KCmdLineArgs::addCmdLineOptions(options);
    app = new KApplication(true, true);
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    QString interpretername = args->getOption("interpreter");
    QString filename = args->getOption("scriptfile");

    QFile f(QFile::encodeName(filename));
    if(f.exists() && f.open(IO_ReadOnly)) {
        QString scriptcode = f.readAll();
        f.close();
        runInterpreter(interpretername, scriptcode);
    }
    else {
        kdWarning() << "Failed to load scriptfile: " << filename << endl;
        result = -1;
    }

    delete app;
    return result;
}
