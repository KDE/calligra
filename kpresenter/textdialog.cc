/****************************************************************************
** Form implementation generated from reading ui file 'textdialog.ui'
**
** Created: Sun Jul 16 20:35:24 2000
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
    resize( 469, 296 ); 
    setProperty( "caption", i18n( "Text Object Settings"  ) );
    setProperty( "sizeGripEnabled", QVariant( TRUE, 0 ) );
    grid = new QGridLayout( this ); 
    grid->setSpacing( 6 );
    grid->setMargin( 11 );

    GroupBox1 = new QGroupBox( this, "GroupBox1" );
    GroupBox1->setProperty( "title", i18n( "&Bullet Lists"  ) );
    GroupBox1->setColumnLayout(0, Qt::Vertical );
    GroupBox1->layout()->setSpacing( 0 );
    GroupBox1->layout()->setMargin( 0 );
    grid_2 = new QGridLayout( GroupBox1->layout() );
    grid_2->setAlignment( Qt::AlignTop );
    grid_2->setSpacing( 6 );
    grid_2->setMargin( 11 );

    TextLabel1 = new QLabel( GroupBox1, "TextLabel1" );
    TextLabel1->setProperty( "text", i18n( "&1. Level:"  ) );

    grid_2->addWidget( TextLabel1, 0, 0 );

    colorBullet1 = new KColorButton( GroupBox1, "colorBullet1" );

    grid_2->addWidget( colorBullet1, 0, 2 );

    colorBullet2 = new KColorButton( GroupBox1, "colorBullet2" );

    grid_2->addWidget( colorBullet2, 1, 2 );

    TextLabel1_2 = new QLabel( GroupBox1, "TextLabel1_2" );
    TextLabel1_2->setProperty( "text", i18n( "&2. Level:"  ) );

    grid_2->addWidget( TextLabel1_2, 1, 0 );

    colorBullet3 = new KColorButton( GroupBox1, "colorBullet3" );

    grid_2->addWidget( colorBullet3, 2, 2 );

    TextLabel1_3 = new QLabel( GroupBox1, "TextLabel1_3" );
    TextLabel1_3->setProperty( "text", i18n( "&3. Level:"  ) );

    grid_2->addWidget( TextLabel1_3, 2, 0 );

    TextLabel1_4 = new QLabel( GroupBox1, "TextLabel1_4" );
    TextLabel1_4->setProperty( "text", i18n( "4&. Level:"  ) );

    grid_2->addWidget( TextLabel1_4, 3, 0 );

    colorBullet4 = new KColorButton( GroupBox1, "colorBullet4" );

    grid_2->addWidget( colorBullet4, 3, 2 );

    comboBullet1 = new QComboBox( FALSE, GroupBox1, "comboBullet1" );
    comboBullet1->insertItem( i18n( "Filled Circle" ) );
    comboBullet1->insertItem( i18n( "Filled Square" ) );
    comboBullet1->insertItem( i18n( "Outlined Circle" ) );
    comboBullet1->insertItem( i18n( "Outlined Square" ) );

    grid_2->addWidget( comboBullet1, 0, 1 );

    comboBullet2 = new QComboBox( FALSE, GroupBox1, "comboBullet2" );
    comboBullet2->insertItem( i18n( "Filled Circle" ) );
    comboBullet2->insertItem( i18n( "Filled Square" ) );
    comboBullet2->insertItem( i18n( "Outlined Circle" ) );
    comboBullet2->insertItem( i18n( "Outlined Square" ) );

    grid_2->addWidget( comboBullet2, 1, 1 );

    comboBullet4 = new QComboBox( FALSE, GroupBox1, "comboBullet4" );
    comboBullet4->insertItem( i18n( "Filled Circle" ) );
    comboBullet4->insertItem( i18n( "Filled Square" ) );
    comboBullet4->insertItem( i18n( "Outlined Circle" ) );
    comboBullet4->insertItem( i18n( "Outlined Square" ) );

    grid_2->addWidget( comboBullet4, 3, 1 );

    comboBullet3 = new QComboBox( FALSE, GroupBox1, "comboBullet3" );
    comboBullet3->insertItem( i18n( "Filled Circle" ) );
    comboBullet3->insertItem( i18n( "Filled Square" ) );
    comboBullet3->insertItem( i18n( "Outlined Circle" ) );
    comboBullet3->insertItem( i18n( "Outlined Square" ) );

    grid_2->addWidget( comboBullet3, 2, 1 );

    grid->addWidget( GroupBox1, 0, 0 );

    GroupBox2 = new QGroupBox( this, "GroupBox2" );
    GroupBox2->setProperty( "title", i18n( "&Enumerated lists"  ) );
    GroupBox2->setColumnLayout(0, Qt::Vertical );
    GroupBox2->layout()->setSpacing( 0 );
    GroupBox2->layout()->setMargin( 0 );
    grid_3 = new QGridLayout( GroupBox2->layout() );
    grid_3->setAlignment( Qt::AlignTop );
    grid_3->setSpacing( 6 );
    grid_3->setMargin( 11 );

    TextLabel1_5 = new QLabel( GroupBox2, "TextLabel1_5" );
    TextLabel1_5->setProperty( "text", i18n( "&Type:"  ) );

    grid_3->addWidget( TextLabel1_5, 0, 0 );

    comboEnumType = new QComboBox( FALSE, GroupBox2, "comboEnumType" );
    comboEnumType->insertItem( i18n( "Numbers" ) );
    comboEnumType->insertItem( i18n( "Characters" ) );

    grid_3->addWidget( comboEnumType, 0, 1 );

    enumColor = new KColorButton( GroupBox2, "enumColor" );

    grid_3->addWidget( enumColor, 1, 1 );

    TextLabel2 = new QLabel( GroupBox2, "TextLabel2" );
    TextLabel2->setProperty( "text", i18n( "&Color"  ) );

    grid_3->addWidget( TextLabel2, 1, 0 );

    grid->addWidget( GroupBox2, 1, 0 );

    GroupBox3 = new QGroupBox( this, "GroupBox3" );
    GroupBox3->setProperty( "title", i18n( "&Spacing"  ) );
    GroupBox3->setColumnLayout(0, Qt::Vertical );
    GroupBox3->layout()->setSpacing( 0 );
    GroupBox3->layout()->setMargin( 0 );
    vbox = new QVBoxLayout( GroupBox3->layout() );
    vbox->setAlignment( Qt::AlignTop );
    vbox->setSpacing( 6 );
    vbox->setMargin( 11 );

    TextLabel1_6 = new QLabel( GroupBox3, "TextLabel1_6" );
    TextLabel1_6->setProperty( "text", i18n( "&Lines-Spacing:"  ) );
    vbox->addWidget( TextLabel1_6 );

    spinLineSpacing = new QSpinBox( GroupBox3, "spinLineSpacing" );
    vbox->addWidget( spinLineSpacing );

    TextLabel1_6_2 = new QLabel( GroupBox3, "TextLabel1_6_2" );
    TextLabel1_6_2->setProperty( "text", i18n( "&Paragraph-Spacing:"  ) );
    vbox->addWidget( TextLabel1_6_2 );

    spinParagSpacing = new QSpinBox( GroupBox3, "spinParagSpacing" );
    vbox->addWidget( spinParagSpacing );

    TextLabel1_6_2_2 = new QLabel( GroupBox3, "TextLabel1_6_2_2" );
    TextLabel1_6_2_2->setProperty( "text", i18n( "&Margin"  ) );
    vbox->addWidget( TextLabel1_6_2_2 );

    spinMargin = new QSpinBox( GroupBox3, "spinMargin" );
    vbox->addWidget( spinMargin );

    grid->addMultiCellWidget( GroupBox3, 0, 1, 1, 1 );

    hbox = new QHBoxLayout; 
    hbox->setSpacing( 6 );
    hbox->setMargin( 0 );
    QSpacerItem* spacer = new QSpacerItem( 273, 20, QSizePolicy::Expanding, QSizePolicy::Fixed );
    hbox->addItem( spacer );

    PushButton2 = new QPushButton( this, "PushButton2" );
    PushButton2->setProperty( "text", i18n( "&OK"  ) );
    PushButton2->setProperty( "default", QVariant( TRUE, 0 ) );
    hbox->addWidget( PushButton2 );

    PushButton1 = new QPushButton( this, "PushButton1" );
    PushButton1->setProperty( "text", i18n( "&Cancel"  ) );
    hbox->addWidget( PushButton1 );

    grid->addMultiCellLayout( hbox, 2, 2, 0, 1 );

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

