/* This file is part of the KDE project
   Copyright (C) 2002 Ariya Hidayat <ariyahidayat@yahoo.de>

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

#ifndef __PALMDOC_H
#define __PALMDOC_H

#include "palmdb.h"
#include <qstring.h>

class PalmDoc: public PalmDB
{
  public:

    PalmDoc();
    virtual ~PalmDoc();

    virtual bool load( const char* filename );
    virtual bool save( const char* filename );

    int result(){ return m_result; }

    QString text(){ return m_text; }
    void setText( QString t){ m_text = t; }

    enum { OK, ReadError, InvalidFormat, WriteError };

  private:

    int m_result;
    QString m_text;

    QByteArray compress( QString text );
    QString uncompress( QByteArray rec );
};

#endif
