/* This file is part of the KDE project
   Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXICOMMANDLINKBUTTON_H
#define KEXICOMMANDLINKBUTTON_H

#include "kexiutils_export.h"

#include <QtGui/QCommandLinkButton>

//! @short Command link button with arrow
/*! Used in assistants to indicate that clicking the button advances
 to next page. */
class KEXIUTILS_EXPORT KexiCommandLinkButton : public QCommandLinkButton
{
    Q_OBJECT
public:
    explicit KexiCommandLinkButton( QWidget * parent = 0 );
    explicit KexiCommandLinkButton( const QString & text, QWidget * parent = 0 );
    KexiCommandLinkButton( const QString & text, const QString & description, QWidget * parent = 0 );
    ~KexiCommandLinkButton();
    
protected:
    virtual void paintEvent(QPaintEvent* event);

private:
    class Private;
    Private * const d;
};

#endif
