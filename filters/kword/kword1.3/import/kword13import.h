//

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef KWORD_1_3_IMPORT_H
#define KWORD_1_3_IMPORT_H

#include <qstring.h>
#include <qcstring.h>

#include <koFilter.h>

class QIODevice;
class KWordDocument;

class KWord13Import : public KoFilter {

    Q_OBJECT

public:
    KWord13Import(KoFilter *parent, const char *name, const QStringList &);
    virtual ~KWord13Import() {}

    virtual KoFilter::ConversionStatus convert( const QCString& from, const QCString& to );
protected:
    bool KWord13Import::parseRoot( QIODevice* io, KWordDocument& kwordDocument );
};
#endif // KWORD_1_3_IMPORT_H
