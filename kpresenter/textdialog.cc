/****************************************************************************
** Form implementation generated from reading ui file 'textdialog.ui'
**
** Created: Mon Jul 17 02:42:01 2000
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "textdialog.h"

#include <kcolorbutton.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a TextDialog which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
TextDialog::TextDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setProperty( "name", "TextDialog" );
    resize( 477, 305 ); 
    setProperty( "caption", i18n( "Text Object Settings"  ) );
    setProperty( "sizeGripEnabled", QVariant( TRUE, 0 ) );
    vbox = new QVBoxLayout( this ); 
    vbox->setSpacing( 6 );
    vbox->setMargin( 11 );

    hbox = new QHBoxLayout; 
    hbox->setSpacing( 6 );
    hbox->setMargin( 0 );

    vbox_2 = new QVBoxLayout; 
    vbox_2->setSpacing( 6 );
    vbox_2->setMargin( 0 );

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setProperty( "title", i18n( "&Bullet Lists"  ) );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 0 );
    GroupBox1->layout()->setMargin( 0 );
    vbox_3 = new QVBoxLayout( GroupBox1->layout() );
    vbox_3->setAlignment( Qt::AlignTop );
    vbox_3->setSpacing( 6 );
    vbox_3->setMargin( 11 );

    hbox_2 = new QHBoxLayout; 
    hbox_2->setSpacing( 6 );
    hbox_2->setMargin( 0 );

    TextLabel1 = new QLabel( GroupBox1, "TextLabel1" );
    TextLabel1->setProperty( "text", i18n( "&1. Level:"  ) );
    hbox_2->addWidget( TextLabel1 );

    comboBullet1 = new QComboBox( FALSE, GroupBox1, "comboBullet1" );
    comboBullet1->insertItem( i18n( "Filled Circle" ) );
    comboBullet1->insertItem( i18n( "Filled Square" ) );
    comboBullet1->insertItem( i18n( "Outlined Circle" ) );
    comboBullet1->insertItem( i18n( "Outlined Square" ) );
    hbox_2->addWidget( comboBullet1 );

    colorBullet1 = new KColorButton( GroupBox1, "colorBullet1" );
    hbox_2->addWidget( colorBullet1 );
    vbox_3->addLayout( hbox_2 );

    hbox_3 = new QHBoxLayout; 
    hbox_3->setSpacing( 6 );
    hbox_3->setMargin( 0 );

    TextLabel1_2 = new QLabel( GroupBox1, "TextLabel1_2" );
    TextLabel1_2->setProperty( "text", i18n( "&2. Level:"  ) );
    hbox_3->addWidget( TextLabel1_2 );

    comboBullet2 = new QComboBox( FALSE, GroupBox1, "comboBullet2" );
    comboBullet2->insertItem( i18n( "Filled Circle" ) );
    comboBullet2->insertItem( i18n( "Filled Square" ) );
    comboBullet2->insertItem( i18n( "Outlined Circle" ) );
    comboBullet2->insertItem( i18n( "Outlined Square" ) );
    hbox_3->addWidget( comboBullet2 );

    colorBullet2 = new KColorButton( GroupBox1, "colorBullet2" );
    hbox_3->addWidget( colorBullet2 );
    vbox_3->addLayout( hbox_3 );

    hbox_4 = new QHBoxLayout; 
    hbox_4->setSpacing( 6 );
    hbox_4->setMargin( 0 );

    TextLabel1_3 = new QLabel( GroupBox1, "TextLabel1_3" );
    TextLabel1_3->setProperty( "text", i18n( "&3. Level:"  ) );
    hbox_4->addWidget( TextLabel1_3 );

    comboBullet3 = new QComboBox( FALSE, GroupBox1, "comboBullet3" );
    comboBullet3->insertItem( i18n( "Filled Circle" ) );
    comboBullet3->insertItem( i18n( "Filled Square" ) );
    comboBullet3->insertItem( i18n( "Outlined Circle" ) );
    comboBullet3->insertItem( i18n( "Outlined Square" ) );
    hbox_4->addWidget( comboBullet3 );

    colorBullet3 = new KColorButton( GroupBox1, "colorBullet3" );
    hbox_4->addWidget( colorBullet3 );
    vbox_3->addLayout( hbox_4 );

    hbox_5 = new QHBoxLayout; 
    hbox_5->setSpacing( 6 );
    hbox_5->setMargin( 0 );

    TextLabel1_4 = new QLabel( GroupBox1, "TextLabel1_4" );
    TextLabel1_4->setProperty( "text", i18n( "&4. Level:"  ) );
    hbox_5->addWidget( TextLabel1_4 );

    comboBullet4 = new QComboBox( FALSE, GroupBox1, "comboBullet4" );
    comboBullet4->insertItem( i18n( "Filled Circle" ) );
    comboBullet4->insertItem( i18n( "Filled Square" ) );
    comboBullet4->insertItem( i18n( "Outlined Circle" ) );
    comboBullet4->insertItem( i18n( "Outlined Square" ) );
    hbox_5->addWidget( comboBullet4 );

    colorBullet4 = new KColorButton( GroupBox1, "colorBullet4" );
    hbox_5->addWidget( colorBullet4 );
    vbox_3->addLayout( hbox_5 );
    vbox_2->addWidget( GroupBox1 );

    GroupBox2 = new QGroupBox( this, "GroupBox2" );
    GroupBox2->setProperty( "title", i18n( "&Enumerated lists"  ) );
    GroupBox2->setColumnLayout(0, Qt::Vertical );
    GroupBox2->layout()->setSpacing( 0 );
    GroupBox2->layout()->setMargin( 0 );
    hbox_6 = new QHBoxLayout( GroupBox2->layout() );
    hbox_6->setAlignment( Qt::AlignTop );
    hbox_6->setSpacing( 6 );
    hbox_6->setMargin( 11 );

    vbox_4 = new QVBoxLayout; 
    vbox_4->setSpacing( 6 );
    vbox_4->setMargin( 0 );

    TextLabel1_5 = new QLabel( GroupBox2, "TextLabel1_5" );
    TextLabel1_5->setProperty( "text", i18n( "&Type:"  ) );
    vbox_4->addWidget( TextLabel1_5 );

    TextLabel2 = new QLabel( GroupBox2, "TextLabel2" );
    TextLabel2->setProperty( "text", i18n( "&Color"  ) );
    vbox_4->addWidget( TextLabel2 );
    hbox_6->addLayout( vbox_4 );

    vbox_5 = new QVBoxLayout; 
    vbox_5->setSpacing( 6 );
    vbox_5->setMargin( 0 );

    comboEnumType = new QComboBox( FALSE, GroupBox2, "comboEnumType" );
    comboEnumType->insertItem( i18n( "Numbers" ) );
    comboEnumType->insertItem( i18n( "Characters" ) );
    vbox_5->addWidget( comboEnumType );

    enumColor = new KColorButton( GroupBox2, "enumColor" );
    vbox_5->addWidget( enumColor );
    hbox_6->addLayout( vbox_5 );
    vbox_2->addWidget( GroupBox2 );
    hbox->addLayout( vbox_2 );

    GroupBox3 = new QGroupBox( this, "GroupBox3" );
    GroupBox3->setProperty( "title", i18n( "&Spacing"  ) );
    GroupBox3->setColumnLayout(0, Qt::Vertical );
    GroupBox3->layout()->setSpacing( 0 );
    GroupBox3->layout()->setMargin( 0 );
    vbox_6 = new QVBoxLayout( GroupBox3->layout() );
    vbox_6->setAlignment( Qt::AlignTop );
    vbox_6->setSpacing( 6 );
    vbox_6->setMargin( 11 );

    TextLabel1_6 = new QLabel( GroupBox3, "TextLabel1_6" );
    TextLabel1_6->setProperty( "text", i18n( "&Lines-Spacing:"  ) );
    vbox_6->addWidget( TextLabel1_6 );

    spinLineSpacing = new QSpinBox( GroupBox3, "spinLineSpacing" );
    vbox_6->addWidget( spinLineSpacing );

    TextLabel1_6_2 = new QLabel( GroupBox3, "TextLabel1_6_2" );
    TextLabel1_6_2->setProperty( "text", i18n( "&Paragraph-Spacing:"  ) );
    vbox_6->addWidget( TextLabel1_6_2 );

    spinParagSpacing = new QSpinBox( GroupBox3, "spinParagSpacing" );
    vbox_6->addWidget( spinParagSpacing );

    TextLabel1_6_2_2 = new QLabel( GroupBox3, "TextLabel1_6_2_2" );
    TextLabel1_6_2_2->setProperty( "text", i18n( "&Margin"  ) );
    vbox_6->addWidget( TextLabel1_6_2_2 );

    spinMargin = new QSpinBox( GroupBox3, "spinMargin" );
    vbox_6->addWidget( spinMargin );
    hbox->addWidget( GroupBox3 );
    vbox->addLayout( hbox );

    hbox_7 = new QHBoxLayout; 
    hbox_7->setSpacing( 6 );
    hbox_7->setMargin( 0 );
    QSpacerItem* spacer = new QSpacerItem( 281, 20, QSizePolicy::Expanding, QSizePolicy::Fixed );
    hbox_7->addItem( spacer );

    PushButton2 = new QPushButton( this, "PushButton2" );
    PushButton2->setProperty( "text", i18n( "&OK"  ) );
    PushButton2->setProperty( "default", QVariant( TRUE, 0 ) );
    hbox_7->addWidget( PushButton2 );

    PushButton1 = new QPushButton( this, "PushButton1" );
    PushButton1->setProperty( "text", i18n( "&Cancel"  ) );
    hbox_7->addWidget( PushButton1 );
    vbox->addLayout( hbox_7 );

    // signals and slots connections
    connect( PushButton1, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( PushButton2, SIGNAL( clicked() ), this, SLOT( accept() ) );

    // buddies
    TextLabel1->setBuddy( comboBullet1 );
    TextLabel1_2->setBuddy( comboBullet2 );
    TextLabel1_3->setBuddy( comboBullet3 );
    TextLabel1_4->setBuddy( comboBullet4 );
    TextLabel1_5->setBuddy( comboEnumType );
    TextLabel2->setBuddy( enumColor );
    TextLabel1_6->setBuddy( spinLineSpacing );
    TextLabel1_6_2->setBuddy( spinParagSpacing );
    TextLabel1_6_2_2->setBuddy( spinMargin );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
TextDialog::~TextDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

