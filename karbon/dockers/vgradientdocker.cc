/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qlabel.h>
#include <qframe.h>

#include <kstandarddirs.h> 
#include <kcolorbutton.h>
#include <kcombobox.h>
#include <klocale.h>
#include <qpainter.h>
#include <qlayout.h>
#include <qvbox.h>
#include <qgroupbox.h>
#include <klistbox.h>
#include <qpushbutton.h>
#include <qdom.h>    
#include <qfile.h>
#include <qpixmap.h>
#include <kiconloader.h>

#include "vgradientwidget.h"
#include "vgradientdocker.h"
#include "../render/vkopainter.h"
#include "vfill.h"

VGradientPreview::VGradientPreview( VGradient*& gradient, QWidget* parent, const char* name )
		: QWidget( parent, name ), m_lpgradient(&gradient)
{  
	setBackgroundMode( Qt::NoBackground );
	setMinimumSize(70, 70);
} // VGradientPreview::VGradientPreview

VGradientPreview::~VGradientPreview()
{
} // VGradientPreview::~VGradientPreview

void VGradientPreview::paintEvent( QPaintEvent* )
{
	VKoPainter gp(this, width(), height());
	gp.setRasterOp( Qt::XorROP );
	gp.newPath();
	VGradient gradient(**m_lpgradient);
	if ( gradient.type() == VGradient::radial )
	{
		gradient.setOrigin( KoPoint(width()/2, height()/2) );
		gradient.setVector( KoPoint(width()/4, height()/4) );
	}
	else
	{
		gradient.setOrigin( KoPoint(width()/3, 2*(height()/3)) );
		gradient.setVector( KoPoint(2*(width()/3), height()/3) );
	}
	VFill fill;
	KIconLoader il;
	fill.pattern() = VPattern( il.iconPath("karbon.png", KIcon::Small) );
	fill.setType( VFill::patt );
	gp.setBrush(fill);
	gp.fillPath();
	fill.gradient() = gradient;
	fill.setType( VFill::grad );
	gp.setBrush(fill);
	gp.moveTo(KoPoint(2, 2));
	gp.lineTo(KoPoint(2, height()-2));
	gp.lineTo(KoPoint(width()-2, height()-2));
	gp.lineTo(KoPoint(width()-2, 2));
	gp.lineTo(KoPoint(2, 2));
	gp.fillPath();
	gp.end();
	
	QPainter p(this);
	
	p.setPen(colorGroup().light());
	p.moveTo(1, height()-1);
	p.lineTo(1, 1);
	p.lineTo(width()-1, 1);
	p.lineTo(width()-1, height()-1);
	p.lineTo(1, height()-1);
	p.setPen(colorGroup().dark());
	p.moveTo(0, height()-1);
	p.lineTo(0, 0);
	p.lineTo(width()-1, 0);
	p.moveTo(width()-2, 2);
	p.lineTo(width()-2, height()-2);
	p.lineTo(2, height()-2);    
} // VGradientPreview::paintEvent

VGradientDocker::VGradientDocker( VGradient& gradient, QWidget* parent, const char* name )
		: VDocker( parent, name ), m_gradient( &gradient )
{
	setOrientation( Vertical );
	
	m_predefGradients.setAutoDelete( true );
	
		// Load the predefined gradients.
	QString fn = locateLocal( "appdata", "gradients.predef" );
	if ( !KStandardDirs::exists( fn ) )
		fn = locate( "appdata", "gradients.predef" );
	QFile f( fn );
	if (!(f.open(IO_ReadOnly)))
	{
		m_predefGradients.append( new VGradient() );
	}
	else
	{
		QDomDocument doc;
		if (!(doc.setContent(&f)))
		{
			f.close();
			m_predefGradients.append( new VGradient() );
		}
		else
		{
			QDomElement e;
			QDomNode n = doc.documentElement().firstChild();
			while (!n.isNull())
			{
				e = n.toElement();
				if (!e.isNull())
				{
					if ( e.tagName() == "GRADIENT" )
					{
						VGradient* gradient = new VGradient();
						gradient->load( e );
						m_predefGradients.append( gradient );
					}
				}
				n = n.nextSibling();
			}
		}
	}
	
	setupUI();
	setupConnections();
	initUI();
} // VGradientDocker::VGradientDocker

VGradientDocker::~VGradientDocker()
{
		// Save the predefined gradients.
	QDomDocument doc("PredefGradients");
	QDomElement root = doc.createElement("predefGradients");
	doc.appendChild(root);
	for (VGradient* g = m_predefGradients.first(); g != NULL; g = m_predefGradients.next())
		g->save( root );
	
	QFile f( locateLocal("appdata", "gradients.predef") );
	if (!(f.open(IO_WriteOnly)))
		return;
	QTextStream ts(&f);
	doc.save(ts, 2);
	f.flush();
	f.close();
} // VGradientDocker::~VGradientDocker

void VGradientDocker::setupUI()
{
	setCaption( i18n("Edit Gradient") );
	QWidget* mainWidget = new QVBox(this);
	
	QGroupBox* predefGroup  = new QGroupBox( 1, Qt::Horizontal, i18n("Predefined gradients:"), mainWidget );
	m_predefGradientsView   = new KListBox( predefGroup );
	m_predefDelete          = new QPushButton( i18n("&Delete"), predefGroup );
	
	QGroupBox* editGroup    = new QGroupBox( i18n("Edit gradient:"), mainWidget );
	QGridLayout* editLayout = new QGridLayout( editGroup, 6, 3 );
	editLayout->setSpacing(3);
	editLayout->setMargin(6);
	editLayout->addRowSpacing(0, 12);
	editLayout->addMultiCellWidget( m_gradientWidget = new VGradientWidget(m_gradient, editGroup), 1, 1, 0, 2 );
	editLayout->addMultiCellWidget( m_gradientPreview = new VGradientPreview(m_gradient, editGroup), 2, 4, 0, 0 );
	editLayout->addWidget( new QLabel(i18n("Type:"), editGroup), 2, 1 );
	editLayout->addWidget( new QLabel(i18n("Repeat:"), editGroup), 3, 1 );
	editLayout->addWidget( new QLabel(i18n("Target:"), editGroup), 4, 1 );
	editLayout->addWidget( m_gradientType = new KComboBox(false, editGroup), 2, 2 );
	m_gradientType->insertItem( i18n( "Linear" ), 0 );
	m_gradientType->insertItem( i18n( "Radial" ), 1 );
	m_gradientType->insertItem( i18n( "Conical" ), 2 );
	editLayout->addWidget( m_gradientRepeat = new KComboBox(false, editGroup), 3, 2 );
	m_gradientRepeat->insertItem( i18n( "None" ), 0 );
	m_gradientRepeat->insertItem( i18n( "Reflect" ), 1 );
	m_gradientRepeat->insertItem( i18n( "Repeat" ), 2 );
	editLayout->addWidget( m_gradientTarget = new KComboBox(false, editGroup), 4, 2 );
	m_gradientTarget->insertItem( i18n( "Stroke" ), 0 );
	m_gradientTarget->insertItem( i18n( "Fill" ), 1 );
	editLayout->addMultiCellWidget( m_addToPredefs = new QPushButton(i18n("&Add to predefined gradients"), editGroup), 5, 5, 0, 2 );
	
	setWidget(mainWidget);
} // VGradientDocker::setupUI

void VGradientDocker::setupConnections()
{
	connect(m_gradientType, SIGNAL(activated(int)), this, SLOT(combosChange(int)));
	connect(m_gradientRepeat, SIGNAL(activated(int)), this, SLOT(combosChange(int)));
	connect(m_gradientWidget, SIGNAL(changed()), m_gradientPreview, SLOT(update()));
	connect(m_addToPredefs, SIGNAL(clicked()), this, SLOT(addGradientToPredefs()));
	connect(m_predefGradientsView, SIGNAL(executed(QListBoxItem*)), this, SLOT(changeToPredef(QListBoxItem*)));
	connect(m_predefDelete, SIGNAL(clicked()), this, SLOT(deletePredef()));
} // VGradientDocker::setupConnection

void VGradientDocker::initUI()
{
	m_gradientType->setCurrentItem( m_gradient->type() );
	m_gradientRepeat->setCurrentItem( m_gradient->repeatMethod() );
	m_gradientTarget->setCurrentItem( FILL );
	
	for (VGradient* g = m_predefGradients.first(); g != NULL; g = m_predefGradients.next())
	{
		QPixmap p;
		p.resize( 160, 16 );
		VKoPainter gp(&p, width(), height());
		gp.setRasterOp( Qt::XorROP );
		gp.newPath();
		VGradient gradient(*g);
		gradient.setOrigin( KoPoint(0, 0) );
		gradient.setVector( KoPoint(p.width()-1, 0) );
		gradient.setType( VGradient::linear );
		VFill fill;
		KIconLoader il;
		fill.pattern() = VPattern( il.iconPath("karbon.png", KIcon::Small) );
		fill.setType( VFill::patt );
		gp.setBrush(fill);
		gp.fillPath();
		fill.gradient() = gradient;
		fill.setType( VFill::grad );
		gp.setBrush(fill);
		gp.moveTo(KoPoint(0, 0));
		gp.lineTo(KoPoint(0, p.height()-1));
		gp.lineTo(KoPoint(p.width()-1, p.height()-1));
		gp.lineTo(KoPoint(p.width()-1, 0));
		gp.lineTo(KoPoint(0, 0));
		gp.fillPath();
		gp.end();
		m_predefGradientsView->insertItem( p );
	}
} // VGradientDocker::initUI

const VGradient* VGradientDocker::gradient()
{
	return m_gradient;
} // VGradientDocker::gradient

void VGradientDocker::setGradient( VGradient& gradient )
{
	m_gradient = &gradient;
	
	initUI();
} // VGradientDocker::setGradient

VGradientDocker::VGradientTarget VGradientDocker::target()
{
	return (VGradientTarget)m_gradientTarget->currentItem();
} // VGradientDocker::target

void VGradientDocker::setTarget(VGradientTarget target)
{
	m_gradientTarget->setCurrentItem( target );
} // VGradientDocker::setTarget

void VGradientDocker::combosChange(int)
{
	m_gradient->setType( (VGradient::VGradientType)m_gradientType->currentItem() );
	m_gradient->setRepeatMethod( (VGradient::VGradientRepeatMethod)m_gradientRepeat->currentItem() );
	
	m_gradientPreview->update();
} // VGradientDocker::combosChange

void VGradientDocker::addGradientToPredefs()
{
	m_predefGradients.append( new VGradient(*m_gradient) );
		
	QPixmap p;
	p.resize( 160, 16 );
	VKoPainter gp(&p, width(), height());
	gp.setRasterOp( Qt::XorROP );
	gp.newPath();
	VGradient gradient(*m_gradient);
	gradient.setOrigin( KoPoint(0, 0) );
	gradient.setVector( KoPoint(p.width()-1, 0) );
	gradient.setType( VGradient::linear );
	VFill fill;
	fill.gradient() = gradient;
	fill.setType( VFill::grad );
	gp.setBrush(fill);
	gp.moveTo(KoPoint(0, 0));
	gp.lineTo(KoPoint(0, p.height()-1));
	gp.lineTo(KoPoint(p.width()-1, p.height()-1));
	gp.lineTo(KoPoint(p.width()-1, 0));
	gp.lineTo(KoPoint(0, 0));
	gp.fillPath();
	gp.end();
	m_predefGradientsView->insertItem( p );
} // VGradientDocker::addGradientToPredefs()

void VGradientDocker::changeToPredef(QListBoxItem*) 
{
	*m_gradient = *(m_predefGradients.at(m_predefGradientsView->currentItem()));
	m_gradientPreview->update();
	m_gradientWidget->update();
} // VGradientDocker::changeToPredef

void VGradientDocker::deletePredef()
{
	int i = m_predefGradientsView->currentItem();
	m_predefGradientsView->removeItem( i );
	m_predefGradients.remove( i );
} // VGradientDocker::deletePredef

#include "vgradientdocker.moc"
