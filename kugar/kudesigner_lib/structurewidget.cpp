/***************************************************************************
*   Copyright (C) 2005 by Alexander Dymo                                  *
*   adymo@kdevelop.org                                                    *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU Library General Public License as       *
*   published by the Free Software Foundation; either version 2 of the    *
*   License, or (at your option) any later version.                       *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU Library General Public     *
*   License along with this program; if not, write to the                 *
*   Free Software Foundation, Inc.,                                       *
*   51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.             *
***************************************************************************/
#include "structurewidget.h"

#include <qpainter.h>
#include <q3canvas.h>
//Added by qt3to4:
#include <Q3ValueList>

#include <koproperty/property.h>

#include "canvas.h"
#include "kugartemplate.h"
#include "reportheader.h"
#include "reportfooter.h"
#include "pageheader.h"
#include "pagefooter.h"
#include "detail.h"
#include "detailfooter.h"
#include "detailheader.h"

#include <kdebug.h>

namespace Kudesigner
{

using namespace KoProperty;

class StructureItem: public K3ListViewItem
{
public:
    StructureItem( K3ListView *parent, const QString &name )
            : K3ListViewItem( parent, name ), m_bold( false )
    {}
    StructureItem( K3ListViewItem *parent, const QString &name )
            : K3ListViewItem( parent, name ), m_bold( false )
    {}
    void setBold( bool b )
    {
        m_bold = b;
    }
    bool isBold() const
    {
        return m_bold;
    }

    virtual void paintCell( QPainter *p, const QColorGroup &cg, int column, int width, int align )
    {
        if ( m_bold )
        {
            QFont f = p->font();
            f.setWeight( 75 );
            p->setFont( f );
        }
        K3ListViewItem::paintCell( p, cg, column, width, align );
    }

private:
    bool m_bold;
};

StructureWidget::StructureWidget( QWidget* parent, const char* name )
        : K3ListView( parent )
{
	setObjectName(name);
    setFullWidth( true );
    addColumn( tr( "Report Structure" ) );
    setSorting( -1 );
    connect( this, SIGNAL( clicked( Q3ListViewItem* ) ), this, SLOT( selectItem( Q3ListViewItem* ) ) );
}

void StructureWidget::refresh()
{
    if ( !m_doc )
        return ;
    clear();
    m_selected.clear();

    StructureItem *root = new StructureItem( this, tr( "Report Template" ) );
    m_items[ m_doc->kugarTemplate() ] = root;
    root->setOpen( true );

    refreshSection( m_doc->kugarTemplate() ->reportFooter, root );
    refreshSection( m_doc->kugarTemplate() ->pageFooter, root );

    for ( std::map<int, DetailBand>::iterator it =
                m_doc->kugarTemplate() ->details.begin();
            it != m_doc->kugarTemplate() ->details.end(); ++it )
    {
        int level = ( *it ).first;
        refreshSection( ( *it ).second.first.second, root, level );
        refreshSection( ( *it ).second.second, root, level );
        refreshSection( ( *it ).second.first.first, root, level );
    }

    refreshSection( m_doc->kugarTemplate() ->pageHeader, root );
    refreshSection( m_doc->kugarTemplate() ->reportHeader, root );
}

void StructureWidget::refreshSection( Kudesigner::Band *section, StructureItem *root, int level )
{
    if ( !section )
        return ;
    QString name;
    switch ( section->rtti() )
    {
    case Kudesigner::Rtti_ReportHeader:
        name = tr( "Report Header" );
        break;
    case Kudesigner::Rtti_ReportFooter:
        name = tr( "Report Footer" );
        break;
    case Kudesigner::Rtti_PageFooter:
        name = tr( "Page Footer" );
        break;
    case Kudesigner::Rtti_PageHeader:
        name = tr( "Page Header" );
        break;
    case Kudesigner::Rtti_Detail:
        name = tr( "Detail" );
        break;
    case Kudesigner::Rtti_DetailHeader:
        name = tr( "Detail Header" );
        break;
    case Kudesigner::Rtti_DetailFooter:
        name = tr( "Detail Footer" );
        break;
    }
    if ( level > 0 )
        name += tr( " (level %1)" ).arg( level );
    StructureItem *item = new StructureItem( root, name );
    m_items[ section ] = item;

    refreshSectionContents( section, item );
}

void StructureWidget::refreshSectionContents( Kudesigner::Band *section, StructureItem *root )
{
    if ( !section )
        return ;

    for ( Q3CanvasItemList::iterator it = section->items.begin(); it != section->items.end(); ++it )
    {
        Kudesigner::Box *box = static_cast<Kudesigner::Box*>( *it );
        if ( !box )
            continue;

        QString name = tr( "<unknown>" );
        int idx;
        switch ( box->rtti() )
        {
        case Kudesigner::Rtti_Label:
            name = tr( "Label: %1" ).arg( box->props[ "Text" ].value().toString() );
            break;
        case Kudesigner::Rtti_Field:
            name = tr( "Field: %1" ).arg( box->props[ "Field" ].value().toString() );
            break;
        case Kudesigner::Rtti_Calculated:
            name = tr( "Calculated Field: %1" ).arg( box->props[ "Field" ].value().toString() );
            break;
        case Kudesigner::Rtti_Special:
            idx = box->props[ "Type" ].listData()->keys.findIndex(
                      box->props[ "Type" ].value().toInt() );
            name = tr( "Special Field: %1" ).arg( box->props[ "Type" ].listData()->keys[ idx ].toString() );
            break;
        case Kudesigner::Rtti_Line:
            name = tr( "Line" );
            break;
        }

        StructureItem *item = new StructureItem( root, name );
        m_items[ box ] = item;
    }
}

void StructureWidget::selectionMade()
{
    m_selected.clear();
    BoxList sel = m_doc->selected;
    for ( BoxList::iterator it = sel.begin(); it != sel.end(); ++it )
    {
        if ( m_items.contains( *it ) )
        {
            StructureItem * item = static_cast<StructureItem*>( m_items[ *it ] );
            item->setBold( true );
            item->repaint();
            m_selected.append( item );
        }
    }
}

void StructureWidget::selectionClear()
{
    for ( Q3ValueList<StructureItem*>::iterator it = m_selected.begin(); it != m_selected.end(); ++it )
    {
        if ( ( *it ) == 0 )
            continue;
        ( *it ) ->setBold( false );
        ( *it ) ->repaint();
    }
    m_selected.clear();
}

void StructureWidget::selectItem( Q3ListViewItem *item )
{
    if ( !item )
        return ;
    int idx = m_items.values().findIndex( static_cast<StructureItem*>( item ) );
    if ( idx == -1 )
        return ;
    Kudesigner::Box *box = m_items.keys() [ idx ];
    if ( box )
        m_doc->selectItem( box, false );
}

void StructureWidget::setDocument( Kudesigner::Canvas *doc )
{
    m_doc = doc;
    m_items.clear();
}

}

#include "structurewidget.moc"
