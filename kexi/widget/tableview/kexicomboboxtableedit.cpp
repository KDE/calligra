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
	{
		currentEditorWidth = 0;
	}
	KPushButton *button;
	int parentRightMargin;
	int currentEditorWidth;
	QSize totalSize;
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

QVariant KexiComboBoxTableEdit::visibleValueForLookupField()
{
	KexiDB::LookupFieldSchema *lookupFieldSchema = this->lookupFieldSchema();
	if (!m_popup || !lookupFieldSchema)
		return QVariant();
	KexiTableItem *it = m_popup->tableView()->selectedItem();
	return it ? it->at( lookupFieldSchema->visibleColumn() ) : QVariant();
}

void KexiComboBoxTableEdit::clear()
{
	m_lineedit->clear();
	KexiComboBoxBase::clear();
}

bool KexiComboBoxTableEdit::valueChanged()
{
	const tristate res = valueChangedInternal();
	if (~res) //no result: just compare values
		return KexiInputTableEdit::valueChanged();
	return res == true;
}

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

void KexiComboBoxTableEdit::slotPopupHidden()
{
	d->button->setOn(false);
}

void KexiComboBoxTableEdit::updateButton()
{
	d->button->setOn(m_popup->isVisible());
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
	if (!column()->isReadOnly()) {
		d->button->show();
	}
}

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
	KexiTableViewData *relData = column() ? column()->relatedData() : 0;
	if (lookupFieldSchema() || relData) {
		// in 'lookupFieldSchema' or  or 'related table data' model 
		// we're assuming val is already the text, not the index
//! @todo ok?
		return qMax(KEXITV_MINIMUM_COLUMN_WIDTH, fm.width(val.toString()));
	}
	//use 'enum hints' model
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
