/****************************************************************************
** $Id$
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "textedit.h"

#include "../qrichtext_p.h"
#include <qtextedit.h>
#include <qtextstream.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qtabwidget.h>
#include <qapplication.h>
#include <qfontdatabase.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qfileinfo.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qprinter.h>
#include <qpaintdevicemetrics.h>
#include <qsimplerichtext.h>
#include <qcolordialog.h>

using namespace Qt3;

TextEdit::TextEdit( QWidget *parent, const char *name )
    : QMainWindow( parent, name )
{
    setupFileActions();
    setupEditActions();
    setupTextActions();

    tabWidget = new QTabWidget( this );
    connect( tabWidget, SIGNAL( currentChanged( QWidget * ) ),
	     this, SLOT( editorChanged( QWidget * ) ) );
    setCentralWidget( tabWidget );

    if ( qApp->argc() == 1 ) {
	load( "example.html" );
    } else {
	for ( int i = 1; i < qApp->argc(); ++i )
	    load( qApp->argv()[ i ] );
    }
}

void TextEdit::setupFileActions()
{
    QToolBar *tb = new QToolBar( this );
    tb->setLabel( "File Actions" );
    QPopupMenu *menu = new QPopupMenu( this );
    menuBar()->insertItem( tr( "&File" ), menu );

    QAction *a;
    a = new QAction( tr( "New" ), QPixmap( "filenew.xpm" ), tr( "&New..." ), CTRL + Key_N, this, "fileNew" );
    connect( a, SIGNAL( activated() ), this, SLOT( fileNew() ) );
    a->addTo( tb );
    a->addTo( menu );
    a = new QAction( tr( "Open" ), QPixmap( "fileopen.xpm" ), tr( "&Open..." ), CTRL + Key_O, this, "fileOpen" );
    connect( a, SIGNAL( activated() ), this, SLOT( fileOpen() ) );
    a->addTo( tb );
    a->addTo( menu );
    menu->insertSeparator();
    a = new QAction( tr( "Save" ), QPixmap( "filesave.xpm" ), tr( "&Save..." ), CTRL + Key_S, this, "fileSave" );
    connect( a, SIGNAL( activated() ), this, SLOT( fileSave() ) );
    a->addTo( tb );
    a->addTo( menu );
    a = new QAction( tr( "Save As" ), QPixmap(), tr( "Save &As..." ), 0, this, "fileSaveAs" );
    connect( a, SIGNAL( activated() ), this, SLOT( fileSaveAs() ) );
    a->addTo( menu );
    menu->insertSeparator();
    a = new QAction( tr( "Print" ), QPixmap( "fileprint.xpm" ), tr( "&Print..." ), CTRL + Key_P, this, "filePrint" );
    connect( a, SIGNAL( activated() ), this, SLOT( filePrint() ) );
    a->addTo( tb );
    a->addTo( menu );
    a = new QAction( tr( "Close" ), QPixmap(), tr( "&Close" ), 0, this, "fileClose" );
    connect( a, SIGNAL( activated() ), this, SLOT( fileClose() ) );
    a->addTo( menu );
    a = new QAction( tr( "Exit" ), QPixmap(), tr( "E&xit" ), 0, this, "fileExit" );
    connect( a, SIGNAL( activated() ), this, SLOT( fileExit() ) );
    a->addTo( menu );
}

void TextEdit::setupEditActions()
{
    QToolBar *tb = new QToolBar( this );
    tb->setLabel( "Edit Actions" );
    QPopupMenu *menu = new QPopupMenu( this );
    menuBar()->insertItem( tr( "&Edit" ), menu );

    QAction *a;
    a = new QAction( tr( "Undo" ), QPixmap( "editundo.xpm" ), tr( "&Undo" ), CTRL + Key_Z, this, "editUndo" );
    connect( a, SIGNAL( activated() ), this, SLOT( editUndo() ) );
    a->addTo( tb );
    a->addTo( menu );
    a = new QAction( tr( "Redo" ), QPixmap( "editredo.xpm" ), tr( "&Redo" ), CTRL + Key_Y, this, "editRedo" );
    connect( a, SIGNAL( activated() ), this, SLOT( editRedo() ) );
    a->addTo( tb );
    a->addTo( menu );
    menu->insertSeparator();
    a = new QAction( tr( "Copy" ), QPixmap( "editcopy.xpm" ), tr( "&Copy" ), CTRL + Key_C, this, "editCopy" );
    connect( a, SIGNAL( activated() ), this, SLOT( editCopy() ) );
    a->addTo( tb );
    a->addTo( menu );
    a = new QAction( tr( "Cut" ), QPixmap( "editcut.xpm" ), tr( "Cu&t" ), CTRL + Key_X, this, "editCut" );
    connect( a, SIGNAL( activated() ), this, SLOT( editCut() ) );
    a->addTo( tb );
    a->addTo( menu );
    a = new QAction( tr( "Paste" ), QPixmap( "editpaste.xpm" ), tr( "&Paste" ), CTRL + Key_V, this, "editPaste" );
    connect( a, SIGNAL( activated() ), this, SLOT( editPaste() ) );
    a->addTo( tb );
    a->addTo( menu );
}

void TextEdit::setupTextActions()
{
    QToolBar *tb = new QToolBar( this );
    tb->setLabel( "Format Actions" );
    QPopupMenu *menu = new QPopupMenu( this );
    menuBar()->insertItem( tr( "F&ormat" ), menu );

    comboStyle = new QComboBox( FALSE, tb );
    comboStyle->insertItem( "Standard" );
    comboStyle->insertItem( "Bullet List (Disc)" );
    comboStyle->insertItem( "Bullet List (Circle)" );
    comboStyle->insertItem( "Bullet List (Square)" );
    comboStyle->insertItem( "Ordered List (Decimal)" );
    comboStyle->insertItem( "Ordered List (Alpha lower)" );
    comboStyle->insertItem( "Ordered List (Alpha upper)" );
    connect( comboStyle, SIGNAL( activated( int ) ),
	     this, SLOT( textStyle( int ) ) );

    comboFont = new QComboBox( TRUE, tb );
    QFontDatabase db;
    comboFont->insertStringList( db.families() );
    connect( comboFont, SIGNAL( activated( const QString & ) ),
	     this, SLOT( textFamily( const QString & ) ) );
    comboFont->lineEdit()->setText( QApplication::font().family() );

    comboSize = new QComboBox( TRUE, tb );
    QValueList<int> sizes = db.standardSizes();
    QValueList<int>::Iterator it = sizes.begin();
    for ( ; it != sizes.end(); ++it )
	comboSize->insertItem( QString::number( *it ) );
    connect( comboSize, SIGNAL( activated( const QString & ) ),
	     this, SLOT( textSize( const QString & ) ) );
    comboSize->lineEdit()->setText( QString::number( QApplication::font().pointSize() ) );

    actionTextBold = new QAction( tr( "Bold" ), QPixmap( "textbold.xpm" ), tr( "&Bold" ), CTRL + Key_B, this, "textBold" );
    connect( actionTextBold, SIGNAL( activated() ), this, SLOT( textBold() ) );
    actionTextBold->addTo( tb );
    actionTextBold->addTo( menu );
    actionTextBold->setToggleAction( TRUE );
    actionTextItalic = new QAction( tr( "Italic" ), QPixmap( "textitalic.xpm" ), tr( "&Italic" ), CTRL + Key_I, this, "textItalic" );
    connect( actionTextItalic, SIGNAL( activated() ), this, SLOT( textItalic() ) );
    actionTextItalic->addTo( tb );
    actionTextItalic->addTo( menu );
    actionTextItalic->setToggleAction( TRUE );
    actionTextUnderline = new QAction( tr( "Underline" ), QPixmap( "textunder.xpm" ), tr( "&Underline" ), CTRL + Key_U, this, "textUnderline" );
    connect( actionTextUnderline, SIGNAL( activated() ), this, SLOT( textUnderline() ) );
    actionTextUnderline->addTo( tb );
    actionTextUnderline->addTo( menu );
    actionTextUnderline->setToggleAction( TRUE );
    menu->insertSeparator();

    QActionGroup *grp = new QActionGroup( this );
    grp->setExclusive( TRUE );
    connect( grp, SIGNAL( selected( QAction* ) ), this, SLOT( textAlign( QAction* ) ) );

    actionAlignLeft = new QAction( tr( "Left" ), QPixmap( "textleft.xpm" ), tr( "&Left" ), CTRL + Key_L, grp, "textLeft" );
    actionAlignLeft->addTo( tb );
    actionAlignLeft->addTo( menu );
    actionAlignLeft->setToggleAction( TRUE );
    actionAlignCenter = new QAction( tr( "Center" ), QPixmap( "textcenter.xpm" ), tr( "C&enter" ), CTRL + Key_E, grp, "textCenter" );
    actionAlignCenter->addTo( tb );
    actionAlignCenter->addTo( menu );
    actionAlignCenter->setToggleAction( TRUE );
    actionAlignRight = new QAction( tr( "Right" ), QPixmap( "textright.xpm" ), tr( "&Right" ), CTRL + Key_R, grp, "textRight" );
    actionAlignRight->addTo( tb );
    actionAlignRight->addTo( menu );
    actionAlignRight->setToggleAction( TRUE );
    actionAlignJustify = new QAction( tr( "Justify" ), QPixmap( "textjustify.xpm" ), tr( "&Justify" ), CTRL + Key_J, grp, "textjustify" );
    actionAlignJustify->addTo( tb );
    actionAlignJustify->addTo( menu );
    actionAlignJustify->setToggleAction( TRUE );

    menu->insertSeparator();

    QPixmap pix( 16, 16 );
    pix.fill( black );
    actionTextColor = new QAction( tr( "Color" ), pix, tr( "&Color..." ), 0, this, "textColor" );
    connect( actionTextColor, SIGNAL( activated() ), this, SLOT( textColor() ) );
    actionTextColor->addTo( tb );
    actionTextColor->addTo( menu );
}

void TextEdit::load( const QString &f )
{
    if ( !QFile::exists( f ) )
	return;
    QTextEdit *edit = new QTextEdit( tabWidget );
    doConnections( edit );
    tabWidget->addTab( edit, QFileInfo( f ).fileName() );
    QFile file( f );
    if ( !file.open( IO_ReadOnly ) )
	return;
    QTextStream ts( &file );
    edit->setText( ts.read() );
    tabWidget->showPage( edit );
    edit->viewport()->setFocus();
    filenames.replace( edit, f );
}

QTextEdit *TextEdit::currentEditor() const
{
    if ( tabWidget->currentPage() &&
	 tabWidget->currentPage()->inherits( "QTextEdit" ) )
	return (QTextEdit*)tabWidget->currentPage();
    return 0;	
}

void TextEdit::doConnections( QTextEdit *e )
{
    connect( e, SIGNAL( currentFontChanged( const QFont & ) ),
	     this, SLOT( fontChanged( const QFont & ) ) );
    connect( e, SIGNAL( currentColorChanged( const QColor & ) ),
	     this, SLOT( colorChanged( const QColor & ) ) );
    connect( e, SIGNAL( currentAlignmentChanged( int ) ),
	     this, SLOT( alignmentChanged( int ) ) );
}

void TextEdit::fileNew()
{
    QTextEdit *edit = new QTextEdit( tabWidget );
    edit->setTextFormat( RichText );
    doConnections( edit );
    tabWidget->addTab( edit, tr( "noname" ) );
    tabWidget->showPage( edit );
    edit->viewport()->setFocus();
}

void TextEdit::fileOpen()
{
    QString fn = QFileDialog::getOpenFileName( QString::null, tr( "HTML-Files (*.htm *.html);;All Files (*)" ), this );
    if ( !fn.isEmpty() )
	load( fn );
}

void TextEdit::fileSave()
{
    if ( !currentEditor() )
	return;
    QString fn;
    if ( filenames.find( currentEditor() ) == filenames.end() ) {
	fileSaveAs();
    } else {
	QFile file( *filenames.find( currentEditor() ) );
	if ( !file.open( IO_WriteOnly ) )
	    return;
	QTextStream ts( &file );
	ts << currentEditor()->text();
    }
}

void TextEdit::fileSaveAs()
{
    if ( !currentEditor() )
	return;
    QString fn = QFileDialog::getSaveFileName( QString::null, tr( "HTML-Files (*.htm *.html);;All Files (*)" ), this );
    if ( !fn.isEmpty() ) {
	filenames.replace( currentEditor(), fn );
	fileSave();
	// QT2HACK tabWidget->setTabLabel( currentEditor(), QFileInfo( fn ).fileName() );
    }
}

void TextEdit::filePrint()
{
    if ( !currentEditor() )
	return;
#if 0 // QT2HACK (QSimpleRichText problem)
#ifndef QT_NO_PRINTER
    QPrinter printer;
    printer.setFullPage(TRUE);
    if ( printer.setup( this ) ) {
	QPainter p( &printer );
	QPaintDeviceMetrics metrics( p.device() );
	int dpix = metrics.logicalDpiX();
	int dpiy = metrics.logicalDpiY();
	const int margin = 72; // pt
	QRect body( margin * dpix / 72, margin * dpiy / 72,
		    metrics.width() - margin * dpix / 72 * 2,
		    metrics.height() - margin * dpiy / 72 * 2 );
	QFont font( "times", 10 );
	QSimpleRichText richText( currentEditor()->text(), font, currentEditor()->context(), currentEditor()->styleSheet(),
				  currentEditor()->mimeSourceFactory(), body.height() );
	richText.setWidth( &p, body.width() );
	QRect view( body );
	int page = 1;
	do {
	    richText.draw( &p, body.left(), body.top(), view, colorGroup() );
	    view.moveBy( 0, body.height() );
	    p.translate( 0 , -body.height() );
	    p.setFont( font );
	    p.drawText( view.right() - p.fontMetrics().width( QString::number( page ) ),
			view.bottom() + p.fontMetrics().ascent() + 5, QString::number( page ) );
	    if ( view.top()  >= richText.height() )
		break;
	    printer.newPage();
	    page++;
	} while (TRUE);
    }
#endif
#endif
}

void TextEdit::fileClose()
{
    delete currentEditor();
    if ( currentEditor() )
	currentEditor()->viewport()->setFocus();				
}

void TextEdit::fileExit()
{
    qApp->quit();
}

void TextEdit::editUndo()
{
    if ( !currentEditor() )
	return;
    currentEditor()->undo();
}

void TextEdit::editRedo()
{
    if ( !currentEditor() )
	return;
    currentEditor()->redo();
}

void TextEdit::editCut()
{
    if ( !currentEditor() )
	return;
    currentEditor()->cut();
}

void TextEdit::editCopy()
{
    if ( !currentEditor() )
	return;
    currentEditor()->copy();
}

void TextEdit::editPaste()
{
    if ( !currentEditor() )
	return;
    currentEditor()->paste();
}

void TextEdit::textBold()
{
    if ( !currentEditor() )
	return;
    currentEditor()->setBold( actionTextBold->isOn() );
}

void TextEdit::textUnderline()
{
    if ( !currentEditor() )
	return;
    currentEditor()->setUnderline( actionTextUnderline->isOn() );
}

void TextEdit::textItalic()
{
    if ( !currentEditor() )
	return;
    currentEditor()->setItalic( actionTextItalic->isOn() );
}

void TextEdit::textFamily( const QString &f )
{
    if ( !currentEditor() )
	return;
    currentEditor()->setFamily( f );
    currentEditor()->viewport()->setFocus();
}

void TextEdit::textSize( const QString &p )
{
    if ( !currentEditor() )
	return;
    currentEditor()->setPointSize( p.toInt() );
    currentEditor()->viewport()->setFocus();
}

void TextEdit::textStyle( int i )
{
    if ( !currentEditor() )
	return;
    if ( i == 0 )
	currentEditor()->setParagType( QStyleSheetItem::DisplayBlock, QStyleSheetItem::ListDisc );
    else if ( i == 1 )
	currentEditor()->setParagType( QStyleSheetItem::DisplayListItem, QStyleSheetItem::ListDisc );
    else if ( i == 2 )
	currentEditor()->setParagType( QStyleSheetItem::DisplayListItem, QStyleSheetItem::ListCircle );
    else if ( i == 3 )
	currentEditor()->setParagType( QStyleSheetItem::DisplayListItem, QStyleSheetItem::ListSquare );
    else if ( i == 4 )
	currentEditor()->setParagType( QStyleSheetItem::DisplayListItem, QStyleSheetItem::ListDecimal );
    else if ( i == 5 )
	currentEditor()->setParagType( QStyleSheetItem::DisplayListItem, QStyleSheetItem::ListLowerAlpha );
    else if ( i == 6 )
	currentEditor()->setParagType( QStyleSheetItem::DisplayListItem, QStyleSheetItem::ListUpperAlpha );
    currentEditor()->viewport()->setFocus();
}

void TextEdit::textColor()
{
    if ( !currentEditor() )
	return;
    QColor col = QColorDialog::getColor( currentEditor()->color(), this );
    if ( !col.isValid() )
	return;
    currentEditor()->setColor( col );
    QPixmap pix( 16, 16 );
    pix.fill( black );
    actionTextColor->setIconSet( pix );
}

void TextEdit::textAlign( QAction *a )
{
    if ( !currentEditor() )
	return;
    if ( a == actionAlignLeft )
	currentEditor()->setAlignment( AlignLeft );
    else if ( a == actionAlignCenter )
	currentEditor()->setAlignment( AlignHCenter );
    else if ( a == actionAlignRight )
	currentEditor()->setAlignment( AlignRight );
    else if ( a == actionAlignJustify )
	currentEditor()->setAlignment( AlignJustify );
}

void TextEdit::fontChanged( const QFont &f )
{
    comboFont->lineEdit()->setText( f.family() );
    comboSize->lineEdit()->setText( QString::number( f.pointSize() ) );
    actionTextBold->setOn( f.bold() );
    actionTextItalic->setOn( f.italic() );
    actionTextUnderline->setOn( f.underline() );
}

void TextEdit::colorChanged( const QColor &c )
{
    QPixmap pix( 16, 16 );
    pix.fill( c );
    actionTextColor->setIconSet( pix );
}

void TextEdit::alignmentChanged( int a )
{
    if ( ( a == AlignAuto ) || ( a & AlignLeft ))
	actionAlignLeft->setOn( TRUE );
    else if ( ( a & AlignHCenter ) )
	actionAlignCenter->setOn( TRUE );
    else if ( ( a & AlignRight ) )
	actionAlignRight->setOn( TRUE );
    else if ( ( a & AlignJustify ) )
	actionAlignJustify->setOn( TRUE );
}

void TextEdit::editorChanged( QWidget * )
{
    if ( !currentEditor() )
	return;
    fontChanged( currentEditor()->font() );
    colorChanged( currentEditor()->color() );
    alignmentChanged( currentEditor()->alignment() );
}
