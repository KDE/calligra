
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
#include "testformulacursor.h"


void TestFormulaCursor::setUp()
{
    history = new KCommandHistory;
    document = new KFormulaDocument(history);
    container = new KFormulaContainer(document);
    rootElement = container->rootElement;
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


void TestFormulaCursor::tearDown()
{
    //container->destroyCursor(cursor);
    delete cursor;
    
    delete container;
    delete document;
    delete history;
}


Test* TestFormulaCursor::suite ()
{
    TestSuite *suite = new TestSuite("TestFormulaCursor");

    suite->addTest(new TestCaller<TestFormulaCursor>("testTextInsertion",
                                                     &TestFormulaCursor::testTextInsertion));
    suite->addTest(new TestCaller<TestFormulaCursor>("testRemoval",
                                                     &TestFormulaCursor::testRemoval));
    suite->addTest(new TestCaller<TestFormulaCursor>("testRemoveIndexElementByMainChild",
                                                     &TestFormulaCursor::testRemoveIndexElementByMainChild));
    suite->addTest(new TestCaller<TestFormulaCursor>("testRemoveBracketElementByMainChild",
                                                     &TestFormulaCursor::testRemoveBracketElementByMainChild));
    suite->addTest(new TestCaller<TestFormulaCursor>("testRemoveBracketElementByNonEmptyMainChild",
                                                     &TestFormulaCursor::testRemoveBracketElementByNonEmptyMainChild));
    suite->addTest(new TestCaller<TestFormulaCursor>("testActiveIndexElement",
                                                     &TestFormulaCursor::testActiveIndexElement));
    suite->addTest(new TestCaller<TestFormulaCursor>("testReplaceByEmptyContent",
                                                     &TestFormulaCursor::testReplaceByEmptyContent));
    suite->addTest(new TestCaller<TestFormulaCursor>("testReplaceByContent",
                                                     &TestFormulaCursor::testReplaceByContent));
    suite->addTest(new TestCaller<TestFormulaCursor>("testReplaceSelectionByElement",
                                                     &TestFormulaCursor::testReplaceSelectionByElement));
    suite->addTest(new TestCaller<TestFormulaCursor>("testCursorSaving",
                                                     &TestFormulaCursor::testCursorSaving));
    
    return suite;
}


/**
 * Simply insert some text and look if its there.
 */
void TestFormulaCursor::testTextInsertion()
{
    cursor->insert(new TextElement('T'));
    cursor->insert(new TextElement('e'));
    cursor->insert(new TextElement('s'));
    cursor->insert(new TextElement('t'));

    assert(rootElement->countChildren() == 9);
    assert(cursor->getPos() == 9);
    assert(cursor->getMark() == 8);
    assert(cursor->isSelectionMode());
    assert(cursor->isSelection());
}


/**
 * Tests the removal of single elements.
 */
void TestFormulaCursor::testRemoval()
{
    cursor->setSelection(false);
    QList<BasicElement> list;
    list.setAutoDelete(true);

    // remove to the right: nothing there.
    cursor->remove(list, BasicElement::afterCursor);
    assert(rootElement->countChildren() == 5);

    // remove to the left: one element removed.
    cursor->remove(list, BasicElement::beforeCursor);
    assert(rootElement->countChildren() == 4);
    list.clear();

    cursor->moveHome();
    cursor->remove(list, BasicElement::beforeCursor);
    assert(rootElement->countChildren() == 4);
    
    cursor->remove(list, BasicElement::afterCursor);
    assert(rootElement->countChildren() == 3);
}


/**
 * Trys to remove the IndexElement after the cursor sits inside
 * its empty main child.
 */
void TestFormulaCursor::testRemoveIndexElementByMainChild()
{
    cursor->goInsideElement(element3);

    QList<BasicElement> list;
    list.setAutoDelete(true);

    cursor->remove(list);

    assert((list.count() == 1) && (list.at(0) == element3));
    assert(rootElement->countChildren() == 4);
}


/**
 * Trys to remove the IndexElement after the cursor sits inside
 * its empty main child.
 */
void TestFormulaCursor::testRemoveBracketElementByMainChild()
{
    cursor->goInsideElement(element1);

    QList<BasicElement> list;
    list.setAutoDelete(true);

    cursor->remove(list);

    assert((list.count() == 1) && (list.at(0) == element1));
    assert(rootElement->countChildren() == 4);
}


/**
 * Trys to remove the IndexElement after the cursor sits inside
 * its empty main child.
 */
void TestFormulaCursor::testRemoveBracketElementByNonEmptyMainChild()
{
    cursor->goInsideElement(element1);

    cursor->insert(new TextElement('T'));
    cursor->insert(new TextElement('e'));
    cursor->insert(new TextElement('s'));
    cursor->insert(new TextElement('t'));

    BasicElement* e = cursor->removeEnclosingElement();

    assert(e == element1);
    assert(rootElement->countChildren() == 8);
    delete e;
}


/**
 * Trys to find the IndexElement.
 */
void TestFormulaCursor::testActiveIndexElement()
{
    cursor->moveLeft();
    cursor->moveLeft();

    // behind the IndexElement
    IndexElement* index = cursor->getActiveIndexElement();
    assert(index == element3);

    // the IndexElement selected
    rootElement->selectChild(cursor, element3);
    index = cursor->getActiveIndexElement();
    assert(index == element3);

    // inside the IndexElement's main child. (empty main child)
    element3->goInside(cursor);
    index = cursor->getActiveIndexElement();
    assert(index == element3);

    // inside the IndexElement's main child. (last position)
    element3->goInside(cursor);
    cursor->insert(new TextElement('T'));
    cursor->insert(new TextElement('e'));
    cursor->insert(new TextElement('s'));
    cursor->insert(new TextElement('t'));
    cursor->setSelection(false);
    index = cursor->getActiveIndexElement();
    assert(index == element3);
}


/**
 * Trys to replace the IndexElement with its empty main child's content.
 */
void TestFormulaCursor::testReplaceByEmptyContent()
{
    element3->setToLowerRight(cursor);
    
    BasicElement* element = cursor->replaceByMainChildContent();
    delete element;
    assert(rootElement->countChildren() == 4);
}


/**
 * Trys to replace the IndexElement with its main child's content.
 */
void TestFormulaCursor::testReplaceByContent()
{
    element3->goInside(cursor);
    cursor->insert(new TextElement('T'));
    cursor->insert(new TextElement('e'));
    cursor->insert(new TextElement('s'));
    cursor->insert(new TextElement('t'));

    element3->setToLowerRight(cursor);

    BasicElement* element = cursor->replaceByMainChildContent();
    delete element;
    assert(rootElement->countChildren() == 8);
}


/**
 * Trys to replace the current selection with the new element.
 */
void TestFormulaCursor::testReplaceSelectionByElement()
{
    cursor->setSelection(false);

    cursor->moveHome(SelectMovement);

    IndexElement* index = new IndexElement();
    cursor->replaceSelectionWith(index);

    assert(rootElement->countChildren() == 1);
    assert(index->getMainChild()->countChildren() == 5);
}


/**
 * Tests the behaviour of a second cursor if the first one
 * removes an element.
 */
void TestFormulaCursor::testCursorSaving()
{
    // Cursors are no longer syncronized. This needs a view.
//     FormulaCursor* cursor2 = container->createCursor();

//     // fake an event
//     cursor2->moveEnd();
//     cursor2->moveLeft();

//     QList<BasicElement> list;
//     list.setAutoDelete(true);
//     cursor2->remove(list);

//     assert(cursor2->getPos() == 3);
//     assert(cursor->getPos() == 0);
    
//     container->destroyCursor(cursor2);
}
