/******************************************************************/
/* KWord - (c) by Reginald Stadlbauer and Torben Weis 1997-1998   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer, Torben Weis                       */
/* E-Mail: reggie@kde.org, weis@kde.org                           */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Stylist                                                */
/******************************************************************/

#include "kword_doc.h"
#include "paraglayout.h"
#include "format.h"
#include "font.h"
#include "stylist.h"
#include "stylist.moc"
#include "defs.h"

#include <qwidget.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qgroupbox.h>
#include <qfont.h>
#include <qcolor.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qpen.h>
#include <qbrush.h>
#include <qstrlist.h>
#include <qcombobox.h>

#include <kbuttonbox.h>
#include <kapp.h>
#include <kcolordlg.h>
#include <kfontdialog.h>
#include <klocale.h>

/******************************************************************/
/* Class: KWStyleManager                                          */
/******************************************************************/

/*================================================================*/
KWStyleManager::KWStyleManager( QWidget *_parent, KWordDocument *_doc, QStrList _fontList )
	: QTabDialog( _parent, "", true )
{
	fontList = _fontList;
	doc = _doc;
	editor = 0L;

	setupTab1();
	setupTab2();

	setOkButton( i18n( "Close" ) );

	connect( this, SIGNAL( applyButtonPressed() ), this, SLOT( apply() ) );

	resize( 500, 400 );
}

/*================================================================*/
void KWStyleManager::setupTab1()
{
	tab1 = new QWidget( this );

	grid1 = new QGridLayout( tab1, 1, 2, 15, 7 );

	lStyleList = new QListBox( tab1 );
	for ( unsigned int i = 0; i < doc->paragLayoutList.count(); i++ )
		lStyleList->insertItem( doc->paragLayoutList.at( i )->getName() );
	connect( lStyleList, SIGNAL( selected( int ) ), this, SLOT( editStyle( int ) ) );
	grid1->addWidget( lStyleList, 0, 0 );

	bButtonBox = new KButtonBox( tab1, KButtonBox::VERTICAL );
	bAdd = bButtonBox->addButton( i18n( "&Add.." ), false );
	connect( bAdd, SIGNAL( clicked() ), this, SLOT( addStyle() ) );
	bDelete = bButtonBox->addButton( i18n( "&Delete" ), false );
	connect( bDelete, SIGNAL( clicked() ), this, SLOT( deleteStyle() ) );
	bButtonBox->addStretch();
	bEdit = bButtonBox->addButton( i18n( "&Edit..." ), false );
	connect( bEdit, SIGNAL( clicked() ), this, SLOT( editStyle() ) );
	bCopy = bButtonBox->addButton( i18n( "&Copy..." ), false );
	bButtonBox->addStretch();
	bUp = bButtonBox->addButton( i18n( "Up" ), false );
	bDown = bButtonBox->addButton( i18n( "D&own" ), false );
	bButtonBox->layout();
	grid1->addWidget( bButtonBox, 0, 1 );

	grid1->addColSpacing( 0, lStyleList->width() );
	grid1->addColSpacing( 1, bButtonBox->width() );
	grid1->setColStretch( 0, 1 );

	grid1->addRowSpacing( 0, lStyleList->height() );
	grid1->addRowSpacing( 0, bButtonBox->height() );
	grid1->setRowStretch( 0, 1 );

	grid1->activate();

	addTab( tab1, i18n( "Style Manager" ) );

	connect( lStyleList, SIGNAL( highlighted( const QString & ) ), this, SLOT( updateButtons( const QString & ) ) );
	lStyleList->setCurrentItem( 0 );
}

/*================================================================*/
void KWStyleManager::setupTab2()
{
	tab2 = new QWidget( this );

	grid2 = new QGridLayout( tab2, 8, 1, 15, 7 );

	cFont = new QComboBox( false, tab2 );
	cFont->insertItem( i18n( "Don't update Fonts" ) );
	cFont->insertItem( i18n( "Update Font Families of same sized Fonts" ) );
	cFont->insertItem( i18n( "Update Font Families of all sized Fonts" ) );
	cFont->insertItem( i18n( "Update Font Families and Attributes of same sized Fonts" ) );
	cFont->insertItem( i18n( "Update Font Families and Attributes of all sized Fonts" ) );
	cFont->resize( cFont->sizeHint() );
	grid2->addWidget( cFont, 0, 0 );

	cColor = new QComboBox( false, tab2 );
	cColor->insertItem( i18n( "Don't update Colors" ) );
	cColor->insertItem( i18n( "Update Colors" ) );
	cColor->resize( cFont->sizeHint() );
	grid2->addWidget( cColor, 1, 0 );

	cIndent = new QComboBox( false, tab2 );
	cIndent->insertItem( i18n( "Don't update Indents and Spacing" ) );
	cIndent->insertItem( i18n( "Update Indents and Spacing" ) );
	cIndent->resize( cFont->sizeHint() );
	grid2->addWidget( cIndent, 2, 0 );

	cAlign = new QComboBox( false, tab2 );
	cAlign->insertItem( i18n( "Don't update Aligns/Flows" ) );
	cAlign->insertItem( i18n( "Update Aligns/Flows" ) );
	cAlign->resize( cFont->sizeHint() );
	grid2->addWidget( cAlign, 3, 0 );

	cNumbering = new QComboBox( false, tab2 );
	cNumbering->insertItem( i18n( "Don't update Numbering" ) );
	cNumbering->insertItem( i18n( "Update Numbering" ) );
	cNumbering->resize( cFont->sizeHint() );
	grid2->addWidget( cNumbering, 4, 0 );

	cBorder = new QComboBox( false, tab2 );
	cBorder->insertItem( i18n( "Don't update Borders" ) );
	cBorder->insertItem( i18n( "Update Borders" ) );
	cBorder->resize( cFont->sizeHint() );
	grid2->addWidget( cBorder, 5, 0 );

	cTabs = new QComboBox( false, tab2 );
	cTabs->insertItem( i18n( "Don't update Tabulators" ) );
	cTabs->insertItem( i18n( "Update Tabulators" ) );
	cTabs->resize( cFont->sizeHint() );
	grid2->addWidget( cTabs, 6, 0 );

	grid2->addColSpacing( 0, cFont->width() );
	grid2->addColSpacing( 0, cAlign->width() );
	grid2->addColSpacing( 0, cColor->width() );
	grid2->addColSpacing( 0, cNumbering->width() );
	grid2->addColSpacing( 0, cIndent->width() );
	grid2->addColSpacing( 0, cBorder->width() );
	grid2->addColSpacing( 0, cTabs->width() );
	grid2->setColStretch( 0, 1 );

	grid2->addRowSpacing( 0, cFont->height() );
	grid2->addRowSpacing( 1, cColor->height() );
	grid2->addRowSpacing( 2, cIndent->height() );
	grid2->addRowSpacing( 3, cAlign->height() );
	grid2->addRowSpacing( 4, cNumbering->height() );
	grid2->addRowSpacing( 5, cBorder->height() );
	grid2->addRowSpacing( 6, cTabs->height() );
	grid2->setRowStretch( 0, 0 );
	grid2->setRowStretch( 1, 0 );
	grid2->setRowStretch( 2, 0 );
	grid2->setRowStretch( 3, 0 );
	grid2->setRowStretch( 4, 0 );
	grid2->setRowStretch( 5, 0 );
	grid2->setRowStretch( 6, 0 );
	grid2->setRowStretch( 7, 1 );

	grid2->activate();

	addTab( tab2, i18n( "Update Configuration" ) );

	if ( doc->getApplyStyleTemplate() & KWordDocument::U_FONT_FAMILY_SAME_SIZE )
		cFont->setCurrentItem( 1 );
	if ( doc->getApplyStyleTemplate() & KWordDocument::U_FONT_FAMILY_ALL_SIZE )
		cFont->setCurrentItem( 2 );
	if ( doc->getApplyStyleTemplate() & KWordDocument::U_FONT_ALL_SAME_SIZE )
		cFont->setCurrentItem( 3 );
	if ( doc->getApplyStyleTemplate() & KWordDocument::U_FONT_ALL_ALL_SIZE )
		cFont->setCurrentItem( 4 );
	if ( doc->getApplyStyleTemplate() & KWordDocument::U_COLOR )
		cColor->setCurrentItem( 1 );
	if ( doc->getApplyStyleTemplate() & KWordDocument::U_INDENT )
		cIndent->setCurrentItem( 1 );
	if ( doc->getApplyStyleTemplate() & KWordDocument::U_BORDER )
		cBorder->setCurrentItem( 1 );
	if ( doc->getApplyStyleTemplate() & KWordDocument::U_ALIGN )
		cAlign->setCurrentItem( 1 );
	if ( doc->getApplyStyleTemplate() & KWordDocument::U_NUMBERING )
		cNumbering->setCurrentItem( 1 );
	if ( doc->getApplyStyleTemplate() & KWordDocument::U_TABS )
		cTabs->setCurrentItem( 1 );
}

/*================================================================*/
void KWStyleManager::editStyle()
{
	if ( editor )
    {
		disconnect( editor, SIGNAL( updateStyleList() ), this, SLOT( updateStyleList() ) );
		delete editor;
		editor = 0L;
    }

	editor = new KWStyleEditor( 0L, doc->paragLayoutList.at( lStyleList->currentItem() ), doc, fontList );
	connect( editor, SIGNAL( updateStyleList() ), this, SLOT( updateStyleList() ) );
	editor->setCaption( i18n( "KWord - Stylist" ) );
	editor->show();
}

/*================================================================*/
void KWStyleManager::addStyle()
{
	QString str;
	str.sprintf( "%s ( %d )", i18n( "New Style Template" ).ascii(), doc->paragLayoutList.count() );
	( void )new KWParagLayout( doc, true, str );
	lStyleList->insertItem( str );
	lStyleList->setCurrentItem( lStyleList->count() - 1 );
	editStyle();
	doc->updateAllStyleLists();
}

/*================================================================*/
void KWStyleManager::deleteStyle()
{
	doc->paragLayoutList.remove( lStyleList->currentItem() );
	updateStyleList();
}

/*================================================================*/
void KWStyleManager::apply()
{
	int f = 0;

	if ( cFont->currentItem() == 1 )
		f = f | KWordDocument::U_FONT_FAMILY_SAME_SIZE;
	else if ( cFont->currentItem() == 2 )
		f = f | KWordDocument::U_FONT_FAMILY_ALL_SIZE;
	else if ( cFont->currentItem() == 3 )
		f = f | KWordDocument::U_FONT_ALL_SAME_SIZE;
	else if ( cFont->currentItem() == 4 )
		f = f | KWordDocument::U_FONT_ALL_ALL_SIZE;

	if ( cColor->currentItem() == 1 )
		f = f | KWordDocument::U_COLOR;
	if ( cAlign->currentItem() == 1 )
		f = f | KWordDocument::U_ALIGN;
	if ( cBorder->currentItem() == 1 )
		f = f | KWordDocument::U_BORDER;
	if ( cNumbering->currentItem() == 1 )
		f = f | KWordDocument::U_NUMBERING;
	if ( cIndent->currentItem() == 1 )
		f = f | KWordDocument::U_INDENT;
	if ( cTabs->currentItem() == 1 )
		f = f | KWordDocument::U_TABS;

	doc->setApplyStyleTemplate( f );
}

/*================================================================*/
void KWStyleManager::updateStyleList()
{
	lStyleList->clear();
	for ( unsigned int i = 0; i < doc->paragLayoutList.count(); i++ )
		lStyleList->insertItem( doc->paragLayoutList.at( i )->getName() );
	doc->updateAllStyleLists();
	lStyleList->setCurrentItem( 0 );
}

/*================================================================*/
void KWStyleManager::updateButtons( const QString &s )
{
	if ( s == QString( "Standard" ) ||
		 s == QString( "Head 1" ) ||
		 s == QString( "Head 2" ) ||
		 s == QString( "Head 3" ) ||
		 s == QString( "Enumerated List" ) ||
		 s == QString( "Bullet List" ) ||
		 s == QString( "Alphabetical List" ) )
		bDelete->setEnabled( false );
	else
		bDelete->setEnabled( true );
}

/******************************************************************/
/* Class: KWStylePreview                                          */
/******************************************************************/

/*================================================================*/
void KWStylePreview::drawContents( QPainter *painter )
{
	QRect r = contentsRect();
	QFontMetrics fm( font() );

	painter->fillRect( r.x() + fm.width( 'W' ), r.y() + fm.height(), r.width() - 2 * fm.width( 'W' ), r.height() - 2 * fm.height(), white );
	painter->setClipRect( r.x() + fm.width( 'W' ), r.y() + fm.height(), r.width() - 2 * fm.width( 'W' ), r.height() - 2 * fm.height() );

	QFont f( style->getFormat().getUserFont()->getFontName(), style->getFormat().getPTFontSize() );
	f.setBold( style->getFormat().getWeight() == 75 ? true : false );
	f.setItalic( static_cast<bool>( style->getFormat().getItalic() ) );
	f.setUnderline( static_cast<bool>( style->getFormat().getUnderline() ) );

	QColor c( style->getFormat().getColor() );

	painter->setPen( QPen( c ) );
	painter->setFont( f );

	fm = QFontMetrics( f );
	int y = height() / 2 - fm.height() / 2;

	painter->drawText( 20 + style->getFirstLineLeftIndent().pt() + style->getLeftIndent().pt(),
					   y, fm.width( i18n( "KWord, KOffice's Wordprocessor" ) ), fm.height(), 0, i18n( "KWord, KOffice's Wordprocessor" ) );
}

/******************************************************************/
/* Class: KWStyleEditor                                           */
/******************************************************************/

/*================================================================*/
KWStyleEditor::KWStyleEditor( QWidget *_parent, KWParagLayout *_style, KWordDocument *_doc, QStrList _fontList )
	: QTabDialog( _parent, "", true )
{
	fontList = _fontList;
	paragDia = 0;
	ostyle = _style;
	style = new KWParagLayout( _doc, false );
	*style = *_style;
	doc = _doc;
	setupTab1();

	setCancelButton( i18n( "Cancel" ) );
	setOkButton( i18n( "OK" ) );

	connect( this, SIGNAL( applyButtonPressed() ), this, SLOT( apply() ) );

	resize( 550, 400 );
}

/*================================================================*/
void KWStyleEditor::setupTab1()
{
	tab1 = new QWidget( this );

	grid1 = new QGridLayout( tab1, 2, 2, 15, 7 );

	nwid = new QWidget( tab1 );
	grid2 = new QGridLayout( nwid, 3, 2, 15, 7 );

	lName = new QLabel( i18n( "Name:" ), nwid );
	lName->resize( lName->sizeHint() );
	lName->setAlignment( AlignRight | AlignVCenter );
	grid2->addWidget( lName, 0, 0 );

	eName = new QLineEdit( nwid );
	eName->resize( eName->sizeHint() );
	eName->setText( style->getName() );
	grid2->addWidget( eName, 0, 1 );

	if ( style->getName() == QString( "Standard" ) ||
		 style->getName() == QString( "Head 1" ) ||
		 style->getName() == QString( "Head 2" ) ||
		 style->getName() == QString( "Head 3" ) ||
		 style->getName() == QString( "Enumerated List" ) ||
		 style->getName() == QString( "Bullet List" ) ||
		 style->getName() == QString( "Alphabetical List" ) )
		eName->setEnabled( false );

	lFollowing = new QLabel( i18n( "Following Style Template:" ), nwid );
	lFollowing->resize( lFollowing->sizeHint() );
	lFollowing->setAlignment( AlignRight | AlignVCenter );
	grid2->addWidget( lFollowing, 1, 0 );

	cFollowing = new QComboBox( false, nwid );
	for ( unsigned int i = 0; i < doc->paragLayoutList.count(); i++ )
    {
		cFollowing->insertItem( doc->paragLayoutList.at( i )->getName() );
		if ( doc->paragLayoutList.at( i )->getName() == style->getFollowingParagLayout() )
			cFollowing->setCurrentItem( i );
    }
	cFollowing->resize( cFollowing->sizeHint() );
	grid2->addWidget( cFollowing, 1, 1 );
	connect( cFollowing, SIGNAL( activated( const QString & ) ), this, SLOT( fplChanged( const QString & ) ) );

	grid2->addRowSpacing( 0, lName->height() );
	grid2->addRowSpacing( 0, eName->height() );
	grid2->addRowSpacing( 1, lFollowing->height() );
	grid2->addRowSpacing( 1, cFollowing->height() );
	grid2->setRowStretch( 0, 0 );
	grid2->setRowStretch( 0, 0 );
	grid2->setRowStretch( 2, 1 );

	grid2->addColSpacing( 0, lName->width() );
	grid2->addColSpacing( 0, lFollowing->width() );
	grid2->addColSpacing( 1, eName->width() );
	grid2->addColSpacing( 1, cFollowing->width() );
	grid2->setColStretch( 0, 0 );
	grid2->setColStretch( 1, 1 );

	grid2->activate();

	grid1->addWidget( nwid, 0, 0 );

	preview = new KWStylePreview( i18n( "Preview" ), tab1, style );
	grid1->addWidget( preview, 1, 0 );

	bButtonBox = new KButtonBox( tab1, KButtonBox::VERTICAL );
	bFont = bButtonBox->addButton( i18n( "&Font..." ), false );
	connect( bFont, SIGNAL( clicked() ), this, SLOT( changeFont() ) );
	bButtonBox->addStretch();
	bColor = bButtonBox->addButton( i18n( "&Color..." ), false );
	connect( bColor, SIGNAL( clicked() ), this, SLOT( changeColor() ) );
	bButtonBox->addStretch();
	bSpacing = bButtonBox->addButton( i18n( "&Spacing and Indents..." ), false );
	connect( bSpacing, SIGNAL( clicked() ), this, SLOT( changeSpacing() ) );
	bButtonBox->addStretch();
	bAlign = bButtonBox->addButton( i18n( "&Alignment..." ), false );
	connect( bAlign, SIGNAL( clicked() ), this, SLOT( changeAlign() ) );
	bButtonBox->addStretch();
	bBorders = bButtonBox->addButton( i18n( "&Borders..." ), false );
	connect( bBorders, SIGNAL( clicked() ), this, SLOT( changeBorders() ) );
	bButtonBox->addStretch();
	bNumbering = bButtonBox->addButton( i18n( "&Numbering..." ), false );
	connect( bNumbering, SIGNAL( clicked() ), this, SLOT( changeNumbering() ) );
	bButtonBox->addStretch();
	bTabulators = bButtonBox->addButton( i18n( "&Tabulators..." ), false );
	connect( bTabulators, SIGNAL( clicked() ), this, SLOT( changeTabulators() ) );
	bButtonBox->layout();
	grid1->addMultiCellWidget( bButtonBox, 0, 1, 1, 1 );

	grid1->addColSpacing( 0, nwid->width() );
	grid1->addColSpacing( 0, preview->width() );
	grid1->addColSpacing( 1, bButtonBox->width() );
	grid1->addColSpacing( 1, bSpacing->width() + 10 );
	grid1->setColStretch( 0, 1 );

	grid1->addRowSpacing( 0, nwid->height() );
	grid1->addRowSpacing( 1, 120 );
	grid1->addRowSpacing( 0, bButtonBox->height() / 3 );
	grid1->addRowSpacing( 1, 2 * bButtonBox->height() / 3 );
	grid1->setRowStretch( 1, 1 );

	grid1->activate();

	addTab( tab1, i18n( "Style Editor" ) );
}

/*================================================================*/
void KWStyleEditor::changeFont()
{
	QFont f( style->getFormat().getUserFont()->getFontName(), style->getFormat().getPTFontSize() );
	f.setBold( style->getFormat().getWeight() == 75 ? true : false );
	f.setItalic( static_cast<bool>( style->getFormat().getItalic() ) );
	f.setUnderline( static_cast<bool>( style->getFormat().getUnderline() ) );

	if ( KFontDialog::getFont( f ) )
    {
		style->getFormat().setUserFont( doc->findUserFont( f.family() ) );
		style->getFormat().setPTFontSize( f.pointSize() );
		style->getFormat().setWeight( f.bold() ? 75 : 50 );
		style->getFormat().setItalic( static_cast<int>( f.italic() ) );
		style->getFormat().setUnderline( static_cast<int>( f.underline() ) );
		preview->repaint( true );
    }
}

/*================================================================*/
void KWStyleEditor::changeColor()
{
	QColor c( style->getFormat().getColor() );
	if ( KColorDialog::getColor( c ) )
    {
		style->getFormat().setColor( c );
		preview->repaint( true );
    }
}

/*================================================================*/
void KWStyleEditor::changeSpacing()
{
	if ( paragDia )
    {
		disconnect( paragDia, SIGNAL( applyButtonPressed() ), this, SLOT( paragDiaOk() ) );
		paragDia->close();
		delete paragDia;
		paragDia = 0;
    }
	paragDia = new KWParagDia( 0, "", fontList, KWParagDia::PD_SPACING, doc );
	paragDia->setCaption( i18n( "KWord - Paragraph Spacing" ) );
	connect( paragDia, SIGNAL( applyButtonPressed() ), this, SLOT( paragDiaOk() ) );
	paragDia->setSpaceBeforeParag( style->getParagHeadOffset() );
	paragDia->setSpaceAfterParag( style->getParagFootOffset() );
	paragDia->setLineSpacing( style->getLineSpacing() );
	paragDia->setLeftIndent( style->getLeftIndent() );
	paragDia->setFirstLineIndent( style->getFirstLineLeftIndent() );
	paragDia->show();
}

/*================================================================*/
void KWStyleEditor::changeAlign()
{
	if ( paragDia )
    {
		disconnect( paragDia, SIGNAL( applyButtonPressed() ), this, SLOT( paragDiaOk() ) );
		paragDia->close();
		delete paragDia;
		paragDia = 0;
    }
	paragDia = new KWParagDia( 0, "", fontList, KWParagDia::PD_FLOW, doc );
	paragDia->setCaption( i18n( "KWord - Paragraph Flow ( Alignment )" ) );
	connect( paragDia, SIGNAL( applyButtonPressed() ), this, SLOT( paragDiaOk() ) );
	paragDia->setFlow( style->getFlow() );
	paragDia->show();
}

/*================================================================*/
void KWStyleEditor::changeBorders()
{
	if ( paragDia )
    {
		disconnect( paragDia, SIGNAL( applyButtonPressed() ), this, SLOT( paragDiaOk() ) );
		paragDia->close();
		delete paragDia;
		paragDia = 0;
    }
	paragDia = new KWParagDia( 0, "", fontList, KWParagDia::PD_BORDERS, doc );
	paragDia->setCaption( i18n( "KWord - Paragraph Borders" ) );
	connect( paragDia, SIGNAL( applyButtonPressed() ), this, SLOT( paragDiaOk() ) );
	paragDia->setLeftBorder( style->getLeftBorder() );
	paragDia->setRightBorder( style->getRightBorder() );
	paragDia->setTopBorder( style->getTopBorder() );
	paragDia->setBottomBorder( style->getBottomBorder() );
	paragDia->show();
}

/*================================================================*/
void KWStyleEditor::changeNumbering()
{
	if ( paragDia )
    {
		disconnect( paragDia, SIGNAL( applyButtonPressed() ), this, SLOT( paragDiaOk() ) );
		paragDia->close();
		delete paragDia;
		paragDia = 0;
    }
	paragDia = new KWParagDia( 0, "", fontList, KWParagDia::PD_NUMBERING, doc );
	paragDia->setCaption( i18n( "KWord - Numbering" ) );
	connect( paragDia, SIGNAL( applyButtonPressed() ), this, SLOT( paragDiaOk() ) );
	paragDia->setCounter( style->getCounter() );
	paragDia->show();
}

/*================================================================*/
void KWStyleEditor::changeTabulators()
{
	if ( paragDia )
    {
		disconnect( paragDia, SIGNAL( applyButtonPressed() ), this, SLOT( paragDiaOk() ) );
		paragDia->close();
		delete paragDia;
		paragDia = 0;
    }
	paragDia = new KWParagDia( 0, "", fontList, KWParagDia::PD_TABS, doc );
	paragDia->setCaption( i18n( "KWord - Tabulators" ) );
	connect( paragDia, SIGNAL( applyButtonPressed() ), this, SLOT( paragDiaOk() ) );
	paragDia->show();
}

/*================================================================*/
void KWStyleEditor::paragDiaOk()
{
	switch ( paragDia->getFlags() )
    {
    case KWParagDia::PD_SPACING:
	{
		style->setParagHeadOffset( paragDia->getSpaceBeforeParag() );
		style->setParagFootOffset( paragDia->getSpaceAfterParag() );
		style->setLineSpacing( paragDia->getLineSpacing() );
		style->setLeftIndent( paragDia->getLeftIndent() );
		style->setFirstLineLeftIndent( paragDia->getFirstLineIndent() );
	} break;
    case KWParagDia::PD_FLOW:
		style->setFlow( paragDia->getFlow() );
		break;
    case KWParagDia::PD_BORDERS:
	{
		style->setLeftBorder( paragDia->getLeftBorder() );
		style->setRightBorder( paragDia->getRightBorder() );
		style->setTopBorder( paragDia->getTopBorder() );
		style->setBottomBorder( paragDia->getBottomBorder() );
	} break;
    case KWParagDia::PD_NUMBERING:
		style->setCounter( paragDia->getCounter() );
		break;
    default: break;
    }

	preview->repaint( true );
}
/*================================================================*/
void KWStyleEditor::apply()
{
	*ostyle = *style;

	if ( eName->text() != style->getName() )
    {
		bool same = false;
		for ( unsigned int i = 0; i < doc->paragLayoutList.count(); i++ )
		{	
			if ( doc->paragLayoutList.at( i )->getName() == eName->text() )
				same = true;
		}

		if ( !same )
		{
			ostyle->setName( eName->text() );
			emit updateStyleList();
		}
    }
	doc->setStyleChanged( style->getName() );
}
