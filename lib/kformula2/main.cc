#include <qfontdatabase.h>

#include <iostream>
#include <memory>


#include <qaccel.h>
#include <qapplication.h>
#include <qlayout.h>
#include <qlist.h>
#include <qmainwindow.h>
#include <qpainter.h>
#include <qwidget.h>

#include "kformulacontainer.h"
#include "kformulawidget.h"


int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    KFormulaContainer* container = new KFormulaContainer;
    KFormulaWidget* mw1 = new KFormulaWidget(container, 0, "test1");
    KFormulaWidget* mw2 = new KFormulaWidget(container, 0, "test2");

    QAccel ac1(mw1);
    ac1.insertItem(Qt::CTRL + Qt::Key_Q, 1);
    ac1.connectItem(1, &app, SLOT(quit()));

    QAccel ac2(mw2);
    ac2.insertItem(Qt::CTRL + Qt::Key_Q, 1);
    ac2.connectItem(1, &app, SLOT(quit()));

    mw1->setCaption("Test1 of the formula engine");
    mw2->setCaption("Test2 of the formula engine");

    mw1->show();
    mw2->show();
    int result = app.exec();

    delete container;
    
    int destruct = BasicElement::getEvilDestructionCount();
    if (destruct != 0) {
        cerr << "EvilDestructionCount: " << destruct << endl;
    }
    else {
        //cerr << "Clean destruction!\n";
    }

    return result;
}
