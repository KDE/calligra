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
    { "+[URL]", I18N_NOOP( "Document to open." ), 0 },
    KCmdLineLastOption
};

int main(int argc, char **argv)
{
    KAboutData about("kformdesigner", I18N_NOOP("KFormDesigner"), version, description,
                     KAboutData::License_LGPL, "(C) 2003 Lucijan Busch", 0, 0, "lucijan@kde.org");
    about.addAuthor( "Lucijan Busch", 0, "lucijan@kde.org" );
    about.addAuthor( "Cedric Pasteur", 0, "cedric.pasteur@free.fr");
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions(options);
    KApplication app;

//    container->show();

/*
    QTabWidget *tab = new QTabWidget();
    QWidget *container = new QWidget(tab);
    KFormDesigner::ContainerFactory *c = new KFormDesigner::ContainerFactory(0, container);
    QPushButton *p = new QPushButton("Pushbutton", container);
    c->addWidget(p, QRect(10, 10, 80, 30));
    tab->addTab(container, "Page 1");
    tab->show();
    QButtonGroup *btn = new QButtonGroup("buttons", container);
    c->addWidget(btn, QRect(10, 50, 150, 150));
    KFormDesigner::ContainerFactory *btnC = new KFormDesigner::ContainerFactory(c, btn);
    QPushButton *btn2 = new QPushButton("PB", btn);
    btnC->addWidget(btn2, QRect(10, 10, 80, 30));

    new KFormDesigner::WidgetLibrary();

    app.setMainWidget(tab);
*/
   KGlobal::iconLoader()->addAppDir("kexi");

    KFMView *v = new KFMView();
    v->show();
    app.setMainWidget(v);


    /*
    // see if we are starting with session management
    if (app.isRestored())
    {
        RESTORE(KFormDesigner);
    }
    else
    {
        // no session.. just start up normally
        KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
        if (args->count() == 0)
        {
            KFormDesigner *widget = new KFormDesigner;
            widget->show();
        }
        else
        {
            int i = 0;
            for (; i < args->count(); i++)
            {
                KFormDesigner *widget = new KFormDesigner;
                widget->show();
                widget->load(args->url(i));
            }
        }
        args->clear();
    }
    */

    return app.exec();
}
