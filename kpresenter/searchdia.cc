/****************************************************************************
** Form implementation generated from reading ui file 'searchdia.ui'
**
** Created: Mon Jul 17 02:41:18 2000
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
    resize( 466, 158 ); 
    setProperty( "caption", i18n( "Find String"  ) );
    vbox = new QVBoxLayout( this ); 
    vbox->setSpacing( 6 );
    vbox->setMargin( 11 );
    QSpacerItem* spacer = new QSpacerItem( 20, 1, QSizePolicy::Fixed, QSizePolicy::Expanding );
    vbox->addItem( spacer );

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setProperty( "text", i18n( "&Search for:"  ) );
    vbox->addWidget( TextLabel1 );

    lineEdit = new QLineEdit( this, "lineEdit" );
    vbox->addWidget( lineEdit );

    hbox = new QHBoxLayout; 
    hbox->setSpacing( 6 );
    hbox->setMargin( 0 );

    cs = new QCheckBox( this, "cs" );
    cs->setProperty( "text", i18n( "C&ase Sensitive"  ) );
    hbox->addWidget( cs );

    back = new QCheckBox( this, "back" );
    back->setProperty( "text", i18n( "&Backwards"  ) );
    hbox->addWidget( back );
    vbox->addLayout( hbox );

    wo = new QCheckBox( this, "wo" );
    wo->setProperty( "text", i18n( "&Whole words only"  ) );
    vbox->addWidget( wo );

    hbox_2 = new QHBoxLayout; 
    hbox_2->setSpacing( 6 );
    hbox_2->setMargin( 0 );
    QSpacerItem* spacer_2 = new QSpacerItem( 270, 20, QSizePolicy::Expanding, QSizePolicy::Fixed );
    hbox_2->addItem( spacer_2 );

    buttonFind = new QPushButton( this, "buttonFind" );
    buttonFind->setProperty( "text", i18n( "&Find"  ) );
    buttonFind->setProperty( "default", QVariant( TRUE, 0 ) );
    hbox_2->addWidget( buttonFind );

    PushButton1 = new QPushButton( this, "PushButton1" );
    PushButton1->setProperty( "text", i18n( "&Close"  ) );
    hbox_2->addWidget( PushButton1 );
    vbox->addLayout( hbox_2 );

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

