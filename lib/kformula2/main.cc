
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
#include <kcommand.h>

#include "elementtype.h"
#include "kformulacommand.h"
#include "kformulacontainer.h"
#include "kformuladocument.h"
#include "kformulawidget.h"


class TestWidget : public KFormulaWidget {
public:
    TestWidget(KFormulaContainer* doc, QWidget* parent=0, const char* name=0, WFlags f=0)
            : KFormulaWidget(doc, parent, name, f) {}

protected:
    virtual void keyPressEvent(QKeyEvent* event);
};


void TestWidget::keyPressEvent(QKeyEvent* event)
{
    KFormulaContainer* document = getDocument();
    
    //int action = event->key();
    int state = event->state();
    //MoveFlag flag = movementFlag(state);

    if (state & Qt::ControlButton) {
        switch (event->key()) {
            case Qt::Key_1: document->addSum(); return;
            case Qt::Key_2: document->addProduct(); return;
            case Qt::Key_3: document->addIntegral(); return;
            case Qt::Key_4: document->addRoot(); return;
            case Qt::Key_5: document->addFraction(); return;
            case Qt::Key_6: document->addMatrix(); return;
            case Qt::Key_A: slotSelectAll(); return;
            case Qt::Key_C: document->copy(); return;
            case Qt::Key_D: document->replaceElementWithMainChild(BasicElement::afterCursor); return;
            case Qt::Key_L: document->addGenericLowerIndex(); return;
            case Qt::Key_O: document->load("test.xml"); return;
            case Qt::Key_Q: kapp->quit(); return;
            case Qt::Key_R: document->replaceElementWithMainChild(BasicElement::beforeCursor); return;
            case Qt::Key_S: document->save("test.xml"); return;
            case Qt::Key_U: document->addGenericUpperIndex(); return;
            case Qt::Key_V: document->paste(); return;
            case Qt::Key_X: document->cut(); return;
            case Qt::Key_Z: (state & Qt::ShiftButton) ? document->getDocument()->redo() : document->getDocument()->undo(); return;
            default:
                //cerr << "Key: " << event->key() << endl;
                break;
        }
    }

    KFormulaWidget::keyPressEvent(event);
}


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

    KFormulaDocument* document = new KFormulaDocument;
    KFormulaContainer* container = new KFormulaContainer(document);
    KFormulaWidget* mw1 = new TestWidget(container, 0, "test1");
    KFormulaWidget* mw2 = new TestWidget(container, 0, "test2");

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
    delete document;
    
    int destruct = BasicElement::getEvilDestructionCount();
    if (destruct != 0) {
        cerr << "BasicElement::EvilDestructionCount: " << destruct << endl;
    }
    destruct = KFormulaCommand::getEvilDestructionCount();
    if (destruct != 0) {
        cerr << "KFormulaCommand::EvilDestructionCount: " << destruct << endl;
    }
    destruct = ElementType::getEvilDestructionCount();
    if (destruct != 0) {
        cerr << "ElementType::EvilDestructionCount: " << destruct << endl;
    }

    return result;
}
