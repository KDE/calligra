
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


// KFormulaWidget::KFormulaWidget()
//     : QWidget(), rootElement(this), cursor(&rootElement)
// {
//     rootElement.setX(10);
//     rootElement.setY(10);
    
//     QList<BasicElement> list;
//     list.setAutoDelete(true);
    
//     list.append(new TextElement('T'));
//     list.append(new TextElement('e'));
//     list.append(new TextElement('s'));
//     list.append(new TextElement('t'));

//     list.append(new IndexElement());

//     cursor.insert(list);

//     IndexElement* indexElement = cursor.getActiveIndexElement();
//     if (indexElement != 0) {
//         //cout << "indexElement != 0\n";
        
//         // that's how to insert an index.
//         indexElement->setToLowerRight(&cursor);
//         list.append(new SequenceElement());
//         cursor.insert(list);

//         list.append(new TextElement('T'));
//         list.append(new TextElement('e'));
//         list.append(new TextElement('s'));
//         list.append(new TextElement('t'));
//         cursor.insert(list);
//     }

//     rootElement.calcSizes(context, 100);
//     rootElement.output(cout) << endl << endl;
// }


int main(int argc, char** argv)
{
    QApplication app(argc, argv);
//     QMainWindow mainwin(0, "mainwin");

//     QWidget content(&mainwin);
//     QVBoxLayout layout(&content, 10);

//     mainwin.setCentralWidget(&content);
//     mainwin.setCaption("Test of the formula engine");


    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    KFormulaContainer* container = new KFormulaContainer;
    KFormulaWidget* mw1 = new KFormulaWidget(container, 0, "test1");
    KFormulaWidget* mw2 = new KFormulaWidget(container, 0, "test2");
//     layout.addWidget(mw1, 1);
//     layout.addWidget(mw2, 1);
//     mw1->setFocus();

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
    //mainwin.show();
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
