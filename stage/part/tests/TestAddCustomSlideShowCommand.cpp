/* This file is part of the KDE project
* Copyright (C) 2011 Paul Mendez <paulestebanms@gmail.com>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Library General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public License
* along with this library; see the file COPYING.LIB.  If not, write to
* the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
* Boston, MA 02110-1301, USA.
*/

#include "TestAddCustomSlideShowCommand.h"

#include "KPrDocument.h"
#include "KoPAMasterPage.h"
#include "KoPAPage.h"
#include "PAMock.h"
#include "commands/KPrAddCustomSlideShowCommand.h"
#include "KPrCustomSlideShows.h"
#include "KPrCustomSlideShowsModel.h"

#include <QTest>

void TestAddCustomSlideShowCommand::addCustomSlideShow()
{
    MockDocument doc;

    KoPAMasterPage *master1 = new KoPAMasterPage();
    doc.insertPage(master1, 0);

    KoPAPage *page1 = new KoPAPage(master1);
    doc.insertPage(page1, 0);

    KoPAPage *p1 = dynamic_cast<KoPAPage *>(doc.pageByIndex(0, false));
    KoPAMasterPage * m1 = dynamic_cast<KoPAMasterPage *>(doc.pageByIndex(0, true));

    QVERIFY(p1 != 0);
    QVERIFY(m1 != 0);

    QString customShowName = "test 1";

    KPrCustomSlideShowsModel model(&doc, 0);

    KPrAddCustomSlideShowCommand cmd(&doc, &model, customShowName);

    cmd.redo();
    QCOMPARE(doc.customSlideShows()->names().count(), 1);

    cmd.undo();
    QCOMPARE(doc.customSlideShows()->names().count(), 0);
}

QTEST_MAIN(TestAddCustomSlideShowCommand)
