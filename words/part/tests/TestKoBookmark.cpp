/*
 *  Copyright (c) 2011 Boudewijn Rempt <boud@valdyas.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "TestKoBookmark.h"

#include <QTest>
#include <QDebug>
#include <QString>
#include <QTextDocument>
#include <QTextTable>
#include <QList>

#include <KoPart.h>
#include <KoBookmark.h>
#include <KoStore.h>
#include <KoTextDocument.h>
#include <KoApplication.h>
#include <KoXmlWriter.h>
#include <KoShapeRegistry.h>
#include <KoXmlReader.h>
#include <KoOdfReadStore.h>
#include <KoOdfWriteStore.h>
#include <KoStyleManager.h>
#include <KoXmlNS.h>
#include <KoDocument.h>
#include <KoTextShapeDataBase.h>
#include <KoTextDocument.h>
#include <KoBookmarkManager.h>
#include <KoInlineTextObjectManager.h>

#include <KWDocument.h>
#include "KWAboutData.h"
#include <frames/KWTextFrameSet.h>


void TestKoBookmark::testInsertion()
{
    QObject parent;

    // create a document
    QTextDocument doc;

    KoInlineTextObjectManager inlineObjectManager(&parent);
    KoTextDocument textDoc(&doc);
    textDoc.setInlineTextObjectManager(&inlineObjectManager);

    KoTextEditor editor(&doc);
    textDoc.setTextEditor(&editor);

    // enter some lorem ipsum
    editor.insertText("1bla bla bla");

    KoBookmark *startmark = new KoBookmark(editor.document());
    startmark->setType(KoBookmark::StartBookmark);
    startmark->setName("start!");
    editor.insertInlineObject(startmark);

    editor.insertText("2bla bla bla");

    KoBookmark *endmark = new KoBookmark(editor.document());
    endmark->setType(KoBookmark::EndBookmark);
    startmark->setEndBookmark(endmark);
    editor.insertInlineObject(endmark);

    editor.insertText("3bla bla bla");

    startmark = new KoBookmark(editor.document());
    startmark->setType(KoBookmark::StartBookmark);
    startmark->setName("start 2!");
    editor.insertInlineObject(startmark);

    editor.insertText("4bla bla bla");

    endmark = new KoBookmark(editor.document());
    endmark->setType(KoBookmark::EndBookmark);
    startmark->setEndBookmark(endmark);
    editor.insertInlineObject(endmark);

    editor.insertText("5bla bla bla");

    QCOMPARE(inlineObjectManager.bookmarkManager()->bookmarkNameList().length(), 2);
    QCOMPARE(inlineObjectManager.inlineTextObjects().length(), 4);

    int bookmarksFound = 0;
    QTextCursor cursor = doc.find(QString(QChar::ObjectReplacementCharacter), 0);
    while (!cursor.isNull()) {
        QTextCharFormat fmt = cursor.charFormat();
        KoInlineObject *obj = inlineObjectManager.inlineTextObject(fmt);
        KoBookmark *bm = dynamic_cast<KoBookmark*>(obj);
        if (bm) {
            bookmarksFound++;
        }
        cursor = doc.find(QString(QChar::ObjectReplacementCharacter), cursor.position());
    }
    QCOMPARE(bookmarksFound, 4);

}

void TestKoBookmark::testRoundtrip()
{
    const QString lorem(
                "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor"
                "incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud"
                "exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.\n"
                "Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla"
                "pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia"
                "deserunt mollit anim id est laborum.\n"
                );
    {
        // Get the words part and create a document
        KWDocument *doc = new KWDocument(new MockPart);
        Q_ASSERT(doc);
        doc->setAutoSave(0);
        doc->initEmpty();

        // get the main text frame
        KWTextFrameSet *mainFrameSet = doc->mainFrameSet();
        Q_ASSERT(mainFrameSet);

        QTextDocument *textDocument = mainFrameSet->document();
        Q_ASSERT(textDocument);

        // Insert some text a bookmark pair
        KoTextDocument koTextDocument(textDocument);
        KoTextEditor *editor = koTextDocument.textEditor();
        editor->insertText(lorem);

        KoBookmark *startmark = new KoBookmark(editor->document());
        startmark->setType(KoBookmark::StartBookmark);
        startmark->setName("START");

        editor->insertTable(5,10);
        const QTextTable *table = editor->currentTable();
        Q_ASSERT(table);

        editor->setPosition(table->firstPosition());
        editor->movePosition(QTextCursor::PreviousCharacter);
        editor->insertInlineObject(startmark);

        KoBookmark *endmark = new KoBookmark(editor->document());
        endmark->setName("END");
        endmark->setType(KoBookmark::EndBookmark);
        startmark->setEndBookmark(endmark);

        QCOMPARE(endmark->name(), startmark->name());

        editor->setPosition(table->lastPosition());
        editor->movePosition(QTextCursor::NextCharacter);
        editor->insertInlineObject(endmark);

        editor->insertText(lorem);

        // Save the document
        KUrl url(QString(FILES_OUTPUT_DIR) + "/bookmark_roundtrip.odt");
        doc->documentPart()->saveAs(url);

        // check the number of bookmark characters
        KoInlineTextObjectManager *inlineObjectManager = koTextDocument.inlineTextObjectManager();
        QCOMPARE(inlineObjectManager->bookmarkManager()->bookmarkNameList().length(), 1);
        QCOMPARE(inlineObjectManager->inlineTextObjects().length(), 2);
        int bookmarksFound = 0;
        QTextCursor cursor = textDocument->find(QString(QChar::ObjectReplacementCharacter), 0);
        while (!cursor.isNull()) {
            QTextCharFormat fmt = cursor.charFormat();
            KoInlineObject *obj = inlineObjectManager->inlineTextObject(fmt);
            KoBookmark *bm = dynamic_cast<KoBookmark*>(obj);
            if (bm) {
                bookmarksFound++;
            }
            cursor = textDocument->find(QString(QChar::ObjectReplacementCharacter), cursor.position());
        }
        QCOMPARE(bookmarksFound, 2);

        delete doc;
    }
    {
        // Load the document
        KWDocument *doc = new KWDocument(new MockPart);
        Q_ASSERT(doc);
        doc->setAutoSave(0);
        KUrl url(QString(FILES_OUTPUT_DIR) + "/bookmark_roundtrip.odt");
        // this also creates a view...
        bool result = doc->openUrl(url);
        Q_ASSERT(result);
        // get the main text frame
        KWTextFrameSet *mainFrameSet = doc->mainFrameSet();
        Q_ASSERT(mainFrameSet);

        QTextDocument *textDocument = mainFrameSet->document();
        Q_ASSERT(textDocument);

        // Insert some text a bookmark pair
        KoTextDocument koTextDocument(textDocument);

        // check the number bookmark characters
        // check the number of bookmark characters
        KoInlineTextObjectManager *inlineObjectManager = koTextDocument.inlineTextObjectManager();
        QCOMPARE(inlineObjectManager->bookmarkManager()->bookmarkNameList().length(), 1);
        QCOMPARE(inlineObjectManager->inlineTextObjects().length(), 2);
        int bookmarksFound = 0;
        QTextCursor cursor = textDocument->find(QString(QChar::ObjectReplacementCharacter), 0);
        while (!cursor.isNull()) {
            QTextCharFormat fmt = cursor.charFormat();
            KoInlineObject *obj = inlineObjectManager->inlineTextObject(fmt);
            KoBookmark *bm = dynamic_cast<KoBookmark*>(obj);
            if (bm) {

                bookmarksFound++;
            }
            cursor = textDocument->find(QString(QChar::ObjectReplacementCharacter), cursor.position());
        }
        QCOMPARE(bookmarksFound, 2);
        delete doc;
    }

}

QTEST_KDEMAIN(TestKoBookmark, GUI)

#include "TestKoBookmark.moc"
