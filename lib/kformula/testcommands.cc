
#include <iostream>

#include <qdom.h>
#include <qptrlist.h>
#include <qstring.h>

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
#include "testcommands.h"

KFORMULA_NAMESPACE_BEGIN

void TestCommands::setUp()
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


CppUnit::Test* TestCommands::suite()
{
    CppUnit::TestSuite *suite = new CppUnit::TestSuite("TestCommands");

    suite->addTest(new CppUnit::TestCaller<TestCommands>("testRemove",
                                                &TestCommands::testRemove));
    suite->addTest(new CppUnit::TestCaller<TestCommands>("testReplace",
                                                &TestCommands::testReplace));
    suite->addTest(new CppUnit::TestCaller<TestCommands>("testAddIndexElement",
                                                &TestCommands::testAddIndexElement));
    suite->addTest(new CppUnit::TestCaller<TestCommands>("testAddMatrix",
                                                &TestCommands::testAddMatrix));
    suite->addTest(new CppUnit::TestCaller<TestCommands>("testPhantom",
                                                &TestCommands::testPhantom));
    suite->addTest(new CppUnit::TestCaller<TestCommands>("testFractionBug",
                                                &TestCommands::testFractionBug));
    suite->addTest(new CppUnit::TestCaller<TestCommands>("testCompacting",
                                                &TestCommands::testCompacting));
    suite->addTest(new CppUnit::TestCaller<TestCommands>("testAddOneByTwoMatrix",
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

    TextRequest request( "ABC" );
    //container->addText('A');
    //container->addText('B');
    //container->addText('C');
    container->performRequest( &request );

    cursor->moveLeft(SelectMovement);
    cursor->moveLeft(SelectMovement);
    cursor->moveLeft(SelectMovement);

    //container->remove(beforeCursor);
    //container->remove(beforeCursor);
    DirectedRemove remove( req_remove, beforeCursor );
    container->performRequest( &remove );
    container->performRequest( &remove );

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
    //container->addText('A');
    TextRequest request( "A" );
    container->performRequest( &request );

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
    //container->addUpperRightIndex();
    IndexRequest request( upperRightPos );
    container->performRequest( &request );
    assert(rootElement->countChildren() == 5);
    assert(element4->getParent() != rootElement);
}

void TestCommands::testAddMatrix()
{
    cursor->moveLeft(SelectMovement);
    cursor->moveLeft(SelectMovement);
    cursor->moveLeft(SelectMovement);
    container->setActiveCursor(cursor);
    //container->addMatrix(5, 5);
    MatrixRequest request( 5, 5 );
    container->performRequest( &request );
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
//     container->remove(afterCursor);
//     container->remove(afterCursor);
//     assert(rootElement->countChildren() == 3);

//     document->undo();
//     document->undo();

//     cursor->moveRight();
//     cursor->moveRight();
//     assert(cursor->getPos() == 6);
//     container->remove(beforeCursor);
//     container->remove(beforeCursor);
//     assert(rootElement->countChildren() == 3);
}

void TestCommands::testFractionBug()
{
    container->setActiveCursor(cursor);
    cursor->moveHome();
    cursor->moveEnd(SelectMovement);
    //container->remove();
    DirectedRemove remove( req_remove, beforeCursor );
    container->performRequest( &remove );

    //container->addFraction();
    Request request( req_addFraction );
    container->performRequest( &request );
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
    TextRequest request1( "*" );
    container->performRequest( &request1 );
    Request request2( req_addNameSequence );
    container->performRequest( &request2 );
    TextRequest request3( "alpha" );
    container->performRequest( &request3 );
    //container->compactExpression();
    Request ce( req_compactExpression );
    container->performRequest( &ce );
    assert(rootElement->countChildren() == 7);
    TextRequest request4( "*" );
    container->performRequest( &request4 );
    //container->makeGreek();
    Request mg( req_makeGreek );
    container->performRequest( &mg );
    assert(rootElement->countChildren() == 8);
}


void TestCommands::testAddOneByTwoMatrix()
{
    container->setActiveCursor(cursor);
    //container->addOneByTwoMatrix();
    Request request( req_addOneByTwoMatrix );
    container->performRequest( &request );
    assert(rootElement->countChildren() == 6);
    document->undo();
    assert(rootElement->countChildren() == 5);
    document->redo();
    assert(rootElement->countChildren() == 6);

    cursor->moveHome(WordMovement);
    cursor->moveEnd(SelectMovement);
    //container->addOneByTwoMatrix();
    container->performRequest( &request );
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

KFORMULA_NAMESPACE_END
