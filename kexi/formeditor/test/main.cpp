/*
 * Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
 */

#include "container.h"
#include "widgetlibrary.h"

#include "kfmview.h"

#include <qtabwidget.h>
#include <qpushbutton.h>
#include <qrect.h>
#include <qbuttongroup.h>

#include <kapplication.h>
#include <kiconloader.h>
#include <dcopclient.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

static const char *description =
    I18N_NOOP("KFormDesigner");

static const char *version = "0.1";

static KCmdLineOptions options[] =
{
    { "+[URL]", I18N_NOOP( "Document to open" ), 0 },
    KCmdLineLastOption
};

int main(int argc, char **argv)
{
    KAboutData about("kformdesigner", I18N_NOOP("KFormDesigner"), version, description,
                     KAboutData::License_LGPL, "(C) 2003-2004 Kexi Team", 0, 0);
    about.addAuthor( "Lucijan Busch", 0, "lucijan@kde.org" );
    about.addAuthor( "Cedric Pasteur", 0, "cedric.pasteur@free.fr");
    about.addCredit( "Jaroslaw Staniek", "Win32 version, some other little changes and lots of ideas", "js@iidea.pl", 0);
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions(options);
    KApplication app;

   KGlobal::iconLoader()->addAppDir("kexi");

    KFMView *v = new KFMView();
    v->show();
    app.setMainWidget(v);



    // see if we are starting with session management
	if (app.isRestored())
	{
		RESTORE(KFMView);
	}
	else
	{
	// no session.. just start up normally
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	if (args->count() >= 1)
	{
            for (int i = 0; i < args->count(); i++)
		v->loadUIFile(args->arg(i));
        }
        args->clear();
    }

    return app.exec();
}
