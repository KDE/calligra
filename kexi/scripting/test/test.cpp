/***************************************************************************
 * test.cpp
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

#include <qstring.h>
#include <qfile.h>

#include <kdebug.h>
#include <kinstance.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

#include "../main/manager.h"

#include "../api/object.h"
#include "../api/class.h"
//#include "../api/module.h"

#include "../api/interpreter.h"
#include "../kexidb/kexidbmodule.h"

KApplication *app = 0;

static KCmdLineOptions options[] =
{
    { "interpreter <interpretername>", I18N_NOOP("Name of the used interpreter."), "python" },
    { "scriptfile <filename>", I18N_NOOP("Scriptfile to execute with the defined interpreter."), "test.py" },
    { 0, 0, 0 }
};

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

        if(! interpreter->execute(script))
            kdWarning() << "Interpreter failed to execute script!" << endl;

        //TODO garbage collect them too?
        delete module; module = 0;
        delete interpreter; interpreter = 0;
    }
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
        QString script = f.readAll();
        f.close();
        runInterpreter(interpretername, script);

        /*TODO
        //second execution crashes cause it seems we don't cleaned
        //everything well at this point. Grrr, guess to use KShared
        //wasn't the best design-decision :-/

        runInterpreter(interpretername, script);
        */
    }
    else {
        kdWarning() << "Failed to load scriptfile: " << filename << endl;
        result = -1;
    }

    delete app;
    return result;
}
