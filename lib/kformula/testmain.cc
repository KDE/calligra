
#include <iostream>

// qt
//#include <qapplication.h>

// kde
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>

// unit test
#include <TestRunner.h>

// local
#include "basicelement.h"
#include "elementtype.h"
#include "kformulacommand.h"
#include "testcommands.h"
#include "testformulacursor.h"
#include "testformulaelement.h"
#include "testindexelement.h"


static char* array[] = { "", "TestFormulaCursor", "TestFormulaElement",
                         "TestIndexElement", "TestCommands" };

static const KCmdLineOptions options[]= {
    {0,0,0}
};

int main (int argc, char **argv)
{
    using namespace KFormula;

    //QApplication app(argc, argv);
    KAboutData aboutData("formula engine test", "KFormula test",
                         "0.01", "test", KAboutData::License_GPL,
                         "(c) 2001, Andrea Rizzi, Ulrich Kuettler");
    aboutData.addAuthor("Andrea Rizzi",0, "rizzi@kde.org");
    aboutData.addAuthor("Ulrich Kuettler",0, "ulrich.kuettler@mailbox.tu-dresden.de");

    KCmdLineArgs::init(argc, argv, &aboutData);
    KCmdLineArgs::addCmdLineOptions(options);
    KApplication app;

    TestRunner runner;

    runner.addTest("TestFormulaCursor", TestFormulaCursor::suite());
    runner.addTest("TestFormulaElement", TestFormulaElement::suite());
    runner.addTest("TestIndexElement", TestIndexElement::suite());
    runner.addTest("TestCommands", TestCommands::suite());
    runner.run( 5, array );

    // Make sure there are no elements in the clipboard.
    // Okey for a debug app.
    QApplication::clipboard()->clear();

    int destruct = BasicElement::getEvilDestructionCount();
    if (destruct != 0) {
        cerr << "BasicElement::EvilDestructionCount: " << destruct << endl;
    }
    destruct = Command::getEvilDestructionCount();
    if (destruct != 0) {
        cerr << "Command::EvilDestructionCount: " << destruct << endl;
    }
    destruct = ElementType::getEvilDestructionCount();
    if (destruct != 0) {
        cerr << "ElementType::EvilDestructionCount: " << destruct << endl;
    }

    return 0;
}
