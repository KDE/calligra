
#ifndef TESTFORMULAELEMENT_H
#define TESTFORMULAELEMENT_H

#include <cppunit/TestCaller.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>

#include "kformuladefs.h"

class KoCommandHistory;

KFORMULA_NAMESPACE_BEGIN

class BracketElement;
class FormulaElement;
class FormulaCursor;
class IndexElement;
class Container;
class Document;
class TextElement;


class TestFormulaElement : public CppUnit::TestCase {
public:
    //TestFormulaElement(std::string name) : CppUnit::TestCase(name) {}

    static CppUnit::Test* suite();

    void setUp();
    void tearDown();

private:

    void testPosition();

    KoCommandHistory* history;
    Document* document;
    Container* container;
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
