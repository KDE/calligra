/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2014-2015 Denis Kuplyakov <dener.kup@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

void TestKoTextEditor::testInsertSectionHandling_data()
{
    QTest::addColumn<int>("insertPosition");
    QTest::addColumn<int>("neededBlockCount");
    QTest::addColumn<QVector<QVector<QString>>>("needStartings");
    QTest::addColumn<QVector<QVector<QString>>>("needEndings");

    QTest::newRow("Test #0") << 0 << 12
                             << (QVector<QVector<QString>>()
                                 << (QVector<QString>() << "0") << (QVector<QString>() << "New section 6") << (QVector<QString>() << "1")
                                 << (QVector<QString>() << "2") << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>() << "3")
                                 << (QVector<QString>()) << (QVector<QString>() << "4") << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>()))
                             << (QVector<QVector<QString>>()
                                 << (QVector<QString>()) << (QVector<QString>() << "New section 6") << (QVector<QString>()) << (QVector<QString>())
                                 << (QVector<QString>() << "2") << (QVector<QString>() << "1") << (QVector<QString>()) << (QVector<QString>() << "3")
                                 << (QVector<QString>()) << (QVector<QString>() << "4") << (QVector<QString>() << "0") << (QVector<QString>()));
    QTest::newRow("Test #1") << 39 << 12
                             << (QVector<QVector<QString>>()
                                 << (QVector<QString>() << "0") << (QVector<QString>() << "1") << (QVector<QString>() << "2") << (QVector<QString>())
                                 << (QVector<QString>()) << (QVector<QString>() << "3") << (QVector<QString>()) << (QVector<QString>() << "4")
                                 << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>() << "New section 6") << (QVector<QString>()))
                             << (QVector<QVector<QString>>()
                                 << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>() << "2")
                                 << (QVector<QString>() << "1") << (QVector<QString>()) << (QVector<QString>() << "3") << (QVector<QString>())
                                 << (QVector<QString>() << "4") << (QVector<QString>())
                                 << (QVector<QString>() << "New section 6"
                                                        << "0")
                                 << (QVector<QString>()));
    QTest::newRow("Test #2") << 40 << 12
                             << (QVector<QVector<QString>>()
                                 << (QVector<QString>() << "0") << (QVector<QString>() << "1") << (QVector<QString>() << "2") << (QVector<QString>())
                                 << (QVector<QString>()) << (QVector<QString>() << "3") << (QVector<QString>()) << (QVector<QString>() << "4")
                                 << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>() << "New section 6"))
                             << (QVector<QVector<QString>>() << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>())
                                                             << (QVector<QString>() << "2") << (QVector<QString>() << "1") << (QVector<QString>())
                                                             << (QVector<QString>() << "3") << (QVector<QString>()) << (QVector<QString>() << "4")
                                                             << (QVector<QString>() << "0") << (QVector<QString>()) << (QVector<QString>() << "New section 6"));
    QTest::newRow("Test #3") << 5 << 12
                             << (QVector<QVector<QString>>()
                                 << (QVector<QString>() << "0") << (QVector<QString>() << "1") << (QVector<QString>() << "New section 6")
                                 << (QVector<QString>() << "2") << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>() << "3")
                                 << (QVector<QString>()) << (QVector<QString>() << "4") << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>()))
                             << (QVector<QVector<QString>>()
                                 << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>() << "New section 6") << (QVector<QString>())
                                 << (QVector<QString>() << "2") << (QVector<QString>() << "1") << (QVector<QString>()) << (QVector<QString>() << "3")
                                 << (QVector<QString>()) << (QVector<QString>() << "4") << (QVector<QString>() << "0") << (QVector<QString>()));
    QTest::newRow("Test #4") << 8 << 12
                             << (QVector<QVector<QString>>()
                                 << (QVector<QString>() << "0") << (QVector<QString>() << "1") << (QVector<QString>() << "2")
                                 << (QVector<QString>() << "New section 6") << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>() << "3")
                                 << (QVector<QString>()) << (QVector<QString>() << "4") << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>()))
                             << (QVector<QVector<QString>>()
                                 << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>() << "New section 6")
                                 << (QVector<QString>() << "2") << (QVector<QString>() << "1") << (QVector<QString>()) << (QVector<QString>() << "3")
                                 << (QVector<QString>()) << (QVector<QString>() << "4") << (QVector<QString>() << "0") << (QVector<QString>()));
    QTest::newRow("Test #5") << 20 << 12
                             << (QVector<QVector<QString>>()
                                 << (QVector<QString>() << "0") << (QVector<QString>() << "1") << (QVector<QString>() << "2") << (QVector<QString>())
                                 << (QVector<QString>()) << (QVector<QString>() << "3") << (QVector<QString>() << "New section 6") << (QVector<QString>())
                                 << (QVector<QString>() << "4") << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>()))
                             << (QVector<QVector<QString>>() << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>())
                                                             << (QVector<QString>() << "2") << (QVector<QString>() << "1") << (QVector<QString>())
                                                             << (QVector<QString>() << "New section 6") << (QVector<QString>() << "3") << (QVector<QString>())
                                                             << (QVector<QString>() << "4") << (QVector<QString>() << "0") << (QVector<QString>()));
    QTest::newRow("Test #6") << 1 << 12
                             << (QVector<QVector<QString>>()
                                 << (QVector<QString>() << "0") << (QVector<QString>() << "New section 6") << (QVector<QString>() << "1")
                                 << (QVector<QString>() << "2") << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>() << "3")
                                 << (QVector<QString>()) << (QVector<QString>() << "4") << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>()))
                             << (QVector<QVector<QString>>()
                                 << (QVector<QString>()) << (QVector<QString>() << "New section 6") << (QVector<QString>()) << (QVector<QString>())
                                 << (QVector<QString>() << "2") << (QVector<QString>() << "1") << (QVector<QString>()) << (QVector<QString>() << "3")
                                 << (QVector<QString>()) << (QVector<QString>() << "4") << (QVector<QString>() << "0") << (QVector<QString>()));
}
