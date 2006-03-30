/* This file is part of the KDE project
   Copyright (C) 2003 Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

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

#include <kapplication.h>
#include <kdebug.h>
#include <kgenericfactory.h>
#include <kglobal.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <KoFilterChain.h>

#include <qtextcodec.h>
//Added by qt3to4:
#include <Q3CString>

#include <kformuladocument.h>
#include <kformulacontainer.h>
#include <kformulamimesource.h>

#include "mathmlimport.h"
#include "mathmlimport.moc"


typedef KGenericFactory<MathMLImport, KoFilter> MathMLImportFactory;
K_EXPORT_COMPONENT_FACTORY( libkfomathmlimport, MathMLImportFactory( "kofficefilters" ) )


MathMLImport::MathMLImport(KoFilter *, const char *, const QStringList&)
    : KoFilter()
{
}

KoFilter::ConversionStatus MathMLImport::convert( const QByteArray& from, const QByteArray& to )
{
    kDebug( KFormula::DEBUGID ) << "From: " << from << endl;
    kDebug( KFormula::DEBUGID ) << "To:   " << to << endl;

    if(from != "application/mathml+xml" || to != "application/x-kformula")
        return KoFilter::NotImplemented;

    KoStore* out = KoStore::createStore(QString(m_chain->outputFile()), KoStore::Write);
    if(!out || !out->open("root")) {
        KMessageBox::error( 0, i18n( "Unable to open output file." ), i18n( "MathML Import Error" ) );
        delete out;
        return KoFilter::FileNotFound;
    }

    KFormula::DocumentWrapper* wrapper = new KFormula::DocumentWrapper( KGlobal::config(), 0 );
    KFormula::Document* doc = new KFormula::Document;
    wrapper->document( doc );
    KFormula::Container* formula = doc->createFormula();

    //formula->loadMathML( m_chain->inputFile() );
    const QString filename( m_chain->inputFile() );
    QFile f( filename );
    if ( !f.open( QIODevice::ReadOnly ) ) {
        KMessageBox::error( 0, i18n( "Failed to open input file: %1" ).arg( filename ), i18n( "MathML Import Error" ) );
        delete wrapper;
        return KoFilter::FileNotFound;
    }

    QDomDocument mathML;
    // Error variables for QDomDocument::setContent
    QString errorMsg;
    int errorLine, errorColumn;
    if ( !mathML.setContent( &f, true, &errorMsg, &errorLine, &errorColumn ) ) {
        delete wrapper;
        QApplication::restoreOverrideCursor();
        kError(KFormula::DEBUGID) << "Parsing error in " << filename << "! Aborting!" << endl
            << " In line: " << errorLine << ", column: " << errorColumn << endl
            << " Error message: " << errorMsg << endl;
        KMessageBox::error( 0, i18n( "Parsing error in MathML file %4 at line %1, column %2\nError message: %3" )
                              .arg( errorLine ).arg( errorColumn ).arg( i18n ( "QXml", errorMsg.utf8() ).arg( filename ) ), i18n( "MathML Import Error" ) );
        return KoFilter::WrongFormat;
    }
    f.close();
    if ( !formula->loadMathML( mathML ) ) {
        delete wrapper;
        return KoFilter::StupidError;
    }

    // taken from KoDocument::saveToStore
    KoStoreDevice dev( out );
    const Q3CString s = doc->saveXML().toCString(); // utf8 already
    const int nwritten = dev.write( s.data(), s.size()-1 );
    if ( nwritten != (int)s.size()-1 ) {
        kWarning() << "wrote " << nwritten << "   - expected " << s.size()-1 << endl;
        KMessageBox::error( 0, i18n( "Failed to write formula." ), i18n( "MathML Import Error" ) );
    }

    out->close();
    delete out;

    delete wrapper;
    return KoFilter::OK;
}

