// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2002, Laurent MONTEL <lmontel@mandrakesoft.com>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPRESENTER_TEXTVIEW_IFACE_H
#define KPRESENTER_TEXTVIEW_IFACE_H

#include <KoDocumentAdaptor.h>
#include <KoTextViewAdaptor.h>

class KPrTextView;
class QString;

class KPrTextViewAdaptor : public KoTextViewAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.koffice.presentation.text.view")

public:
    KPrTextViewAdaptor( KPrTextView *_textview );

public Q_SLOTS:
    void insertVariable( int type, int subtype = 0 );
    void insertCustomVariable( const QString &name);
    void insertLink(const QString &_linkName, const QString & hrefName);
    void insertVariable( const QString & var);
    void insertNote(const QString &_note);

    void clearSelection();
    void selectAll();

    void cut();
    void copy();
    void paste();

private:
    KPrTextView *m_textview;

};

#endif
