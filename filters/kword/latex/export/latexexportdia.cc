#include <klocale.h>
/****************************************************************************
** Form implementation generated from reading ui file './latexexportdia.ui'
**
** Created: sam fév 22 22:13:14 2003
**      by: The User Interface Compiler ()
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "latexexportdia.h"

#include <qvariant.h>
#include <kcombobox.h>
#include <klistbox.h>
#include <knuminput.h>
#include <kurlrequester.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

/* 
 *  Constructs a LatexExportDia as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
LatexExportDia::LatexExportDia( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )

{
    if ( !name )
	setName( "LatexExportDia" );
    setSizeGripEnabled( TRUE );
    LatexExportDiaLayout = new QGridLayout( this, 1, 1, 11, 6, "LatexExportDiaLayout"); 

    _tab = new QTabWidget( this, "_tab" );

    Widget2 = new QWidget( _tab, "Widget2" );

    typeGroup = new QButtonGroup( Widget2, "typeGroup" );
    typeGroup->setGeometry( QRect( 12, 86, 378, 75 ) );

    fullDocButton = new QRadioButton( typeGroup, "fullDocButton" );
    fullDocButton->setGeometry( QRect( 14, 21, 349, 23 ) );
    fullDocButton->setChecked( TRUE );

    embededButton = new QRadioButton( typeGroup, "embededButton" );
    embededButton->setGeometry( QRect( 14, 45, 351, 23 ) );

    styleGroup = new QButtonGroup( Widget2, "styleGroup" );
    styleGroup->setGeometry( QRect( 13, 9, 378, 73 ) );

    kwordStyleButton = new QRadioButton( styleGroup, "kwordStyleButton" );
    kwordStyleButton->setGeometry( QRect( 11, 43, 354, 23 ) );
    kwordStyleButton->setChecked( TRUE );

    latexStyleButton = new QRadioButton( styleGroup, "latexStyleButton" );
    latexStyleButton->setGeometry( QRect( 11, 20, 354, 23 ) );

    TextLabel1_2 = new QLabel( Widget2, "TextLabel1_2" );
    TextLabel1_2->setGeometry( QRect( 38, 174, 187, 18 ) );

    TextLabel2 = new QLabel( Widget2, "TextLabel2" );
    TextLabel2->setGeometry( QRect( 39, 208, 186, 18 ) );

    defaultFontSizeTextZone = new QLabel( Widget2, "defaultFontSizeTextZone" );
    defaultFontSizeTextZone->setGeometry( QRect( 39, 241, 186, 18 ) );

    classComboBox = new QComboBox( FALSE, Widget2, "classComboBox" );
    classComboBox->setGeometry( QRect( 240, 168, 125, 27 ) );

    defaultFontSize = new KIntNumInput( Widget2, "defaultFontSize" );
    defaultFontSize->setGeometry( QRect( 241, 236, 125, 29 ) );
    defaultFontSize->setValue( 12 );
    defaultFontSize->setMinValue( 0 );
    defaultFontSize->setMaxValue( 1000 );

    qualityComboBox = new KComboBox( FALSE, Widget2, "qualityComboBox" );
    qualityComboBox->setGeometry( QRect( 241, 201, 125, 28 ) );
    _tab->insertTab( Widget2, "" );

    Widget3 = new QWidget( _tab, "Widget3" );

    pathPictures = new KURLRequester( Widget3, "pathPictures" );
    pathPictures->setGeometry( QRect( 22, 80, 263, 42 ) );

    pictureCheckBox = new QCheckBox( Widget3, "pictureCheckBox" );
    pictureCheckBox->setGeometry( QRect( 21, 21, 262, 23 ) );
    pictureCheckBox->setChecked( TRUE );

    TextLabel1 = new QLabel( Widget3, "TextLabel1" );
    TextLabel1->setGeometry( QRect( 24, 58, 255, 18 ) );
    _tab->insertTab( Widget3, "" );

    tab = new QWidget( _tab, "tab" );

    ButtonGroup3 = new QButtonGroup( tab, "ButtonGroup3" );
    ButtonGroup3->setGeometry( QRect( 18, 17, 359, 73 ) );

    encodingComboBox = new KComboBox( FALSE, ButtonGroup3, "encodingComboBox" );
    encodingComboBox->setGeometry( QRect( 25, 31, 157, 26 ) );

    GroupBox4 = new QGroupBox( tab, "GroupBox4" );
    GroupBox4->setGeometry( QRect( 19, 102, 359, 150 ) );

    rmLanguageBtn = new QPushButton( GroupBox4, "rmLanguageBtn" );
    rmLanguageBtn->setGeometry( QRect( 147, 83, 92, 29 ) );

    addLanguageBtn = new QPushButton( GroupBox4, "addLanguageBtn" );
    addLanguageBtn->setGeometry( QRect( 147, 48, 92, 29 ) );

    languagesList = new KListBox( GroupBox4, "languagesList" );
    languagesList->setGeometry( QRect( 16, 30, 122, 105 ) );

    langUsedList = new KListBox( GroupBox4, "langUsedList" );
    langUsedList->setGeometry( QRect( 246, 30, 96, 101 ) );
    _tab->insertTab( tab, "" );

    LatexExportDiaLayout->addWidget( _tab, 0, 1 );

    Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1"); 

    buttonHelp = new QPushButton( this, "buttonHelp" );
    buttonHelp->setAccel( QKeySequence( 4144 ) );
    buttonHelp->setAutoDefault( TRUE );
    Layout1->addWidget( buttonHelp );
    QSpacerItem* spacer = new QSpacerItem( 20, 0, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( spacer );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setAccel( QKeySequence( 0 ) );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    Layout1->addWidget( buttonOk );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setAccel( QKeySequence( 0 ) );
    buttonCancel->setAutoDefault( TRUE );
    Layout1->addWidget( buttonCancel );

    LatexExportDiaLayout->addMultiCellLayout( Layout1, 1, 1, 0, 1 );
    languageChange();
    resize( QSize(434, 364).expandedTo(minimumSizeHint()) );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( addLanguageBtn, SIGNAL( clicked() ), this, SLOT( addLanguage() ) );
    connect( rmLanguageBtn, SIGNAL( clicked() ), this, SLOT( removeLanguage() ) );
}

/*
 *  Destroys the object and frees any allocated resources
 */
LatexExportDia::~LatexExportDia()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void LatexExportDia::languageChange()
{
    setCaption( tr2i18n( "Latex Export Filter configuration" ) );
    typeGroup->setTitle( tr2i18n( "Document Type" ) );
    fullDocButton->setText( tr2i18n( "Independent document" ) );
    QToolTip::add( fullDocButton, tr2i18n( "The document will be able to be compiled alone." ) );
    QWhatsThis::add( fullDocButton, tr2i18n( "The document will be generated as a full latex document since all the include will be generated before the \\begin[document} and \\end{document} commands." ) );
    embededButton->setText( tr2i18n( "Document to include" ) );
    QToolTip::add( embededButton, tr2i18n( "Don't generate neither the latex header nor the document environment." ) );
    QWhatsThis::add( embededButton, tr2i18n( "The document will be generated as a latex document which will have to incldue in a main latex document. It's allow you to generate several little files for each chapter of your document." ) );
    styleGroup->setTitle( tr2i18n( "Document Style" ) );
    kwordStyleButton->setText( tr2i18n( "KWord style" ) );
    latexStyleButton->setText( tr2i18n( "Latex style" ) );
    TextLabel1_2->setText( tr2i18n( "Document class :" ) );
    TextLabel2->setText( tr2i18n( "Quality :" ) );
    defaultFontSizeTextZone->setText( tr2i18n( "Default Font Size :" ) );
    classComboBox->clear();
    classComboBox->insertItem( tr2i18n( "article" ) );
    classComboBox->insertItem( tr2i18n( "book" ) );
    classComboBox->insertItem( tr2i18n( "letter" ) );
    classComboBox->insertItem( tr2i18n( "report" ) );
    classComboBox->insertItem( tr2i18n( "slides" ) );
    qualityComboBox->clear();
    qualityComboBox->insertItem( tr2i18n( "final" ) );
    qualityComboBox->insertItem( tr2i18n( "draft" ) );
    _tab->changeTab( Widget2, tr2i18n( "Document" ) );
    QToolTip::add( pathPictures, tr2i18n( "This directory will contains the eps pictures of your document." ) );
    pictureCheckBox->setText( tr2i18n( "Convert the pictures" ) );
    TextLabel1->setText( tr2i18n( "Pictures directory :" ) );
    _tab->changeTab( Widget3, tr2i18n( "Pictures" ) );
    ButtonGroup3->setTitle( tr2i18n( "Encoding" ) );
    encodingComboBox->clear();
    encodingComboBox->insertItem( tr2i18n( "unicode" ) );
    encodingComboBox->insertItem( tr2i18n( "ansinew" ) );
    encodingComboBox->insertItem( tr2i18n( "applemac" ) );
    encodingComboBox->insertItem( tr2i18n( "ascii" ) );
    encodingComboBox->insertItem( tr2i18n( "latin1" ) );
    encodingComboBox->insertItem( tr2i18n( "latin2" ) );
    encodingComboBox->insertItem( tr2i18n( "latin3" ) );
    encodingComboBox->insertItem( tr2i18n( "latin5" ) );
    encodingComboBox->insertItem( tr2i18n( "cp437" ) );
    encodingComboBox->insertItem( tr2i18n( "cp437de" ) );
    encodingComboBox->insertItem( tr2i18n( "cp850" ) );
    encodingComboBox->insertItem( tr2i18n( "cp852" ) );
    encodingComboBox->insertItem( tr2i18n( "cp865" ) );
    encodingComboBox->insertItem( tr2i18n( "cp1250" ) );
    encodingComboBox->insertItem( tr2i18n( "cp1252" ) );
    encodingComboBox->insertItem( tr2i18n( "decmulti" ) );
    encodingComboBox->insertItem( tr2i18n( "next" ) );
    GroupBox4->setTitle( tr2i18n( "Language" ) );
    rmLanguageBtn->setText( tr2i18n( "Remove" ) );
    addLanguageBtn->setText( tr2i18n( "Add" ) );
    languagesList->clear();
    languagesList->insertItem( tr2i18n( "american" ) );
    languagesList->insertItem( tr2i18n( "austrian" ) );
    languagesList->insertItem( tr2i18n( "bahasa" ) );
    languagesList->insertItem( tr2i18n( "brazil" ) );
    languagesList->insertItem( tr2i18n( "breton" ) );
    languagesList->insertItem( tr2i18n( "catalan" ) );
    languagesList->insertItem( tr2i18n( "croatian" ) );
    languagesList->insertItem( tr2i18n( "czech" ) );
    languagesList->insertItem( tr2i18n( "danish" ) );
    languagesList->insertItem( tr2i18n( "dutch" ) );
    languagesList->insertItem( tr2i18n( "english" ) );
    languagesList->insertItem( tr2i18n( "esperanto" ) );
    languagesList->insertItem( tr2i18n( "finnish" ) );
    languagesList->insertItem( tr2i18n( "francais" ) );
    languagesList->insertItem( tr2i18n( "french" ) );
    languagesList->insertItem( tr2i18n( "galician" ) );
    languagesList->insertItem( tr2i18n( "german" ) );
    languagesList->insertItem( tr2i18n( "germanb" ) );
    languagesList->insertItem( tr2i18n( "hungarian" ) );
    languagesList->insertItem( tr2i18n( "magyar" ) );
    languagesList->insertItem( tr2i18n( "italian" ) );
    languagesList->insertItem( tr2i18n( "norsk" ) );
    languagesList->insertItem( tr2i18n( "nynorsk" ) );
    languagesList->insertItem( tr2i18n( "polish" ) );
    languagesList->insertItem( tr2i18n( "portuges" ) );
    languagesList->insertItem( tr2i18n( "romanian" ) );
    languagesList->insertItem( tr2i18n( "russian" ) );
    languagesList->insertItem( tr2i18n( "spanish" ) );
    languagesList->insertItem( tr2i18n( "slovak" ) );
    languagesList->insertItem( tr2i18n( "slovene" ) );
    languagesList->insertItem( tr2i18n( "swedish" ) );
    languagesList->insertItem( tr2i18n( "turkish" ) );
    _tab->changeTab( tab, tr2i18n( "Language" ) );
    buttonHelp->setText( tr2i18n( "Help" ) );
    buttonOk->setText( tr2i18n( "OK" ) );
    buttonCancel->setText( tr2i18n( "Cancel" ) );
}

void LatexExportDia::addLanguage()
{
    qWarning( "LatexExportDia::addLanguage(): Not implemented yet" );
}

void LatexExportDia::removeLanguage()
{
    qWarning( "LatexExportDia::removeLanguage(): Not implemented yet" );
}

#include "latexexportdia.moc"
