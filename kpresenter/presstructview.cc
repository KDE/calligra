/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Page Structure Viewer                                  */
/******************************************************************/

#include "kpresenter_doc.h"
#include "kpresenter_view.h"
#include "page.h"
#include "presstructview.h"
#include "presstructview.moc"
#include "kpobject.h"
#include "kpbackground.h"
#include "kplineobject.h"
#include "kprectobject.h"
#include "kpellipseobject.h"
#include "kpautoformobject.h"
#include "kpclipartobject.h"
#include "kptextobject.h"
#include "kppixmapobject.h"
#include "kppieobject.h"
#include "kppartobject.h"

#include <qevent.h>
#include <qpixmap.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qfileinfo.h>
#include <qpainter.h>
#include <qpen.h>
#include <qbrush.h>
#include <qfont.h>
#include <qfontmetrics.h>

#include <knewpanner.h>
#include <ktreelist.h>
#include <kapp.h>
#include <ktablistbox.h>
#include <ktoolbar.h>
#include <klocale.h>
#include <kiconloader.h>

/******************************************************************/
/* Class: PVTimeTable                                             */
/******************************************************************/

/*================================================================*/
PVTimeTable::PVTimeTable( QWidget *parent, KPresenterDoc *_doc )
	: QTableView( parent )
{
	doc = _doc;

	QFontMetrics fm( font() );

	unsigned int wid = 0;
	for ( unsigned int i = 0; i < NUM_OBJ_TYPES; i++ )
		wid = max(static_cast<int>(wid),fm.width(i18n(ObjName[i])) + fm.width("999"));

	wid += wid / 2;

	setCellWidth( wid );
	setCellHeight( fm.height() * 2 + fm.height() / 2 );

	setBackgroundColor( colorGroup().base() );
	setNumCols( 0 );
	setNumRows( 0 );
	page = -1;

	setTableFlags( tableFlags() | Tbl_autoVScrollBar | Tbl_autoHScrollBar | Tbl_clipCellPainting );

	objList.setAutoDelete( false );
}

/*================================================================*/
void PVTimeTable::setPageNum( int _num )
{
	objList.setAutoDelete( false );
	objList.clear();
	objList.setAutoDelete( false );
	page = _num;

	QList<int> intList;
	intList.setAutoDelete( false );
	intList = doc->reorderPage( page + 1, 0, 0, 1.0 );

	unsigned int objs = 0;
	for ( unsigned int i = 0; i < doc->objectList()->count(); i++ )
    {
		if ( doc->getPageOfObj( i, 0, 0, 1.0 ) == page + 1 )
		{
			objs++;
			objList.append( doc->objectList()->at( i ) );
		}
    }
	objs++;

	setNumRows( objs );
	setNumCols( intList.count() + 1 );

	if ( objs < 2 )
    {
		setNumCols( 0 );
		setNumRows( 0 );
    }

	repaint( true );
}


/*================================================================*/
void PVTimeTable::paintCell( QPainter *painter, int row, int col )
{
	if ( page >= 0 && page < static_cast<int>( doc->backgroundList()->count() ) )
    {
		painter->setPen( QPen( black ) );

		if ( row == 0 || col == 0 )
			painter->fillRect( 0, 0, cellWidth(), cellHeight(), colorGroup().background() );

		painter->setPen( colorGroup().text() );
		QFontMetrics fm( font() );
		QString obj_name;

		if ( row == 0 && col == 0 )
		{
			painter->drawText( cellWidth() - fm.width( i18n( "Step" ) ) - 3, fm.ascent() + 3, i18n( "Step" ) );
			painter->drawText( 3, cellHeight() - 3 - fm.descent(), i18n( "Object" ) );
			painter->drawLine( 0, 0, cellWidth(), cellHeight() );
		}
		if ( row == 0 && col > 0 )
		{
			QString str;
			str.sprintf( "%d", col );

			unsigned int wid = fm.width( str );
			unsigned int hei = fm.height();

			painter->drawText( ( cellWidth() - wid ) / 2, ( cellHeight() - hei ) / 2 + fm.ascent(), str ); 	
		}
		else if ( col == 0 && row > 0 )
		{
			obj_name.sprintf( "%s ( %d )", i18n( ObjName[ static_cast<int>( objList.at( row - 1 )->getType() ) ] ).ascii(),
							  doc->objectList()->find( objList.at( row - 1 ) ) + 1 );
			unsigned int wid = fm.width( obj_name );
			unsigned int hei = fm.height();

			painter->drawText( ( cellWidth() - wid ) / 2, ( cellHeight() - hei ) / 2 + fm.ascent(), obj_name ); 	
		}
		else if ( col > 0 && row > 0 )
		{
			KPObject *kpobject = objList.at( row - 1 );
			if ( kpobject->getPresNum() == col )
			{
				painter->fillRect( 0, 5, cellWidth(), cellHeight() - 10, green );
				painter->drawText( ( cellWidth() - fm.width( i18n( "appear" ) ) ) / 2, ( cellHeight() - fm.height() ) / 2 + fm.ascent(), i18n( "appear" ) );
			}
			else if ( kpobject->getPresNum() < col )
			{
				painter->fillRect( 0, 5, cellWidth(), cellHeight() - 10, red );
				painter->drawText( ( cellWidth() - fm.width( i18n( "stay" ) ) ) / 2, ( cellHeight() - fm.height() ) / 2 + fm.ascent(), i18n( "stay" ) );
			}
		}

		painter->setPen( black );
		painter->drawLine( cellWidth() - 1, 0, cellWidth() - 1, cellHeight() );
		painter->drawLine( 0, cellHeight() - 1, cellWidth(), cellHeight() - 1 );
		if ( row == 0 )
			painter->drawLine( 0, 0, cellWidth(), 0 );
		if ( col == 0 )
			painter->drawLine( 0, 0, 0, cellHeight() );
    }
}

/******************************************************************/
/* Class: PresStructViewer                                        */
/******************************************************************/

/*================================================================*/
PresStructViewer::PresStructViewer( QWidget *parent, const char *name, KPresenterDoc *_doc, KPresenterView *_view )
	: QDialog( parent, name, false )
{
	doc = _doc;
	view = _view;
	lastSelected = 0;

	pageList.setAutoDelete( true );
	objList.setAutoDelete( true );

	h_panner = new KNewPanner( this, "", KNewPanner::Horizontal, KNewPanner::Percent, 50 );
	panner = new KNewPanner( h_panner, "panner", KNewPanner::Vertical, KNewPanner::Percent, 30 );

	treelist = new KTreeList( panner, "" );
	setupTreeView();

	list = new KTabListBox( panner, "", 2 );
	list->setTableFlags( list->tableFlags() | Tbl_smoothHScrolling | Tbl_smoothVScrolling );
	list->setColumn( 0, i18n( "Description" ), 250 );
	list->setColumn( 1, i18n( "Value" ), 190 );

	setupToolBar();

	timeTable = new PVTimeTable( h_panner, doc );

	panner->activate( treelist, list );

	h_panner->activate( panner, timeTable );

	resize( 600, 400 );

	toolbar->move( 0, 0 );
	toolbar->resize( 600, toolbar->height() );

	h_panner->move( 0, toolbar->height() );
	h_panner->resize( 600, 400 - toolbar->height() );
}

/*================================================================*/
void PresStructViewer::itemSelected( int _index )
{
	KTreeListItem *item = treelist->itemAt( _index );
	QString text( item->getText() );

	// must be a page
	if ( text.contains( "Page", false ) > 0 )
    {
		ItemInfo *info = 0;
		for ( unsigned int i = 0; i < pageList.count(); i++ )
		{
			info = pageList.at( i );
			if ( info->item == item ) fillWithPageInfo( doc->backgroundList()->at( info->num ), info->num );
		}
    }

	// must be an object
	else
    {
		ItemInfo *info = 0;
		for ( unsigned int i = 0; i < objList.count(); i++ )
		{
			info = objList.at( i );
			if ( info->item == item ) fillWithObjInfo( doc->objectList()->at( info->num ), info->num );
		}
    }
}

/*================================================================*/
void PresStructViewer::resizeEvent( QResizeEvent *e )
{
	QDialog::resizeEvent( e );
	toolbar->resize( width(), toolbar->height() );
	h_panner->resize( width(), height() - toolbar->height() );
}

/*================================================================*/
void PresStructViewer::closeEvent( QCloseEvent *e )
{
	QDialog::closeEvent( e );
	emit presStructViewClosed();
}

/*================================================================*/
void PresStructViewer::setupTreeView()
{
	KTreeListItem *item = 0;
	ItemInfo *info = 0;

	QString page_name, obj_name;
	int pgnum;

	for ( unsigned int i = 0; i < doc->backgroundList()->count(); i++ )
    {
		page_name.sprintf( "%d. Page", i + 1 );
		item = new KTreeListItem( page_name.data(), new QPixmap( ICON( "filenew.xpm" ) ) );
		treelist->insertItem( item, -1, false );
		info = new ItemInfo;
		info->num = i;
		info->item = item;
		pageList.append( info );
    }

	KPObject *kpobject = 0;
	for ( unsigned int i = 0; i < doc->objectList()->count(); i++ )
    {
		kpobject = doc->objectList()->at( i );
		obj_name.sprintf( "%s ( %d )", i18n( ObjName[ static_cast<int>( kpobject->getType() ) ] ).ascii(), i + 1 );

		QString str;
		switch ( kpobject->getType() )
		{
		case OT_RECT: str = "mini_rect.xpm";
			break;
		case OT_PICTURE: str = "mini_picture.xpm";
			break;
		case OT_CLIPART: str = "mini_clipart.xpm";
			break;
		case OT_LINE: str = "mini_line.xpm";
			break;
		case OT_ELLIPSE: str = "mini_circle.xpm";
			break;
		case OT_TEXT: str = "mini_text.xpm";
			break;
		case OT_AUTOFORM: str = "mini_autoform.xpm";
			break;
		case OT_PIE: str = "mini_pie.xpm";
			break;
		default: str = "dot.xpm";
			break;
		}

		item = new KTreeListItem( obj_name.data(), new QPixmap( ICON( str ) ) );
		pgnum = doc->getPageOfObj( i, 0, 0 );
		if ( pgnum != -1 )
		{
			pgnum--;
			treelist->addChildItem( item, treelist->itemIndex( pageList.at( pgnum )->item ) ); ;
			info = new ItemInfo;
			info->num = i;
			info->item = item;
			objList.append( info );
		}
    }

	connect( treelist, SIGNAL( highlighted( int ) ), this, SLOT( itemSelected( int ) ) );
}

/*================================================================*/
void PresStructViewer::setupToolBar()
{
	toolbar = new KToolBar( this );

	//QString pixdir = KApplication::kde_datadir() + "/kpresenter/toolbar/";

	toolbar->insertButton( ICON( "style.xpm" ), B_STYLE, SIGNAL( clicked() ), this, SLOT( slotStyle() ), false, i18n( "Pen & Brush" ) );
	toolbar->insertButton( ICON( "rotate.xpm" ), B_ROTATE, SIGNAL( clicked() ), this, SLOT( slotRotate() ), false, i18n( "Rotate" ) );
	toolbar->insertButton( ICON( "shadow.xpm" ), B_SHADOW, SIGNAL( clicked() ), this, SLOT( slotShadow() ), false, i18n( "Shadow" ) );
	toolbar->insertButton( ICON( "alignobjs.xpm" ), B_ALIGN, SIGNAL( clicked() ), this, SLOT( slotAlign() ), false, i18n( "Align" ) );
	toolbar->insertButton( ICON( "effect.xpm" ), B_EFFECT, SIGNAL( clicked() ), this, SLOT( slotEffect() ), false, i18n( "Assign effect" ) );

	toolbar->insertSeparator();

	toolbar->insertButton( ICON( "raise.xpm" ), B_LOWER, SIGNAL( clicked() ), this, SLOT( slotRaise() ), false, i18n( "Raise" ) );
	toolbar->insertButton( ICON( "lower.xpm" ), B_RAISE, SIGNAL( clicked() ), this, SLOT( slotLower() ), false, i18n( "Lower" ) );

	toolbar->insertSeparator();

	toolbar->insertButton( ICON( "delete.xpm" ), B_DELETE, SIGNAL( clicked() ), this, SLOT( slotDelete() ), false, i18n( "Delete" ) );
	toolbar->insertButton( ICON( "edit_text.xpm" ), B_EDIT, SIGNAL( clicked() ), this, SLOT( slotEdit() ), false, i18n( "Edit Text" ) );
	toolbar->insertButton( ICON( "edit_pie.xpm" ), B_EDIT_PIE, SIGNAL( clicked() ), this, SLOT( slotEditPie() ), false, i18n( "Configure Pie" ) );
	toolbar->insertButton( ICON( "rectangle2.xpm" ), B_EDIT_RECT, SIGNAL( clicked() ), this, SLOT( slotEditRect() ),
						   false, i18n( "Configure Rectangle" ) );
	toolbar->insertButton( ICON( "fileopen.xpm" ), B_CFILEN,
						   SIGNAL( clicked() ), this, SLOT( slotChangeFilename() ), false, i18n( "Change Filename" ) );

	toolbar->insertSeparator();

	toolbar->insertButton( ICON( "screen.xpm" ), B_BACK, SIGNAL( clicked() ), this, SLOT( slotBackground() ), false, i18n( "Background" ) );
	toolbar->insertButton( ICON( "filenew.xpm" ), B_CPAGES,
						   SIGNAL( clicked() ), this, SLOT( slotConfigPages() ), false, i18n( "Configure Pages" ) );

	toolbar->setFullWidth( true );
	toolbar->enableFloating( false );
	toolbar->setBarPos( KToolBar::Top );
	toolbar->enableMoving( false );
	toolbar->updateRects();
}

/*================================================================*/
void PresStructViewer::fillWithPageInfo( KPBackGround *_page, int _num )
{
	QString str;
	int r, g, b;
	QColor c;

	list->setNumRows( 0 );
	str.sprintf( "%d", _num + 1 );
	list->appendItem( i18n( "Number" ) );
	list->changeItemPart( str, list->count() - 1, 1 );

	list->appendItem( i18n( "Back Type" ) );
	list->changeItemPart( i18n( BackTypeName[ static_cast<int>( _page->getBackType() ) ] ),
						  list->count() - 1, 1 );

	list->appendItem( i18n( "Back View" ) );
	list->changeItemPart( i18n( BackViewName[ static_cast<int>( _page->getBackView() ) ] ),
						  list->count() - 1, 1 );

	c = _page->getBackColor1();
	c.rgb( &r, &g, &b );
	str.sprintf( "#%02X%02X%02X", r, g, b );
	list->appendItem( i18n( "Color1" ) );
	list->changeItemPart( str, list->count() - 1, 1 );

	c = _page->getBackColor2();
	c.rgb( &r, &g, &b );
	str.sprintf( "#%02X%02X%02X", r, g, b );
	list->appendItem( i18n( "Color2" ) );
	list->changeItemPart( str, list->count() - 1, 1 );

	list->appendItem( i18n( "Picture Filename" ) );
	list->changeItemPart( _page->getBackPixFilename(), list->count() - 1, 1 );

	list->appendItem( i18n( "Clipart Filename" ) );
	list->changeItemPart( _page->getBackClipFilename(), list->count() - 1, 1 );

	list->appendItem( i18n( "Effect for changing to next page" ) );
	list->changeItemPart( i18n( PageEffectName[ static_cast<int>( _page->getPageEffect() ) ] ),
						  list->count() - 1, 1 );

	list->appendItem( i18n( "Speed for screenpresentations" ) );
	list->changeItemPart( i18n( PresSpeedName[ static_cast<int>( doc->getPresSpeed() ) ] ),
						  list->count() - 1, 1 );

	list->appendItem( i18n( "Manual switching to next page" ) );
	list->changeItemPart( i18n( YesNo[ static_cast<int>( !doc->spManualSwitch() ) ] ),
						  list->count() - 1, 1 );

	list->appendItem( i18n( "Infinit loop" ) );
	list->changeItemPart( i18n( YesNo[ static_cast<int>( !doc->spInfinitLoop() ) ] ),
						  list->count() - 1, 1 );

	view->skipToPage( _num );

	disableAllFunctions();
	toolbar->setItemEnabled( B_BACK, true );
	toolbar->setItemEnabled( B_CPAGES, true );

	timeTable->setPageNum( _num );
}

/*================================================================*/
void PresStructViewer::fillWithObjInfo( KPObject *_obj, int _num )
{
	QString str;

	list->setNumRows( 0 );
	str.sprintf( "%d", _num + 1 );
	list->appendItem( i18n( "Number" ) );
	list->changeItemPart( str, list->count() - 1, 1 );

	list->appendItem( i18n( "Type" ) );
	list->changeItemPart( i18n( ObjName[ static_cast<int>( _obj->getType() ) ] ),
						  list->count() - 1, 1 );

	str.sprintf( "%d", _obj->getOrig().x() );
	list->appendItem( i18n( "X-Coordinate" ) );
	list->changeItemPart( str, list->count() - 1, 1 );

	str.sprintf( "%d", _obj->getOrig().y() );
	list->appendItem( i18n( "Y-Coordinate" ) );
	list->changeItemPart( str, list->count() - 1, 1 );

	str.sprintf( "%d", _obj->getSize().width() );
	list->appendItem( i18n( "Width" ) );
	list->changeItemPart( str, list->count() - 1, 1 );

	str.sprintf( "%d", _obj->getSize().height() );
	list->appendItem( i18n( "Height" ) );
	list->changeItemPart( str, list->count() - 1, 1 );

	str.sprintf( "%g", _obj->getAngle() );
	list->appendItem( i18n( "Angle" ) );
	list->changeItemPart( str, list->count() - 1, 1 );

	str.sprintf( "%d", _obj->getShadowDistance() );
	list->appendItem( i18n( "Shadow Distance" ) );
	list->changeItemPart( str, list->count() - 1, 1 );

	list->appendItem( i18n( "Shadow Direction" ) );
	list->changeItemPart( i18n( ShadowDirectionName[ static_cast<int>( _obj->getShadowDirection() ) ] ),
						  list->count() - 1, 1 );

	str.sprintf( "%d", _obj->getPresNum() );
	list->appendItem( i18n( "Appearing number in the Presentation" ) );
	list->changeItemPart( str, list->count() - 1, 1 );

	list->appendItem( i18n( "Effect" ) );
	list->changeItemPart( i18n( EffectName[ static_cast<int>( _obj->getEffect() ) ] ),
						  list->count() - 1, 1 );

	list->appendItem( i18n( "Objectspecific Effect" ) );
	list->changeItemPart( i18n( Effect2Name[ static_cast<int>( _obj->getEffect2() ) ] ),
						  list->count() - 1, 1 );

	switch ( _obj->getType() )
    {
    case OT_RECT:
	{
		list->appendItem( i18n( "Pen Color" ) );
		if ( dynamic_cast<KPRectObject*>( _obj )->getPen().style() == NoPen )
			list->changeItemPart( "no pen", list->count() - 1, 1 );
		else
			list->changeItemPart( getColor( dynamic_cast<KPRectObject*>( _obj )->getPen().color() ), list->count() - 1, 1 );

		list->appendItem( i18n( "Pen Style" ) );
		list->changeItemPart( i18n( PenStyleName[ static_cast<int>( dynamic_cast<KPRectObject*>( _obj )->getPen().style() ) ] ),
							  list->count() - 1, 1 );

		str.sprintf( "%d", dynamic_cast<KPRectObject*>( _obj )->getPen().width() );
		list->appendItem( i18n( "Pen Width" ) );
		list->changeItemPart( str, list->count() - 1, 1 );

		if ( dynamic_cast<KPRectObject*>( _obj )->getFillType() == FT_BRUSH )
		{
			list->appendItem( i18n( "Brush Color" ) );
			if ( dynamic_cast<KPRectObject*>( _obj )->getBrush().style() == NoBrush )
				list->changeItemPart( "no brush", list->count() - 1, 1 );
			else
				list->changeItemPart( getColor( dynamic_cast<KPRectObject*>( _obj )->getBrush().color() ), list->count() - 1, 1 );
	
			list->appendItem( i18n( "Brush Style" ) );
			list->changeItemPart( i18n( BrushStyleName[ static_cast<int>( dynamic_cast<KPRectObject*>( _obj )->getBrush().style() ) ] ),
								  list->count() - 1, 1 );
		}
		else
		{
			list->appendItem( i18n( "Gradient Color1" ) );
			list->changeItemPart( getColor( dynamic_cast<KPRectObject*>( _obj )->getGColor1() ), list->count() - 1, 1 );

			list->appendItem( i18n( "Gradient Color2" ) );
			list->changeItemPart( getColor( dynamic_cast<KPRectObject*>( _obj )->getGColor2() ), list->count() - 1, 1 );

			list->appendItem( i18n( "Gradient Type" ) );
			list->changeItemPart( i18n( BackColorTypeName[ static_cast<int>( dynamic_cast<KPRectObject*>( _obj )->getGType() ) ] ),
								  list->count() - 1, 1 );
		}

		int rndx, rndy;
		dynamic_cast<KPRectObject*>( _obj )->getRnds( rndx, rndy );
	
		list->appendItem( i18n( "Roundedness X" ) );
		str.sprintf( "%d", rndx );
		list->changeItemPart( str, list->count() - 1, 1 );
	
		list->appendItem( i18n( "Roundedness Y" ) );
		str.sprintf( "%d", rndy );
		list->changeItemPart( str, list->count() - 1, 1 );
	} break;
    case OT_ELLIPSE:
	{
		list->appendItem( i18n( "Pen Color" ) );
		if ( dynamic_cast<KPEllipseObject*>( _obj )->getPen().style() == NoPen )
			list->changeItemPart( "no pen", list->count() - 1, 1 );
		else
			list->changeItemPart( getColor( dynamic_cast<KPEllipseObject*>( _obj )->getPen().color() ), list->count() - 1, 1 );

		list->appendItem( i18n( "Pen Style" ) );
		list->changeItemPart( i18n( PenStyleName[ static_cast<int>( dynamic_cast<KPEllipseObject*>( _obj )->getPen().style() ) ] ),
							  list->count() - 1, 1 );

		str.sprintf( "%d", dynamic_cast<KPEllipseObject*>( _obj )->getPen().width() );
		list->appendItem( i18n( "Pen Width" ) );
		list->changeItemPart( str, list->count() - 1, 1 );

		if ( dynamic_cast<KPEllipseObject*>( _obj )->getFillType() == FT_BRUSH )
		{
			list->appendItem( i18n( "Brush Color" ) );
			if ( dynamic_cast<KPEllipseObject*>( _obj )->getBrush().style() == NoBrush )
				list->changeItemPart( "no brush", list->count() - 1, 1 );
			else
				list->changeItemPart( getColor( dynamic_cast<KPEllipseObject*>( _obj )->getBrush().color() ), list->count() - 1, 1 );
	
			list->appendItem( i18n( "Brush Style" ) );
			list->changeItemPart( i18n( BrushStyleName[ static_cast<int>( dynamic_cast<KPEllipseObject*>( _obj )->getBrush().style() ) ] ),
								  list->count() - 1, 1 );
		}
		else
		{
			list->appendItem( i18n( "Gradient Color1" ) );
			list->changeItemPart( getColor( dynamic_cast<KPEllipseObject*>( _obj )->getGColor1() ), list->count() - 1, 1 );

			list->appendItem( i18n( "Gradient Color2" ) );
			list->changeItemPart( getColor( dynamic_cast<KPEllipseObject*>( _obj )->getGColor2() ), list->count() - 1, 1 );

			list->appendItem( i18n( "Gradient Type" ) );
			list->changeItemPart( i18n( BackColorTypeName[ static_cast<int>( dynamic_cast<KPEllipseObject*>( _obj )->getGType() ) ] ),
								  list->count() - 1, 1 );
		}
	} break;
    case OT_PIE:
	{
		list->appendItem( i18n( "Sub-Type" ) );
		list->changeItemPart( i18n( PieTypeName[ static_cast<int>( dynamic_cast<KPPieObject*>( _obj )->getPieType() ) ] ),
							  list->count() - 1, 1 );
		list->appendItem( i18n( "Pen Color" ) );
		if ( dynamic_cast<KPPieObject*>( _obj )->getPen().style() == NoPen )
			list->changeItemPart( "no pen", list->count() - 1, 1 );
		else
			list->changeItemPart( getColor( dynamic_cast<KPPieObject*>( _obj )->getPen().color() ), list->count() - 1, 1 );

		list->appendItem( i18n( "Pen Style" ) );
		list->changeItemPart( i18n( PenStyleName[ static_cast<int>( dynamic_cast<KPPieObject*>( _obj )->getPen().style() ) ] ),
							  list->count() - 1, 1 );

		str.sprintf( "%d", dynamic_cast<KPPieObject*>( _obj )->getPen().width() );
		list->appendItem( i18n( "Pen Width" ) );
		list->changeItemPart( str, list->count() - 1, 1 );

		if ( dynamic_cast<KPPieObject*>( _obj )->getFillType() == FT_BRUSH )
		{
			list->appendItem( i18n( "Brush Color" ) );
			if ( dynamic_cast<KPPieObject*>( _obj )->getBrush().style() == NoBrush )
				list->changeItemPart( "no brush", list->count() - 1, 1 );
			else
				list->changeItemPart( getColor( dynamic_cast<KPPieObject*>( _obj )->getBrush().color() ), list->count() - 1, 1 );
	
			list->appendItem( i18n( "Brush Style" ) );
			list->changeItemPart( i18n( BrushStyleName[ static_cast<int>( dynamic_cast<KPPieObject*>( _obj )->getBrush().style() ) ] ),
								  list->count() - 1, 1 );
		}
		else
		{
			list->appendItem( i18n( "Gradient Color1" ) );
			list->changeItemPart( getColor( dynamic_cast<KPPieObject*>( _obj )->getGColor1() ), list->count() - 1, 1 );

			list->appendItem( i18n( "Gradient Color2" ) );
			list->changeItemPart( getColor( dynamic_cast<KPPieObject*>( _obj )->getGColor2() ), list->count() - 1, 1 );

			list->appendItem( i18n( "Gradient Type" ) );
			list->changeItemPart( i18n( BackColorTypeName[ static_cast<int>( dynamic_cast<KPPieObject*>( _obj )->getGType() ) ] ),
								  list->count() - 1, 1 );
		}
		list->appendItem( i18n( "Line Begin" ) );
		list->changeItemPart( i18n( LineEndName[ static_cast<int>( dynamic_cast<KPPieObject*>( _obj )->getLineBegin() ) ] ),
							  list->count() - 1, 1 );

		list->appendItem( i18n( "Line End" ) );
		list->changeItemPart( i18n( LineEndName[ static_cast<int>( dynamic_cast<KPPieObject*>( _obj )->getLineEnd() ) ] ),
							  list->count() - 1, 1 );

		str.sprintf( "%d ( =%g deg. )", dynamic_cast<KPPieObject*>( _obj )->getPieAngle(),
					 static_cast<float>( dynamic_cast<KPPieObject*>( _obj )->getPieAngle() ) / 16.0 );
		list->appendItem( i18n( "Pie/Arc/Chord Angle" ) );
		list->changeItemPart( str, list->count() - 1, 1 );

		str.sprintf( "%d ( =%g deg. )", dynamic_cast<KPPieObject*>( _obj )->getPieLength(),
					 static_cast<float>( dynamic_cast<KPPieObject*>( _obj )->getPieLength() ) / 16.0 );
		list->appendItem( i18n( "Pie/Arc/Chord Length" ) );
		list->changeItemPart( str, list->count() - 1, 1 );
	} break;
    case OT_LINE:
	{
		list->appendItem( i18n( "Direction" ) );
		list->changeItemPart( i18n( LineTypeName[ static_cast<int>( dynamic_cast<KPLineObject*>( _obj )->getLineType() ) ] ),
							  list->count() - 1, 1 );

		list->appendItem( i18n( "Pen Color" ) );
		if ( dynamic_cast<KPLineObject*>( _obj )->getPen().style() == NoPen )
			list->changeItemPart( "no pen", list->count() - 1, 1 );
		else
			list->changeItemPart( getColor( dynamic_cast<KPLineObject*>( _obj )->getPen().color() ), list->count() - 1, 1 );

		list->appendItem( i18n( "Pen Style" ) );
		list->changeItemPart( i18n( PenStyleName[ static_cast<int>( dynamic_cast<KPLineObject*>( _obj )->getPen().style() ) ] ),
							  list->count() - 1, 1 );

		str.sprintf( "%d", dynamic_cast<KPLineObject*>( _obj )->getPen().width() );
		list->appendItem( i18n( "Pen Width" ) );
		list->changeItemPart( str, list->count() - 1, 1 );

		list->appendItem( i18n( "Line Begin" ) );
		list->changeItemPart( i18n( LineEndName[ static_cast<int>( dynamic_cast<KPLineObject*>( _obj )->getLineBegin() ) ] ),
							  list->count() - 1, 1 );

		list->appendItem( i18n( "Line End" ) );
		list->changeItemPart( i18n( LineEndName[ static_cast<int>( dynamic_cast<KPLineObject*>( _obj )->getLineEnd() ) ] ),
							  list->count() - 1, 1 );
	} break;
    case OT_AUTOFORM:
	{
		list->appendItem( i18n( "Name" ) );

		QString filename = dynamic_cast<KPAutoformObject*>( _obj )->getFileName();
		QFileInfo fi( filename );
		list->changeItemPart( fi.baseName(), list->count() - 1, 1 );

		list->appendItem( i18n( "Pen Color" ) );
		if ( dynamic_cast<KPAutoformObject*>( _obj )->getPen().style() == NoPen )
			list->changeItemPart( "no pen", list->count() - 1, 1 );
		else
			list->changeItemPart( getColor( dynamic_cast<KPAutoformObject*>( _obj )->getPen().color() ), list->count() - 1, 1 );

		list->appendItem( i18n( "Pen Style" ) );
		list->changeItemPart( i18n( PenStyleName[ static_cast<int>( dynamic_cast<KPAutoformObject*>( _obj )->getPen().style() ) ] ),
							  list->count() - 1, 1 );

		str.sprintf( "%d", dynamic_cast<KPAutoformObject*>( _obj )->getPen().width() );
		list->appendItem( i18n( "Pen Width" ) );
		list->changeItemPart( str, list->count() - 1, 1 );

		list->appendItem( i18n( "Line Begin" ) );
		list->changeItemPart( i18n( LineEndName[ static_cast<int>( dynamic_cast<KPAutoformObject*>( _obj )->getLineBegin() ) ] ),
							  list->count() - 1, 1 );

		list->appendItem( i18n( "Line End" ) );
		list->changeItemPart( i18n( LineEndName[ static_cast<int>( dynamic_cast<KPAutoformObject*>( _obj )->getLineEnd() ) ] ),
							  list->count() - 1, 1 );

		if ( dynamic_cast<KPAutoformObject*>( _obj )->getFillType() == FT_BRUSH )
		{
			list->appendItem( i18n( "Brush Color" ) );
			if ( dynamic_cast<KPAutoformObject*>( _obj )->getBrush().style() == NoBrush )
				list->changeItemPart( "no brush", list->count() - 1, 1 );
			else
				list->changeItemPart( getColor( dynamic_cast<KPAutoformObject*>( _obj )->getBrush().color() ), list->count() - 1, 1 );
	
			list->appendItem( i18n( "Brush Style" ) );
			list->changeItemPart( i18n( BrushStyleName[ static_cast<int>( dynamic_cast<KPAutoformObject*>( _obj )->getBrush().style() ) ] ),
								  list->count() - 1, 1 );
		}
		else
		{
			list->appendItem( i18n( "Gradient Color1" ) );
			list->changeItemPart( getColor( dynamic_cast<KPAutoformObject*>( _obj )->getGColor1() ), list->count() - 1, 1 );

			list->appendItem( i18n( "Gradient Color2" ) );
			list->changeItemPart( getColor( dynamic_cast<KPAutoformObject*>( _obj )->getGColor2() ), list->count() - 1, 1 );

			list->appendItem( i18n( "Gradient Type" ) );
			list->changeItemPart( i18n( BackColorTypeName[ static_cast<int>( dynamic_cast<KPAutoformObject*>( _obj )->getGType() ) ] ),
								  list->count() - 1, 1 );
		}
	} break;
    case OT_PICTURE:
	{
		list->appendItem( i18n( "Filename" ) );
		list->changeItemPart( dynamic_cast<KPPixmapObject*>( _obj )->getFileName(), list->count() - 1, 1 );
	} break;
    case OT_CLIPART:
	{
		list->appendItem( i18n( "Filename" ) );
		list->changeItemPart( dynamic_cast<KPClipartObject*>( _obj )->getFileName(), list->count() - 1, 1 );
	} break;
    default: break;
    }

	if ( lastSelected )
    {
		lastSelected->setSelected( false );
		doc->repaint( lastSelected );
    }

	view->makeRectVisible( _obj->getBoundingRect( 0, 0 ) );
	_obj->setSelected( true );
	doc->repaint( _obj );
	lastSelected = _obj;

	disableAllFunctions();

	if ( _obj->getType() == OT_TEXT )
		toolbar->setItemEnabled( B_EDIT, true );
	else if ( _obj->getType() == OT_CLIPART || _obj->getType() == OT_PICTURE )
		toolbar->setItemEnabled( B_CFILEN, true );
	else
		toolbar->setItemEnabled( B_STYLE, true );
	if ( _obj->getType() == OT_PIE )
		toolbar->setItemEnabled( B_EDIT_PIE, true );
	if ( _obj->getType() == OT_RECT )
		toolbar->setItemEnabled( B_EDIT_RECT, true );

	toolbar->setItemEnabled( B_ROTATE, true );
	toolbar->setItemEnabled( B_SHADOW, true );
	toolbar->setItemEnabled( B_ALIGN, true );
	toolbar->setItemEnabled( B_EFFECT, true );
	toolbar->setItemEnabled( B_LOWER, true );
	toolbar->setItemEnabled( B_RAISE, true );
	toolbar->setItemEnabled( B_DELETE, true );
}

/*================================================================*/
QString PresStructViewer::getColor( QColor _color )
{
	QFile rgbFile( "/usr/X11R6/lib/X11/rgb.txt" );
	QString str;

	if ( rgbFile.exists() )
    {
		int r, g, b;
		str.sprintf( "( %d, %d, %d )", _color.red(), _color.green(), _color.blue() );

		QTextStream t( &rgbFile );
		QString s = "";

		char name[ 255 ];

		if ( rgbFile.open( IO_ReadOnly ) )
		{
			while ( !t.eof() )
			{
				s = t.readLine();
				sscanf( s, "%d %d %d %s", &r, &g, &b, name );
				if ( r == _color.red() && g == _color.green() && b == _color.blue() )
				{
					str.sprintf( "%s", name );
					str = str.stripWhiteSpace();
					break;
				}
			}
			rgbFile.close();
		}
    }
	else
		str.sprintf( "( %d, %d, %d )", _color.red(), _color.green(), _color.blue() );

	return str;
}

/*================================================================*/
void PresStructViewer::slotStyle()
{
	view->extraPenBrush();
	fillWithObjInfo( lastSelected, doc->objectList()->findRef( lastSelected ) );
}

/*================================================================*/
void PresStructViewer::slotRotate()
{
	view->extraRotate();
	fillWithObjInfo( lastSelected, doc->objectList()->findRef( lastSelected ) );
}

/*================================================================*/
void PresStructViewer::slotShadow()
{
	view->extraShadow();
	fillWithObjInfo( lastSelected, doc->objectList()->findRef( lastSelected ) );
}

/*================================================================*/
void PresStructViewer::slotAlign()
{
	view->extraAlignObj();
	fillWithObjInfo( lastSelected, doc->objectList()->findRef( lastSelected ) );
}

/*================================================================*/
void PresStructViewer::slotEffect()
{
	view->screenAssignEffect();
	fillWithObjInfo( lastSelected, doc->objectList()->findRef( lastSelected ) );
}

/*================================================================*/
void PresStructViewer::slotLower()
{
	view->extraLower();
	fillWithObjInfo( lastSelected, doc->objectList()->findRef( lastSelected ) );
}

/*================================================================*/
void PresStructViewer::slotRaise()
{
	view->extraRaise();
	fillWithObjInfo( lastSelected, doc->objectList()->findRef( lastSelected ) );
}

/*================================================================*/
void PresStructViewer::slotDelete()
{
	view->editDelete();
	view->restartPresStructView();
}

/*================================================================*/
void PresStructViewer::slotEdit()
{
	view->getPage()->editSelectedTextArea();
}

/*================================================================*/
void PresStructViewer::slotEditPie()
{
	view->extraConfigPie();
}

/*================================================================*/
void PresStructViewer::slotEditRect()
{
	view->extraConfigRect();
}

/*================================================================*/
void PresStructViewer::slotBackground()
{
	view->extraBackground();
}

/*================================================================*/
void PresStructViewer::slotConfigPages()
{
	view->screenConfigPages();
}

/*================================================================*/
void PresStructViewer::slotChangeFilename()
{
	if ( lastSelected )
    {
		if ( lastSelected->getType() == OT_PICTURE )
			view->getPage()->chPic();
		else
			view->getPage()->chClip();
    }
	fillWithObjInfo( lastSelected, doc->objectList()->findRef( lastSelected ) );
}

/*================================================================*/
void PresStructViewer::disableAllFunctions()
{
	toolbar->setItemEnabled( B_STYLE, false );
	toolbar->setItemEnabled( B_ROTATE, false );
	toolbar->setItemEnabled( B_SHADOW, false );
	toolbar->setItemEnabled( B_ALIGN, false );
	toolbar->setItemEnabled( B_EFFECT, false );
	toolbar->setItemEnabled( B_LOWER, false );
	toolbar->setItemEnabled( B_RAISE, false );
	toolbar->setItemEnabled( B_DELETE, false );
	toolbar->setItemEnabled( B_EDIT, false );
	toolbar->setItemEnabled( B_BACK, false );
	toolbar->setItemEnabled( B_CPAGES, false );
	toolbar->setItemEnabled( B_CFILEN, false );
	toolbar->setItemEnabled( B_EDIT_PIE, false );
	toolbar->setItemEnabled( B_EDIT_RECT, false );
}
