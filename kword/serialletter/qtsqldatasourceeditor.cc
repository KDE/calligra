#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './qtsqldatasourceeditor.ui'
**
** Created: jeu nov 8 07:48:34 2001
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/
#include "qtsqldatasourceeditor.h"

#include <qvariant.h>   // first for gcc 2.7.2
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qframe.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include "qtsqldatasourceeditor.ui.h"

/* 
 *  Constructs a QTSQLDataSourceEditor which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 */
QTSQLDataSourceEditor::QTSQLDataSourceEditor( QWidget* parent,  const char* name, WFlags fl )
    : QWidget( parent, name, fl )
{
    if ( !name )
	setName( "QTSQLDataSourceEditor" );
    resize( 493, 270 ); 
    setCaption( tr2i18n( "Form1" ) );
    QTSQLDataSourceEditorLayout = new QVBoxLayout( this, 11, 6, "QTSQLDataSourceEditorLayout"); 

    Layout5 = new QHBoxLayout( 0, 0, 6, "Layout5"); 

    TextLabel1 = new QLabel( this, "TextLabel1" );
    TextLabel1->setText( tr2i18n( "Table:" ) );
    Layout5->addWidget( TextLabel1 );

    tableCombo = new QComboBox( FALSE, this, "tableCombo" );
    Layout5->addWidget( tableCombo );

    filterCheckBox = new QCheckBox( this, "filterCheckBox" );
    filterCheckBox->setText( tr2i18n( "Filter output" ) );
    Layout5->addWidget( filterCheckBox );

    editFilter = new QPushButton( this, "editFilter" );
    editFilter->setText( tr2i18n( "View/Edit filterrules" ) );
    Layout5->addWidget( editFilter );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout5->addItem( spacer );
    QTSQLDataSourceEditorLayout->addLayout( Layout5 );

    Line1 = new QFrame( this, "Line1" );
    Line1->setProperty( "frameShape", (int)QFrame::HLine );
    Line1->setFrameShadow( QFrame::Sunken );
    Line1->setFrameShape( QFrame::HLine );
    QTSQLDataSourceEditorLayout->addWidget( Line1 );

    Layout6 = new QHBoxLayout( 0, 0, 6, "Layout6"); 

    TextLabel2 = new QLabel( this, "TextLabel2" );
    TextLabel2->setText( tr2i18n( "Used database records:" ) );
    Layout6->addWidget( TextLabel2 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout6->addItem( spacer_2 );
    QTSQLDataSourceEditorLayout->addLayout( Layout6 );

    DataTable = new QDataTable( this, "DataTable" );
    QTSQLDataSourceEditorLayout->addWidget( DataTable );

    // signals and slots connections
    connect( filterCheckBox, SIGNAL( toggled(bool) ), this, SLOT( filterCheckBox_toggled(bool) ) );
}

/*  
 *  Destroys the object and frees any allocated resources
 */
QTSQLDataSourceEditor::~QTSQLDataSourceEditor()
{
    // no need to delete child widgets, Qt does it all for us
}

/*  
 *  Widget polish.  Reimplemented to handle
 *  default data table initialization
 */
void QTSQLDataSourceEditor::polish()
{
    QWidget::polish();
}

#include "qtsqldatasourceeditor.moc"
