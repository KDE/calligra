
#ifndef TESTINDEXELEMENT_H
#define TESTINDEXELEMENT_H

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


class TestIndexElement : public TestCase {
public:
    TestIndexElement(string name) : TestCase(name) {}

    static Test* suite();

    void setUp();
    void tearDown();

private:

    void testIndexes();
    void testIndexesGeneric();
    void testMainChild();
    void testMovement();

    void removeAddIndex(ElementIndexPtr& index);

    KoCommandHistory* history;
    Document* document;
    Container* container;
    FormulaElement* rootElement;
    FormulaCursor* cursor;
    IndexElement* indexElement;

    SequenceElement* upperLeft;
    SequenceElement* upperRight;
    SequenceElement* lowerLeft;
    SequenceElement* lowerRight;
};

KFORMULA_NAMESPACE_END

#endif // TESTINDEXELEMENT_H
