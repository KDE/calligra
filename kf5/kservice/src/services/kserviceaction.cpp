/* This file is part of the KDE project
   Copyright 2007 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#include "kserviceaction.h"
#include <QVariant>
#include <QDataStream>

class KServiceActionPrivate : public QSharedData
{
public:
    KServiceActionPrivate(const QString& name, const QString& text,
                          const QString& icon, const QString& exec,
                          bool noDisplay)
        : m_name(name), m_text(text), m_icon(icon), m_exec(exec), m_noDisplay(noDisplay) {}
    QString m_name;
    QString m_text;
    QString m_icon;
    QString m_exec;
    QVariant m_data;
    bool m_noDisplay;
    // warning keep QDataStream operators in sync if adding data here
};

KServiceAction::KServiceAction()
    : d(new KServiceActionPrivate(QString(), QString(), QString(), QString(), false))
{
}

KServiceAction::KServiceAction(const QString& name, const QString& text,
                               const QString& icon, const QString& exec,
                               bool noDisplay)
    : d(new KServiceActionPrivate(name, text, icon, exec, noDisplay))
{
}

KServiceAction::~KServiceAction()
{
}

KServiceAction::KServiceAction(const KServiceAction& other)
    : d(other.d)
{
}

KServiceAction& KServiceAction::operator=(const KServiceAction& other)
{
    d = other.d;
    return *this;
}

QVariant KServiceAction::data() const
{
     return d->m_data;
}

void KServiceAction::setData( const QVariant& data )
{
     d->m_data = data;
}

QString KServiceAction::name() const
{
    return d->m_name;
}

QString KServiceAction::text() const
{
    return d->m_text;
}

QString KServiceAction::icon() const
{
    return d->m_icon;
}

QString KServiceAction::exec() const
{
    return d->m_exec;
}

bool KServiceAction::noDisplay() const
{
    return d->m_noDisplay;
}

bool KServiceAction::isSeparator() const
{
    return d->m_name == QLatin1String("_SEPARATOR_");
}

QDataStream& operator>>( QDataStream& str, KServiceAction& act )
{
    KServiceActionPrivate* d = act.d;
    str >> d->m_name;
    str >> d->m_text;
    str >> d->m_icon;
    str >> d->m_exec;
    str >> d->m_data;
    str >> d->m_noDisplay;
    return str;
}

QDataStream& operator<<( QDataStream& str, const KServiceAction& act )
{
    const KServiceActionPrivate* d = act.d;
    str << d->m_name;
    str << d->m_text;
    str << d->m_icon;
    str << d->m_exec;
    str << d->m_data;
    str << d->m_noDisplay;
    return str;
}
