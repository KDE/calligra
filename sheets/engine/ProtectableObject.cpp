/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ProtectableObject.h"

#include <QCryptographicHash>

using namespace Calligra::Sheets;

ProtectableObject::ProtectableObject() = default;

ProtectableObject::ProtectableObject(const ProtectableObject &other)
{
    m_password = other.m_password;
}

QByteArray ProtectableObject::passwordHash() const
{
    return m_password;
}

bool ProtectableObject::isProtected() const
{
    return !m_password.isNull();
}

void ProtectableObject::setProtected(QByteArray const &password)
{
    m_password = password;
}

void ProtectableObject::setProtected(const QString &password)
{
    if (password.isEmpty())
        m_password = QByteArray();
    else
        m_password = passwordHash(password);
}

bool ProtectableObject::checkPassword(const QString &password) const
{
    return (passwordHash(password) == m_password);
}

QByteArray ProtectableObject::passwordHash(const QString &password) const
{
    return QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha1);
}
