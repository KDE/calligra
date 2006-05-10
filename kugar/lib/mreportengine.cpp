/***************************************************************************
            mreportengine.cpp  -  Kugar report engine
            -------------------
  begin     : Sun Aug 15 1999
  copyright : (C) 1999 by Mutiny Bay Software
  email     : info@mutinybaysoftware.com
  copyright : (C) 2002 Alexander Dymo
  email     : cloudtemple@mksat.net
***************************************************************************/

#include <QFile>
#include <qprinter.h>
//Added by qt3to4:
#include <Q3PtrList>
#include <Q3MemArray>

#include "mreportengine.h"
#include "mutil.h"

#include <kdebug.h>

#include <math.h>

namespace Kugar
{

/** Constructor */
MReportEngine::MReportEngine() : QObject()
{
    // Set page params
    m_pageCollection = 0;
    m_needRegeneration = true;
    m_refCount = 1;
    pageSize = MReportEngine::Letter;
    pageOrientation = MReportEngine::Portrait;
    topMargin = 0;
    bottomMargin = 0;
    leftMargin = 0;
    rightMargin = 0;
    heightOfDetails = 0;
    widthDelta = 1;
    heightDelta = 1;

    // Set cancel flag
    cancelRender = false;

    // Set the grand total list to AutoDelete
    grandTotal.setAutoDelete( true );
    // Set the details lists to AutoDelete
    dHeaders.setAutoDelete( true );
    details.setAutoDelete( true );
    dFooters.setAutoDelete( true );

    rHeader.setPrintFrequency( MReportSection::FirstPage );
    pHeader.setPrintFrequency( MReportSection::EveryPage );
    pFooter.setPrintFrequency( MReportSection::EveryPage );
    rFooter.setPrintFrequency( MReportSection::LastPage );

    // Set the default page metrics
    QSize ps = getPageMetrics( pageSize, pageOrientation );
    pageWidth = ps.width();
    pageHeight = ps.height();
}

/** Copy constructor */
MReportEngine::MReportEngine( const MReportEngine& mReportEngine )   /*: QObject((QObject &) mReportEngine)*/
{
    copy( &mReportEngine );
}

/** Assignment operator */
MReportEngine MReportEngine::operator=( const MReportEngine& mReportEngine )
{
    if ( &mReportEngine == this )
        return * this;

    // Copy the derived class's data
    copy( &mReportEngine );

    // Copy the base class's data
    //((QObject &) *this) = mReportEngine;

    return *this;
}

/** Destructor */
MReportEngine::~MReportEngine()
{
    // Clear the grand total data
    grandTotal.clear();
    kDebug( 31000 ) << k_funcinfo << endl;
}

void MReportEngine::addRef()
{
    m_refCount++;
}

void MReportEngine::removeRef()
{
    m_refCount--;
    if ( !m_refCount )
        deleteLater();
}

/** Clears report formatting */
void MReportEngine::clearFormatting()
{
    rHeader.clear();
    pHeader.clear();
    dHeaders.clear();
    details.clear();
    dFooters.clear();
    pFooter.clear();
    rFooter.clear();
}


//Set the report data from an existing QDomDocument
bool MReportEngine::setReportData( const QDomDocument &data )
{
    rd = data.cloneNode( true ).toDocument();
    initData();

    return true;
}


// Set the report's data from an inline string.  Return true if it was valid
// data.

bool MReportEngine::setReportData( const QString &data )
{

    if ( !rd.setContent( data ) )
    {
        kDebug() << "Unable to parse report data" << endl;
        return false;
    }
    initData();

    return true;
}


// Set the report's data from an i/o device.  Return true if it was valid data.

bool MReportEngine::setReportData( QIODevice *dev )
{
    if ( !rd.setContent( dev ) )
    {
        kDebug() << "Unable to parse report data" << endl;
        return false;
    }
    initData();

    return true;
}


// Initialise after reading a report's data.

void MReportEngine::initData()
{
    m_needRegeneration = true;
    // Get the record set (we assume there is only one).

    for ( QDomNode n = rd.firstChild(); !n.isNull(); n = n.nextSibling() )
        if ( n.nodeName() == "KugarData" )
        {
            // Get the records.

            records = n.childNodes();

            // See if there is a prefered template given.

            QDomNamedNodeMap attr = n.attributes();
            QDomNode tempattr = attr.namedItem( "Template" );
            QString tempname = tempattr.nodeValue();

            if ( !tempname.isNull() )
                emit preferedTemplate( tempname );

            break;
        }
}


// Set the report's template from an inline string.  Return true if it was a
// valid template.

bool MReportEngine::setReportTemplate( const QString &tpl )
{
    clearFormatting();

    if ( !rt.setContent( tpl ) )
    {
        kDebug() << "Unable to parse report template" << endl;
        return false;
    }

    initTemplate();

    return true;
}


// Set the report's template from an i/o device.  Return true if it was a valid
// template.

bool MReportEngine::setReportTemplate( QIODevice *dev )
{
    clearFormatting();

    if ( !rt.setContent( dev ) )
    {
        kDebug() << "Unable to parse report template" << endl;
        return false;
    }

    initTemplate();

    return true;
}


/** Cancels rendering of a report */
void MReportEngine::slotCancelRendering()
{
    cancelRender = true;
}

/** Finds the detail header object, which is apropriate for the given level */
MReportSection *MReportEngine::findDetailHeader( int level )
{
    MReportSection * sec;
    for ( sec = dHeaders.first(); sec; sec = dHeaders.next() )
    {
        if ( sec->getLevel() == level )
            return sec;
    }
    return NULL;
}

/** Finds the detail object, which is apropriate for the given level */
MReportDetail *MReportEngine::findDetail( int level )
{
    MReportDetail * sec;
    for ( sec = details.first(); sec; sec = details.next() )
    {
        if ( sec->getLevel() == level )
            return sec;
    }
    return NULL;
}

/** Finds the detail footer object, which is apropriate for the given level */
MReportSection *MReportEngine::findDetailFooter( int level )
{
    MReportSection * sec;
    for ( sec = dFooters.first(); sec; sec = dFooters.next() )
    {
        if ( sec->getLevel() == level )
            return sec;
    }
    return NULL;
}

/** Renders the report as a page collection - the caller
  * is responsible for de-allocating the returned
  * collection
  */
MPageCollection* MReportEngine::renderReport()
{
    if ( !m_needRegeneration )
        return m_pageCollection;
    if ( m_pageCollection )
    {
        m_pageCollection->deleteLater();
        m_pageCollection = 0;
    }

    unsigned int j;
    unsigned int i;

    // Set cancel flag
    cancelRender = false;

    // Create the page collection
    MPageCollection* pages = new MPageCollection( this );

    // Initialize the basic page data
    currHeight = pageHeight - ( topMargin + bottomMargin + pFooter.getHeight() );
    currPage = 0;
    currDate = QDate::currentDate();

    // Initialise global report variables
    unsigned int rowCount = records.length();
    for ( j = 0; j < rowCount; j++ )
    {
        QDomNode record = records.item( j );
        if ( ( record.nodeType() == QDomNode::ElementNode )
                && ( record.nodeName() == "Var" ) )
        {
            // Process the variable ...
            QDomNamedNodeMap fields = record.attributes();
            for ( int k = 0; k < fields.count(); ++k )
            {
                rHeader.setFieldData( fields.item( k ).nodeName(), fields.item( k ).nodeValue() );
                rFooter.setFieldData( fields.item( k ).nodeName(), fields.item( k ).nodeValue() );
                pHeader.setFieldData( fields.item( k ).nodeName(), fields.item( k ).nodeValue() );
                pFooter.setFieldData( fields.item( k ).nodeName(), fields.item( k ).nodeValue() );
            }
        }
    }

    // Initialize the grand total array
    grandTotal.clear();
    for ( int i = 0; i < rFooter.getCalcFieldCount(); i++ )
        grandTotal.append( new Q3MemArray<double> );

    // Create the first page
    startPage( pages );

    // Draw the detail section by looping through all the records
    int chkRow = 0;
    int curDetailLevel = 0;
    int prevDetailLevel = -1;
    int curDetailHeader = -1;
    int curDetailFooter = -1;

    for ( j = 0; j < rowCount; j++ )
    {
        QString detailValue;
        QDomNode record = records.item( j );

        if ( ( record.nodeType() == QDomNode::ElementNode )
                && ( record.nodeName() == "Row" ) )
        {
            // Update status event
            if ( ( chkRow = ( j / 2 ) % 20 ) == 0 )
                emit signalRenderStatus( j / 2 );

            // Check for cancel action
            if ( cancelRender )
            {
                p.end();
                delete pages;
                return 0;
            }

            // Process the record ...
            QDomNamedNodeMap fields = record.attributes();

            // Find the detail object to process with rendering
            detailValue = fields.namedItem( "level" ).nodeValue();
            curDetailLevel = detailValue.toInt();

            // Reset the header/footer detail levels?
            curDetailHeader = curDetailLevel > curDetailHeader ? -1 : curDetailHeader;
            curDetailFooter = curDetailLevel > curDetailFooter ? -1 : curDetailFooter;

            // Draw detail footer if appropriate
            if ( curDetailLevel <= prevDetailLevel )
            {
                // Draw footer for previous detail levels
                //   from prevDetailLevel up to curDetailLevel
                // Draw footer for current detail level
                for ( int i = prevDetailLevel; i >= curDetailLevel; i-- )
                {
                    MReportSection *footer = findDetailFooter( i );
                    if ( footer )
                    {
                        footer->setPageNumber( currPage );
                        footer->setReportDate( currDate );
                        footer->setCalcFieldData( &grandTotal );
                        if ( ( currY + footer->getHeight() ) > currHeight )
                        {
                            newPage( pages );
                        }
                        footer->draw( &p, leftMargin, currY );
                        currY += footer->getHeight();
                    }
                }
            }

            MReportDetail *detail = findDetail( detailValue.toInt() );
            detail->setPageNumber( currPage );
            detail->setReportDate( currDate );

            int headerHeight = 0;
            MReportSection * header = findDetailHeader( curDetailLevel );
            if ( header && curDetailHeader != curDetailLevel )
                headerHeight = header->getHeight();

            bool repeating = ( currY + detail->getHeight() + headerHeight ) > currHeight ||
                             ( detail->getRepeat() && currY + heightOfDetails > currHeight );
            if ( repeating )
            {
                newPage( pages );

                MReportDetail *sec;
                for ( sec = details.first(); sec; sec = details.next() )
                {
                    if ( sec->getLevel() != curDetailLevel && sec->getRepeat() )
                    {
                        sec->draw( &p, leftMargin, currY );
                        currY += sec->getHeight();
                    }
                }
            }

            // Draw detail header for level curDetailLevel
            // if it hasn't already been drawn
            if ( curDetailHeader != curDetailLevel || repeating )
            {
                MReportSection * header = findDetailHeader( curDetailLevel );
                if ( header )
                {
                    header->setPageNumber( currPage );
                    header->setReportDate( currDate );
                    if ( ( currY + header->getHeight() ) > currHeight )
                    {
                        newPage( pages );
                    }
                    header->draw( &p, leftMargin, currY );
                    currY += header->getHeight();
                    curDetailHeader = curDetailLevel;
                }
            }

            unsigned int fieldCount = detail->getFieldCount();
            for ( i = 0; i < fieldCount; i++ )
            {
                // Get the field value
                detailValue = fields.namedItem( detail->getFieldName( i ) ).nodeValue();
                // Set the field
                detail->setFieldData( i, detailValue );
                // Set the grand total data
                int calcIdx = rFooter.getCalcFieldIndex( detail->getFieldName( i ) );
                if ( calcIdx != -1 )
                {
                    int vsize = grandTotal.at( calcIdx ) ->size();
                    grandTotal.at( calcIdx ) ->resize( vsize + 1 );
                    grandTotal.at( calcIdx ) ->at( vsize ) = detailValue.toDouble();
                }
            }

            detail->draw( &p, leftMargin, currY );
            currY += detail->getHeight();
            prevDetailLevel = curDetailLevel;
        }
    }

    // Draw detail footers that were not drawn before
    //   for details from curDetailLevel up to prevDetailLevel
    for ( int i = prevDetailLevel; i >= 0; i-- )
    {
        MReportSection *footer = findDetailFooter( i );
        if ( footer )
        {
            footer->setPageNumber( currPage );
            footer->setReportDate( currDate );
            footer->setCalcFieldData( &grandTotal );
            if ( ( currY + footer->getHeight() ) > currHeight )
            {
                newPage( pages );
            }

            footer->draw( &p, leftMargin, currY );
            currY += footer->getHeight();
        }
    }

    // Finish the last page of the report
    endPage( pages );

    // Destroy the page painter
    p.end();

    // Set the page collection attributes
    pages->setPageDimensions( QSize( pageWidth, pageHeight ) );
    pages->setPageSize( pageSize );
    pages->setPageOrientation( pageOrientation );

    // Send final status
    emit signalRenderStatus( rowCount / 2 );
    m_needRegeneration = false;
    m_pageCollection = pages;
    return pages;
}

/** Starts a new page of the report */
void MReportEngine::startPage( MPageCollection* pages )
{
    // Set the current y pos
    currY = topMargin;

    // Create a new page
    pages->appendPage();
    // Increment the page count
    currPage++;

    // Set the page painter to the current page
    p.begin( pages->getCurrentPage() );

    // Draw the report header
    drawReportHeader( pages );
    // Draw the page header
    drawPageHeader( pages );
}

/** Finishes the current page of the report */
void MReportEngine::endPage( MPageCollection* pages )
{
    // Draw the report footer
    drawReportFooter( pages );
    // Draw the page footer
    drawPageFooter( pages );
}

/** Finishes the current page and adds a new page */
void MReportEngine::newPage( MPageCollection* pages )
{
    // Draw the page footer
    drawPageFooter( pages );

    // Stop the painter
    p.end();

    // Start a new page
    startPage( pages );
}

/** Draws the report header object to the current page */
void MReportEngine::drawReportHeader( MPageCollection* pages )
{
    if ( rHeader.getHeight() == 0 )
        return ;

    if ( ( rHeader.printFrequency() == MReportSection::FirstPage && currPage == 1 )
            || ( rHeader.printFrequency() == MReportSection::EveryPage ) )
    {

        rHeader.setPageNumber( currPage );
        rHeader.setReportDate( currDate );
        rHeader.draw( &p, leftMargin, currY );
        currY += rHeader.getHeight();
    }
}

/** Draws the page header to the current page */
void MReportEngine::drawPageHeader( MPageCollection* pages )
{
    if ( pHeader.getHeight() == 0 )
        return ;

    if ( ( currY + pHeader.getHeight() ) > currHeight )
        newPage( pages );

    if ( ( pHeader.printFrequency() == MReportSection::FirstPage && currPage == 1 )
            || ( pHeader.printFrequency() == MReportSection::EveryPage ) )
    {

        pHeader.setPageNumber( currPage );
        pHeader.setReportDate( currDate );
        pHeader.draw( &p, leftMargin, currY );
        currY += pHeader.getHeight();
    }
}

/** Draws the page footer to the current page */
void MReportEngine::drawPageFooter( MPageCollection* pages )
{
    if ( pFooter.getHeight() == 0 )
        return ;

    if ( ( pFooter.printFrequency() == MReportSection::FirstPage && currPage == 1 )
            || ( pFooter.printFrequency() == MReportSection::EveryPage ) )
    {

        pFooter.setPageNumber( currPage );
        pFooter.setReportDate( currDate );
        pFooter.draw( &p, leftMargin, ( pageHeight - bottomMargin ) - pFooter.getHeight() );
        currY += pFooter.getHeight();
    }
}

/** Draws the report footer to the current page */
void MReportEngine::drawReportFooter( MPageCollection* pages )
{
    if ( rFooter.getHeight() == 0 )
        return ;

    if ( ( currY + rFooter.getHeight() ) > currHeight )
        newPage( pages );

    if ( ( rFooter.printFrequency() == MReportSection::EveryPage )
            || ( rFooter.printFrequency() == MReportSection::LastPage ) )
    {

        rFooter.setCalcFieldData( &grandTotal );

        rFooter.setPageNumber( currPage );
        rFooter.setReportDate( currDate );
        rFooter.draw( &p, leftMargin, currY );
        currY += rFooter.getHeight();
    }
}

/** Gets the metrics for the selected page size & orientation */
QSize MReportEngine::getPageMetrics( int size, int orientation )
{

    QPrinter * printer;
    QSize ps;

    // Set the page size
    printer = new QPrinter();
    printer->setFullPage( true );
    printer->setPageSize( ( QPrinter::PageSize ) size );
    printer->setOrientation( ( QPrinter::Orientation ) orientation );


    // Display the first page of the report
    ps.setWidth( printer->width() );
    ps.setHeight( printer->height() );

    delete printer;

    return ps;
}


/**
This recalculates report dimensions to 96 dpi, standard on Windows
platform.

FIXME: TODO: get the proper screen resolution and not hardcode 96dpi
FIXME: TODO: make this function work on X11 too
*/
void MReportEngine::recalcDimensions()
{
    QDomNode report;
    QDomNode child;
    for ( report = rt.firstChild(); !report.isNull(); report = report.nextSibling() )
        if ( report.nodeName() == "KugarTemplate" )
            break;

    QDomNamedNodeMap rattributes = report.attributes();
    recalcAttribute( "BottomMargin", rattributes );
    recalcAttribute( "TopMargin", rattributes );
    recalcAttribute( "LeftMargin", rattributes );
    recalcAttribute( "RightMargin", rattributes );

    QDomNodeList children = report.childNodes();
    int childCount = children.length();

    for ( int j = 0; j < childCount; j++ )
    {
        child = children.item( j );
        QDomNamedNodeMap attributes = child.attributes();

        QDomNodeList children2 = child.childNodes();
        int childCount2 = children2.length();
        recalcAttribute( "Height", attributes );

        for ( int k = 0; k < childCount2; k++ )
        {
            QDomNode child2 = children2.item( k );
            QDomNamedNodeMap attributes = child2.attributes();
            recalcAttribute( "X", attributes );
            recalcAttribute( "Y", attributes );
            recalcAttribute( "Width", attributes );
            recalcAttribute( "Height", attributes );
            recalcAttribute( "X1", attributes );
            recalcAttribute( "X2", attributes );
            recalcAttribute( "Y1", attributes );
            recalcAttribute( "Y2", attributes );
        }
    }

}

void MReportEngine::recalcAttribute( const QString& name, QDomNamedNodeMap attributes )
{
    if ( !attributes.namedItem( name ).isNull() )
    {
        attributes.namedItem( name ).setNodeValue( QString( "%1" ).arg( attributes.namedItem( name ).nodeValue().toInt() * 93 / 81 ) );
    }
}

/** Walks the document tree, setting the report layout */
void MReportEngine::initTemplate()
{
    heightOfDetails = 0;

#ifdef Q_WS_WIN

    recalcDimensions();
#endif

    QDomNode report;
    QDomNode child;

    m_needRegeneration = true;
    // Get the report - assume there is only one.

    for ( report = rt.firstChild(); !report.isNull(); report = report.nextSibling() )
        if ( report.nodeName() == "KugarTemplate" )
            break;

    setReportAttributes( &report );

    // Get all the child report elements
    QDomNodeList children = report.childNodes();
    int childCount = children.length();

    for ( int j = 0; j < childCount; j++ )
    {
        child = children.item( j );

        if ( child.nodeType() == QDomNode::ElementNode )
        {
            // Report Header
            if ( child.nodeName() == "ReportHeader" )
                setSectionAttributes( &rHeader, &child );
            else if ( child.nodeName() == "PageHeader" )
                setSectionAttributes( &pHeader, &child );
            else if ( child.nodeName() == "DetailHeader" )
            {
                MReportSection * dHeader = new MReportSection;
                dHeaders.append( dHeader );
                setDetMiscAttributes( dHeader, &child );
            }
            else if ( child.nodeName() == "Detail" )
                setDetailAttributes( &child );
            else if ( child.nodeName() == "DetailFooter" )
            {
                MReportSection * dFooter = new MReportSection;
                setDetMiscAttributes( dFooter, &child );
                dFooters.append( dFooter );
            }
            else if ( child.nodeName() == "PageFooter" )
                setSectionAttributes( &pFooter, &child );
            else if ( child.nodeName() == "ReportFooter" )
                setSectionAttributes( &rFooter, &child );
        }
    }
}

/** Sets the main layout attributes for the report */
void MReportEngine::setReportAttributes( QDomNode* report )
{
    // Get the attributes for the report
    QDomNamedNodeMap attributes = report->attributes();

    pageSize = attributes.namedItem( "PageSize" ).nodeValue().toInt();
    pageOrientation = attributes.namedItem( "PageOrientation" ).nodeValue().toInt();
    int templateWidth = attributes.namedItem( "PageWidth" ).nodeValue().toInt();
    int templateheight = attributes.namedItem( "PageHeight" ).nodeValue().toInt();

    QSize ps = getPageMetrics( pageSize, pageOrientation );
    pageWidth = ps.width();
    pageHeight = ps.height();

    widthDelta = ( float ) pageWidth / templateWidth;
    heightDelta = ( float ) pageHeight / templateheight;

    kDebug( 30001 ) << "pagewidth: " << pageWidth
    << " pageheight: " << pageHeight << "\n"
    << "templateWidth: " << templateWidth
    << " templateheight: " << templateheight << "\n"
    << "widthDelta: " << widthDelta
    << " heightDelta: " << heightDelta
    << endl;

    topMargin = scaleDeltaHeight( attributes.namedItem( "TopMargin" ).nodeValue().toInt() );
    bottomMargin = scaleDeltaHeight( attributes.namedItem( "BottomMargin" ).nodeValue().toInt() );
    leftMargin = scaleDeltaWidth( attributes.namedItem( "LeftMargin" ).nodeValue().toInt() );
    rightMargin = scaleDeltaWidth( attributes.namedItem( "RightMargin" ).nodeValue().toInt() );
}

int MReportEngine::scaleDeltaWidth( int width ) const
{
    float f = width * widthDelta;
    return f > 1 ? int( f + 0.5) : ceil( f );
}

int MReportEngine::scaleDeltaHeight( int height ) const
{
    float f = height * heightDelta;
    return f > 1 ? int( f + 0.5 ) : ceil( f );
}

/** Sets the layout attributes for the given report section */
void MReportEngine::setSectionAttributes( MReportSection* section, QDomNode* report )
{
    // Get the attributes for the section
    QDomNamedNodeMap attributes = report->attributes();

    // Get the section attributes
    section->setHeight( scaleDeltaHeight( attributes.namedItem( "Height" ).nodeValue().toInt() ) );
    section->setPrintFrequency( attributes.namedItem( "PrintFrequency" ).nodeValue().toInt() );

    // Process the sections labels
    QDomNodeList children = report->childNodes();
    int childCount = children.length();

    // For each label, extract the attr list and add the new label
    // to the sections's label collection
    for ( int j = 0; j < childCount; j++ )
    {
        QDomNode child = children.item( j );
        if ( child.nodeType() == QDomNode::ElementNode )
        {
            if ( child.nodeName() == "Line" )
            {
                QDomNamedNodeMap attributes = child.attributes();
                MLineObject* line = new MLineObject();
                setLineAttributes( line, &attributes );
                section->addLine( line );
            }
            else if ( child.nodeName() == "Label" )
            {
                QDomNamedNodeMap attributes = child.attributes();
                MLabelObject* label = new MLabelObject();
                setLabelAttributes( label, &attributes );
                section->addLabel( label );
            }
            else if ( child.nodeName() == "Special" )
            {
                QDomNamedNodeMap attributes = child.attributes();
                MSpecialObject* field = new MSpecialObject();
                setSpecialAttributes( field, &attributes );
                section->addSpecialField( field );
            }
            else if ( child.nodeName() == "CalculatedField" )
            {
                QDomNamedNodeMap attributes = child.attributes();
                MCalcObject* field = new MCalcObject();
                setCalculatedFieldAttributes( field, &attributes );
                section->addCalculatedField( field );
            }
            else if ( child.nodeName() == "Field" )
            {
                QDomNamedNodeMap attributes = child.attributes();
                MFieldObject* field = new MFieldObject();
                setFieldAttributes( field, &attributes );
                section->addField( field );
            }
        }
    }
}

/** Sets the layout attributes for the detail headers and footers */
void MReportEngine::setDetMiscAttributes( MReportSection* section, QDomNode* report )
{
    // Get the attributes for the section
    QDomNamedNodeMap attributes = report->attributes();

    // Get the section attributes
    section->setLevel( attributes.namedItem( "Level" ).nodeValue().toInt() );

    // Set other section attributes
    setSectionAttributes( section, report );
}


/** Sets the layout attributes for the detail section */
void MReportEngine::setDetailAttributes( QDomNode* report )
{
    // Get the attributes for the detail section
    QDomNamedNodeMap attributes = report->attributes();

    // Get the report detail attributes
    MReportDetail *detail = new MReportDetail;
    int height = scaleDeltaHeight( attributes.namedItem( "Height" ).nodeValue().toInt() );
    heightOfDetails += height;
    detail->setHeight( height );
    detail->setLevel( attributes.namedItem( "Level" ).nodeValue().toInt() );
    detail->setRepeat( attributes.namedItem( "Repeat" ).nodeValue() == "true" );

    // Process the report detail labels
    QDomNodeList children = report->childNodes();
    int childCount = children.length();

    for ( int j = 0; j < childCount; j++ )
    {
        QDomNode child = children.item( j );
        if ( child.nodeType() == QDomNode::ElementNode )
        {
            if ( child.nodeName() == "Line" )
            {
                QDomNamedNodeMap attributes = child.attributes();
                MLineObject* line = new MLineObject();
                setLineAttributes( line, &attributes );
                detail->addLine( line );
            }
            else if ( child.nodeName() == "Label" )
            {
                QDomNamedNodeMap attributes = child.attributes();
                MLabelObject* label = new MLabelObject();
                setLabelAttributes( label, &attributes );
                detail->addLabel( label );
            }
            else if ( child.nodeName() == "Special" )
            {
                QDomNamedNodeMap attributes = child.attributes();
                MSpecialObject* field = new MSpecialObject();
                setSpecialAttributes( field, &attributes );
                detail->addSpecialField( field );
            }
            else if ( child.nodeName() == "Field" )
            {
                QDomNamedNodeMap attributes = child.attributes();
                MFieldObject* field = new MFieldObject();
                setFieldAttributes( field, &attributes );
                detail->addField( field );
            }
        }
    }
    // Append a newly created detail to the list
    details.append( detail );
}

/** Sets a line's layout attributes */
void MReportEngine::setLineAttributes( MLineObject* line, QDomNamedNodeMap* attr )
{
    line->setLine( scaleDeltaWidth( attr->namedItem( "X1" ).nodeValue().toInt() ),
                   scaleDeltaHeight( attr->namedItem( "Y1" ).nodeValue().toInt() ),
                   scaleDeltaWidth( attr->namedItem( "X2" ).nodeValue().toInt() ),
                   scaleDeltaHeight( attr->namedItem( "Y2" ).nodeValue().toInt() ) );

    QString tmp = attr->namedItem( "Color" ).nodeValue();

    line->setColor( tmp.left( tmp.find( "," ) ).toInt(),
                    tmp.mid( tmp.find( "," ) + 1, ( tmp.findRev( "," ) - tmp.find( "," ) ) - 1 ).toInt(),
                    tmp.right( tmp.length() - tmp.findRev( "," ) - 1 ).toInt() );

    line->setWidth( attr->namedItem( "Width" ).nodeValue().toInt() );
    line->setStyle( attr->namedItem( "Style" ).nodeValue().toInt() );
}

/** Sets a label's layout attributes */
void MReportEngine::setLabelAttributes( MLabelObject* label, QDomNamedNodeMap* attr )
{
    QString tmp;

    label->setText( attr->namedItem( "Text" ).nodeValue() );
    label->setGeometry( scaleDeltaWidth( attr->namedItem( "X" ).nodeValue().toInt() ),
                        scaleDeltaHeight( attr->namedItem( "Y" ).nodeValue().toInt() ),
                        scaleDeltaWidth( attr->namedItem( "Width" ).nodeValue().toInt() ),
                        scaleDeltaHeight( attr->namedItem( "Height" ).nodeValue().toInt() ) );

    tmp = attr->namedItem( "BackgroundColor" ).nodeValue();
    label->setBackgroundColor( tmp.left( tmp.find( "," ) ).toInt(),
                               tmp.mid( tmp.find( "," ) + 1, ( tmp.findRev( "," ) - tmp.find( "," ) ) - 1 ).toInt(),
                               tmp.right( tmp.length() - tmp.findRev( "," ) - 1 ).toInt() );

    tmp = attr->namedItem( "ForegroundColor" ).nodeValue();
    label->setForegroundColor( tmp.left( tmp.find( "," ) ).toInt(),
                               tmp.mid( tmp.find( "," ) + 1, ( tmp.findRev( "," ) - tmp.find( "," ) ) - 1 ).toInt(),
                               tmp.right( tmp.length() - tmp.findRev( "," ) - 1 ).toInt() );

    tmp = attr->namedItem( "BorderColor" ).nodeValue();
    label->setBorderColor( tmp.left( tmp.find( "," ) ).toInt(),
                           tmp.mid( tmp.find( "," ) + 1, ( tmp.findRev( "," ) - tmp.find( "," ) ) - 1 ).toInt(),
                           tmp.right( tmp.length() - tmp.findRev( "," ) - 1 ).toInt() );

    label->setBorderWidth( attr->namedItem( "BorderWidth" ).nodeValue().toInt() );
    label->setBorderStyle( attr->namedItem( "BorderStyle" ).nodeValue().toInt() );
    if ( !attr->namedItem( "DrawBottom" ).isNull() )
        label->setDrawBottom( attr->namedItem( "DrawBottom" ).nodeValue() == "true" );
    if ( !attr->namedItem( "DrawTop" ).isNull() )
        label->setDrawTop( attr->namedItem( "DrawTop" ).nodeValue() == "true" );
    if ( !attr->namedItem( "DrawLeft" ).isNull() )
        label->setDrawLeft( attr->namedItem( "DrawLeft" ).nodeValue() == "true" );
    if ( !attr->namedItem( "DrawRight" ).isNull() )
        label->setDrawRight( attr->namedItem( "DrawRight" ).nodeValue() == "true" );
    label->setFont( attr->namedItem( "FontFamily" ).nodeValue(),
                    attr->namedItem( "FontSize" ).nodeValue().toInt(),
                    attr->namedItem( "FontWeight" ).nodeValue().toInt(),
                    ( attr->namedItem( "FontItalic" ).nodeValue().toInt() == 0 ? false : true ) );
    label->setHorizontalAlignment( attr->namedItem( "HAlignment" ).nodeValue().toInt() );
    label->setVerticalAlignment( attr->namedItem( "VAlignment" ).nodeValue().toInt() );
    label->setWordWrap( attr->namedItem( "WordWrap" ).nodeValue().toInt() == 0 ? false : true );
}

/** Sets a special field's layout attributes */
void MReportEngine::setSpecialAttributes( MSpecialObject* field, QDomNamedNodeMap* attr )
{
    field->setType( attr->namedItem( "Type" ).nodeValue().toInt() );
    field->setDateFormat( attr->namedItem( "DateFormat" ).nodeValue().toInt() );

    setLabelAttributes( ( MLabelObject * ) field, attr );
}

/** Sets a field's layout attributes */
void MReportEngine::setFieldAttributes( MFieldObject* field, QDomNamedNodeMap* attr )
{
    field->setFieldName( attr->namedItem( "Field" ).nodeValue() );
    field->setDataType( attr->namedItem( "DataType" ).nodeValue().toInt() );
    field->setDateFormat( attr->namedItem( "DateFormat" ).nodeValue().toInt() );
    field->setPrecision( attr->namedItem( "Precision" ).nodeValue().toInt() );
    field->setCurrency( attr->namedItem( "Currency" ).nodeValue().toInt() );
    field->setCommaSeparator( attr->namedItem( "CommaSeparator" ).nodeValue().toInt() );
    field->setInputMask( attr->namedItem( "InputMask" ).nodeValue() );

    QString tmp = attr->namedItem( "NegValueColor" ).nodeValue();

    field->setNegValueColor( tmp.left( tmp.find( "," ) ).toInt(),
                             tmp.mid( tmp.find( "," ) + 1, ( tmp.findRev( "," ) - tmp.find( "," ) ) - 1 ).toInt(),
                             tmp.right( tmp.length() - tmp.findRev( "," ) - 1 ).toInt() );

    setLabelAttributes( ( MLabelObject * ) field, attr );
}

/** Sets a calculated field's layout attributes */
void MReportEngine::setCalculatedFieldAttributes( MCalcObject* field, QDomNamedNodeMap* attr )
{
    field->setCalculationType( attr->namedItem( "CalculationType" ).nodeValue().toInt() );
    setFieldAttributes( ( MFieldObject * ) field, attr );
}

/** Copies member data from one object to another.
  Used by the copy constructor and assignment operator */
void MReportEngine::copy( const MReportEngine* mReportEngine )
{
    // Copy document data
    m_refCount = 1;
    rd = mReportEngine->rd;
    rt = mReportEngine->rt;

    // Copy page metrics
    pageSize = mReportEngine->pageSize;
    pageOrientation = mReportEngine->pageOrientation;
    topMargin = mReportEngine->topMargin;
    bottomMargin = mReportEngine->bottomMargin;
    leftMargin = mReportEngine->leftMargin;
    rightMargin = mReportEngine->rightMargin;
    pageWidth = mReportEngine->pageWidth;
    pageHeight = mReportEngine->pageHeight;
    heightOfDetails = mReportEngine->heightOfDetails;

    // Copy the report header
    rHeader = mReportEngine->rHeader;
    // Copy the page header
    pHeader = mReportEngine->pHeader;
    // Copy the detail sections
    MReportDetail *detail;
    Q3PtrList<MReportDetail> temp = mReportEngine->details;
    temp.setAutoDelete( false );
    for ( detail = temp.first(); detail; detail = temp.next() )
    {
        MReportDetail * new_detail = new MReportDetail;
        *new_detail = *detail;
        details.append( new_detail );
    }
    // Copy the page footer
    pFooter = mReportEngine->pFooter;
    // Copy the report footer
    rFooter = mReportEngine->rFooter;

    // Copy the rendering state
    currY = mReportEngine->currY;
    currHeight = mReportEngine->currHeight;
    currPage = mReportEngine->currPage;
    currDate = mReportEngine->currDate;
    cancelRender = mReportEngine->cancelRender;

    // Copy grand totals list
    grandTotal = mReportEngine->grandTotal;
    m_pageCollection = mReportEngine->m_pageCollection;
    m_needRegeneration = mReportEngine->m_needRegeneration;
    if ( m_pageCollection )
        m_pageCollection->addRef();
}

}

#include "mreportengine.moc"
