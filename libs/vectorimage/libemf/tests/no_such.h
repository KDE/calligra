/*
  SPDX-FileCopyrightText: 2008 Brad Hards <bradh@frogmouth.net>

  SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef NO_SUCH_H
#define NO_SUCH_H

#include <QtTest>

class NoSuch : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void checkNonExisting();
};

#endif
