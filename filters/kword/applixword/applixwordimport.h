/* This file is part of the KDE project
   Copyright (C) 2000 Enno Bartels <ebartels@nwn.de>

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

#ifndef APPLIXWORDIMPORT_H
#define APPLIXWORDIMPORT_H

#include <qstring.h>
#include <qfile.h>
#include <qobject.h>
#include <qtextstream.h>

#include <koFilter.h>
#include <koStore.h>

typedef struct 
{
  int c;
  int m;
  int y;
  int k;

  int r;
  int g;
  int b;
}t_mycolor;

class APPLIXWORDImport : public KoFilter {

    Q_OBJECT

public:
    APPLIXWORDImport(KoFilter *parent, const char *name);
    virtual ~APPLIXWORDImport() {}
    /** filtering :) */
    virtual const bool filter(const QString &fileIn, const QString &fileOut,
                              const QString &from, const QString &to,
                              const QString &config=QString::null);
    QChar   specCharfind     (QChar , QChar );
    QString readTagLine      (QTextStream &, QFile &);
    void    replaceSpecial   (QString &);

};
#endif // APPLIXWORDIMPORT_H

