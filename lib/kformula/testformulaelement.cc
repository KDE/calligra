
#include <iostream>

#include <qlist.h>

#include <kapplication.h>
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
    history = new KoCommandHistory;
    document = new Document( kapp->config(), history );
    container = new Container(document);
    rootElement = container->rootElement();
    cursor = container->createCursor();

    element1 = new BracketElement(LeftSquareBracket, RightSquareBracket);
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


CppUnit::Test* TestFormulaElement::suite ()
{
    CppUnit::TestSuite *suite = new CppUnit::TestSuite("TestFormulaElement");

    suite->addTest(new CppUnit::TestCaller<TestFormulaElement>("testPosition",
                                                               &TestFormulaElement::testPosition));
    return suite;
}

void TestFormulaElement::testPosition()
{
    assert(rootElement->getX() == 0);
    assert(rootElement->getY() == 0);
}

KFORMULA_NAMESPACE_END
