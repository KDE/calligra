
#include <iostream>

// qt
//#include <qapplication.h>

// kde
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>

// unit test
#include <cppunit/ui/text/TestRunner.h>

// local
#include "basicelement.h"
#include "elementtype.h"
#include "kformulacommand.h"
#include "testcommands.h"
#include "testformulacursor.h"
#include "testformulaelement.h"
#include "testindexelement.h"


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

    CppUnit::TextUi::TestRunner runner;

    runner.addTest(TestFormulaCursor::suite());
    runner.addTest(TestFormulaElement::suite());
    runner.addTest(TestIndexElement::suite());
    runner.addTest(TestCommands::suite());
    runner.run();

    // Make sure there are no elements in the clipboard.
    // Okey for a debug app.
    QApplication::clipboard()->clear();

    int destruct = BasicElement::getEvilDestructionCount();
    if (destruct != 0) {
        std::cerr << "BasicElement::EvilDestructionCount: " << destruct << std::endl;
    }
    destruct = Command::getEvilDestructionCount();
    if (destruct != 0) {
        std::cerr << "Command::EvilDestructionCount: " << destruct << std::endl;
    }
    destruct = ElementType::getEvilDestructionCount();
    if (destruct != 0) {
        std::cerr << "ElementType::EvilDestructionCount: " << destruct << std::endl;
    }

    return 0;
}
