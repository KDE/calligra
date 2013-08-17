/* This file is part of the KDE libraries
    Copyright (C) 2001 Holger Freyther (freyher@yahoo.com)
                  based on ideas from Martijn and Simon
                  many thanks to Simon

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

#include "kguiitem.h"

#include <QPushButton>

class KGuiItem::KGuiItemPrivate
{
public:
    KGuiItemPrivate()
    {
        m_enabled = true;
        m_hasIcon = false;
    }

    KGuiItemPrivate( const KGuiItemPrivate &rhs )
    {
        ( *this ) = rhs;
    }

    KGuiItemPrivate &operator=( const KGuiItemPrivate &rhs )
    {
        m_text = rhs.m_text;
        m_icon = rhs.m_icon;
        m_iconName = rhs.m_iconName;
        m_toolTip = rhs.m_toolTip;
        m_whatsThis = rhs.m_whatsThis;
        m_statusText = rhs.m_statusText;
        m_enabled = rhs.m_enabled;
        m_hasIcon = rhs.m_hasIcon;

        return *this;
    }

    QString m_text;
    QString m_toolTip;
    QString m_whatsThis;
    QString m_statusText;
    QString m_iconName;
    QIcon m_icon;
    bool m_hasIcon : 1;
    bool m_enabled : 1;
};


KGuiItem::KGuiItem() {
    d = new KGuiItemPrivate;
}

KGuiItem::KGuiItem( const QString &text,    const QString &iconName,
                    const QString &toolTip, const QString &whatsThis )
{
    d = new KGuiItemPrivate;
    d->m_text = text;
    d->m_toolTip = toolTip;
    d->m_whatsThis = whatsThis;
    setIconName( iconName );
}

KGuiItem::KGuiItem( const QString &text,    const QIcon &icon,
                    const QString &toolTip, const QString &whatsThis )
{
    d = new KGuiItemPrivate;
    d->m_text = text;
    d->m_toolTip = toolTip;
    d->m_whatsThis = whatsThis;
    setIcon( icon );
}

KGuiItem::KGuiItem( const KGuiItem &rhs )
    : d( 0 )
{
    ( *this ) = rhs;
}

KGuiItem &KGuiItem::operator=( const KGuiItem &rhs )
{
    if ( d == rhs.d )
        return *this;

    Q_ASSERT( rhs.d );

    delete d;
    d = new KGuiItemPrivate( *rhs.d );

    return *this;
}

KGuiItem::~KGuiItem()
{
    delete d;
}

QString KGuiItem::text() const
{
    return d->m_text;
}


QString KGuiItem::plainText() const
{
    const int len = d->m_text.length();

    if (len == 0)
        return d->m_text;

    //Can assume len >= 1 from now on.
    QString stripped;

    int resultLength = 0;
    stripped.resize(len);

    const QChar* data    = d->m_text.unicode();
    for ( int pos = 0; pos < len; ++pos )
    {
        if ( data[ pos ] != QLatin1Char('&') )
            stripped[ resultLength++ ] = data[ pos ];
        else if ( pos + 1 < len && data[ pos + 1 ] == QLatin1Char('&') )
            stripped[ resultLength++ ] = data[ pos++ ];
    }

    stripped.truncate(resultLength);

    return stripped;
}


QIcon KGuiItem::icon( ) const
{
    if (d->m_hasIcon) {
        if (!d->m_iconName.isEmpty()) {
            return QIcon::fromTheme(d->m_iconName);
        } else {
            return d->m_icon;
        }
    }
    return QIcon();
}

QString KGuiItem::iconName() const
{
    return d->m_iconName;
}

QString KGuiItem::toolTip() const
{
    return d->m_toolTip;
}

QString KGuiItem::whatsThis() const
{
    return d->m_whatsThis;
}

bool KGuiItem::isEnabled() const
{
    return d->m_enabled;
}

bool KGuiItem::hasIcon() const
{
    return d->m_hasIcon;
}

void KGuiItem::setText( const QString &text )
{
    d->m_text = text;
}

void KGuiItem::setIcon( const QIcon &icon )
{
    d->m_icon = icon;
    d->m_iconName.clear();
    d->m_hasIcon = !icon.isNull();
}

void KGuiItem::setIconName( const QString &iconName )
{
    d->m_iconName = iconName;
    d->m_icon = QIcon();
    d->m_hasIcon = !iconName.isEmpty();
}

void KGuiItem::setToolTip( const QString &toolTip )
{
    d->m_toolTip = toolTip;
}

void KGuiItem::setWhatsThis( const QString &whatsThis )
{
    d->m_whatsThis = whatsThis;
}

void KGuiItem::setEnabled( bool enabled )
{
    d->m_enabled = enabled;
}

void KGuiItem::assign(QPushButton *button, const KGuiItem &item)
{
    button->setText(item.d->m_text);
    button->setIcon(item.icon());
    button->setToolTip(item.d->m_toolTip);
    button->setWhatsThis(item.d->m_whatsThis);
}

// vim: set et sw=4:

