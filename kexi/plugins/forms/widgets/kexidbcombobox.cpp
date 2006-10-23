/* This file is part of the KDE project
   Copyright (C) 2006 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexidbcombobox.h"
#include "kexidblineedit.h"
#include "../kexiformscrollview.h"

#include <kcombobox.h>
#include <kdebug.h>
#include <kapplication.h>

#include <qmetaobject.h>
#include <qpainter.h>
#include <qstyle.h>
#include <qdrawutil.h>

#include <kexidb/queryschema.h>
#include <widget/tableview/kexicomboboxpopup.h>
#include <kexiutils/utils.h>

//! @internal
class KexiDBComboBox::Private
{
	public:
		Private()
		 : isEditable(false)
		 , buttonPressed(false)
		 , mouseOver(false)
		{
		}

	KComboBox *paintedCombo; //!< fake combo used only to pass it as 'this' for QStyle (because styles use <static_cast>)
	QSize sizeHint; //!< A cache for KexiDBComboBox::sizeHint(), 
	                //!< rebuilt by KexiDBComboBox::fontChange() and KexiDBComboBox::styleChange()
	bool isEditable : 1;
	bool buttonPressed : 1;
	bool mouseOver : 1;
};

//-------------------------------------

KexiDBComboBox::KexiDBComboBox(QWidget *parent, const char *name, bool designMode)
 : KexiDBAutoField(parent, name, designMode, NoLabel)
 , KexiComboBoxBase()
 , d(new Private())
// , m_menuExtender(this, this)
// , m_slotTextChanged_enabled(true)
{
//	connect(this, SIGNAL(textChanged()), this, SLOT(slotTextChanged()));
	setMouseTracking(true);
	installEventFilter(this);
	d->paintedCombo = new KComboBox(this);
	d->paintedCombo->hide();
	d->paintedCombo->move(0,0);
}

KexiDBComboBox::~KexiDBComboBox()
{
	delete d;
}

void KexiDBComboBox::setEditable(bool set)
{
	if (d->isEditable == set)
		return;
	d->isEditable = set;
	d->paintedCombo->setEditable(set);
	if (set)
		createEditor();
	else {
		delete m_subwidget;
		m_subwidget = 0;
	}
	update();
}

bool KexiDBComboBox::isEditable() const
{
	return d->isEditable;
}

void KexiDBComboBox::paintEvent( QPaintEvent * )
{
	QPainter p( this );
	QColorGroup cg( palette().active() );
//	if ( hasFocus() )
//		cg.setColor(QColorGroup::Base, cg.highlight());
//	else
		cg.setColor(QColorGroup::Base, paletteBackgroundColor()); //update base color using (reimplemented) bg color
	p.setPen(cg.text());

	QStyle::SFlags flags = QStyle::Style_Default;
	if (isEnabled())
		flags |= QStyle::Style_Enabled;
	if (hasFocus())
		flags |= QStyle::Style_HasFocus;
	if (d->mouseOver)
		flags |= QStyle::Style_MouseOver;

	if ( width() < 5 || height() < 5 ) {
		qDrawShadePanel( &p, rect(), cg, FALSE, 2, &cg.brush( QColorGroup::Button ) );
		return;
	}

//todo	bool reverse = QApplication::reverseLayout();
	style().drawComplexControl( QStyle::CC_ComboBox, &p, d->paintedCombo /*this*/, rect(), cg,
		flags, (uint)QStyle::SC_All, 
		(d->buttonPressed ? QStyle::SC_ComboBoxArrow : QStyle::SC_None )
	);

	if (d->isEditable) {
		//if editable, editor paints itself, nothing to do
	}
	else { //not editable: we need to paint the current item
		QRect editorGeometry( this->editorGeometry() );
		if ( hasFocus() ) {
			if (0==qstrcmp(style().name(), "windows")) //a hack
				p.fillRect( editorGeometry, cg.brush( QColorGroup::Highlight ) );
			style().drawPrimitive( QStyle::PE_FocusRect, &p, 
				QStyle::visualRect( style().subRect( QStyle::SR_ComboBoxFocusRect, d->paintedCombo ), this ), 
				cg, flags | QStyle::Style_FocusAtBorder, QStyleOption(cg.highlight()));
		}
		//todo
	}
}

QRect KexiDBComboBox::editorGeometry() const
{
	QRect r( QStyle::visualRect(
		style().querySubControlMetrics(QStyle::CC_ComboBox, d->paintedCombo,
		QStyle::SC_ComboBoxEditField), d->paintedCombo ) );
	
	//if ((height()-r.bottom())<6)
//		r.setBottom(height()-6);
//	kDebug() << r << geometry() <<endl;
	return r;
}

void KexiDBComboBox::createEditor()
{
	if (!d->isEditable)
		return;
	KexiDBAutoField::createEditor();
	if (m_subwidget) {
//		m_subwidget->setMinimumSize(0,0);
		m_subwidget->setGeometry( editorGeometry() );
	}
	updateGeometry();
}

void KexiDBComboBox::setLabelPosition(LabelPosition position)
{
	if(m_subwidget) {
		if (-1 != m_subwidget->metaObject()->findProperty("frameShape", true))
			m_subwidget->setProperty("frameShape", QVariant((int)QFrame::NoFrame));
		m_subwidget->setGeometry( editorGeometry() );
//		KexiSubwidgetInterface *subwidgetInterface = dynamic_cast<KexiSubwidgetInterface*>((QWidget*)m_subwidget);
		// update size policy
//		if (subwidgetInterface && subwidgetInterface->subwidgetStretchRequired(this)) {
	}
			QSizePolicy sizePolicy( this->sizePolicy() );
			if(position == Left)
				sizePolicy.setHorData( QSizePolicy::Minimum );
			else
				sizePolicy.setVerData( QSizePolicy::Minimum );
			//m_subwidget->setSizePolicy(sizePolicy);
			setSizePolicy(sizePolicy);
		//}
//	}
}

QRect KexiDBComboBox::buttonGeometry() const
{
	QRect arrowRect( 
		style().querySubControlMetrics( QStyle::CC_ComboBox, d->paintedCombo, QStyle::SC_ComboBoxArrow) );
	arrowRect = QStyle::visualRect(arrowRect, d->paintedCombo);
	arrowRect.setHeight( QMAX(  height() - (2 * arrowRect.y()), arrowRect.height() ) ); // a fix for Motif style
	return arrowRect;
}

void KexiDBComboBox::mousePressEvent( QMouseEvent *e )
{
	if ( e->button() != Qt::LeftButton )
		return;
/*todo	if ( m_discardNextMousePress ) {
		d->discardNextMousePress = FALSE;
		return;
    }*/

	if ( /*count() &&*/ ( !isEditable() || buttonGeometry().contains( e->pos() ) ) ) {
		d->buttonPressed = false;

/*	if ( d->usingListBox() ) {
	    listBox()->blockSignals( TRUE );
	    qApp->sendEvent( listBox(), e ); // trigger the listbox's autoscroll
	    listBox()->setCurrentItem(d->current);
	    listBox()->blockSignals( FALSE );
	    popup();
	    if ( arrowRect.contains( e->pos() ) ) {
		d->arrowPressed = TRUE;
		d->arrowDown    = TRUE;
		repaint( FALSE );
	    }
	} else {*/
	    showPopup();
		return;
	}
//	QTimer::singleShot( 200, this, SLOT(internalClickTimeout()));
//	d->shortClick = TRUE;
//  }
	KexiDBAutoField::mousePressEvent( e );
}

void KexiDBComboBox::mouseDoubleClickEvent( QMouseEvent *e )
{
	mousePressEvent( e );
}

bool KexiDBComboBox::eventFilter( QObject *o, QEvent *e )
{
	if (o==this) {
		if (e->type()==QEvent::Resize) {
			d->paintedCombo->resize(size());
			if (m_subwidget)
				m_subwidget->setGeometry( editorGeometry() );
		}
		else if (e->type()==QEvent::Enter) {
			if (!d->isEditable 
				|| /*over button if editable combo*/buttonGeometry().contains( static_cast<QMouseEvent*>(e)->pos() )) 
			{
				d->mouseOver = true;
				update();
			}
		}
		else if (e->type()==QEvent::MouseMove) {
			if (d->isEditable) {
				const bool overButton = buttonGeometry().contains( static_cast<QMouseEvent*>(e)->pos() );
				if (overButton != d->mouseOver) {
					d->mouseOver = overButton;
					update();
				}
			}
		}
		else if (e->type()==QEvent::Leave) {
			d->mouseOver = false;
			update();
		}
	}
	return KexiDBAutoField::eventFilter( o, e );
}

bool KexiDBComboBox::subwidgetStretchRequired(KexiDBAutoField* autoField) const
{
	Q_UNUSED(autoField);
	return true;
}

void KexiDBComboBox::setPaletteBackgroundColor( const QColor & color )
{
	KexiDBAutoField::setPaletteBackgroundColor(color);
	QPalette pal(palette());
	QColorGroup cg(pal.active());
	pal.setColor(QColorGroup::Base, red);
	pal.setColor(QColorGroup::Background, red);
	pal.setActive(cg);
	QWidget::setPalette(pal);
	update();
}

/*
void KexiDBComboBox::setInvalidState( const QString& displayText )
{
	setReadOnly(true);
//! @todo move this to KexiDataItemInterface::setInvalidStateInternal() ?
	if (focusPolicy() & TabFocus)
		setFocusPolicy(QWidget::ClickFocus);
	KTextEdit::setText(displayText);
}

void KexiDBComboBox::setValueInternal(const QVariant& add, bool removeOld)
{
	if (m_columnInfo && m_columnInfo->field->type()==KexiDB::Field::Boolean) {
//! @todo temporary solution for booleans!
		KTextEdit::setText( add.toBool() ? "1" : "0" );
	}
	else {
		if (removeOld)
			KTextEdit::setText( add.toString() );
		else
			KTextEdit::setText( m_origValue.toString() + add.toString() );
	}
}

QVariant KexiDBComboBox::value()
{
	return text();
}

void KexiDBComboBox::slotTextChanged()
{
	if (!m_slotTextChanged_enabled)
		return;
	signalValueChanged();
}

bool KexiDBComboBox::valueIsNull()
{
	return text().isNull();
}

bool KexiDBComboBox::valueIsEmpty()
{
	return text().isEmpty();
}

bool KexiDBComboBox::isReadOnly() const
{
	return KTextEdit::isReadOnly();
}

void KexiDBComboBox::setReadOnly( bool readOnly )
{
	KTextEdit::setReadOnly( readOnly );
	QPalette p = palette();
	QColor c(readOnly ? lighterGrayBackgroundColor(kapp->palette()) : p.color(QPalette::Normal, QColorGroup::Base));
	setPaper( c );
	p.setColor(QColorGroup::Base, c);
	p.setColor(QColorGroup::Background, c);
	setPalette( p );
}

void KexiDBComboBox::setText( const QString & text, const QString & context )
{
	KTextEdit::setText(text, context);
}

QWidget* KexiDBComboBox::widget()
{
	return this;
}

bool KexiDBComboBox::cursorAtStart()
{
	int para, index;
	getCursorPosition ( &para, &index );
	return para==0 && index==0;
}

bool KexiDBComboBox::cursorAtEnd()
{
	int para, index;
	getCursorPosition ( &para, &index );
	return (paragraphs()-1)==para && (paragraphLength(paragraphs()-1)-1)==index;
}

void KexiDBComboBox::clear()
{
	setText(QString::null, QString::null);
}

void KexiDBComboBox::setColumnInfo(KexiDB::QueryColumnInfo* cinfo)
{
	KexiFormDataItemInterface::setColumnInfo(cinfo);
	if (!cinfo)
		return;
	KexiDBTextWidgetInterface::setColumnInfo(m_columnInfo, this);
}

void KexiDBComboBox::paintEvent ( QPaintEvent *pe )
{
	KTextEdit::paintEvent( pe );
	QPainter p(this);
	KexiDBTextWidgetInterface::paint( this, &p, text().isEmpty(), alignment(), hasFocus() );
}

QPopupMenu * KexiDBComboBox::createPopupMenu(const QPoint & pos)
{
	QPopupMenu *contextMenu = KTextEdit::createPopupMenu(pos);
	m_menuExtender.createTitle(contextMenu);
	return contextMenu;
}

void KexiDBComboBox::undo()
{
	cancelEditor();
}

void KexiDBComboBox::setDisplayDefaultValue(QWidget* widget, bool displayDefaultValue)
{
	KexiFormDataItemInterface::setDisplayDefaultValue(widget, displayDefaultValue);
	// initialize display parameters for default / entered value
	KexiDisplayUtils::DisplayParameters * const params 
		= displayDefaultValue ? m_displayParametersForDefaultValue : m_displayParametersForEnteredValue;
	QPalette pal(palette());
	pal.setColor(QPalette::Active, QColorGroup::Text, params->textColor);
	setPalette(pal);
	setFont(params->font);
//! @todo support rich text...
}*/

void KexiDBComboBox::moveCursorToEndInInternalEditor()
{
	//! @todo
}

void KexiDBComboBox::selectAllInInternalEditor()
{
	//! @todo
}

void KexiDBComboBox::setValueInInternalEditor(const QVariant& value)
{
	Q_UNUSED(value);
	//! @todo
}

QVariant KexiDBComboBox::valueFromInternalEditor() const
{
	//! @todo
	return QVariant();
}

QPoint KexiDBComboBox::mapFromParentToGlobal(const QPoint& pos) const
{
	const KexiFormScrollView* view = KexiUtils::findParentConst<const KexiFormScrollView>(this, "KexiFormScrollView"); 
	if (!view)
		return QPoint(-1,-1);
	return view->viewport()->mapToGlobal(pos);
}

int KexiDBComboBox::popupWidthHint() const
{
	return m_popup->width();
}

void KexiDBComboBox::fontChange( const QFont & oldFont )
{
	d->sizeHint = QSize(); //force rebuild the cache
	KexiDBAutoField::fontChange(oldFont);
}

void KexiDBComboBox::styleChange( QStyle& oldStyle )
{
	KexiDBAutoField::styleChange( oldStyle );
	d->sizeHint = QSize(); //force rebuild the cache
	if (m_subwidget)
		m_subwidget->setGeometry( editorGeometry() );
}

QSize KexiDBComboBox::sizeHint() const
{
	if ( isVisible() && d->sizeHint.isValid() )
		return d->sizeHint;

	const int maxWidth = 7 * fontMetrics().width(QChar('x')) + 18;
	const int maxHeight = qMax( fontMetrics().lineSpacing(), 14 ) + 2;
	d->sizeHint = (style().sizeFromContents(QStyle::CT_ComboBox, d->paintedCombo,
		QSize(maxWidth, maxHeight)).expandedTo(QApplication::globalStrut()));

	return d->sizeHint;
}

#include "kexidbcombobox.moc"
