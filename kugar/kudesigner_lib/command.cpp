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
#include "command.h"

#include <klocale.h>
#include <kdebug.h>

//#include "kudesigner_doc.h"

#include <koproperty/editor.h>
#include <koproperty/property.h>

#include "view.h"
#include "canvas.h"

#include "field.h"
#include "calcfield.h"
#include "label.h"
#include "line.h"
#include "specialfield.h"

#include "kugartemplate.h"
#include "reportheader.h"
#include "reportfooter.h"
#include "pageheader.h"
#include "pagefooter.h"
#include "detailheader.h"
#include "detailfooter.h"
#include "detail.h"


namespace Kudesigner
{

//AddDetailFooterCommand

AddDetailFooterCommand::AddDetailFooterCommand( int level, Canvas *doc )
        : KNamedCommand( QObject::tr( "Insert Detail Footer Section" ) ), m_level( level ), m_doc( doc )
{}

void AddDetailFooterCommand::execute()
{
    m_section = new DetailFooter( m_doc->kugarTemplate() ->props[ "LeftMargin" ].value().toInt(),
                                  0, m_doc->kugarTemplate() ->width() - m_doc->kugarTemplate() ->props[ "RightMargin" ].value().toInt() -
                                  m_doc->kugarTemplate() ->props[ "LeftMargin" ].value().toInt(),
                                  50, m_level, m_doc );
    m_section->props[ "Level" ].setValue( m_level );
    m_doc->kugarTemplate() ->details[ m_level ].first.second = m_section;
    m_doc->kugarTemplate() ->arrangeSections();
    m_doc->setStructureModified();
}

void AddDetailFooterCommand::unexecute()
{
    m_doc->kugarTemplate() ->removeReportItem( m_section );
    m_doc->setStructureModified();
}

//AddDetailCommand

AddDetailCommand::AddDetailCommand( int level, Canvas *doc )
        : KNamedCommand( QObject::tr( "Insert Detail Section" ) ), m_level( level ), m_doc( doc )
{}

void AddDetailCommand::execute()
{
    m_section = new Detail( m_doc->kugarTemplate() ->props[ "LeftMargin" ].value().toInt(),
                            0, m_doc->kugarTemplate() ->width() - m_doc->kugarTemplate() ->props[ "RightMargin" ].value().toInt() -
                            m_doc->kugarTemplate() ->props[ "LeftMargin" ].value().toInt(),
                            50, m_level, m_doc );
    m_section->props[ "Level" ].setValue( m_level );
    m_doc->kugarTemplate() ->details[ m_level ].second = m_section;
    m_doc->kugarTemplate() ->arrangeSections();
    m_doc->kugarTemplate() ->detailsCount++;
    m_doc->setStructureModified();
}

void AddDetailCommand::unexecute()
{
    m_doc->kugarTemplate() ->removeReportItem( m_section );
    m_doc->setStructureModified();
}

//AddDetailHeaderCommand
AddDetailHeaderCommand::AddDetailHeaderCommand( int level, Canvas *doc )
        : KNamedCommand( QObject::tr( "Insert Detail Header Section" ) ), m_level( level ), m_doc( doc )
{}

void AddDetailHeaderCommand::execute()
{
    m_section = new DetailHeader( m_doc->kugarTemplate() ->props[ "LeftMargin" ].value().toInt(),
                                  0, m_doc->kugarTemplate() ->width() - m_doc->kugarTemplate() ->props[ "RightMargin" ].value().toInt() -
                                  m_doc->kugarTemplate() ->props[ "LeftMargin" ].value().toInt(),
                                  50, m_level, m_doc );
    m_section->props[ "Level" ].setValue( m_level );
    m_doc->kugarTemplate() ->details[ m_level ].first.first = m_section;
    m_doc->kugarTemplate() ->arrangeSections();
    m_doc->setStructureModified();
}

void AddDetailHeaderCommand::unexecute()
{
    m_doc->kugarTemplate() ->removeReportItem( m_section );
    m_doc->setStructureModified();
}

//AddPageFooterCommand

AddPageFooterCommand::AddPageFooterCommand( Canvas *doc )
        : KNamedCommand( QObject::tr( "Insert Page Footer Section" ) ), m_doc( doc )
{}

void AddPageFooterCommand::execute()
{
    m_section = new PageFooter( m_doc->kugarTemplate() ->props[ "LeftMargin" ].value().toInt(),
                                0, m_doc->kugarTemplate() ->width() - m_doc->kugarTemplate() ->props[ "RightMargin" ].value().toInt() -
                                m_doc->kugarTemplate() ->props[ "LeftMargin" ].value().toInt(),
                                50, m_doc );
    m_doc->kugarTemplate() ->pageFooter = m_section;
    m_doc->kugarTemplate() ->arrangeSections();
    m_doc->setStructureModified();
}

void AddPageFooterCommand::unexecute()
{
    m_doc->kugarTemplate() ->removeReportItem( m_section );
    m_doc->setStructureModified();
}

//AddPageHeaderCommand

AddPageHeaderCommand::AddPageHeaderCommand( Canvas *doc )
        : KNamedCommand( QObject::tr( "Insert Page Header Section" ) ), m_doc( doc )
{}

void AddPageHeaderCommand::execute()
{
    m_section = new PageHeader( m_doc->kugarTemplate() ->props[ "LeftMargin" ].value().toInt(),
                                0, m_doc->kugarTemplate() ->width() - m_doc->kugarTemplate() ->props[ "RightMargin" ].value().toInt() -
                                m_doc->kugarTemplate() ->props[ "LeftMargin" ].value().toInt(),
                                50, m_doc );
    m_doc->kugarTemplate() ->pageHeader = m_section;
    m_doc->kugarTemplate() ->arrangeSections();
    m_doc->setStructureModified();
}

void AddPageHeaderCommand::unexecute()
{
    m_doc->kugarTemplate() ->removeReportItem( m_section );
    m_doc->setStructureModified();
}

//AddReportFooterCommand

AddReportFooterCommand::AddReportFooterCommand( Canvas *doc )
        : KNamedCommand( QObject::tr( "Insert Report Footer Section" ) ), m_doc( doc )
{}

void AddReportFooterCommand::execute()
{
    m_section = new ReportFooter( m_doc->kugarTemplate() ->props[ "LeftMargin" ].value().toInt(),
                                  0, m_doc->kugarTemplate() ->width() - m_doc->kugarTemplate() ->props[ "RightMargin" ].value().toInt() -
                                  m_doc->kugarTemplate() ->props[ "LeftMargin" ].value().toInt(),
                                  50, m_doc );
    m_doc->kugarTemplate() ->reportFooter = m_section;
    m_doc->kugarTemplate() ->arrangeSections();
    m_doc->setStructureModified();
}

void AddReportFooterCommand::unexecute()
{
    m_doc->kugarTemplate() ->removeReportItem( m_section );
    m_doc->setStructureModified();
}

//AddReportHeaderCommand

AddReportHeaderCommand::AddReportHeaderCommand( Canvas *doc )
        : KNamedCommand( QObject::tr( "Insert Report Header Section" ) ), m_doc( doc )
{}

void AddReportHeaderCommand::execute()
{
    m_section = new ReportHeader( m_doc->kugarTemplate() ->props[ "LeftMargin" ].value().toInt(),
                                  0, m_doc->kugarTemplate() ->width() - m_doc->kugarTemplate() ->props[ "RightMargin" ].value().toInt() -
                                  m_doc->kugarTemplate() ->props[ "LeftMargin" ].value().toInt(),
                                  50, m_doc );
    m_doc->kugarTemplate() ->reportHeader = m_section;
    m_doc->kugarTemplate() ->arrangeSections();
    m_doc->setStructureModified();
}

void AddReportHeaderCommand::unexecute()
{
    m_doc->kugarTemplate() ->removeReportItem( m_section );
    m_doc->setStructureModified();
}

//AddReportItemCommand

AddReportItemCommand::AddReportItemCommand( Canvas *doc, View *rc, int x, int y, RttiValues section, int sectionLevel ) :
        KNamedCommand( QObject::tr( "Insert Report Item" ) ), m_doc( doc ), m_rc( rc ), m_x( x ), m_y( y ), m_section( section ), m_sectionLevel( sectionLevel )
{
    m_rtti = m_rc->itemToInsert;
    setName( "Insert " + Kudesigner::rttiName( m_rc->itemToInsert ) );
}

void AddReportItemCommand::execute()
{
    //    kdDebug() << "Execute: rtti = " << m_rtti << endl;
    switch ( m_rtti )
    {
    case Rtti_Label:
        m_item = new Label( 0, 0, DefaultItemWidth, DefaultItemHeight, m_doc );
        break;
    case Rtti_Field:
        m_item = new Field( 0, 0, DefaultItemWidth, DefaultItemHeight, m_doc );
        break;
    case Rtti_Special:
        m_item = new SpecialField( 0, 0, DefaultItemWidth, DefaultItemHeight, m_doc );
        break;
    case Rtti_Calculated:
        m_item = new CalculatedField( 0, 0, DefaultItemWidth, DefaultItemHeight, m_doc );
        break;
    case Rtti_Line:
        m_item = new Line( 0, 0, DefaultItemWidth, DefaultItemHeight, m_doc );
        break;
    default:
        m_item = 0;
        return ;
    }

    m_item->setX( m_x );
    m_item->setY( m_y );
    //    kdDebug() << "Execute: item created" << endl;
    m_item->setSection( m_doc->kugarTemplate() ->band( m_section, m_sectionLevel ) );
    m_item->updateGeomProps();

    m_doc->selectItem( m_item, false );

    m_item->show();
    m_doc->kugarTemplate() ->band( m_section, m_sectionLevel ) ->items.append( m_item );
    m_doc->setStructureModified();
}

void AddReportItemCommand::unexecute()
{
    if ( m_item )
    {
        m_doc->unselectItem( m_item );
        m_doc->kugarTemplate() ->removeReportItem( m_item );
        m_doc->setStructureModified();
    }
}

DeleteReportItemsCommand::DeleteReportItemsCommand( Canvas * doc, QValueList< Box* > & items )
        : KNamedCommand( QObject::tr( "Delete Report Item(s)" ) ), m_doc( doc ), m_items( items )
{}

void DeleteReportItemsCommand::execute( )
{
    m_doc->unselectAll();

    for ( QValueList< Box* >::iterator it = m_items.begin(); it != m_items.end(); ++it )
    {
        Box *b = *it;
        m_doc->kugarTemplate() ->removeReportItem( b );
    }

    m_doc->setStructureModified();
}

void DeleteReportItemsCommand::unexecute( )
{
    /*    Box *b;
        for (b = m_items.first(); b; b = m_items.next())
        {
            b->show();*/
    //        m_doc->kugarTemplate()->removeReportItem( b );
    //     }

    m_doc->setStructureModified();
}

}
