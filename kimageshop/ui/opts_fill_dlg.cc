/*
 *  opt_fill_dialog.cc - part of Krayon
 *
 *  Copyright (c) 2001 John Califf <jcaliff@compuzone.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <qbuttongroup.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qcheckbox.h>

#include <klocale.h>

#include "opts_fill_dlg.h"

FillOptionsDialog::FillOptionsDialog( int _opacity, 
    bool _pattern, bool _gradient,
    int _redtolerance, int _bluetolerance, int _greentolerance,
    QWidget *parent, const char *name )
    : KDialog( parent, name, true )
{
    setCaption( i18n("Fill Options") );
    
    // parent, border
    QVBoxLayout* layout = new QVBoxLayout( this, 4 );
    // parent, cols, rows
    QGridLayout* grid = new QGridLayout( layout, 2, 6);

    // min, max, incr, parent
    mpOpacity = new QSpinBox( 0, 255, 16, this );
    mpOpacity->setValue( _opacity );
    QLabel* opacity_label = new QLabel( mpOpacity, 
        i18n("Opacity"), this );

    mpToleranceRed = new QSpinBox( 0, 255, 1, this );
    mpToleranceRed->setValue( _redtolerance );
    QLabel* red_label = new QLabel( mpToleranceRed, 
        i18n("Red Tolerance"), this );

    mpToleranceGreen = new QSpinBox( 0, 255, 1, this );
    mpToleranceGreen->setValue( _greentolerance );
    QLabel* green_label = new QLabel( mpToleranceGreen, 
        i18n("Green Tolerance"), this );

    mpToleranceBlue = new QSpinBox( 0, 255, 1, this );
    mpToleranceBlue->setValue( _bluetolerance );
    QLabel* blue_label = new QLabel( mpToleranceBlue, 
        i18n("Blue Tolerance"), this );

    mpUsePattern = new QCheckBox( this );
    mpUsePattern->setChecked( _pattern );
    QLabel* pattern_label = new QLabel( mpUsePattern, 
        i18n("Use Current Pattern"), this );

    mpUseGradient = new QCheckBox( this );
    mpUseGradient->setChecked( _gradient );
    QLabel* gradient_label = new QLabel( mpUseGradient, 
        i18n("Use Current Gradient"), this );

    grid->addWidget( opacity_label, 0, 0 );
    grid->addWidget( mpOpacity, 0, 1 );

    grid->addWidget( red_label, 1, 0 );
    grid->addWidget( mpToleranceRed, 1, 1 );

    grid->addWidget( green_label, 2, 0 );
    grid->addWidget( mpToleranceGreen, 2, 1 );

    grid->addWidget( blue_label, 3, 0 );
    grid->addWidget( mpToleranceBlue, 3, 1 );

    grid->addWidget( pattern_label, 4, 0 );
    grid->addWidget( mpUsePattern, 4, 1 );

    grid->addWidget( gradient_label, 5, 0 );
    grid->addWidget( mpUseGradient, 5, 1 );

    // standard button action group

    QHBoxLayout* buttons = new QHBoxLayout( layout, 3 );
    buttons->addStretch( 3 );

    QPushButton *ok, *cancel, *save;
    ok = new QPushButton( i18n("&OK"), this );
    ok->setDefault( true );
    ok->setMinimumSize( ok->sizeHint() );
    connect( ok, SIGNAL(clicked()), SLOT(accept()) );
    buttons->addWidget( ok );

    cancel = new QPushButton( i18n("&Cancel"), this );
    cancel->setMinimumSize( cancel->sizeHint() );
    connect( cancel, SIGNAL(clicked()), SLOT(reject()) );
    buttons->addWidget( cancel );

    save = new QPushButton( i18n("&Save"), this );
    save->setMinimumSize( save->sizeHint() );
    //connect( save, SIGNAL(clicked()), SLOT(reject()) );
    buttons->addWidget( save );
 
    resize( 1, 1 );
}

#include "opts_fill_dlg.moc"
