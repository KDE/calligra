/* This file is part of the KDE project
 Copyright (C) 2002-2004 Alexander Dymo <adymo@mksat.net>

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
#include "canvas.h"

#include <qdom.h>
#include <qstringlist.h>

#include <koproperty/property.h>

#include "box.h"
#include "kugartemplate.h"

#include "reportheader.h"
#include "reportfooter.h"
#include "pageheader.h"
#include "pagefooter.h"
#include "detailheader.h"
#include "detailfooter.h"
#include "detail.h"

#include "reportitem.h"
#include "calcfield.h"
#include "field.h"
#include "label.h"
#include "line.h"
#include "specialfield.h"

#include "propertyserializer.h"

#include <kdebug.h>

namespace Kudesigner
{

Canvas::Canvas( int w, int h /*, Canvas *doc*/ )
        : Q3Canvas( w, h ), m_plugin( 0 ), m_kugarTemplate( 0 )  //, m_doc(doc)
{
}

Canvas::~Canvas()
{
    delete m_kugarTemplate;
}

void Canvas::scaleCanvas( int scale )
{
    resize( width() * scale, height() * scale );

    //scale all child items if they are textbox's
    /*    QCanvasItemList l=this->allItems();
        for (QCanvasItemList::Iterator it=l.begin(); it!=l.end(); ++it)
        {
            if ((*it)->rtti() == Rtti_TextBox)
            {
                CanvasTextBox* b = (CanvasTextBox*)(*it);
                b->scale(scale);
            }
        }*/
}

void Canvas::drawForeground ( QPainter & painter, const QRect & /*clip*/ )
{
    //kDebug()<<"Canvas::drawForeGround"<<endl;
    //     qWarning("drawForeground ");
    for ( BoxList::iterator it = selected.begin(); it != selected.end(); ++it )
    {
        //         qWarning("sel:");
        ( *it ) ->drawHolders( painter );
    }
}

KuDesignerPlugin * Canvas::plugin( )
{
    return m_plugin;
}

void Canvas::setPlugin( KuDesignerPlugin * plugin )
{
    m_plugin = plugin;
}

void Canvas::unselectAll()
{
    for ( BoxList::iterator it = selected.begin(); it != selected.end(); ++it )
    {
        Box *b = *it;
        b->setSelected( false );
        setChanged( b->rect() );
    }

    selected.clear();
    update();
}

void Canvas::selectAll()
{
    for ( Q3CanvasItemList::Iterator it = allItems().begin(); it != allItems().end(); ++it )
    {
        if ( ( ( *it ) ->rtti() > 2001 ) && ( ( *it ) ->isVisible() ) )
            selectItem( static_cast<Box*>( *it ) );
    }
}

void Canvas::selectItem( Box *it, bool addToSelection )
{
    if ( !it->isVisible() )
        return ;
    if ( !addToSelection )
        unselectAll();
    selected.append( it );
    it->setSelected( true );

    emit itemSelected();
    /*    if (!selectionStarted)
            finishSelection();*/
}

void Canvas::unselectItem( Box *it )
{
    selected.remove( it );
    it->setSelected( false );
}

KugarTemplate *Canvas::kugarTemplate()
{
    return m_kugarTemplate;
}

void Canvas::setKugarTemplate( KugarTemplate *kugarTemplate )
{
    m_kugarTemplate = kugarTemplate;
}

bool Canvas::loadXML( const QDomNode &report )
{
    QDomNamedNodeMap attributes = report.attributes();

    //creating KugarTemplate object
    KugarTemplate *templ = new KugarTemplate( 0, 0, width(), height(), this );
    templ->show();
    templ->props[ "PageSize" ].setValue( attributes.namedItem( "PageSize" ).nodeValue() );
    templ->props[ "PageOrientation" ].setValue( attributes.namedItem( "PageOrientation" ).nodeValue() );
    templ->props[ "TopMargin" ].setValue( attributes.namedItem( "TopMargin" ).nodeValue().toInt() );
    templ->props[ "BottomMargin" ].setValue( attributes.namedItem( "BottomMargin" ).nodeValue().toInt() );
    templ->props[ "LeftMargin" ].setValue( attributes.namedItem( "LeftMargin" ).nodeValue().toInt() );
    templ->props[ "RightMargin" ].setValue( attributes.namedItem( "RightMargin" ).nodeValue().toInt() );

    // Get all the child report elements
    QDomNodeList children = report.childNodes();
    int childCount = children.length();
    for ( int j = 0; j < childCount; j++ )
    {
        QDomNode child = children.item( j );

        if ( child.nodeType() == QDomNode::ElementNode )
        {
            if ( child.nodeName() == "ReportHeader" )
                setReportHeaderAttributes( &child );
            else if ( child.nodeName() == "PageHeader" )
                setPageHeaderAttributes( &child );
            else if ( child.nodeName() == "DetailHeader" )
                setDetailHeaderAttributes( &child );
            else if ( child.nodeName() == "Detail" )
            {
                templ->detailsCount++;
                setDetailAttributes( &child );
            }
            else if ( child.nodeName() == "DetailFooter" )
                setDetailFooterAttributes( &child );
            else if ( child.nodeName() == "PageFooter" )
                setPageFooterAttributes( &child );
            else if ( child.nodeName() == "ReportFooter" )
                setReportFooterAttributes( &child );
        }
    }
    templ->arrangeSections( false );
    Q3CanvasItemList l = allItems();
    for ( Q3CanvasItemList::Iterator it = l.begin(); it != l.end(); ++it )
    {
        ( *it ) ->show();
    }
    update();

    return true;
}

void Canvas::setReportHeaderAttributes( QDomNode *node )
{
    QDomNamedNodeMap attributes = node->attributes();

    ReportHeader *rh = new ReportHeader( kugarTemplate() ->props[ "LeftMargin" ].value().toInt(),
                                         0, kugarTemplate() ->width() - kugarTemplate() ->props[ "RightMargin" ].value().toInt() -
                                         kugarTemplate() ->props[ "LeftMargin" ].value().toInt(),
                                         attributes.namedItem( "Height" ).nodeValue().toInt(), this );
    rh->props[ "Height" ].setValue( attributes.namedItem( "Height" ).nodeValue().toInt() );
    kugarTemplate() ->reportHeader = rh;
    addReportItems( node, rh );
}

void Canvas::setReportFooterAttributes( QDomNode *node )
{
    QDomNamedNodeMap attributes = node->attributes();

    ReportFooter *rf = new ReportFooter( kugarTemplate() ->props[ "LeftMargin" ].value().toInt(),
                                         0, kugarTemplate() ->width() - kugarTemplate() ->props[ "RightMargin" ].value().toInt() -
                                         kugarTemplate() ->props[ "LeftMargin" ].value().toInt(),
                                         attributes.namedItem( "Height" ).nodeValue().toInt(), this );
    rf->props[ "Height" ].setValue( attributes.namedItem( "Height" ).nodeValue().toInt() );
    kugarTemplate() ->reportFooter = rf;
    addReportItems( node, rf );
}

void Canvas::setPageHeaderAttributes( QDomNode *node )
{
    QDomNamedNodeMap attributes = node->attributes();

    PageHeader *ph = new PageHeader( kugarTemplate() ->props[ "LeftMargin" ].value().toInt(),
                                     0, kugarTemplate() ->width() - kugarTemplate() ->props[ "RightMargin" ].value().toInt() -
                                     kugarTemplate() ->props[ "LeftMargin" ].value().toInt(),
                                     attributes.namedItem( "Height" ).nodeValue().toInt(), this );
    ph->props[ "Height" ].setValue( attributes.namedItem( "Height" ).nodeValue().toInt() );
    kugarTemplate() ->pageHeader = ph;
    addReportItems( node, ph );
}

void Canvas::setPageFooterAttributes( QDomNode *node )
{
    QDomNamedNodeMap attributes = node->attributes();

    PageFooter *pf = new PageFooter( kugarTemplate() ->props[ "LeftMargin" ].value().toInt(),
                                     0, kugarTemplate() ->width() - kugarTemplate() ->props[ "RightMargin" ].value().toInt() -
                                     kugarTemplate() ->props[ "LeftMargin" ].value().toInt(),
                                     attributes.namedItem( "Height" ).nodeValue().toInt(), this );
    pf->props[ "Height" ].setValue( attributes.namedItem( "Height" ).nodeValue().toInt() );
    kugarTemplate() ->pageFooter = pf;
    addReportItems( node, pf );
}

void Canvas::setDetailHeaderAttributes( QDomNode *node )
{
    QDomNamedNodeMap attributes = node->attributes();

    DetailHeader *dh = new DetailHeader( kugarTemplate() ->props[ "LeftMargin" ].value().toInt(),
                                         0, kugarTemplate() ->width() - kugarTemplate() ->props[ "RightMargin" ].value().toInt() -
                                         kugarTemplate() ->props[ "LeftMargin" ].value().toInt(),
                                         attributes.namedItem( "Height" ).nodeValue().toInt(),
                                         attributes.namedItem( "Level" ).nodeValue().toInt(), this );
    dh->props[ "Level" ].setValue( attributes.namedItem( "Level" ).nodeValue().toInt() );
    dh->props[ "Height" ].setValue( attributes.namedItem( "Height" ).nodeValue().toInt() );
    kugarTemplate() ->details[ attributes.namedItem( "Level" ).nodeValue().toInt() ].first.first = dh;
    addReportItems( node, dh );
}

void Canvas::setDetailAttributes( QDomNode *node )
{
    QDomNamedNodeMap attributes = node->attributes();

    Detail *d = new Detail( kugarTemplate() ->props[ "LeftMargin" ].value().toInt(),
                            0, kugarTemplate() ->width() - kugarTemplate() ->props[ "RightMargin" ].value().toInt() -
                            kugarTemplate() ->props[ "LeftMargin" ].value().toInt(),
                            attributes.namedItem( "Height" ).nodeValue().toInt(),
                            attributes.namedItem( "Level" ).nodeValue().toInt(), this );
    d->props[ "Level" ].setValue( attributes.namedItem( "Level" ).nodeValue().toInt() );
    d->props[ "Height" ].setValue( attributes.namedItem( "Height" ).nodeValue().toInt() );
    d->props[ "Repeat" ].setValue( QVariant( attributes.namedItem( "Repeat" ).nodeValue() == "true", 3 ) );
    kugarTemplate() ->details[ attributes.namedItem( "Level" ).nodeValue().toInt() ].second = d;
    addReportItems( node, d );
}

void Canvas::setDetailFooterAttributes( QDomNode *node )
{
    QDomNamedNodeMap attributes = node->attributes();

    DetailFooter *df = new DetailFooter( kugarTemplate() ->props[ "LeftMargin" ].value().toInt(),
                                         0, kugarTemplate() ->width() - kugarTemplate() ->props[ "RightMargin" ].value().toInt() -
                                         kugarTemplate() ->props[ "LeftMargin" ].value().toInt(),
                                         attributes.namedItem( "Height" ).nodeValue().toInt(),
                                         attributes.namedItem( "Level" ).nodeValue().toInt(), this );
    df->props[ "Level" ].setValue( attributes.namedItem( "Level" ).nodeValue().toInt() );
    df->props[ "Height" ].setValue( attributes.namedItem( "Height" ).nodeValue().toInt() );
    kugarTemplate() ->details[ attributes.namedItem( "Level" ).nodeValue().toInt() ].first.second = df;
    addReportItems( node, df );
}


void Canvas::addReportItems( QDomNode *node, Band *section )
{
    QDomNodeList children = node->childNodes();
    int childCount = children.length();

    for ( int j = 0; j < childCount; j++ )
    {
        QDomNode child = children.item( j );
        if ( child.nodeType() == QDomNode::ElementNode )
        {
            if ( child.nodeName() == "Line" )
            {
                Line * line = new Line( 0, 0, 50, 20, this );
                section->items.append( line );

                setReportItemAttributes( &child, line );
                line->setSectionUndestructive( section );
                //line->show();
            }
            else if ( child.nodeName() == "Label" )
            {
                Label * label = new Label( 0, 0, 50, 20, this );
                section->items.append( label );
                setReportItemAttributes( &child, label );
                label->setSectionUndestructive( section );
                //label->show();
            }
            else if ( child.nodeName() == "Special" )
            {
                SpecialField * special = new SpecialField( 0, 0, 50, 20, this );
                section->items.append( special );
                setReportItemAttributes( &child, special );
                special->setSectionUndestructive( section );
                //special->show();
            }
            else if ( child.nodeName() == "Field" )
            {
                Field * field = new Field( 0, 0, 50, 20, this );
                section->items.append( field );
                setReportItemAttributes( &child, field );
                field->setSectionUndestructive( section );
                //field->show();
            }
            else if ( child.nodeName() == "CalculatedField" )
            {
                CalculatedField * calcField = new CalculatedField( 0, 0, 50, 20, this );
                section->items.append( calcField );
                setReportItemAttributes( &child, calcField );
                calcField->setSectionUndestructive( section );
                //calcField->show();
            }
        }
    }
}

void Canvas::setReportItemAttributes( QDomNode *node, ReportItem *item )
{
    QDomNamedNodeMap attributes = node->attributes();

    for ( unsigned int i = 0; i < attributes.count(); i++ )
    {
        QString propertyName = attributes.item( i ).nodeName();
        QString propertyValue = attributes.item( i ).nodeValue();

        item->props[ propertyName.utf8() ].setValue(
            PropertySerializer::fromString( &item->props[ propertyName.utf8() ], propertyValue ) );
    }
}

void Canvas::changed()
{
    for ( Kudesigner::BoxList::iterator it = selected.begin();
            it != selected.end(); ++it )
    {
        Kudesigner::Box *b = *it;
        b->hide();
        b->show();
        if ( ( b->rtti() >= 1800 ) && ( b->rtti() < 2000 ) )
            kugarTemplate() ->arrangeSections();
    }
}

}

#include "canvas.moc"
