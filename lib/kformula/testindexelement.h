
#ifndef TESTINDEXELEMENT_H
#define TESTINDEXELEMENT_H

#include <TestCaller.h>
#include <TestCase.h>
#include <TestSuite.h>

#include "elementindex.h"
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

    KCommandHistory* history;
    KFormulaDocument* document;
    KFormulaContainer* container;
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
