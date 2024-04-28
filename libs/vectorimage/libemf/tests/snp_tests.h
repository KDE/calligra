/*
  SPDX-FileCopyrightText: 2008 Brad Hards <bradh@frogmouth.net>

  SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef SNP_TESTS_H
#define SNP_TESTS_H

#include <QtTest>

class SnpTests : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void test1();
    void test2();
    void test3();
};

#endif
