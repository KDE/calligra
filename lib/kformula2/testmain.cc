
#include <iostream>

// qt
#include <qapplication.h>

// unit test
#include <TestRunner.h>

// local
#include "basicelement.h"
#include "elementtype.h"
#include "kformulacommand.h"
#include "testcommands.h"
#include "testformulacursor.h"
#include "testindexelement.h"

 
int main (int argc, char **argv)
{
    QApplication app(argc, argv);
    TestRunner runner;
 
    runner.addTest("TestFormulaCursor", TestFormulaCursor::suite());
    runner.addTest("TestIndexElement", TestIndexElement::suite());
    runner.addTest("TestCommands", TestCommands::suite());
    runner.run(argc, argv);

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
    
    return 0;
}
