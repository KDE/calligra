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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KPRESENTER_TEXTVIEW_IFACE_H
#define KPRESENTER_TEXTVIEW_IFACE_H

#include <KoDocumentIface.h>
#include <dcopref.h>
#include <KoTextViewIface.h>

class KPTextView;
class QString;

class KPTextViewIface :  virtual public KoTextViewIface
{
    K_DCOP
public:
    KPTextViewIface( KPTextView *_textview );

k_dcop:
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
    KPTextView *m_textview;

};

#endif
