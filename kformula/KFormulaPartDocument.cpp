/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
		 2006 Martin Pfeiffer <hubipete@gmx.net>

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
   Boston, MA 02110-1301, USA.
*/


#include "KFormulaPartDocument.h"
#include "KFormulaPartView.h"
#include "KFormulaPartFactory.h"

#include <QPainter>
#include <QString>
#include <QFile>
#include <QTextStream>

#include <KoGlobal.h>
#include <KoXmlWriter.h>
#include <KoMainWindow.h>
#include <KoStoreDevice.h>
#include <ktempfile.h>
#include <klocale.h>


KFormulaPartDocument::KFormulaPartDocument( QWidget *parentWidget, QObject* parent,
                                              bool singleViewMode )
                     : KoDocument( parentWidget, parent, singleViewMode )
{
    setInstance( KFormulaPartFactory::global(), false );

//    m_formulaShape = new FormulaShape();

    m_commandHistory = new KCommandHistory( actionCollection() );
/*    connect( m_commandHistory, SIGNAL( commandExecuted() ),
		         this, SLOT( slotExecuted() ) );
    connect( m_commandHistory, SIGNAL( documentRestored() ),
		         this, SLOT( documentRestored() ) );*/
}


KFormulaPartDocument::~KFormulaPartDocument()
{
    delete m_commandHistory;
//    delete m_formulaShape;
}


bool KFormulaPartDocument::saveOasis( KoStore* store, KoXmlWriter* manifestWriter )
{
/*    if ( !store->open( "content.xml" ) )
        return false;

    KoStoreDevice dev( store );
    KoXmlWriter* contentWriter = createOasisXmlWriter( &dev, "math:math" );

    KTempFile contentTmpFile;
    contentTmpFile.setAutoDelete( true );
    QFile* tmpFile = contentTmpFile.file();

    QTextStream stream( tmpFile );
    formula->saveMathML( stream, true );

    tmpFile->close();
    contentWriter->addCompleteElement( tmpFile );
    contentTmpFile.close();
    contentWriter->endElement();
    delete contentWriter;

    if( !store->close() )
        return false;

    manifestWriter->addManifestEntry("content.xml", "text/xml");
    setModified( false );*/
    return true;
}

bool KFormulaPartDocument::loadOasis( const QDomDocument& doc, KoOasisStyles&,
                                      const QDomDocument&, KoStore* )
{
/*    if ( document->loadOasis( doc ) )
    {
        m_commandHistory->clear();
        m_commendHistory->documentSaved();
        return true;
    }*/
    
    return false;
}

bool KFormulaPartDocument::loadXML(QIODevice *, const QDomDocument& doc)
{
/*    if ( doc.doctype().name().lower() == "math" ) // FIXME: This is ugly
        if ( document->loadOasis( doc ) ) {
            history->clear();
            history->documentSaved();
            return true;
        }
    if ( document->loadXML( doc ) )
      {
           history->clear();
           history->documentSaved();
           return true;
      }*/
      return false;
}

KoView* KFormulaPartDocument::createViewInstance( QWidget* parent )
{
    return new KFormulaPartView( this, parent );
}

void KFormulaPartDocument::commandExecuted()
{
    setModified( true );
}

void KFormulaPartDocument::documentRestored()
{
    setModified( false );
}

void KFormulaPartDocument::showStartUpWidget( KoMainWindow* parent, bool )
{
    parent->setRootDocument( this );
}

bool KFormulaPartDocument::showEmbedInitDialog(QWidget* /*parent*/)
{
    return true;
}

void KFormulaPartDocument::paintContent( QPainter& painter, const QRect& rect,
                                         bool transparent, double zoomX, double zoomY )
{
    bool forPrint = painter.device() && painter.device()->devType() == QInternal::Printer;

    if ( !transparent )
        painter.fillRect( rect, Qt::white );
    
//    m_formulaShape->drawFormula( painter, rect );
}

QString KFormulaPartDocument::configFile() const
{
    return QString();
}

#include "KFormulaPartDocument.moc"
