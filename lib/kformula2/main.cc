
#include <iostream>

#include <qapplication.h>
#include <qlist.h>
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

//     IndexElement* indexElement = cursor.getAktiveIndexElement();
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
    KFormulaContainer container;
    KFormulaWidget* mw = new KFormulaWidget(&container);
    mw->setCaption("Test of the formula engine");
    mw->show();
    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
    return app.exec();
}
