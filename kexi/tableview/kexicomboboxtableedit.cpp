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
#include <qwindowsstyle.h>
#include <qpainter.h>

#include "kexicomboboxtableedit.h"
#include "kexicomboboxpopup.h"
#include "kexitableview.h"
#include "kexitableitem.h"
#include "kexi_utils.h"

#include <kpushbutton.h>
#include <klineedit.h>

class KDownArrowPushButton : public KPushButton
{
	public:
		KDownArrowPushButton( QWidget *parent )
		 : KPushButton(parent)
		{
			setToggleButton(true);
			styleChange(style());
		}
	protected:
		/**
		 * Reimplemented from @ref QPushButton. */
		virtual void drawButton(QPainter *p) {
//			QStyle::PrimitiveElement e = QStyle::PE_ArrowDown;
			int flags = QStyle::Style_Enabled | QStyle::Style_HasFocus;
			if (isDown())
				flags |= QStyle::Style_Down;

			KPushButton::drawButton(p);

			QRect r = rect();
			r.setHeight(r.height()+m_fixForHeight);
			if (m_drawComplexControl) {
//				style().drawControl(QStyle::CE_PushButton, p, this, rect(), colorGroup(), flags);
				style().drawComplexControl( QStyle::CC_ComboBox, p, this, r, colorGroup(),
				    flags, (uint)(QStyle::SC_ComboBoxArrow /*| QStyle::SC_ComboBoxFrame*/), //QStyle::SC_All ^ QStyle::SC_ComboBoxFrame,
				     /*QStyle::SC_ComboBoxArrow */ QStyle::SC_None );
			}
			else {
				r.setWidth(r.width()+2);
				style().drawPrimitive( QStyle::PE_ArrowDown, p, r, colorGroup(), flags);
			}
		}
		virtual void styleChange( QStyle & oldStyle ) {
			//<hack>
		  if (qstricmp(style().name(),"thinkeramik")==0) {
				m_fixForHeight = 3;
			}
			else
				m_fixForHeight = 0;
			//</hack>
			m_drawComplexControl = style().inherits("KStyle") || qstricmp(style().name(),"platinum")==0;
			setFixedWidth( style().querySubControlMetrics( QStyle::CC_ComboBox, 
				this, QStyle::SC_ComboBoxArrow ).width() +1 );
			KPushButton::styleChange(oldStyle);
		}

		int m_fixForHeight;
		bool m_drawComplexControl : 1;
};

//======================================================

//KexiComboBoxTableEdit::KexiComboBoxTableEdit(KexiDB::Field &f, QScrollView *parent)
KexiComboBoxTableEdit::KexiComboBoxTableEdit(KexiTableViewColumn &column, QScrollView *parent)
 : KexiInputTableEdit(column, parent)
{
	setName("KexiComboBoxTableEdit");
//	QHBoxLayout* layout = new QHBoxLayout(this);
	m_popup = 0;
	m_button = new KDownArrowPushButton( parent->viewport() );
//	m_button->setFixedWidth(35);
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

void KexiComboBoxTableEdit::init(const QString& add, bool /*removeOld*/)
{
//	m_combo->setCurrentItem(m_origValue.toInt() - 1);
	if (add.isEmpty()) {
		KexiTableViewData *relData = column()->relatedData();
		QString stringValue;
		if (relData) {
			//use 'related table data' model
//			KexiTableItem *it = m_popup ? m_popup->tableView()->selectedItem() : 0;
//			if (it)
			stringValue = m_origValue.toString();
//				stringValue = it->at(1).toString();
		}
		else {
			//use 'enum hints' model
			const int row = m_origValue.toInt();
			stringValue = field()->enumHint(row);
		}
		m_lineedit->setText( stringValue );
		
		if (m_popup) {
			if (m_origValue.isNull())
				m_popup->tableView()->clearSelection();
			else {
				if (relData) {
					//TODO: select proper row using m_origValue key
				}
				else {
					m_popup->tableView()->selectRow(m_origValue.toInt());
				}
			}
		}
	}
	else {
		//todo: autocompl.?
		if (m_popup)
			m_popup->tableView()->clearSelection();
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
	m_totalSize = QSize(w,h);
	QWidget::resize(w - m_button->width(), h);
	m_button->resize( h, h );
	m_rightMargin = m_parentRightMargin + m_button->width();
//	int xx = m_scrollView->contentsX();
	QRect r( pos().x(), pos().y(), w+1, h+1 );
	r.moveBy(m_scrollView->contentsX(),0);
	updateFocus( r );
	if (m_popup) {
//		KexiTableView *tv = static_cast<KexiTableView*>(m_scrollView);
//		m_popup->move( tv->viewport()->mapToGlobal(pos()) + QPoint(0,height()) );//+ rect().bottomLeft() ) );
//		m_popup->resize(w, m_popup->height());
		m_popup->updateSize();
	}
}

// internal
void KexiComboBoxTableEdit::updateFocus( const QRect& r )
{
	if (m_button->width() > r.width())
		moveChild(m_button, r.right() + 1, r.top());
	else
		moveChild(m_button, r.right() - m_button->width(), r.top() );

/*	if (m_button->width() > r.width())
		m_button->move( r.right() + 1, r.top() );
	else
		m_button->move( r.right() - m_button->width(), r.top() );*/
}

void KexiComboBoxTableEdit::hideFocus()
{
	m_button->hide();
}

QVariant KexiComboBoxTableEdit::value(bool &ok)
{
	ok = true;
	if (m_popup) {
		KexiTableViewData *relData = column()->relatedData();
		if (relData) {
			//use 'related table data' model
			KexiTableItem *it = m_popup->tableView()->selectedItem();
			return it ? it->at(0) : m_origValue;//QVariant();
		}
		else {
			//use 'enum hints' model
			const int row = m_popup->tableView()->currentRow();
			if (row>=0)
				return QVariant( row );
//		else
//			return m_origValue; //QVariant();
		}

	}
	if (m_lineedit->text().isEmpty())
		return QVariant();
/*js: TODO dont return just 1st row, but use autocompletion feature
      and: show message box if entered text does not match! */
//	return 0; //1st row
	return m_origValue; //unchanged
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
	if (m_popup)
		m_popup->hide();
}

bool KexiComboBoxTableEdit::valueChanged()
{
	//avoid comparing values:
	KexiTableViewData *relData = column()->relatedData();
	if (relData) {
		//use 'related table data' model
		KexiTableItem *it = m_popup->tableView()->selectedItem();
		if (!it)
			return false;
	}
	else {
		//use 'enum hints' model
		const int row = m_popup->tableView()->currentRow();
		if (row<0)
			return false;
	}

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
	if (!val.isNull()) {
		KexiTableViewData *relData = column()->relatedData();
		if (relData) {
			//use 'related table data' model
//			KexiTableItem *it = m_popup->tableView()->selectedItem();
//			if (it)
//				stringValue = it->at(1).toString();
			txt = val.toString();
		}
		else {
			//use 'enum hints' model
			txt = field()->enumHint( val.toInt() );
		}
	}
}

void KexiComboBoxTableEdit::slotButtonClicked()
{
	kdDebug() << "KexiComboBoxTableEdit::slotButtonClicked()" << endl;
//	if (!m_popup || !m_popup->isVisible()) {
	if (!m_popup || m_button->isOn()) {
		kdDebug() << "SHOW POPUP" << endl;
		showPopup();
		m_button->setOn(true);
	}
	else {
		m_popup->hide();
		//TODO: cancel selection
		m_button->setOn(false);
	}
}

void KexiComboBoxTableEdit::showPopup()
{
	if (!m_popup) {
//js TODO: now it's only for simple ENUM case!
//		m_popup = new KexiComboBoxPopup(this, *field());
		m_popup = new KexiComboBoxPopup(this, *m_column);
		connect(m_popup, SIGNAL(rowAccepted(KexiTableItem*,int)), 
			this, SLOT(slotRowAccepted(KexiTableItem*,int)));
		connect(m_popup, SIGNAL(cancelled()), this, SIGNAL(cancelRequested()));
		connect(m_popup, SIGNAL(hidden()), this, SLOT(slotPopupHidden()));
		connect(m_popup->tableView(), SIGNAL(itemSelected(KexiTableItem*)),
			this, SLOT(slotItemSelected(KexiTableItem*)));

		m_popup->setFocusProxy( m_lineedit );
		m_popup->tableView()->setFocusProxy( m_lineedit );
		m_popup->installEventFilter(this);

//		m_popup->tableView()->selectRow(m_origValue.toInt());//update selection
		if (m_origValue.isNull())
			m_popup->tableView()->clearSelection();
		else
			m_popup->tableView()->selectRow(m_origValue.toInt());
	}
	if (!m_lineedit->isVisible())
		emit editRequested();

//	KexiTableView *tv = Kexi::findParent<KexiTableView>(this, "KexiTableView");
	KexiTableView *tv = static_cast<KexiTableView*>(m_scrollView);
	if (tv) {
		m_popup->move( tv->viewport()->mapToGlobal(pos()) + QPoint(0,height()) );//+ rect().bottomLeft() ) );
		//to avoid flickering: first resize to 0-height, then show and resize back to prev. height
//		const int h = m_popup->height()
		const int w = m_popup->width();
		m_popup->resize(w, 0);
		m_popup->show();
		m_popup->updateSize();
//		m_popup->resize(w, h);
	}

	m_lineedit->setFocus();
}

void KexiComboBoxTableEdit::slotPopupHidden()
{
	m_button->setOn(false);
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
	m_button->setOn(false);
//	m_button->hide();
}

void KexiComboBoxTableEdit::show()
{
	KexiInputTableEdit::show();
	m_button->show();
	m_button->setOn(false);
}

void KexiComboBoxTableEdit::slotRowAccepted(KexiTableItem * item, int /*row*/)
{
	//update our value
	//..nothing to do?
//	emit acceptRequested();
	slotItemSelected(item);
	emit acceptRequested();
}

bool KexiComboBoxTableEdit::handleKeyPress( QKeyEvent *ke, bool editorActive )
{
	const int k = ke->key();
	if (k==Key_F4) {//show popup
		slotButtonClicked();
		return true;
	}
	else if (editorActive && k==Key_Up) {
		if (m_popup && m_popup->isVisible()) {
			m_popup->tableView()->selectPrevRow();
			return true;
		}
	}
	else if (editorActive && k==Key_Down) {
		if (m_popup && m_popup->isVisible()) {
			m_popup->tableView()->selectNextRow();
			return true;
		}
	}
	else if (editorActive && k==Key_PageUp) {
		if (m_popup && m_popup->isVisible()) {
			m_popup->tableView()->selectPrevPage();
			return true;
		}
	}
	else if (editorActive && k==Key_PageDown) {
		if (m_popup && m_popup->isVisible()) {
			m_popup->tableView()->selectNextPage();
			return true;
		}
	}
	return false;
}

void KexiComboBoxTableEdit::slotItemSelected(KexiTableItem*)
{
	QString stringValue;
	KexiTableViewData *relData = column()->relatedData();
	if (relData) {
		//use 'related table data' model
		KexiTableItem *it = m_popup->tableView()->selectedItem();
		if (it)
			stringValue = it->at(1).toString();
	}
	else {
		//use 'enum hints' model
		stringValue = field()->enumHint( m_popup->tableView()->currentRow() );
	}
	m_lineedit->setText( stringValue );
	m_lineedit->end(false);
	m_lineedit->selectAll();
}

void KexiComboBoxTableEdit::slotLineEditTextChanged(const QString &newtext)
{
	if (newtext.isEmpty()) {
		if (m_popup) {
			m_popup->tableView()->clearSelection();
		}
		return;
	}
	//todo: select matching row for given prefix
}

int KexiComboBoxTableEdit::widthForValue( QVariant &val, QFontMetrics &fm )
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
}

bool KexiComboBoxTableEdit::eventFilter( QObject * /*o*/, QEvent *e )
{
	if (e->type()==QEvent::MouseButtonPress) {
		kdDebug() <<"??" <<endl;
	}
	return false;
}

//======================================================

KexiComboBoxEditorFactoryItem::KexiComboBoxEditorFactoryItem()
{
}

KexiComboBoxEditorFactoryItem::~KexiComboBoxEditorFactoryItem()
{
}

KexiTableEdit* KexiComboBoxEditorFactoryItem::createEditor(
	KexiTableViewColumn &column, QScrollView* parent)
{
	return new KexiComboBoxTableEdit(column, parent);
}


#include "kexicomboboxtableedit.moc"
