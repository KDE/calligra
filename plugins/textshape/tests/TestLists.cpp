#include "TestDocumentLayout.h"

#include <KoListLevelProperties.h>
#include <KoListStyle.h>
#include <KoParagraphStyle.h>
#include <KoStyleManager.h>
#include <KoTextBlockData.h>

#include <QtGui>

void TestDocumentLayout::testBasicList()
{
    initForNewTest("Base\nListItem\nListItem2: The quick brown fox jums over the lazy dog.\nNormal\nNormal");

    KoParagraphStyle style;
    QTextBlock block = m_doc->begin();
    style.applyStyle(block);
    block = block.next();
    QVERIFY(block.isValid());

    KoListStyle listStyle;
    KoListLevelProperties level1;
    level1.setStyle(KoListStyle::Bullet);
    listStyle.setLevelProperties(level1);
    style.setListStyle(&listStyle);
    style.applyStyle(block); // make this a listStyle
    QVERIFY(block.textList());
    QCOMPARE(block.textList()->format().intProperty(QTextListFormat::ListStyle), (int)KoListStyle::Bullet);
    block = block.next();
    QVERIFY(block.isValid());
    style.applyStyle(block); // make this a listStyle

    m_layout->layout();
    QTextLayout *blockLayout = m_block.layout();

    QCOMPARE(blockLayout->lineAt(0).x(), 0.0);
    block = m_doc->begin().next();
    QVERIFY(block.isValid());
    blockLayout = block.layout(); // parag 2
    KoTextBlockData *data = dynamic_cast<KoTextBlockData *>(block.userData());
    QVERIFY(data);
    qreal counterSpacing = data->counterSpacing();
    QVERIFY(counterSpacing > 0.);
    // 12 is hardcoded to be the width of a discitem (taken from the default font):
    QCOMPARE(blockLayout->lineAt(0).x(), 12.0 + counterSpacing);
    block = block.next();
    QVERIFY(block.isValid());
    blockLayout = block.layout(); // parag 3
    QCOMPARE(blockLayout->lineAt(0).x(), 12.0 + counterSpacing);
    QVERIFY(blockLayout->lineCount() > 1);
    QCOMPARE(blockLayout->lineAt(1).x(), 12.0 + counterSpacing); // make sure not only the first line is indented
    block = block.next();
    QVERIFY(block.isValid());
    blockLayout = block.layout(); // parag 4
    QCOMPARE(blockLayout->lineAt(0).x(), 0.0);
}

void TestDocumentLayout::testNumberedList()
{
    initForNewTest(
        "Base\nListItem1\nListItem2\nListItem3\nListItem4\nListItem5\nListItem6\nListItem6\nListItem7\nListItem8\nListItem9\nListItem10\nListItem11\nListItem12"
        "\n");

    KoParagraphStyle style;
    m_styleManager->add(&style);
    QTextBlock block = m_doc->begin();
    style.applyStyle(block);
    block = block.next();

    KoListStyle listStyle;
    KoListLevelProperties llp;
    llp.setStyle(KoListStyle::DecimalItem);
    listStyle.setLevelProperties(llp);
    style.setListStyle(&listStyle);

    QTextList *previous = 0;
    int i;
    for (i = 1; i <= 9; i++) {
        QVERIFY(block.isValid());
        // qDebug() << "->" << block.text();
        style.applyStyle(block);
        QTextList *textList = block.textList();
        QVERIFY(textList);
        if (previous == 0)
            previous = textList;
        else
            QCOMPARE(textList, previous);
        QCOMPARE(textList->format().intProperty(QTextListFormat::ListStyle), (int)(KoListStyle::DecimalItem));
        block = block.next();
    }
    m_layout->layout();
    QTextLayout *blockLayout = m_block.layout();

    QCOMPARE(blockLayout->lineAt(0).x(), 0.0);
    QTextBlock blok = m_doc->begin().next();
    qreal indent = blok.layout()->lineAt(0).x();
    QVERIFY(indent > 0.0);
    for (i = 1; i <= 9; ++i) {
        // qDebug() << "=>" << blok.text();
        QTextList *textList = blok.textList();
        QVERIFY(textList);
        QCOMPARE(blok.layout()->lineAt(0).x(), indent); // all the same indent.
        blok = blok.next();
    }

    // now make number of listitems be more than 10, so we use 2 digits.
    for (i = 9; i <= 12; ++i) {
        QVERIFY(block.isValid());
        style.applyStyle(block);
        // qDebug() << "->" << block.text();
        block = block.next();
    }
    m_layout->layout();
    blockLayout = m_block.layout();

    QCOMPARE(blockLayout->lineAt(0).x(), 0.0);
    blok = m_doc->begin().next();
    qreal indent2 = blok.layout()->lineAt(0).x();
    QVERIFY(indent2 > indent); // since it takes an extra digit
    for (i = 2; i <= 12; ++i) {
        // qDebug() << "=>" << blok.text();
        QCOMPARE(blok.layout()->lineAt(0).x(), indent2); // all the same indent.
        blok = blok.next();
    }

    // now to make sure the text is actually properly set.
    block = m_doc->begin().next();
    i = 1;
    while (block.isValid() && i < 13) {
        KoTextBlockData *data = dynamic_cast<KoTextBlockData *>(block.userData());
        QVERIFY(data);
        QCOMPARE(data->counterText(), QString::number(i++));
        block = block.next();
    }

    llp.setListItemSuffix(".");
    llp.setStartValue(4);
    listStyle.setLevelProperties(llp);

    QTextCursor cursor(m_doc);
    cursor.setPosition(10); // listItem1
    QTextBlockFormat format = cursor.blockFormat();
    format.setProperty(KoParagraphStyle::ListStartValue, 4);
    cursor.setBlockFormat(format);

    cursor.setPosition(40); // listItem4
    format = cursor.blockFormat();
    format.setProperty(KoParagraphStyle::ListStartValue, 12);
    cursor.setBlockFormat(format);

    // at this point we start numbering at 4. Have 4, 5, 6, 12, 13, 14, 15 etc
    m_layout->layout();

    // now to make sur the text is actually properly set.
    block = m_doc->begin().next();
    i = 4;
    while (block.isValid() && i < 22) {
        if (i == 7)
            i = 12;
        KoTextBlockData *data = dynamic_cast<KoTextBlockData *>(block.userData());
        QVERIFY(data);
        QCOMPARE(data->counterText(), QString::number(i++));
        block = block.next();
    }
}

void TestDocumentLayout::testInterruptedLists()
{
    initForNewTest("ListItem1\nListItem2\nNormal Parag\nAnother parag\nListItem3\n");
    // expect that normal paragraphs do not break a list (i.e not restart it)

    KoParagraphStyle style;
    KoListStyle listStyle;
    KoListLevelProperties llp = listStyle.levelProperties(1);
    llp.setStyle(KoListStyle::DecimalItem);
    llp.setStartValue(1);
    llp.setListItemSuffix(".");
    listStyle.setLevelProperties(llp);
    style.setListStyle(&listStyle);

    QTextBlock block = m_doc->begin();
    style.applyStyle(block);
    block = block.next();
    style.applyStyle(block);
    block = block.next();
    block = block.next();
    block = block.next();
    style.applyStyle(block);

    m_layout->layout();

    block = m_doc->begin();
    KoTextBlockData *data = dynamic_cast<KoTextBlockData *>(block.userData());
    QVERIFY(data);
    QVERIFY(data->counterText() == "1.");
    block = block.next();
    data = dynamic_cast<KoTextBlockData *>(block.userData());
    QVERIFY(data);
    QVERIFY(data->counterText() == "2.");
    block = block.next();
    QCOMPARE(block.layout()->lineAt(0).x(), 0.0);
    QVERIFY(block.userData() == 0);
    block = block.next();
    QCOMPARE(block.layout()->lineAt(0).x(), 0.0);
    QVERIFY(block.userData() == 0);
    block = block.next(); // list item 3
    data = dynamic_cast<KoTextBlockData *>(block.userData());
    QVERIFY(data);
    // qDebug() << data->counterText();
    QVERIFY(data->counterText() == "3.");

// I have doubts what consecutiveNumbering should do.  Disable the feature for now.
#if 0
    // now the other way around
    block = m_doc->begin();
    listStyle.setConsecutiveNumbering(false);
    listStyle.applyStyle(block);
    m_layout->layout();

    data = dynamic_cast<KoTextBlockData*>(block.userData());
    QVERIFY(data);
    QVERIFY(data->counterText() == "1.");
    block = block.next();
    data = dynamic_cast<KoTextBlockData*>(block.userData());
    QVERIFY(data);
    QVERIFY(data->counterText() == "2.");
    block = block.next();
    QCOMPARE(block.layout()->lineAt(0).x(), 0.0);
    QVERIFY(block.userData() ==  0);
    block = block.next();
    QCOMPARE(block.layout()->lineAt(0).x(), 0.0);
    QVERIFY(block.userData() ==  0);
    block = block.next(); // list item 3
    data = dynamic_cast<KoTextBlockData*>(block.userData());
    QVERIFY(data);
    qDebug() << data->counterText();
    QVERIFY(data->counterText() == "1.");
#endif
}

void TestDocumentLayout::testNestedLists()
{
    initForNewTest("Root\nplants\nherbs\ncinnamon\ncurry\nroses\nhumans\nFrank\nAnkje\nOther\nSkip\nLastItem");

    KoParagraphStyle h1;
    m_styleManager->add(&h1);
    KoParagraphStyle h2;
    m_styleManager->add(&h2);
    KoParagraphStyle h3;
    m_styleManager->add(&h3);
    KoParagraphStyle h4;
    m_styleManager->add(&h4);

    KoListStyle listStyle;
    KoListLevelProperties llp1;
    llp1.setStartValue(1);
    llp1.setStyle(KoListStyle::DecimalItem);

    listStyle.setLevelProperties(llp1);
    h1.setListStyle(&listStyle);

    KoListStyle listStyle2;
    KoListLevelProperties llp2;
    llp2.setStartValue(1);
    llp2.setStyle(KoListStyle::DecimalItem);
    llp2.setLevel(2);
    llp2.setListItemSuffix(".");
    llp2.setDisplayLevel(2);
    listStyle2.setLevelProperties(llp2);
    h2.setListStyle(&listStyle2);
    // purposfully leave this one out, as it should default to the only known one: // h2.setListLevel(2);

    KoListLevelProperties llp3;
    llp3.setStartValue(1);
    llp3.setStyle(KoListStyle::DecimalItem);
    llp3.setLevel(3);
    llp3.setListItemSuffix("");
    llp3.setDisplayLevel(3);
    KoListStyle listStyle3;
    listStyle3.setLevelProperties(llp3);
    h3.setListStyle(&listStyle3);
    h3.setListLevel(3);

    KoListStyle listStyle4;
    KoListLevelProperties llp4;
    llp4.setStartValue(1);
    llp4.setStyle(KoListStyle::DecimalItem);
    llp4.setLevel(4);
    llp4.setDisplayLevel(2);
    listStyle4.setLevelProperties(llp4);
    h4.setListStyle(&listStyle4);
    h4.setListLevel(4);

    QTextBlock block = m_doc->begin().next();
    h1.applyStyle(block);
    block = block.next();
    h2.applyStyle(block);
    block = block.next();
    h3.applyStyle(block);
    block = block.next();
    h3.applyStyle(block);
    block = block.next(); // roses
    h2.applyStyle(block);
    block = block.next();
    h1.applyStyle(block); // humans
    block = block.next();
    h2.applyStyle(block);
    block = block.next();
    h2.applyStyle(block);
    block = block.next();
    h1.applyStyle(block);
    block = block.next();
    h3.applyStyle(block); // notice missing h2
    block = block.next();
    QVERIFY(block.isValid());
    h4.applyStyle(block);

    m_layout->layout();

    block = m_doc->begin();
    QVERIFY(block.userData() == 0);
    block = block.next();
    static const char *const texts[] = {"1", "1.1.", "1.1.1", "1.1.2", "1.2.", "2", "2.1.", "2.2.", "3", "3.1.1", "1.1"};
    int i = 0;
    qreal indent = 0.0;
    while (block.isValid()) {
        KoTextBlockData *data = dynamic_cast<KoTextBlockData *>(block.userData());
        // qDebug() << "text: " << block.text();
        // qDebug() << "expected: " << texts[i];
        QVERIFY(data);
        // qDebug() << data->counterText();
        QCOMPARE(data->counterText(), QString(texts[i++]));
        if (i < 3) {
            // qDebug() << "indent:" << data->counterWidth();
            QVERIFY(indent < data->counterWidth()); // deeper indent, larger width
            indent = data->counterWidth();
        }
        block = block.next();
    }
}

void TestDocumentLayout::testNestedPrefixedLists()
{
    /* A list with different prefix for each level should show only the prefix of that level
     * Specifically we should not concatenate the prefixes of the higher levels
     * Specifically we should not concatenate the suffixes of the higher levels
     * That is only the prefix and the suffix of the current level should be applied
     */
    initForNewTest("MMMM\nSSSS\n");

    KoParagraphStyle h1;
    m_styleManager->add(&h1);
    KoParagraphStyle h2;
    m_styleManager->add(&h2);

    KoListStyle listStyle;
    KoListLevelProperties llp1;
    llp1.setStartValue(1);
    llp1.setStyle(KoListStyle::DecimalItem);
    llp1.setListItemPrefix("Main");
    llp1.setListItemSuffix(":");

    listStyle.setLevelProperties(llp1);

    KoListLevelProperties llp2;
    llp2.setStartValue(1);
    llp2.setStyle(KoListStyle::DecimalItem);
    llp2.setLevel(2);
    llp2.setListItemPrefix("Sub");
    llp2.setListItemSuffix("*");
    llp2.setDisplayLevel(2);
    listStyle.setLevelProperties(llp2);

    h1.setListStyle(&listStyle);
    h2.setListLevel(2);
    h2.setListStyle(&listStyle);

    QVERIFY(listStyle.hasLevelProperties(1));
    QVERIFY(listStyle.hasLevelProperties(2));
    QVERIFY(!listStyle.hasLevelProperties(3));

    QTextBlock block = m_doc->begin().next();
    h1.applyStyle(block);
    block = block.next();
    QVERIFY(block.isValid());
    h2.applyStyle(block);

    m_layout->layout();

    block = m_doc->begin();
    QVERIFY(block.userData() == 0);
    block = block.next();
    static const char *const texts[] = {"Main1:", "Sub1.1*"};
    int i = 0;
    while (block.isValid()) {
        KoTextBlockData *data = dynamic_cast<KoTextBlockData *>(block.userData());
        // qDebug() << "text: " << block.text();
        // qDebug() << "expected: " << texts[i];
        QVERIFY(data);
        // qDebug() << data->counterText();
        QCOMPARE(data->counterText(), QString(texts[i++]));
        block = block.next();
    }
}

void TestDocumentLayout::testAutoRestartList()
{
    initForNewTest("Humans\nGandhi\nEinstein\nInventions\nCar\nToilet\nLaboratory\n");

    KoParagraphStyle h1;
    m_styleManager->add(&h1);
    KoParagraphStyle h2;
    m_styleManager->add(&h2);

    KoListStyle listStyle;
    KoListLevelProperties llp = listStyle.levelProperties(1);
    llp.setStyle(KoListStyle::DecimalItem);
    llp.setStartValue(1);
    llp.setListItemSuffix(".");
    listStyle.setLevelProperties(llp);
    h1.setListStyle(&listStyle);

    KoListStyle listStyle2;
    KoListLevelProperties llp2 = listStyle2.levelProperties(2);
    llp2.setStyle(KoListStyle::DecimalItem);
    llp2.setStartValue(1);
    llp2.setDisplayLevel(2);
    llp2.setListItemSuffix(".");
    listStyle2.setLevelProperties(llp2);
    h2.setListStyle(&listStyle2);

    QTextBlock block = m_doc->begin();
    h1.applyStyle(block);
    block = block.next();
    h2.applyStyle(block);
    block = block.next();
    h2.applyStyle(block);
    block = block.next();
    h1.applyStyle(block); // inventions
    block = block.next();
    h2.applyStyle(block);
    QTextBlock car = block;
    block = block.next();
    h2.applyStyle(block);
    block = block.next();
    h2.applyStyle(block);

    m_layout->layout();

    KoTextBlockData *data = dynamic_cast<KoTextBlockData *>(car.userData());
    QVERIFY(data);
    // qDebug() << data->counterText();
    QCOMPARE(data->counterText(), QString("2.1."));
}

void TestDocumentLayout::testListParagraphIndent()
{
    // test that the list item is drawn indented on an indented paragraph.
    initForNewTest("Foo\nBar\n");

    KoParagraphStyle h1;
    m_styleManager->add(&h1);
    KoParagraphStyle h2;
    m_styleManager->add(&h2);
    h2.setTextIndent(10);

    KoListStyle listStyle;
    h1.setListStyle(&listStyle);
    KoListStyle listStyle2;
    h2.setListStyle(&listStyle2);

    QTextBlock block = m_doc->begin();
    h1.applyStyle(block);
    block = block.next();
    h2.applyStyle(block);

    m_layout->layout();

    // still at h2 parag!
    KoTextBlockData *data = dynamic_cast<KoTextBlockData *>(block.userData());
    QVERIFY(data);
    QCOMPARE(data->counterPosition(), QPointF(10, 14.4));
}

void TestDocumentLayout::testRomanNumbering()
{
    // Create, say 25 parags. layout. Then look to see if the items are proper roman numerals.
}

void TestDocumentLayout::testUpperAlphaNumbering()
{
    // Create, say 27 parags. layout. Then look to see if the items are proper A B C D
}

void TestDocumentLayout::testRestartNumbering()
{
    // create 5 items; restart the 3th. Check numbering.
    initForNewTest("a\nb\na\nb\nc");

    KoParagraphStyle h1;
    m_styleManager->add(&h1);
    KoListStyle listStyle;
    KoListLevelProperties llp;
    llp.setStyle(KoListStyle::DecimalItem);
    llp.setStartValue(1);
    listStyle.setLevelProperties(llp);
    h1.setListStyle(&listStyle);

    QTextBlock block = m_doc->begin();
    while (block.isValid()) {
        h1.applyStyle(block);
        block = block.next();
    }

    QTextCursor cursor(m_doc);
    cursor.setPosition(5);
    QCOMPARE(cursor.block().text(), QString("a"));
    QTextBlockFormat format = cursor.blockFormat();
    format.setProperty(KoParagraphStyle::RestartListNumbering, true);
    cursor.setBlockFormat(format);

    m_layout->layout();

    static const char *const values[] = {"1", "2", "1", "2", "3"};
    block = m_doc->begin();
    int i = 0;
    while (block.isValid()) {
        KoTextBlockData *data = dynamic_cast<KoTextBlockData *>(block.userData());
        QVERIFY(data);
        // qDebug() << data->counterText() << QString(values[i]);
        QCOMPARE(data->counterText(), QString(values[i++]));

        block = block.next();
    }
}

void TestDocumentLayout::testRightToLeftList()
{
    initForNewTest("a\nb\nc");
    KoParagraphStyle h1;
    h1.setTextProgressionDirection(KoText::RightLeftTopBottom);
    m_styleManager->add(&h1);
    KoListStyle listStyle;
    KoListLevelProperties llp = listStyle.levelProperties(1);
    llp.setStyle(KoListStyle::DecimalItem);
    listStyle.setLevelProperties(llp);
    h1.setListStyle(&listStyle);

    QTextBlock block = m_doc->begin();
    h1.applyStyle(block);
    block = block.next();
    h1.applyStyle(block);
    block = block.next();
    h1.applyStyle(block);
    block = block.next();

    m_layout->layout();

    block = m_doc->begin();
    while (block.isValid()) {
        KoTextBlockData *data = dynamic_cast<KoTextBlockData *>(block.userData());
        QVERIFY(data);
        QVERIFY(data->counterWidth() > 2);
        QVERIFY(data->counterPosition().x() > 100);
        QTextLine line = block.layout()->lineAt(0);
        QVERIFY(line.isValid());
        QCOMPARE(line.x(), (qreal)0);
        QCOMPARE(line.width() + data->counterWidth() + data->counterSpacing(), (qreal)200);
        block = block.next();
    }
}

void TestDocumentLayout::testLetterSynchronization()
{
    // make numbering be  'y, z, aa, bb, cc'
    initForNewTest("a\nb\na\nb\nc");

    KoParagraphStyle h1;
    m_styleManager->add(&h1);
    KoListStyle listStyle;
    KoListLevelProperties llp;
    llp.setStyle(KoListStyle::AlphaLowerItem);
    llp.setLetterSynchronization(true);
    llp.setStartValue(25);
    listStyle.setLevelProperties(llp);
    h1.setListStyle(&listStyle);

    QTextBlock block = m_doc->begin();
    while (block.isValid()) {
        h1.applyStyle(block);
        block = block.next();
    }

    m_layout->layout();

    static const char *const values[] = {"y", "z", "aa", "bb", "cc"};
    block = m_doc->begin();
    int i = 0;
    while (block.isValid()) {
        KoTextBlockData *data = dynamic_cast<KoTextBlockData *>(block.userData());
        QVERIFY(data);
        // qDebug() << "-> " << data->counterText() << Qt::endl;
        QCOMPARE(data->counterText(), QString(values[i++]));

        block = block.next();
    }
}

void TestDocumentLayout::testInvalidateLists()
{
    initForNewTest("Base\nListItem1\nListItem2");

    // KoParagraphStyle style;
    KoListStyle listStyle;
    KoListLevelProperties llp;
    llp.setStyle(KoListStyle::DecimalItem);
    listStyle.setLevelProperties(llp);
    // style.setListStyle(&listStyle);

    QTextBlock block = m_doc->begin().next();
    QVERIFY(block.isValid());
    listStyle.applyStyle(block);
    block = block.next();
    QVERIFY(block.isValid());
    listStyle.applyStyle(block);

    m_layout->layout();

    // check the list items were done (semi) properly
    block = m_doc->begin().next();
    QVERIFY(block.textList());
    KoTextBlockData *data = dynamic_cast<KoTextBlockData *>(block.userData());
    QVERIFY(data);
    QVERIFY(data->hasCounterData());

    QTextCursor cursor(m_doc);
    cursor.setPosition(10); // list item1
    cursor.insertText("x");
    QCOMPARE(data->hasCounterData(), true); // nothing changed

    cursor.setPosition(22); // list item2
    cursor.insertText("x");
    QCOMPARE(data->hasCounterData(), true); // nothing changed
    cursor.deleteChar();
    QCOMPARE(data->hasCounterData(), true); // nothing changed

    cursor.setPosition(25); // end of doc
    cursor.insertBlock();
    block = cursor.block();
    QVERIFY(block.textList());
    QVERIFY(block.userData() == 0);

    QCOMPARE(data->hasCounterData(), false); // inserting a new block on this list made the list be invalidated
}

void TestDocumentLayout::testCenteredItems()
{
    initForNewTest("ListItem\nListItem\nListItem");

    KoListStyle listStyle;
    KoListLevelProperties llp;
    llp.setStyle(KoListStyle::DecimalItem);
    listStyle.setLevelProperties(llp);

    QTextBlock block = m_doc->begin(); // normal block
    QVERIFY(block.isValid());
    listStyle.applyStyle(block);
    block = block.next(); // centered block
    QVERIFY(block.isValid());
    listStyle.applyStyle(block);
    QTextBlockFormat fmt;
    fmt.setAlignment(Qt::AlignHCenter);
    QTextCursor cursor(block);
    cursor.mergeBlockFormat(fmt);
    block = block.next(); // centered RTL text.
    listStyle.applyStyle(block);
    cursor = QTextCursor(block);
    fmt.setProperty(KoParagraphStyle::TextProgressionDirection, KoText::RightLeftTopBottom);
    cursor.mergeBlockFormat(fmt);

    m_layout->layout();

    block = m_doc->begin();
    QTextLayout *layout = block.layout();
    QTextLine line1 = layout->lineAt(0);
    KoTextBlockData *data1 = dynamic_cast<KoTextBlockData *>(block.userData());
    QVERIFY(line1.isValid());
    QVERIFY(line1.width() < 200); // the counter takes some space.

    block = block.next();
    layout = block.layout();
    QTextLine line2 = layout->lineAt(0);
    KoTextBlockData *data2 = dynamic_cast<KoTextBlockData *>(block.userData());
    QVERIFY(line2.isValid());
    QVERIFY(line2.width() < 200); // the counter takes some space.
    QCOMPARE(line1.width(), line2.width());

    const qreal width1 = line1.naturalTextWidth() + data1->counterWidth() + data1->counterSpacing();
    const qreal width2 = line2.naturalTextWidth() + data2->counterWidth() + data2->counterSpacing();
    QCOMPARE(width1, width2);
    QVERIFY(data1->counterPosition().x() < data2->counterPosition().x());
    const qreal padding = (200 - width2) / 2;
    QVERIFY(padding > 0); // not really a layout test, but the rest will be bogus otherwise.
    QCOMPARE(data2->counterPosition().x(), padding); // close to the centered text.

    // right to left parag places the counter on the right. Its centered, so not the far right.
    block = block.next();
    layout = block.layout();
    QTextLine line = layout->lineAt(0);
    KoTextBlockData *data = dynamic_cast<KoTextBlockData *>(block.userData());
    QCOMPARE(data->counterPosition().x(), 200 - padding - data->counterWidth());
}

void TestDocumentLayout::testMultiLevel()
{
    initForNewTest("ListItem1\n");
    KoListStyle listStyle;
    KoListLevelProperties llp;
    llp.setStyle(KoListStyle::DecimalItem);
    llp.setLevel(3);
    llp.setDisplayLevel(4); // we won't show a .0 at the end so this is truncated to 3
    listStyle.setLevelProperties(llp);

    QTextBlock block = m_doc->begin();
    QVERIFY(block.isValid());
    listStyle.applyStyle(block);
    QVERIFY(block.textList());

    m_layout->layout();

    KoTextBlockData *data = dynamic_cast<KoTextBlockData *>(block.userData());
    QVERIFY(data);
    QVERIFY(data->hasCounterData());
    QCOMPARE(data->counterText(), QString("1.1.1"));
}
