
#ifndef TESTCOMMANDS_H
#define TESTCOMMANDS_H

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


class TestCommands : public CppUnit::TestCase {
public:
    //TestCommands(std::string name) : CppUnit::TestCase(name) {}

    static CppUnit::Test* suite();

    void setUp();
    void tearDown();

private:

    void testRemove();
    void testReplace();
    void testAddIndexElement();
    void testAddMatrix();
    void testPhantom();
    void testFractionBug();
    void testCompacting();
    void testAddOneByTwoMatrix();

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

#endif // TESTCOMMANDS_H
