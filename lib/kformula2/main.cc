
#include <iostream>
#include <memory>

#include <qaccel.h>
#include <qlayout.h>
#include <qlist.h>
#include <qmainwindow.h>
#include <qpainter.h>
#include <qwidget.h>

#include <kapp.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>

#include "kformulacommand.h"
#include "kformulacontainer.h"
#include "kformulawidget.h"


static const KCmdLineOptions options[]= {
    {0,0,0}
};

int main(int argc, char** argv)
{
    KAboutData aboutData("formula engine test", "KFormula test",
                         "0.01", "test", KAboutData::License_GPL,
                         "(c) 2001, Andrea Rizzi, Ulrich Kuettler");
    aboutData.addAuthor("Andrea Rizzi",0, "rizzi@kde.org");
    aboutData.addAuthor("Ulrich Kuettler",0, "ulrich.kuettler@mailbox.tu-dresden.delete");

    KCmdLineArgs::init(argc, argv, &aboutData);
    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app;

    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    KFormulaContainer* container = new KFormulaContainer;
    KFormulaWidget* mw1 = new KFormulaWidget(container, 0, "test1");
    KFormulaWidget* mw2 = new KFormulaWidget(container, 0, "test2");

//     QAccel ac1(mw1);
//     ac1.insertItem(Qt::CTRL + Qt::Key_Q, 1);
//     ac1.connectItem(1, &app, SLOT(quit()));

//     QAccel ac2(mw2);
//     ac2.insertItem(Qt::CTRL + Qt::Key_Q, 1);
//     ac2.connectItem(1, &app, SLOT(quit()));

    mw1->setCaption("Test1 of the formula engine");
    mw2->setCaption("Test2 of the formula engine");

    mw1->show();
    mw2->show();
    int result = app.exec();

    delete container;
    
    int destruct = BasicElement::getEvilDestructionCount();
    if (destruct != 0) {
        cerr << "BasicElement::EvilDestructionCount: " << destruct << endl;
    }
    destruct = KFormulaCommand::getEvilDestructionCount();
    if (destruct != 0) {
        cerr << "KFormulaCommand::EvilDestructionCount: " << destruct << endl;
    }

    return result;
}
