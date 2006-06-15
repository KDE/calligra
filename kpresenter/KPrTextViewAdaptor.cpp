// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 2002 Laurent MONTEL <lmontel@mandrakesoft.com>

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

#include "KPrTextViewAdaptor.h"

#include "KPrTextObject.h"
#include <kapplication.h>

KPrTextViewAdaptor::KPrTextViewAdaptor( KPrTextView *_textview )
    :KoTextViewAdaptor(_textview)
{
    m_textview = _textview;
}

void KPrTextViewAdaptor::insertVariable( int type, int subtype )
{
    m_textview->insertVariable(type,subtype);
}

void KPrTextViewAdaptor::insertCustomVariable( const QString &name)
{
    m_textview->insertCustomVariable(name);
}

void KPrTextViewAdaptor::insertLink(const QString &_linkName, const QString & hrefName)
{
    m_textview->insertLink(_linkName,hrefName);
}

void KPrTextViewAdaptor::clearSelection()
{
    m_textview->clearSelection();
}

void KPrTextViewAdaptor::selectAll()
{
    m_textview->selectAll();
}

void KPrTextViewAdaptor::cut()
{
    m_textview->cut();
}

void KPrTextViewAdaptor::copy()
{
    m_textview->copy();
}

void KPrTextViewAdaptor::paste()
{
    m_textview->paste();
}

void KPrTextViewAdaptor::insertVariable( const QString & var)
{
    if( var=="DATE")
        m_textview->insertVariable(0,1);
    else if (var=="FIXED-DATE")
        m_textview->insertVariable(0,0);
    else if (var=="TIME")
        m_textview->insertVariable(2,1);
    else if (var=="FIXED-TIME")
        m_textview->insertVariable(2,0);
    else if (var=="FILENAME")
        m_textview->insertVariable(8,0);
    else if (var=="AUTHORNAME")
        m_textview->insertVariable(8,2);
    else if (var=="EMAIL")
        m_textview->insertVariable(8,3);
    else if (var=="TITLE")
        m_textview->insertVariable(8,10);
    else if (var=="ABSTRACT")
        m_textview->insertVariable(8,11);
    else if (var=="DIRECTORYNAME")
        m_textview->insertVariable(8,1);
    else if (var=="COMPANYNAME")
        m_textview->insertVariable(8,4);
    else if (var=="PATHFILENAME")
        m_textview->insertVariable(8,5);
    else if (var=="FILENAMEWITHOUTEXTENSION")
        m_textview->insertVariable(8,6);
    else if (var=="CURRENTPAGE")
        m_textview->insertVariable(4,0);
    else if (var=="TOTALPAGE")
        m_textview->insertVariable(4,1);
    else if (var=="TELEPHONE")
        m_textview->insertVariable(8,7);
    else if (var=="FAX")
        m_textview->insertVariable(8,8);
    else if (var=="COUNTRY")
        m_textview->insertVariable(8,9);
    else if (var=="POSTAL_CODE")
        m_textview->insertVariable(8,12);
    else if (var=="CITY")
        m_textview->insertVariable(8,13);
    else if (var=="STREET")
        m_textview->insertVariable(8,14);
}

void KPrTextViewAdaptor::insertNote(const QString &_note)
{
    m_textview->insertComment(_note);
}
