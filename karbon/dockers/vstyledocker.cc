/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
   Copyright (C) 2002, 2003 The Karbon Developers

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

#include <QLabel>
#include <QLayout>
#include <qtabwidget.h>
#include <qsize.h>
#include <Q3HButtonGroup>
#include <qtoolbutton.h>
#include <QMouseEvent>
#include <Q3PtrList>
#include <QVBoxLayout>
#include <QRectF>

#include <klocale.h>
#include <kiconloader.h>
#include <KoMainWindow.h>
#include <KoFilterManager.h>
#include <kfiledialog.h>

#include "karbon_part.h"
#include "karbon_view.h"
#include "karbon_factory.h"
#include "karbon_resourceserver.h"
#include "karbon_drag.h"
#include "vselection.h"
#include "vlayer.h"
#include "vfill.h"
#include "vfillcmd.h"
#include "vtransformcmd.h"

#include "vstyledocker.h"

#include <unistd.h>

ClipartChooser::ClipartChooser( QSize iconSize, QWidget *parent, const char *name )
	: KoIconChooser( iconSize, parent, name )
{
	setDragEnabled( true );
}

void
ClipartChooser::startDrag()
{
	KoIconChooser::startDrag();
	KarbonDrag* kd = new KarbonDrag( this );
	VObjectList objects;
	VClipartIconItem *selectedClipart = (VClipartIconItem *)currentItem();
	double s = qMax( selectedClipart->originalWidth(), selectedClipart->originalHeight() );
	VObject *clipart = selectedClipart->clipart()->clone();

	QMatrix mat( s, 0, 0, -s, -( s / 2 ), ( s / 2 ) );

	VTransformCmd trafo( 0L, mat );
	trafo.visit( *clipart );

	objects.append( clipart );
	kd->setObjectList( objects );
	kd->start(Qt::CopyAction);
}

VStyleDocker::VStyleDocker( KarbonPart* part, KarbonView* parent, const char* /*name*/ )
	: QWidget(), m_part ( part ), m_view( parent )
{
	setWindowTitle( i18n( "Resources" ) );

	mTabWidget = new QTabWidget( this );

	//Pattern
	KoPatternChooser *pPatternChooser = new KoPatternChooser( KarbonFactory::rServer()->patterns(), mTabWidget );
	pPatternChooser->setWindowTitle( i18n( "Patterns" ) );

	connect( pPatternChooser, SIGNAL(selected( KoIconItem * ) ), this, SLOT( slotItemSelected( KoIconItem * )));
	mTabWidget->addTab( pPatternChooser, i18n( "Patterns" ) );

	//Clipart
	ClipartWidget *pClipartWidget = new ClipartWidget( KarbonFactory::rServer()->cliparts(), part, mTabWidget );
	mTabWidget->addTab( pClipartWidget, i18n( "Clipart" ) );

	QVBoxLayout *mainWidgetLayout = new QVBoxLayout;
	mainWidgetLayout->addWidget( mTabWidget );
	mainWidgetLayout->activate();
	setLayout(mainWidgetLayout);

	setMinimumHeight( 174 );
	setMinimumWidth( 194 );
}

VStyleDocker::~VStyleDocker()
{
}

void VStyleDocker::slotItemSelected( KoIconItem *item )
{
	VPattern *pattern = (VPattern *)item;
	if( !pattern ) return;
	kDebug(38000) << "loading pattern : " << pattern->tilename().toLatin1() << endl;
	if( m_part && m_part->document().selection() )
	{
		VFill fill;
		fill.pattern() = *pattern;//.load( pattern->tilename() );
		//fill.setColor( *m_color );
		fill.setType( VFill::patt );
		m_part->addCommand( new VFillCmd( &m_part->document(), fill ), true );
	}
}

void
VStyleDocker::mouseReleaseEvent( QMouseEvent * )
{
}

ClipartWidget::ClipartWidget( Q3PtrList<VClipartIconItem>* clipartItems, KarbonPart *part, QWidget* parent )
	: QWidget( parent ), m_part( part )
{
	KIconLoader il;

	QVBoxLayout* layout = new QVBoxLayout;
	layout->addWidget( m_clipartChooser = new ClipartChooser( QSize( 32, 32 ), this ) );
	layout->addWidget( m_buttonGroup = new Q3HButtonGroup( this ) );
	QToolButton* m_addClipartButton;
	m_buttonGroup->insert( m_addClipartButton = new QToolButton( m_buttonGroup ) );
	m_buttonGroup->insert( m_importClipartButton = new QToolButton( m_buttonGroup ) );
	m_buttonGroup->insert( m_deleteClipartButton = new QToolButton( m_buttonGroup ) );
	m_addClipartButton->setIcon( SmallIcon( "14_layer_newlayer" ) );
	m_addClipartButton->setText( i18n( "Add" ) );
	m_importClipartButton->setIcon( SmallIcon( "fileimport" ) );
	m_importClipartButton->setText( i18n( "Import" ) );
	m_deleteClipartButton->setIcon( SmallIcon( "14_layer_deletelayer" ) );
	m_deleteClipartButton->setText( i18n( "Delete" ) );

	m_buttonGroup->setInsideMargin( 3 );

	//setFrameStyle( Box | Sunken );
	layout->setMargin( 3 );

	connect( m_buttonGroup, SIGNAL( clicked( int ) ), this, SLOT( slotButtonClicked( int ) ) );
	//connect( m_deleteClipartButton, SIGNAL( clicked() ), this, SLOT( deleteClipart() ) );
	connect( m_clipartChooser, SIGNAL( selected( KoIconItem* ) ), this, SLOT( clipartSelected( KoIconItem* ) ) );

	m_clipartChooser->setAutoDelete( false );
	VClipartIconItem* item = 0L;

	for( item = clipartItems->first(); item; item = clipartItems->next() )
		m_clipartChooser->addItem( item );

	m_clipartItem = ( clipartItems->first() ) ? clipartItems->first()->clone() : 0;
	if( !m_clipartItem )
		m_deleteClipartButton->setEnabled( false );

	setLayout(layout);
}

ClipartWidget::~ClipartWidget()
{
	delete m_clipartItem;
}

VClipartIconItem* ClipartWidget::selectedClipart()
{
	return m_clipartItem;
}

void
ClipartWidget::clipartSelected( KoIconItem* item )
{
	if( item )
	{
		delete m_clipartItem;
		VClipartIconItem* clipartItem = ( VClipartIconItem* ) item;
		m_deleteClipartButton->setEnabled( clipartItem->canDelete() );
		m_selectedItem = clipartItem;
		m_clipartItem = clipartItem->clone();
	}
}

void
ClipartWidget::addClipart()
{
	VObject* clipart = 0L;
	VSelection* selection = m_part->document().selection();

	if( selection->objects().count() == 1 )
	{
		clipart = selection->objects().getFirst()->clone();
		clipart->setParent( 0L );
	}

	if( selection->objects().count() > 1 )
	{
		Q3PtrVector<VObject> objects;
		selection->objects().toVector( &objects );
		VGroup* group = new VGroup( 0L );

		for( unsigned int i = 0; i < objects.count(); i++ )
		{
			VObject *obj = objects[ i ]->clone();
			obj->setParent( 0L );
			group->append( obj );
		}

		clipart = group;
	}

	if( clipart )
	{
		QRectF clipartBox = clipart->boundingBox();
		double scaleFactor = 1. / qMax( clipartBox.width(), clipartBox.height() );
		QMatrix trMatrix( scaleFactor, 0, 0, scaleFactor, -clipartBox.x() * scaleFactor, -clipartBox.y() * scaleFactor );

		VTransformCmd trafo( 0L, trMatrix );
		trafo.visit( *clipart );

		// center the clipart
		trMatrix.reset();
		double size = qMax( clipart->boundingBox().width(), clipart->boundingBox().height() );
		trMatrix.translate( ( size - clipart->boundingBox().width() ) / 2, ( size - clipart->boundingBox().height() ) / 2 );

		trafo.setMatrix( trMatrix );
		trafo.visit( *clipart );

		// remove Y-mirroring
		trMatrix.reset();
		trMatrix.scale( 1, -1 );
		trMatrix.translate( 0, -1 );

		trafo.setMatrix( trMatrix );
		trafo.visit( *clipart );

		m_clipartChooser->addItem( KarbonFactory::rServer()->addClipart( clipart, clipartBox.width(), clipartBox.height() ) );
	}

	m_clipartChooser->updateContents();
}

void
ClipartWidget::importClipart()
{
	QStringList filter;
	filter << "application/x-karbon" << "image/svg+xml" << "image/x-wmf" << "image/x-eps" << "application/postscript";
	KFileDialog dialog( KUrl("foo"), "", 0);
	dialog.setCaption("Choose Graphic to Add");
	dialog.setModal(true);
	dialog.setMimeFilter( filter, "application/x-karbon" );
	if( dialog.exec()!=QDialog::Accepted )
	{
		return;
	}
	QString fname = dialog.selectedFile();
	if( m_part->nativeFormatMimeType() == dialog.currentMimeFilter().toLatin1() )
		m_part->mergeNativeFormat( fname );
	else
	{
		KoFilterManager man( m_part );
		KoFilter::ConversionStatus status;
		QString importedFile = man.import( fname, status );
		if( status == KoFilter::OK )
			m_part->mergeNativeFormat( importedFile );
		if( !importedFile.isEmpty() )
			unlink( QFile::encodeName( importedFile ) );
		if( status != KoFilter::OK )
			return;
	}
	m_part->document().selection()->clear();
	// TODO: porting to flake
	//m_part->document().selection()->append( m_part->document().activeLayer()->objects() );
	addClipart();
	m_part->document().selection()->clear();
	m_part->document().removeLayer( m_part->document().activeLayer() );
}

void
ClipartWidget::deleteClipart()
{
	VClipartIconItem* clipartItem = m_clipartItem;
	KarbonFactory::rServer()->removeClipart( clipartItem );
	m_clipartChooser->removeItem( m_selectedItem );
	m_clipartChooser->updateContents();
}

void
ClipartWidget::slotButtonClicked( int id )
{
	switch( id )
	{
		case 0: addClipart(); break;
		case 1: importClipart(); break;
		case 2: deleteClipart();
	}
}

#include "vstyledocker.moc"

