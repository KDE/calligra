/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#include "optiondia.h"
#include "optiondia.moc"

#include <qlabel.h>
#include <qstring.h>
#include <qgroupbox.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qtabwidget.h>

#include <kapp.h>
#include <klocale.h>
#include <knuminput.h>
#include <kbuttonbox.h>

#include <stdlib.h>

/******************************************************************/
/* class OptionDia                                                */
/******************************************************************/

/*==================== constructor ===============================*/
OptionDia::OptionDia( QWidget *parent, const char *name )
    :QDialog( parent, name, true )
{
    /* Tab: General */

    QVBoxLayout *layout = new QVBoxLayout( this );
    layout->setMargin( 5 );
    layout->setSpacing( 5 );
    
    QVBoxLayout *vbox = new QVBoxLayout( layout );
    vbox->setSpacing( 5 );

    QTabWidget *tabWidget = new QTabWidget( this );
    vbox->addWidget( tabWidget );
    
    QVBox *general = new QVBox( tabWidget );
    general->setMargin( 5 );
    general->setSpacing( 5 );

    (void) new QLabel( i18n( "Horizontal Raster: " ), general );

    eRastX = new KIntNumInput(20, general);
      
    (void) new QLabel( i18n( "Vertical Raster: " ), general );
  
    eRastY = new KIntNumInput(20, general); 

    tabWidget->addTab(general, i18n( "General" ));
    
    /* Tab: Objects */
    
    QVBox *objects = new QVBox( tabWidget );
    objects->setMargin( 5 );
    objects->setSpacing( 5 );
    
    gbObjects = new QGroupBox( 2, Qt::Horizontal, i18n( "Objects in editing mode" ), objects );
        
    (void) new QLabel( i18n( "Background color:" ), gbObjects );
    
    bBackCol = new KColorButton( white, gbObjects );

    tabWidget->addTab(objects, i18n( "Objects" )); 
    
    /* Buttons */
    KButtonBox *bb = new KButtonBox( this );
    bb->addStretch();

    okBut = bb->addButton( i18n( "&OK" ) );
    cancelBut = bb->addButton( i18n( "&Cancel" ) );
    okBut->setDefault( true );

    connect( cancelBut, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( okBut, SIGNAL( clicked() ), this, SLOT( accept() ) );

    bb->layout();

    layout->addWidget( bb );
}

/*===================== destructor ===============================*/
OptionDia::~OptionDia()
{
}
