/****************************************************************************
** Form implementation generated from reading ui file 'searchdia.ui'
**
** Created: Mon Jul 17 00:09:49 2000
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "searchdia.h"

#include <qcheckbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a SearchDialog which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
SearchDialog::SearchDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setProperty( "name", "SearchDialog" );
    resize( 466, 155 ); 
    setProperty( "caption", i18n( "Find String"  ) );
    grid = new QGridLayout( this ); 
    grid->setSpacing( 6 );
    grid->setMargin( 11 );

    back = new QCheckBox( this, "back" );
    back->setProperty( "text", i18n( "&Backwards"  ) );

    grid->addWidget( back, 3, 1 );

    lineEdit = new QLineEdit( this, "lineEdit" );

    grid->addMultiCellWidget( lineEdit, 2, 2, 0, 2 );

    hbox = new QHBoxLayout; 
    hbox->setSpacing( 6 );
    hbox->setMargin( 0 );
    QSpacerItem* spacer = new QSpacerItem( 270, 20, QSizePolicy::Expanding, QSizePolicy::Fixed );
    hbox->addItem( spacer );

    buttonFind = new QPushButton( this, "buttonFind" );
    buttonFind->setProperty( "text", i18n( "&Find"  ) );
    buttonFind->setProperty( "default", QVariant( TRUE, 0 ) );
    hbox->addWidget( buttonFind );

    PushButton1 = new QPushButton( this, "PushButton1" );
    PushButton1->setProperty( "text", i18n( "&Close"  ) );
    hbox->addWidget( PushButton1 );

    grid->addMultiCellLayout( hbox, 5, 5, 0, 2 );

    cs = new QCheckBox( this, "cs" );
    cs->setProperty( "text", i18n( "C&ase Sensitive"  ) );

    grid->addWidget( cs, 3, 0 );

    wo = new QCheckBox( this, "wo" );
    wo->setProperty( "text", i18n( "&Whole words only"  ) );

    grid->addWidget( wo, 4, 0 );

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setProperty( "text", i18n( "&Search for:"  ) );

    grid->addMultiCellWidget( TextLabel1, 1, 1, 0, 2 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 1, QSizePolicy::Fixed, QSizePolicy::Expanding );
    grid->addItem( spacer_2, 0, 2 );

    // signals and slots connections
    connect( PushButton1, SIGNAL( clicked() ), this, SLOT( reject() ) );

    // buddies
    TextLabel1->setBuddy( lineEdit );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
SearchDialog::~SearchDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

