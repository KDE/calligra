/*
   This file is part of the KDE project
   Copyright (C) 2004 Nicolas GOUTTE <goutte@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KWORD_1_3_IMPORT_H
#define KWORD_1_3_IMPORT_H

#include <qstring.h>
#include <qcstring.h>

#include <KoFilter.h>

class QIODevice;
class KoStore;
class KWord13Document;

class KWord13Import : public KoFilter {

    Q_OBJECT

public:
    KWord13Import(KoFilter *parent, const char *name, const QStringList &);
    virtual ~KWord13Import() {}

    virtual KoFilter::ConversionStatus convert( const QCString& from, const QCString& to );
protected:
    bool parseInfo( QIODevice* io, KWord13Document& kwordDocument );
    bool parseRoot( QIODevice* io, KWord13Document& kwordDocument );
    bool postParse( KoStore* store, KWord13Document& doc );    
};
#endif // KWORD_1_3_IMPORT_H
