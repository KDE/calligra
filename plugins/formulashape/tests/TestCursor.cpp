// This file is part of the KDE project
// SPDX-FileCopyrightText: 2009 Jeremias Epperlein <jeeree@web.de>
// SPDX-License-Identifier: LGPL-2.0-only

#include "TestCursor.h"

#include "FormulaCommand.h"
#include "FormulaCommandUpdate.h"
#include "FormulaCursor.h"
#include "FormulaData.h"
#include "FractionElement.h"
#include "KoFormulaTool.h"
#include "RowElement.h"
#include "SubSupElement.h"
#include "TableDataElement.h"
#include "TableElement.h"
#include "TableRowElement.h"
#include "TokenElement.h"
#include <FormulaEditor.h>
#include <KoCanvasBase.h>
#include <KoDocument.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoUnit.h>
#include <QTest>

using namespace Qt::StringLiterals;

class MockCanvas : public KoCanvasBase
{
public:
    using KoCanvasBase::addCommand;

    KUndo2QStack stack;
    KoShapeManager *manager;
    MockCanvas()
        : KoCanvasBase(nullptr)
        , manager(new KoShapeManager(this))
    {
    }

    ~MockCanvas() override
    {
    }

    void gridSize(qreal *, qreal *) const override
    {
    }
    bool snapToGrid() const override
    {
        return false;
    }

    void addCommand(std::unique_ptr<KUndo2Command> &&c) override
    {
        //         c->redo();
        stack.push(std::move(c));
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
    MockCanvas canvas;
    KoFormulaShape shape(nullptr);
    canvas.shapeManager()->addShape(&shape);
    canvas.shapeManager()->selection()->select(&shape);
    QCOMPARE(canvas.shapeManager()->selection()->count(), 1);
    KoFormulaTool tool(&canvas);
    QSet<KoShape *> selectedShapes;
    selectedShapes << &shape;
    tool.activate(KoToolBase::DefaultActivation, selectedShapes);
    FormulaEditor *editor = tool.formulaEditor();
    FormulaElement *root = editor->formulaData()->formulaElement();
    canvas.addCommand(new FormulaCommandUpdate(&shape, editor->insertText("ade")));
    editor->cursor().moveTo(root->childElements()[0], 1);
    //(a|de)
    canvas.addCommand(new FormulaCommandUpdate(&shape, editor->insertText("bc")));
    editor->cursor().moveTo(root->childElements()[0], 6);
    //(abcde|)
    editor->cursor().move(MoveLeft);
    //(abcd|e)
    QCOMPARE(editor->cursor().position(), 5);
    editor->cursor().moveTo(root->childElements()[0], 0);
    editor->cursor().move(MoveLeft);
    //|(abcde)
    QCOMPARE(editor->cursor().position(), 0);
    canvas.addCommand(new FormulaCommandUpdate(&shape, editor->insertText("123")));
    QCOMPARE(root->childElements().count(), 2);
    //(12)(abcde)
    canvas.stack.undo();
    //(abcde)
    canvas.stack.redo();
    //(12)(abcde)
    QCOMPARE(root->childElements().count(), 2);
    canvas.stack.clear();
}

void TestCursor::nestedRowBoundaries()
{
    MockCanvas canvas;
    KoFormulaShape shape(nullptr);
    canvas.shapeManager()->addShape(&shape);
    canvas.shapeManager()->selection()->select(&shape);
    KoFormulaTool tool(&canvas);
    tool.activate(KoToolBase::DefaultActivation, QSet<KoShape *>{&shape});
    auto *editor = tool.formulaEditor();
    canvas.addCommand(new FormulaCommandUpdate(&shape, editor->insertMathML(u"<mrow><mi>a</mi><mrow><mi>b</mi><mi>d</mi></mrow><mi>c</mi></mrow>"_s)));

    auto *outer = static_cast<RowElement *>(editor->formulaData()->formulaElement()->childElements().first());
    auto *inner = static_cast<RowElement *>(outer->childElements().at(1));
    auto *innerToken = inner->childElements().first();
    editor->cursor().moveTo(innerToken, innerToken->endPosition());
    editor->cursor().move(MoveRight);
    QCOMPARE(editor->cursor().currentElement(), inner);
    bool reachedOuter = false;
    for (int i = 0; i < 8; ++i) {
        editor->cursor().move(MoveRight);
        if (editor->cursor().currentElement() == outer) {
            reachedOuter = true;
            break;
        }
    }
    QVERIFY(reachedOuter);
}

void TestCursor::scriptBoundaries()
{
    MockCanvas canvas;
    KoFormulaShape shape(nullptr);
    canvas.shapeManager()->addShape(&shape);
    canvas.shapeManager()->selection()->select(&shape);
    KoFormulaTool tool(&canvas);
    tool.activate(KoToolBase::DefaultActivation, QSet<KoShape *>{&shape});
    auto *editor = tool.formulaEditor();
    canvas.addCommand(new FormulaCommandUpdate(&shape, editor->insertMathML(u"<msubsup><mi>x</mi><mi>i</mi><mi>j</mi></msubsup>"_s)));

    auto *script = static_cast<SubSupElement *>(editor->formulaData()->formulaElement()->childElements().first());
    auto *base = script->childElements().first();
    editor->cursor().moveTo(base->childElements().first(), 1);
    editor->cursor().move(MoveRight);
    QVERIFY(editor->cursor().currentElement()->parentElement() == script);
}

void TestCursor::tableBoundaries()
{
    MockCanvas canvas;
    KoFormulaShape shape(nullptr);
    canvas.shapeManager()->addShape(&shape);
    canvas.shapeManager()->selection()->select(&shape);
    KoFormulaTool tool(&canvas);
    tool.activate(KoToolBase::DefaultActivation, QSet<KoShape *>{&shape});
    auto *editor = tool.formulaEditor();
    canvas.addCommand(new FormulaCommandUpdate(&shape, editor->insertMathML(u"<mtable><mtr><mtd><mi>a</mi></mtd><mtd><mi>b</mi></mtd></mtr></mtable>"_s)));

    auto *table = static_cast<TableElement *>(editor->formulaData()->formulaElement()->childElements().first());
    auto *row = static_cast<TableRowElement *>(table->childElements().first());
    auto *firstCell = static_cast<TableDataElement *>(row->childElements().first());
    auto *firstToken = firstCell->childElements().first();
    editor->cursor().moveTo(firstToken, firstToken->endPosition());
    editor->cursor().move(MoveRight);
    editor->cursor().move(MoveRight);
    QCOMPARE(editor->cursor().currentElement(), row->childElements().at(1));
    QCOMPARE(editor->cursor().position(), 0);
}

void TestCursor::emptyElementBoundaries()
{
    MockCanvas canvas;
    KoFormulaShape shape(nullptr);
    canvas.shapeManager()->addShape(&shape);
    canvas.shapeManager()->selection()->select(&shape);
    KoFormulaTool tool(&canvas);
    tool.activate(KoToolBase::DefaultActivation, QSet<KoShape *>{&shape});
    auto *editor = tool.formulaEditor();
    canvas.addCommand(new FormulaCommandUpdate(&shape, editor->insertMathML(u"<mfrac></mfrac>"_s)));

    auto *fraction = static_cast<FractionElement *>(editor->formulaData()->formulaElement()->childElements().first());
    auto *numerator = fraction->childElements().first();
    auto *denominator = fraction->childElements().last();
    editor->cursor().moveTo(numerator, 0);
    for (int i = 0; i < 12 && editor->cursor().currentElement() != denominator; ++i) {
        editor->cursor().move(MoveRight);
    }
    QVERIFY(editor->cursor().currentElement() != numerator);
    QVERIFY(editor->cursor().isAccepted());
}

void TestCursor::structuralCommands()
{
    MockCanvas canvas;
    KoFormulaShape shape(nullptr);
    canvas.shapeManager()->addShape(&shape);
    canvas.shapeManager()->selection()->select(&shape);
    KoFormulaTool tool(&canvas);
    tool.activate(KoToolBase::DefaultActivation, QSet<KoShape *>{&shape});
    auto *editor = tool.formulaEditor();
    auto *root = editor->formulaData()->formulaElement();

    canvas.addCommand(new FormulaCommandUpdate(&shape, editor->insertMathML(u"<mrow><mi>a</mi><mi>b</mi></mrow>"_s)));
    auto *row = static_cast<RowElement *>(root->childElements().first());
    QCOMPARE(row->childElements().count(), 2);

    FormulaCursor selection(row, true, 0, row->endPosition());
    editor->setCursor(selection);
    canvas.addCommand(new FormulaCommandUpdate(&shape, editor->remove(false)));
    QCOMPARE(row->childElements().count(), 0);
    canvas.stack.undo();
    QCOMPARE(row->childElements().count(), 2);
    canvas.stack.redo();
    QCOMPARE(row->childElements().count(), 0);
    canvas.stack.clear();

    KoFormulaShape tableShape(nullptr);
    canvas.shapeManager()->addShape(&tableShape);
    canvas.shapeManager()->selection()->select(&tableShape);
    KoFormulaTool tableTool(&canvas);
    tableTool.activate(KoToolBase::DefaultActivation, QSet<KoShape *>{&tableShape});
    auto *tableEditor = tableTool.formulaEditor();
    canvas.addCommand(new FormulaCommandUpdate(&tableShape, tableEditor->insertMathML(u"<mtable><mtr><mtd><mi>x</mi></mtd></mtr></mtable>"_s)));
    auto *table = static_cast<TableElement *>(tableEditor->formulaData()->formulaElement()->childElements().first());

    canvas.addCommand(new FormulaCommandUpdate(&tableShape, new FormulaCommandReplaceRow(tableEditor->formulaData(), tableEditor->cursor(), table, 0, 0, 1)));
    QCOMPARE(table->childElements().count(), 2);
    canvas.stack.undo();
    QCOMPARE(table->childElements().count(), 1);
    canvas.stack.redo();
    QCOMPARE(table->childElements().count(), 2);

    canvas.addCommand(
        new FormulaCommandUpdate(&tableShape, new FormulaCommandReplaceColumn(tableEditor->formulaData(), tableEditor->cursor(), table, 0, 0, 1)));
    QCOMPARE(table->childElements().first()->childElements().count(), 2);
    canvas.stack.undo();
    QCOMPARE(table->childElements().first()->childElements().count(), 1);
    canvas.stack.redo();
    QCOMPARE(table->childElements().first()->childElements().count(), 2);
    canvas.stack.clear();
}

void TestCursor::editCommands()
{
    MockCanvas canvas;
    KoFormulaShape shape(nullptr);
    canvas.shapeManager()->addShape(&shape);
    canvas.shapeManager()->selection()->select(&shape);
    KoFormulaTool tool(&canvas);
    QSet<KoShape *> selectedShapes{&shape};
    tool.activate(KoToolBase::DefaultActivation, selectedShapes);

    FormulaEditor *editor = tool.formulaEditor();
    canvas.addCommand(new FormulaCommandUpdate(&shape, editor->insertText(u"ab"_s)));
    auto *root = editor->formulaData()->formulaElement();
    auto *token = static_cast<TokenElement *>(root->childElements().first());
    QCOMPARE(token->text(), u"ab"_s);

    editor->cursor().moveTo(token, 1);
    canvas.addCommand(new FormulaCommandUpdate(&shape, editor->remove(true)));
    QCOMPARE(token->text(), u"b"_s);
    canvas.stack.undo();
    QCOMPARE(token->text(), u"ab"_s);
    canvas.stack.redo();
    QCOMPARE(token->text(), u"b"_s);

    FormulaCursor selection(token, true, 0, 1);
    editor->setCursor(selection);
    canvas.addCommand(new FormulaCommandUpdate(&shape, editor->insertText(u"x"_s)));
    QCOMPARE(token->text(), u"x"_s);
    canvas.stack.undo();
    QCOMPARE(token->text(), u"b"_s);
    canvas.stack.redo();
    QCOMPARE(token->text(), u"x"_s);

    editor->cursor().moveTo(token, token->endPosition());
    auto *fraction = editor->insertMathML(u"<mfrac><mi>a</mi><mi>b</mi></mfrac>"_s);
    QVERIFY(fraction);
    canvas.addCommand(new FormulaCommandUpdate(&shape, fraction));
    QCOMPARE(root->childElements().count(), 2);
    canvas.stack.undo();
    QCOMPARE(root->childElements().count(), 1);
    canvas.stack.redo();
    QCOMPARE(root->childElements().count(), 2);
    canvas.stack.clear();
}

QTEST_MAIN(TestCursor)
