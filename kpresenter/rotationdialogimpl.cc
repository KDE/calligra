// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
#include <qradiobutton.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qbuttongroup.h>

#include <kdebug.h>
#include <klocale.h>
#include <knuminput.h>

#include "rotationdialogimpl.h"
#include "rotationpropertyui.h"
#include "textpreview.h"

RotationDialogImpl::RotationDialogImpl( QWidget *parent, const char* name )
: KDialogBase( parent, name, true, i18n( "Rotation"), Ok|Cancel|Apply, Ok, true )
, m_dialog( new RotationPropertyUI( this, name ) )
{
    m_preview = new TextPreview( m_dialog->previewPanel );
    QHBoxLayout *lay = new QHBoxLayout( m_dialog->previewPanel, m_dialog->previewPanel->lineWidth(), 0 );
    lay->addWidget( m_preview );

    m_dialog->customInput->setRange( 0, 360, 5.0, TRUE );
//    connect( m_dialog->customRadio, SIGNAL( toggled( bool ) ),
//             m_dialog->customInput, SLOT( setEnabled( bool ) ) );
    connect( m_dialog->customInput, SIGNAL( valueChanged( double ) ),
             this, SLOT( angleChanged( double ) ) );
    connect( m_dialog->angleGroup, SIGNAL( clicked( int ) ), 
             this, SLOT( angleMode( int ) ) );

    connect( this, SIGNAL( okClicked() ), this, SLOT( slotOk() ) );

    setMainWidget( m_dialog );
}

void RotationDialogImpl::slotOk()
{
    emit apply();
    accept();
}

void RotationDialogImpl::setAngle( double angle )
{
    m_dialog->customInput->setValue( angle );

    if ( angle == 90
         || angle == 180
         || angle == 270 )
        m_dialog->angleGroup->setButton( (int)angle );
    else
    {
        m_dialog->angleGroup->setButton( 0 );
        m_dialog->customInput->setFocus();
    }
}

double RotationDialogImpl::angle()
{
    int id = m_dialog->angleGroup->id( m_dialog->angleGroup->selected() );

    switch( id ) {
    case 1:
        return 0;
    case 90:
        return 90;
    case 180:
        return 180;
    case 270:
        return 270;
    default:
        return m_dialog->customInput->value();
    }
}

void RotationDialogImpl::angleChanged( double a )
{
    m_preview->setAngle( a );
    m_dialog->angleGroup->setButton( 0 );
}

void RotationDialogImpl::angleMode( int id )
{
    double a = 0;
    if ( id == 1 )
        a = 0;
    else if ( id == 90 || id == 180 || id == 270 )
        a = id;
    else
        a = m_dialog->customInput->value();
    disconnect( m_dialog->customInput, SIGNAL( valueChanged( double ) ),
                this, SLOT( angleChanged( double ) ) );
    m_dialog->customInput->setValue( a );
    connect( m_dialog->customInput, SIGNAL( valueChanged( double ) ),
             this, SLOT( angleChanged( double ) ) );
    m_preview->setAngle( a );
}
#include "rotationdialogimpl.moc"
