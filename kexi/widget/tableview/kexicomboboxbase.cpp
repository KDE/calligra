/* This file is part of the KDE project
   Copyright (C) 2002 Peter Simonsson <psn@linux.se>
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

#include "kexicomboboxbase.h"
#include <widget/utils/kexicomboboxdropdownbutton.h>
#include "kexicomboboxpopup.h"
#include "kexitableview.h"
#include "kexitableitem.h"
#include "kexi.h"

#include <klineedit.h>

//! @internal
/*
class KexiComboBoxBase::Private
{
public:
	Private()
	 : popup(0)
	{
//moved		mouseBtnPressedWhenPopupVisible = false;
		currentEditorWidth = 0;
		slotLineEditTextChanged_enabled = true;
	}
//moved	KPushButton *button;
//	KexiComboBoxPopup *popup;
	int parentRightMargin;
	int currentEditorWidth;
	QSize totalSize;
	QString userEnteredText; //!< text entered by hand (by user)
//moved	bool mouseBtnPressedWhenPopupVisible : 1;
	bool slotLineEditTextChanged_enabled : 1;
//	bool userEnteredTextChanged : 1;
};*/

//======================================================

KexiComboBoxBase::KexiComboBoxBase()
{
//moved	connect(m_lineedit, SIGNAL(textChanged(const QString&)), this, SLOT(slotLineEditTextChanged(const QString&)));

	m_popup = 0;
	m_internalEditorValueChanged = false; //user has text or other value inside editor
	m_slotLineEditTextChanged_enabled = true;
	m_mouseBtnPressedWhenPopupVisible = false;
	m_insideCreatePopup = false;

//moved	d->parentRightMargin = m_rightMargin;
}

KexiComboBoxBase::~KexiComboBoxBase()
{
}

KexiDB::LookupFieldSchema *KexiComboBoxBase::lookupFieldSchema() const
{
	if (field() && field()->table())
		return field()->table()->lookupFieldSchema( *field() );
	return 0;
}

int KexiComboBoxBase::rowToHighlightForLookupTable() const
{
	if (!m_popup)
		return 0;//err
	KexiDB::LookupFieldSchema *lookupFieldSchema = this->lookupFieldSchema();
	if (!lookupFieldSchema)
		return -1;
	if (lookupFieldSchema->boundColumn()==-1)
		return -1; //err
	bool ok;
	const int rowUid = origValue().toInt();
//todo			tvData->findByUID( row );
//! @todo for now we're assuming the id is INTEGER
	KexiTableViewData *tvData = m_popup->tableView()->data();
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

void KexiComboBoxBase::setValueInternal(const QVariant& add_, bool removeOld)
{
	Q_UNUSED(removeOld);
	m_mouseBtnPressedWhenPopupVisible = false;
	QString add(add_.toString());
	if (add.isEmpty()) {
		KexiTableViewData *relData = column() ? column()->relatedData() : 0;
		QVariant valueToSet;
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
			if (m_popup) {
					bool ok;
					const int rowUid = origValue().toInt();
		//todo			tvData->findByUID( row );
		//! @todo for now we're assuming the id is INTEGER
					KexiTableViewData *tvData = m_popup->tableView()->data();
					const int boundColumn = lookupFieldSchema->boundColumn();
					KexiTableViewData::Iterator it(tvData->iterator());
					int row=0;
					for (;it.current();++it, row++)
					{
						if (it.current()->at(boundColumn).toInt(&ok) == rowUid && ok || !ok)
							break;
					}
					if (!ok || !it.current()) {//item not found: highlight 1st row, if available
						row = 0;
					}
					else {
						if (lookupFieldSchema->visibleColumn()!=-1 
							&& (int)it.current()->size() >= lookupFieldSchema->visibleColumn())
						{
							valueToSet = it.current()->at( lookupFieldSchema->visibleColumn() );
						}
					}
					m_popup->tableView()->setHighlightedRow(row);
			}
		}
		else if (relData) {
			//use 'related table data' model
//			KexiTableItem *it = m_popup ? m_popup->tableView()->selectedItem() : 0;
//			if (it)
			valueToSet = valueForString(origValue().toString(), &rowToHighlight, 0, 1);
////			stringValue = m_origValue.toString();
//				stringValue = it->at(1).toString();
		}
		else {
			//use 'enum hints' model
			const int row = origValue().toInt();
			valueToSet = field()->enumHint(row).trimmed();
		}
		setValueOrTextInInternalEditor( valueToSet );
		/*impl.*/moveCursorToEndInInternalEditor();
		/*impl.*/selectAllInInternalEditor();
		
		if (m_popup) {
			if (origValue().isNull()) {
				m_popup->tableView()->clearSelection();
				m_popup->tableView()->setHighlightedRow(0);
			} else {
				if (relData) {
					if (rowToHighlight!=-1)
						m_popup->tableView()->setHighlightedRow(rowToHighlight);
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
						m_popup->tableView()->setHighlightedRow(row);
					}
					else {
						//item not found: highlight 1st row, if available
						if (!relData->isEmpty())
							m_popup->tableView()->setHighlightedRow(0);
					}*/
					//TODO: select proper row using origValue key
				}
				else {
					//m_popup->tableView()->selectRow(origValue().toInt());
					m_popup->tableView()->setHighlightedRow(origValue().toInt());
				}
			}
		}
	}
	else {
		//todo: autocompl.?
		if (m_popup)
			m_popup->tableView()->clearSelection();
		/*impl.*/setValueInInternalEditor(add); //not setLineEditText(), because 'add' is entered by user!
		//setLineEditText( add );
		/*impl.*/moveCursorToEndInInternalEditor();
	}
}

KexiTableItem* KexiComboBoxBase::selectItemForStringInLookupTable(const QString& str)
{
	KexiDB::LookupFieldSchema *lookupFieldSchema = this->lookupFieldSchema();
	if (!m_popup || !lookupFieldSchema)
		return 0; //safety
//-not effective for large sets: please cache it!
//.trimmed() is not generic!

	const QString txt( str.trimmed().lower() );
	KexiTableViewData *lookupData = m_popup->tableView()->data();
//	const int boundColumn = lookupFieldSchema->boundColumn();
	const int visibleColumn = lookupFieldSchema->visibleColumn();
	KexiTableViewData::Iterator it(lookupData->iterator());
	int row;
	for (row = 0;it.current();++it, row++) {
		if (it.current()->at(visibleColumn).toString().trimmed().lower()==txt)
			break;
	}
	if (it.current()) {
		m_popup->tableView()->selectRow(row);
	}
	else {
		m_popup->tableView()->clearSelection();
	}
	return it.current();
}

QString KexiComboBoxBase::valueForString(const QString& str, int* row, 
	uint lookInColumn, uint returnFromColumn, bool allowNulls)
{
	KexiTableViewData *relData = column() ? column()->relatedData() : 0;
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

	kexiwarn << "KexiComboBoxBase::valueForString(): no related row found, ID will be painted!" << endl;
	if (allowNulls)
		return QString::null;
	return str; //for sanity but it's weird to show id to the user
}

/* moved
void KexiComboBoxBase::showFocus( const QRect& r, bool readOnly )
{
//	d->button->move( pos().x()+ width(), pos().y() );
	updateFocus( r );
	d->button->setEnabled(!readOnly);
	if (readOnly)
		d->button->hide();
	else
		d->button->show();
}*/

/* moved
void KexiComboBoxBase::resize(int w, int h)
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
}*/

/* moved
// internal
void KexiComboBoxBase::updateFocus( const QRect& r )
{
	if (!column()->isReadOnly()) {
		if (d->button->width() > r.width())
			moveChild(d->button, r.right() + 1, r.top());
		else
			moveChild(d->button, r.right() - d->button->width(), r.top() );
	}
}*/

/* moved
void KexiComboBoxBase::hideFocus()
{
	d->button->hide();
}*/

QVariant KexiComboBoxBase::value()
{
//	ok = true;
	KexiTableViewData *relData = column() ? column()->relatedData() : 0;
	KexiDB::LookupFieldSchema *lookupFieldSchema = this->lookupFieldSchema();
	if (relData) {
		if (m_internalEditorValueChanged) {
			//we've user-entered text: look for id
//TODO: make error if matching text not found?
			int rowToHighlight;
			return valueForString(m_userEnteredText, &rowToHighlight, 1, 0, true/*allowNulls*/);
		}
		else {
			//use 'related table data' model
			KexiTableItem *it = m_popup->tableView()->selectedItem();
			return it ? it->at(0) : origValue();//QVariant();
		}
	}
	else if (lookupFieldSchema)
	{
		if (lookupFieldSchema->boundColumn()==-1)
			return origValue();
		KexiTableItem *it = m_popup ? m_popup->tableView()->selectedItem() : 0;
		if (!it && m_internalEditorValueChanged && !m_userEnteredText.isEmpty()) { //
			//try to select a row using the user-entered text
			if (!m_popup)
				createPopup(false);
			it = selectItemForStringInLookupTable( m_userEnteredText );
		}
		return it ? it->at( lookupFieldSchema->boundColumn() ) : QVariant();
	}
	else if (m_popup) {
		//use 'enum hints' model
		const int row = m_popup->tableView()->currentRow();
		if (row>=0)
			return QVariant( row );
//		else
//			return origValue(); //QVariant();
	}

	if (valueFromInternalEditor().toString().isEmpty())
		return QVariant();
/*! \todo don't return just 1st row, but use autocompletion feature
      and: show message box if entered text does not match! */
//	return 0; //1st row
	return origValue(); //unchanged
}

QVariant KexiComboBoxBase::visibleValueForLookupField()
{
	KexiDB::LookupFieldSchema *lookupFieldSchema = this->lookupFieldSchema();
	if (!m_popup || !lookupFieldSchema)
		return QVariant();
	KexiTableItem *it = m_popup->tableView()->selectedItem();
	return it ? it->at( lookupFieldSchema->visibleColumn() ) : QVariant();
}

void KexiComboBoxBase::clear()
{
	if (m_popup)
		m_popup->hide();
	slotLineEditTextChanged(QString::null);
}

tristate KexiComboBoxBase::valueChangedInternal()
{
	//avoid comparing values:
	KexiTableViewData *relData = column() ? column()->relatedData() : 0;
	KexiDB::LookupFieldSchema *lookupFieldSchema = this->lookupFieldSchema();
	if (relData || lookupFieldSchema) {
		if (m_internalEditorValueChanged)
			return true;

		//use 'related table data' model
		KexiTableItem *it = m_popup ? m_popup->tableView()->selectedItem() : 0;
		if (!it)
			return false;
	}
	else {
		//use 'enum hints' model
		const int row = m_popup ? m_popup->tableView()->currentRow() : -1;
		if (row<0 && !m_internalEditorValueChanged/*true if text box is cleared*/)
			return false;
	}

	return cancelled;
}

bool KexiComboBoxBase::valueIsNull()
{
//	bool ok;
	QVariant v( value() );
	return v.isNull();
//	return !ok || v.isNull();
}

bool KexiComboBoxBase::valueIsEmpty()
{
	return valueIsNull();
}

/* moved
void KexiComboBoxBase::paintFocusBorders( QPainter *p, QVariant &, int x, int y, int w, int h )
{
	d->currentEditorWidth = w;
	if (!column()->isReadOnly()) {
		if (w > d->button->width())
			w -= d->button->width();
	}
	p->drawRect(x, y, w, h);
}*/

/* moved
void KexiComboBoxBase::setupContents( QPainter *p, bool focused, const QVariant& val, 
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
}*/

/* moved
void KexiComboBoxBase::slotButtonClicked()
{
	// this method is sometimes called by hand: 
	// do not allow to simulate clicks when the button is disabled
	if (column()->isReadOnly() || !d->button->isEnabled())
		return;

	if (d->mouseBtnPressedWhenPopupVisible) {
		d->mouseBtnPressedWhenPopupVisible = false;
		d->button->setOn(false);
		return;
	}
	kDebug() << "KexiComboBoxBase::slotButtonClicked()" << endl;
	if (!m_popup || !m_popup->isVisible()) {
		kDebug() << "SHOW POPUP" << endl;
		showPopup();
		d->button->setOn(true);
	}
}*/

void KexiComboBoxBase::showPopup()
{
	createPopup(true);
}

void KexiComboBoxBase::createPopup(bool show)
{
	if (!field())
		return;
	m_insideCreatePopup = true;
	QWidget* thisWidget = dynamic_cast<QWidget*>(this);
	QWidget *widgetToFocus = internalEditor() ? internalEditor() : thisWidget;
	if (!m_popup) {
		m_popup = column() ? new KexiComboBoxPopup(thisWidget, *column()) 
			: new KexiComboBoxPopup(thisWidget, *field());
		QObject::connect(m_popup, SIGNAL(rowAccepted(KexiTableItem*,int)), 
			thisWidget, SLOT(slotRowAccepted(KexiTableItem*,int)));
//moved		connect(m_popup, SIGNAL(cancelled()), this, SIGNAL(cancelRequested()));
//moved		connect(m_popup, SIGNAL(hidden()), this, SLOT(slotPopupHidden()));
		QObject::connect(m_popup->tableView(), SIGNAL(itemSelected(KexiTableItem*)),
			thisWidget, SLOT(slotItemSelected(KexiTableItem*)));

		m_popup->setFocusProxy( widgetToFocus );	
		m_popup->tableView()->setFocusProxy( widgetToFocus );
		m_popup->installEventFilter(thisWidget);

//		m_popup->tableView()->selectRow(origValue().toInt());//update selection
		if (origValue().isNull())
			m_popup->tableView()->clearSelection();
		else
			m_popup->tableView()->setHighlightedRow( 0 );
//			m_popup->tableView()->selectRow(origValue().toInt());
	}
	if (show && internalEditor() && !internalEditor()->isVisible())
		/*emit*/editRequested();

	QPoint posMappedToGlobal = mapFromParentToGlobal(thisWidget->pos());
	if (posMappedToGlobal != QPoint(-1,-1)) {
//! todo alter the position to fit the popup within screen boundaries
		m_popup->move( posMappedToGlobal + QPoint(0, thisWidget->height()) );
		//to avoid flickering: first resize to 0-height, then show and resize back to prev. height
//		const int h = m_popup->height()
		const int w = popupWidthHint();
		m_popup->resize(w, 0);
		if (show)
			m_popup->show();
		m_popup->updateSize(w);
//		m_popup->resize(w, h);
		int rowToHighlight = -1;
		KexiDB::LookupFieldSchema *lookupFieldSchema = this->lookupFieldSchema();
		KexiTableViewData *relData = column() ? column()->relatedData() : 0;
		if (lookupFieldSchema) {
			rowToHighlight = rowToHighlightForLookupTable();
		}
		else if (relData) {
			(void)valueForString(origValue().toString(), &rowToHighlight, 0, 1);
		}
		else //enum hint
			rowToHighlight = origValue().toInt();

		if (rowToHighlight!=-1) {
			m_popup->tableView()->selectRow( rowToHighlight );
//js ok?			rowToHighlight = -1; //don't highlight: we've a selection
//moved up			rowToHighlight = origValue().toInt();
		}
		else {
			rowToHighlight = qMax( m_popup->tableView()->highlightedRow(), 0);
		}
		m_popup->tableView()->setHighlightedRow( rowToHighlight );
		if (rowToHighlight < m_popup->tableView()->rowsPerPage())
			m_popup->tableView()->ensureCellVisible( 0, -1 );
	}

	widgetToFocus->setFocus();
	m_insideCreatePopup = false;
}

/* moved
void KexiComboBoxBase::slotPopupHidden()
{
	d->button->setOn(false);
}*/

void KexiComboBoxBase::hide()
{
//moved	KexiInputTableEdit::hide();
	if (m_popup)
		m_popup->hide();
//moved	d->button->setOn(false);
}

/* moved
void KexiComboBoxBase::show()
{
	KexiInputTableEdit::show();
	if (!column()->isReadOnly())
		d->button->show();
}*/

void KexiComboBoxBase::slotRowAccepted(KexiTableItem * item, int row)
{
	Q_UNUSED(row);
	//update our value
	//..nothing to do?
	updateButton();
	slotItemSelected(item);
	/*emit*/acceptRequested();
}

/* moved
bool KexiComboBoxBase::handleKeyPress( QKeyEvent *ke, bool editorActive )
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
		if (enterPressed && d->userEnteredTextChanged) {
			createPopup(false);
			selectItemForStringInLookupTable( d->userEnteredText );
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
}*/

void KexiComboBoxBase::slotItemSelected(KexiTableItem*)
{
	QVariant valueToSet;
	KexiTableViewData *relData = column() ? column()->relatedData() : 0;
	KexiDB::LookupFieldSchema *lookupFieldSchema = this->lookupFieldSchema();
	if (relData) {
		//use 'related table data' model
//		KexiTableItem *it = m_popup->tableView()->selectedItem();
		KexiTableItem *item = m_popup->tableView()->selectedItem();
		if (item)
			valueToSet = item->at(1);
	}
	else if (lookupFieldSchema) {
		KexiTableItem *item = m_popup->tableView()->selectedItem();
		if (item && lookupFieldSchema->visibleColumn()!=-1 && (int)item->size() >= lookupFieldSchema->visibleColumn()) {
			valueToSet = item->at( lookupFieldSchema->visibleColumn() );
		}
	}
	else {
		//use 'enum hints' model
		valueToSet = field()->enumHint( m_popup->tableView()->currentRow() );
		if (valueToSet.toString().isEmpty() && !m_insideCreatePopup) {
			clear();
			QWidget* thisWidget = dynamic_cast<QWidget*>(this);
			thisWidget->parentWidget()->setFocus();
			return;
		}
	}
	setValueOrTextInInternalEditor( valueToSet );
	moveCursorToEndInInternalEditor();
	selectAllInInternalEditor();
}

void KexiComboBoxBase::slotLineEditTextChanged(const QString &newtext)
{
	if (!m_slotLineEditTextChanged_enabled)
		return;
	m_userEnteredText = newtext;
	m_internalEditorValueChanged = true;
	if (newtext.isEmpty()) {
		if (m_popup) {
			m_popup->tableView()->clearSelection();
		}
		return;
	}
	//todo: select matching row for given prefix
}

/* moved
void KexiComboBoxBase::updateTextForHighlightedRow()
{
	KexiTableViewData *relData = column()->relatedData();
	if (relData) {
		//use 'related table data' model
		const KexiTableItem *item = m_popup ? m_popup->tableView()->highlightedItem() : 0;
		if (item) {
			m_slotLineEditTextChanged_enabled = false; //temp. disable slot
			setValueInInternalEditor( item->at(1) );
			m_slotLineEditTextChanged_enabled = true;
			moveCursorToEndInInternalEditor(); //m_lineedit->setCursorPosition(m_lineedit->text().length());
			selectAllInInternalEditor(); //m_lineedit->selectAll();
		}
	}
}*/

/* moved
int KexiComboBoxBase::widthForValue( QVariant &val, const QFontMetrics &fm )
{
	QValueVector<QString> hints = field()->enumHints();
	bool ok;
	int idx = val.toInt(&ok);
	if (!ok || idx < 0 || idx > int(hints.size()-1))
		return KEXITV_MINIMUM_COLUMN_WIDTH;
	QString txt = hints.at( idx, &ok );
	if (!ok)
		return KEXITV_MINIMUM_COLUMN_WIDTH;
	return fm.width( txt );
}*/

/* moved
bool KexiComboBoxBase::eventFilter( QObject *o, QEvent *e )
{
	if (!column()->isReadOnly() && e->type()==QEvent::MouseButtonPress) {
		QPoint gp = static_cast<QMouseEvent*>(e)->globalPos() 
			+ QPoint(m_scrollView->childX(d->button), m_scrollView->childY(d->button));
		QRect r(d->button->mapToGlobal(d->button->geometry().topLeft()), 
			d->button->mapToGlobal(d->button->geometry().bottomRight()));
		if (o==m_popup && m_popup->isVisible() && r.contains( gp )) {
			d->mouseBtnPressedWhenPopupVisible = true;
		}
	}
	return false;
}*/

/* moved
QSize KexiComboBoxBase::totalSize() const
{
	return d->totalSize;
}*/

void KexiComboBoxBase::setValueOrTextInInternalEditor(const QVariant& value)
{
	setValueInInternalEditor( value );
	//this text is not entered by hand:
	m_userEnteredText = QString::null;
	m_internalEditorValueChanged = false;
}

