
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
#include "sequenceelement.h"
#include "textelement.h"

#include "testindexelement.h"


Test* TestIndexElement::suite()
{
    TestSuite *suite = new TestSuite("TestIndexElement");

    suite->addTest(new TestCaller<TestIndexElement>("testIndexes",
						    &TestIndexElement::testIndexes));
    suite->addTest(new TestCaller<TestIndexElement>("testIndexesGeneric",
						    &TestIndexElement::testIndexesGeneric));
    suite->addTest(new TestCaller<TestIndexElement>("testMainChild",
						    &TestIndexElement::testMainChild));
    suite->addTest(new TestCaller<TestIndexElement>("testMovement",
						    &TestIndexElement::testMovement));
    return suite;
}

void TestIndexElement::setUp()
{
    history = new KCommandHistory;
    document = new KFormulaDocument(history);
    container = new KFormulaContainer(document);
    rootElement = container->rootElement();
    cursor = container->createCursor();

    indexElement = new IndexElement();
    cursor->insert(indexElement);

	// create indexes
    upperLeft = new SequenceElement();
    upperRight = new SequenceElement();
    lowerLeft = new SequenceElement();
    lowerRight = new SequenceElement();
    indexElement->setToUpperLeft(cursor);
    cursor->insert(upperLeft);
    indexElement->setToUpperRight(cursor);
    cursor->insert(upperRight);
    indexElement->setToLowerLeft(cursor);
    cursor->insert(lowerLeft);
    indexElement->setToLowerRight(cursor);
    cursor->insert(lowerRight);
}

void TestIndexElement::tearDown()
{
    //container->destroyCursor(cursor);
    delete cursor;

    delete container;
    delete document;
    delete history;
}


void TestIndexElement::testIndexes()
{
    assert(indexElement->hasUpperLeft());
    assert(indexElement->hasLowerLeft());
    assert(indexElement->hasUpperRight());
    assert(indexElement->hasLowerRight());

    QList<BasicElement> list;
    list.setAutoDelete(true);

    indexElement->moveToUpperLeft(cursor, BasicElement::beforeCursor);
    cursor->remove(list);
    assert(!indexElement->hasUpperLeft());

    cursor->normalize();
    assert(cursor->getElement() == indexElement->getMainChild());

    list.clear();
    indexElement->moveToUpperRight(cursor, BasicElement::beforeCursor);
    cursor->remove(list);
    assert(!indexElement->hasUpperRight());

    cursor->normalize();
    assert(cursor->getElement() == indexElement->getMainChild());

    list.clear();
    indexElement->moveToLowerLeft(cursor, BasicElement::beforeCursor);
    cursor->remove(list);
    assert(!indexElement->hasLowerLeft());

    cursor->normalize();
    assert(cursor->getElement() == indexElement->getMainChild());

    list.clear();
    indexElement->moveToLowerRight(cursor, BasicElement::beforeCursor);
    cursor->remove(list);
    assert(!indexElement->hasLowerRight());

    cursor->normalize();
    assert(cursor->getElement() == indexElement->getMainChild());
}

void TestIndexElement::testIndexesGeneric()
{
    ElementIndexPtr upperLeft = indexElement->getUpperLeft();
    removeAddIndex(upperLeft);

    ElementIndexPtr lowerLeft = indexElement->getLowerLeft();
    removeAddIndex(lowerLeft);

    ElementIndexPtr upperRight = indexElement->getUpperRight();
    removeAddIndex(upperRight);

    ElementIndexPtr lowerRight = indexElement->getLowerRight();
    removeAddIndex(lowerRight);
}

void TestIndexElement::removeAddIndex(ElementIndexPtr& index)
{
    assert(index->hasIndex());

    QList<BasicElement> list;
    list.setAutoDelete(true);

    index->moveToIndex(cursor, BasicElement::beforeCursor);
    cursor->remove(list);
    assert(!index->hasIndex());

    index->setToIndex(cursor);
    cursor->insert(new SequenceElement());
    assert(index->hasIndex());
}

void TestIndexElement::testMainChild()
{
//     SequenceElement* oldChild = indexElement->getMainChild();
//     SequenceElement* child = new SequenceElement();
//     indexElement->setMainChild(child);

//     delete oldChild;

//     assert(child->getParent() == indexElement);
//     assert(indexElement->getMainChild() == child);
}

void TestIndexElement::testMovement()
{
    cursor->setLinearMovement(false);
    indexElement->moveLeft(cursor, rootElement);
    assert(cursor->getElement() == indexElement->getMainChild());
    indexElement->moveRight(cursor, rootElement);
    assert(cursor->getElement() == indexElement->getMainChild());
}
