/*
 * Copyright (C) 2003 Cédric Pasteur <cedric.pasteur@free.fr>
 */

#include "test.h"
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

static const char description[] = "A KDE KPart Application";

static const char version[] = "0.1";

static KCmdLineOptions options[] =
{
//    { "+[URL]", I18N_NOOP( "Document to open" ), 0 },
    KCmdLineLastOption
};

int main(int argc, char **argv)
{
    KAboutData about("test", "test", version, description,
                     KAboutData::License_GPL, "(C) 2003 Cédric Pasteur", 0, 0, "cedric.pasteur@free.fr");
    about.addAuthor( "Cédric Pasteur", 0, "cedric.pasteur@free.fr" );
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions( options );
    KApplication app;
    test *mainWin = 0;

    if (app.isRestored())
    {
        RESTORE(test);
    }
    else
    {
        // no session.. just start up normally
        KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

        /// @todo do something with the command line args here

        mainWin = new test();
        app.setMainWidget( mainWin );
        mainWin->show();

        args->clear();
    }

    // mainWin has WDestructiveClose flag by default, so it will delete itself.
    return app.exec();
}

