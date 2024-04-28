/*
  SPDX-FileCopyrightText: 2008 Brad Hards <bradh@frogmouth.net>

  SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef BAD_HEADER_H
#define BAD_HEADER_H

#include <QtTest>

class BadHeader : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void checkWmfHeader();
};

#endif
