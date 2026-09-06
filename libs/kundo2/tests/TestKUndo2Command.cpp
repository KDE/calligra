/*
 * SPDX-FileCopyrightText: 2026 Calligra contributors
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "../kundo2commandutils.h"
#include "../kundo2undostore.h"

#include <QTest>

class CountingCommand final : public KUndo2Command
{
public:
    explicit CountingCommand(int *redoCount, int *undoCount, KUndo2Command *parent = nullptr)
        : KUndo2Command(parent)
        , m_redoCount(redoCount)
        , m_undoCount(undoCount)
    {
    }

    void redo() override
    {
        ++*m_redoCount;
    }
    void undo() override
    {
        ++*m_undoCount;
    }

private:
    int *m_redoCount;
    int *m_undoCount;
};

class AnnihilatingCommand final : public KUndo2Command
{
public:
    explicit AnnihilatingCommand(int id)
        : m_id(id)
    {
    }

    int id() const override
    {
        return 1;
    }
    bool canAnnihilateWith(const KUndo2Command *other) const override
    {
        return static_cast<const AnnihilatingCommand *>(other)->m_id == -m_id;
    }

private:
    int m_id;
};

class TestKUndo2Command final : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void timestampInjection();
    void annihilation();
    void lambdaCommand();
    void nullStoreDeletesCommands();
    void cumulativeUndoData();
    void compositeCommand();
};

void TestKUndo2Command::timestampInjection()
{
    KUndo2Command command;
    const QTime time(1, 2, 3, 4);
    command.setTime(time);
    command.setEndTime(time);
    QCOMPARE(command.time(), time);
    QCOMPARE(command.endTime(), time);
}

void TestKUndo2Command::annihilation()
{
    KUndo2QStack stack;
    stack.push(new AnnihilatingCommand(1));
    stack.push(new AnnihilatingCommand(-1));
    QCOMPARE(stack.count(), 0);
    QCOMPARE(stack.index(), 0);
}

void TestKUndo2Command::lambdaCommand()
{
    int redoCount = 0;
    int undoCount = 0;
    KUndo2LambdaCommand command([&] {
        return std::make_unique<CountingCommand>(&redoCount, &undoCount);
    });
    command.redo();
    command.undo();
    command.redo();
    QCOMPARE(redoCount, 2);
    QCOMPARE(undoCount, 1);
}

void TestKUndo2Command::nullStoreDeletesCommands()
{
    KUndo2NullUndoStore store;
    store.addCommand(new KUndo2Command);
    QVERIFY(store.presentCommand() == nullptr);
}

void TestKUndo2Command::cumulativeUndoData()
{
    KUndo2QStack stack;
    KUndo2CumulativeUndoData expected;
    expected.excludeFromMerge = 3;
    expected.mergeTimeout = 2400;
    expected.maxGroupSeparation = 750;
    expected.maxGroupDuration = 9200;
    stack.setCumulativeUndoData(expected);
    const auto actual = stack.cumulativeUndoData();
    QCOMPARE(actual.excludeFromMerge, expected.excludeFromMerge);
    QCOMPARE(actual.mergeTimeout, expected.mergeTimeout);
    QCOMPARE(actual.maxGroupSeparation, expected.maxGroupSeparation);
    QCOMPARE(actual.maxGroupDuration, expected.maxGroupDuration);
}

void TestKUndo2Command::compositeCommand()
{
    int redoCount = 0;
    int undoCount = 0;
    KUndo2CompositeCommand command;
    command.addCommand(std::make_unique<CountingCommand>(&redoCount, &undoCount));
    command.addCommand(std::make_unique<CountingCommand>(&redoCount, &undoCount));
    command.redo();
    command.undo();
    QCOMPARE(redoCount, 2);
    QCOMPARE(undoCount, 2);
}

QTEST_MAIN(TestKUndo2Command)
#include "TestKUndo2Command.moc"
