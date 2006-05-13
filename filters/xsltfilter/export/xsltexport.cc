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

#include <xsltexport.h>

#include <kdebug.h>
#include <KoFilterChain.h>
#include <kgenericfactory.h>

#include <xsltexportdia.h>
//Added by qt3to4:
#include <QByteArray>

typedef KGenericFactory<XSLTExport> XSLTExportFactory;
K_EXPORT_COMPONENT_FACTORY( libxsltexport, XSLTExportFactory( "kofficefilters" ) )

// Check for XSLT files
extern "C" {
    int check_libxsltexport() {
        return 0;
    }
}


XSLTExport::XSLTExport(QObject* parent, const QStringList&) :
                     KoFilter(parent) {
}

KoFilter::ConversionStatus XSLTExport::convert( const QByteArray& from, const QByteArray&)
{
    if(from != "application/x-kword" &&
       from != "application/x-kontour" && from != "application/x-kspread" &&
       from != "application/x-kivio" && from != "application/x-kchart" &&
       from != "application/x-kpresenter")
        return KoFilter::NotImplemented;
    kDebug() << "In the xslt filter" << endl;

    KoStoreDevice* in = m_chain->storageFile("root", KoStore::Read);

    if(!in) {
        kError() << "Unable to open input file!" << endl;
        return KoFilter::FileNotFound;
    }

    XSLTExportDia* dialog = new XSLTExportDia(in, from, 0, "Exportation", true);
    dialog->setOutputFile(m_chain->outputFile());
    dialog->exec();
    delete dialog;
    return KoFilter::OK;
}

#include <xsltexport.moc>
