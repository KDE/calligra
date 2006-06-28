/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/


#include "kformula_doc.h"
#include "kformula_view.h"
#include "kformula_factory.h"

#include <qbitmap.h>
#include <qcolor.h>
#include <qdom.h>
#include <qpainter.h>
#include <qpopupmenu.h>
#include <qprinter.h>
#include <qstring.h>
#include <qwmatrix.h>
#include <qfile.h>

#include <config.h>
#include <unistd.h>

#include <kaboutdialog.h>
#include <kaction.h>
#include <kapplication.h>
#include <kdebug.h>
#include <KoGlobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kstdaction.h>
#include <kstandarddirs.h>
#include <kurl.h>
#include <KoXmlWriter.h>
#include <KoStoreDevice.h>
#include <ktempfile.h>
#include <KoMainWindow.h>

#include <kformulacontainer.h>
#include <kformuladocument.h>


KFormulaDoc::KFormulaDoc(QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, bool singleViewMode)
        : KoDocument(parentWidget, widgetName, parent, name, singleViewMode)
{
    setInstance(KFormulaFactory::global(), false);
    //kdDebug(39001) << "General Settings" << endl;

    history = new KoCommandHistory( actionCollection() );
    wrapper = new KFormula::DocumentWrapper( kapp->config(),
                                             actionCollection(),
                                             history );
    document = new KFormula::Document;
    wrapper->document( document );
    formula = document->createFormula();

    document->setEnabled( true );

    // the modify flag
    connect(history, SIGNAL(commandExecuted()), this, SLOT(commandExecuted()));
    connect(history, SIGNAL(documentRestored()), this, SLOT(documentRestored()));
    dcopObject();
}


KFormulaDoc::~KFormulaDoc()
{
    delete history;
    delete wrapper;
}


bool KFormulaDoc::saveOasis( KoStore* store, KoXmlWriter* manifestWriter )
{
    if ( !store->open( "content.xml" ) )
        return false;

    KoStoreDevice dev( store );
    KoXmlWriter* contentWriter = createOasisXmlWriter( &dev, "math:math" );


    KTempFile contentTmpFile;
    contentTmpFile.setAutoDelete( true );
    QFile* tmpFile = contentTmpFile.file();

    //todo save content
    QTextStream stream(tmpFile);
    stream.setEncoding( QTextStream::UnicodeUTF8 );
    formula->saveMathML( stream, true );

    tmpFile->close();
    contentWriter->addCompleteElement( tmpFile );
    contentTmpFile.close();



    contentWriter->endElement();
    delete contentWriter;

    if(!store->close())
        return false;

    manifestWriter->addManifestEntry("content.xml", "text/xml");

    setModified( false );

    return true;
}


QDomDocument KFormulaDoc::saveXML()
{
    QDomDocument doc = document->saveXML();
    history->documentSaved();
    return doc;
}

bool KFormulaDoc::loadOasis( const QDomDocument& doc, KoOasisStyles&, const QDomDocument&, KoStore* )
{
    // we don't have style into this format
    // we don't have settings into kformula (for the moment)
    // necessary to adapt kformula code to load MathML format with Oasis Extension.
    
    if ( document->loadOasis( doc ) ) {
        history->clear();
        history->documentSaved();
        return true;
    }
    return false;
}

bool KFormulaDoc::loadXML(QIODevice *, const QDomDocument& doc)
{
    if ( doc.doctype().name().lower() == "math"
         || doc.documentElement().tagName().lower() == "math" )
        if ( document->loadOasis( doc ) ) {
            history->clear();
            history->documentSaved();
            return true;
        }
    if ( document->loadXML( doc ) ) {
        history->clear();
        history->documentSaved();
        return true;
    }
    return false;
}

/**
 * Saves the document in native format, to a given file.
 * It is reimplemented to handle the special case of MathML, since it is
 * a native format but not compressed.
 */
bool KFormulaDoc::saveNativeFormat( const QString & file )
{
    QCString mimeType = outputMimeType();
    bool mathml = !mimeType.isEmpty() && mimeType.contains( "mathml", false );
    if ( mathml ) {
        QFile f( file );
        if ( f.open( IO_WriteOnly | IO_Translate ) )
        {
            QTextStream stream( &f );
            stream.setEncoding( QTextStream::UnicodeUTF8 );
            formula->saveMathML( stream, true );
            f.close();
            return true;
        }
        else
            return false;
    }
    // If it's not MathML, let the parent handle it
    return KoDocument::saveNativeFormat( file );
}

KoView* KFormulaDoc::createViewInstance(QWidget* _parent, const char *name)
{
    return new KFormulaPartView(this, _parent, name);
}

void KFormulaDoc::commandExecuted()
{
    if (formula->isEmpty()) {
        setEmpty();
    }
    setModified(true);
}

void KFormulaDoc::documentRestored()
{
    setModified(false);
}


bool KFormulaDoc::initDoc(InitDocFlags /*flags*/, QWidget* /*parentWidget*/)
{
    // If nothing is loaded, do initialize here
    return TRUE;
}

void KFormulaDoc::showStartUpWidget(KoMainWindow* parent, bool /*alwaysShow*/)
{
    parent->setRootDocument( this );
}

bool KFormulaDoc::showEmbedInitDialog(QWidget* /*parent*/)
{
  return true;
}

void KFormulaDoc::paintContent(QPainter& painter, const QRect& rect, bool transparent, double zoomX, double zoomY)
{
    // ####### handle transparency and zoom
    // Need to draw only the document rectangle described in the parameter rect.

    bool forPrint = painter.device() && painter.device()->devType() == QInternal::Printer;
    document->setZoomAndResolution( 100, zoomX, zoomY, true, forPrint );
    if ( !transparent ) {
        painter.fillRect( rect, Qt::white );
    }
    formula->draw( painter, rect );
}

QString KFormulaDoc::configFile() const
{
//    return readConfigFile( locate( "data", "kformula/kformula.rc",
//                                 KFormulaFactory::global() ) );

//    return readConfigFile( "kformula.rc" );
    return QString::null;
}

#include "kformula_doc.moc"
