/*
   This file is part of the KDE project
   Copyright (C) 2003 Tobias Koenig <tokoe@kde.org>

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

#ifndef _KWMAILMERGE_KABC_H_
#define _KWMAILMERGE_KABC_H_

#include <qdom.h>
#include <qguardedptr.h>

#include <kabc/addressbook.h>

#include "mailmerge_interface.h"

class KWMailMergeKABC: public KWMailMergeDataSource
{
  Q_OBJECT

  public:
    KWMailMergeKABC( KInstance *inst, QObject *parent );
    ~KWMailMergeKABC();

    virtual void save( QDomDocument&, QDomElement& ) {};
    virtual void load( QDomElement& ) {};
    virtual class QString getValue( const class QString &name, int record = -1 ) const;
    virtual int getNumRecords() const;
    virtual void refresh( bool force );

    virtual bool showConfigDialog( QWidget*, int ) { return true; }

  private:
    KABC::AddressBook *mAddressBook;
    mutable KABC::AddressBook::ConstIterator mIterator;
};

#endif
