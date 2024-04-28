/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Boudewijn Rempt (boud@valdyas.org)
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef TestKoShapeRegistry_H
#define TestKoShapeRegistry_H

#include <QObject>

class TestKoShapeRegistry : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    // tests
    void testGetKoShapeRegistryInstance();
    void testCreateShapes();
    void testCreateFramedShapes();
};

#endif
