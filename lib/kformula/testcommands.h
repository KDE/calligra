
#ifndef TESTCOMMANDS_H
#define TESTCOMMANDS_H

#include <TestCaller.h>
#include <TestCase.h>
#include <TestSuite.h>

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


class TestCommands : public TestCase {
public:
    TestCommands(string name) : TestCase(name) {}

    static Test* suite();

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
