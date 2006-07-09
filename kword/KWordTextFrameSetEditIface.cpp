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

#include "KWordTextFrameSetEditIface.h"

#include "KWTextFrameSet.h"
#include <dcopclient.h>
#include "defs.h"

KWordTextFrameSetEditIface::KWordTextFrameSetEditIface( KWTextFrameSetEdit *_framesetedit )
    :KoTextViewIface(_framesetedit)
{
   m_framesetedit = _framesetedit;
}

void KWordTextFrameSetEditIface::insertExpression(const QString &_c)
{
    m_framesetedit->insertExpression(_c);
}

void KWordTextFrameSetEditIface::insertFrameBreak()
{
    m_framesetedit->insertFrameBreak();
}

void KWordTextFrameSetEditIface::insertVariable( int type, int subtype )
{
    m_framesetedit->insertVariable(type,subtype);
}

void KWordTextFrameSetEditIface::insertCustomVariable( const QString &name)
{
    m_framesetedit->insertCustomVariable(name);
}

void KWordTextFrameSetEditIface::insertLink(const QString &_linkName, const QString & hrefName)
{
    m_framesetedit->insertLink(_linkName,hrefName);
}

void KWordTextFrameSetEditIface::insertTOC()
{
    m_framesetedit->insertTOC();
}

void KWordTextFrameSetEditIface::cut()
{
    m_framesetedit->cut();
}

void KWordTextFrameSetEditIface::copy()
{
    m_framesetedit->copy();
}

void KWordTextFrameSetEditIface::paste()
{
    m_framesetedit->paste();
}

void KWordTextFrameSetEditIface::insertNonbreakingSpace()
{
    m_framesetedit->insertNonbreakingSpace();
}

void KWordTextFrameSetEditIface::insertVariable( const QString & var)
{
  if( var=="DATE")
    m_framesetedit->insertVariable(0,1);
  else if (var=="FIXED-DATE")
    m_framesetedit->insertVariable(0,0);
  else if (var=="TIME")
    m_framesetedit->insertVariable(2,1);
  else if (var=="FIXED-TIME")
    m_framesetedit->insertVariable(2,0);
  else if (var=="FILENAME")
    m_framesetedit->insertVariable(8,0);
  else if (var=="AUTHORNAME")
    m_framesetedit->insertVariable(8,2);
  else if (var=="EMAIL")
    m_framesetedit->insertVariable(8,3);
  else if (var=="TITLE")
    m_framesetedit->insertVariable(8,10);
  else if (var=="ABSTRACT")
    m_framesetedit->insertVariable(8,11);
  else if (var=="DIRECTORYNAME")
    m_framesetedit->insertVariable(8,1);
  else if (var=="COMPANYNAME")
    m_framesetedit->insertVariable(8,4);
  else if (var=="PATHFILENAME")
    m_framesetedit->insertVariable(8,5);
  else if (var=="FILENAMEWITHOUTEXTENSION")
    m_framesetedit->insertVariable(8,6);
  else if (var=="CURRENTPAGE")
    m_framesetedit->insertVariable(4,0);
  else if (var=="TOTALPAGE")
    m_framesetedit->insertVariable(8,1);
  else if (var=="TELEPHONE")
    m_framesetedit->insertVariable(8,7);
  else if (var=="FAX")
    m_framesetedit->insertVariable(8,8);
  else if (var=="COUNTRY")
    m_framesetedit->insertVariable(8,9);
  else if (var=="POSTAL_CODE")
    m_framesetedit->insertVariable(8,12);
  else if (var=="CITY")
    m_framesetedit->insertVariable(8,13);
  else if (var=="STREET")
    m_framesetedit->insertVariable(8,14);
}

void KWordTextFrameSetEditIface::insertComment(const QString &_comment)
{
    m_framesetedit->insertComment(_comment);
}


void KWordTextFrameSetEditIface::insertAutoFootNote( const QString & type )
{
    if ( type.lower() =="footnote")
    {
        m_framesetedit->insertFootNote( FootNote, KWFootNoteVariable::Auto, QString::null);
    }
    else if ( type.lower()=="endnote")
    {
        m_framesetedit->insertFootNote( EndNote, KWFootNoteVariable::Auto, QString::null);
    }
}

void KWordTextFrameSetEditIface::insertManualFootNote( const QString & type, const QString & noteText )
{
    if ( type.lower() =="footnote")
    {
        m_framesetedit->insertFootNote( FootNote, KWFootNoteVariable::Manual, noteText);
    }
    else if ( type.lower()=="endnote")
    {
        m_framesetedit->insertFootNote( EndNote, KWFootNoteVariable::Manual, noteText);
    }
}

void KWordTextFrameSetEditIface::selectAll()
{
    m_framesetedit->selectAll();
}
