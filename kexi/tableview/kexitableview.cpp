/* This file is part of the KDE project
   Copyright (C) 2002 Till Busch <till@bux.at>
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Original Author:  Till Busch <till@bux.at>
   Original Project: buX (www.bux.at)
*/

#include <qpainter.h>
#include <qkeycode.h>
#include <qheader.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qwmatrix.h>
#include <qtimer.h>
#include <qpopupmenu.h>
#include <qcursor.h>
#include <qstyle.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

#include <unistd.h>

#include <config.h>

#include <kglobal.h>
#include <klocale.h>
#include <kdebug.h>
#include <kapp.h>
#include <kiconloader.h>
#include <kmessagebox.h>

#ifndef KEXI_NO_PRINT
# include <kprinter.h>
#endif

#include "kexitableview.h"
#include "kexitablerm.h"
#include "kexi_utils.h"
#include "kexivalidator.h"

#include "kexidatetableedit.h"
#include "kexicelleditorfactory.h"
#include "kexitableedit.h"
#include "kexiinputtableedit.h"
#include "kexicomboboxtableedit.h"
#include "kexiblobtableedit.h"
#include "kexibooltableedit.h"

#include "kexitableview_p.h"

//sanity check
#define CHECK_DATA(r) \
	if (!m_data) { kdWarning() << "KexiTableView: No data assigned!" << endl; return r; }
#define CHECK_DATA_ \
	if (!m_data) { kdWarning() << "KexiTableView: No data assigned!" << endl; return; }

bool KexiTableView_cellEditorFactoriesInitialized = false;

// Initializes standard editor cell editor factories
void KexiTableView::initCellEditorFactories()
{
	if (KexiTableView_cellEditorFactoriesInitialized)
		return;
	KexiCellEditorFactoryItem* item;
	item = new KexiBlobEditorFactoryItem();
	KexiCellEditorFactory::registerItem( KexiDB::Field::BLOB, *item );

	item = new KexiDateEditorFactoryItem();
	KexiCellEditorFactory::registerItem( KexiDB::Field::Date, *item );

	item = new KexiComboBoxEditorFactoryItem();
	KexiCellEditorFactory::registerItem( KexiDB::Field::Enum, *item );

	item = new KexiBoolEditorFactoryItem();
	KexiCellEditorFactory::registerItem( KexiDB::Field::Boolean, *item );

	//default type
	item = new KexiInputEditorFactoryItem();
	KexiCellEditorFactory::registerItem( KexiDB::Field::InvalidType, *item );

	KexiTableView_cellEditorFactoriesInitialized = true;
}



KexiTableView::KexiTableView(KexiTableViewData* data, QWidget* parent, const char* name)
:QScrollView(parent, name, /*Qt::WRepaintNoErase | */Qt::WStaticContents /*| Qt::WResizeNoErase*/)
{
	KexiTableView::initCellEditorFactories();

	d = new KexiTableViewPrivate();

	m_data = new KexiTableViewData(); //to prevent crash because m_data==0
	m_owner = true;                   //-this will be deleted if needed

	setResizePolicy(Manual);
	viewport()->setBackgroundMode(NoBackground);
//	viewport()->setFocusPolicy(StrongFocus);
	viewport()->setFocusPolicy(WheelFocus);
	setFocusPolicy(WheelFocus); //<--- !!!!! important (was NoFocus), 
	//                             otherwise QApplication::setActiveWindow() won't activate 
	//                             this widget when needed!
//	setFocusProxy(viewport());
	viewport()->installEventFilter(this);

	//setup colors defaults
	setBackgroundMode(PaletteBackground);
	setEmptyAreaColor(palette().active().color(QColorGroup::Base));

	d->baseColor = colorGroup().base();
	d->textColor = colorGroup().text();

	d->altColor = KGlobalSettings::alternateBackgroundColor();
	d->grayColor = QColor(200,200,200);
	d->diagonalGrayPattern = QBrush(d->grayColor, BDiagPattern);

	setLineWidth(1);
	horizontalScrollBar()->installEventFilter(this);
	horizontalScrollBar()->raise();
	verticalScrollBar()->raise();
	
	// setup scrollbar tooltip
	d->scrollBarTip = new QLabel("abc",0, "scrolltip",WStyle_Customize |WStyle_NoBorder|WX11BypassWM|WStyle_StaysOnTop|WStyle_Tool);
	d->scrollBarTip->setPalette(QToolTip::palette());
	d->scrollBarTip->setMargin(2);
	d->scrollBarTip->setIndent(0);
	d->scrollBarTip->setAlignment(AlignCenter);
	d->scrollBarTip->setFrameStyle( QFrame::Plain | QFrame::Box );
	d->scrollBarTip->setLineWidth(1);
	connect(verticalScrollBar(),SIGNAL(sliderReleased()),this,SLOT(vScrollBarSliderReleased()));
	connect(&d->scrollBarTipTimer,SIGNAL(timeout()),this,SLOT(scrollBarTipTimeout()));
	
	//context menu
	d->pContextMenu = new KPopupMenu(this, "contextMenu");
#if 0 //moved to mainwindow's actions
	d->menu_id_addRecord = d->pContextMenu->insertItem(i18n("Add Record"), this, SLOT(addRecord()), CTRL+Key_Insert);
	d->menu_id_removeRecord = d->pContextMenu->insertItem(
		kapp->iconLoader()->loadIcon("button_cancel", KIcon::Small),
		i18n("Remove Record"), this, SLOT(removeRecord()), CTRL+Key_Delete);
#endif

#ifdef Q_WS_WIN
	d->rowHeight = fontMetrics().lineSpacing() + 4;
#else
	d->rowHeight = fontMetrics().lineSpacing() + 1;
#endif

	if(d->rowHeight < 17)
		d->rowHeight = 17;

	d->pUpdateTimer = new QTimer(this);

//	setMargins(14, fontMetrics().height() + 4, 0, 0);

	// Create headers
	d->pTopHeader = new QHeader(this, "topHeader");
	d->pTopHeader->setOrientation(Horizontal);
	d->pTopHeader->setTracking(false);
	d->pTopHeader->setMovingEnabled(false);
	connect(d->pTopHeader, SIGNAL(sizeChange(int,int,int)), this, SLOT(slotTopHeaderSizeChange(int,int,int)));

	d->pVerticalHeader = new KexiTableRM(this);
	d->pVerticalHeader->setCellHeight(d->rowHeight);
//	d->pVerticalHeader->setFixedWidth(d->rowHeight);
	d->pVerticalHeader->setCurrentRow(-1);

	setMargins(
		QMIN(d->pTopHeader->sizeHint().height(), d->rowHeight),
		d->pTopHeader->sizeHint().height(), 0, 0);

	setupNavigator();

//	setMinimumHeight(horizontalScrollBar()->height() + d->rowHeight + topMargin());

//	navPanelLyr->addStretch(25);
//	enableClipper(true);

	if (data)
		setData( data );

#if 0//(js) doesn't work!
	d->scrollTimer = new QTimer(this);
	connect(d->scrollTimer, SIGNAL(timeout()), this, SLOT(slotAutoScroll()));
#endif

	setBackgroundAltering(true);
	setFullRowSelectionEnabled(false);

	setAcceptDrops(true);
	viewport()->setAcceptDrops(true);

	// Connect header, table and scrollbars
	connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), d->pTopHeader, SLOT(setOffset(int)));
	connect(verticalScrollBar(), SIGNAL(valueChanged(int)),	d->pVerticalHeader, SLOT(setOffset(int)));
	connect(d->pTopHeader, SIGNAL(sizeChange(int, int, int)), this, SLOT(columnWidthChanged(int, int, int)));
	connect(d->pTopHeader, SIGNAL(clicked(int)), this, SLOT(sortColumnInternal(int)));

	connect(d->pUpdateTimer, SIGNAL(timeout()), this, SLOT(slotUpdate()));
	
//	horizontalScrollBar()->show();
//	updateScrollBars();
//	resize(sizeHint());
//	updateContents();
//	setMinimumHeight(horizontalScrollBar()->height() + d->rowHeight + topMargin());

//TMP
//setVerticalHeaderVisible(false);
//setHorizontalHeaderVisible(false);
}

KexiTableView::~KexiTableView()
{
	cancelRowEdit();

	if (m_owner)
		delete m_data;
	m_data = 0;
	delete d;
}

/*void KexiTableView::initActions(KActionCollection *ac)
{
	emit reloadActions(ac);
}*/

//! Setup navigator widget
void KexiTableView::setupNavigator()
{
	updateScrollBars();
	
	d->navPanel = new QFrame(this, "navPanel");
	d->navPanel->setFrameStyle(QFrame::Panel|QFrame::Raised);
	d->navPanel->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
	QHBoxLayout *navPanelLyr = new QHBoxLayout(d->navPanel,0,0,"nav_lyr");
	navPanelLyr->setAutoAdd(true);

	new QLabel(QString(" ")+i18n("Row:")+" ",d->navPanel);
	
	int bw = 6+SmallIcon("navigator_first").width(); //QMIN( horizontalScrollBar()->height(), 20);
	QFont f = d->navPanel->font();
	f.setPixelSize((bw > 12) ? 12 : bw);
	QFontMetrics fm(f);
	d->nav1DigitWidth = fm.width("8");

	d->navBtnFirst = new QToolButton(d->navPanel);
	d->navBtnFirst->setMaximumWidth(bw);
	d->navBtnFirst->setFocusPolicy(NoFocus);
	d->navBtnFirst->setIconSet( SmallIconSet("navigator_first") );
	QToolTip::add(d->navBtnFirst, i18n("First row"));
	
	d->navBtnPrev = new QToolButton(d->navPanel);
	d->navBtnPrev->setMaximumWidth(bw);
	d->navBtnPrev->setFocusPolicy(NoFocus);
	d->navBtnPrev->setIconSet( SmallIconSet("navigator_prev") );
	QToolTip::add(d->navBtnPrev, i18n("Previous row"));
	
	QWidget *spc = new QFrame(d->navPanel);
	spc->setFixedWidth(6);
	
	d->navRowNumber = new KLineEdit(d->navPanel);
	d->navRowNumber->setAlignment(AlignRight | AlignVCenter);
	d->navRowNumber->setFocusPolicy(ClickFocus);
//	d->navRowNumber->setFixedWidth(fw);
	d->navRowNumberValidator = new QIntValidator(1, 1, this);
	d->navRowNumber->setValidator(d->navRowNumberValidator);
	d->navRowNumber->installEventFilter(this);
	QToolTip::add(d->navRowNumber, i18n("Current row number"));
	
	KLineEdit *lbl_of = new KLineEdit(i18n("of"), d->navPanel);
	lbl_of->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
	lbl_of->setMaximumWidth(fm.width(lbl_of->text())+8);
	lbl_of->setReadOnly(true);
	lbl_of->setLineWidth(0);
	lbl_of->setFocusPolicy(NoFocus);
	lbl_of->setAlignment(AlignCenter);
	
	d->navRowCount = new KLineEdit(d->navPanel);
//	d->navRowCount->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
	d->navRowCount->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Preferred);
//	d->navRowCount->setMaximumWidth(fw);
	d->navRowCount->setReadOnly(true);
	d->navRowCount->setLineWidth(0);
	d->navRowCount->setFocusPolicy(NoFocus);
	d->navRowCount->setAlignment(AlignLeft | AlignVCenter);

	lbl_of->setFont(f);
	d->navRowNumber->setFont(f);
	d->navRowCount->setFont(f);
	d->navPanel->setFont(f);

	d->navBtnNext = new QToolButton(d->navPanel);
	d->navBtnNext->setMaximumWidth(bw);
	d->navBtnNext->setFocusPolicy(NoFocus);
	d->navBtnNext->setIconSet( SmallIconSet("navigator_next") );
	QToolTip::add(d->navBtnNext, i18n("Next row"));
	
	d->navBtnLast = new QToolButton(d->navPanel);
	d->navBtnLast->setMaximumWidth(bw);
	d->navBtnLast->setFocusPolicy(NoFocus);
	d->navBtnLast->setIconSet( SmallIconSet("navigator_last") );
	QToolTip::add(d->navBtnLast, i18n("Last row"));
	
	spc = new QWidget(d->navPanel);
	spc->setFixedWidth(6);
	d->navBtnNew = new QToolButton(d->navPanel);
	d->navBtnNew->setMaximumWidth(bw);
	d->navBtnNew->setFocusPolicy(NoFocus);
	d->navBtnNew->setIconSet( SmallIconSet("navigator_new") );
	QToolTip::add(d->navBtnNew, i18n("New row"));
	d->navBtnNext->setEnabled(isInsertingEnabled());
	
	spc = new QFrame(d->navPanel);
	spc->setFixedWidth(6);

	connect(d->navRowNumber,SIGNAL(returnPressed(const QString&)),
		this,SLOT(navRowNumber_ReturnPressed(const QString&)));
	connect(d->navRowNumber,SIGNAL(lostFocus()),
		this,SLOT(navRowNumber_lostFocus()));
	connect(d->navBtnPrev,SIGNAL(clicked()),this,SLOT(navBtnPrevClicked()));
	connect(d->navBtnNext,SIGNAL(clicked()),this,SLOT(navBtnNextClicked()));
	connect(d->navBtnLast,SIGNAL(clicked()),this,SLOT(navBtnLastClicked()));
	connect(d->navBtnFirst,SIGNAL(clicked()),this,SLOT(navBtnFirstClicked()));
	connect(d->navBtnNew,SIGNAL(clicked()),this,SLOT(navBtnNewClicked()));
	connect(verticalScrollBar(),SIGNAL(valueChanged(int)),
		this,SLOT(vScrollBarValueChanged(int)));

	d->navPanel->updateGeometry();
}


void KexiTableView::setNavRowNumber(int newrow)
{
	QString n;
	if (newrow>=0) {
		n = QString::number(newrow+1);
		if (d->navRowNumber->text().length() != n.length()) {//resize
			d->navRowNumber->setFixedWidth(
				d->nav1DigitWidth*QMAX( QMAX(n.length(),2)+1,d->navRowCount->text().length()+1)+6 
			);
		}
	}
	d->navRowNumber->setText(n);
	d->navRowCount->deselect();
	d->navBtnPrev->setEnabled(newrow>0);
	d->navBtnFirst->setEnabled(newrow>0);
	d->navBtnNext->setEnabled(newrow<(rows()-1+(isInsertingEnabled()?1:0)));
	d->navBtnLast->setEnabled(newrow!=(rows()-1));
}

void KexiTableView::setNavRowCount(int newrows)
{
	const QString & n = QString::number(newrows);
	if (d->navRowCount->text().length() != n.length()) {//resize
		d->navRowCount->setFixedWidth(d->nav1DigitWidth*n.length()+6);
	}
	d->navRowCount->setText(n);
	d->navRowCount->deselect();
}

void KexiTableView::setData( KexiTableViewData *data, bool owner )
{
	const bool theSameData = m_data && m_data==data;
	if (m_owner && m_data && m_data!=data/*don't destroy if it's the same*/) {
		kdDebug(44021) << "KexiTableView::setData(): destroying old data (owned)" << endl;
		delete m_data; //destroy old data
		m_data = 0;
	}
	m_owner = owner;
	if(!data) {
		m_data = new KexiTableViewData();
		m_owner = true;
	}
	else {
		m_data = data;
		m_owner = owner;
		kdDebug(44021) << "KexiTableView::setData(): using shared data" << endl;
		//add columns
//		d->pTopHeader->setUpdatesEnabled(false);
		while(d->pTopHeader->count()>0)
			d->pTopHeader->removeLabel(0);

		{
			for (KexiTableViewColumn::ListIterator it(m_data->columns);
				it.current(); ++it) 
			{
				if (it.current()->visible()) {
					int wid = it.current()->field->width();
					if (wid==0)
						wid=KEXITV_DEFAULT_COLUMN_WIDTH;//default col width in pixels
//js: TODO - add col width configuration and storage
					d->pTopHeader->addLabel(it.current()->field->captionOrName(), wid);
				}
			}
		}

//		d->pTopHeader->setUpdatesEnabled(true);
		//add rows
//		triggerUpdate();
		d->pVerticalHeader->addLabels(m_data->count());
	}
	
	if (!theSameData) {
		connect(m_data, SIGNAL(refreshRequested()), this, SLOT(slotRefreshRequested()));
		connect(m_data, SIGNAL(destroying()), this, SLOT(slotDataDestroying()));
	}

	if (!data) {
		clearData();
	}

	if (!d->pInsertItem) {//first setData() call - add 'insert' item
		d->pInsertItem = new KexiTableItem(columns());
	}
	else {//just reinit
		d->pInsertItem->init(columns());
	}

	//update gui mode
	d->navBtnNew->setEnabled(isInsertingEnabled());
	d->pVerticalHeader->showInsertRow(isInsertingEnabled());

	initDataContents();

	emit dataSet( m_data );

//	QSize s(tableSize());
//	resizeContents(s.width(),s.height());
}

void KexiTableView::initDataContents()
{
	//set current row:
	d->pCurrentItem = 0;
	int curRow = -1, curCol = -1;
//	d->curRow = -1;
//	d->curCol = -1;
	if (m_data->columnsCount()>0) {
		if (rows()>0) {
			d->pCurrentItem = m_data->first();
			curRow = 0;
			curCol = 0;
		}
		else {//no data
			if (isInsertingEnabled()) {
				d->pCurrentItem = d->pInsertItem;
				curRow = 0;
				curCol = 0;
			}
		}
	}
	
	QSize s(tableSize());
	resizeContents(s.width(),s.height());
	
	updateRowCountInfo();
	
	setCursor(curRow, curCol);
	ensureVisible(0,0);
	updateContents();

	emit dataRefreshed();
}

void KexiTableView::slotDataDestroying()
{
	m_data = 0;
}

/*void KexiTableView::addDropFilter(const QString &filter)
{
	d->dropFilters.append(filter);
	viewport()->setAcceptDrops(true);
}*/

void KexiTableView::setFont(const QFont &f)
{
	QWidget::setFont(f);
#ifdef Q_WS_WIN
	d->rowHeight = fontMetrics().lineSpacing() + 4;
#else
	d->rowHeight = fontMetrics().lineSpacing() + 1;
#endif
	if (d->fullRowSelectionEnabled) {
		d->rowHeight -= 1;
	}
	if(d->rowHeight < 17)
		d->rowHeight = 17;
//	if(d->rowHeight < 22)
//		d->rowHeight = 22;
	setMargins(
		QMIN(d->pTopHeader->sizeHint().height(), d->rowHeight),
		d->pTopHeader->sizeHint().height(), 0, 0);
//	setMargins(14, d->rowHeight, 0, 0);
	d->pVerticalHeader->setCellHeight(d->rowHeight);
	updateContents();
}

bool KexiTableView::beforeDeleteItem(KexiTableItem *)
{
	//always return
	return true;
}

bool KexiTableView::deleteItem(KexiTableItem *item)/*, bool moveCursor)*/
{
	if (!item || !beforeDeleteItem(item))
		return false;

	QString msg, desc;
	if (!m_data->deleteRow(*d->pCurrentItem)) {
		//error
		if (m_data->result()->desc.isEmpty())
			KMessageBox::sorry(this, m_data->result()->msg);
		else
			KMessageBox::detailedSorry(this, m_data->result()->msg, m_data->result()->desc);
		return false;
	}
	else {
		d->pCurrentItem = m_data->current();
	}

	int row = d->curRow;
	if (!isInsertingEnabled() && row>=rows())
		row--; //move up

	QSize s(tableSize());
	resizeContents(s.width(),s.height());

	setCursor(row, d->curCol, true/*forceSet*/);

	d->pVerticalHeader->removeLabel();
//		if(moveCursor)
//		selectPrev();
//		d->pUpdateTimer->start(1,true);
	//get last visible row
	int r = rowAt(clipper()->height());
	if (r==-1) {
		r = rows()+1+(isInsertingEnabled()?1:0);
	}
	//update all visible rows below 
	updateContents( contentsX(), rowPos(d->curRow), clipper()->width(), d->rowHeight*(r-d->curRow));

	//update navigator's data
	setNavRowCount(rows());

	return true;
}

void KexiTableView::deleteCurrentRow()
{
	if (d->newRowEditing) {//we're editing fresh new row: just cancel this!
		cancelRowEdit();
		return;
	}
	if (!isDeleteEnabled() || !d->pCurrentItem || d->pCurrentItem == d->pInsertItem)
		return;
	switch (d->deletionPolicy) {
	case NoDelete:
		return;
	case ImmediateDelete:
		break;
	case AskDelete:
		if (KMessageBox::questionYesNo(this, i18n("Do you want to delete selected row?"), 0, 
			KStdGuiItem::yes(), KStdGuiItem::no(), "dontAskBeforeDeleteRow"/*config entry*/)==KMessageBox::No)
			return;
		break;
	case SignalDelete:
		emit itemDeleteRequest(d->pCurrentItem, d->curRow, d->curCol);
		emit currentItemDeleteRequest();
		return;
	default:
		return;
	}

	if (!deleteItem(d->pCurrentItem)) {//nothing
	}
}

void KexiTableView::insertEmptyRow(int row)
{
	if ( !acceptRowEdit() || !isEmptyRowInsertingEnabled() 
		|| (row!=-1 && row >= (rows()+isInsertingEnabled()?1:0) ) )
		return;

	KexiTableItem *newItem = new KexiTableItem(columns());
	insertItem(newItem, row);
}

void KexiTableView::insertItem(KexiTableItem *newItem, int row)
{
	bool changeCurrent = (row==-1 || row==d->curRow);
	if (changeCurrent) {
		row = (d->curRow >= 0 ? d->curRow : 0);
		d->pCurrentItem = newItem;
		d->curRow = row;
	}
	m_data->insertRow(*newItem, row);

	QSize s(tableSize());
	resizeContents(s.width(),s.height());

	//redraw only this row and below:
	int leftcol = d->pTopHeader->sectionAt( d->pTopHeader->offset() );
//	updateContents( columnPos( leftcol ), rowPos(d->curRow), 
//		clipper()->width(), clipper()->height() - (rowPos(d->curRow) - contentsY()) );
	updateContents( columnPos( leftcol ), rowPos(row), 
		clipper()->width(), clipper()->height() - (rowPos(row) - contentsY()) );

	d->pVerticalHeader->addLabel();

	//update navigator's data
	setNavRowCount(rows());
}

void KexiTableView::clearData(bool repaint)
{
	CHECK_DATA_;
	//	cancelRowEdit();
//	acceptRowEdit();
//	d->pVerticalHeader->clear();
	m_data->clear();

//	d->clearVariables();
//	d->pVerticalHeader->setCurrentRow(-1);

//	d->pUpdateTimer->start(1,true);
	if (repaint)
		viewport()->repaint();
}

void KexiTableView::clearColumns(bool repaint)
{
	clearData(false);
	while(d->pTopHeader->count()>0)
		d->pTopHeader->removeLabel(0);

	if (repaint)
		viewport()->repaint();

/*	for(int i=0; i < rows(); i++)
	{
		d->pVerticalHeader->removeLabel();
	}

	editorCancel();
	m_contents->clear();

	d->clearVariables();
	d->numCols = 0;

	while(d->pTopHeader->count()>0)
		d->pTopHeader->removeLabel(0);

	d->pVerticalHeader->setCurrentRow(-1);

	viewport()->repaint();

//	d->pColumnTypes.resize(0);
//	d->pColumnModes.resize(0);
//	d->pColumnDefaults.clear();*/
}

void KexiTableView::slotRefreshRequested()
{
//	cancelRowEdit();
	acceptRowEdit();
	d->pVerticalHeader->clear();

	if (d->curCol>=0 && d->curCol<columns()) {
		//find the editor for this column
		KexiTableEdit *edit = editor( d->curCol );
		if (edit) {
			edit->hideFocus();
		}
	}
//	setCursor(-1, -1, true);
	d->clearVariables();
	d->pVerticalHeader->setCurrentRow(-1);
	d->initDataContentsOnShow = true;
}

#if 0 //todo
int KexiTableView::findString(const QString &string)
{
	int row = 0;
	int col = sorting();
	if(col == -1)
		return -1;
	if(string.isEmpty())
	{
		setCursor(0, col);
		return 0;
	}

	QPtrListIterator<KexiTableItem> it(*m_contents);

	if(string.at(0) != QChar('*'))
	{
		switch(columnType(col))
		{
			case QVariant::String:
			{
				QString str2 = string.lower();
				for(; it.current(); ++it)
				{
					if(it.current()->at(col).toString().left(string.length()).lower().compare(str2)==0)
					{
						center(columnPos(col), rowPos(row));
						setCursor(row, col);
						return row;
					}
					row++;
				}
				break;
			}
			case QVariant::Int:
			case QVariant::Bool:
				for(; it.current(); ++it)
				{
					if(QString::number(it.current()->at(col).toInt()).left(string.length()).compare(string)==0)
					{
						center(columnPos(col), rowPos(row));
						setCursor(row, col);
						return row;
					}
					row++;
				}
				break;

			default:
				break;
		}
	}
	else
	{
		QString str2 = string.mid(1);
		switch(columnType(col))
		{
			case QVariant::String:
				for(; it.current(); ++it)
				{
					if(it.current()->at(col).toString().find(str2,0,false) >= 0)
					{
						center(columnPos(col), rowPos(row));
						setCursor(row, col);
						return row;
					}
					row++;
				}
				break;
			case QVariant::Int:
			case QVariant::Bool:
				for(; it.current(); ++it)
				{
					if(QString::number(it.current()->at(col).toInt()).find(str2,0,true) >= 0)
					{
						center(columnPos(col), rowPos(row));
						setCursor(row, col);
						return row;
					}
					row++;
				}
				break;

			default:
				break;
		}
	}
	return -1;
}
#endif

void KexiTableView::slotUpdate()
{
//	kdDebug(44021) << " KexiTableView::slotUpdate() -- " << endl;
//	QSize s(tableSize());
//	viewport()->setUpdatesEnabled(false);
///	resizeContents(s.width(), s.height());
//	viewport()->setUpdatesEnabled(true);

	updateContents();

//	updateContents(0, contentsY()+clipper()->height()-2*d->rowHeight, clipper()->width(), d->rowHeight*3);
	
	//updateGeometries();
//	updateContents(0, 0, viewport()->width(), contentsHeight());
//	updateGeometries();
}

bool KexiTableView::isSortingEnabled() const
{
	return d->isSortingEnabled;
}

void KexiTableView::setSortingEnabled(bool set)
{
	if (d->isSortingEnabled && !set)
		setSorting(-1);
	d->isSortingEnabled = set;
}

int KexiTableView::sortedColumn()
{
	if (m_data && d->isSortingEnabled)
		return m_data->sortedColumn();
	return -1;
}

bool KexiTableView::sortingAscending() const
{ 
	return m_data && m_data->sortingAscending();
}

void KexiTableView::setSorting(int col, bool ascending/*=true*/)
{
	if (!m_data || !d->isSortingEnabled)
		return;
//	d->sortOrder = ascending;
//	d->sortedColumn = col;
	d->pTopHeader->setSortIndicator(col, ascending);//d->sortOrder);
	m_data->setSorting(col, ascending); //d->sortOrder);
//	sort();
//	columnSort(col);
}

void KexiTableView::sort()
{
	if (!m_data || !d->isSortingEnabled || rows() < 2)
		return;

	cancelRowEdit();
			
	if (m_data->sortedColumn()!=-1)
		m_data->sort();

	//locate current record
	if (!d->pCurrentItem) {
		d->pCurrentItem = m_data->first();
		d->curRow = 0;
		if (!d->pCurrentItem)
			return;
	}
	if (d->pCurrentItem != d->pInsertItem) {
		d->curRow = m_data->findRef(d->pCurrentItem);
	}

//	d->pCurrentItem = m_data->at(d->curRow);

	int cw = columnWidth(d->curCol);
	int rh = rowHeight();

//	d->pVerticalHeader->setCurrentRow(d->curRow);
	center(columnPos(d->curCol) + cw / 2, rowPos(d->curRow) + rh / 2);
//	updateCell(oldRow, d->curCol);
//	updateCell(d->curRow, d->curCol);
	d->pVerticalHeader->setCurrentRow(d->curRow);
//	slotUpdate();

	updateContents();
//	d->pUpdateTimer->start(1,true);
}

void KexiTableView::sortColumnInternal(int col)
{
//	bool i = false;
//js	QVariant hint;
	//-select sorting 
	bool asc;
	if (col==sortedColumn())
		asc = !sortingAscending(); //inverse sorting for this column
	else
		asc = true;
	
	setSorting( col, asc );
	
	//-perform sorting 
	sort();
	
#if 0//todo
	if(d->pInsertItem)
	{
		i = true;
//js		hint = d->pInsertItem->getHint();
//		delete d->pInsertItem;
		remove(d->pInsertItem);
		d->pInsertItem = 0;
//		d->pVerticalHeader->removeLabel(rows());
	}

	if(m_data->sortedColumn() == col) {
		m_data->setSorting(col, !m_data->sortingAscending());
	}
	else {
		m_data->setSorting(col);
	}
//js	else
//js		d->sortOrder = true;
//	d->sortedColumn = col;
	d->pTopHeader->setSortIndicator(col, m_data->sortingAscending());
	sort();

	if(i)
	{
		KexiTableItem *insert = new KexiTableItem(columns());
//js		insert->setHint(hint);
//js		insert->setInsertItem(true);
		d->pInsertItem = insert;

	}
//	updateContents( 0, 0, viewport()->width(), viewport()->height());
#endif
	emit sortedColumnChanged(col);
}

QSizePolicy KexiTableView::sizePolicy() const
{
	// this widget is expandable
	return QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

QSize KexiTableView::sizeHint() const
{
	const QSize &ts = tableSize();
	int w = QMAX( ts.width() + leftMargin()+ verticalScrollBar()->sizeHint().width() + 2*2, 
		(d->navPanel->isVisible() ? d->navPanel->width() : 0) );
	int h = QMAX( ts.height()+topMargin()+horizontalScrollBar()->sizeHint().height(), 
		minimumSizeHint().height() );
	w = QMIN( w, qApp->desktop()->width()*3/4 ); //stretch
	h = QMIN( h, qApp->desktop()->height()*3/4 ); //stretch

//	kdDebug() << "KexiTableView::sizeHint()= " <<w <<", " <<h << endl;

	return QSize(w, h);
		/*QSize(
		QMAX( ts.width() + leftMargin() + 2*2, (d->navPanel ? d->navPanel->width() : 0) ),
		//+ QMIN(d->pVerticalHeader->width(),d->rowHeight) + margin()*2,
		QMAX( ts.height()+topMargin()+horizontalScrollBar()->sizeHint().height(), 
			minimumSizeHint().height() )
	);*/
//		QMAX(ts.height() + topMargin(), minimumSizeHint().height()) );
}

QSize KexiTableView::minimumSizeHint() const
{
	return QSize(
		leftMargin() + ((columns()>0)?columnWidth(0):KEXITV_DEFAULT_COLUMN_WIDTH) + 2*2, 
		d->rowHeight*5/2 + topMargin() + (d->navPanel && d->navPanel->isVisible() ? d->navPanel->height() : 0)
	);
}

void KexiTableView::createBuffer(int width, int height)
{
	if(!d->pBufferPm)
		d->pBufferPm = new QPixmap(width, height);
	else
		if(d->pBufferPm->width() < width || d->pBufferPm->height() < height)
			d->pBufferPm->resize(width, height);
//	d->pBufferPm->fill();
}

//internal
inline void KexiTableView::paintRow(KexiTableItem *item,
	QPainter *pb, int r, int rowp, int cx, int cy, 
	int colfirst, int collast, int maxwc)
{
	if (!item)
		return;
	// Go through the columns in the row r
	// if we know from where to where, go through [colfirst, collast],
	// else go through all of them
	if (colfirst==-1)
		colfirst=0;
	if (collast==-1)
		collast=columns()-1;

	int transly = rowp-cy;

	if(d->bgAltering && (r%2 != 0))
		pb->fillRect(0, transly, maxwc, d->rowHeight, d->altColor);
//		pb->fillRect(0, transly, maxwc, d->rowHeight - 1, d->altColor);
	else
		pb->fillRect(0, transly, maxwc, d->rowHeight, d->baseColor);
//		pb->fillRect(0, transly, maxwc, d->rowHeight - 1, d->baseColor);

	for(int c = colfirst; c <= collast; c++)
	{
		// get position and width of column c
		int colp = columnPos(c);
		if (colp==-1)
			continue; //invisible column?
		int colw = columnWidth(c);
		int translx = colp-cx;

		// Translate painter and draw the cell
		pb->saveWorldMatrix();
		pb->translate(translx, transly);
			paintCell( pb, item, c, r, QRect(colp, rowp, colw, d->rowHeight));
		pb->restoreWorldMatrix();
	}

	if (d->dragIndicatorLine>=0 && d->dragIndicatorLine==r) {
		pb->setPen( QPen(d->textColor, 3) );
//		pb->setRasterOp
		pb->drawLine(0, transly+1, maxwc, transly+1);
	}
}

void KexiTableView::drawContents( QPainter *p, int cx, int cy, int cw, int ch)
{
	if (d->disableDrawContents)
		return;
	int colfirst = columnAt(cx);
	int rowfirst = rowAt(cy);
	int collast = columnAt(cx + cw-1);
	int rowlast = rowAt(cy + ch-1);
	bool inserting = isInsertingEnabled();
	bool plus1row = false; //true if we should show 'inserting' row at the end
	bool paintOnlyInsertRow = false;

/*	kdDebug(44021) << QString(" KexiTableView::drawContents(cx:%1 cy:%2 cw:%3 ch:%4)")
			.arg(cx).arg(cy).arg(cw).arg(ch) << endl;*/

	if (rowlast == -1) {
		rowlast = rows() - 1;
		plus1row = inserting;
		if (rowfirst == -1) {
			if (rowAt(cy - d->rowHeight) != -1) {
				paintOnlyInsertRow = true;
//				kdDebug(44021) << "-- paintOnlyInsertRow --" << endl;
			}
		}
	}
//	kdDebug(44021) << "rowfirst="<<rowfirst<<" rowlast="<<rowlast<<" rows()="<<rows()<<endl;
//	kdDebug(44021)<<" plus1row=" << plus1row<<endl;
	
	if ( collast == -1 )
		collast = columns() - 1;

// 	qDebug("cx:%3d cy:%3d w:%3d h:%3d col:%2d..%2d row:%2d..%2d tsize:%4d,%4d", 
//	cx, cy, cw, ch, colfirst, collast, rowfirst, rowlast, tableSize().width(), tableSize().height());
//	triggerUpdate();

	if (rowfirst == -1 || colfirst == -1) {
		if (!paintOnlyInsertRow) {
			paintEmptyArea(p, cx, cy, cw, ch);
			return;
		}
	}

	createBuffer(cw, ch);
	if(d->pBufferPm->isNull())
		return;
	QPainter *pb = new QPainter(d->pBufferPm, this);
//	pb->fillRect(0, 0, cw, ch, colorGroup().base());

//	int maxwc = QMIN(cw, (columnPos(d->numCols - 1) + columnWidth(d->numCols - 1)));
	int maxwc = columnPos(columns() - 1) + columnWidth(columns() - 1);
//	kdDebug(44021) << "KexiTableView::drawContents(): maxwc: " << maxwc << endl;

	pb->fillRect(cx, cy, cw, ch, colorGroup().base());

	int rowp;
	int r;
	if (paintOnlyInsertRow) {
		r = rows();
		rowp = rowPos(r); // 'insert' row's position
	}
	else {
		QPtrListIterator<KexiTableItem> it(*m_data);
		it += rowfirst;//move to 1st row
		rowp = rowPos(rowfirst); // row position 
		for (r = rowfirst;r <= rowlast; r++, ++it, rowp+=d->rowHeight) {
			paintRow(it.current(), pb, r, rowp, cx, cy, colfirst, collast, maxwc);
		}
	}

	if (plus1row) { //additional - 'insert' row
		paintRow(d->pInsertItem, pb, r, rowp, cx, cy, colfirst, collast, maxwc);
	}

	delete pb;

	p->drawPixmap(cx,cy,*d->pBufferPm, 0,0,cw,ch);

  //(js)
	paintEmptyArea(p, cx, cy, cw, ch);
}

void KexiTableView::paintCell(QPainter* p, KexiTableItem *item, int col, int row, const QRect &cr, bool print)
{
//	kdDebug() <<"KexiTableView::paintCell(col=" << col <<"row="<<row<<")"<<endl;
	Q_UNUSED(print);
	int w = cr.width();
	int h = cr.height();
	int x2 = w - 1;
	int y2 = h - 1;

	//	Draw our lines
	QPen pen(p->pen());

	if (!d->fullRowSelectionEnabled) {
		p->setPen(d->grayColor);
		p->drawLine( x2, 0, x2, y2 );	// right
		p->drawLine( 0, y2, x2, y2 );	// bottom
	}
	p->setPen(pen);

	if (d->pEditor && row == d->curRow && col == d->curCol //don't paint contents of edited cell
		&& d->pEditor->hasFocusableWidget() //..if it's visible
	   )
		return;

	KexiTableEdit *edit = editor( col, /*ignoreMissingEditor=*/true );
//	if (!edit)
//		return;

/*
#ifdef Q_WS_WIN
	int x = 1;
	int y_offset = -1;
#else
	int x = 1;
	int y_offset = 0;
#endif

//	const int ctype = columnType(col);*/
//	int x=1;
	int x = edit ? edit->leftMargin() : 0;
	int y_offset=0;

	int align = SingleLine | AlignVCenter;
	QString txt; //text to draw

	QVariant cell_value;
	if ((uint)col < item->count()) {
		if (d->pCurrentItem == item) {
			if (d->pEditor && row == d->curRow && col == d->curCol && !d->pEditor->hasFocusableWidget()) {
				//we're over editing cell and the editor has no widget
				// - we're displaying internal values, not buffered
				bool ok;
				cell_value = d->pEditor->value(ok);
			}
			else {
				//we're displaying values from edit buffer, if available
				cell_value = *bufferedValueAt(col);
			}
		}
		else {
			cell_value = item->at(col);
		}
	}

	if (edit)
		edit->setupContents( p, d->pCurrentItem == item && col == d->curCol, 
			cell_value, txt, align, x, y_offset, w, h );

/*
	if (KexiDB::Field::isFPNumericType( ctype )) {
#ifdef Q_WS_WIN
#else
			x = 0;
#endif
//js TODO: ADD OPTION to desplaying NULL VALUES as e.g. "(null)"
		if (!cell_value.isNull())
			txt = KGlobal::locale()->formatNumber(cell_value.toDouble());
		w -= 6;
		align |= AlignRight;
	}
	else if (ctype == KexiDB::Field::Enum)
	{
		txt = m_data->column(col)->field->enumHints().at(cell_value.toInt());
		align |= AlignLeft;
	}
	else if (KexiDB::Field::isIntegerType( ctype )) {
		int num = cell_value.toInt();
#ifdef Q_WS_WIN
		x = 1;
#else
		x = 0;
#endif
		w -= 6;
		align |= AlignRight;
		if (!cell_value.isNull())
			txt = QString::number(num);
	}
	else if (ctype == KexiDB::Field::Boolean) {
		int s = QMAX(h - 5, 12);
		QRect r(w/2 - s/2 + x, h/2 - s/2 - 1, s, s);
		p->setPen(QPen(colorGroup().text(), 1));
		p->drawRect(r);
		if (cell_value.asBool())
		{
			p->drawLine(r.x() + 2, r.y() + 2, r.right() - 1, r.bottom() - 1);
			p->drawLine(r.x() + 2, r.bottom() - 2, r.right() - 1, r.y() + 1);
		}
	}
	else if (ctype == KexiDB::Field::Date) { //todo: datetime & time
#ifdef Q_WS_WIN
		x = 5;
#else
		x = 5;
#endif
		if(cell_value.toDate().isValid())
		{
#ifdef USE_KDE
			txt = KGlobal::locale()->formatDate(cell_value.toDate(), true);
#else
			if (!cell_value.isNull())
				txt = cell_value.toDate().toString(Qt::LocalDate);
#endif
		}
		align |= AlignLeft;
	}
	else {//default:
#ifdef Q_WS_WIN
		x = 5;
//		y_offset = -1;
#else
		x = 5;
//		y_offset = 0;
#endif
		if (!cell_value.isNull())
			txt = cell_value.toString();
		align |= AlignLeft;
	}*/
	
	// draw selection background
//	const bool has_focus = hasFocus() || viewport()->hasFocus() || d->pContextMenu->hasFocus();

	const bool columnReadOnly = m_data->column(col)->readOnly();

	if (d->pCurrentItem == item && col == d->curCol) {
/*		edit->paintSelectionBackground( p, isEnabled(), txt, align, x, y_offset, w, h,
			has_focus ? colorGroup().highlight() : gray,
			columnReadOnly, d->fullRowSelectionEnabled );*/
		if (edit)
			edit->paintSelectionBackground( p, isEnabled(), txt, align, x, y_offset, w, h,
				isEnabled() ? colorGroup().highlight() : d->grayColor,
				columnReadOnly, d->fullRowSelectionEnabled );
	}

/*
	if (!txt.isEmpty() && d->pCurrentItem == item 
		&& col == d->curCol && !columnReadOnly) //js: && !d->recordIndicator)
	{
		QRect bound=fontMetrics().boundingRect(x, y_offset, w - (x+x), h, align, txt);
		bound.setX(bound.x()-1);
		bound.setY(0);
		bound.setWidth( QMIN( bound.width()+2, w - (x+x)+1 ) );
		bound.setHeight(d->rowHeight-1);
		if (has_focus)
			p->fillRect(bound, colorGroup().highlight());
		else
			p->fillRect(bound, gray);
	}
*/	
	if (!edit){
		p->fillRect(0, 0, x2, y2, d->diagonalGrayPattern);
	}

//	If we are in the focus cell, draw indication
	if(d->pCurrentItem == item && col == d->curCol //js: && !d->recordIndicator)
		&& !d->fullRowSelectionEnabled) 
	{
//		kdDebug() << ">>> CURRENT CELL ("<<d->curCol<<"," << d->curRow<<") focus="<<has_focus<<endl;
//		if (has_focus) {
		if (isEnabled()) {
			p->setPen(colorGroup().text());
		}
		else {
			QPen gray_pen(p->pen());
			gray_pen.setColor(d->grayColor);
			p->setPen(gray_pen);
		}
		if (edit)
			edit->paintFocusBorders( p, cell_value, 0, 0, x2, y2 );
		else
			p->drawRect(0, 0, x2, y2);
	}
	
	// draw text
	if (!txt.isEmpty()) {
		if (d->pCurrentItem == item && col == d->curCol && !columnReadOnly)
			p->setPen(colorGroup().highlightedText());
		else
			p->setPen(colorGroup().text());
		p->drawText(x, y_offset, w - (x + x)- ((align & AlignLeft)?2:0)/*right space*/, h, align, txt);
	}
}

QPoint KexiTableView::contentsToViewport2( const QPoint &p )
{
	return QPoint( p.x() - contentsX(), p.y() - contentsY() );
}

void KexiTableView::contentsToViewport2( int x, int y, int& vx, int& vy )
{
	const QPoint v = contentsToViewport2( QPoint( x, y ) );
	vx = v.x();
	vy = v.y();
}

QPoint KexiTableView::viewportToContents2( const QPoint& vp )
{
	return QPoint( vp.x() + contentsX(),
		   vp.y() + contentsY() );
}

void KexiTableView::paintEmptyArea( QPainter *p, int cx, int cy, int cw, int ch )
{
//  qDebug("%s: paintEmptyArea(x:%d y:%d w:%d h:%d)", (const char*)parentWidget()->caption(),cx,cy,cw,ch);

	// Regions work with shorts, so avoid an overflow and adjust the
	// table size to the visible size
	QSize ts( tableSize() );
//	ts.setWidth( QMIN( ts.width(), visibleWidth() ) );
//	ts.setHeight( QMIN( ts.height() - (d->navPanel ? d->navPanel->height() : 0), visibleHeight()) );
/*	kdDebug(44021) << QString(" (cx:%1 cy:%2 cw:%3 ch:%4)")
			.arg(cx).arg(cy).arg(cw).arg(ch) << endl;
	kdDebug(44021) << QString(" (w:%3 h:%4)")
			.arg(ts.width()).arg(ts.height()) << endl;*/
	
	// Region of the rect we should draw, calculated in viewport
	// coordinates, as a region can't handle bigger coordinates
	contentsToViewport2( cx, cy, cx, cy );
	QRegion reg( QRect( cx, cy, cw, ch ) );

	// Subtract the table from it
	reg = reg.subtract( QRect( QPoint( 0, 0 ), ts-QSize(0,d->navPanel->isVisible() ? d->navPanel->height() : 0) ) );

	// And draw the rectangles (transformed inc contents coordinates as needed)
	QMemArray<QRect> r = reg.rects();
	for ( int i = 0; i < (int)r.count(); ++i ) {
		QRect rect( viewportToContents2(r[i].topLeft()), r[i].size() );
/*		kdDebug(44021) << QString("- pEA: p->fillRect(x:%1 y:%2 w:%3 h:%4)")
			.arg(rect.x()).arg(rect.y())
			.arg(rect.width()).arg(rect.height()) << endl;*/
//		p->fillRect( QRect(viewportToContents2(r[i].topLeft()),r[i].size()), d->emptyAreaColor );
		p->fillRect( rect, d->emptyAreaColor );
//		p->fillRect( QRect(viewportToContents2(r[i].topLeft()),r[i].size()), viewport()->backgroundBrush() );
	}
}

void KexiTableView::contentsMouseDoubleClickEvent(QMouseEvent *e)
{
//	kdDebug(44021) << "KexiTableView::contentsMouseDoubleClickEvent()" << endl;
	d->contentsMousePressEvent_dblClick = true;
	contentsMousePressEvent(e);
	d->contentsMousePressEvent_dblClick = false;

	if(d->pCurrentItem)
	{
		if(d->editOnDoubleClick && columnEditable(d->curCol) 
			&& columnType(d->curCol) != KexiDB::Field::Boolean)
		{
			startEditCurrentCell();
//			createEditor(d->curRow, d->curCol, QString::null);
		}

		emit itemDblClicked(d->pCurrentItem, d->curRow, d->curCol);
	}
}

void KexiTableView::contentsMousePressEvent( QMouseEvent* e )
{
//	kdDebug(44021) << "KexiTableView::contentsMousePressEvent() ??" << endl;
	setFocus();
	if(m_data->count()==0 && !isInsertingEnabled()) {
		QScrollView::contentsMousePressEvent( e );
		return;
	}

	if (columnAt(e->pos().x())==-1) { //outside a colums
		QScrollView::contentsMousePressEvent( e );
		return;
	}
//	d->contentsMousePressEvent_ev = *e;
//	d->contentsMousePressEvent_enabled = true;
//	QTimer::singleShot(2000, this, SLOT( contentsMousePressEvent_Internal() ));
//	d->contentsMousePressEvent_timer.start(100,true);
	
//	if (!d->contentsMousePressEvent_enabled)
//		return;
//	d->contentsMousePressEvent_enabled=false;
	
	// remember old focus cell
	int oldRow = d->curRow;
	int oldCol = d->curCol;
	kdDebug(44021) << "oldRow=" << oldRow <<" oldCol=" << oldCol <<endl;
	bool onInsertItem = false;

	int newrow, newcol;
	//compute clicked row nr
	if (isInsertingEnabled()) {
		if (rowAt(e->pos().y())==-1) {
			newrow = rowAt(e->pos().y() - d->rowHeight);
			if (newrow==-1 && m_data->count()>0) {
				QScrollView::contentsMousePressEvent( e );
				return;
			}
			newrow++;
			kdDebug(44021) << "Clicked just on 'insert' row." << endl;
			onInsertItem=true;
		}
		else {
			// get new focus cell
			newrow = rowAt(e->pos().y());
		}
	}
	else {
		if (rowAt(e->pos().y())==-1 || columnAt(e->pos().x())==-1) {
			QScrollView::contentsMousePressEvent( e );
			return; //clicked outside a grid
		}
		// get new focus cell
		newrow = rowAt(e->pos().y());
	}
	newcol = columnAt(e->pos().x());

	if(e->button() != NoButton) {
		setCursor(newrow,newcol);
	}

//	kdDebug(44021)<<"void KexiTableView::contentsMousePressEvent( QMouseEvent* e ) by now the current items should be set, if not -> error + crash"<<endl;
	if(e->button() == RightButton)
	{
		showContextMenu(e->globalPos());
	}
	else if(e->button() == LeftButton)
	{
		if(columnType(d->curCol) == KexiDB::Field::Boolean && columnEditable(d->curCol))
		{
			boolToggled();
		}
#if 0 //js: TODO
		else if(columnType(d->curCol) == QVariant::StringList && columnEditable(d->curCol))
		{
			createEditor(d->curRow, d->curCol);
		}
#endif
	}
//ScrollView::contentsMousePressEvent( e );
}

void KexiTableView::contentsMouseReleaseEvent( QMouseEvent* e )
{
//	kdDebug(44021) << "KexiTableView::contentsMousePressEvent() ??" << endl;
	if(m_data->count()==0 && !isInsertingEnabled())
		return;

	int col = columnAt(e->pos().x());
	int row = rowAt(e->pos().y());
	if (!d->pCurrentItem || col==-1 || row==-1 || col!=d->curCol || row!=d->curRow)//outside a current cell
		return;

	QScrollView::contentsMouseReleaseEvent( e );

	emit itemMouseReleased(d->pCurrentItem, d->curRow, d->curCol);
}

KPopupMenu* KexiTableView::popup() const
{
	return d->pContextMenu;
}

void KexiTableView::showContextMenu(QPoint pos)
{
	if (!d->contextMenuEnabled)
		return;
	if (pos==QPoint(-1,-1)) {
		pos = viewport()->mapToGlobal( QPoint( columnPos(d->curCol), rowPos(d->curRow) + d->rowHeight ) );
	}
	//show own context menu if configured
//	if (updateContextMenu()) {
		selectRow(d->curRow);
		d->pContextMenu->exec(pos);
/*	}
	else {
		//request other context menu
		emit contextMenuRequested(d->pCurrentItem, d->curCol, pos);
	}*/
}

void KexiTableView::contentsMouseMoveEvent( QMouseEvent *e )
{
#if 0//(js) doesn't work!

	// do the same as in mouse press
	int x,y;
	contentsToViewport(e->x(), e->y(), x, y);

	if(y > visibleHeight())
	{
		d->needAutoScroll = true;
		d->scrollTimer->start(70, false);
		d->scrollDirection = ScrollDown;
	}
	else if(y < 0)
	{
		d->needAutoScroll = true;
		d->scrollTimer->start(70, false);
		d->scrollDirection = ScrollUp;
	}
	else if(x > visibleWidth())
	{
		d->needAutoScroll = true;
		d->scrollTimer->start(70, false);
		d->scrollDirection = ScrollRight;
	}
	else if(x < 0)
	{
		d->needAutoScroll = true;
		d->scrollTimer->start(70, false);
		d->scrollDirection = ScrollLeft;
	}
	else
	{
		d->needAutoScroll = false;
		d->scrollTimer->stop();
		contentsMousePressEvent(e);
	}
#endif
	QScrollView::contentsMouseMoveEvent(e);
}

void KexiTableView::startEditCurrentCell()
{
//	if (columnType(d->curCol) == KexiDB::Field::Boolean)
//		return;
	if (isReadOnly() || !columnEditable(d->curCol))
		return;
	if (d->pEditor)
		return;
	ensureVisible(columnPos(d->curCol), rowPos(d->curRow)+rowHeight(), columnWidth(d->curCol), rowHeight());
	createEditor(d->curRow, d->curCol, QString::null, false);
}

void KexiTableView::deleteAndStartEditCurrentCell()
{
	if (isReadOnly() || !columnEditable(d->curCol))
		return;
	if (d->pEditor) {//if we've editor - just clear it
		d->pEditor->clear();
		return;
	}
	if (columnType(d->curCol) == KexiDB::Field::Boolean)
		return;
	ensureVisible(columnPos(d->curCol), rowPos(d->curRow)+rowHeight(), columnWidth(d->curCol), rowHeight());
	createEditor(d->curRow, d->curCol, QString::null, false/*removeOld*/);
	if (!d->pEditor)
		return;
	d->pEditor->clear();
}

#if 0//(js) doesn't work!
void KexiTableView::contentsMouseReleaseEvent(QMouseEvent *)
{
	if(d->needAutoScroll)
	{
		d->scrollTimer->stop();
	}
}
#endif

void KexiTableView::plugSharedAction(KAction* a)
{
	if (!a)
		return;
	d->sharedActions.insert(a->name(), a);
}

bool KexiTableView::shortCutPressed( QKeyEvent *e, const QCString &action_name )
{
	KAction *action = d->sharedActions[action_name];
	if (action) {
		if (!action->isEnabled())//this action is disabled - don't process it!
			return false; 
		if (action->shortcut() == KShortcut( KKey(e) ))
			return false;//this shortcut is owned by shared action - don't process it!
	}

	//check default shortcut
	if (action_name=="data_save_row")
		return (e->key() == Key_Return || e->key() == Key_Enter) && e->state()==ShiftButton;
	if (action_name=="edit_delete_row")
		return e->key() == Key_Delete && e->state()==ShiftButton;
	if (action_name=="edit_delete")
		return e->key() == Key_Delete && e->state()==NoButton;
	if (action_name=="data_insert_empty_row")
		return e->key() == Key_Insert && e->state()==(ShiftButton | ControlButton);

	return false;
}

void KexiTableView::keyPressEvent(QKeyEvent* e)
{
	CHECK_DATA_;
//	kdDebug() << "KexiTableView::keyPressEvent: key=" <<e->key() << " txt=" <<e->text()<<endl;

	const bool ro = isReadOnly();
	QWidget *w = focusWidget();
//	if (!w || w!=viewport() && w!=this && (!d->pEditor || w!=d->pEditor->view() && w!=d->pEditor)) {
//	if (!w || w!=viewport() && w!=this && (!d->pEditor || w!=d->pEditor->view())) {
	if (!w || w!=viewport() && w!=this && (!d->pEditor || !Kexi::hasParent(d->pEditor, w))) {
		//don't process stranger's events
		e->ignore();
		return;
	}
	if (d->skipKeyPress) {
		d->skipKeyPress=false;
		e->ignore();
		return;
	}
	
	if(d->pCurrentItem == 0 && (m_data->count() > 0 || isInsertingEnabled()))
	{
		setCursor(0,0);
	}
	else if(m_data->count() == 0 && !isInsertingEnabled())
	{
		e->accept();
		return;
	}

	if(d->pEditor) {// if a cell is edited, do some special stuff
		if (e->key() == Key_Escape) {
			cancelEditor();
			e->accept();
			return;
		} else if (e->key() == Key_Return || e->key() == Key_Enter) {
			if (columnType(d->curCol) == KexiDB::Field::Boolean) {
				boolToggled();
			}
			else {
				acceptEditor();
			}
			e->accept();
			return;
		}
	}
	else if (d->rowEditing) {// if a row is in edit mode, do some special stuff
		if (shortCutPressed( e, "data_save_row")) {
			kdDebug() << "shortCutPressed!!!" <<endl;
			acceptRowEdit();
			return;
		}
	}

	if(e->key() == Key_Return || e->key() == Key_Enter)
	{
		emit itemReturnPressed(d->pCurrentItem, d->curRow, d->curCol);
	}

	int curRow = d->curRow;
	int curCol = d->curCol;

	const bool nobtn = e->state()==NoButton;
	bool printable = false;

	//check shared shortcuts
	if (!ro) {
		if (shortCutPressed(e, "edit_delete_row")) {
			deleteCurrentRow();
			e->accept();
			return;
		} else if (shortCutPressed(e, "edit_delete")) {
			deleteAndStartEditCurrentCell();
			e->accept();
			return;
		}
		else if (shortCutPressed(e, "data_insert_empty_row")) {
			insertEmptyRow();
			e->accept();
			return;
		}
	}

	switch (e->key())
	{
/*	case Key_Delete:
		if (e->state()==Qt::ControlButton) {//remove current row
			deleteCurrentRow();
		}
		else if (nobtn) {//remove contents of the current cell
			deleteAndStartEditCurrentCell();
		}
		break;*/

	case Key_Shift:
	case Key_Alt:
	case Key_Control:
	case Key_Meta:
		e->ignore();
		break;

	case Key_Left:
		if (nobtn)
			curCol = QMAX(0, curCol - 1);
		break;
	case Key_Right:
		if (nobtn)
			curCol = QMIN(columns() - 1, curCol + 1);
		break;
	case Key_Tab:
	case Key_Backtab:
		if (nobtn && e->key()==Key_Tab) {
			if (acceptEditor()) {
				curCol = QMIN(columns() - 1, curCol + 1);
			}
		}
		else if ((e->state()==ShiftButton && e->key()==Key_Tab)
		 || (e->state()==NoButton && e->key()==Key_Backtab)
		 || (e->state()==ShiftButton && e->key()==Key_Backtab)) {
			if (acceptEditor()) {
				curCol = QMAX(0, curCol - 1);
			}
		}
		break;
	case Key_Up:
		if (nobtn) {
//			curRow = QMAX(0, curRow - 1);
			selectPrevRow();
			e->accept();
			return;
		}
		break;
	case Key_Down:
		if (nobtn) {
//			curRow = QMIN(rows() - 1 + (isInsertingEnabled()?1:0), curRow + 1);
			selectNextRow();
			e->accept();
			return;
		}
		break;
	case Key_PageUp:
		if (nobtn) {
//			curRow -= visibleHeight() / d->rowHeight;
//			curRow = QMAX(0, curRow);
			selectPrevPage();
			e->accept();
			return;
		}
		break;
	case Key_PageDown:
		if (nobtn) {
//			curRow += visibleHeight() / d->rowHeight;
//			curRow = QMIN(rows() - 1 + (isInsertingEnabled()?1:0), curRow);
			selectNextPage();
			e->accept();
			return;
		}
		break;
	case Key_Home:
		if (d->fullRowSelectionEnabled) {
			//we're in row-selection mode: home key always moves to 1st row
			curRow = 0;//to 1st row
		}
		else {//cell selection mode: different actions depending on ctrl and shift keys state
			if (nobtn) {
				curCol = 0;//to 1st col
			}
			else if (e->state()==ControlButton) {
				curRow = 0;//to 1st row
			}
			else if (e->state()==(ControlButton|ShiftButton)) {
				curRow = 0;//to 1st row and col
				curCol = 0;
			}
		}
		break;
	case Key_End:
		if (d->fullRowSelectionEnabled) {
			//we're in row-selection mode: home key always moves to last row
			curRow = m_data->count()-1+(isInsertingEnabled()?1:0);//to last row
		}
		else {//cell selection mode: different actions depending on ctrl and shift keys state
			if (nobtn) {
				curCol = columns()-1;//to last col
			}
			else if (e->state()==ControlButton) {
				curRow = m_data->count()-1+(isInsertingEnabled()?1:0);//to last row
			}
			else if (e->state()==(ControlButton|ShiftButton)) {
				curRow = m_data->count()-1+(isInsertingEnabled()?1:0);//to last row and col
				curCol = columns()-1;//to last col
			}
		}
		break;
	case Key_Enter:
	case Key_Return:
	case Key_F2:
		if (nobtn && !ro && columnEditable(curCol)) {
			if (columnType(d->curCol) == KexiDB::Field::Boolean) {
				if (e->key()==Key_Enter || e->key()==Key_Return) {
					boolToggled();
					return;
				}
			}
			else {
				startEditCurrentCell();
				return;
			}
		}
		break;
	case Key_Backspace:
		if (nobtn && !ro && columnType(curCol) != KexiDB::Field::Boolean && columnEditable(curCol))
			createEditor(curRow, curCol, QString::null, true);
		break;
	case Key_Space:
		if (nobtn && !ro && columnEditable(curCol)) {
			if (columnType(curCol) == KexiDB::Field::Boolean) {
				boolToggled();
				break;
			}
			else
				printable = true; //just space key
		}
	case Key_Escape:
		if (nobtn && d->rowEditing) {
			cancelRowEdit();
			return;
		}
		break;
	default:
		//others:
		if (nobtn && e->key()==KGlobalSettings::contextMenuKey()) { //Key_Menu:
			showContextMenu();
		}
		else {
			qDebug("KexiTableView::KeyPressEvent(): default");
			if (e->text().isEmpty() || !e->text().isEmpty() && !e->text()[0].isPrint() ) {
				kdDebug(44021) << "NOT PRINTABLE: 0x0" << QString("%1").arg(e->key(),0,16) <<endl;
				e->ignore();
				return;
			}

			printable = true;
		}
	}
	//finally: we've printable char:
	if (printable && !ro) {
		KexiTableViewColumn *colinfo = m_data->column(curCol);
		if (colinfo->acceptsFirstChar(e->text()[0])) {
			kdDebug(44021) << "KexiTableView::KeyPressEvent(): ev pressed: acceptsFirstChar()==true";
	//			if (e->text()[0].isPrint())
			createEditor(curRow, curCol, e->text(), true);
		}
		else 
			kdDebug(44021) << "KexiTableView::KeyPressEvent(): ev pressed: acceptsFirstChar()==false";
	}

	d->vScrollBarValueChanged_enabled=false;

	// if focus cell changes, repaint
	setCursor(curRow, curCol);

	d->vScrollBarValueChanged_enabled=true;

	e->accept();
}

void KexiTableView::emitSelected()
{
	if(d->pCurrentItem)
		emit itemSelected(d->pCurrentItem);
}

void KexiTableView::boolToggled()
{
	startEditCurrentCell();
	if (d->pEditor) {
		d->pEditor->clickedOnContents();
	}
	acceptEditor();
	updateCell(d->curRow, d->curCol);

/*	int s = d->pCurrentItem->at(d->curCol).toInt();
	QVariant oldValue=d->pCurrentItem->at(d->curCol);
	(*d->pCurrentItem)[d->curCol] = QVariant(s ? 0 : 1);
	updateCell(d->curRow, d->curCol);
//	emit itemChanged(d->pCurrentItem, d->curRow, d->curCol, oldValue);
//	emit itemChanged(d->pCurrentItem, d->curRow, d->curCol);*/
}

void KexiTableView::clearSelection()
{
//	selectRow( -1 );
	int oldRow = d->curRow;
	int oldCol = d->curCol;
	d->curRow = -1;
	d->curCol = -1;
	d->pCurrentItem = 0;
	updateRow( oldRow );
	setNavRowNumber(-1);
}

void KexiTableView::selectNextRow()
{
	selectRow( QMIN( rows() - 1 +(isInsertingEnabled()?1:0), d->curRow + 1 ) );
}

void KexiTableView::selectPrevPage()
{
	selectRow( 
		QMAX( 0, d->curRow - visibleHeight() / d->rowHeight )
	);
}

void KexiTableView::selectNextPage()
{
	selectRow( 
		QMIN( 
			rows() - 1 + (isInsertingEnabled()?1:0),
			d->curRow + visibleHeight() / d->rowHeight
		)
	);
}

void KexiTableView::selectFirstRow()
{
	selectRow(0);
}

void KexiTableView::selectLastRow()
{
	selectRow(rows() - 1 + (isInsertingEnabled()?1:0));
}

void KexiTableView::selectRow(int row)
{
	setCursor(row, -1);
}

void KexiTableView::selectPrevRow()
{
	selectRow( QMAX( 0, d->curRow - 1 ) );
}

KexiTableEdit *KexiTableView::editor( int col, bool ignoreMissingEditor )
{
	if (!m_data || col<0 || col>=columns())
		return 0;
	KexiTableViewColumn *tvcol = m_data->column(col);
//	int t = tvcol->field->type();

	//find the editor for this column
	KexiTableEdit *editor = d->editors[ tvcol ];
	if (editor)
		return editor;

	//not found: create
//	editor = KexiCellEditorFactory::createEditor(*m_data->column(col)->field, this);
	editor = KexiCellEditorFactory::createEditor(*m_data->column(col), this);
	if (!editor) {//create error!
		if (!ignoreMissingEditor) {
			//js TODO: show error???
			cancelRowEdit();
		}
		return 0;
	}
	editor->hide();
	connect(editor,SIGNAL(editRequested()),this,SLOT(slotEditRequested()));
	connect(editor,SIGNAL(cancelRequested()),this,SLOT(cancelEditor()));
	connect(editor,SIGNAL(acceptRequested()),this,SLOT(acceptEditor()));

	editor->resize(columnWidth(col)-1, rowHeight()-1);
	editor->installEventFilter(this);
	if (editor->view())
		editor->view()->installEventFilter(this);
	//store
	d->editors.insert( tvcol, editor );
	return editor;
}

void KexiTableView::editorShowFocus( int row, int col )
{
	KexiTableEdit *edit = editor( col );
	/*nt p = rowPos(row);
	 (!edit || (p < contentsY()) || (p > (contentsY()+clipper()->height()))) {
		kdDebug()<< "KexiTableView::editorShowFocus() : OUT" << endl;
		return;
	}*/
	if (edit) {
		kdDebug()<< "KexiTableView::editorShowFocus() : IN" << endl;
		QRect rect = cellGeometry( d->curRow, d->curCol );
//		rect.moveBy( -contentsX(), -contentsY() );
		edit->showFocus( rect );
	}
}

void KexiTableView::slotEditRequested()
{
//	KexiTableEdit *edit = editor( d->curCol );
//	if (edit) {

	createEditor(d->curRow, d->curCol);
}

void KexiTableView::createEditor(int row, int col, const QString& addText, bool removeOld)
{
	kdDebug(44021) << "KexiTableView::createEditor('"<<addText<<"',"<<removeOld<<")"<<endl;
	if (isReadOnly()) {
		kdDebug(44021) << "KexiTableView::createEditor(): DATA IS READ ONLY!"<<endl;
		return;
	}

	if (m_data->column(col)->readOnly()) {//d->pColumnModes.at(d->numCols-1) & ColumnReadOnly)
		kdDebug(44021) << "KexiTableView::createEditor(): COL IS READ ONLY!"<<endl;
		return;
	}
	
/*	QVariant val;
	if (!removeOld) {
		val = *bufferedValueAt(col);
//		val = d->pCurrentItem->at(col);
//		val = d->pCurrentItem->at(d->curCol);
	}*/
/*	switch(columnType(col))
	{
		case QVariant::Date:
			#ifdef USE_KDE
//			val = KGlobal::locale()->formatDate(d->pCurrentItem->getDate(col), true);

			#else
//			val = d->pCurrentItem->getDate(col).toString(Qt::LocalDate);
			#endif
			break;

		default:
//			val = d->pCurrentItem->getText(d->curCol);
			val = d->pCurrentItem->getValue(d->curCol);

			break;
	}*/

	const bool startRowEdit = !d->rowEditing; //remember if we're starting row edit

	if (!d->rowEditing) {
		//we're starting row editing session
		m_data->clearRowEditBuffer();
		
		d->rowEditing = true;
		//indicate on the vheader that we are editing:
		d->pVerticalHeader->setEditRow(d->curRow);
		if (isInsertingEnabled() && d->pCurrentItem==d->pInsertItem) {
			//we should know that we are in state "new row editing"
			d->newRowEditing = true;
			//'insert' row editing: show another row after that:
			m_data->append( d->pInsertItem );
			//new empty insert item
			d->pInsertItem = new KexiTableItem(columns());
//			updateContents();
			d->pVerticalHeader->addLabel();
			QSize s(tableSize());
			resizeContents(s.width(), s.height());
			updateContents(columnPos(0), rowPos(row+1), viewport()->width(), d->rowHeight);
			qApp->processEvents(500);
			ensureVisible(columnPos(d->curCol), rowPos(row+1)+d->rowHeight-1, columnWidth(d->curCol), d->rowHeight);
		}
	}	
//	else {//just reinit
//		d->pAfterInsertItem->init(columns());
//			d->paintAfterInsertRow = true;
//	}

	d->pEditor = editor( col );
	if (!d->pEditor)
		return;

	d->pEditor->init(*bufferedValueAt(col), addText, removeOld);
	if (d->pEditor->hasFocusableWidget()) {
		moveChild(d->pEditor, columnPos(d->curCol), rowPos(d->curRow));

		d->pEditor->resize(columnWidth(d->curCol)-1, rowHeight()-1);
		d->pEditor->show();

		d->pEditor->setFocus();
	}

	if (startRowEdit)
		emit rowEditStarted(d->curRow);
}

void KexiTableView::focusInEvent(QFocusEvent*)
{
	updateCell(d->curRow, d->curCol);
}


void KexiTableView::focusOutEvent(QFocusEvent*)
{
	d->scrollBarTipTimer.stop();
	d->scrollBarTip->hide();
	
	updateCell(d->curRow, d->curCol);
}

bool KexiTableView::focusNextPrevChild(bool next)
{
	if (d->pEditor)
		return true;
	return QScrollView::focusNextPrevChild(next);
}

void KexiTableView::resizeEvent(QResizeEvent *e)
{
	QScrollView::resizeEvent(e);
	//updateGeometries();
	
	if (d->navPanel) {
		QRect g = d->navPanel->geometry();
//		kdDebug(44021) << "**********"<< g.top() << " " << g.left() <<endl;
		d->navPanel->setGeometry(
			frameWidth(),
			height() - horizontalScrollBar()->sizeHint().height()-frameWidth(),
			d->navPanel->sizeHint().width(), // - verticalScrollBar()->sizeHint().width() - horizontalScrollBar()->sizeHint().width(),
			horizontalScrollBar()->sizeHint().height()
		);

	if ((contentsHeight() - e->size().height()) <= d->rowHeight) {
		slotUpdate();
		triggerUpdate();
	}

/*		d->navPanel->setGeometry(
			frameWidth(),
			viewport()->height() +d->pTopHeader->height() 
			-(horizontalScrollBar()->isVisible() ? 0 : horizontalScrollBar()->sizeHint().height())
			+frameWidth(),
			d->navPanel->sizeHint().width(), // - verticalScrollBar()->sizeHint().width() - horizontalScrollBar()->sizeHint().width(),
			horizontalScrollBar()->sizeHint().height()
		);*/
//		updateContents();
//		d->navPanel->setGeometry(1,horizontalScrollBar()->pos().y(),
	//		d->navPanel->width(), horizontalScrollBar()->height());
	}
//	updateContents(0,0,2000,2000);//js
//	erase(); repaint();
}

void KexiTableView::viewportResizeEvent( QResizeEvent *e )
{
	QScrollView::viewportResizeEvent( e );
	updateGeometries();
//	erase(); repaint();
}

void KexiTableView::showEvent(QShowEvent *e)
{
	QScrollView::showEvent(e);
	if (!d->maximizeColumnsWidthOnShow.isEmpty()) {
		maximizeColumnsWidth(d->maximizeColumnsWidthOnShow);
		d->maximizeColumnsWidthOnShow.clear();
	}

	if (d->initDataContentsOnShow) {
		//full init
		d->initDataContentsOnShow = false;
		initDataContents();
	}
	else {
		//just update size
		QSize s(tableSize());
//	QRect r(cellGeometry(rows() - 1 + (isInsertingEnabled()?1:0), columns() - 1 ));
//	resizeContents(r.right() + 1, r.bottom() + 1);
		resizeContents(s.width(),s.height());
	}
	updateGeometries();
}

bool KexiTableView::dropsAtRowEnabled() const
{
	return d->dropsAtRowEnabled;
}

void KexiTableView::setDropsAtRowEnabled(bool set)
{
	const bool old = d->dropsAtRowEnabled;
	if (!set)
		d->dragIndicatorLine = -1;
	if (d->dropsAtRowEnabled && !set) {
		d->dropsAtRowEnabled = false;
		update();
	}
	else {
		d->dropsAtRowEnabled = set;
	}
}

void KexiTableView::contentsDragMoveEvent(QDragMoveEvent *e)
{
	CHECK_DATA_;
	if (d->dropsAtRowEnabled) {
		QPoint p = e->pos();
		int row = rowAt(p.y());
		KexiTableItem *item = m_data->at(row);
		emit dragOverRow(item, row, e);
		if (e->isAccepted()) {
			if (d->dragIndicatorLine>=0 && d->dragIndicatorLine != row) {
				//erase old indicator
				updateRow(d->dragIndicatorLine);
			}
			if (d->dragIndicatorLine != row) {
				d->dragIndicatorLine = row;
				updateRow(d->dragIndicatorLine);
			}
		}
		else {
			if (d->dragIndicatorLine>=0) {
				//erase old indicator
				updateRow(d->dragIndicatorLine);
			}
			d->dragIndicatorLine = -1;
		}
	}
	else
		e->acceptAction(false);
/*	for(QStringList::Iterator it = d->dropFilters.begin(); it != d->dropFilters.end(); it++)
	{
		if(e->provides((*it).latin1()))
		{
			e->acceptAction(true);
			return;
		}
	}*/
//	e->acceptAction(false);
}

void KexiTableView::contentsDropEvent(QDropEvent *ev)
{
	CHECK_DATA_;
	if (d->dropsAtRowEnabled) {
		//we're no longer dragging over the table
		if (d->dragIndicatorLine>=0) {
			int row2update = d->dragIndicatorLine;
			d->dragIndicatorLine = -1;
			updateRow(row2update);
		}
		QPoint p = ev->pos();
		int row = rowAt(p.y());
		KexiTableItem *item = m_data->at(row);
		KexiTableItem *newItem = 0;
		emit droppedAtRow(item, row, ev, newItem);
		if (newItem) {
			const int realRow = (row==d->curRow ? -1 : row);
			insertItem(newItem, realRow);
			setCursor(row, 0);
//			d->pCurrentItem = newItem;
		}
	}
}

void KexiTableView::viewportDragLeaveEvent( QDragLeaveEvent * )
{
	CHECK_DATA_;
	if (d->dropsAtRowEnabled) {
		//we're no longer dragging over the table
		if (d->dragIndicatorLine>=0) {
			int row2update = d->dragIndicatorLine;
			d->dragIndicatorLine = -1;
			updateRow(row2update);
		}
	}
}

void KexiTableView::updateCell(int row, int col)
{
//	kdDebug(44021) << "updateCell("<<row<<", "<<col<<")"<<endl;
	updateContents(cellGeometry(row, col));
/*	QRect r = cellGeometry(row, col);
	r.setHeight(r.height()+6);
	r.setTop(r.top()-3);
	updateContents();*/
}

void KexiTableView::updateRow(int row)
{
//	kdDebug(44021) << "updateRow("<<row<<")"<<endl;
	int leftcol = d->pTopHeader->sectionAt( d->pTopHeader->offset() );
//	int rightcol = d->pTopHeader->sectionAt( clipper()->width() );
	updateContents( QRect( columnPos( leftcol ), rowPos(row), clipper()->width(), rowHeight() ) ); //columnPos(rightcol)+columnWidth(rightcol), rowHeight() ) );
}

void KexiTableView::columnWidthChanged( int, int, int )
{
	QSize s(tableSize());
	int w = contentsWidth();
	viewport()->setUpdatesEnabled(false);
	resizeContents( s.width(), s.height() );
	viewport()->setUpdatesEnabled(true);
	if (contentsWidth() < w)
		updateContents(contentsX(), 0, viewport()->width(), contentsHeight());
//		repaintContents( s.width(), 0, w - s.width() + 1, contentsHeight(), TRUE );
	else
	//	updateContents( columnPos(col), 0, contentsWidth(), contentsHeight() );
		updateContents(contentsX(), 0, viewport()->width(), contentsHeight());
	//	viewport()->repaint();

//	updateContents(0, 0, d->pBufferPm->width(), d->pBufferPm->height());
	if (d->pEditor)
	{
		d->pEditor->resize(columnWidth(d->curCol)-1, rowHeight()-1);
		moveChild(d->pEditor, columnPos(d->curCol), rowPos(d->curRow));
	}
	updateGeometries();
}

void KexiTableView::updateGeometries()
{
	QSize ts = tableSize();
	if (d->pTopHeader->offset() && ts.width() < (d->pTopHeader->offset() + d->pTopHeader->width()))
		horizontalScrollBar()->setValue(ts.width() - d->pTopHeader->width());

//	d->pVerticalHeader->setGeometry(1, topMargin() + 1, leftMargin(), visibleHeight());
	d->pTopHeader->setGeometry(leftMargin() + 1, 1, visibleWidth(), topMargin());
	d->pVerticalHeader->setGeometry(1, topMargin() + 1, leftMargin(), visibleHeight());
}

int KexiTableView::columnWidth(int col) const
{
	CHECK_DATA(0);
	int vcID = m_data->visibleColumnID( col );
	return vcID==-1 ? 0 : d->pTopHeader->sectionSize( vcID );
}

int KexiTableView::rowHeight() const
{
	return d->rowHeight;
}

int KexiTableView::columnPos(int col) const
{
	CHECK_DATA(0);
	//if this column is hidden, find first column before that is visible
	int c = QMIN(col, (int)m_data->columnsCount()-1), vcID = 0;
	while (c>=0 && (vcID=m_data->visibleColumnID( c ))==-1)
		c--;
	if (c<0)
		return 0;
	if (c==col)
		return d->pTopHeader->sectionPos(vcID);
	return d->pTopHeader->sectionPos(vcID)+d->pTopHeader->sectionSize(vcID);
}

int KexiTableView::rowPos(int row) const
{
	return d->rowHeight*row;
}

int KexiTableView::columnAt(int pos) const
{
	CHECK_DATA(-1);
	int r = d->pTopHeader->sectionAt(pos);
	if (r<0)
		return r;
	return m_data->globalColumnID( r );

//	if (r==-1)
//		kdDebug() << "columnAt("<<pos<<")==-1 !!!" << endl;
//	return r;
}

int KexiTableView::rowAt(int pos, bool ignoreEnd) const
{
	CHECK_DATA(-1);
	pos /=d->rowHeight;
	if (pos < 0)
		return 0;
	if ((pos >= (int)m_data->count()) && !ignoreEnd)
		return -1;
	return pos;
}

QRect KexiTableView::cellGeometry(int row, int col) const
{
	return QRect(columnPos(col), rowPos(row),
		columnWidth(col), rowHeight());
}

QSize KexiTableView::tableSize() const
{
	if ((rows()+ (isInsertingEnabled()?1:0) ) > 0 && columns() > 0) {
/*		kdDebug() << "tableSize()= " << columnPos( columns() - 1 ) + columnWidth( columns() - 1 ) 
			<< ", " << rowPos( rows()-1+(isInsertingEnabled()?1:0)) + d->rowHeight
//			+ QMAX(d->navPanel ? d->navPanel->height() : 0, horizontalScrollBar()->sizeHint().height())
			+ (d->navPanel->isVisible() ? QMAX( d->navPanel->height(), horizontalScrollBar()->sizeHint().height() ) :0 )
			+ margin() << endl;
*/
//		kdDebug()<< d->navPanel->isVisible() <<" "<<d->navPanel->height()<<" "
//		<<horizontalScrollBar()->sizeHint().height()<<" "<<rowPos( rows()-1+(isInsertingEnabled()?1:0))<<endl;

		return QSize( 
			columnPos( columns() - 1 ) + columnWidth( columns() - 1 ),
			rowPos( rows()-1+(isInsertingEnabled()?1:0)) + d->rowHeight
//			+ QMAX( d->navPanel ? d->navPanel->height() : 0, horizontalScrollBar()->sizeHint().height())
			+ (d->navPanel->isVisible() ? QMAX( d->navPanel->height(), horizontalScrollBar()->sizeHint().height() ) :0 )
//			- (horizontalScrollBar()->isVisible() ? horizontalScrollBar()->sizeHint().height() :0 )
			+ margin() 
//-2*d->rowHeight
		);
//			+horizontalScrollBar()->sizeHint().height() + margin() );
	}
	return QSize(0,0);
}

int KexiTableView::rows() const
{
	CHECK_DATA(0);
	return m_data->count();
}

int KexiTableView::columns() const
{
	CHECK_DATA(0);
	return m_data->columns.count();
}

void KexiTableView::ensureCellVisible(int row, int col/*=-1*/)
{
	//quite clever: ensure the cell is visible:
	QRect r( columnPos(col==-1 ? d->curCol : col), rowPos(row), 
		columnWidth(col==-1 ? d->curCol : col), rowHeight());

/*	if (d->navPanel && horizontalScrollBar()->isHidden() && row == rows()-1) {
		//when cursor is moved down and navigator covers the cursor's area,
		//area is scrolled up
		if ((viewport()->height() - d->navPanel->height()) < r.bottom()) {
			scrollBy(0,r.bottom() - (viewport()->height() - d->navPanel->height()));
		}
	}*/

	if (d->navPanel && horizontalScrollBar()->isHidden()) {
		//a hack: for visible navigator: increase height of the visible rect 'r'
		r.setBottom(r.bottom()+d->navPanel->height());
	}

	QPoint pcenter = r.center();
	ensureVisible(pcenter.x(), pcenter.y(), r.width()/2, r.height()/2);
	slotUpdate();
}

void KexiTableView::setCursor(int row, int col/*=-1*/, bool forceSet)
{
	int newrow = row;
	int newcol = col;

	if(rows() <= 0) {
		d->pVerticalHeader->setCurrentRow(-1);
		if (isInsertingEnabled()) {
			d->pCurrentItem=d->pInsertItem;
			newrow=0;
			if (col>=0)
				newcol=col;
			else
				newcol=0;
		}
		else {
			d->pCurrentItem=0;
			d->curRow=-1;
			d->curCol=-1;
			return;
		}
	}

	if(col>=0)
	{
		newcol = QMAX(0, col);
		newcol = QMIN(columns() - 1, newcol);
	}
	else {
		newcol = d->curCol; //no changes
		newcol = QMAX(0, newcol); //may not be < 0 !
	}
	newrow = QMAX( 0, row);
	newrow = QMIN( rows() - 1 + (isInsertingEnabled()?1:0), newrow);

//	d->pCurrentItem = itemAt(d->curRow);
//	kdDebug(44021) << "setCursor(): d->curRow=" << d->curRow << " oldRow=" << oldRow << " d->curCol=" << d->curCol << " oldCol=" << oldCol << endl;

	if ( forceSet || d->curRow != newrow || d->curCol != newcol )
	{
		kdDebug(44021) << "setCursor(): " <<QString("old:%1,%2 new:%3,%4").arg(d->curCol).arg(d->curRow).arg(newcol).arg(newrow) << endl;
		
		// cursor moved: get rid of editor
		if (d->pEditor) {
			if (!d->contentsMousePressEvent_dblClick) {
				if (!acceptEditor()) {
					return;
				}
			}
		}

		if (d->curRow != newrow) {//update current row info
			setNavRowNumber(newrow);
//			d->navBtnPrev->setEnabled(newrow>0);
//			d->navBtnFirst->setEnabled(newrow>0);
//			d->navBtnNext->setEnabled(newrow<(rows()-1+(isInsertingEnabled()?1:0)));
//			d->navBtnLast->setEnabled(newrow!=(rows()-1));
		}

		// cursor moved to other row: end of row editing
		if (d->rowEditing && d->curRow != newrow) {
			if (!acceptRowEdit()) {
				//accepting failed: cancel setting the cursor
				return;
			}
			//update row number, because number of rows changed
			newrow = QMIN( rows() - 1 + (isInsertingEnabled()?1:0), newrow);
		}

		//change position
		int oldRow = d->curRow;
		int oldCol = d->curCol;
		d->curRow = newrow;
		d->curCol = newcol;

//		int cw = columnWidth( d->curCol );
//		int rh = rowHeight();
//		ensureVisible( columnPos( d->curCol ) + cw / 2, rowPos( d->curRow ) + rh / 2, cw / 2, rh / 2 );
//		center(columnPos(d->curCol) + cw / 2, rowPos(d->curRow) + rh / 2, cw / 2, rh / 2);
//	kdDebug(44021) << " contentsY() = "<< contentsY() << endl;

//js		if (oldRow > d->curRow)
//js			ensureVisible(columnPos(d->curCol), rowPos(d->curRow) + rh, columnWidth(d->curCol), rh);
//js		else// if (oldRow <= d->curRow)
//js		ensureVisible(columnPos(d->curCol), rowPos(d->curRow), columnWidth(d->curCol), rh);


		//show editor-dependent focus, if we're changing the current column
		if (oldCol>=0 && oldCol<columns() && d->curCol!=oldCol) {
			//find the editor for this column
			KexiTableEdit *edit = editor( oldCol );
			if (edit) {
				edit->hideFocus();
			}
		}

		//show editor-dependent focus, if needed
		//find the editor for this column
		editorShowFocus( d->curRow, d->curCol );

		updateCell( oldRow, oldCol );

		//quite clever: ensure the cell is visible:
		ensureCellVisible(d->curRow, d->curCol);

//		QPoint pcenter = QRect( columnPos(d->curCol), rowPos(d->curRow), columnWidth(d->curCol), rh).center();
//		ensureVisible(pcenter.x(), pcenter.y(), columnWidth(d->curCol)/2, rh/2);

//		ensureVisible(columnPos(d->curCol), rowPos(d->curRow) - contentsY(), columnWidth(d->curCol), rh);
		d->pVerticalHeader->setCurrentRow(d->curRow);
		updateCell( d->curRow, d->curCol );
		if (d->curCol != oldCol || d->curRow != oldRow ) //ensure this is also refreshed
			updateCell( oldRow, d->curCol );
		if (isInsertingEnabled() && d->curRow == rows()) {
			kdDebug(44021) << "NOW insert item is current" << endl;
			d->pCurrentItem = d->pInsertItem;
		}
		else {
			kdDebug(44021) << QString("NOW item at %1 (%2) is current").arg(d->curRow).arg((ulong)itemAt(d->curRow)) << endl;
//NOT EFFECTIVE!!!!!!!!!!!
			d->pCurrentItem = itemAt(d->curRow);
		}

		emit itemSelected(d->pCurrentItem);
		emit cellSelected(d->curCol, d->curRow);
	}
}

void KexiTableView::removeEditor()
{
	if (!d->pEditor)
		return;

//	d->pEditor->blockSignals(true);
	d->pEditor->hide();
//	delete d->pEditor;
	d->pEditor = 0;
	viewport()->setFocus();
}

bool KexiTableView::acceptEditor()
{
	CHECK_DATA(true);
	if (!d->pEditor || d->inside_acceptEditor)
		return true;

	d->inside_acceptEditor = true;//avoid recursion

	QVariant newval;
	KexiValidator::Result res = KexiValidator::Ok;
	QString msg, desc;
	bool setNull = false;
//	bool allow = true;
//	static const QString msg_NOT_NULL = i18n("\"%1\" column requires a value to be entered.");

	if (d->pEditor->valueIsNull()) {//null value entered
		if (d->pEditor->field()->isNotNull()) {
			kdDebug() << "KexiTableView::acceptEditor(): NULL NOT ALLOWED!" << endl;
			res = KexiValidator::Error;
			msg = KexiValidator::msgColumnNotEmpty().arg(d->pEditor->field()->captionOrName());
			desc = i18n("The column's constraint is declared as NOT NULL.");
//			allow = false;
//			removeEditor();
//			return true;
		}
		else {
			kdDebug() << "KexiTableView::acceptEditor(): NULL VALUE WILL BE SET" << endl;
			//ok, just leave newval as NULL
			setNull = true;
		}
	}
	else if (d->pEditor->valueIsEmpty()) {//empty value entered
		if (d->pEditor->field()->hasEmptyProperty()) {
			if (d->pEditor->field()->isNotEmpty()) {
				kdDebug() << "KexiTableView::acceptEditor(): EMPTY NOT ALLOWED!" << endl;
				res = KexiValidator::Error;
				msg = KexiValidator::msgColumnNotEmpty().arg(d->pEditor->field()->captionOrName());
				desc = i18n("The column's constraint is declared as NOT EMPTY.");
//				allow = false;
//				removeEditor();
//				return true;
			}
			else {
				kdDebug() << "KexiTableView::acceptEditor(): EMPTY VALUE WILL BE SET" << endl;
			}
		}
		else {
			if (d->pEditor->field()->isNotNull()) {
				kdDebug() << "KexiTableView::acceptEditor(): NEITHER NULL NOR EMPTY VALUE CAN BE SET!" << endl;
				res = KexiValidator::Error;
				msg = KexiValidator::msgColumnNotEmpty().arg(d->pEditor->field()->captionOrName());
				desc = i18n("The column's constraint is declared as NOT EMPTY and NOT NULL.");
//				allow = false;
//				removeEditor();
//				return true;
			}
			else {
				kdDebug() << "KexiTableView::acceptEditor(): NULL VALUE WILL BE SET BECAUSE EMPTY IS NOT ALLOWED" << endl;
				//ok, just leave newval as NULL
				setNull = true;
			}
		}
	}

	//try to get the value entered:
	if (res == KexiValidator::Ok) {
		if (!setNull && !d->pEditor->valueChanged()
			|| setNull && d->pCurrentItem->at(d->curCol).isNull()) {
			kdDebug() << "KexiTableView::acceptEditor(): VALUE NOT CHANGED." << endl;
			removeEditor();
			d->inside_acceptEditor = false;
			if (d->acceptsRowEditAfterCellAccepting || d->internal_acceptsRowEditAfterCellAccepting)
				acceptRowEdit();
			return true;
		}
		if (!setNull) {//get the new value 
			bool ok;
			newval = d->pEditor->value(ok);
			if (!ok) {
				kdDebug() << "KexiTableView::acceptEditor(): INVALID VALUE - NOT CHANGED." << endl;
				res = KexiValidator::Error;
//js: TODO get detailed info on why d->pEditor->value() failed
				msg = i18n("Entered value is invalid.");
//				removeEditor();
//				return true;
			}
		}

		//Check other validation rules:
		//1. check using validator
		KexiValidator *validator = m_data->column(d->curCol)->validator();
		if (validator) {
			res = validator->check(m_data->column(d->curCol)->field->captionOrName(), 
				newval, msg, desc);
		}
	}

	//show the validation result if not OK:
	if (res == KexiValidator::Error) {
		if (desc.isEmpty())
			KMessageBox::sorry(this, msg);
		else
			KMessageBox::detailedSorry(this, msg, desc);
//		allow = false;
	}
	else if (res == KexiValidator::Warning) {
		//js: todo: message!!!
		KMessageBox::messageBox(this, KMessageBox::Sorry, msg + "\n" + desc);
	}

	if (res == KexiValidator::Ok) {
		//2. check using signal
		//bool allow = true;
//		emit aboutToChangeCell(d->pCurrentItem, newval, allow);
//		if (allow) {
		//send changes to the backend
		if (m_data->updateRowEditBuffer(d->pCurrentItem,d->curCol,newval)) {
			kdDebug() << "KexiTableView::acceptEditor(): ------ EDIT BUFFER CHANGED TO:" << endl;
			m_data->rowEditBuffer()->debug();
		} else {
			kdDebug() << "KexiTableView::acceptEditor(): ------ CHANGE FAILED in KexiTableViewData::updateRowEditBuffer()" << endl;
			res = KexiValidator::Error;
		}
	}

	if (res == KexiValidator::Ok) {
		removeEditor();
		emit itemChanged(d->pCurrentItem, d->curRow, d->curCol, d->pCurrentItem->at(d->curCol));
		emit itemChanged(d->pCurrentItem, d->curRow, d->curCol);
	}
	d->inside_acceptEditor = false;
	if (res == KexiValidator::Ok) {
		if (d->acceptsRowEditAfterCellAccepting || d->internal_acceptsRowEditAfterCellAccepting)
			acceptRowEdit();
		return true;
	}
	return false;
}

void KexiTableView::cancelEditor()
{
	if (!d->pEditor)
		return;

	removeEditor();
}

bool KexiTableView::acceptRowEdit()
{
	if (!d->rowEditing)
		return true;
	if (d->inside_acceptEditor) {
		d->internal_acceptsRowEditAfterCellAccepting = true;
		return true;
	}
	d->internal_acceptsRowEditAfterCellAccepting = false;
	if (!acceptEditor())
		return false;
	kdDebug() << "EDIT ROW ACCEPTING..." << endl;

	bool success = true;
//	bool allow = true;
//	int faultyColumn = -1; // will be !=-1 if cursor has to be moved to that column
	const bool inserting = d->newRowEditing;
//	QString msg, desc;
//	bool inserting = d->pInsertItem && d->pInsertItem==d->pCurrentItem;

	if (m_data->rowEditBuffer()->isEmpty()) {
		if (d->newRowEditing) {
			cancelRowEdit();
			kdDebug() << "-- NOTHING TO INSERT!!!" << endl;
			return true;
		}
		else {
//			success = true;
			kdDebug() << "-- NOTHING TO ACCEPT!!!" << endl;
		}
	}
	else {//not empty edit buffer:
		if (d->newRowEditing) {
//			emit aboutToInsertRow(d->pCurrentItem, m_data->rowEditBuffer(), success, &faultyColumn);
//			if (success) {
			kdDebug() << "-- INSERTING: " << endl;
			m_data->rowEditBuffer()->debug();
			success = m_data->saveNewRow(*d->pCurrentItem);
//				if (!success) {
//				}
//			}
		}
		else {
//			emit aboutToUpdateRow(d->pCurrentItem, m_data->rowEditBuffer(), success, &faultyColumn);
			if (success) {
				//accept changes for this row:
				kdDebug() << "-- UPDATING: " << endl;
				m_data->rowEditBuffer()->debug();
				success = m_data->saveRowChanges(*d->pCurrentItem);//, &msg, &desc, &faultyColumn);
//				if (!success) {
//				}
			}
		}
	}

	if (success) {
		//editing is finished:
		d->rowEditing = false;
		d->newRowEditing = false;
		//indicate on the vheader that we are not editing
		d->pVerticalHeader->setEditRow(-1);
		//redraw
		updateRow(d->curRow);

		kdDebug() << "EDIT ROW ACCEPTED:" << endl;
		/*debug*/itemAt(d->curRow);

		if (inserting) {
//			emit rowInserted(d->pCurrentItem);
			//update navigator's data
			setNavRowCount(rows());
		}
		else {
//			emit rowUpdated(d->pCurrentItem);
		}

		emit rowEditTerminated(d->curRow);
	}
	else {
//		if (!allow) {
//			kdDebug() << "INSERT/EDIT ROW - DISALLOWED by signal!" << endl;
//		}
//		else {
//			kdDebug() << "EDIT ROW - ERROR!" << endl;
//		}
		if (m_data->result()->column>=0 && m_data->result()->column<columns()) {
			//move to faulty column
			setCursor(d->curRow, m_data->result()->column);
		}
		if (m_data->result()->desc.isEmpty())
			KMessageBox::sorry(this, m_data->result()->msg);
		else
			KMessageBox::detailedSorry(this, m_data->result()->msg, m_data->result()->desc);
	}

	return success;
}

void KexiTableView::cancelRowEdit()
{
	CHECK_DATA_;
	if (!d->rowEditing)
		return;
	cancelEditor();
	d->rowEditing = false;
	//indicate on the vheader that we are not editing
	d->pVerticalHeader->setEditRow(-1);
	if (d->newRowEditing) {
		d->newRowEditing = false;
		//remove current edited row (it is @ the end of list)
		m_data->removeLast();
		//current item is now empty, last row
		d->pCurrentItem = d->pInsertItem;
		//update visibility
		d->pVerticalHeader->removeLabel(false); //-1 label
//		updateContents(columnPos(0), rowPos(rows()), 
//			viewport()->width(), d->rowHeight*3 + (d->navPanel ? d->navPanel->height() : 0)*3 );
		updateContents(); //js: above didnt work well so we do that dirty
//TODO: still doesn't repaint properly!!
		QSize s(tableSize());
		resizeContents(s.width(), s.height());
		d->pVerticalHeader->update();
		//--no cancel action is needed for datasource, 
		//  because the row was not yet stored.
	}

	m_data->clearRowEditBuffer();
	updateRow(d->curRow);
	
//! \todo (js): cancel changes for this row!
	kdDebug(44021) << "EDIT ROW CANCELLED." << endl;

	emit rowEditTerminated(d->curRow);
}

bool KexiTableView::acceptsRowEditAfterCellAccepting() const
{
	return d->acceptsRowEditAfterCellAccepting;
}

void KexiTableView::setAcceptsRowEditAfterCellAccepting(bool set)
{
	d->acceptsRowEditAfterCellAccepting = set;
}

/*void KexiTableView::setInsertionPolicy(InsertionPolicy policy)
{
	d->insertionPolicy = policy;
//	updateContextMenu();
}

KexiTableView::InsertionPolicy KexiTableView::insertionPolicy() const
{
	return d->insertionPolicy;
}*/

void KexiTableView::setDeletionPolicy(DeletionPolicy policy)
{
	d->deletionPolicy = policy;
//	updateContextMenu();
}

KexiTableView::DeletionPolicy KexiTableView::deletionPolicy() const
{
	return d->deletionPolicy;
}

#if 0
bool KexiTableView::updateContextMenu()
{
  // delete d->pContextMenu;
  //  d->pContextMenu = 0L;
//  d->pContextMenu->clear();
//	if(d->pCurrentItem && d->pCurrentItem->isInsertItem())
//	return;

//	if(d->additionPolicy != NoAdd || d->deletionPolicy != NoDelete)
//	{
//		d->pContextMenu = new QPopupMenu(this);
  d->pContextMenu->setItemVisible(d->menu_id_addRecord, d->additionPolicy != NoAdd);
#if 0 //todo(js)
  d->pContextMenu->setItemVisible(d->menu_id_removeRecord, d->deletionPolicy != NoDelete
	&& d->pCurrentItem && !d->pCurrentItem->isInsertItem());
#else
  d->pContextMenu->setItemVisible(d->menu_id_removeRecord, d->deletionPolicy != NoDelete
	&& d->pCurrentItem);
#endif
  for (int i=0; i<(int)d->pContextMenu->count(); i++) {
	if (d->pContextMenu->isItemVisible( d->pContextMenu->idAt(i) ))
	  return true;
  }
  return false;
}
#endif

//(js) unused
void KexiTableView::slotAutoScroll()
{
	kdDebug(44021) << "KexiTableView::slotAutoScroll()" <<endl;
	if (!d->needAutoScroll)
		return;

	switch(d->scrollDirection)
	{
		case ScrollDown:
			setCursor(d->curRow + 1, d->curCol);
			break;

		case ScrollUp:
			setCursor(d->curRow - 1, d->curCol);
			break;
		case ScrollLeft:
			setCursor(d->curRow, d->curCol - 1);
			break;

		case ScrollRight:
			setCursor(d->curRow, d->curCol + 1);
			break;
	}
}

#ifndef KEXI_NO_PRINT
void
KexiTableView::print(KPrinter &/*printer*/)
{
//	printer.setFullPage(true);
#if 0
	int leftMargin = printer.margins().width() + 2 + d->rowHeight;
	int topMargin = printer.margins().height() + 2;
//	int bottomMargin = topMargin + ( printer.realPageSize()->height() * printer.resolution() + 36 ) / 72;
	int bottomMargin = 0;
	kdDebug(44021) << "KexiTableView::print: bottom = " << bottomMargin << endl;

	QPainter p(&printer);

	KexiTableItem *i;
	int width = leftMargin;
	for(int col=0; col < columns(); col++)
	{
		p.fillRect(width, topMargin - d->rowHeight, columnWidth(col), d->rowHeight, QBrush(gray));
		p.drawRect(width, topMargin - d->rowHeight, columnWidth(col), d->rowHeight);
		p.drawText(width, topMargin - d->rowHeight, columnWidth(col), d->rowHeight, AlignLeft | AlignVCenter, d->pTopHeader->label(col));
		width = width + columnWidth(col);
	}

	int yOffset = topMargin;
	int row = 0;
	int right = 0;
	for(i = m_data->first(); i; i = m_data->next())
	{
		if(!i->isInsertItem())
		{	kdDebug(44021) << "KexiTableView::print: row = " << row << " y = " << yOffset << endl;
			int xOffset = leftMargin;
			for(int col=0; col < columns(); col++)
			{
				kdDebug(44021) << "KexiTableView::print: col = " << col << " x = " << xOffset << endl;
				p.saveWorldMatrix();
				p.translate(xOffset, yOffset);
				paintCell(&p, i, col, QRect(0, 0, columnWidth(col) + 1, d->rowHeight), true);
				p.restoreWorldMatrix();
//			p.drawRect(xOffset, yOffset, columnWidth(col), d->rowHeight);
				xOffset = xOffset + columnWidth(col);
				right = xOffset;
			}

			row++;
			yOffset = topMargin  + row * d->rowHeight;
		}

		if(yOffset > 900)
		{
			p.drawLine(leftMargin, topMargin, leftMargin, yOffset);
			p.drawLine(leftMargin, topMargin, right - 1, topMargin);
			printer.newPage();
			yOffset = topMargin;
			row = 0;
		}
	}
	p.drawLine(leftMargin, topMargin, leftMargin, yOffset);
	p.drawLine(leftMargin, topMargin, right - 1, topMargin);

//	p.drawLine(60,60,120,150);
	p.end();
#endif
}
#endif

KexiTableRM* KexiTableView::verticalHeader() const
{
	return d->pVerticalHeader; 
}

QString KexiTableView::columnCaption(int colNum) const
{
	return d->pTopHeader->label(colNum);
}

KexiDB::Field* KexiTableView::field(int colNum) const
{
	if (!m_data || !m_data->column(colNum))
		return 0;
	return m_data->column(colNum)->field;
}

void KexiTableView::adjustColumnWidthToContents(int colNum)
{
	CHECK_DATA_;
	if (columns()<=colNum || colNum < -1)
		return;

	if (colNum==-1) {
		const int cols = columns();
		for (int i=0; i<columns(); i++)
			adjustColumnWidthToContents(i);
		return;
	}

	KexiCellEditorFactoryItem *item = KexiCellEditorFactory::item( columnType(colNum) );
	if (!item)
		return;
	QFontMetrics fm(font());
	int maxw = fm.width( d->pTopHeader->label( colNum ) );
//	int start = rowAt(contentsY());
//	int end = QMAX( start, rowAt( contentsY() + viewport()->height() - 1 ) );
//	for (int i=start; i<=end; i++) {

//js TODO: this is NOT EFFECTIVE for big data sets!!!!

	KexiTableEdit *ed = editor( colNum );
	KexiDB::Field *f = m_data->column( colNum )->field;
	if (ed) {
//		KexiDB::Field *f = m_data->column(colNum)->field;
		for (QPtrListIterator<KexiTableItem> it( *m_data ); it.current(); ++it) {
			maxw = QMAX( maxw, ed->widthForValue( it.current()->at( colNum ), fm ) );
//			maxw = QMAX( maxw, item->widthForValue( *f, it.current()->at( colNum ), fm ) );
		}
		maxw += (fm.width("  ") + ed->leftMargin() + ed->rightMargin());
	}
	if (maxw < KEXITV_MINIMUM_COLUMN_WIDTH )
		maxw = KEXITV_MINIMUM_COLUMN_WIDTH; //not too small
	setColumnWidth( colNum, maxw );
}

void KexiTableView::setColumnWidth(int colNum, int width)
{
	if (columns()<=colNum || colNum < 0)
		return;
	const int oldWidth = d->pTopHeader->sectionSize( colNum );
	d->pTopHeader->resizeSection( colNum, width );
	slotTopHeaderSizeChange( colNum, oldWidth, d->pTopHeader->sectionSize( colNum ) );
}

void KexiTableView::maximizeColumnsWidth( const QValueList<int> &columnList )
{
	if (!isVisible()) {
		d->maximizeColumnsWidthOnShow += columnList;
		return;
	}
	if (width() <= d->pTopHeader->headerWidth())
		return;
	//sort the list and make it unique
	QValueList<int>::const_iterator it;
	QValueList<int> cl, sortedList = columnList;
	qHeapSort(sortedList);
	int i=-999;

#if (QT_VERSION >= 0x030200) //TMP
	for (it=sortedList.constBegin(); it!=sortedList.end(); ++it) {
#else
	for (it=sortedList.begin(); it!=sortedList.end(); ++it) {
#endif
		if (i!=(*it)) {
			cl += (*it);
			i = (*it);
		}
	}
	//resize
	int sizeToAdd = (width() - d->pTopHeader->headerWidth()) / cl.count() - verticalHeader()->width();
	if (sizeToAdd<=0)
		return;
#if (QT_VERSION >= 0x030200) //TMP
	for (it=cl.constBegin(); it!=cl.end(); ++it) {
#else
	for (it=cl.begin(); it!=cl.end(); ++it) {
#endif
		int w = d->pTopHeader->sectionSize(*it);
		if (w>0) {
			d->pTopHeader->resizeSection(*it, w+sizeToAdd);
		}
	}
	updateContents();
	editorShowFocus( d->curRow, d->curCol );
}

void KexiTableView::adjustHorizontalHeaderSize()
{
	d->pTopHeader->adjustHeaderSize();
}

void KexiTableView::setColumnStretchEnabled( bool set, int colNum )
{
	d->pTopHeader->setStretchEnabled( set, colNum );
}

int KexiTableView::currentColumn() const
{ 
	return d->curCol;
}

int KexiTableView::currentRow() const
{
	return d->curRow;
}

KexiTableItem *KexiTableView::selectedItem() const
{
	return d->pCurrentItem;
}

void KexiTableView::setBackgroundAltering(bool altering) { d->bgAltering = altering; }
bool KexiTableView::backgroundAltering()  const { return d->bgAltering; }

void KexiTableView::setEditableOnDoubleClick(bool set) { d->editOnDoubleClick = set; }
bool KexiTableView::editableOnDoubleClick() const { return d->editOnDoubleClick; }

void KexiTableView::setEmptyAreaColor(const QColor& c)
{
	d->emptyAreaColor = c;
}

QColor KexiTableView::emptyAreaColor() const
{
	return d->emptyAreaColor;
}

bool KexiTableView::fullRowSelectionEnabled() const
{
	return d->fullRowSelectionEnabled;
}

void KexiTableView::setFullRowSelectionEnabled(bool set)
{
	if (d->fullRowSelectionEnabled == set)
		return;
	if (set) {
		d->rowHeight -= 1;
	}
	else {
		d->rowHeight += 1;
	}
	d->fullRowSelectionEnabled = set;
	if (d->pVerticalHeader)
		d->pVerticalHeader->setCellHeight(d->rowHeight);
	if (d->pTopHeader) {
		setMargins(
			QMIN(d->pTopHeader->sizeHint().height(), d->rowHeight),
			d->pTopHeader->sizeHint().height(), 0, 0);
	}
	setFont(font());//update
}

bool KexiTableView::verticalHeaderVisible() const
{
	return d->pVerticalHeader->isVisible();
}

void KexiTableView::setVerticalHeaderVisible(bool set)
{
	int left_width;
	if (set) {
		d->pVerticalHeader->show();
		left_width = QMIN(d->pTopHeader->sizeHint().height(), d->rowHeight);
	}
	else {
		d->pVerticalHeader->hide();
		left_width = 0;
	}
	setMargins( left_width, horizontalHeaderVisible() ? d->pTopHeader->sizeHint().height() : 0, 0, 0);
}

bool KexiTableView::horizontalHeaderVisible() const
{
	return d->pTopHeader->isVisible();
}

void KexiTableView::setHorizontalHeaderVisible(bool set)
{
	int top_height;
	if (set) {
		d->pTopHeader->show();
		top_height = d->pTopHeader->sizeHint().height();
	}
	else {
		d->pTopHeader->hide();
		top_height = 0;
	}
	setMargins( verticalHeaderVisible() ? d->pVerticalHeader->width() : 0, top_height, 0, 0);
}

void KexiTableView::triggerUpdate()
{
	kdDebug(44021) << "KexiTableView::triggerUpdate()" << endl;
//	if (!d->pUpdateTimer->isActive())
		d->pUpdateTimer->start(20, true);
//		d->pUpdateTimer->start(200, true);
}

int KexiTableView::columnType(int col) const
{
	return m_data ? m_data->column(col)->field->type() : KexiDB::Field::InvalidType;
}

bool KexiTableView::columnEditable(int col) const
{
	return m_data ? !m_data->column(col)->readOnly() : false;
}

QVariant KexiTableView::columnDefaultValue(int col) const
{
	return QVariant(0);
//TODO(js)	
//	return m_data->columns[col].defaultValue;
}

void KexiTableView::setReadOnly(bool set)
{
	if (isReadOnly() == set || (m_data && m_data->isReadOnly() && !set))
		return; //not allowed!
	d->readOnly = (set ? 1 : 0);
	if (set)
		setInsertingEnabled(false);
	update();
	emit reloadActions();
}

bool KexiTableView::isReadOnly() const
{
	if (d->readOnly == 1 || d->readOnly == 0)
		return (bool)d->readOnly;
	CHECK_DATA(true);
	return m_data->isReadOnly();
}

void KexiTableView::setInsertingEnabled(bool set)
{
	if (isInsertingEnabled() == set || (m_data && !m_data->isInsertingEnabled() && set))
		return; //not allowed!
	d->insertingEnabled = (set ? 1 : 0);
	d->navBtnNew->setEnabled(set);
	d->pVerticalHeader->showInsertRow(set);
	if (set)
		setReadOnly(false);
	update();
	emit reloadActions();
}

bool KexiTableView::isInsertingEnabled() const
{
	if (d->insertingEnabled == 1 || d->insertingEnabled == 0)
		return (bool)d->insertingEnabled;
	CHECK_DATA(true);
	return m_data->isInsertingEnabled();
}

bool KexiTableView::isEmptyRowInsertingEnabled() const
{
	return d->emptyRowInsertingEnabled;//js && isInsertingEnabled();
}

void KexiTableView::setEmptyRowInsertingEnabled(bool set)
{
	d->emptyRowInsertingEnabled = set;
	emit reloadActions();
}

bool KexiTableView::isDeleteEnabled() const
{
	return d->deletionPolicy != NoDelete;
}

bool KexiTableView::rowEditing() const
{
	return d->rowEditing;
}

bool KexiTableView::navigatorEnabled() const
{
	return d->navigatorEnabled;
}
	
void KexiTableView::setNavigatorEnabled(bool set)
{
	if (d->navigatorEnabled==set)
		return;
	d->navigatorEnabled = set;
	if(!set)
		d->navPanel->hide();
	else
		d->navPanel->show();
}

bool KexiTableView::contextMenuEnabled() const
{
	return d->contextMenuEnabled;
}

void KexiTableView::setContextMenuEnabled(bool set)
{
	d->contextMenuEnabled = set;
}

void KexiTableView::setHBarGeometry( QScrollBar & hbar, int x, int y, int w, int h )
{
/*todo*/
	kdDebug(44021)<<"KexiTableView::setHBarGeometry"<<endl;
	if (navigatorEnabled()) {
		hbar.setGeometry( x + d->navPanel->width(), y, w - d->navPanel->width(), h );
	}
	else
	{
		hbar.setGeometry( x , y, w, h );
	}
}

void KexiTableView::setFilteringEnabled(bool set)
{
	d->filteringEnabled = set;
}

bool KexiTableView::filteringEnabled() const
{
	return d->filteringEnabled;
}

void KexiTableView::setSpreadSheetMode()
{
	setNavigatorEnabled( false );
	setSortingEnabled( false );
	setInsertingEnabled( false );
	setAcceptsRowEditAfterCellAccepting( true );
	setFilteringEnabled( false );
	setEmptyRowInsertingEnabled( true );
}

bool KexiTableView::scrollbarToolTipsEnabled() const
{
	return d->scrollbarToolTipsEnabled;
}

void KexiTableView::setScrollbarToolTipsEnabled(bool set)
{
	d->scrollbarToolTipsEnabled=set;
}

int KexiTableView::validRowNumber(const QString& text)
{
	bool ok=true;
	int r = text.toInt(&ok);
	if (!ok || r<1)
		r = 1;
	else if (r > (rows()+(isInsertingEnabled()?1:0)))
		r = rows()+(isInsertingEnabled()?1:0);
	return r-1;
}

//! for navigator
void KexiTableView::navRowNumber_ReturnPressed(const QString& text)
{
	setFocus();
	selectRow( validRowNumber(text) );
	d->skipKeyPress=true;
}

void KexiTableView::navRowNumber_lostFocus()
{
	int r = validRowNumber(d->navRowNumber->text());
	setNavRowNumber(r);
//	d->navRowNumber->setText( QString::number( r+1 ) );
	selectRow( r );
}

void KexiTableView::updateRowCountInfo()
{
	d->navRowNumberValidator->setRange(1,rows()+(isInsertingEnabled()?1:0));
	setNavRowCount(rows());
//	d->navRowCount->setText(QString::number(rows()));
}

void KexiTableView::navBtnLastClicked()
{
	setFocus();
	selectRow(rows()>0 ? (rows()-1) : 0);
}

void KexiTableView::navBtnPrevClicked()
{
	setFocus();
	selectPrevRow();
}

void KexiTableView::navBtnNextClicked()
{
	setFocus();
	selectNextRow();
}

void KexiTableView::navBtnFirstClicked()
{
	setFocus();
	selectFirstRow();
}

void KexiTableView::navBtnNewClicked()
{
	if (!isInsertingEnabled())
		return;
	setFocus();
	selectRow(rows());
	startEditCurrentCell();
}

bool KexiTableView::eventFilter( QObject *o, QEvent *e )
{
	//don't allow to stole key my events by others:
	if (e->spontaneous() && (e->type()==QEvent::KeyPress /*|| e->type()==QEvent::AccelOverride*/)) {
		QKeyEvent *ke = static_cast<QKeyEvent*>(e);
		int k = ke->key();
		//cell editor's events:
		KexiTableEdit *edit = editor( d->curCol );
		if (edit && edit->handleKeyPress(ke, d->pEditor==edit)) {
			//try to handle the event @ editor's level
			ke->accept();
			return true;
		}
		else if (d->pEditor && (o==d->pEditor || o==d->pEditor->view())) {
			if (   (k==Key_Tab && (k==NoButton || k==ShiftButton))
				|| (k==Key_Enter || k==Key_Return || k==Key_Up || k==Key_Down) 
				|| (k==Key_Left && d->pEditor->cursorAtStart())
				|| (k==Key_Right && d->pEditor->cursorAtEnd())
			   ) {
				keyPressEvent(ke);
				if (ke->isAccepted())
					return true;
			}
		}
		else if (e->type()==QEvent::KeyPress && (o==this /*|| o==viewport()*/)) {
			keyPressEvent(ke);
			if (ke->isAccepted())
				return true;
		}
	}
/*	else if (e->type()==QEvent::FocusOut && o->inherits("QWidget")) {
		//hp==true if currently focused widget is a child of this table view
		const bool hp = Kexi::hasParent( static_cast<QWidget*>(o), focusWidget());
		if (!hp && Kexi::hasParent( this, static_cast<QWidget*>(o))) {
			//accept row editing if focus is moved to foreign widget 
			//(not a child, like eg. editor) from one of our table view's children
			//or from table view itself
			if (!acceptRowEdit()) {
				static_cast<QWidget*>(o)->setFocus();
				return true;
			}
		}
	}*/
	return QScrollView::eventFilter(o,e);
}

void KexiTableView::vScrollBarValueChanged(int v)
{
	if (!d->vScrollBarValueChanged_enabled)
		return;
	kdDebug(44021) << "VCHANGED: " << v << " / " << horizontalScrollBar()->maxValue() <<  endl;
	
//	updateContents();
	d->pVerticalHeader->update(); //<-- dirty but needed

	if (d->scrollbarToolTipsEnabled) {
		QRect r = verticalScrollBar()->sliderRect();
		int row = rowAt(contentsY())+1;
		if (row<=0) {
			d->scrollBarTipTimer.stop();
			d->scrollBarTip->hide();
			return;
		}
		d->scrollBarTip->setText( i18n("Row: ") + QString::number(row) );
		d->scrollBarTip->adjustSize();
		d->scrollBarTip->move( 
		 mapToGlobal( r.topLeft() + verticalScrollBar()->pos() ) + QPoint( - d->scrollBarTip->width()-5, r.height()/2 - d->scrollBarTip->height()/2) );
		if (verticalScrollBar()->draggingSlider()) {
			kdDebug(44021) << "  draggingSlider()  " << endl;
			d->scrollBarTipTimer.stop();
			d->scrollBarTip->show();
			d->scrollBarTip->raise();
		}
		else {
			d->scrollBarTipTimerCnt++;
			if (d->scrollBarTipTimerCnt>4) {
				d->scrollBarTipTimerCnt=0;
				d->scrollBarTip->show();
				d->scrollBarTip->raise();
				d->scrollBarTipTimer.start(500, true);
			}
		}
	}
	//update bottom view region
	if (d->navPanel && (contentsHeight() - contentsY() - clipper()->height()) <= QMAX(d->rowHeight,d->navPanel->height())) {
		slotUpdate();
		triggerUpdate();
	}
}

void KexiTableView::vScrollBarSliderReleased()
{
	kdDebug(44021) << "vScrollBarSliderReleased()" << endl;
	d->scrollBarTip->hide();
}

void KexiTableView::scrollBarTipTimeout()
{
	if (d->scrollBarTip->isVisible()) {
		kdDebug(44021) << "TIMEOUT! - hide" << endl;
		if (d->scrollBarTipTimerCnt>0) {
			d->scrollBarTipTimerCnt=0;
			d->scrollBarTipTimer.start(500, true);
			return;
		}
		d->scrollBarTip->hide();
	}
	d->scrollBarTipTimerCnt=0;
}

void KexiTableView::slotTopHeaderSizeChange( int section, int oldSize, int newSize )
{
	editorShowFocus( d->curRow, d->curCol );
}

QVariant* KexiTableView::bufferedValueAt(int col)
{
	if (d->rowEditing && m_data->rowEditBuffer())
	{
		KexiTableViewColumn* tvcol = m_data->column(col);
		if (tvcol->isDBAware) {
//			QVariant *cv = m_data->rowEditBuffer()->at( *static_cast<KexiDBTableViewColumn*>(tvcol)->field );
			QVariant *cv = m_data->rowEditBuffer()->at( *tvcol->field );
			if (cv)
				return cv;

			return &d->pCurrentItem->at(col);
		}
		QVariant *cv = m_data->rowEditBuffer()->at( tvcol->field->name() );
		if (cv)
			return cv;
	}
	return &d->pCurrentItem->at(col);
}

#include "kexitableview.moc"

