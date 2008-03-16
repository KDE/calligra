/*
 * OpenRPT report writer and rendering engine
 * Copyright (C) 2001-2007 by OpenMFG, LLC
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */

#include "reportdesigner.h"
#include "reportsection.h"

#include "reportentities.h"
#include "reportentitylabel.h"
#include "reportentityfield.h"
#include "reportentitytext.h"
#include "reportentityline.h"
#include "reportentitybarcode.h"
#include "reportentityimage.h"

#include "reportscene.h"
#include "reportsceneview.h"

// qt
#include <qlabel.h>
#include <qdom.h>
#include <qlayout.h>
#include <QGridLayout>
#include <QMouseEvent>

#include <KoGlobal.h>
#include <KoRuler.h>
#include <KoZoomHandler.h>
#include <koproperty/editor.h>

#include <kdebug.h>


//
// ReportSection method implementations
//

ReportSection::ReportSection ( ReportDesigner * rptdes, const char * name )
		: QWidget ( rptdes )
{
	_data = new KRSectionData();
	QObject::connect ( _data->properties(), SIGNAL ( propertyChanged ( KoProperty::Set &, KoProperty::Property & ) ), this, SLOT ( slotPropertyChanged ( KoProperty::Set &, KoProperty::Property & ) ) );
	int dpiY = KoGlobal::dpiY();
	
	_rd = rptdes;
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	
	QGridLayout * glayout = new QGridLayout ( this );
	glayout->setSpacing ( 0 );
	glayout->setMargin ( 0 );
	glayout->setColumnStretch(1,1);
	glayout->setRowStretch(1,1);
	glayout->setSizeConstraint(QLayout::SetFixedSize);
	
	// ok create the base interface
	title = new ReportSectionTitle ( this );
	title->setObjectName ( "detail" );
	title->setText ( i18n( "Detail" ) );
	
	sectionRuler = new KoRuler ( this, Qt::Vertical, _rd->zoomHandler() );
	scene = new ReportScene ( _rd->pageWidthPx(), dpiY, rptdes );
	sceneview = new ReportSceneView ( rptdes, scene, this,"scene view" );
	sceneview->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	
	rb = new ReportResizeBar ( this );

	QObject::connect ( rb,SIGNAL ( barDragged ( int ) ),this,SLOT ( slotResizeBarDragged ( int ) ) );
	
	QObject::connect ( _rd,SIGNAL ( pagePropertyChanged ( KoProperty::Set & ) ), this, SLOT ( slotPageOptionsChanged ( KoProperty::Set & ) ) );
	
	QObject::connect ( scene, SIGNAL( clicked() ), this, (SLOT(slotSceneClicked() )));
	QObject::connect ( scene, SIGNAL( lostFocus() ), this, (SLOT(slotSceneLostFocus() )));
	
	QObject::connect (title, SIGNAL (doubleClicked() ), this, (SLOT(slotTitleDoubleClicked())));
	glayout->addWidget ( title, 0, 0, 1, 2 );
	glayout->addWidget ( sectionRuler, 1, 0 );
	glayout->addWidget ( sceneview , 1, 1);
	glayout->addWidget ( rb, 2, 0, 1, 2);
	
	title->setMinimumWidth(_rd->pageWidthPx() + sectionRuler->frameSize().width());
	
	setLayout ( glayout );
	slotResizeBarDragged(0);
}

ReportSection::~ReportSection()
{
	// Qt should be handling everything for us
}

void ReportSection::setTitle ( const QString & s )
{
	title->setText ( s );
}

void ReportSection::slotResizeBarDragged ( int delta )
{
	
	if ( sceneview->document() && sceneview->document()->propertySet()->property("PageSize").value().toString() == "Labels" )
	{
		return; // we don't want to allow this on reports that are for labels
	}

	qreal h = scene->height() + delta;

	if ( h < 1 ) h = 1;
        
	h = scene->gridPoint(QPointF(0, h)).y();
	
	sectionRuler->setRulerLength ( h );
	
	scene->setSceneRect ( 0,0,scene->width(), h );
	sceneview->setSceneRect ( 0,0,scene->width(), h );
	sceneview->resizeContents ( QSize(scene->width(), h) );

	_rd->setModified ( true );
	_rd->adjustSize();
}

void ReportSection::buildXML ( QDomDocument & doc, QDomElement & section )
{
	int dpiY = KoGlobal::dpiY();
	qreal f = INCH_TO_POINT(scene->height() / dpiY);
	//f = ( ( f - ( int ) f ) > .5 ? f : f + 1 );
	QDomElement height = doc.createElement ( "height" );
	height.appendChild ( doc.createTextNode ( QString::number ( f ) ) );
	section.appendChild ( height );

	QDomElement bgcolor = doc.createElement ( "bgcolor" );
	bgcolor.appendChild ( doc.createTextNode ( _data->bgColor().name() ));
	section.appendChild ( bgcolor );
	
	QDomElement event_onrender = doc.createElement ( "event_onrender" );
	event_onrender.appendChild ( doc.createTextNode ( _data->eventOnRender() ));
	section.appendChild ( event_onrender );
	
	// now get a list of all the QCanvasItems on this scene and output them.
	QGraphicsItemList list = scene->items();
	for ( QGraphicsItemList::iterator it = list.begin();
	        it != list.end(); it++ )
	{
		ReportEntity::buildXML ( ( *it ),doc,section );
	}
	
	
}

void ReportSection::initFromXML ( QDomNode & section )
{
	QDomNodeList nl = section.childNodes();
	QDomNode node;
	QString n;

	for ( int i = 0; i < nl.count(); i++ )
	{
		node = nl.item ( i );
		n = node.nodeName();
		if ( n == "height" )
		{
			qreal h = node.firstChild().nodeValue().toDouble();
			h  = POINT_TO_INCH(h) * KoGlobal::dpiY();
			kDebug() << "Section Height: " << h << endl;
			scene->setSceneRect ( 0,0,scene->width(), h );
			slotResizeBarDragged ( 0 );
		}
		else if ( n == "bgcolor" )
		{
			_data->_bgColor->setValue(QColor(node.firstChild().nodeValue()));
		}
		else if ( n == "event_onrender" )
		{
			_data->_event_onrender = node.firstChild().nodeValue();
		}
		//Objects
		else if ( n == "label" )
		{
			( new ReportEntityLabel ( node, sceneview->document(), scene ) )->setVisible ( true );
		}
		else if ( n == "field" )
		{
			( new ReportEntityField ( node, sceneview->document(), scene ) )->setVisible ( true );
		}
		else if ( n == "text" )
		{
			( new ReportEntityText ( node, sceneview->document(), scene ) )->setVisible ( true );
		}
		else if ( n == "line" )
		{
			( new ReportEntityLine ( node, sceneview->document(), scene ) )->setVisible ( true );
		}
		else if ( n == "barcode" )
		{
			( new ReportEntityBarcode ( node, sceneview->document(), scene ) )->setVisible ( true );
		}
		else if ( n == "image" )
		{
			( new ReportEntityImage ( node, sceneview->document(), scene ) )->setVisible ( true );
		} //TODO add graph
		//else if(n == "graph") {
		//  (new ReportEntityGraph(node, sceneview->document(), scene))->setVisible(true);
		//}
		else if ( n == "key" || n == "firstpage" || n == "lastpage"
		          || n == "odd" || n == "even" )
		{
			// these are all handled elsewhere but we don't want to show errors
			// because they are expected sometimes
		}
		else
		{
			kDebug() << "Encountered unknown node while parsing section: " << n << endl;
		}
	}
}

QSize ReportSection::sizeHint() const
{
	return QSize( scene->width()  + sectionRuler->frameSize().width(), title->frameSize().height() + sceneview->sizeHint().height() + rb->frameSize().height());;
}

void ReportSection::slotPageOptionsChanged ( KoProperty::Set &set )
{
	KoUnit unit = _rd->pageUnit();
	
	//update items position with unit
	QList<QGraphicsItem*> itms = scene->items();
	for (int i = 0; i < itms.size(); ++i)
	{
		if (itms[i]->type() >= 65550 && itms[i]->type() <= 65555)
		{
			dynamic_cast<ReportRectEntity*>(itms[i])->setUnit(unit);	
		}
	}

	scene->setSceneRect ( 0,0,_rd->pageWidthPx(), scene->height() );
	title->setMinimumWidth(_rd->pageWidthPx() + sectionRuler->frameSize().width());
	sectionRuler->setUnit(_rd->pageUnit());
	
	//Trigger a redraw of the background
	sceneview->resetCachedContent();
	
	_rd->adjustSize();
	_rd->repaint();
}

void ReportSection::slotSceneClicked()
{
	title->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	_rd->changeSet(_data->properties());
}

void ReportSection::slotSceneLostFocus()
{
	title->setFrameStyle(QFrame::Panel | QFrame::Raised);
}

void ReportSection::slotTitleDoubleClicked()
{
	kDebug() << endl;
	_data->_event_onrender = _rd->editorText(_data->_event_onrender);
	if ( _rd ) _rd->setModified ( true );
}

void ReportSection::slotPropertyChanged ( KoProperty::Set &s, KoProperty::Property &p )
{
	kDebug() << endl;
	//Handle Position
	if ( p.name() == "BackgroundColor" )
	{
		scene->setBackgroundBrush(p.value().value<QColor>());
	}
	
	if ( _rd ) _rd->setModified ( true );
	
	scene->update();
}

//
// class ReportResizeBar
//
ReportResizeBar::ReportResizeBar ( QWidget * parent, Qt::WFlags f )
		: QFrame ( parent, f )
{
	setMinimumHeight ( 5 );
	setMaximumHeight ( 5 );
	setCursor ( QCursor ( Qt::SizeVerCursor ) );
	setFrameStyle ( QFrame::HLine );
}

void ReportResizeBar::mouseMoveEvent ( QMouseEvent * e )
{
	e->accept();
	emit barDragged ( e->y() );
}

//=============================================================================

ReportSectionTitle::ReportSectionTitle(QWidget*parent) : QLabel(parent)
{
	setFrameStyle(QFrame::Panel | QFrame::Raised);
	setMaximumHeight(minimumSizeHint().height());
	setMinimumHeight(minimumSizeHint().height());
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

ReportSectionTitle::~ReportSectionTitle()
{
	
}

void ReportSectionTitle::mouseDoubleClickEvent( QMouseEvent * event )
{
	emit(doubleClicked());
}