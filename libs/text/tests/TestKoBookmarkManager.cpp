/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2011 Boudewijn Rempt <boud@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "TestKoBookmarkManager.h"

#include <QList>
#include <QString>
#include <QTest>
#include <QTextDocument>

#include <KoBookmark.h>
#include <KoBookmarkManager.h>
#include <KoInlineTextObjectManager.h>
#include <KoTextDocument.h>
#include <KoTextEditor.h>

void TestKoBookmarkManager::testCreation()
{
    KoBookmarkManager *manager = new KoBookmarkManager();
    Q_ASSERT(manager);

    KoBookmark *bm = manager->bookmark("bla");
    Q_ASSERT(bm == nullptr);
    Q_UNUSED(bm);

    QList<QString> bmlist = manager->bookmarkNameList();
    Q_ASSERT(bmlist.isEmpty());

    delete manager;
}

void TestKoBookmarkManager::testInsertAndRetrieve()
{
    KoBookmarkManager manager;
    QTextDocument doc;

    // Insert a startmark
    QTextCursor cursor(doc.firstBlock());
    KoBookmark *mark = new KoBookmark(cursor);
    manager.insert("start!", mark);
    manager.insert("another1", new KoBookmark(cursor));
    manager.insert("another2", new KoBookmark(cursor));

    KoBookmark *bm = manager.bookmark("start!");
    Q_ASSERT(bm == mark);
    Q_UNUSED(bm);
}

void TestKoBookmarkManager::testRemove()
{
    KoBookmarkManager manager;
    QTextDocument doc;

    // Insert a mark
    QTextCursor cursor(doc.firstBlock());
    KoBookmark *mark = new KoBookmark(cursor);
    manager.insert("start!", mark);
    manager.insert("another1", new KoBookmark(cursor));
    manager.insert("another2", new KoBookmark(cursor));

    manager.remove("start!");

    Q_ASSERT(manager.bookmark("start!") == nullptr);
    Q_ASSERT(manager.bookmarkNameList().length() == 2);
    Q_ASSERT(!manager.bookmarkNameList().contains("start!"));
}

void TestKoBookmarkManager::testRename()
{
    KoBookmarkManager manager;
    QTextDocument doc;

    // Insert a mark
    QTextCursor cursor(doc.firstBlock());
    KoBookmark *mark = new KoBookmark(cursor);
    manager.insert("start!", mark);
    Q_ASSERT(mark->name() == "start!");
    Q_ASSERT(manager.bookmarkNameList().length() == 1);
    Q_ASSERT(manager.bookmarkNameList().contains("start!"));

    KoBookmark *another = new KoBookmark(cursor);
    manager.insert("another", another);

    manager.rename("start!", "renamed!");

    Q_ASSERT(mark->name() == "renamed!");
    Q_ASSERT(another->name() == "another");
}

QTEST_MAIN(TestKoBookmarkManager)
