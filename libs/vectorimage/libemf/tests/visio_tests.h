/*
  SPDX-FileCopyrightText: 2008 Brad Hards <bradh@frogmouth.net>

  SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef VISIO_TESTS_H
#define VISIO_TESTS_H

#include <QtTest>

class VisioTests : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void test1();
    void test_kde41();
};

#endif
