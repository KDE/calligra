// This file is part of the KDE project
// SPDX-FileCopyrightText: 2007 Alfredo Beaumont Sainz <alfredo.beaumont@gmail.com>
// SPDX-License-Identifier: LGPL-2.0-only

#ifndef _TESTLAYOUT_H_
#define _TESTLAYOUT_H_

#include <QObject>

class TestLayout : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    // Token Elements
    void identifierElement_data();
    void identifierElement();

    // General Layout Elements
    void fencedElement_data();
    void fencedElement();
};

#endif // _TESTLAYOUT_H_
