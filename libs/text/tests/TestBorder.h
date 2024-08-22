/*
 *  This file is part of Calligra tests
 *
 *  SPDX-FileCopyrightText: 2006-2010 Thomas Zander <zander@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef TESTBORDER_H
#define TESTBORDER_H

#include <QObject>

class TestBorder : public QObject
{
    Q_OBJECT
public:
    TestBorder() = default;

private Q_SLOTS:
    void testBorder();
};

#endif
