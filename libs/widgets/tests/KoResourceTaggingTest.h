/*
 *  SPDX-FileCopyrightText: 2011 Srikanth Tiyyagura <srikanth.tulasiram@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KORESOURCETAGGING_TEST_H
#define KORESOURCETAGGING_TEST_H

#include "KoResourceTagStore.h"
#include <KoConfig.h>
#include <QObject>

class KoResourceTaggingTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase();

    // tests
    void testInitialization();
    void testTagging();
    void testReadWriteXML();
};

#endif
