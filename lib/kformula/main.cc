
#include <iostream>
#include <memory>

#include <qaccel.h>
#include <qdom.h>
#include <qfile.h>
#include <qlayout.h>
#include <qptrlist.h>
#include <qmainwindow.h>
#include <qpainter.h>
#include <qstring.h>
#include <qtextstream.h>
#include <qwidget.h>
#include <qfileinfo.h>

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kcommand.h>
#include <kdebug.h>
#include <kfiledialog.h>

#include "elementtype.h"
#include "kformulacommand.h"
#include "kformulacontainer.h"
#include "kformuladocument.h"
#include "kformulawidget.h"
#include "scrollview.h"

using namespace KFormula;


class TestWidget : public KFormulaWidget {
public:
    TestWidget(Container* doc, QWidget* parent=0, const char* name=0, WFlags f=0)
            : KFormulaWidget(doc, parent, name, f) {}

protected:
    virtual void keyPressEvent(QKeyEvent* event);

private:
};


void save( QString filename, QDomDocument doc )
{
    QFile f( filename );
    if(!f.open(IO_Truncate | IO_ReadWrite)) {
        kdWarning( DEBUGID ) << "Error opening file " << filename.latin1() << endl;
        return;
    }

    QTextStream stream(&f);
    stream.setEncoding(QTextStream::UnicodeUTF8);
    doc.save(stream,4);
    f.close();
}


void load( KFormula::Document* document, QString filename )
{
    QFile f(filename);
    if (!f.open(IO_ReadOnly)) {
        kdWarning( DEBUGID ) << "Error opening file " << filename.latin1() << endl;
        return;
    }
    QTextStream stream(&f);
    stream.setEncoding(QTextStream::UnicodeUTF8);
    QString content = stream.read();
    f.close();
    //kdDebug( DEBUGID ) << content << endl;
    QDomDocument doc;
    if (!doc.setContent(content)) {
        return;
    }
    if ( !document->loadXML( doc ) ) {
        kdWarning( DEBUGID ) << "Failed." << endl;
    }
}


void saveMathML( KFormula::Container* formula, QString filename )
{
    QFile f( filename );
    if ( !f.open( IO_Truncate | IO_ReadWrite ) ) {
        kdWarning( DEBUGID ) << "Error opening file " << filename.latin1() << endl;
        return;
    }

    QTextStream stream( &f );
    stream.setEncoding( QTextStream::UnicodeUTF8 );
    formula->saveMathML( stream );
    f.close();
}


void loadMathML( KFormula::Container* formula, QString filename )
{
    QFile f( filename );
    if ( !f.open( IO_ReadOnly ) ) {
        kdWarning( DEBUGID ) << "Error opening file " << filename.latin1() << endl;
        return;
    }
    QTextStream stream( &f );
    stream.setEncoding( QTextStream::UnicodeUTF8 );
    QString content = stream.read();

    QDomDocument doc;
    QString errorMsg;
    int errorLine;
    int errorColumn;
    if ( !doc.setContent( content, true,
                          &errorMsg, &errorLine, &errorColumn ) ) {
        kdWarning( DEBUGID ) << "MathML built error: " << errorMsg
                             << " at line " << errorLine
                             << " and column " << errorColumn << endl;
        f.close();
        return;
    }

    /*kdDebug( DEBUGID ) << "Container::loadMathML\n"
      << doc.toCString() << endl;*/

    if ( !formula->loadMathML( doc ) ) {
        kdWarning( DEBUGID ) << "Failed." << endl;
    }
    f.close();
}


void TestWidget::keyPressEvent(QKeyEvent* event)
{
    Container* document = getDocument();

    //int action = event->key();
    int state = event->state();
    //MoveFlag flag = movementFlag(state);

    if ( ( state & Qt::ShiftButton ) && ( state & Qt::ControlButton ) ) {
        switch (event->key()) {
            case Qt::Key_B: document->document()->appendColumn(); return;
            case Qt::Key_I: document->document()->insertColumn(); return;
            case Qt::Key_R: document->document()->removeColumn(); return;
            case Qt::Key_Z: document->document()->redo(); return;

            case Qt::Key_M: saveMathML( document, "test.mml" ); return;
            case Qt::Key_O: {
                QString file = KFileDialog::getOpenFileName();
                kdDebug( DEBUGID ) << file << endl;
                if( !file.isEmpty() ) {
                    QFileInfo fi( file );
                    if ( fi.extension() == "mml" ) {
                        loadMathML( document, file );
                    }
                    else if ( fi.extension() == "xml" ) {
                        load( document->document(), file );
                    }
                }
                return;
        }
        }
    }
    else if (state & Qt::ControlButton) {
        switch (event->key()) {
            case Qt::Key_1: document->document()->addSum(); return;
            case Qt::Key_2: document->document()->addProduct(); return;
            case Qt::Key_3: document->document()->addIntegral(); return;
            case Qt::Key_4: document->document()->addRoot(); return;
            case Qt::Key_5: document->document()->addFraction(); return;
            case Qt::Key_6: document->document()->addMatrix(); return;
	    case Qt::Key_7: document->document()->addOneByTwoMatrix(); return;
	    case Qt::Key_8: document->document()->addOverline(); return;
	    case Qt::Key_9: document->document()->addUnderline(); return;
            case Qt::Key_A: slotSelectAll(); return;
            case Qt::Key_B: document->document()->appendRow(); return;
            case Qt::Key_C: document->document()->copy(); return;
            case Qt::Key_D: document->document()->removeEnclosing(); return;
            case Qt::Key_G: document->document()->makeGreek(); return;
            case Qt::Key_I: document->document()->insertRow(); return;
            case Qt::Key_R: document->document()->removeRow(); return;
            case Qt::Key_K: document->document()->addMultiline(); return;
            case Qt::Key_L: document->document()->addGenericLowerIndex(); return;
            case Qt::Key_M: loadMathML( document, "test.mml" ); return;
            case Qt::Key_O: load( document->document(), "test.xml" ); return;
            case Qt::Key_Q: kapp->quit(); return;
            case Qt::Key_S: save( "test.xml", document->document()->saveXML() ); return;
            case Qt::Key_T: std::cout << document->texString().latin1() << std::endl; return;
            case Qt::Key_U: document->document()->addGenericUpperIndex(); return;
            case Qt::Key_V: document->document()->paste(); return;
            case Qt::Key_X: document->document()->cut(); return;
            case Qt::Key_Z: document->document()->undo(); return;
            default:
                //std::cerr << "Key: " << event->key() << std::endl;
                break;
        }
    }

    KFormulaWidget::keyPressEvent(event);
}


ScrollView::ScrollView()
        : QScrollView(), child(0)
{
}

void ScrollView::addChild(KFormulaWidget* c, int x, int y)
{
    QScrollView::addChild(c, x, y);
    child = c;
    connect(child, SIGNAL(cursorChanged(bool, bool)),
            this, SLOT(cursorChanged(bool, bool)));
}

void ScrollView::focusInEvent(QFocusEvent*)
{
    if (child != 0) child->setFocus();
}


void ScrollView::cursorChanged(bool visible, bool /*selecting*/)
{
    if (visible) {
        int x = child->getCursorPoint().x();
        int y = child->getCursorPoint().y();
        ensureVisible(x, y);
    }
}


static const KCmdLineOptions options[]= {
    { "+file", "File to Open", 0 },
    KCmdLineLastOption
};

int main(int argc, char** argv)
{
    KAboutData aboutData("formula engine test", "KFormula test",
                         "0.01", "test", KAboutData::License_GPL,
                         "(c) 2001, Andrea Rizzi, Ulrich Kuettler");
    aboutData.addAuthor("Andrea Rizzi",0, "rizzi@kde.org");
    aboutData.addAuthor("Ulrich Kuettler",0, "ulrich.kuettler@mailbox.tu-dresden.de");

    KCmdLineArgs::init(argc, argv, &aboutData);
    KCmdLineArgs::addCmdLineOptions(options);

    KApplication app;

    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));

    Document* document = new Document( kapp->config() );
    Container* container1 = new Container( document );
    Container* container2 = new Container( document );

    ScrollView* scrollview1a = new ScrollView;
    ScrollView* scrollview1b = new ScrollView;
    ScrollView* scrollview2a = new ScrollView;
    ScrollView* scrollview2b = new ScrollView;

    KFormulaWidget* mw1a = new TestWidget(container1, scrollview1a, "test1a");
    KFormulaWidget* mw1b = new TestWidget(container1, scrollview1b, "test1b");
    KFormulaWidget* mw2a = new TestWidget(container2, scrollview2a, "test2a");
    KFormulaWidget* mw2b = new TestWidget(container2, scrollview2b, "test2b");

    scrollview1a->addChild(mw1a);
    scrollview1b->addChild(mw1b);
    scrollview2a->addChild(mw2a);
    scrollview2b->addChild(mw2b);

    scrollview1a->setCaption("Test1a of the formula engine");
    scrollview1b->setCaption("Test1b of the formula engine");
    scrollview2a->setCaption("Test2a of the formula engine (ro)");
    scrollview2b->setCaption("Test2b of the formula engine");

    scrollview1a->show();
    scrollview1b->show();
    scrollview2a->show();
    scrollview2b->show();

    // to keep things interessting
    mw2a->setReadOnly(true);

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    for ( int i = 0; i < args->count(); ++i ) {
        QFileInfo fi( args->url( i ).path() );
        if ( fi.extension() == "mml" )
            loadMathML( container2, args->url( i ).path() );
        else if ( fi.extension() == "xml" )
            load( container2->document(), args->url( i ).path() );
    }

    int result = app.exec();

    delete container2;
    delete container1;
    delete document;

    // Make sure there are no elements in the clipboard.
    // Okey for a debug app.
    QApplication::clipboard()->clear();

    int destruct = BasicElement::getEvilDestructionCount();
    if (destruct != 0) {
        std::cerr << "BasicElement::EvilDestructionCount: " << destruct << std::endl;
    }
    destruct = PlainCommand::getEvilDestructionCount();
    if (destruct != 0) {
        std::cerr << "PlainCommand::EvilDestructionCount: " << destruct << std::endl;
    }
    destruct = ElementType::getEvilDestructionCount();
    if (destruct != 0) {
        std::cerr << "ElementType::EvilDestructionCount: " << destruct << std::endl;
    }

    return result;
}

#include "scrollview.moc"
