/* This file is part of the KDE project
   Copyright (C) 2001 Enno Bartels <ebartels@nwn.de>

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

#ifndef APPLIXSPREADIMPORT_H
#define APPLIXSPREADIMPORT_H

#include <QString>
#include <QFile>
#include <QObject>
#include <QTextStream>
#include <q3ptrlist.h>
//Added by qt3to4:
#include <Q3CString>

#include <KoFilter.h>
#include <KoStore.h>

typedef struct
{
  int r;
  int g;
  int b;

  int c;
  int m;
  int y;
  int k;
} t_mycolor;

typedef struct
{
  QStringList tabname;
  QStringList rc;
} t_rc;


class APPLIXSPREADImport : public KoFilter {

    Q_OBJECT

public:
    APPLIXSPREADImport ( QObject *parent, const QStringList& );
    virtual ~APPLIXSPREADImport() {}

    virtual KoFilter::ConversionStatus convert( const QByteArray& from, const QByteArray& to );

protected:
    QString nextLine           (QTextStream &);
    QChar   specCharfind       (QChar , QChar );
    void    writePen           (QString &, int, int, QString);
    QString writeColor         (t_mycolor *);
    void    readTypefaceTable  (QTextStream &, QStringList &);
    void    readColormap       (QTextStream &, Q3PtrList<t_mycolor> &);
    void    readView           (QTextStream &, QString, t_rc &);
    void    filterSHFGBG       (QString, int *, int *, int *);
    void    transPenFormat     (QString, int *, int *);
    int     readHeader         (QTextStream &);
    int     translateColumnNumber (QString);

private:
    int m_stepsize;
    int m_instep;
    int m_progress;
};
#endif // APPLIXSPREADIMPORT_H
