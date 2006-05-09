/* This file is part of the KDE project
   Copyright (C) 2002 Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

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

#include <qstring.h>
#include <qtextstream.h>
#include <qfile.h>
//Added by qt3to4:
#include <Q3CString>

#include <kapplication.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <kmessagebox.h>

#include <KoFilterChain.h>
#include <KoStoreDevice.h>

#include <kformuladocument.h>
#include <kformulacontainer.h>
#include <kformulamimesource.h>
#include <kglobal.h>

#include "latexexport.h"


typedef KGenericFactory<LATEXExport> LATEXExportFactory;
K_EXPORT_COMPONENT_FACTORY( libkfolatexexport, LATEXExportFactory( "kofficefilters" ) )


LATEXExport::LATEXExport( QObject* parent, const QStringList& )
    : KoFilter(parent)
{
}


KoFilter::ConversionStatus LATEXExport::convert( const QByteArray& from, const QByteArray& to )
{
    if ( to != "text/x-tex" || from != "application/x-kformula" )
        return KoFilter::NotImplemented;

    KoStoreDevice* in = m_chain->storageFile( "root", KoStore::Read );
    if(!in) {
        QApplication::restoreOverrideCursor();
        KMessageBox::error( 0, i18n( "Failed to read data." ), i18n( "LaTeX Export Error" ) );
        return KoFilter::StorageCreationError;
    }

    QDomDocument dom( "KFORMULA" );
    if ( !dom.setContent( in, false ) ) {
        QApplication::restoreOverrideCursor();
        KMessageBox::error( 0, i18n( "Malformed XML data." ), i18n( "LaTeX Export Error" ) );
        return KoFilter::WrongFormat;
    }

    QFile f( m_chain->outputFile() );
    if( !f.open( QIODevice::Truncate | QIODevice::ReadWrite ) ) {
        QApplication::restoreOverrideCursor();
        KMessageBox::error( 0, i18n( "Failed to write file." ), i18n( "LaTeX Export Error" ) );
        return KoFilter::FileNotFound;
    }

    KFormula::DocumentWrapper* wrapper = new KFormula::DocumentWrapper( KGlobal::config(), 0 );
    KFormula::Document* doc = new KFormula::Document;
    wrapper->document( doc );
    KFormula::Container* formula = doc->createFormula();
    if ( !doc->loadXML( dom ) ) {
        kError(30522) << "Failed." << endl;
    }

    QTextStream stream(&f);
    //stream.setEncoding(QTextStream::UnicodeUTF8);
    stream << "\\documentclass{article}\n\\usepackage{amsmath}\n\\begin{document}\n\\[\n"
           << formula->texString()
           << "\n\\]\n\\end{document}";
    f.close();

    delete formula;
    delete wrapper;

    return KoFilter::OK;
}

#include "latexexport.moc"
