/* This file is part of the KDE libraries
    Copyright (c) 2005 Thomas Braxton <brax108@cox.net>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef KLOCALIZEDSTRINGTEST_H
#define KLOCALIZEDSTRINGTEST_H

#include <QtCore/QObject>
#include <QtCore/QTemporaryDir>

class KLocalizedStringTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void correctSubs();
    void wrongSubs();
    void removeAcceleratorMarker();
    void miscMethods();
    void translateToFrenchLowlevel();
    void translateToFrench();
    void translateQt();

    void testThreads();

private:
    bool m_hasFrench;
    QTemporaryDir m_tempDir;
    bool compileCatalogs(const QDir &dataDir);
};

#endif // KLOCALIZEDSTRINGTEST_H
