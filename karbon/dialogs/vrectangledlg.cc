/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#include <qevent.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qwidget.h>

#include <klocale.h>
#include <knuminput.h>
#include "vrectangledlg.h"
#include "karbon_part.h"

VRectangleDlg::VRectangleDlg( KarbonPart*part, QWidget* parent, const char* name )
	: KDialog( parent, name, true, Qt::WStyle_Customize |
	  WType_Dialog | Qt::WStyle_NormalBorder | Qt::WStyle_Title ),
          m_part(part)
{
	setCaption( i18n( "Insert Rectangle" ) );

	QBoxLayout* outerbox = new QHBoxLayout( this );

	// add input fields on the left:
	QGroupBox* group = new QGroupBox( 2, Qt::Horizontal, i18n( "Properties" ), this );
 	outerbox->addWidget( group );

	// add width/height-input:
	new QLabel( i18n( "Width(%1):" ).arg(m_part->getUnitName()), group );
	m_width = new KDoubleNumInput( 0, group );
	new QLabel( i18n( "Height(%1):" ).arg(m_part->getUnitName()), group );
	m_height = new KDoubleNumInput( 0, group );

	outerbox->addSpacing( 2 );

	// add buttons on the right side:
	QBoxLayout* innerbox = new QVBoxLayout( outerbox );

	innerbox->addStretch();

	QPushButton* okbutton = new QPushButton( i18n( "&Ok" ), this );
	QPushButton* cancelbutton = new QPushButton( i18n( "&Cancel" ), this );

	okbutton->setMaximumSize( okbutton->sizeHint() );
	cancelbutton->setMaximumSize( cancelbutton->sizeHint() );

	okbutton->setFocus();

	innerbox->addWidget( okbutton );
	innerbox->addSpacing( 2 );
	innerbox->addWidget( cancelbutton );

	// signals and slots:
	connect( okbutton, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( cancelbutton, SIGNAL( clicked() ), this, SLOT( reject() ) );
        resize( 300, 80);
}

double
VRectangleDlg::width() const
{
    	return KoUnit::ptFromUnit(m_width->value(),m_part->getUnit()) ;
}

double
VRectangleDlg::height() const
{
    return KoUnit::ptFromUnit(m_height->value(),m_part->getUnit()) ;
}

void
VRectangleDlg::setWidth( double value )
{
    m_width->setValue(KoUnit::ptToUnit( value, m_part->getUnit() ));
}

void
VRectangleDlg::setHeight( double value )
{
    m_height->setValue( KoUnit::ptToUnit( value, m_part->getUnit() ) );
}

#include "vrectangledlg.moc"

