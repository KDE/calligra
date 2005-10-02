/**************************************************************************
   mdatabasereportengine.cpp  -  Kugar database report engine
                        -------------------
begin                : 2003-03-22 23:58:12
copyright            : (C) 2003 by Joris Marcillac
email                : joris@marcillac.com
**************************************************************************/
/**************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
*   Library General Public License for more details.                      *
*                                                                         *
*   You should have received a copy of the GNU Library General Public     *
*   License along with this library; if not, write to the Free            *
*   Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA                                                    *
*                                                                         *
**************************************************************************/
#include <qsqldatabase.h>
#include <qstring.h>
#include <qsqlrecord.h>
#include <qglobal.h>
#include <qcombobox.h>
#include <qmessagebox.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qfile.h>
#include <qtextstream.h>

#include <kdebug.h>

/** connection dialog */
//#include "connect.h"
#include "mdatabasereportengine.h"

/** internal class, using as possible the same MReportEngine kugar interface.
Some parts of code copyright    : (C) 1999 by Mutiny Bay Software (info@mutinybaysoftware.com)
Some parts of code copyright    : (C) 2002 Alexander Dymo (cloudtemple@mksat.net)
*/

namespace Kugar
{

MDatabaseReportEngine::MDatabaseReportEngine( QWidget *parent, const char *name )
        : QObject( parent, name ), m_strIndent( "    " )
{
    details.setAutoDelete( true );
}

MDatabaseReportEngine::~MDatabaseReportEngine()
{}

bool MDatabaseReportEngine::setReportTemplate( const QString &tpl )
{
    if ( !rt.setContent( tpl ) )
    {
        kdDebug(30001) << "Unable to parse database report template" << endl;
        return false;
    }

    initTemplate();

    return true;
}


bool MDatabaseReportEngine::setReportTemplate( QIODevice *dev )
{
    if ( !rt.setContent( dev ) )
    {
        kdDebug(30001) << "Unable to parse database report template" << endl;
        return false;
    }

    initTemplate();

    return true;
}

/** Walks the document tree, setting the report layout */
void MDatabaseReportEngine::initTemplate()
{
    QDomNode report;
    QDomNode child;

    for ( report = rt.firstChild(); !report.isNull(); report = report.nextSibling() )
        if ( report.nodeName() == "KugarTemplate" )
            break;

    // get the database and details elements only
    QDomNodeList children = report.childNodes();
    int childCount = children.length();

    for ( int j = 0; j < childCount; j++ )
    {
        child = children.item( j );
        if ( child.nodeType() == QDomNode::ElementNode )
        {
            if ( child.nodeName() == "DataBase" )
                setDatabaseAttributes( &child );
            else if ( child.nodeName() == "Detail" )
                setDetailAttributes( &child );
        }
    }
}
/** Sets the database attributes */
void MDatabaseReportEngine::setDatabaseAttributes( QDomNode* report )
{
    // Get all the child database report elements
    QDomNodeList children = report->childNodes();
    int childCount = children.length();

    for ( int j = 0; j < childCount; j++ )
    {
        QDomNode child = children.item( j );
        if ( child.nodeType() == QDomNode::ElementNode )
        {
            if ( child.nodeName() == "Driver" )
                setDriverAttributes( &child );
            else if ( child.nodeName() == "SqlQuery" )
                setSqlQueryAttributes( &child );
            else if ( child.nodeName() == "GroupBy" )
            {
                setGroupByAttributes( &child );     // define the CLinkFieldLevel for the parser
                setSqlOrderByAttributes( &child );  // define the order by for the sql query
            }
        }
    }
}
/** Sets the database driver attributes */
void MDatabaseReportEngine::setDriverAttributes( QDomNode* report )
{
    // Get the driver attributes for the connection database
    QDomNamedNodeMap attributes = report->attributes();

    m_strDriverType = attributes.namedItem( "driverType" ).nodeValue();
    m_strDatabaseName = attributes.namedItem( "databaseName" ).nodeValue();
    m_strUserName = attributes.namedItem( "userName" ).nodeValue();
    m_strPassword = attributes.namedItem( "password" ).nodeValue();
    m_strHostName = attributes.namedItem( "hostName" ).nodeValue();
    m_strPort = attributes.namedItem( "port" ).nodeValue();
}
/** Sets the sql query attributes */
void MDatabaseReportEngine::setSqlQueryAttributes( QDomNode* report )
{
    // Get the attributes for the connection database
    QDomNamedNodeMap attributes = report->attributes();
    m_strSql = attributes.namedItem( "SqlText" ).nodeValue();
}
/** Sets the group by attributes */
void MDatabaseReportEngine::setGroupByAttributes( QDomNode* report )
{
    QDomNodeList children = report->childNodes();
    int childCount = children.length();

    // For each GroupHeader, extract the attr list and set QMap
    for ( int j = 0; j < childCount; j++ )
    {
        QDomNode child = children.item( j );
        QDomNamedNodeMap attributes = child.attributes();
        int level = attributes.namedItem( "Level" ).nodeValue().toInt();
        CLinkFieldLevel field2Level = CLinkFieldLevel(
                                          attributes.namedItem( "NameField" ).nodeValue(),
                                          "",
                                          attributes.namedItem( "Level" ).nodeValue().toInt()
                                      );
        m_mapOldValue[ level ] = field2Level;
    }
}
/** Sets the order by attributes */
void MDatabaseReportEngine::setSqlOrderByAttributes( QDomNode* report )
{
    QDomNodeList children = report->childNodes();
    int childCount = children.length();
    m_strOrderBy = "";
    // For each GroupHeader, extract the attr list and set 'order by' sql query
    for ( int j = 0; j < childCount; ++j )
    {
        QDomNode child = children.item( j );
        QDomNamedNodeMap attributes = child.attributes();

        m_strOrderBy += attributes.namedItem( "NameField" ).nodeValue();
        m_strOrderBy += " ";
        m_strOrderBy += attributes.namedItem( "OrderBy" ).nodeValue();
        // add the comma separator if needed
        if ( j < childCount - 1 )
            m_strOrderBy += ',';
    }
}
/** Sets the detail attributes */
void MDatabaseReportEngine::setDetailAttributes( QDomNode* report )
{
    // Get the attributes for the detail section
    QDomNamedNodeMap attributes = report->attributes();

    // Get the level report detail attributes
    int level = attributes.namedItem( "Level" ).nodeValue().toInt();
    QStringList* detail = new QStringList;

    // Process the report detail Field only
    QDomNodeList children = report->childNodes();
    int childCount = children.length();

    for ( int j = 0; j < childCount; j++ )
    {
        QDomNode child = children.item( j );
        if ( child.nodeType() == QDomNode::ElementNode )
        {
            if ( child.nodeName() == "Field" )
            {
                QDomNamedNodeMap attributes = child.attributes();
                *detail << attributes.namedItem( "Field" ).nodeValue();
            }
        }
    }
    // Append a newly created detail to the list
    details.insert( level, detail );
}
/** init the database connection */
void MDatabaseReportEngine::initDatabase()
{
    // close old connection (if any)
    //  if ( QSqlDatabase::contains() ) {
    //QSqlDatabase* oldDb = QSqlDatabase::database();
    //oldDb->close();
    //QSqlDatabase::removeDatabase( QSqlDatabase::defaultConnection );
    //  }
    // create the database object
    QSqlDatabase * db = QSqlDatabase::addDatabase( m_strDriverType );
    if ( !db )
    {
        //QMessageBox::warning( 0, tr( "Database connection Error" ), tr( "Could not open driver database." ) );
        kdDebug(30001) << "Could not open driver database." << endl;
        return ;
    }
    db->setDatabaseName( m_strDatabaseName );
    db->setUserName( m_strUserName );
    db->setPassword( m_strPassword );
    db->setHostName( m_strHostName );
    db->setPort( m_strPort.toInt() );
    // open the new connection
    if ( !db->open() )
    {
        QString strError =
            "Failed to open database: " +
            db->lastError().driverText() +
            db->lastError().databaseText() ;
        return ;
    }
}
/** create data file for kugar */
bool MDatabaseReportEngine::createReportDataFile( QIODevice* dev, const QString& templateFile )
{
    QFile ft( templateFile );

    if ( ft.open( IO_ReadOnly ) )
    {
        if ( !setReportTemplate( &ft ) )
        {
            kdDebug(30001) << "Invalid data file: " << QFile::encodeName( templateFile ).data() << endl;
            return false;
        }
        ft.close();
    }
    else
    {
        kdDebug(30001) << "Unable to open data file: " << QFile::encodeName( templateFile ).data() << endl;
        return false;
    }

    m_strDataBuffer = new QTextStream( dev );
    initDatabase();
    setHeaderDataFile();
    setSQLQuery();
    setBufferFromDatabase( templateFile );
    delete m_strDataBuffer;
    return true;
}

/** merge database data with existing report data file (it is represented as QIODevice *dev)*/
QString MDatabaseReportEngine::mergeReportDataFile( QIODevice* dev )
{
    QDomDocument dom;
    dom.setContent( dev );

    QString result = dom.toString( 4 );

    QDomNode *formerDataSource = 0;
    QDomNode n = dom.documentElement().firstChild();
    while ( !n.isNull() )
    {
        if ( n.isElement() )
        {
            QDomElement e = n.toElement();
            if ( e.tagName() == QString( "DataSource" ) )
            {
                formerDataSource = &e;
                break;
            }
        }
        n = n.nextSibling();
    }
    if ( formerDataSource == 0 )
    {
        return result;
    }

    QDomElement docElem = dom.documentElement();
    QString templateFile = docElem.attribute( "Template" );
    if ( templateFile.isEmpty() )
        return result;

    QFile ft( templateFile );

    if ( ft.open( IO_ReadOnly ) )
    {
        if ( !setReportTemplate( &ft ) )
        {
            kdDebug(30001) << "Invalid template file: " << templateFile.latin1() << endl;
            return result;
        }
        ft.close();
    }
    else
    {
        kdDebug(30001) << "Unable to open template file: " << templateFile.latin1() << endl;
        return result;
    }

    QString data;
    m_strDataBuffer = new QTextStream( &data, IO_ReadWrite );

    initDatabase();
    //    setHeaderDataFile();
    setSQLQuery();
    setBufferFromDatabase( templateFile, true );

    // perform merging with existing data in the data file

    QDomDocument d;
    d.setContent( "<temp>" + data + "</temp>" );
    kdDebug(30001) << "temp dom is: " << d.toString( 4 ).latin1() << endl;
    n = d.documentElement().lastChild();
    while ( !n.isNull() )
    {
        if ( n.isElement() )
        {
            QDomNode n2 = n.cloneNode();
            docElem.insertAfter( n2, *formerDataSource );
        }
        n = n.previousSibling();
    }

    kdDebug(30001) << "DOM (before): " << dom.toString( 4 ).latin1() << endl;

    dom.documentElement().removeChild( *formerDataSource );

    kdDebug(30001) << "DOM: " << dom.toString( 4 ).latin1() << endl;

    delete m_strDataBuffer;

    return dom.toString( 4 );
}

/** set buffer data from database recordset */
bool MDatabaseReportEngine::setBufferFromDatabase( const QString& strTemplateFile, bool merge )
{
    Q_ASSERT( !m_strSql.isEmpty() || !m_strSql.isNull() );
    Q_ASSERT( !m_strSql.isEmpty() || !m_strSql.isNull() );
    Q_ASSERT( QSqlDatabase::contains() );

    CSqlCursor* cursor = new CSqlCursor( m_strSql );
    if ( !cursor->isActive() )
    {
        kdDebug(30001) << "Unable to get data from database, the cursor is not active." << endl;
        delete cursor;
        return false;
    }

    if ( !merge )
    {
        // fill the row attribut list with the field name of recordset
        for ( uint i = 0; i < cursor->count(); ++i )
        {
            *m_strDataBuffer << cursor->fieldName( i );
            *m_strDataBuffer << " CDATA #IMPLIED" ;
            // we put 2 indents for all the field
            if ( i < ( cursor->count() - 1 ) )
                * m_strDataBuffer << endl << m_strIndent << m_strIndent;
            // for the last one we close the attribut list
            else if ( i == ( cursor->count() - 1 ) )
                * m_strDataBuffer << ">" << endl;
        }
        *m_strDataBuffer << "]>" << endl << endl;
        *m_strDataBuffer << "<KugarData Template=\"";
        *m_strDataBuffer << strTemplateFile;
        *m_strDataBuffer << "\">" << endl << m_strIndent;
    }

    // load data buffer from the recordset
    while ( cursor->next() )
    {
        // parse throw all the details levels
        for ( uint i = 0; i < m_mapOldValue.count(); ++i )
        {
            QString fieldName = m_mapOldValue[ i ].fieldName();
            QStringList* detail = details[ i ]; // for info i == m_mapOldValue[i].level()
            // if the value of recordset is different from last parse, we create a new data row
            if ( cursor->value( fieldName ) != m_mapOldValue[ i ].oldValue() )
            {
                // set the old value as current value
                m_mapOldValue[ i ].setOldValue( cursor->value( fieldName ) );
                addDataRow( i, detail, cursor );
            }
            else if ( i == ( m_mapOldValue.count() - 1 ) )
            {
                // we create the row with the last level number if we have the same data value
                addDataRow( i, detail, cursor );
            }
        }
    }
    if ( !merge )
        * m_strDataBuffer << "</KugarData>" << endl;
    delete cursor;
    return true;
}
/** add data row in the string data buffer with XML format */
void MDatabaseReportEngine::addDataRow( int level, QStringList* detail, CSqlCursor* cursor )
{
    *m_strDataBuffer << "<Row level=\""
    << QString::number( level )   // for info level == m_mapOldValue[i].level()
    << "\" ";
    for ( QStringList::Iterator it = detail->begin(); it != detail->end(); ++it )
    {
        *m_strDataBuffer << cursor->getXMLValue( *it );
    }
    *m_strDataBuffer << "/>" << endl << m_strIndent;

}
/** set header data file */
void MDatabaseReportEngine::setHeaderDataFile()
{
    *m_strDataBuffer << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl << endl;
    *m_strDataBuffer << "<!DOCTYPE KugarData [" << endl << m_strIndent;
    *m_strDataBuffer << "<!ELEMENT KugarData (Row* )>" << endl << m_strIndent;
    *m_strDataBuffer << "<!ATTLIST KugarData" << endl << m_strIndent << m_strIndent;
    *m_strDataBuffer << "Template CDATA #REQUIRED>" << endl << endl << m_strIndent;

    *m_strDataBuffer << "<!ELEMENT Row EMPTY>" << endl << m_strIndent;
    *m_strDataBuffer << "<!ATTLIST Row" << endl << m_strIndent << m_strIndent;
    *m_strDataBuffer << "level CDATA #REQUIRED" << endl << m_strIndent << m_strIndent;
}
/** pop up a dialog to ask the user for create a database connection */
void MDatabaseReportEngine::initDatabaseDlg()
{
    //  ConnectDialog* cnnDlg = new ConnectDialog( 0, tr("Database connection dialog"), TRUE );
    //    if ( cnnDlg->exec() != QDialog::Accepted )
    //      return;
    // set data for the new connection
    //  m_strDriverType = cnnDlg->comboDriver->currentText();
    //  m_strDatabaseName = cnnDlg->editDatabase->text();
    //  m_strUserName = cnnDlg->editUsername->text();
    //  m_strPassword = cnnDlg->editPassword->text();
    //  m_strHostName = cnnDlg->editHostname->text();
    //  m_strPort = cnnDlg->portSpinBox->value();
    // create the connection
    //  initDatabase();
}

void MDatabaseReportEngine::setSQLQuery()
{
    m_strSql += " ORDER BY ";
    m_strSql += m_strOrderBy;
    m_strSql += ';';
}

}

#include "mdatabasereportengine.moc"
