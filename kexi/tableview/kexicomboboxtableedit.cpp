/* This file is part of the KDE project
   Copyright (C) 2002   Peter Simonsson <psn@linux.se>
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
 */

#include <qlayout.h>
#include <qstyle.h>
#include <qpainter.h>

#include "kexicomboboxtableedit.h"
#include "kexicomboboxpopup.h"
#include "kexitableview.h"
#include "kexi_utils.h"

#include <kpushbutton.h>
#include <klineedit.h>

class KDownArrowPushButton : public KPushButton
{
	public:
		KDownArrowPushButton( QWidget *parent )
		 : KPushButton(parent) {}
	protected:
		/**
		 * Reimplemented from @ref QPushButton. */
		virtual void drawButton(QPainter *p) {
			KPushButton::drawButton(p);
//			QStyle::PrimitiveElement e = QStyle::PE_ArrowDown;
			int flags = QStyle::Style_Enabled;
			if ( isDown() )
				flags |= QStyle::Style_Down;
//			const unsigned int arrowSize = 16;
//			unsigned int x = (width() - arrowSize) / 2;
//			unsigned int y = (height() - arrowSize) / 2;
//			style().drawPrimitive( e, p, QRect( QPoint( x, y ), QSize( arrowSize, arrowSize ) ),
//				colorGroup(), flags );
//	QRect r = style().querySubControlMetrics( QStyle::CC_ComboBox, this,
//					QStyle::SC_ComboBoxArrow );
//	resize(r.size());
	
//	style().drawComplexControl( QStyle::CC_ComboBox, p, this, rect(), colorGroup(),
//				    flags, QStyle::SC_All,
//				     QStyle::SC_ComboBoxArrow);
			style().drawPrimitive( QStyle::PE_ArrowDown, p, rect(), colorGroup(),
				flags);
		}
};

//======================================================

KexiComboBoxTableEdit::KexiComboBoxTableEdit(KexiDB::Field &f, QWidget *parent)
 : KexiInputTableEdit(f, parent,"KexiComboBoxTableEdit")
{
//	QHBoxLayout* layout = new QHBoxLayout(this);
	m_popup = 0;
	m_button = new KDownArrowPushButton( parent );
	m_button->hide();
	m_button->setFocusPolicy( NoFocus );
	connect(m_button, SIGNAL(clicked()), this, SLOT(slotButtonClicked()));

	connect(m_lineedit, SIGNAL(textChanged(const QString&)), this, SLOT(slotLineEditTextChanged(const QString&)));

	m_parentRightMargin = m_rightMargin;
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
//	for (QStringList::iterator it = f.enumHints().begin(); it!=f.enumHints().end(): ++it) {
//		if(!hints.at(i).isEmpty())
//			m_combo->insertItem(hints.at(i));
//	}

//js:	TODO
//js	static_cast<KComboBox*>(m_view)->insertStringList(list);
//js	static_cast<KComboBox*>(m_view)->setCurrentItem(static_cast<int>(t));
}

void KexiComboBoxTableEdit::init(const QString& add)
{
//	m_combo->setCurrentItem(m_origValue.toInt() - 1);
	if (add.isEmpty()) {
		const int row = m_origValue.toInt();
		m_lineedit->setText( m_field->enumHint(row) );
		if (m_popup)
			m_popup->tableView()->selectRow(m_origValue.toInt());
	}
	else {
		m_lineedit->setText( add );
	}
	m_lineedit->end(false);
}

void KexiComboBoxTableEdit::showFocus( const QRect& r )
{
//	m_button->move( pos().x()+ width(), pos().y() );
	updateFocus( r );
	m_button->show();
}

void KexiComboBoxTableEdit::resize(int w, int h)
{
	QWidget::resize(w - m_button->width(), h);
	m_button->resize( h, h );
	m_rightMargin = m_parentRightMargin + m_button->width();
	updateFocus( QRect( pos().x(), pos().y(), w+1, h+1 ) );
	if (m_popup)
		m_popup->resize(w, m_popup->height());
//	m_lineedit->resize(w - m_button->width()-1, h);
}

// internal
void KexiComboBoxTableEdit::updateFocus( const QRect& r )
{
	if (m_button->width() > r.width())
		m_button->move( r.right() + 1, r.top() );
	else
		m_button->move( r.right() - m_button->width(), r.top() );
}

void KexiComboBoxTableEdit::hideFocus()
{
	m_button->hide();
}

QVariant KexiComboBoxTableEdit::value(bool &ok)
{
	ok = true;
	if (m_popup) {
		const int row = m_popup->tableView()->currentRow();
		if (row>=0)
			return QVariant( row );
		else
			return QVariant();
	}
	return m_origValue; //unchanged
}

bool KexiComboBoxTableEdit::cursorAtStart()
{
	//TODO?
	return false;
}

bool KexiComboBoxTableEdit::cursorAtEnd()
{
	//TODO?
	return false;
}

void KexiComboBoxTableEdit::clear()
{
	m_lineedit->clear();
	if (m_popup)
		m_popup->hide();
}

bool KexiComboBoxTableEdit::valueChanged()
{
	//just compare values
	return KexiTableEdit::valueChanged();
}

bool KexiComboBoxTableEdit::valueIsNull()
{
	bool ok;
	QVariant v = value(ok);
	return !ok || v.isNull();
}

bool KexiComboBoxTableEdit::valueIsEmpty()
{
	return valueIsNull();
}

void KexiComboBoxTableEdit::paintFocusBorders( QPainter *p, QVariant &, int x, int y, int w, int h )
{
	if (w > m_button->width())
		w -= m_button->width();
	p->drawRect(x, y, w, h);
}

void KexiComboBoxTableEdit::setupContents( QPainter *p, bool focused, QVariant val, 
	QString &txt, int &align, int &x, int &y_offset, int &w, int &h  )
{
	KexiTableEdit::setupContents( p, focused, val, txt, align, x, y_offset, w, h );
	if (focused && (w > m_button->width()))
		w -= (m_button->width() - x);
	if (!val.isNull())
		txt = m_field->enumHint( val.toInt() );
}

void KexiComboBoxTableEdit::slotButtonClicked()
{
	if (!m_popup || !m_popup->isVisible()) {
		showPopup();
	}
	else {
		m_popup->hide();
		//TODO: cancel selection
	}
}

void KexiComboBoxTableEdit::showPopup()
{
	if (!m_popup) {
//js TODO: now it's only for simple ENUM case!
		m_popup = new KexiComboBoxPopup(this, *m_field);
		connect(m_popup, SIGNAL(rowAccepted(KexiTableItem*,int)), 
			this, SLOT(slotRowAccepted(KexiTableItem*,int)));
		connect(m_popup, SIGNAL(cancelled()), this, SIGNAL(cancelRequested()));
		connect(m_popup->tableView(), SIGNAL(itemSelected(KexiTableItem*)),
			this, SLOT(slotItemSelected(KexiTableItem*)));

		m_popup->setFocusProxy( m_lineedit );
		m_popup->tableView()->setFocusProxy( m_lineedit );

		m_popup->tableView()->selectRow(m_origValue.toInt());//update selection
	}
	if (!m_lineedit->isVisible())
		emit editRequested();

	KexiTableView *tv = Kexi::findParent<KexiTableView>(this, "KexiTableView");
	if (tv) {
		m_popup->move( tv->viewport()->mapToGlobal(pos()) + QPoint(0,height()) );//+ rect().bottomLeft() ) );
		m_popup->show();
	}
	m_lineedit->setFocus();
}

/*int KexiComboBoxTableEdit::rightMargin()
{
	return m_button->width();
}*/

void KexiComboBoxTableEdit::hide()
{
	KexiInputTableEdit::hide();
	if (m_popup)
		m_popup->hide();
//	m_button->hide();
}

void KexiComboBoxTableEdit::show()
{
	KexiInputTableEdit::show();
	m_button->show();
}

void KexiComboBoxTableEdit::slotRowAccepted(KexiTableItem * item, int /*row*/)
{
	//update our value
	//..nothing to do?
//	emit acceptRequested();
	slotItemSelected(item);
}

bool KexiComboBoxTableEdit::handleKeyPress( QKeyEvent *ke, bool editorActive )
{
	const int k = ke->key();
	if (k==Key_F4) {//show popup
		slotButtonClicked();
		return true;
	}
	else if (editorActive && k==Key_Up) {
		m_popup->tableView()->selectPrevRow();
		return true;
	}
	else if (editorActive && k==Key_Down) {
		m_popup->tableView()->selectNextRow();
		return true;
	}
	else if (editorActive && k==Key_PageUp) {
		m_popup->tableView()->selectPrevPage();
		return true;
	}
	else if (editorActive && k==Key_PageDown) {
		m_popup->tableView()->selectNextPage();
		return true;
	}
	return false;
}

void KexiComboBoxTableEdit::slotItemSelected(KexiTableItem*)
{
	m_lineedit->setText( m_field->enumHint( m_popup->tableView()->currentRow() ) );
	m_lineedit->end(false);
	m_lineedit->selectAll();
}

void KexiComboBoxTableEdit::slotLineEditTextChanged(const QString &newtext)
{
	//todo
}

int KexiComboBoxTableEdit::widthForValue( QVariant &val, QFontMetrics &fm )
{
	QValueVector<QString> hints = m_field->enumHints();
	bool ok;
	int idx = val.toInt(&ok);
	if (!ok || idx < 0 || idx > int(hints.size()-1))
		return KEXITV_MINIMUM_COLUMN_WIDTH;
	QString txt = hints.at( idx, &ok );
	if (!ok)
		return KEXITV_MINIMUM_COLUMN_WIDTH;
	return fm.width( txt );
}

//======================================================

KexiComboBoxEditorFactoryItem::KexiComboBoxEditorFactoryItem()
{
}

KexiComboBoxEditorFactoryItem::~KexiComboBoxEditorFactoryItem()
{
}

KexiTableEdit* KexiComboBoxEditorFactoryItem::createEditor(
	KexiDB::Field &f, QWidget* parent)
{
	return new KexiComboBoxTableEdit(f, parent);
}


#include "kexicomboboxtableedit.moc"
