/*
 *  This file is part of Calligra tests
 *
 *  SPDX-FileCopyrightText: 2018 Dag Andersen <danders@get2net.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef TestSelection_H
#define TestSelection_H

#include <QObject>

class TestAllowedInteraction : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testVisible();
    void testSelectable();
    void testDeletable();
    void testGeometryProtected();
    void testContentProtected();
    void testEditable();

    void testShape();
    void testParentAllowedInteractions();
    void testParentAllowedInteraction();
    void testGrandParentAllowedInteractions();
    void testGrandParentAllowedInteraction();
};

#endif
