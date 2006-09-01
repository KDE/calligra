/* This file is part of the KDE project
   Copyright (C) 2002   Peter Simonsson <psn@linux.se>
   Copyright (C) 2003-2006 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <qlayout.h>
#include <qstyle.h>
#include <qwindowsstyle.h>
#include <qpainter.h>
//Added by qt3to4:
#include <QKeyEvent>
#include <QEvent>
#include <QMouseEvent>

#include "kexicomboboxtableedit.h"
#include <widget/utils/kexicomboboxdropdownbutton.h>
#include "kexicomboboxpopup.h"
#include "kexitableview.h"
#include "kexitableitem.h"
#include "kexi.h"

#include <klineedit.h>

//! @internal
class KexiComboBoxTableEdit::Private
{
public:
	Private()
//moved	 : popup(0)
	{
//moved		mouseBtnPressedWhenPopupVisible = false;
		currentEditorWidth = 0;
//moved		slotLineEditTextChanged_enabled = true;
//moved as m_internalEditorValueChanged		userEnteredTextChanged = false;
	}
	KPushButton *button;
//moved	KexiComboBoxPopup *popup;
	int parentRightMargin;
	int currentEditorWidth;
	QSize totalSize;
//moved	as m_internalEditorValueChanged QString userEnteredText; //!< text entered by hand (by user)
//moved	bool mouseBtnPressedWhenPopupVisible : 1;
//moved	bool slotLineEditTextChanged_enabled : 1;
	bool userEnteredTextChanged : 1;
};

//======================================================

KexiComboBoxTableEdit::KexiComboBoxTableEdit(KexiTableViewColumn &column, Q3ScrollView *parent)
 : KexiInputTableEdit(column, parent)
 , KexiComboBoxBase()
 , d(new Private())
{
	setName("KexiComboBoxTableEdit");
//	QHBoxLayout* layout = new QHBoxLayout(this);
	d->button = new KexiComboBoxDropDownButton( parent->viewport() );
	d->button->hide();
	d->button->setFocusPolicy( NoFocus );
	connect(d->button, SIGNAL(clicked()), this, SLOT(slotButtonClicked()));

	connect(m_lineedit, SIGNAL(textChanged(const QString&)), this, SLOT(slotLineEditTextChanged(const QString&)));

	d->parentRightMargin = m_rightMargin;
//	m_lineedit = new KLineEdit(this, "lineedit");
//	m_lineedit->setFrame(false);
//	m_lineedit->setFrameStyle( QFrame::Plain | QFrame::Box );
//	m_lineedit->setLineWidth( 1 );
//	if (f.isNumericType()) {
//		m_lineedit->setAlignment(AlignRight);
//	}
//	setView( m_lineedit );

//	layout->addWidget(m_view);
//	m_combo->setEditable( true );
//	m_combo->clear();
//	m_combo->insertStringList(f.enumHints());
//	QStringList::ConstIterator it, end( f.enumHints().constEnd() );
//	for ( it = f.enumHints().constBegin(); it != end; ++it) {
//		if(!hints.at(i).isEmpty())
//			m_combo->insertItem(hints.at(i));
//	}

//js:	TODO
//js	static_cast<KComboBox*>(m_view)->insertStringList(list);
//js	static_cast<KComboBox*>(m_view)->setCurrentItem(static_cast<int>(t));
}

KexiComboBoxTableEdit::~KexiComboBoxTableEdit()
{
	delete d;
}

#if 0 // in KexiComboBoxBase
KexiDB::LookupFieldSchema *KexiComboBoxTableEdit::lookupFieldSchema() const
{
	if (column()->field() && column()->field()->table())
		return column()->field()->table()->lookupFieldSchema( *column()->field() );
	return 0;
}

int KexiComboBoxTableEdit::rowToHighlightForLookupTable() const
{
	if (!d->popup)
		return 0;//err
	KexiDB::LookupFieldSchema *lookupFieldSchema = this->lookupFieldSchema();
	if (!lookupFieldSchema)
		return -1;
	if (lookupFieldSchema->boundColumn()==-1)
		return -1; //err
	bool ok;
	const int rowUid = m_origValue.toInt();
//todo			tvData->findByUID( row );
//! @todo for now we're assuming the id is INTEGER
	KexiTableViewData *tvData = d->popup->tableView()->data();
	const int boundColumn = lookupFieldSchema->boundColumn();
	KexiTableViewData::Iterator it(tvData->iterator());
	int row=0;
	for (;it.current();++it, row++)
	{
		if (it.current()->at(boundColumn).toInt(&ok) == rowUid && ok || !ok)
			break;
	}
	if (!ok || !it.current()) //item not found: highlight 1st row, if available
		return -1;
	return row;
}

void KexiComboBoxTableEdit::setValueInternal(const QVariant& add_, bool removeOld)
{
	Q_UNUSED(removeOld);
	m_mouseBtnPressedWhenPopupVisible = false;
//	m_combo->setCurrentItem(m_origValue.toInt() - 1);
	QString add(add_.toString());
	if (add.isEmpty()) {
		KexiTableViewData *relData = column()->relatedData();
		QString stringValue;
		int rowToHighlight = -1;
		KexiDB::LookupFieldSchema *lookupFieldSchema = this->lookupFieldSchema();
		if (lookupFieldSchema) {
			//use 'lookup field' model
//! @todo support more RowSourceType's, not only table
//			KexiDB::TableSchema *lookupTable 
	//			= column()->field()->table()->connection()->tableSchema( lookupFieldSchema->rowSource() );
			if (lookupFieldSchema->boundColumn()==-1)
//! @todo errmsg
				return;
			if (d->popup) {
					bool ok;
					const int rowUid = m_origValue.toInt();
		//todo			tvData->findByUID( row );
		//! @todo for now we're assuming the id is INTEGER
					KexiTableViewData *tvData = d->popup->tableView()->data();
					const int boundColumn = lookupFieldSchema->boundColumn();
					KexiTableViewData::Iterator it(tvData->iterator());
					int row=0;
					for (;it.current();++it, row++)
					{
						if (it.current()->at(boundColumn).toInt(&ok) == rowUid && ok || !ok)
							break;
					}
					if (!ok || !it.current()) //item not found: highlight 1st row, if available
						row = 0;
					else {
						if (lookupFieldSchema->visibleColumn()!=-1 && (int)it.current()->size() >= lookupFieldSchema->visibleColumn()) {
							stringValue = it.current()->at( lookupFieldSchema->visibleColumn() ).toString();
						}
					}
					d->popup->tableView()->setHighlightedRow(row);
			}
		}
		else if (relData) {
			//use 'related table data' model
//			KexiTableItem *it = d->popup ? d->popup->tableView()->selectedItem() : 0;
//			if (it)
			stringValue = valueForString(m_origValue.toString(), &rowToHighlight, 0, 1);
////			stringValue = m_origValue.toString();
//				stringValue = it->at(1).toString();
		}
		else {
			//use 'enum hints' model
			const int row = m_origValue.toInt();
			stringValue = field()->enumHint(row).trimmed();
		}
		setLineEditText( stringValue );
		m_lineedit->end(false);
		m_lineedit->selectAll();
		
		if (d->popup) {
			if (m_origValue.isNull()) {
				d->popup->tableView()->clearSelection();
				d->popup->tableView()->setHighlightedRow(0);
			} else {
				if (relData) {
					if (rowToHighlight!=-1)
						d->popup->tableView()->setHighlightedRow(rowToHighlight);
/*
					int row = 0;
					KexiTableViewData::Iterator it(relData->iterator());
					for (;it.current();++it, row++)
					{
						kexidbg << "- '" <<it.current()->at(0).toString() << "' '"<<it.current()->at(1).toString()<<"'"<<endl;
						if (it.current()->at(0).toString()==stringValue)
							break;
					}
					if (it.current()) {
						d->popup->tableView()->setHighlightedRow(row);
					}
					else {
						//item not found: highlight 1st row, if available
						if (!relData->isEmpty())
							d->popup->tableView()->setHighlightedRow(0);
					}*/
					//TODO: select proper row using m_origValue key
				}
				else {
					//d->popup->tableView()->selectRow(m_origValue.toInt());
					d->popup->tableView()->setHighlightedRow(m_origValue.toInt());
				}
			}
		}
	}
	else {
		//todo: autocompl.?
		if (d->popup)
			d->popup->tableView()->clearSelection();
		m_lineedit->setText(add); //not setLineEditText(), because 'add' is entered by user!
		//setLineEditText( add );
		m_lineedit->end(false);
	}
}

KexiTableItem* KexiComboBoxTableEdit::selectItemForStringInLookupTable(const QString& str)
{
	KexiDB::LookupFieldSchema *lookupFieldSchema = this->lookupFieldSchema();
	if (!d->popup || !lookupFieldSchema)
		return 0; //safety
//-not effective for large sets: please cache it!
//.trimmed() is not generic!

	const QString txt( str.trimmed().lower() );
	KexiTableViewData *lookupData = d->popup->tableView()->data();
//	const int boundColumn = lookupFieldSchema->boundColumn();
	const int visibleColumn = lookupFieldSchema->visibleColumn();
	KexiTableViewData::Iterator it(lookupData->iterator());
	int row;
	for (row = 0;it.current();++it, row++) {
		if (it.current()->at(visibleColumn).toString().trimmed().lower()==txt)
			break;
	}
	if (it.current()) {
		d->popup->tableView()->selectRow(row);
	}
	else {
		d->popup->tableView()->clearSelection();
	}
	return it.current();
}

QString KexiComboBoxTableEdit::valueForString(const QString& str, int* row, 
	uint lookInColumn, uint returnFromColumn, bool allowNulls)
{
	KexiTableViewData *relData = column()->relatedData();
	if (!relData)
		return QString::null; //safety
	//use 'related table data' model
//-not effective for large sets: please cache it!
//.trimmed() is not generic!

	const QString txt = str.trimmed().lower();
	KexiTableViewData::Iterator it( relData->iterator() );
	for (*row = 0;it.current();++it, (*row)++) {
		if (it.current()->at(lookInColumn).toString().trimmed().lower()==txt)
			break;
	}
	if (it.current())
		return it.current()->at(returnFromColumn).toString();
//		return it.current()->at(returnFromColumn).toString().trimmed();

	*row = -1;

	if (column()->relatedDataEditable())
		return str; //new value entered and that's allowed

	kexiwarn << "KexiComboBoxTableEdit::valueForString(): no related row found, ID will be painted!" << endl;
	if (allowNulls)
		return QString::null;
	return str; //for sanity but it's weird to show id to the user
}
#endif // in KexiComboBoxBase

void KexiComboBoxTableEdit::showFocus( const QRect& r, bool readOnly )
{
//	d->button->move( pos().x()+ width(), pos().y() );
	updateFocus( r );
	d->button->setEnabled(!readOnly);
	if (readOnly)
		d->button->hide();
	else
		d->button->show();
}

void KexiComboBoxTableEdit::resize(int w, int h)
{
	d->totalSize = QSize(w,h);
	if (!column()->isReadOnly()) {
		d->button->resize( h, h );
		QWidget::resize(w - d->button->width(), h);
	}
	m_rightMargin = d->parentRightMargin + (column()->isReadOnly() ? 0 : d->button->width());
	QRect r( pos().x(), pos().y(), w+1, h+1 );
	r.moveBy(m_scrollView->contentsX(),m_scrollView->contentsY());
	updateFocus( r );
	if (m_popup) {
		m_popup->updateSize();
	}
}

// internal
void KexiComboBoxTableEdit::updateFocus( const QRect& r )
{
	if (!column()->isReadOnly()) {
		if (d->button->width() > r.width())
			moveChild(d->button, r.right() + 1, r.top());
		else
			moveChild(d->button, r.right() - d->button->width(), r.top() );
	}
}

void KexiComboBoxTableEdit::hideFocus()
{
	d->button->hide();
}

/* in KexiComboBoxBase
QVariant KexiComboBoxTableEdit::value()
{
//	ok = true;
	KexiTableViewData *relData = column()->relatedData();
	KexiDB::LookupFieldSchema *lookupFieldSchema = this->lookupFieldSchema();
	if (relData) {
		if (d->userEnteredTextChanged) {
			//we've user-entered text: look for id
//TODO: make error if matching text not found?
			int rowToHighlight;
			return valueForString(d->userEnteredText, &rowToHighlight, 1, 0, true);
		}
		else {
			//use 'related table data' model
			KexiTableItem *it = d->popup->tableView()->selectedItem();
			return it ? it->at(0) : m_origValue;//QVariant();
		}
	}
	else if (lookupFieldSchema)
	{
		if (lookupFieldSchema->boundColumn()==-1)
			return m_origValue;
		KexiTableItem *it = d->popup ? d->popup->tableView()->selectedItem() : 0;
		if (!it && d->userEnteredTextChanged && !d->userEnteredText.isEmpty()) { //
			//try to select a row using the user-entered text
			if (!d->popup)
				createPopup(false);
			it = selectItemForStringInLookupTable( d->userEnteredText );
		}
		return it ? it->at( lookupFieldSchema->boundColumn() ) : QVariant();
	}
	else if (d->popup) {
		//use 'enum hints' model
		const int row = d->popup->tableView()->currentRow();
		if (row>=0)
			return QVariant( row );
//		else
//			return m_origValue; //QVariant();
	}

	if (m_lineedit->text().isEmpty())
		return QVariant(); 
//! \todo don't return just 1st row, but use autocompletion feature
//!      and: show message box if entered text does not match! 
//	return 0; //1st row
	return m_origValue; //unchanged
}*/

QVariant KexiComboBoxTableEdit::visibleValueForLookupField()
{
	KexiDB::LookupFieldSchema *lookupFieldSchema = this->lookupFieldSchema();
	if (!m_popup || !lookupFieldSchema)
		return QVariant();
	KexiTableItem *it = m_popup->tableView()->selectedItem();
	return it ? it->at( lookupFieldSchema->visibleColumn() ) : QVariant();
}

/*bool KexiComboBoxTableEdit::cursorAtStart()
{
	//TODO?
	return false;
}

bool KexiComboBoxTableEdit::cursorAtEnd()
{
	//TODO?
	return false;
}*/

void KexiComboBoxTableEdit::clear()
{
	m_lineedit->clear();
	KexiComboBoxBase::clear();
//in KexiComboBoxBase	slotLineEditTextChanged(QString::null);
}

bool KexiComboBoxTableEdit::valueChanged()
{
	const tristate res = valueChangedInternal();
	if (~res) //no result: just compare values
		return KexiInputTableEdit::valueChanged();
	return res == true;
}

/* in KexiComboBoxBase
bool KexiComboBoxTableEdit::valueIsNull()
{
//	bool ok;
	QVariant v( value() );
	return v.isNull();
//	return !ok || v.isNull();
}

bool KexiComboBoxTableEdit::valueIsEmpty()
{
	return valueIsNull();
}*/

void KexiComboBoxTableEdit::paintFocusBorders( QPainter *p, QVariant &, int x, int y, int w, int h )
{
	d->currentEditorWidth = w;
	if (!column()->isReadOnly()) {
		if (w > d->button->width())
			w -= d->button->width();
	}
	p->drawRect(x, y, w, h);
}

void KexiComboBoxTableEdit::setupContents( QPainter *p, bool focused, const QVariant& val, 
	QString &txt, int &align, int &x, int &y_offset, int &w, int &h  )
{
	KexiInputTableEdit::setupContents( p, focused, val, txt, align, x, y_offset, w, h );
	if (!column()->isReadOnly() && focused && (w > d->button->width()))
		w -= (d->button->width() - x);
	if (!val.isNull()) {
		KexiTableViewData *relData = column()->relatedData();
		KexiDB::LookupFieldSchema *lookupFieldSchema = this->lookupFieldSchema();
		if (relData) {
			int rowToHighlight;
			txt = valueForString(val.toString(), &rowToHighlight, 0, 1);
		}
		else if (lookupFieldSchema) {
			if (m_popup) {
				KexiTableItem *it = m_popup->tableView()->selectedItem();
				if (it && lookupFieldSchema->visibleColumn()!=-1 && (int)it->size() >= lookupFieldSchema->visibleColumn())
					txt = it->at( lookupFieldSchema->visibleColumn() ).toString();
			}
		}
		else {
			//use 'enum hints' model
			txt = field()->enumHint( val.toInt() );
		}
	}
}

void KexiComboBoxTableEdit::slotButtonClicked()
{
	// this method is sometimes called by hand: 
	// do not allow to simulate clicks when the button is disabled
	if (column()->isReadOnly() || !d->button->isEnabled())
		return;

	if (m_mouseBtnPressedWhenPopupVisible) {
		m_mouseBtnPressedWhenPopupVisible = false;
		d->button->setOn(false);
		return;
	}
	kDebug() << "KexiComboBoxTableEdit::slotButtonClicked()" << endl;
	if (!m_popup || !m_popup->isVisible()) {
		kDebug() << "SHOW POPUP" << endl;
		showPopup();
		d->button->setOn(true);
	}
}

/* in KexiComboBoxBase
void KexiComboBoxTableEdit::showPopup()
{
	createPopup(true);
}

void KexiComboBoxTableEdit::createPopup(bool show)
{
	if (!d->popup) {
		d->popup = new KexiComboBoxPopup(this, *m_column);
		connect(d->popup, SIGNAL(rowAccepted(KexiTableItem*,int)), 
			this, SLOT(slotRowAccepted(KexiTableItem*,int)));
		connect(d->popup, SIGNAL(cancelled()), this, SIGNAL(cancelRequested()));
		connect(d->popup, SIGNAL(hidden()), this, SLOT(slotPopupHidden()));
		connect(d->popup->tableView(), SIGNAL(itemSelected(KexiTableItem*)),
			this, SLOT(slotItemSelected(KexiTableItem*)));

		d->popup->setFocusProxy( m_lineedit );
		d->popup->tableView()->setFocusProxy( m_lineedit );
		d->popup->installEventFilter(this);

//		d->popup->tableView()->selectRow(m_origValue.toInt());//update selection
		if (m_origValue.isNull())
			d->popup->tableView()->clearSelection();
		else
			d->popup->tableView()->setHighlightedRow( 0 );
//			d->popup->tableView()->selectRow(m_origValue.toInt());
	}
	if (show && !m_lineedit->isVisible())
		emit editRequested();

	KexiTableView *tv = dynamic_cast<KexiTableView*>(m_scrollView);
	if (tv) {
		d->popup->move( tv->viewport()->mapToGlobal(pos()) + QPoint(0,height()) );//+ rect().bottomLeft() ) );
		//to avoid flickering: first resize to 0-height, then show and resize back to prev. height
//		const int h = d->popup->height()
		const int w = qMax(d->popup->width(), d->currentEditorWidth);
		d->popup->resize(w, 0);
		if (show)
			d->popup->show();
		d->popup->updateSize(w);
//		d->popup->resize(w, h);
		int rowToHighlight = -1;
		KexiDB::LookupFieldSchema *lookupFieldSchema = this->lookupFieldSchema();
		KexiTableViewData *relData = column()->relatedData();
		if (lookupFieldSchema) {
			rowToHighlight = rowToHighlightForLookupTable();
		}
		else if (relData) {
			(void)valueForString(m_origValue.toString(), &rowToHighlight, 0, 1);
		}
		else //enum hint
			rowToHighlight = m_origValue.toInt();

		if (rowToHighlight!=-1) {
			d->popup->tableView()->selectRow( rowToHighlight );
//js ok?			rowToHighlight = -1; //don't highlight: we've a selection
//moved up			rowToHighlight = m_origValue.toInt();
		}
		else {
			rowToHighlight = qMax( d->popup->tableView()->highlightedRow(), 0);
		}
		d->popup->tableView()->setHighlightedRow( rowToHighlight );
		if (rowToHighlight < d->popup->tableView()->rowsPerPage())
			d->popup->tableView()->ensureCellVisible( 0, -1 );
	}

	m_lineedit->setFocus();
}*/

void KexiComboBoxTableEdit::slotPopupHidden()
{
	d->button->setOn(false);
}

void KexiComboBoxTableEdit::hide()
{
	KexiInputTableEdit::hide();
	KexiComboBoxBase::hide();
	d->button->setOn(false);
}

void KexiComboBoxTableEdit::show()
{
	KexiInputTableEdit::show();
	if (!column()->isReadOnly())
		d->button->show();
}

/* in KexiComboBoxBase
void KexiComboBoxTableEdit::slotRowAccepted(KexiTableItem * item, int row)
{
	//update our value
	//..nothing to do?
//	emit acceptRequested();
	slotItemSelected(item);
	emit acceptRequested();
}*/

bool KexiComboBoxTableEdit::handleKeyPress( QKeyEvent *ke, bool editorActive )
{
	const int k = ke->key();
	if ((ke->state()==NoButton && k==Qt::Key_F4)
		|| (ke->state()==AltButton && k==Qt::Key_Down))
	{
		//show popup
		slotButtonClicked();
		return true;
	}
	else if (editorActive){
		const bool enterPressed = k==Qt::Key_Enter || k==Qt::Key_Return;
		if (enterPressed && m_internalEditorValueChanged) {
			createPopup(false);
			selectItemForStringInLookupTable( m_userEnteredText );
			return false;
		}

		// The editor may be active but the pull down menu not existant/visible,
		// e.g. when the user has pressed a normal button to activate the editor
		// Don't handle the event here in that case.
		if (!m_popup || (!enterPressed && !m_popup->isVisible())) {
			return false;
		}

		int highlightedOrSelectedRow = m_popup ? m_popup->tableView()->highlightedRow() : -1;
		if (m_popup && highlightedOrSelectedRow < 0)
			highlightedOrSelectedRow = m_popup->tableView()->currentRow();

		switch (k) {
		case Qt::Key_Up:
	//			m_popup->tableView()->selectPrevRow();
				m_popup->tableView()->setHighlightedRow( 
					qMax(highlightedOrSelectedRow-1, 0) );
				updateTextForHighlightedRow();
				return true;
		case Qt::Key_Down:
	//			m_popup->tableView()->selectNextRow();
				m_popup->tableView()->setHighlightedRow( 
					qMin(highlightedOrSelectedRow+1, m_popup->tableView()->rows()-1) );
				updateTextForHighlightedRow();
				return true;
		case Qt::Key_PageUp:
	//			m_popup->tableView()->selectPrevPage();
				m_popup->tableView()->setHighlightedRow( 
					qMax(highlightedOrSelectedRow-m_popup->tableView()->rowsPerPage(), 0) );
				updateTextForHighlightedRow();
				return true;
		case Qt::Key_PageDown:
	//			m_popup->tableView()->selectNextPage();
				m_popup->tableView()->setHighlightedRow( 
					qMin(highlightedOrSelectedRow+m_popup->tableView()->rowsPerPage(), 
					 m_popup->tableView()->rows()-1) );
				updateTextForHighlightedRow();
				return true;
		case Qt::Key_Home:
				m_popup->tableView()->setHighlightedRow( 0 );
				updateTextForHighlightedRow();
				return true;
		case Qt::Key_End:
				m_popup->tableView()->setHighlightedRow( m_popup->tableView()->rows()-1 );
				updateTextForHighlightedRow();
				return true;
		case Qt::Key_Enter:
		case Qt::Key_Return: //accept
				//select row that is highlighted
				if (m_popup->tableView()->highlightedRow()>=0)
					m_popup->tableView()->selectRow( m_popup->tableView()->highlightedRow() );
				//do not return true: allow to process event
		default: ;
		}
	}
	return false;
}

/* in KexiComboBoxBase
void KexiComboBoxTableEdit::slotItemSelected(KexiTableItem*)
{
	QString stringValue;
	KexiTableViewData *relData = column()->relatedData();
	KexiDB::LookupFieldSchema *lookupFieldSchema = this->lookupFieldSchema();
	if (relData) {
		//use 'related table data' model
//		KexiTableItem *it = m_popup->tableView()->selectedItem();
		KexiTableItem *item = m_popup->tableView()->selectedItem();
		if (item)
			stringValue = item->at(1).toString();
	}
	else if (lookupFieldSchema) {
		KexiTableItem *item = m_popup->tableView()->selectedItem();
		if (item && lookupFieldSchema->visibleColumn()!=-1 && (int)item->size() >= lookupFieldSchema->visibleColumn()) {
			stringValue = item->at( lookupFieldSchema->visibleColumn() ).toString();
		}
	}
	else {
		//use 'enum hints' model
		stringValue = field()->enumHint( m_popup->tableView()->currentRow() );
	}
	setLineEditText( stringValue );
	m_lineedit->end(false);
	m_lineedit->selectAll();
}

void KexiComboBoxTableEdit::slotLineEditTextChanged(const QString &newtext)
{
	if (!d->slotLineEditTextChanged_enabled)
		return;
	d->userEnteredText = newtext;
	d->userEnteredTextChanged = true;
	if (newtext.isEmpty()) {
		if (m_popup) {
			m_popup->tableView()->clearSelection();
		}
		return;
	}
	//todo: select matching row for given prefix
}*/

void KexiComboBoxTableEdit::updateTextForHighlightedRow()
{
	KexiTableViewData *relData = column()->relatedData();
	if (relData) {
		//use 'related table data' model
		const KexiTableItem *item = m_popup ? m_popup->tableView()->highlightedItem() : 0;
		if (item) {
			m_slotLineEditTextChanged_enabled = false; //temp. disable slot
			setValueInInternalEditor( item->at(1) );
			m_slotLineEditTextChanged_enabled = true;
			m_lineedit->setCursorPosition(m_lineedit->text().length());
			m_lineedit->selectAll();
		}
	}
}

int KexiComboBoxTableEdit::widthForValue( QVariant &val, const QFontMetrics &fm )
{
	Q3ValueVector<QString> hints = field()->enumHints();
	bool ok;
	int idx = val.toInt(&ok);
	if (!ok || idx < 0 || idx > int(hints.size()-1))
		return KEXITV_MINIMUM_COLUMN_WIDTH;
	QString txt = hints.at( idx, &ok );
	if (!ok)
		return KEXITV_MINIMUM_COLUMN_WIDTH;
	return fm.width( txt );
}

bool KexiComboBoxTableEdit::eventFilter( QObject *o, QEvent *e )
{

	if (!column()->isReadOnly() && e->type()==QEvent::MouseButtonPress) {
		QPoint gp = static_cast<QMouseEvent*>(e)->globalPos() 
			+ QPoint(m_scrollView->childX(d->button), m_scrollView->childY(d->button));
		QRect r(d->button->mapToGlobal(d->button->geometry().topLeft()), 
			d->button->mapToGlobal(d->button->geometry().bottomRight()));
		if (o==m_popup && m_popup->isVisible() && r.contains( gp )) {
			m_mouseBtnPressedWhenPopupVisible = true;
		}
	}
	return false;
}

QSize KexiComboBoxTableEdit::totalSize() const
{
	return d->totalSize;
}

/* moved as KexiComboBoxBase::setValueOrTextInInternalEditor
void KexiComboBoxTableEdit::setLineEditText(const QString& text)
{
	m_lineedit->setText( text );
	//this text is not entered by hand:
	d->userEnteredText = QString::null;
	d->userEnteredTextChanged = false;
}*/

QWidget *KexiComboBoxTableEdit::internalEditor() const
{
	return m_lineedit;
}

void KexiComboBoxTableEdit::moveCursorToEndInInternalEditor()
{
	m_lineedit->end(false);
}

void KexiComboBoxTableEdit::selectAllInInternalEditor()
{
	m_lineedit->selectAll();
}

void KexiComboBoxTableEdit::setValueInInternalEditor(const QVariant& value)
{
	m_lineedit->setText(value.toString());
}

QVariant KexiComboBoxTableEdit::valueFromInternalEditor() const
{
	return m_lineedit->text();
}

QPoint KexiComboBoxTableEdit::mapFromParentToGlobal(const QPoint& pos) const
{
	KexiTableView *tv = dynamic_cast<KexiTableView*>(m_scrollView);
	if (!tv)
		return QPoint(-1,-1);
	return tv->viewport()->mapToGlobal(pos);
}

int KexiComboBoxTableEdit::popupWidthHint() const
{
	return qMax(m_popup->width(), d->currentEditorWidth);
}

KEXI_CELLEDITOR_FACTORY_ITEM_IMPL(KexiComboBoxEditorFactoryItem, KexiComboBoxTableEdit)

#include "kexicomboboxtableedit.moc"
