#include <qradiobutton.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qbuttongroup.h>

#include <kdebug.h>
#include <knuminput.h>

#include "rotationdialogimpl.h"
#include "textpreview.h"

RotationDialogImpl::RotationDialogImpl( QWidget *parent, const char* name )
    :  RotationDialogBase( parent, name )
{
    _preview = new TextPreview( previewPanel );
    QHBoxLayout *lay = new QHBoxLayout( previewPanel, previewPanel->lineWidth(), 0 );
    lay->addWidget( _preview );

    customInput->setRange( 0, 360, 0.1, TRUE );
    connect( customRadio, SIGNAL( toggled( bool ) ),
	     customInput, SLOT( setEnabled( bool ) ) );
    connect( customInput, SIGNAL( valueChanged( double ) ),
	     this, SLOT( angleChanged( double ) ) );
}

void RotationDialogImpl::applyClicked()
{
    emit apply();
}

void RotationDialogImpl::okClicked()
{
    applyClicked();
    accept();
}

void RotationDialogImpl::setAngle( double angle )
{
    customInput->setValue( angle );

    if ( angle == 90
	 || angle == 180
	 || angle == 270 )
	angleGroup->setButton( (int)angle );
    else if ( angle == 0 )
	angleGroup->setButton( 1 );
    else
        angleGroup->setButton( 0 );
}

double RotationDialogImpl::angle()
{
    int id = angleGroup->id( angleGroup->selected() );

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
	    return customInput->value();
    }
}

void RotationDialogImpl::angleChanged( double a )
{
    _preview->setAngle( a );
}

void RotationDialogImpl::angleMode( int id )
{
    double a = 0;
    if ( id == 1 )
        a = 0;
    else if ( id == 90 || id == 180 || id == 270 )
        a = id;
    else
        a = customInput->value();
    _preview->setAngle( a );
}
#include "rotationdialogimpl.moc"
