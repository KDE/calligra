/* This file is part of the KDE project
   Copyright 2009 Jeremias Epperlein <jeeree@web.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "TestCursor.h"

#include <qtest_kde.h>
#include "FormulaCursor.h"
#include "FormulaData.h"
#include <KoDocument.h>
#include "FormulaCommand.h"
#include "FormulaCommandUpdate.h"
#include "KoFormulaTool.h"
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoCanvasBase.h>
#include <kdebug.h>

class MockCanvas : public KoCanvasBase
{
public:
    QUndoStack *stack;
    KoShapeManager *manager;
    MockCanvas(): KoCanvasBase(0) {
        stack=new QUndoStack();
        manager=new KoShapeManager(this);
    }
    ~MockCanvas() {
        delete stack;
    }

    void gridSize(qreal *, qreal *) const {}
    bool snapToGrid() const  {
        return false;
    }
    
    void addCommand(QUndoCommand* c) {
//         c->redo();
        stack->push(c);
    }
    KoShapeManager *shapeManager() const  {
        return manager;
    }
    void updateCanvas(const QRectF&)  {}
    KoToolProxy * toolProxy() const {
        return 0;
    }
    KoViewConverter *viewConverter() const {
        return 0;
    }
    QWidget* canvasWidget() {
        return 0;
    }
    const QWidget* canvasWidget() const {
        return 0;
    }
    KoUnit unit() const {
        return KoUnit(KoUnit::Millimeter);
    }
    void updateInputMethodInfo() {}
};

void TestCursor::moveCursor()
{
    MockCanvas* canvas=new MockCanvas();
    KoFormulaShape* shape = new KoFormulaShape();
    canvas->shapeManager()->add(shape);
    canvas->shapeManager()->selection()->select(shape);
    QCOMPARE(canvas->shapeManager()->selection()->count(),1);
    KoFormulaTool* tool= new KoFormulaTool(canvas);
    tool->activate();
/*
    FormulaCursor* cursor=tool->formulaCursor();
    FormulaElement* root=cursor->formulaData()->formulaElement();
    canvas->addCommand(new FormulaCommandUpdate(shape,cursor->insertText("ade")));
    cursor->moveTo(root->childElements()[0],1);
    //(a|de)
    canvas->addCommand(new FormulaCommandUpdate(shape,cursor->insertText("bc")));
    cursor->moveTo(root->childElements()[0],6);
    //(abcde|)
    cursor->move(MoveLeft);
    //(abcd|e)
    QCOMPARE(cursor->position(),5);
    cursor->moveTo(root->childElements()[0],0);
    cursor->move(MoveLeft);
    //|(abcde)
    QCOMPARE(cursor->position(),0);
    canvas->addCommand(new FormulaCommandUpdate(shape,cursor->insertText("123")));
    QCOMPARE(root->childElements().count(),2);
    //(12)(abcde)
    canvas->stack->undo();
    //(abcde)
    canvas->stack->redo();
    //(12)(abcde)
    QCOMPARE(root->childElements().count(),2);
    canvas->stack->clear();
*/
}


QTEST_KDEMAIN(TestCursor,GUI)
#include "TestCursor.moc"
