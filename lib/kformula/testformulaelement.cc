
#include <iostream>

#include <qlist.h>

#include <kcommand.h>

#include "basicelement.h"
#include "bracketelement.h"
#include "formulacursor.h"
#include "formulaelement.h"
#include "indexelement.h"
#include "kformulacontainer.h"
#include "kformuladocument.h"
#include "textelement.h"
#include "testformulaelement.h"


KFORMULA_NAMESPACE_BEGIN

void TestFormulaElement::setUp()
{
    history = new KCommandHistory;
    document = new KFormulaDocument(history);
    container = new KFormulaContainer(document);
    rootElement = container->rootElement();
    cursor = container->createCursor();

    element1 = new BracketElement('(', ']');
    element2 = new TextElement('2');
    element3 = new IndexElement();
    element4 = new TextElement('4');
    element5 = new TextElement('5');

    cursor->insert(element1);
    cursor->insert(element2);
    cursor->insert(element3);
    cursor->insert(element4);
    cursor->insert(element5);
}


void TestFormulaElement::tearDown()
{
    delete cursor;

    delete container;
    delete document;
    delete history;
}


Test* TestFormulaElement::suite ()
{
    TestSuite *suite = new TestSuite("TestFormulaElement");

    suite->addTest(new TestCaller<TestFormulaElement>("testPosition",
                                                      &TestFormulaElement::testPosition));
    return suite;
}

void TestFormulaElement::testPosition()
{
    assert(rootElement->getX() == 0);
    assert(rootElement->getY() == 0);
}

KFORMULA_NAMESPACE_END
