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
	 : popup(0)
	 , currentEditorWidth(0)
	 , visibleTableViewColumn(0)
	 , internalEditor(0)
	{
	}
	~Private()
	{
		delete internalEditor;
		delete visibleTableViewColumn;
	}

	KPushButton *button;
	KexiComboBoxPopup *popup;
	int currentEditorWidth;
	QSize totalSize;
	KexiTableViewColumn* visibleTableViewColumn;
	KexiTableEdit* internalEditor;
};

//======================================================

KexiComboBoxTableEdit::KexiComboBoxTableEdit(KexiTableViewColumn &column, QWidget *parent)
 : KexiInputTableEdit(column, parent)
 , KexiComboBoxBase()
 , d(new Private())
{
	setName("KexiComboBoxTableEdit");
//	QHBoxLayout* layout = new QHBoxLayout(this);
	d->button = new KexiComboBoxDropDownButton( parentWidget() /*usually a viewport*/ );
	d->button->hide();
	d->button->setFocusPolicy( NoFocus );
	connect(d->button, SIGNAL(clicked()), this, SLOT(slotButtonClicked()));

	connect(m_lineedit, SIGNAL(textChanged(const QString&)), this, SLOT(slotLineEditTextChanged(const QString&)));

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

void KexiComboBoxTableEdit::createInternalEditor(KexiDB::QuerySchema& schema)
{
	if (!m_column->visibleLookupColumnInfo || d->visibleTableViewColumn/*sanity*/)
		return;
	const KexiDB::Field::Type t = m_column->visibleLookupColumnInfo->field->type();
//! @todo subtype?
	KexiCellEditorFactoryItem *item = KexiCellEditorFactory::item(t);
	if (!item || item->className()=="KexiInputTableEdit")
		return; //unsupported type or there is no need to use subeditor for KexiInputTableEdit
	//special cases: BLOB, Bool datatypes
//todo
	//find real type to display
	KexiDB::QueryColumnInfo *ci = m_column->visibleLookupColumnInfo;
	KexiDB::QueryColumnInfo *visibleLookupColumnInfo = 0;
	if (ci->indexForVisibleLookupValue() != -1) {
		//Lookup field is defined
		visibleLookupColumnInfo = schema.expandedOrInternalField( ci->indexForVisibleLookupValue() );
	}
	d->visibleTableViewColumn = new KexiTableViewColumn(schema, *ci, visibleLookupColumnInfo);
//! todo set d->internalEditor visible and use it to enable data entering by hand
	d->internalEditor = KexiCellEditorFactory::createEditor(*d->visibleTableViewColumn, 0);
	m_lineedit->hide();
}

KexiComboBoxPopup *KexiComboBoxTableEdit::popup() const
{
	return d->popup;
}

void KexiComboBoxTableEdit::setPopup(KexiComboBoxPopup *popup)
{
	d->popup = popup;
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
	m_rightMarginWhenFocused = m_rightMargin + (column()->isReadOnly() ? 0 : d->button->width());
	QRect r( pos().x(), pos().y(), w+1, h+1 );
	if (m_scrollView)
		r.moveBy(m_scrollView->contentsX(), m_scrollView->contentsY());
	updateFocus( r );
	if (popup()) {
		popup()->updateSize();
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

QVariant KexiComboBoxTableEdit::visibleValue()
{
	return KexiComboBoxBase::visibleValue();
/*	KexiDB::LookupFieldSchema *lookupFieldSchema = this->lookupFieldSchema();
	if (!popup() || !lookupFieldSchema)
		return QVariant();
	KexiTableItem *it = popup()->tableView()->selectedItem();
	return it ? it->at( lookupFieldSchema->visibleColumn() ) : QVariant();*/
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
//	d->currentEditorWidth = w;
	if (!column()->isReadOnly()) {
		if (w > d->button->width())
			w -= d->button->width();
	}
	p->drawRect(x, y, w, h);
}

void KexiComboBoxTableEdit::setupContents( QPainter *p, bool focused, const QVariant& val, 
	QString &txt, int &align, int &x, int &y_offset, int &w, int &h  )
{
	if (d->internalEditor) {
		d->internalEditor->setupContents( p, focused, val, txt, align, x, y_offset, w, h );
	}
	else {
		KexiInputTableEdit::setupContents( p, focused, val, txt, align, x, y_offset, w, h );
	}
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
		/* handled at at KexiTableView level
			if (popup()) {
				KexiTableItem *it = popup()->tableView()->selectedItem();
				if (it && lookupFieldSchema->visibleColumn()!=-1 && (int)it->size() >= lookupFieldSchema->visibleColumn())
					txt = it->at( lookupFieldSchema->visibleColumn() ).toString();
			}*/
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
	kdDebug() << "KexiComboBoxTableEdit::slotButtonClicked()" << endl;
	if (!popup() || !popup()->isVisible()) {
		kdDebug() << "SHOW POPUP" << endl;
		showPopup();
		d->button->setOn(true);
	}
}

void KexiComboBoxTableEdit::slotPopupHidden()
{
	d->button->setOn(false);
//	d->currentEditorWidth = 0;
}

void KexiComboBoxTableEdit::updateButton()
{
	d->button->setOn(popup()->isVisible());
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
	else if (editorActive) {
		const bool enterPressed = k==Qt::Key_Enter || k==Qt::Key_Return;
		if (enterPressed && m_internalEditorValueChanged) {
			createPopup(false);
			selectItemForEnteredValueInLookupTable( m_userEnteredValue );
			return false;
		}

		return handleKeyPressForPopup( ke );
	}

	return false;
}

void KexiComboBoxTableEdit::slotLineEditTextChanged(const QString& s)
{
	slotInternalEditorValueChanged(s);
}

int KexiComboBoxTableEdit::widthForValue( QVariant &val, const QFontMetrics &fm )
{
	KexiTableViewData *relData = column() ? column()->relatedData() : 0;
	if (lookupFieldSchema() || relData) {
		// in 'lookupFieldSchema' or  or 'related table data' model 
		// we're assuming val is already the text, not the index
//! @todo ok?
		return QMAX(KEXITV_MINIMUM_COLUMN_WIDTH, fm.width(val.toString()));
	}
	//use 'enum hints' model
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

bool KexiComboBoxTableEdit::eventFilter( QObject *o, QEvent *e )
{
	if (!column()->isReadOnly() && e->type()==QEvent::MouseButtonPress && m_scrollView) {
		QPoint gp = static_cast<QMouseEvent*>(e)->globalPos() 
			+ QPoint(m_scrollView->childX(d->button), m_scrollView->childY(d->button));
		QRect r(d->button->mapToGlobal(d->button->geometry().topLeft()), 
			d->button->mapToGlobal(d->button->geometry().bottomRight()));
		if (o==popup() && popup()->isVisible() && r.contains( gp )) {
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
	moveCursorToEnd();
}

void KexiComboBoxTableEdit::selectAllInInternalEditor()
{
	selectAll();
}

void KexiComboBoxTableEdit::moveCursorToEnd()
{
	m_lineedit->end(false/*!mark*/);
}

void KexiComboBoxTableEdit::moveCursorToStart()
{
	m_lineedit->home(false/*!mark*/);
}

void KexiComboBoxTableEdit::selectAll()
{
	m_lineedit->selectAll();
}

void KexiComboBoxTableEdit::setValueInInternalEditor(const QVariant& value)
{
	m_lineedit->setText(value.toString());
}

QVariant KexiComboBoxTableEdit::valueFromInternalEditor()
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
	return m_lineedit->width() + m_leftMargin + m_rightMarginWhenFocused; //QMAX(popup()->width(), d->currentEditorWidth);
}

KEXI_CELLEDITOR_FACTORY_ITEM_IMPL(KexiComboBoxEditorFactoryItem, KexiComboBoxTableEdit)

#include "kexicomboboxtableedit.moc"
