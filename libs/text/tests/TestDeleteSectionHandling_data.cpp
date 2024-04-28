/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2014-2015 Denis Kuplyakov <dener.kup@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

void TestKoTextEditor::testDeleteSectionHandling_data()
{
    QTest::addColumn<int>("selectionStart");
    QTest::addColumn<int>("selectionEnd");
    QTest::addColumn<int>("neededBlockCount");
    QTest::addColumn<QVector<QVector<QString>>>("needStartings");
    QTest::addColumn<QVector<QVector<QString>>>("needEndings");

    QTest::newRow("Simple deletion, no effect to sections.")
        << 1 << 2 << 11
        << (QVector<QVector<QString>>() << (QVector<QString>() << "0") << (QVector<QString>() << "1") << (QVector<QString>() << "2") << (QVector<QString>())
                                        << (QVector<QString>()) << (QVector<QString>() << "3") << (QVector<QString>()) << (QVector<QString>() << "4")
                                        << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>()))
        << (QVector<QVector<QString>>() << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>() << "2")
                                        << (QVector<QString>() << "1") << (QVector<QString>()) << (QVector<QString>() << "3") << (QVector<QString>())
                                        << (QVector<QString>() << "4") << (QVector<QString>() << "0") << (QVector<QString>()));
    QTest::newRow("Deleting entire 1st section begin.")
        << 4 << 8 << 10
        << (QVector<QVector<QString>>() << (QVector<QString>() << "0")
                                        << (QVector<QString>() << "1"
                                                               << "2")
                                        << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>() << "3") << (QVector<QString>())
                                        << (QVector<QString>() << "4") << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>()))
        << (QVector<QVector<QString>>() << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>() << "2") << (QVector<QString>() << "1")
                                        << (QVector<QString>()) << (QVector<QString>() << "3") << (QVector<QString>()) << (QVector<QString>() << "4")
                                        << (QVector<QString>() << "0") << (QVector<QString>()));
    QTest::newRow("Deleting entire 1st section begin and part of 2nd.")
        << 4 << 9 << 10
        << (QVector<QVector<QString>>() << (QVector<QString>() << "0")
                                        << (QVector<QString>() << "1"
                                                               << "2")
                                        << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>() << "3") << (QVector<QString>())
                                        << (QVector<QString>() << "4") << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>()))
        << (QVector<QVector<QString>>() << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>() << "2") << (QVector<QString>() << "1")
                                        << (QVector<QString>()) << (QVector<QString>() << "3") << (QVector<QString>()) << (QVector<QString>() << "4")
                                        << (QVector<QString>() << "0") << (QVector<QString>()));
    QTest::newRow("Deleting part of 1st section begin.")
        << 5 << 8 << 10
        << (QVector<QVector<QString>>() << (QVector<QString>() << "0") << (QVector<QString>() << "1") << (QVector<QString>() << "2") << (QVector<QString>())
                                        << (QVector<QString>() << "3") << (QVector<QString>()) << (QVector<QString>() << "4") << (QVector<QString>())
                                        << (QVector<QString>()) << (QVector<QString>()))
        << (QVector<QVector<QString>>() << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>() << "2") << (QVector<QString>() << "1")
                                        << (QVector<QString>()) << (QVector<QString>() << "3") << (QVector<QString>()) << (QVector<QString>() << "4")
                                        << (QVector<QString>() << "0") << (QVector<QString>()));
    QTest::newRow("Deleting part of 1st section begin and part of 2nd.")
        << 5 << 9 << 10
        << (QVector<QVector<QString>>() << (QVector<QString>() << "0") << (QVector<QString>() << "1") << (QVector<QString>() << "2") << (QVector<QString>())
                                        << (QVector<QString>() << "3") << (QVector<QString>()) << (QVector<QString>() << "4") << (QVector<QString>())
                                        << (QVector<QString>()) << (QVector<QString>()))
        << (QVector<QVector<QString>>() << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>() << "2") << (QVector<QString>() << "1")
                                        << (QVector<QString>()) << (QVector<QString>() << "3") << (QVector<QString>()) << (QVector<QString>() << "4")
                                        << (QVector<QString>() << "0") << (QVector<QString>()));
    QTest::newRow("Deleting all sections except 0th one.")
        << 4 << 36 << 3 << (QVector<QVector<QString>>() << (QVector<QString>() << "0") << (QVector<QString>()) << (QVector<QString>()))
        << (QVector<QVector<QString>>() << (QVector<QString>()) << (QVector<QString>() << "0") << (QVector<QString>()));
    QTest::newRow("Deleting 3rd and part of 4th.")
        << 20 << 32 << 8
        << (QVector<QVector<QString>>() << (QVector<QString>() << "0") << (QVector<QString>() << "1") << (QVector<QString>() << "2") << (QVector<QString>())
                                        << (QVector<QString>()) << (QVector<QString>() << "4") << (QVector<QString>()) << (QVector<QString>()))
        << (QVector<QVector<QString>>() << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>() << "2")
                                        << (QVector<QString>() << "1") << (QVector<QString>() << "4") << (QVector<QString>() << "0") << (QVector<QString>()));
    QTest::newRow("Deleting all the sections.") << 0 << 40 << 1 << (QVector<QVector<QString>>() << (QVector<QString>()))
                                                << (QVector<QVector<QString>>() << (QVector<QString>()));
    QTest::newRow("Deleting part of 3rd and part of 4th.")
        << 25 << 29 << 10
        << (QVector<QVector<QString>>() << (QVector<QString>() << "0") << (QVector<QString>() << "1") << (QVector<QString>() << "2") << (QVector<QString>())
                                        << (QVector<QString>()) << (QVector<QString>() << "3") << (QVector<QString>()) << (QVector<QString>() << "4")
                                        << (QVector<QString>()) << (QVector<QString>()))
        << (QVector<QVector<QString>>() << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>() << "2")
                                        << (QVector<QString>() << "1") << (QVector<QString>()) << (QVector<QString>() << "3") << (QVector<QString>() << "4")
                                        << (QVector<QString>() << "0") << (QVector<QString>()));
    QTest::newRow("Deleting 2nd end.") << 12 << 16 << 10
                                       << (QVector<QVector<QString>>()
                                           << (QVector<QString>() << "0") << (QVector<QString>() << "1") << (QVector<QString>() << "2") << (QVector<QString>())
                                           << (QVector<QString>() << "3") << (QVector<QString>()) << (QVector<QString>() << "4") << (QVector<QString>())
                                           << (QVector<QString>()) << (QVector<QString>()))
                                       << (QVector<QVector<QString>>()
                                           << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>() << "2") << (QVector<QString>() << "1")
                                           << (QVector<QString>()) << (QVector<QString>() << "3") << (QVector<QString>()) << (QVector<QString>() << "4")
                                           << (QVector<QString>() << "0") << (QVector<QString>()));
    QTest::newRow("Deleting 2nd end and part of 1st.")
        << 12 << 17 << 10
        << (QVector<QVector<QString>>() << (QVector<QString>() << "0") << (QVector<QString>() << "1") << (QVector<QString>() << "2") << (QVector<QString>())
                                        << (QVector<QString>() << "3") << (QVector<QString>()) << (QVector<QString>() << "4") << (QVector<QString>())
                                        << (QVector<QString>()) << (QVector<QString>()))
        << (QVector<QVector<QString>>() << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>() << "2") << (QVector<QString>() << "1")
                                        << (QVector<QString>()) << (QVector<QString>() << "3") << (QVector<QString>()) << (QVector<QString>() << "4")
                                        << (QVector<QString>() << "0") << (QVector<QString>()));
    QTest::newRow("Deleting part of 2nd end.") << 13 << 16 << 10
                                               << (QVector<QVector<QString>>()
                                                   << (QVector<QString>() << "0") << (QVector<QString>() << "1") << (QVector<QString>() << "2")
                                                   << (QVector<QString>()) << (QVector<QString>() << "3") << (QVector<QString>()) << (QVector<QString>() << "4")
                                                   << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>()))
                                               << (QVector<QVector<QString>>()
                                                   << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>())
                                                   << (QVector<QString>() << "2"
                                                                          << "1")
                                                   << (QVector<QString>()) << (QVector<QString>() << "3") << (QVector<QString>()) << (QVector<QString>() << "4")
                                                   << (QVector<QString>() << "0") << (QVector<QString>()));
    QTest::newRow("Deleting part of 2nd end and part of 1st.")
        << 13 << 17 << 10
        << (QVector<QVector<QString>>() << (QVector<QString>() << "0") << (QVector<QString>() << "1") << (QVector<QString>() << "2") << (QVector<QString>())
                                        << (QVector<QString>() << "3") << (QVector<QString>()) << (QVector<QString>() << "4") << (QVector<QString>())
                                        << (QVector<QString>()) << (QVector<QString>()))
        << (QVector<QVector<QString>>() << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>())
                                        << (QVector<QString>() << "2"
                                                               << "1")
                                        << (QVector<QString>()) << (QVector<QString>() << "3") << (QVector<QString>()) << (QVector<QString>() << "4")
                                        << (QVector<QString>() << "0") << (QVector<QString>()));
    QTest::newRow("Random test #0") << 5 << 36 << 3
                                    << (QVector<QVector<QString>>() << (QVector<QString>() << "0") << (QVector<QString>() << "1") << (QVector<QString>()))
                                    << (QVector<QVector<QString>>() << (QVector<QString>())
                                                                    << (QVector<QString>() << "1"
                                                                                           << "0")
                                                                    << (QVector<QString>()));
    QTest::newRow("Random test #1") << 0 << 23 << 6
                                    << (QVector<QVector<QString>>() << (QVector<QString>() << "0"
                                                                                           << "3")
                                                                    << (QVector<QString>()) << (QVector<QString>() << "4") << (QVector<QString>())
                                                                    << (QVector<QString>()) << (QVector<QString>()))
                                    << (QVector<QVector<QString>>() << (QVector<QString>()) << (QVector<QString>() << "3") << (QVector<QString>())
                                                                    << (QVector<QString>() << "4") << (QVector<QString>() << "0") << (QVector<QString>()));
    QTest::newRow("Random test #2") << 7 << 19 << 8
                                    << (QVector<QVector<QString>>()
                                        << (QVector<QString>() << "0") << (QVector<QString>() << "1") << (QVector<QString>() << "3") << (QVector<QString>())
                                        << (QVector<QString>() << "4") << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>()))
                                    << (QVector<QVector<QString>>()
                                        << (QVector<QString>()) << (QVector<QString>() << "1") << (QVector<QString>()) << (QVector<QString>() << "3")
                                        << (QVector<QString>()) << (QVector<QString>() << "4") << (QVector<QString>() << "0") << (QVector<QString>()));
    QTest::newRow("Random test #3") << 6 << 32 << 4
                                    << (QVector<QVector<QString>>()
                                        << (QVector<QString>() << "0") << (QVector<QString>() << "1") << (QVector<QString>()) << (QVector<QString>()))
                                    << (QVector<QVector<QString>>()
                                        << (QVector<QString>()) << (QVector<QString>() << "1") << (QVector<QString>() << "0") << (QVector<QString>()));
    QTest::newRow("Random test #4") << 17 << 23 << 10
                                    << (QVector<QVector<QString>>()
                                        << (QVector<QString>() << "0") << (QVector<QString>() << "1") << (QVector<QString>() << "2") << (QVector<QString>())
                                        << (QVector<QString>()) << (QVector<QString>() << "3") << (QVector<QString>() << "4") << (QVector<QString>())
                                        << (QVector<QString>()) << (QVector<QString>()))
                                    << (QVector<QVector<QString>>()
                                        << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>() << "2")
                                        << (QVector<QString>() << "1") << (QVector<QString>() << "3") << (QVector<QString>()) << (QVector<QString>() << "4")
                                        << (QVector<QString>() << "0") << (QVector<QString>()));
    QTest::newRow("Random test #5") << 6 << 27 << 6
                                    << (QVector<QVector<QString>>() << (QVector<QString>() << "0") << (QVector<QString>() << "1") << (QVector<QString>() << "4")
                                                                    << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>()))
                                    << (QVector<QVector<QString>>() << (QVector<QString>()) << (QVector<QString>() << "1") << (QVector<QString>())
                                                                    << (QVector<QString>() << "4") << (QVector<QString>() << "0") << (QVector<QString>()));
    QTest::newRow("Random test #6") << 6 << 17 << 8
                                    << (QVector<QVector<QString>>()
                                        << (QVector<QString>() << "0") << (QVector<QString>() << "1") << (QVector<QString>() << "3") << (QVector<QString>())
                                        << (QVector<QString>() << "4") << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>()))
                                    << (QVector<QVector<QString>>()
                                        << (QVector<QString>()) << (QVector<QString>() << "1") << (QVector<QString>()) << (QVector<QString>() << "3")
                                        << (QVector<QString>()) << (QVector<QString>() << "4") << (QVector<QString>() << "0") << (QVector<QString>()));
    QTest::newRow("Random test #7") << 8 << 22 << 8
                                    << (QVector<QVector<QString>>()
                                        << (QVector<QString>() << "0") << (QVector<QString>() << "1") << (QVector<QString>() << "3") << (QVector<QString>())
                                        << (QVector<QString>() << "4") << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>()))
                                    << (QVector<QVector<QString>>()
                                        << (QVector<QString>()) << (QVector<QString>() << "1") << (QVector<QString>()) << (QVector<QString>() << "3")
                                        << (QVector<QString>()) << (QVector<QString>() << "4") << (QVector<QString>() << "0") << (QVector<QString>()));
    QTest::newRow("Random test #8") << 14 << 19 << 10
                                    << (QVector<QVector<QString>>()
                                        << (QVector<QString>() << "0") << (QVector<QString>() << "1") << (QVector<QString>() << "2") << (QVector<QString>())
                                        << (QVector<QString>() << "3") << (QVector<QString>()) << (QVector<QString>() << "4") << (QVector<QString>())
                                        << (QVector<QString>()) << (QVector<QString>()))
                                    << (QVector<QVector<QString>>() << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>())
                                                                    << (QVector<QString>() << "2"
                                                                                           << "1")
                                                                    << (QVector<QString>()) << (QVector<QString>() << "3") << (QVector<QString>())
                                                                    << (QVector<QString>() << "4") << (QVector<QString>() << "0") << (QVector<QString>()));
    QTest::newRow("Random test #9") << 3 << 13 << 8
                                    << (QVector<QVector<QString>>()
                                        << (QVector<QString>() << "0") << (QVector<QString>() << "1") << (QVector<QString>() << "3") << (QVector<QString>())
                                        << (QVector<QString>() << "4") << (QVector<QString>()) << (QVector<QString>()) << (QVector<QString>()))
                                    << (QVector<QVector<QString>>()
                                        << (QVector<QString>()) << (QVector<QString>() << "1") << (QVector<QString>()) << (QVector<QString>() << "3")
                                        << (QVector<QString>()) << (QVector<QString>() << "4") << (QVector<QString>() << "0") << (QVector<QString>()));
}
