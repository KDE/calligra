/* This file is part of the KDE project
   Copyright (C) 2002 Chris Machemer <machey@ceinetworks.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kdatabase_part.h"
#include "kdatabase_view.h"
#include "kdatabase_factory.h"
#include "kdebug.h"
#include "kstandarddirs.h"	

#include "kdbdocbrowser.h"
#include <koTemplateChooseDia.h>

#include <qpainter.h>
#include <qfileinfo.h>

KDatabasePart::KDatabasePart( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, bool singleViewMode )
    : KoDocument( parentWidget, widgetName, parent, name, singleViewMode )
{
   //myTabDlg = new kdatabase_tabdlg(parentWidget,widgetName);
}

bool KDatabasePart::initDoc()
{
    // If nothing is loaded, do initialize here
    QString _template;
    bool ok = FALSE;

    KoTemplateChooseDia::ReturnType ret = KoTemplateChooseDia::choose(
        KDatabaseFactory::global(), _template, "application/x-kdatabase", "*.kdb", i18n("KDatabase"),
        KoTemplateChooseDia::Everything, "kdatabase_template");
    if ( ret == KoTemplateChooseDia::Template ) {
        QFileInfo fileInfo( _template );
        QString fileName( fileInfo.dirPath( TRUE ) + "/" + fileInfo.baseName() + ".kdt" );
        resetURL();
        ok = loadNativeFormat( fileName );
//        initUnit();
//        setEmpty();
    } else if ( ret == KoTemplateChooseDia::File ) {
        KURL url( _template);
        kdDebug() << "KDatabase_Part::initDoc opening URL " << url.prettyURL() << endl;
        ok = openURL( url );
    } else if ( ret == KoTemplateChooseDia::Empty ) {
        QString fileName( locate( "kdatabase_template", "Normal/.source/PlainText.kdb" , KDatabaseFactory::global() ) );
        resetURL();
        ok = loadNativeFormat( fileName );
//        initUnit();
//        setEmpty();
    }
    setModified( FALSE );

    return TRUE;
}

KoView* KDatabasePart::createViewInstance( QWidget* parent, const char* name )
{
    return new KDatabaseView( this, parent, name );
}

bool KDatabasePart::loadXML( QIODevice *, const QDomDocument &doc)
{
    QString value;
    QDomElement kdb = doc.documentElement();

    value = kdb.attribute( "mime", QString::null );
    if ( value.isEmpty() )
    {
        kdError(32001) << "No mime type specified!" << endl;
        setErrorMessage( i18n( "Invalid document. No mimetype specified." ) );
        return false;
    }
    else if ( value != "application/x-kdatabase" )
    {
        kdError(32001) << "Unknown mime type " << value << endl;
        setErrorMessage( i18n( "Invalid document. Expected mimetype application/x-kdatabase, got %1" ).arg( value ) );
        return false;
    }
    KDBFile=doc;
    kdDebug() << "Load succeeded" << endl;
    return true;
}

QDomDocument KDatabasePart::saveXML()
{
    // TODO save the document into a QDomDocument
    return QDomDocument();
}


void KDatabasePart::paintContent( QPainter& painter, const QRect& rect, bool /*transparent*/,
                                double /*zoomX*/, double /*zoomY*/ )
{
    // ####### handle transparency

    // Need to draw only the document rectangle described in the parameter rect.
    int left = rect.left() / 20;
    int right = rect.right() / 20 + 1;
    int top = rect.top() / 20;
    int bottom = rect.bottom() / 20 + 1;

    for( int x = left; x < right; ++x )
        painter.drawLine( x * 20, top * 20, x * 20, bottom * 20 );
    for( int y = left; y < right; ++y )
        painter.drawLine( left * 20, y * 20, right * 20, y * 20 );
}

QDomDocument* KDatabasePart::getKDBFile()
{

return(&KDBFile);
}

#include "kdatabase_part.moc"
