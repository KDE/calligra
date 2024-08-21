// This file is part of the KDE project
// SPDX-FileCopyrightText: 2009 Jeremias Epperlein <jeeree@web.de>
// SPDX-License-Identifier: LGPL-2.0-only

#include "TestCursor.h"

#include "FormulaCommand.h"
#include "FormulaCommandUpdate.h"
#include "FormulaCursor.h"
#include "FormulaData.h"
#include "KoFormulaTool.h"
#include <FormulaEditor.h>
#include <KoCanvasBase.h>
#include <KoDocument.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoUnit.h>
#include <QTest>

class MockCanvas : public KoCanvasBase
{
public:
    KUndo2QStack *stack;
    KoShapeManager *manager;
    MockCanvas()
        : KoCanvasBase(nullptr)
    {
        stack = new KUndo2QStack();
        manager = new KoShapeManager(this);
    }
    ~MockCanvas() override
    {
        delete stack;
    }

    void gridSize(qreal *, qreal *) const override
    {
    }
    bool snapToGrid() const override
    {
        return false;
    }

    void addCommand(KUndo2Command *c) override
    {
        //         c->redo();
        stack->push(c);
    }
    KoShapeManager *shapeManager() const override
    {
        return manager;
    }
    void updateCanvas(const QRectF &) override
    {
    }
    KoToolProxy *toolProxy() const override
    {
        return nullptr;
    }
    KoViewConverter *viewConverter() const override
    {
        return nullptr;
    }
    QWidget *canvasWidget() override
    {
        return nullptr;
    }
    const QWidget *canvasWidget() const override
    {
        return nullptr;
    }
    KoUnit unit() const override
    {
        return KoUnit(KoUnit::Millimeter);
    }
    void updateInputMethodInfo() override
    {
    }
    void setCursor(const QCursor &) override
    {
    }
};

void TestCursor::moveCursor()
{
    MockCanvas *canvas = new MockCanvas();
    KoFormulaShape *shape = new KoFormulaShape(nullptr); // FIXME: Do we need a real resourceManager here?
    canvas->shapeManager()->addShape(shape);
    canvas->shapeManager()->selection()->select(shape);
    QCOMPARE(canvas->shapeManager()->selection()->count(), 1);
    KoFormulaTool *tool = new KoFormulaTool(canvas);
    QSet<KoShape *> selectedShapes;
    selectedShapes << shape;
    tool->activate(KoToolBase::DefaultActivation, selectedShapes);
    FormulaEditor *editor = tool->formulaEditor();
    FormulaElement *root = editor->formulaData()->formulaElement();
    canvas->addCommand(new FormulaCommandUpdate(shape, editor->insertText("ade")));
    editor->cursor().moveTo(root->childElements()[0], 1);
    //(a|de)
    canvas->addCommand(new FormulaCommandUpdate(shape, editor->insertText("bc")));
    editor->cursor().moveTo(root->childElements()[0], 6);
    //(abcde|)
    editor->cursor().move(MoveLeft);
    //(abcd|e)
    QCOMPARE(editor->cursor().position(), 5);
    editor->cursor().moveTo(root->childElements()[0], 0);
    editor->cursor().move(MoveLeft);
    //|(abcde)
    QCOMPARE(editor->cursor().position(), 0);
    canvas->addCommand(new FormulaCommandUpdate(shape, editor->insertText("123")));
    QCOMPARE(root->childElements().count(), 2);
    //(12)(abcde)
    canvas->stack->undo();
    //(abcde)
    canvas->stack->redo();
    //(12)(abcde)
    QCOMPARE(root->childElements().count(), 2);
    canvas->stack->clear();
}

QTEST_MAIN(TestCursor)
