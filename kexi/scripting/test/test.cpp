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
//#include "../api/module.h"

#include "../api/script.h"
#include "../api/interpreter.h"
#include "../kexidb/kexidbmodule.h"

#include <qstring.h>
#include <qfile.h>

#include <kdebug.h>
#include <kinstance.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

KApplication *app = 0;

static KCmdLineOptions options[] =
{
    { "interpreter <interpretername>", I18N_NOOP("Name of the used interpreter."), "python" },
    { "scriptfile <filename>", I18N_NOOP("Scriptfile to execute with the defined interpreter."), "test.py" },
    //{ "functionname <functioname>", I18N_NOOP("Execute the function in the defined scriptfile."), "" },
    //{ "functionargs <functioarguments>", I18N_NOOP("List of arguments to pass to the function on execution."), "" },
    { 0, 0, 0 }
};

class TestObject : public QObject
{
        Q_OBJECT
    signals:
        void testSignal();
    public slots:
        void testSlot() {
            kdDebug() << "TestObject::testSlot called" << endl;
            emit testSignal();
        }
        void testSignalSlot() {
            kdDebug() << "TestObject::testSignalSlot called" << endl;
        }
    public:
        TestObject() : QObject(app) {
            kdDebug() << "TestObject::TestObject called" << endl;
            connect(this, SIGNAL(testSignal()), this, SLOT(testSignalSlot()));
        }
        ~TestObject() {
            kdDebug() << "TestObject::~TestObject called" << endl;
        }
};

/*
void runInterpreter(const QString& interpretername, const QString& script)
{
    Kross::Api::Manager* manager = new Kross::Api::Manager();
    if(! manager) {
        kdWarning() << "Failed to create Kross::Api::Manager instance!" << endl;
        return;
    }
    Kross::Api::Interpreter* interpreter = manager->getInterpreter(interpretername); // instance will be auto deleted if manager got deleted.
    if(! interpreter)
        kdWarning() << QString("Failed to create Kross::Api::Interpreter instance for interpreter '%1'!").arg(interpretername) << endl;
    else {
        // Publish some modules. The choosen interpreter will decide what to do with them.
        //interpreter->addModule( new Kross::KexiDB::TestModule() );
        Kross::KexiDB::KexiDBModule* module = new Kross::KexiDB::KexiDBModule();
        interpreter->addModule(module);

        if(! interpreter->setScript(script))
            kdWarning() << "Interpreter failed to parse script!" << endl;
        else {
            if(! interpreter->execute())
                kdWarning() << "Interpreter failed to execute script!" << endl;
        }
        //TODO garbage collect them too?
        delete module; module = 0;
    }
    delete manager;
}
*/

void runInterpreter(const QString& interpretername, const QString& scriptcode)
{
    // Create a manager instance. The manager is used as main entry point
    // to work with Kross. You should instanciate only one instance and
    // use it the whole lifetime of your application.
    Kross::Api::Manager* manager = new Kross::Api::Manager();
    if(! manager) {
        kdWarning() << "Failed to create Kross::Api::Manager instance!" << endl;
        return;
    }

    // Add modules that should be accessible by scripting. Those
    // modules are wrappers around functionality you want to be
    // able to access from within scripts. You don't need to take
    // care of freeing them cause that will be done by Kross.
    //manager->addModule( new Kross::KexiDB::TestModule() );
    manager->addModule( new Kross::KexiDB::KexiDBModule() );

    // To represent a script that shgould be executed Kross uses
    // the Script container class. You are able to fill them with
    // what is needed and just execute them.
    Kross::Api::Script* script = manager->getScript("MyScriptName");
    //script->enableModule("KexiDB");
    script->setInterpreter(interpretername);
    script->setCode(scriptcode);
    script->execute();
    //delete script; // not needed cause Kross::Api::Manager will take care of it.

    // Finally free our manager.
    delete manager;
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

        /*TODO
        //second execution crashes cause it seems we don't cleaned
        //everything well at this point. Grrr, guess to use KShared
        //wasn't the best design-decision :-/

        runInterpreter(interpretername, scriptcode);
        */
    }
    else {
        kdWarning() << "Failed to load scriptfile: " << filename << endl;
        result = -1;
    }

    delete app;
    return result;
}
