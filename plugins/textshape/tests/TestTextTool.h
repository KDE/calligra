/*
 *  This file is part of Calligra tests
 *
 *  SPDX-FileCopyrightText: 2006-2010 Thomas Zander <zander@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef TESTTEXTTOOL_H
#define TESTTEXTTOOL_H

#include <QObject>
#include <QString>
#include <qtest_kde.h>

class TestTextTool : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testCaretRect();
};

#endif
