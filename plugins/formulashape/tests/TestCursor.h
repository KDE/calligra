// This file is part of the KDE project
// SPDX-FileCopyrightText: 2009 Jeremias Epperlein <jeeree@web.de>
// SPDX-License-Identifier: LGPL-2.0-only

#ifndef _TESTCURSOR_H_
#define _TESTCURSOR_H_

#include <QObject>

class TestCursor : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    // General Layout Elements
    void moveCursor();
};

#endif // _TESTCURSOR_H_
