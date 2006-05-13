/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
	 								2003 Robert Jacolin <rjacolin@ifrance.com>

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

#include <latexexport.h>
#include <latexexport.moc>
#include <kdebug.h>
#include <KoFilterChain.h>
#include <kgenericfactory.h>
#include <kglobal.h>
#include <klocale.h>
#include <QTextCodec>
//Added by qt3to4:
#include <Q3CString>
#include "kwordlatexexportdia.h"

typedef KGenericFactory<LATEXExport> LATEXExportFactory;
K_EXPORT_COMPONENT_FACTORY( libkwordlatexexport, LATEXExportFactory( "kofficefilters" ) )


LATEXExport::LATEXExport(QObject* parent, const QStringList&) :
                     KoFilter(parent) {
}

KoFilter::ConversionStatus LATEXExport::convert( const QByteArray& from, const QByteArray& to )
{
    QString config;

    if(to != "text/x-tex" || from != "application/x-kword")
        return KoFilter::NotImplemented;

    KoStore* in = KoStore::createStore(m_chain->inputFile(), KoStore::Read);
    if(!in || !in->open("root")) {
        kError(30503) << "Unable to open input file!" << endl;
        delete in;
        return KoFilter::FileNotFound;
    }
    /* input file Reading */
    in->close();

    KWordLatexExportDia* dialog = new KWordLatexExportDia(in);
    dialog->setOutputFile(m_chain->outputFile());

    dialog->exec();
    delete dialog;
    delete in;

    return KoFilter::OK;
}
