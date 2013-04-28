/*
 *  Copyright (c) 2011 Boudewijn Rempt <boud@valdyas.org>
 *  Copyright (c) 2012 C. Boemann <cbo@boemann.dk>
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
#include <KoTextRangeManager.h>

#include <KWDocument.h>
#include <KWPart.h>
#include "KWAboutData.h"
#include <frames/KWTextFrameSet.h>

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
        KWDocument *doc = new KWDocument(new KWPart(0));
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

        KoTextRangeManager *rangeManager = koTextDocument.textRangeManager();
        QTextCursor cursor(editor->document());
        KoBookmark *mark = new KoBookmark(cursor);
        mark->setName("TESTMARK");
        rangeManager->insert(mark);

        editor->insertTable(5,10);
        const QTextTable *table = editor->currentTable();
        Q_ASSERT(table);

        editor->setPosition(table->lastPosition());
        mark->setRangeEnd(editor->position());

        editor->insertText(lorem);

        // Save the document
        KUrl url(QString(FILES_OUTPUT_DIR) + "/bookmark_roundtrip.odt");
        doc->documentPart()->saveAs(url);

        // check the number of bookmarks
        QCOMPARE(rangeManager->bookmarkManager()->bookmarkNameList().length(), 1);
        QCOMPARE(rangeManager->textRanges().length(), 1);

        delete doc;
    }
    {
        // Load the document
        KWDocument *doc = new KWDocument(new KWPart(0));
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

        KoTextDocument koTextDocument(textDocument);

        // check the number of bookmarks
        KoTextRangeManager *rangeManager = koTextDocument.textRangeManager();
        QCOMPARE(rangeManager->bookmarkManager()->bookmarkNameList().length(), 1);
        QCOMPARE(rangeManager->textRanges().length(), 1);

        delete doc;
    }

}

QTEST_KDEMAIN(TestKoBookmark, GUI)

#include "TestKoBookmark.moc"
