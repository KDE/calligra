
#include <iostream>

#include <qdom.h>
#include <qlist.h>
#include <qstring.h>

#include <kcommand.h>

#include "basicelement.h"
#include "bracketelement.h"
#include "formulacursor.h"
#include "formulaelement.h"
#include "indexelement.h"
#include "kformulacontainer.h"
#include "kformuladocument.h"
#include "textelement.h"
#include "testcommands.h"


void TestCommands::setUp()
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
    cursor->setSelection(false);
}


void TestCommands::tearDown()
{
    //container->destroyCursor(cursor);
    delete cursor;
    
    delete container;
    delete document;
    delete history;
}


Test* TestCommands::suite()
{
    TestSuite *suite = new TestSuite("TestCommands");

    suite->addTest(new TestCaller<TestCommands>("testRemove",
                                                &TestCommands::testRemove));
    suite->addTest(new TestCaller<TestCommands>("testReplace",
                                                &TestCommands::testReplace));
    suite->addTest(new TestCaller<TestCommands>("testAddIndexElement",
                                                &TestCommands::testAddIndexElement));
    suite->addTest(new TestCaller<TestCommands>("testAddMatrix",
                                                &TestCommands::testAddMatrix));
    suite->addTest(new TestCaller<TestCommands>("testPhantom",
                                                &TestCommands::testPhantom));
    suite->addTest(new TestCaller<TestCommands>("testFractionBug",
                                                &TestCommands::testFractionBug));
    suite->addTest(new TestCaller<TestCommands>("testCompacting",
                                                &TestCommands::testCompacting));
    suite->addTest(new TestCaller<TestCommands>("testAddOneByTwoMatrix",
                                                &TestCommands::testAddOneByTwoMatrix));
    return suite;
}

void TestCommands::testRemove()
{
    ElementIndexPtr index = element3->getLowerRight();
    index->setToIndex(cursor);
    cursor->insert(new SequenceElement());

    QDomDocument beginDoc("Test");
    beginDoc.appendChild(rootElement->getElementDom(beginDoc));
    QString begin = beginDoc.toString();

    container->setActiveCursor(cursor);
    
    container->addText('A');
    container->addText('B');
    container->addText('C');

    cursor->moveLeft(SelectMovement);
    cursor->moveLeft(SelectMovement);
    cursor->moveLeft(SelectMovement);

    container->remove(BasicElement::beforeCursor);
    container->remove(BasicElement::beforeCursor);

    assert(rootElement->countChildren() == 4);

    for (int i = 0; i < 5; i++)
        document->undo();

    QDomDocument endDoc("Test");
    endDoc.appendChild(rootElement->getElementDom(endDoc));
    QString end = endDoc.toString();

    //cerr << begin.latin1() << endl;
    //cerr << end.latin1() << endl;
    assert(end == begin);
}

void TestCommands::testReplace()
{
    //cursor->moveEnd();
    cursor->moveHome(SelectMovement);
    container->setActiveCursor(cursor);
    container->addText('A');

//     QDomDocument endDoc("Test");
//     endDoc.appendChild(rootElement->getElementDom(endDoc));
//     QString end = endDoc.toString();
//     cerr << end.latin1() << endl;

    assert(rootElement->countChildren() == 1);
}

void TestCommands::testAddIndexElement()
{
    cursor->moveLeft();
    container->setActiveCursor(cursor);
    container->addUpperRightIndex();
    assert(rootElement->countChildren() == 5);
    assert(element4->getParent() != rootElement);
}

void TestCommands::testAddMatrix()
{
    cursor->moveLeft(SelectMovement);
    cursor->moveLeft(SelectMovement);
    cursor->moveLeft(SelectMovement);
    container->setActiveCursor(cursor);
    container->addMatrix(5, 5);
    assert(rootElement->countChildren() == 3);
    document->undo();
    assert(rootElement->countChildren() == 5);
    document->redo();
    assert(rootElement->countChildren() == 3);
    document->undo();
    assert(rootElement->countChildren() == 5);
}

void TestCommands::testPhantom()
{
//     cursor->moveLeft();
//     container->setActiveCursor(cursor);
//     container->addText('\\');
//     assert(rootElement->countChildren() == 6);

//     cursor->moveLeft();
//     assert(cursor->getPos() == 3);
//     container->remove(BasicElement::afterCursor);
//     container->remove(BasicElement::afterCursor);
//     assert(rootElement->countChildren() == 3);

//     document->undo();
//     document->undo();

//     cursor->moveRight();
//     cursor->moveRight();
//     assert(cursor->getPos() == 6);
//     container->remove(BasicElement::beforeCursor);
//     container->remove(BasicElement::beforeCursor);
//     assert(rootElement->countChildren() == 3);
}

void TestCommands::testFractionBug()
{
    container->setActiveCursor(cursor);
    cursor->moveHome();
    cursor->moveEnd(SelectMovement);
    container->remove();

    container->addFraction();
    cursor->moveDown();
    container->copy();
    container->paste();
    document->undo();
    document->undo();

    assert(rootElement->countChildren() == 5);
}

void TestCommands::testCompacting()
{
    container->setActiveCursor(cursor);
    container->addText('*');
    container->addText('\\');
    container->addText('a');
    container->addText('l');
    container->addText('p');
    container->addText('h');
    container->addText('a');
    container->compactExpression();
    assert(rootElement->countChildren() == 7);
    container->addText('a');
    container->makeGreek();
    assert(rootElement->countChildren() == 8);
}


void TestCommands::testAddOneByTwoMatrix()
{
    container->setActiveCursor(cursor);
    container->addOneByTwoMatrix();
    assert(rootElement->countChildren() == 6);
    document->undo();
    assert(rootElement->countChildren() == 5);
    document->redo();
    assert(rootElement->countChildren() == 6);

    cursor->moveHome(WordMovement);
    cursor->moveEnd(SelectMovement);
    container->addOneByTwoMatrix();
    assert(rootElement->countChildren() == 1);
    document->undo();
    document->redo();
    document->undo();
    document->redo();
    document->undo();
    assert(rootElement->countChildren() == 6);
    document->redo();
    assert(rootElement->countChildren() == 1);
}
