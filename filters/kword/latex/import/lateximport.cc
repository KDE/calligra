/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include <lateximport.h>
#include <lateximport.moc>
#include <kdebug.h>
#include <KoFilterChain.h>
#include <kgenericfactory.h>
#include <kglobal.h>
#include <klocale.h>
#include <qtextcodec.h>
//Added by qt3to4:
#include <Q3CString>
#include "lateximportdia.h"

typedef KGenericFactory<LATEXImport> LATEXImportFactory;
K_EXPORT_COMPONENT_FACTORY( libkwordlateximport, LATEXImportFactory( "kofficefilters" ) )


LATEXImport::LATEXImport(KoFilter *, const char *, const QStringList&) :
                     KoFilter(parent) {
}

KoFilter::ConversionStatus LATEXImport::convert( const QByteArray& from, const QByteArray& to )
{
    QString config;

    if(from != "text/x-tex" || to != "application/x-kword")
        return KoFilter::NotImplemented;

    KoStore* out = KoStore::createStore(QString(m_chain->outputFile()),
				KoStore::Write, "application/x-kword");
    /*if(!out || !out->open("root")) {
        kError(30503) << "Unable to open output file!" << endl;
				delete out;
        return KoFilter::FileNotFound;
    }

    out->close();*/

    LATEXImportDia* dialog = new LATEXImportDia(out);
    dialog->setInputFile(m_chain->inputFile());

    dialog->exec();
    delete dialog;
    delete out;

    return KoFilter::OK;
}

