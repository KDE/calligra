#include "TestChangeListCommand.h"
#include "../commands/ChangeListCommand.h"

#include <KoListStyle.h>

#include <QTextDocument>
#include <QTextCursor>
#include <QTextList>

void TestChangeListCommand::addList() {
    QTextDocument doc;
    QTextCursor cursor(&doc);
    cursor.insertText("Root\nparag1\nparag2\nparag3\nparag4\n");

    QTextBlock block = doc.begin().next();
    ChangeListCommand clc(block, KoListStyle::DecimalItem);
    clc.redo();

    block = doc.begin();
    QVERIFY(block.textList() == 0);
    block = block.next();
    QVERIFY(block.textList()); // this one we just changed
    QTextList *tl = block.textList();
    block = block.next();
    QVERIFY(block.textList() == 0);

    QTextListFormat format = tl->format();
    QCOMPARE(format.intProperty(QTextListFormat::ListStyle), (int) KoListStyle::DecimalItem);

    ChangeListCommand clc2(block, KoListStyle::DiscItem);
    clc2.redo();

    block = doc.begin();
    QVERIFY(block.textList() == 0);
    block = block.next();
    QCOMPARE(block.textList(), tl);
    block = block.next();
    QVERIFY(block.textList());
    QVERIFY(block.textList() != tl);
}

void TestChangeListCommand::removeList() {
    QTextDocument doc;
    QTextCursor cursor(&doc);
    cursor.insertText("Root\nparag1\nparag2\nparag3\nparag4\n");
    KoListStyle style;
    QTextBlock block = doc.begin().next();
    while(block.isValid()) {
        style.applyStyle(block);
        block = block.next();
    }

    block = doc.begin().next();
    QVERIFY(block.textList()); // init, we should not have to test KoListStyle here ;)

    ChangeListCommand clc(block, KoListStyle::NoItem);
    clc.redo();

    block = doc.begin();
    QVERIFY(block.textList() == 0);
    block = block.next();
    QVERIFY(block.textList() == 0);
    block = block.next();
    QVERIFY(block.textList());
    block = block.next();
    QVERIFY(block.textList());
    block = block.next();
    QVERIFY(block.textList());

    ChangeListCommand clc2(block, KoListStyle::NoItem);
    clc2.redo();
    block = doc.begin();
    QVERIFY(block.textList() == 0);
    block = block.next();
    QVERIFY(block.textList() == 0);
    block = block.next();
    QVERIFY(block.textList());
    block = block.next();
    QVERIFY(block.textList());
    block = block.next();
    QVERIFY(block.textList() == 0);

    // also test usecase of removing one in the middle.
}

void TestChangeListCommand::joinList() {
    QTextDocument doc;
    QTextCursor cursor(&doc);
    cursor.insertText("Root\nparag1\nparag2\nparag3\nparag4\n");
    KoListStyle style;
    QTextBlock block = doc.begin().next();
    style.applyStyle(block);
    block = block.next();
    block = block.next(); // skip one.
    style.applyStyle(block);
    block = block.next();
    style.applyStyle(block);

    block = doc.begin().next();
    QTextList *tl = block.textList();
    QVERIFY(tl); // init, we should not have to test KoListStyle here ;)
    block = block.next().next();
    QVERIFY(block.textList() == 0);

    ChangeListCommand clc(block, KoListStyle::DiscItem);
    clc.redo();
    QCOMPARE(block.textList(), tl);


    // also test usecases of joining with the one before and the one after based on similar styles.
}

QTEST_MAIN(TestChangeListCommand)

#include <TestChangeListCommand.moc>
