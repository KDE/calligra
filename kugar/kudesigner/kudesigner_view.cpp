/* This file is part of the KDE project
  Copyright (C) 2002-2004 Alexander Dymo <cloudtemple@mksat.net>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MEm_viewHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/
#include "kudesigner_view.h"
#include "kudesigner_factory.h"
#include "kudesigner_doc.h"

#include <map>

#include <qpainter.h>
#include <qiconset.h>
#include <qinputdialog.h>
#include <qevent.h>
#include <qmainwindow.h>
#include <qaction.h>
#include <qlayout.h>
#include <qdockwindow.h>
#include <qmenubar.h>
#include <qmessagebox.h>
#include <qspinbox.h>
#include <qlabel.h>

#include <kaction.h>
#include <kstdaction.h>
#include <klocale.h>
#include <kdebug.h>

#include <KoMainWindow.h>

#include <commdefs.h>
#include <view.h>
#include <structurewidget.h>
#include <canvas.h>
#include <command.h>

#include <field.h>
#include <calcfield.h>
#include <label.h>
#include <line.h>
#include <specialfield.h>

#include <kugartemplate.h>
#include <reportheader.h>
#include <reportfooter.h>
#include <pageheader.h>
#include <pagefooter.h>
#include <detailheader.h>
#include <detailfooter.h>
#include <detail.h>

#include <koproperty/editor.h>
#include <koproperty/property.h>

using namespace Kudesigner;

KudesignerView::KudesignerView( KudesignerDoc* part, QWidget* parent, const char* name )
        : KoView( part, parent, name ), m_propertyEditor( 0 ), m_doc( part )
{
    setInstance( KudesignerFactory::global() );
    if ( !part->isReadWrite() )  // readonly case, e.g. when embedded into konqueror
        setXMLFile( "kudesigner_readonly.rc" ); // simplified GUI
    else
        setXMLFile( "kudesignerui.rc" );

    QVBoxLayout *l = new QVBoxLayout( this, 0, 0 );
    m_view = new Kudesigner::View( part->canvas(), this );
    if ( part->plugin() )
    {
        m_view->setAcceptDrops( part->plugin() ->acceptsDrops() );
        m_view->viewport() ->setAcceptDrops( part->plugin() ->acceptsDrops() );
        m_view->setPlugin( part->plugin() );
    }
    l->addWidget( m_view );

    m_view->viewport() ->setFocusProxy( m_view );
    m_view->viewport() ->setFocusPolicy( WheelFocus );
    m_view->setFocus();

    m_view->itemToInsert = 0;

    QDockWindow *dw1 = new QDockWindow( QDockWindow::OutsideDock, shell() );
    QDockWindow *dw2 = new QDockWindow( QDockWindow::OutsideDock, shell() );
    m_structure = new Kudesigner::StructureWidget( dw1 );
    m_propertyEditor = new Editor( dw2 );
    dw1->boxLayout() ->addWidget( m_structure, 1 );
    dw2->boxLayout() ->addWidget( m_propertyEditor, 1 );
    dw1->setFixedExtentWidth( 400 );
    dw1->setResizeEnabled( true );
    dw2->setFixedExtentWidth( 400 );
    dw2->setResizeEnabled( true );

    if ( m_doc->plugin() )
    {
        //                 connect( m_propertyEditor, SIGNAL(createPluggedInEditor(QWidget*&, Editor *, Property*, Box *)),
        //                          m_doc->plugin(), SLOT(createPluggedInEditor(QWidget*&, Editor *, Property*, Box *)));

        kdDebug() << "*************Property and plugin have been connected" << endl;
    }

    shell() ->addDockWindow( dw1, m_doc->propertyPosition() );
    shell() ->addDockWindow( dw2, m_doc->propertyPosition() );

    m_structure->setDocument( m_doc->canvas() );

    connect( m_doc, SIGNAL( canvasChanged( Kudesigner::Canvas * ) ),
             m_structure, SLOT( setDocument( Kudesigner::Canvas * ) ) );
    connect( m_doc->canvas(), SIGNAL( structureModified() ),
             m_structure, SLOT( refresh() ) );

    connect( m_view, SIGNAL( selectionMade( Buffer* ) ),
             this, SLOT( populateProperties( Buffer* ) ) );

    connect( m_view, SIGNAL( selectionClear() ),
             m_propertyEditor, SLOT( clear() ) );

    connect( m_view, SIGNAL( changed() ),
             m_doc, SLOT( setModified() ) );

    connect( m_view, SIGNAL( selectionMade( Buffer* ) ),
             m_structure, SLOT( selectionMade() ) );
    connect( m_view, SIGNAL( selectionClear() ),
             m_structure, SLOT( selectionClear() ) );

    connect( m_view, SIGNAL( selectedActionProcessed() ), this, SLOT( unselectItemAction() ) );
    connect( m_view, SIGNAL( modificationPerformed() ), part, SLOT( setModified() ) );
    connect( m_view, SIGNAL( itemPlaced( int, int, int, int ) ), this, SLOT( placeItem( int, int, int, int ) ) );

    gridLabel = new QLabel( i18n( "Grid size:" ), shell() );
    gridBox = new QSpinBox( 1, 100, 1, shell() );
    gridBox->setValue( 10 );
    connect( gridBox, SIGNAL( valueChanged( int ) ), m_view, SLOT( setGridSize( int ) ) );

    initActions();

    show();
    m_view->show();
    m_structure->refresh();
}

KudesignerView::~KudesignerView()
{
    delete gridLabel;
    delete gridBox;
}

void KudesignerView::paintEvent( QPaintEvent* ev )
{
    QPainter painter;
    painter.begin( this );

    // ### TODO: Scaling

    // Let the document do the drawing
    koDocument() ->paintEverything( painter, ev->rect(), FALSE, this );

    painter.end();
}

void KudesignerView::resizeEvent( QResizeEvent* /*_ev*/ )
{
    m_view->setGeometry( 0, 0, width(), height() );
}

void KudesignerView::initActions()
{
    cutAction = KStdAction::cut( this, SLOT( cut() ), actionCollection() );
    copyAction = KStdAction::copy( this, SLOT( copy() ), actionCollection() );
    pasteAction = KStdAction::paste( this, SLOT( paste() ), actionCollection() );
    selectAllAction = KStdAction::selectAll( this, SLOT( selectAll() ), actionCollection() );
    deleteAction = new KAction( i18n( "Delete" ), "editdelete", 0, this,
                                SLOT( deleteItems() ), actionCollection(), "edit_delete" );
    cutAction->setEnabled( false );
    copyAction->setEnabled( false );
    pasteAction->setEnabled( false );
    //    deleteAction->setEnabled(false);

    sectionsReportHeader = new KAction( i18n( "Report Header" ), "irh", 0, this,
                                        SLOT( slotAddReportHeader() ), actionCollection(), "rheader" );
    sectionsReportFooter = new KAction( i18n( "Report Footer" ), "irf", 0, this,
                                        SLOT( slotAddReportFooter() ), actionCollection(), "rfooter" );
    sectionsPageHeader = new KAction( i18n( "Page Header" ), "iph", 0, this,
                                      SLOT( slotAddPageHeader() ), actionCollection(), "pheader" );
    sectionsPageFooter = new KAction( i18n( "Page Footer" ), "ipf", 0, this,
                                      SLOT( slotAddPageFooter() ), actionCollection(), "pfooter" );
    sectionsDetailHeader = new KAction( i18n( "Detail Header" ), "idh", 0, this,
                                        SLOT( slotAddDetailHeader() ), actionCollection(), "dheader" );
    sectionsDetail = new KAction( i18n( "Detail" ), "id", 0, this,
                                  SLOT( slotAddDetail() ), actionCollection(), "detail" );
    sectionsDetailFooter = new KAction( i18n( "Detail Footer" ), "idf", 0, this,
                                        SLOT( slotAddDetailFooter() ), actionCollection(), "dfooter" );

    itemsNothing = new KRadioAction( i18n( "Clear Selection" ), "frame_edit", 0, this,
                                     SLOT( slotAddItemNothing() ), actionCollection(), "nothing" );
    itemsNothing->setExclusiveGroup( "itemsToolBar" );
    itemsNothing->setChecked( true );
    itemsLabel = new KRadioAction( i18n( "Label" ), "frame_text", 0, this,
                                   SLOT( slotAddItemLabel() ), actionCollection(), "label" );
    itemsLabel->setExclusiveGroup( "itemsToolBar" );
    itemsField = new KRadioAction( i18n( "Field" ), "frame_field", 0, this,
                                   SLOT( slotAddItemField() ), actionCollection(), "field" );
    itemsField->setExclusiveGroup( "itemsToolBar" );
    itemsSpecial = new KRadioAction( i18n( "Special Field" ), "frame_query", 0, this,
                                     SLOT( slotAddItemSpecial() ), actionCollection(), "special" );
    itemsSpecial->setExclusiveGroup( "itemsToolBar" );
    itemsCalculated = new KRadioAction( i18n( "Calculated Field" ), "frame_formula", 0, this,
                                        SLOT( slotAddItemCalculated() ), actionCollection(), "calcfield" );
    itemsCalculated->setExclusiveGroup( "itemsToolBar" );
    itemsLine = new KRadioAction( i18n( "Line" ), "frame_chart", 0, this,
                                  SLOT( slotAddItemLine() ), actionCollection(), "line" );
    itemsLine->setExclusiveGroup( "itemsToolBar" );

    gridActionLabel = new KWidgetAction( gridLabel, i18n( "Grid Label" ), 0, this,
                                         0, actionCollection(), "gridlabel" );

    gridAction = new KWidgetAction( gridBox, i18n( "Grid Size" ), 0, this,
                                    0, actionCollection(), "gridaction" );
}

void KudesignerView::updateReadWrite( bool /*readwrite*/ )
{
}

void KudesignerView::guiActivateEvent( KParts::GUIActivateEvent *ev )
{
    if ( ev->activated() )
        m_propertyEditor->show();
    else
        m_propertyEditor->hide();
    KoView::guiActivateEvent( ev );
}

void KudesignerView::populateProperties( Buffer *buf )
{
    connect( buf, SIGNAL( propertyChanged() ), m_doc->canvas(), SLOT( changed() ) );
    m_propertyEditor->changeSet( buf );
}

void KudesignerView::cut()
{
    //    kdDebug(31000) << "KudesignerView::cut(): CUT called" << endl;
}

void KudesignerView::copy()
{
    //    kdDebug(31000) << "KudesignerView::copy(): COPY called" << endl;
}

void KudesignerView::paste( )
{}

void KudesignerView::deleteItems( )
{
    if ( m_doc->canvas() ->selected.count() > 0 )
        m_doc->addCommand( new DeleteReportItemsCommand( m_doc->canvas(), m_doc->canvas() ->selected ) );
}

void KudesignerView::selectAll( )
{
    m_doc->canvas() ->selectAll();
}

void KudesignerView::slotAddReportHeader()
{
    if ( !( ( ( KudesignerDoc * ) ( koDocument() ) ) ) ->canvas() ->kugarTemplate() ->reportHeader )
    {
        m_doc->addCommand( new AddReportHeaderCommand( m_doc->canvas() ) );
    }
}

void KudesignerView::slotAddReportFooter()
{
    if ( !( ( ( KudesignerDoc * ) ( koDocument() ) ) ) ->canvas() ->kugarTemplate() ->reportFooter )
    {
        m_doc->addCommand( new AddReportFooterCommand( m_doc->canvas() ) );
    }
}

void KudesignerView::slotAddPageHeader()
{
    if ( !( ( ( KudesignerDoc * ) ( koDocument() ) ) ) ->canvas() ->kugarTemplate() ->pageHeader )
    {
        m_doc->addCommand( new AddPageHeaderCommand( m_doc->canvas() ) );
    }
}

void KudesignerView::slotAddPageFooter()
{
    if ( !( ( ( KudesignerDoc * ) ( koDocument() ) ) ) ->canvas() ->kugarTemplate() ->pageFooter )
    {
        m_doc->addCommand( new AddPageFooterCommand( m_doc->canvas() ) );
    }
}

void KudesignerView::slotAddDetailHeader()
{
    bool Ok = false;
    unsigned int level = QInputDialog::getInteger( tr( "Add Detail Header" ), tr( "Enter detail level:" ),
                         0, 0, 100, 1, &Ok, this );
    if ( !Ok )
        return ;
    if ( m_doc->canvas() ->kugarTemplate() ->detailsCount >= level )
    {
        m_doc->addCommand( new AddDetailHeaderCommand( level, m_doc->canvas() ) );
    }
}

void KudesignerView::slotAddDetail()
{
    bool Ok = false;
    unsigned int level = QInputDialog::getInteger( tr( "Add Detail" ), tr( "Enter detail level:" ),
                         0, 0, 100, 1, &Ok, this );
    if ( !Ok )
        return ;
    if ( ( ( level == 0 ) && ( m_doc->canvas() ->kugarTemplate() ->detailsCount == 0 ) )
            || ( m_doc->canvas() ->kugarTemplate() ->detailsCount == level ) )
    {
        m_doc->addCommand( new AddDetailCommand( level, m_doc->canvas() ) );
    }
}

void KudesignerView::slotAddDetailFooter()
{
    bool Ok = false;
    unsigned int level = QInputDialog::getInteger( tr( "Add Detail Footer" ), tr( "Enter detail level:" ),
                         0, 0, 100, 1, &Ok, this );
    if ( !Ok )
        return ;

    if ( m_doc->canvas() ->kugarTemplate() ->detailsCount >= level )
    {
        m_doc->addCommand( new AddDetailFooterCommand( level, m_doc->canvas() ) );
    }
}

void KudesignerView::slotAddItemNothing()
{
    if ( m_doc->canvas() )
    {
        if ( m_view->itemToInsert )
        {
            m_view->itemToInsert = 0;
        }
    }
}

void KudesignerView::slotAddItemLabel()
{
    if ( m_doc->canvas() )
    {
        m_view->itemToInsert = Rtti_Label;
    }
}

void KudesignerView::slotAddItemField()
{
    if ( m_doc->canvas() )
    {
        m_view->itemToInsert = Rtti_Field;
    }
}

void KudesignerView::slotAddItemSpecial()
{
    if ( m_doc->canvas() )
    {
        m_view->itemToInsert = Rtti_Special;
    }
}

void KudesignerView::slotAddItemCalculated()
{
    if ( m_doc->canvas() )
    {
        m_view->itemToInsert = Rtti_Calculated;
    }
}

void KudesignerView::slotAddItemLine()
{
    if ( m_doc->canvas() )
    {
        m_view->itemToInsert = Rtti_Line;
    }
}

void KudesignerView::unselectItemAction()
{
    /*    itemsNothing->setOn(true);*/
}

void KudesignerView::placeItem( int x, int y, int band, int bandLevel )
{
    m_doc->addCommand( new AddReportItemCommand( m_doc->canvas(), m_view, x, y, ( Kudesigner::RttiValues ) band, bandLevel ) );
}

#include "kudesigner_view.moc"
