/* This file is part of the KDE project
   Copyright (C) 1999 David Faure <faure@kde.org>

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

#ifndef QPROIMPORT_H
#define QPROIMPORT_H

#include <KoFilter.h>
#include <qpro/tablenames.h>
//Added by qt3to4:
#include <Q3CString>

namespace KSpread
{
class Sheet;
}

class QpImport : public KoFilter {

    Q_OBJECT

public:
    QpImport(KoFilter* parent, const char* name, const QStringList&);
    virtual ~QpImport() {}

    virtual KoFilter::ConversionStatus convert( const QByteArray& from, const QByteArray& to );
    void InitTableName(int pIdx, QString& pResult);
};

class QpTableList : public QpTableNames
{
public:
   QpTableList();
   ~QpTableList();

   void          table(unsigned pIdx, KSpread::Sheet* pTable);
   KSpread::Sheet* table(unsigned pIdx);
protected:
  KSpread::Sheet* cTable[cNameCnt];
};

#endif // QPROIMPORT_H
