#include <kformulaedit.h>
#include <kformulatoolbar.h>
#include <ktmainwindow.h>
#include <kapp.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qnamespace.h>
#include <qaccel.h>

//Simple test for kformula.  Doesn't test evaluation, though.

int main( int argc, char **argv )
{
    KApplication a( argc, argv, "KFormulaTest" );

    KTMainWindow w;
    w.resize( 640, 480 );

    KFormulaToolBar bar(&w);

    QWidget tmp(&w);
    QVBoxLayout l(&tmp, 10);

    w.setView(&tmp);

    QAccel ac(&w);

    ac.insertItem(Qt::CTRL + Qt::Key_Q, 1);
    ac.connectItem(1, qApp, SLOT(quit()));

    KFormulaEdit x(&tmp);

    l.addWidget(&x, 1);

    QLabel lab("Press CTRL+Q to quit.  Try CTRL + 234568[]./^-_", &tmp);

    lab.setFont(QFont("courier", 20));

    l.addWidget(&lab, 0);

    bar.connectToFormula(&x);

    w.addToolBar(&bar);

    w.enableToolBar();

    a.setMainWidget( &w );
    x.setFocus();
    w.show();
    return a.exec();
}                                 










