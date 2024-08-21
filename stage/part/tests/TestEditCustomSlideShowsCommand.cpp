/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Paul Mendez <paulestebanms@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "TestEditCustomSlideShowsCommand.h"
#include "KPrCustomSlideShows.h"
#include "KPrCustomSlideShowsModel.h"
#include "KPrDocument.h"
#include "KoPAMasterPage.h"
#include "KoPAPage.h"
#include "PAMock.h"
#include "commands/KPrEditCustomSlideShowsCommand.h"

#include <QTest>

void TestEditCustomSlideShowsCommand::moveSingleSlide()
{
    MockDocument doc;

    KoPAMasterPage *master1 = new KoPAMasterPage();
    doc.insertPage(master1, 0);

    KoPAPage *page1 = new KoPAPage(master1);
    doc.insertPage(page1, 0);

    KoPAPage *p1 = dynamic_cast<KoPAPage *>(doc.pageByIndex(0, false));
    KoPAMasterPage *m1 = dynamic_cast<KoPAMasterPage *>(doc.pageByIndex(0, true));

    QVERIFY(p1 != nullptr);
    QVERIFY(m1 != nullptr);

    KoPAPage *page2 = new KoPAPage(master1);
    doc.insertPage(page2, 0);

    KoPAPage *page3 = new KoPAPage(master1);
    doc.insertPage(page3, 0);

    QList<KoPAPageBase *> slideList;

    slideList.append(page1);
    slideList.append(page2);
    slideList.append(page3);

    QString customShowName = "test 1";

    doc.customSlideShows()->insert(customShowName, slideList);

    QList<KoPAPageBase *> initialSlideShow = doc.customSlideShows()->getByName(customShowName);

    QCOMPARE(initialSlideShow.count(), 3);

    initialSlideShow.move(0, 2);

    KPrEditCustomSlideShowsCommand command(&doc, customShowName, initialSlideShow);

    command.redo();
    QList<KoPAPageBase *> modifiedSlideShow = doc.customSlideShows()->getByName(customShowName);
    QCOMPARE(modifiedSlideShow, initialSlideShow);

    command.undo();
    modifiedSlideShow = doc.customSlideShows()->getByName(customShowName);
    QCOMPARE(modifiedSlideShow.at(0), initialSlideShow.at(2));
    QCOMPARE(modifiedSlideShow.at(1), initialSlideShow.at(0));
    QCOMPARE(modifiedSlideShow.at(2), initialSlideShow.at(1));
}

QTEST_MAIN(TestEditCustomSlideShowsCommand)
