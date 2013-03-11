/* This file is part of the KDE project
   Copyright (C) 2012-2013 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXITESTER_H
#define KEXITESTER_H

#include <QPointer>
#include <kexiutils/kexiutils_export.h>

class QWidget;
class KexiTestObject;

//! Global object for setting up tests.
class KEXIUTILS_EXPORT KexiTester : public QObject
{
    Q_OBJECT
public:
    ~KexiTester();

    QObject *object(const QString &name) const;

    QWidget *widget(const QString &name) const;

    template <class W>
    W widget(const QString &name) const { return qobject_cast<W>(widget(name)); }

private:
    KexiTester();
    static KexiTester* self();

    Q_DISABLE_COPY(KexiTester)
    friend KexiTester& operator<<(KexiTester&, const KexiTestObject &);
    friend KexiTester& kexiTester();

    class Private;
    Private * const d;
};

//! KexiTestObject is a container for object added for tests
class KEXIUTILS_EXPORT KexiTestObject
{
public:
    explicit KexiTestObject(QObject *object, const QString &name = QString());
private:
    QPointer<QObject> m_object;
    QString m_name;

    friend KexiTester& operator<<(KexiTester&, const KexiTestObject &);
};

//! @return global KexiTester object.
inline KexiTester& kexiTester() { return *KexiTester::self(); }

//! Adds test object @a object to the tester.
KEXIUTILS_EXPORT KexiTester& operator<<(KexiTester& tester, const KexiTestObject &object);

#ifndef COMPILING_TESTS
#define kexiTester while (false) kexiTester // Completely skip tester for final code
#endif

#endif // KEXITESTER_H
