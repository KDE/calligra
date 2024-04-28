/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2021 Pierre Ducroquet <pinaraf@pinaraf.info>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TESTTEXTSTATISTICS_H
#define TESTTEXTSTATISTICS_H

#include <QObject>

class TestTextStatistics : public QObject
{
    Q_OBJECT
private Q_SLOTS: // tests
    void testTextStatistics();
    void testListenBehaviour();
};

#endif
