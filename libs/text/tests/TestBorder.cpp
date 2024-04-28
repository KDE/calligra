/*
 *  This file is part of Calligra tests
 *
 *  SPDX-FileCopyrightText: 2006-2010 Thomas Zander <zander@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "TestBorder.h"

#include "KoTextBlockBorderData.h"
#include "styles/KoParagraphStyle.h"

#include <QTest>

void TestBorder::testBorder()
{
    KoParagraphStyle style;
    style.setLeftBorderWidth(4.0);
    style.setLeftBorderSpacing(2.0);
    style.setLeftInnerBorderWidth(1.0);
    style.setTopBorderWidth(6.0);
    style.setTopBorderSpacing(7.0);
    style.setTopInnerBorderWidth(8.0);
    QTextBlockFormat format;
    style.applyStyle(format);

    KoTextBlockBorderData data(QRectF(10, 10, 100, 100));

    data.setEdge(KoTextBlockBorderData::Left,
                 format,
                 KoParagraphStyle::LeftBorderStyle,
                 KoParagraphStyle::LeftBorderWidth,
                 KoParagraphStyle::LeftBorderColor,
                 KoParagraphStyle::LeftBorderSpacing,
                 KoParagraphStyle::LeftInnerBorderWidth);
    data.setEdge(KoTextBlockBorderData::Top,
                 format,
                 KoParagraphStyle::TopBorderStyle,
                 KoParagraphStyle::TopBorderWidth,
                 KoParagraphStyle::TopBorderColor,
                 KoParagraphStyle::TopBorderSpacing,
                 KoParagraphStyle::TopInnerBorderWidth);

    // QCOMPARE(QRectF(10, 10, 100, 80), data.rect());
    QCOMPARE(7., data.inset(KoTextBlockBorderData::Left));
    QCOMPARE(0., data.inset(KoTextBlockBorderData::Right));
    QCOMPARE(21., data.inset(KoTextBlockBorderData::Top));
    QCOMPARE(0., data.inset(KoTextBlockBorderData::Bottom));
}

QTEST_MAIN(TestBorder)
