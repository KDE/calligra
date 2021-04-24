/*
 *  SPDX-FileCopyrightText: 2011 Srikanth Tiyyagura <srikanth.tulasiram@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KORESOURCETAGGING_TEST_H
#define KORESOURCETAGGING_TEST_H

#include <QObject>
#include <KoConfig.h>
#include "KoResourceTagStore.h"

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
