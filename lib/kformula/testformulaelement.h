
#ifndef TESTFORMULAELEMENT_H
#define TESTFORMULAELEMENT_H

#include <TestCaller.h>
#include <TestCase.h>
#include <TestSuite.h>

#include "formuladefs.h"

class KCommandHistory;

KFORMULA_NAMESPACE_BEGIN

class BracketElement;
class FormulaElement;
class FormulaCursor;
class IndexElement;
class KFormulaContainer;
class KFormulaDocument;
class TextElement;


class TestFormulaElement : public TestCase {
public:
    TestFormulaElement(string name) : TestCase(name) {}

    static Test* suite();

    void setUp();
    void tearDown();

private:

    void testPosition();

    KCommandHistory* history;
    KFormulaDocument* document;
    KFormulaContainer* container;
    FormulaElement* rootElement;
    FormulaCursor* cursor;

    BracketElement* element1;
    TextElement* element2;
    IndexElement* element3;
    TextElement* element4;
    TextElement* element5;
};

KFORMULA_NAMESPACE_END

#endif // TESTFORMULAELEMENT_H
