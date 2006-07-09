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

#include <xsltimport.h>
#include <xsltimport.moc>
#include <kdebug.h>
#include <KoFilterChain.h>
#include <kgenericfactory.h>
#include <kglobal.h>
#include <klocale.h>
#include <qtextcodec.h>
#include "xsltimportdia.h"


typedef KGenericFactory<XSLTImport, KoFilter> XSLTImportFactory;
K_EXPORT_COMPONENT_FACTORY( libxsltimport, XSLTImportFactory( "kofficefilters" ) )


XSLTImport::XSLTImport(KoFilter *, const char *, const QStringList&) :
                     KoFilter() {
}

KoFilter::ConversionStatus XSLTImport::convert( const QCString&, const QCString& to )
{
    QString config;

    if(to != "application/x-kword" &&
		to != "application/x-kontour" && to != "application/x-kspread" &&
		to != "application/x-kivio" && to != "application/x-kchart" &&
		to != "application/x-kpresenter")
        return KoFilter::NotImplemented;

    // ## missing appIdentification
    KoStore* out = KoStore::createStore(QString(m_chain->outputFile()), KoStore::Write);
    if(!out || !out->open("root"))
    {
        kdError() << "Unable to create output file!" << endl;
        delete out;
        return KoFilter::FileNotFound;
    }
    /* input file Reading */
    out->close();
    kdDebug() << "here" << endl;
    XSLTImportDia* dialog = new XSLTImportDia(out, to, 0, "Importation", true);
    dialog->setInputFile(m_chain->inputFile());

    dialog->exec();
    delete dialog;
    delete out;

    return KoFilter::OK;
}
