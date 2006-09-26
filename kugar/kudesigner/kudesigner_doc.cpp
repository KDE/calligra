/* This file is part of the KDE project
  Copyright (C) 2002 Alexander Dymo <cloudtemple@mksat.net>

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
#include "kudesigner_doc.h"
#include "kudesigner_factory.h"
#include "kudesigner_view.h"

#include <KoTemplateChooseDia.h>
#include <kparts/componentfactory.h>
#include <kdebug.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kcommand.h>

#include <qpainter.h>
#include <qprinter.h>
#include <qfileinfo.h>
#include <q3dockwindow.h>
#include <qdom.h>
#include <qtextstream.h>
#include <qtextcodec.h>

#include <canvas.h>
#include <kugartemplate.h>

#include "kudesigner_view.h"

KudesignerDoc::KudesignerDoc( QWidget *parentWidget, QObject* parent, bool singleViewMode )
        : KoDocument( parentWidget, parent, singleViewMode ), m_plugin( 0 ), m_propPos( Qt::RightDockWidgetArea ), m_modified( false )
{
    setInstance( KudesignerFactory::global(), false );
    setTemplateType("kudesigner_template");
    history = new KCommandHistory( actionCollection() );

    //     connect( history, SIGNAL( documentRestored() ), this, SLOT( slotDocumentRestored() ) );
    //     connect( history, SIGNAL( commandExecuted() ), this, SLOT( slotCommandExecuted() ) );
    docCanvas = new Kudesigner::Canvas( 100, 100 );
    emit canvasChanged( docCanvas );
}

KudesignerDoc::~KudesignerDoc()
{
    delete history;
}

void KudesignerDoc::addCommand( KCommand *cmd )
{
    cmd->execute();
    setModified( true );
    /*    history->addCommand(cmd);*/
}

void KudesignerDoc::initEmpty()
{
    QString fileName( KStandardDirs::locate( "kudesigner_template", "General/.source/A4.ktm", KudesignerFactory::global() ) );
    bool ok = loadNativeFormat( fileName );
    if ( !ok )
        showLoadingErrorDialog();
    setEmpty();
    resetURL();
    setModified(false);
}

KoView* KudesignerDoc::createViewInstance( QWidget* parent)
{
    return new KudesignerView( this, parent);
}

bool KudesignerDoc::loadOasis( const QDomDocument&, KoOasisStyles&, const QDomDocument&, KoStore* )
{
    return false;
}

bool KudesignerDoc::saveOasis( KoStore*, KoXmlWriter* )
{
    return false;
}

void KudesignerDoc::paintContent( QPainter& painter, const QRect& rect, bool /*transparent*/,
                                  double /*zoomX*/, double /*zoomY*/ )
{
    // ####### handle transparency

    // Need to draw only the document rectangle described in the parameter rect.

    canvas() ->drawArea( rect, &painter, true );

}

void KudesignerDoc::commandExecuted()
{
    setModified( true );
}

void KudesignerDoc::documentRestored()
{
    setModified( false );
}

Kudesigner::Canvas *KudesignerDoc::canvas()
{
    return docCanvas;
}

int KudesignerDoc::supportedSpecialFormats() const
{
    return SaveAsDirectoryStore | SaveAsFlatXML;
}

QDomDocument KudesignerDoc::saveXML()
{
    QDomDocument doc;
    doc.setContent( docCanvas->kugarTemplate() ->getXml() );
    return doc;
}

bool KudesignerDoc::saveToStream( QIODevice * dev )
{
    QTextStream ts( dev );
    ts.setCodec( QTextCodec::codecForName( "UTF-8" ) );
    ts << docCanvas->kugarTemplate() ->getXml();
    return true;
}

void KudesignerDoc::loadPlugin( const QString &name )
{
    kDebug() << "Trying to load plugin: " << name << endl;
    KuDesignerPlugin *plug = KLibLoader::createInstance<KuDesignerPlugin>( name.utf8(), this );
    m_plugin = plug;
    if ( m_plugin )
        kDebug() << "plugin has been loaded" << endl;
    else
        kDebug() << "plugin couldn't be loaded :(" << endl;
}

bool KudesignerDoc::completeSaving( KoStore* store )
{
    if ( m_plugin )
        return m_plugin->store( store );
    return true;
}

bool KudesignerDoc::completeLoading( KoStore* store )
{
    if ( m_plugin )
        return m_plugin->load( store );
    return true;
}

KuDesignerPlugin *KudesignerDoc::plugin()
{
    return canvas() ->plugin();
}

Qt::DockWidgetArea KudesignerDoc::propertyPosition()
{
    return m_propPos;
}

void KudesignerDoc::setForcedPropertyEditorPosition( Qt::DockWidgetArea d )
{
    m_propPos = d;
}

void KudesignerDoc::setModified( const bool val )
{
    m_modified = val;
    emit modificationMade( val );
}

bool KudesignerDoc::modified( ) const
{
    return m_modified;
}

bool KudesignerDoc::loadXML( QIODevice *, const QDomDocument &rt )
{
    QDomNode report, rep;
    for ( QDomNode report = rt.firstChild(); !report.isNull(); report = report.nextSibling() )
    {
        if ( report.nodeName() == "KugarTemplate" )
        {
            rep = report;
            break;
        }
    }
    report = rep;

    QDomNamedNodeMap attributes = report.attributes();
    //getting the page width and height
    int height = 297;
    int width = 210;

    if ( attributes.namedItem( "PageOrientation" ).nodeValue().toInt() )
    {
        int temp = height;
        height = width;
        width = temp;
    }

    //zooming canvas according to screen resolution
    QPrinter* printer;

    // Set the page size
    printer = new QPrinter();
    printer->setFullPage( true );
    printer->setPageSize( ( QPrinter::PageSize ) attributes.namedItem( "PageSize" ).nodeValue().toInt() );
    printer->setOrientation( ( QPrinter::Orientation ) attributes.namedItem( "PageOrientation" ).nodeValue().toInt() );

    // Get the page metrics and set appropriate wigth and height
    width = printer->width();
    height = printer->height();

    //this is not needed anymore
    delete printer;

    //creating canvas
    if ( docCanvas )
        delete docCanvas;
    docCanvas = new Kudesigner::Canvas( width, height );
    emit canvasChanged( docCanvas );

    docCanvas->setAdvancePeriod( 30 );

    return docCanvas->loadXML( report );
}

void KudesignerDoc::setModified()
{
    setModified( true );
}

#include "kudesigner_doc.moc"
