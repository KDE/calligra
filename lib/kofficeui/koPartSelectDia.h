/* This file is part of the KDE libraries
    Copyright (C) 1998 Torben Weis <weis@kde.org>

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

#ifndef DlgPartSelect_included
#define DlgPartSelect_included

#include <koScanParts.h>

#include "koPartSelectDia_data.h"

class KoBeListBoxItem : public QListBoxItem
{
public:
  KoBeListBoxItem( const char *s, const QPixmap p ) : QListBoxItem(), pm(p)
  { setText( s ); }
protected:
  virtual void paint( QPainter * );
  virtual int height( const QListBox * ) const;
  virtual int width( const QListBox * ) const;
  virtual const QPixmap *pixmap() { return &pm; }
private:
  QPixmap pm;
};

class KoPartSelectDia : public DlgPartSelectData
{
    Q_OBJECT
public:
    KoPartSelectDia( QWidget* parent = NULL, const char* name = NULL );
    virtual ~KoPartSelectDia();
  
    KoPartEntry* result();

    /**
     * Do NOT delete the object returned by this function.
     *
     * @returns 0L if no part was selected, or a pointer to the
     *          parts record otherwise.
     */
    static KoPartEntry* selectPart();
};

#endif // DlgPartSelect_included
