/* This file is part of the KOffice project
 * Copyright (C) 2005-2010 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KWPAGECOMMANDS_H
#define KWPAGECOMMANDS_H

#include <QObject>
#include <QtTest/QtTest>
#include <qtest_kde.h>
class TestPageCommands : public QObject
{
    Q_OBJECT
private slots: // tests
    void init();
    void documentPages();
    void testInsertPageCommand();
    void testInsertPageCommand2();
    void testInsertPageCommand3();
    void testRemovePageCommand();
    void testRemovePageCommand2();
    void testRemovePageCommand3();
    void testRemovePageCommand4();
    void testPageStylePropertiesCommand();
    void testNewPageStyleCommand();
};

#endif
