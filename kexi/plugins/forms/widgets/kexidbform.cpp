/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2005-2007 Jaroslaw Staniek <js@iidea.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <qobject.h>
#include <qpainter.h>
#include <qcursor.h>
#include <qapplication.h>
#include <QPixmap>
#include <QFocusEvent>
#include <QList>
#include <QEvent>
#include <QKeyEvent>
#include <QDragMoveEvent>
#include <QDropEvent>

#include <kdebug.h>

#include "kexidbform.h"
#include "kexiformpart.h"
#include "kexiformscrollview.h"

#include <formeditor/objecttree.h>
#include <formeditor/formmanager.h> 
#include <formeditor/widgetlibrary.h>
#include <widget/tableview/kexidataawareobjectiface.h>
#include <widget/kexiscrollview.h>
#include <kexiutils/utils.h>
#include <kexi_global.h>

//! @internal
class KexiDBForm::Private
{
	public:
		Private()
		 : dataAwareObject(0)
		 , autoTabStops(false)
		 , popupFocused(false)
		{
		}

		~Private()
		{
		}

		//! \return index of data-aware widget \a widget
		int indexOfDataAwareWidget(QWidget *widget) const
		{
			if (!dynamic_cast<KexiDataItemInterface*>(widget))
				return -1;
			return indexOfDataItem( dynamic_cast<KexiDataItemInterface*>(widget) );
		}

		//! \return index of data item \a item, or -1 if not found
		int indexOfDataItem( KexiDataItemInterface* item ) const
		{
			QHash<KexiDataItemInterface*, uint>::const_iterator indicesForDataAwareWidgetsIt(
				indicesForDataAwareWidgets.find(item));
			if (indicesForDataAwareWidgetsIt == indicesForDataAwareWidgets.constEnd())
				return -1;
			kexipluginsdbg << "KexiDBForm: column # for item: "
				<< indicesForDataAwareWidgetsIt.value() << endl;
			return indicesForDataAwareWidgetsIt.value();
		}

		//! Sets orderedFocusWidgetsIterator member to a position pointing to \a widget
		void setOrderedFocusWidgetsIteratorTo( QWidget *widget )
		{
			if (*orderedFocusWidgetsIterator == widget)
				return;
			orderedFocusWidgetsIterator = orderedFocusWidgets.begin();
			while (orderedFocusWidgetsIterator!=orderedFocusWidgets.end()
				&& *orderedFocusWidgetsIterator!=widget)
			{
				++orderedFocusWidgetsIterator;
			}
		}

		KexiDataAwareObjectInterface* dataAwareObject;
		//! ordered list of focusable widgets (can be both data-widgets or buttons, etc.)
		QList<QWidget*> orderedFocusWidgets;
		//! ordered list of data-aware widgets
		QList<QWidget*> orderedDataAwareWidgets;
		//! a subset of orderedFocusWidgets mapped to indices
		QHash<KexiDataItemInterface*, uint> indicesForDataAwareWidgets;
		QList<QWidget*>::iterator orderedFocusWidgetsIterator;
		QPixmap buffer; //!< stores grabbed entire form's area for redraw
		QRect prev_rect; //!< previously selected rectangle
		bool autoTabStops : 1;
		bool popupFocused : 1; //!< used in KexiDBForm::eventFilter()
};

//========================

KexiDBForm::KexiDBForm(QWidget *parent, KexiDataAwareObjectInterface* dataAwareObject)
 : KexiDBFormBase(parent)
 , KexiFormDataItemInterface()
 , d(new Private())
{
	installEventFilter(this);
//test	setDisplayMode( KexiGradientWidget::SimpleGradient );
	editedItem = 0;
	d->dataAwareObject = dataAwareObject;
	m_hasFocusableWidget = false;

	kexipluginsdbg << "KexiDBForm::KexiDBForm(): " << endl;
	setCursor(QCursor(Qt::ArrowCursor)); //to avoid keeping Size cursor when moving from form's boundaries
	setAcceptDrops( true );
}

KexiDBForm::~KexiDBForm()
{
	kexipluginsdbg << "KexiDBForm::~KexiDBForm(): close" << endl;
	delete d;
}

KexiDataAwareObjectInterface* KexiDBForm::dataAwareObject() const { return d->dataAwareObject; }

//repaint all children widgets
static void repaintAll(QWidget *w)
{
	const QList<QWidget*> list( w->findChildren<QWidget*>() );
	foreach (QWidget *widget, list)
		widget->repaint();
}

void
KexiDBForm::drawRect(const QRect& r, int type)
{
	QList<QRect> l;
	l.append(r);
	drawRects(l, type);
}

void
KexiDBForm::drawRects(const QList<QRect> &list, int type)
{
	QPainter p;
	p.begin(this);
#ifdef __GNUC__
#warning ???	p.begin(this, true);
#else
#pragma WARNING( ???	p.begin(this, true); )
#endif
	bool unclipped = testAttribute( Qt::WA_PaintUnclipped );
	setAttribute( Qt::WA_PaintUnclipped, true );

	if (d->prev_rect.isValid()) {
		//redraw prev. selection's rectangle
		p.drawPixmap( QPoint(d->prev_rect.x()-2, d->prev_rect.y()-2), d->buffer, 
			QRect(d->prev_rect.x()-2, d->prev_rect.y()-2, d->prev_rect.width()+4, d->prev_rect.height()+4));
	}
	p.setBrush(Qt::NoBrush);
	if(type == 1) // selection rect
		p.setPen(QPen(Qt::white, 1, Qt::DotLine));
	else if(type == 2) // insert rect
		p.setPen(QPen(Qt::white, 2));
	p.setCompositionMode(QPainter::CompositionMode_Xor);

	d->prev_rect = QRect();
	foreach (const QRect& r, list) {
		p.drawRect(r);
		if (d->prev_rect.isValid())
			d->prev_rect = d->prev_rect.unite(r);
		else
			d->prev_rect = r;
	}

	if (!unclipped)
		setAttribute( Qt::WA_PaintUnclipped, false );
	p.end();
}

void
KexiDBForm::initBuffer()
{
	repaintAll(this);
	//Qt4 d->buffer.resize( width(), height() );
	d->buffer = QPixmap::grabWindow( winId() );
	d->prev_rect = QRect();
}

void
KexiDBForm::clearForm()
{
	QPainter p;
#ifdef __GNUC__
#warning ??	p.begin(this, true);
#else
#pragma WARNING( ??	p.begin(this, true); )
#endif
	p.begin(this);
	bool unclipped = testAttribute( Qt::WA_PaintUnclipped );
	setAttribute( Qt::WA_PaintUnclipped, true );

	//redraw entire form surface
	p.drawPixmap( QPoint(0,0), d->buffer, QRect(0,0,d->buffer.width(), d->buffer.height()) );

	if (!unclipped)
		setAttribute( Qt::WA_PaintUnclipped, false );
	
	p.end();
	repaintAll(this);
}

void
KexiDBForm::highlightWidgets(QWidget *from, QWidget *to)//, const QPoint &point)
{
	QPoint fromPoint, toPoint;
	if(from && from->parentWidget() && (from != this))
		fromPoint = from->parentWidget()->mapTo(this, from->pos());
	if(to && to->parentWidget() && (to != this))
		toPoint = to->parentWidget()->mapTo(this, to->pos());

	QPainter p;
#ifdef __GNUC__
#warning ???	p.begin(this, true);
#else
#pragma WARNING( ???	p.begin(this, true); )
#endif
	p.begin(this);
	bool unclipped = testAttribute( Qt::WA_PaintUnclipped );
	setAttribute( Qt::WA_PaintUnclipped, true );

	if (d->prev_rect.isValid()) {
		//redraw prev. selection's rectangle
		p.drawPixmap( QPoint(d->prev_rect.x(), d->prev_rect.y()), d->buffer, 
			QRect(d->prev_rect.x(), d->prev_rect.y(), d->prev_rect.width(), d->prev_rect.height()));
	}

	p.setPen( QPen(Qt::red, 2) );

	if(to)
	{
		QPixmap pix1 = QPixmap::grabWidget(from);
		QPixmap pix2 = QPixmap::grabWidget(to);

		if((from != this) && (to != this))
			p.drawLine( from->parentWidget()->mapTo(this, from->geometry().center()), to->parentWidget()->mapTo(this, to->geometry().center()) );

		p.drawPixmap(fromPoint.x(), fromPoint.y(), pix1);
		p.drawPixmap(toPoint.x(), toPoint.y(), pix2);

		if(to == this)
			p.drawRoundRect(2, 2, width()-4, height()-4, 4, 4);
		else
			p.drawRoundRect(toPoint.x(), toPoint.y(), to->width(), to->height(), 5, 5);
	}

	if(from == this)
		p.drawRoundRect(2, 2, width()-4, height()-4, 4, 4);
	else
		p.drawRoundRect(fromPoint.x(),  fromPoint.y(), from->width(), from->height(), 5, 5);

	if((to == this) || (from == this))
		d->prev_rect = QRect(0, 0, d->buffer.width(), d->buffer.height());
	else if(to)
	{
		d->prev_rect.setX( (fromPoint.x() < toPoint.x()) ? (fromPoint.x() - 5) : (toPoint.x() - 5) );
		d->prev_rect.setY( (fromPoint.y() < toPoint.y()) ? (fromPoint.y() - 5) : (toPoint.y() - 5) );
		d->prev_rect.setRight( (fromPoint.x() < toPoint.x()) ? (toPoint.x() + to->width() + 10) : (fromPoint.x() + from->width() + 10) );
		d->prev_rect.setBottom( (fromPoint.y() < toPoint.y()) ? (toPoint.y() + to->height() + 10) : (fromPoint.y() + from->height() + 10) ) ;
	}
	else
		d->prev_rect = QRect(fromPoint.x()- 5,  fromPoint.y() -5, from->width() + 10, from->height() + 10);

	if (!unclipped)
		setAttribute( Qt::WA_PaintUnclipped, false );
	
	p.end();
}

QSize
KexiDBForm::sizeHint() const
{
	//todo: find better size (user configured?)
	return QSize(400,300);
}

void KexiDBForm::setInvalidState( const QString& displayText )
{
	Q_UNUSED( displayText );

	//! @todo draw "invalid data source" text on the surface?
}

bool KexiDBForm::autoTabStops() const
{
	return d->autoTabStops;
}

void KexiDBForm::setAutoTabStops(bool set)
{
	d->autoTabStops = set;
}

QList<QWidget*>* KexiDBForm::orderedFocusWidgets() const
{
	return &d->orderedFocusWidgets;
}

QList<QWidget*>* KexiDBForm::orderedDataAwareWidgets() const
{
	return &d->orderedDataAwareWidgets;
}

void KexiDBForm::updateTabStopsOrder(KFormDesigner::Form* form)
{
	QWidget *fromWidget = 0;
	//QWidget *topLevelWidget = form->widget()->topLevelWidget();
//js	form->updateTabStopsOrder(); //certain widgets can have now updated focusPolicy properties, fix this
	uint numberOfDataAwareWidgets = 0;
//	if (d->orderedFocusWidgets.isEmpty()) {
		//generate a new list
		for (KFormDesigner::ObjectTreeListIterator it(form->tabStopsIterator()); it.current(); ++it) {
			if (it.current()->widget()->focusPolicy() & Qt::TabFocus) {
				//this widget has tab focus:
				it.current()->widget()->installEventFilter(this);
				//also filter events for data-aware children of this widget (i.e. KexiDBAutoField's editors)
				QList<QWidget*> children( it.current()->widget()->findChildren<QWidget*>() );
				foreach (QWidget* widget, children) {
	//				if (dynamic_cast<KexiFormDataItemInterface*>(childrenIt.current())) {
						kexipluginsdbg << "KexiDBForm::updateTabStopsOrder(): also adding '" 
							<< widget->metaObject()->className()
								<< " " << widget->objectName()  
							<< "' child to filtered widgets" << endl;
						//it.current()->widget()->installEventFilter(static_cast<QWidget*>(childrenIt.current()));
						widget->installEventFilter(this);
		//			}
				}
				if (fromWidget) {
					kexipluginsdbg << "KexiDBForm::updateTabStopsOrder() tab order: "
						<< fromWidget->objectName() 
						<< " -> " << it.current()->widget()->objectName() << endl;
	//				setTabOrder( fromWidget, it.current()->widget() );
				}
				fromWidget = it.current()->widget();
				d->orderedFocusWidgets.append( it.current()->widget() );
			}

			KexiFormDataItemInterface* dataItem
				= dynamic_cast<KexiFormDataItemInterface*>( it.current()->widget() );
			if (dataItem && !dataItem->dataSource().isEmpty()) {
				kexipluginsdbg << "#" << numberOfDataAwareWidgets << ": " 
					<< dataItem->dataSource() << " (" << it.current()->widget()->objectName() << ")" << endl;

//	/*! @todo d->indicesForDataAwareWidgets SHOULDNT BE UPDATED HERE BECAUSE
//	THERE CAN BE ALSO NON-TABSTOP DATA WIDGETS!
//	*/
				d->indicesForDataAwareWidgets.insert( dataItem, numberOfDataAwareWidgets );
				numberOfDataAwareWidgets++;

				d->orderedDataAwareWidgets.append( it.current()->widget() );
			}
		}//for
//	}
/*	else {
		//restore ordering
		for (QPtrListIterator<QWidget> it(d->orderedFocusWidgets); it.current(); ++it) {
			if (fromWidget) {
				kDebug() << "KexiDBForm::updateTabStopsOrder() tab order: " << fromWidget->name() 
					<< " -> " << it.current()->name() << endl;
				setTabOrder( fromWidget, it.current() );
			}
			fromWidget = it.current();
		}
//		SET_FOCUS_USING_REASON(focusWidget(), QFocusEvent::Tab);
	}*/
}

void KexiDBForm::updateTabStopsOrder()
{
	for (QList<QWidget*>::iterator it( d->orderedFocusWidgets.begin() );
		it!=d->orderedFocusWidgets.end(); )
	{
		if (! ((*it)->focusPolicy() & Qt::TabFocus))
			it = d->orderedFocusWidgets.erase( it );
		else
			++it;
	}
}

void KexiDBForm::updateReadOnlyFlags()
{
	foreach (QWidget *w, d->orderedDataAwareWidgets) {
		KexiFormDataItemInterface* dataItem = dynamic_cast<KexiFormDataItemInterface*>( w );
		if (dataItem && !dataItem->dataSource().isEmpty()) {
			if (dataAwareObject()->isReadOnly()) {
				dataItem->setReadOnly( true );
			}
		}
	}
}

bool KexiDBForm::eventFilter( QObject * watched, QEvent * e )
{
	//kexipluginsdbg << e->type() << endl;
	if (e->type()==QEvent::Resize && watched == this)
		kexipluginsdbg << "RESIZE" << endl;
	if (e->type()==QEvent::KeyPress) {
		if (preview()) {
			QKeyEvent *ke = static_cast<QKeyEvent*>(e);
			const int key = ke->key();
			bool tab = ke->modifiers() == Qt::NoModifier && key == Qt::Key_Tab;
			bool backtab = ((ke->modifiers() == Qt::NoModifier || ke->modifiers() == Qt::ShiftModifier) && key == Qt::Key_Backtab)
				|| (ke->modifiers() == Qt::ShiftModifier && key == Qt::Key_Tab);
			QObject *o = watched; //focusWidget();
			QWidget* realWidget = dynamic_cast<QWidget*>(o); //will beused below (for tab/backtab handling)

			if (!tab && !backtab) {
				//for buttons, left/up and right/down keys act like tab/backtab (see qbutton.cpp)
				if (realWidget->inherits("QButton")) {
					if (ke->modifiers() == Qt::NoModifier && (key == Qt::Key_Right || key == Qt::Key_Down))
						tab = true;
					else if (ke->modifiers() == Qt::NoModifier && (key == Qt::Key_Left || key == Qt::Key_Up))
						backtab = true;
				}
			}

			if (!tab && !backtab) {
				// allow the editor widget to grab the key press event
				while (true) {
					if (!o || o == dynamic_cast<QObject*>(d->dataAwareObject))
						break;
					if (dynamic_cast<KexiFormDataItemInterface*>(o)) {
						realWidget = dynamic_cast<QWidget*>(o); //will be used below
						if (realWidget == this) //we have encountered 'this' form surface, give up
							return false;
						KexiFormDataItemInterface* dataItemIface = dynamic_cast<KexiFormDataItemInterface*>(o);
						while (dataItemIface) {
							if (dataItemIface->keyPressed(ke))
								return false;
							dataItemIface = dynamic_cast<KexiFormDataItemInterface*>(dataItemIface->parentInterface()); //try in parent, e.g. in combobox
						}
						break;
					}
					o = o->parent();
				}
				// try to handle global shortcuts at the KexiDataAwareObjectInterface 
				// level (e.g. for "next record" action)
				int curRow = d->dataAwareObject->currentRow();
				int curCol = d->dataAwareObject->currentColumn();
				bool moveToFirstField; //if true, we'll move focus to the first field (in tab order)
				bool moveToLastField; //if true, we'll move focus to the first field (in tab order)
				if (! (ke->modifiers() == Qt::NoModifier && (key == Qt::Key_Home 
					     || key == Qt::Key_End || key == Qt::Key_Down || key == Qt::Key_Up)) 
							    /* ^^ home/end/down/up are already handled by widgets */
					&& d->dataAwareObject->handleKeyPress(
						ke, curRow, curCol, false/*!fullRowSelection*/, &moveToFirstField, &moveToLastField))
				{
					if (ke->isAccepted())
						return true;
					QWidget* widgetToFocus;
					if (moveToFirstField) {
						widgetToFocus = d->orderedFocusWidgets.first(); //?
						curCol = d->indexOfDataAwareWidget( widgetToFocus );
					}
					else if (moveToLastField) {
						widgetToFocus = d->orderedFocusWidgets.last(); //?
						curCol = d->indexOfDataAwareWidget( widgetToFocus );
					}
					else
						widgetToFocus = d->orderedDataAwareWidgets.at( curCol ); //?

					d->dataAwareObject->setCursorPosition( curRow, curCol );

					if (widgetToFocus)
						widgetToFocus->setFocus();
					else
						kexipluginswarn << "KexiDBForm::eventFilter(): widgetToFocus not found!" << endl;

					ke->accept();
					return true;
				}
				if (key == Qt::Key_Delete && ke->modifiers()==Qt::ControlModifier) {
//! @todo remove hardcoded shortcuts: can be reconfigured...
					d->dataAwareObject->deleteCurrentRow();
					return true;
				}
			}
			// handle Esc key
			if (ke->modifiers() == Qt::NoModifier && key == Qt::Key_Escape) {
				//cancel field editing/row editing if possible
				if (d->dataAwareObject->cancelEditor())
					return true;
				else if (d->dataAwareObject->cancelRowEdit())
					return true;
				return false; // canceling not needed - pass the event to the active widget
			}
			// jstaniek: Fix for Qt bug (handling e.g. Alt+2, Ctrl+2 keys on every platform)
			//           It's important because we're using alt+2 short cut by default
			//           Damn! I've reported this to Trolltech in November 2004 - still not fixed.
			if (ke->isAccepted() && (ke->modifiers() & Qt::AltModifier) && ke->text()>="0" && ke->text()<="9")
				return true;

			if (tab || backtab) {
				//the watched widget can be a subwidget of a real widget, e.g. a drop down button of image box: find it
				while ( !KexiFormPart::library()->widgetInfoForClassName(
					realWidget->metaObject()->className()) )
				{
					realWidget = realWidget->parentWidget();
				}
				if (!realWidget)
					return true; //ignore
				//the watched widget can be a subwidget of a real widget, e.g. autofield: find it
				//QWidget* realWidget = static_cast<QWidget*>(watched);
				while (dynamic_cast<KexiDataItemInterface*>(realWidget) && dynamic_cast<KexiDataItemInterface*>(realWidget)->parentInterface())
					realWidget = dynamic_cast<QWidget*>( dynamic_cast<KexiDataItemInterface*>(realWidget)->parentInterface() );

				d->setOrderedFocusWidgetsIteratorTo( realWidget );
				kexipluginsdbg << realWidget->objectName() << endl;

				// find next/prev widget to focus
				QWidget *widgetToUnfocus = realWidget;
				QWidget *widgetToFocus = 0;
				bool wasAtFirstWidget = false; //used to protect against infinite loop
				while (true) {
					if (tab) {
						if (d->orderedFocusWidgets.first() && realWidget == d->orderedFocusWidgets.last()) {
							if (wasAtFirstWidget)
								break;
							d->orderedFocusWidgetsIterator = d->orderedFocusWidgets.begin();
							wasAtFirstWidget = true;
						}
						else if (realWidget == *d->orderedFocusWidgetsIterator) {
							++d->orderedFocusWidgetsIterator; //next
						}
						else
							return true; //ignore
					}
					else {//backtab
						if (d->orderedFocusWidgets.last() && realWidget == d->orderedFocusWidgets.first()) {
							d->orderedFocusWidgetsIterator 
								= d->orderedFocusWidgets.begin() + (d->orderedFocusWidgets.count()-1);
						}
						else if (realWidget == *d->orderedFocusWidgetsIterator) {
							--d->orderedFocusWidgetsIterator; //prev
						}
						else
							return true; //ignore
					}

					widgetToFocus = *d->orderedFocusWidgetsIterator;

					QObject *pageFor_widgetToFocus = 0;
					KFormDesigner::TabWidget *tabWidgetFor_widgetToFocus 
						= KFormDesigner::findParent<KFormDesigner::TabWidget>(
							widgetToFocus, "KFormDesigner::TabWidget", pageFor_widgetToFocus);
					if (tabWidgetFor_widgetToFocus
						&& tabWidgetFor_widgetToFocus->currentWidget()!=pageFor_widgetToFocus)
					{
						realWidget = widgetToFocus;
						continue; // the new widget to focus is placed on invisible tab page:
						          // move to next widget
					}
					break;
				}//while
			
				//set focus, but don't use just setFocus() because certain widgets
				//behaves differently (e.g. QLineEdit calls selectAll()) when 
				//focus event's reason is Qt::TabFocusReason
				if (widgetToFocus->focusProxy())
					widgetToFocus = widgetToFocus->focusProxy();
				if (widgetToFocus && d->dataAwareObject->acceptEditor()) {
					if (tab) {
						//try to accept this will validate the current input (if any)
						KexiUtils::unsetFocusWithReason(widgetToUnfocus, Qt::TabFocusReason);
						KexiUtils::setFocusWithReason(widgetToFocus, Qt::TabFocusReason);
						kexipluginsdbg << "focusing " << widgetToFocus->objectName() << endl;
					}
					else {//backtab
						KexiUtils::unsetFocusWithReason(widgetToUnfocus, Qt::BacktabFocusReason);
						//set focus, see above note
						KexiUtils::setFocusWithReason(*d->orderedFocusWidgetsIterator, Qt::BacktabFocusReason);
						kexipluginsdbg << "focusing " 
							<< (*d->orderedFocusWidgetsIterator)->objectName() << endl;
					}
				}
				return true;
			}
		}
	}
	else if (e->type()==QEvent::FocusIn) {
		bool focusDataWidget = preview();
		if (static_cast<QFocusEvent*>(e)->reason()==Qt::PopupFocusReason) {
			kDebug() << "->>> focus IN, popup" <<endl;
			focusDataWidget = !d->popupFocused;
			d->popupFocused = false;
//			if (d->widgetFocusedBeforePopup) {
//				watched = d->widgetFocusedBeforePopup;
//				d->widgetFocusedBeforePopup = 0;
//			}
		}

		if (focusDataWidget) {
			kexipluginsdbg << "KexiDBForm: FocusIn: " << watched->metaObject()->className()
				<< " " << watched->objectName() << endl;
			if (d->dataAwareObject) {
				QWidget *dataItem = dynamic_cast<QWidget*>(watched);
				while (dataItem) {
					while (dataItem && !dynamic_cast<KexiDataItemInterface*>(dataItem))
						dataItem = dataItem->parentWidget();
					if (!dataItem)
						break;
					kexipluginsdbg << "KexiDBForm: FocusIn: FOUND " 
						<< dataItem->metaObject()->className() << " " << dataItem->objectName() << endl;

					const int index = d->indexOfDataAwareWidget(dataItem);
					if (index>=0) {
						kexipluginsdbg << "KexiDBForm: moving cursor to column #" << index << endl;
						editedItem = 0;
						if ((int)index!=d->dataAwareObject->currentColumn()) {
							d->dataAwareObject->setCursorPosition( d->dataAwareObject->currentRow(), index /*column*/ );
						}
						break;
					}
					else
						dataItem = dataItem->parentWidget();

					dataItem->update();
				}
			}
		}
	}
	else if (e->type()==QEvent::FocusOut) {
		if (static_cast<QFocusEvent*>(e)->reason()==Qt::PopupFocusReason) {
			//d->widgetFocusedBeforePopup = (QWidget*)watched;
			d->popupFocused = true;
		}
		else
			d->popupFocused = false;
//			d->widgetFocusedBeforePopup = 0;
//		kDebug() << "e->type()==QEvent::FocusOut " << watched->className() << " " <<watched->name() << endl;
//		UNSET_FOCUS_USING_REASON(watched, static_cast<QFocusEvent*>(e)->reason());
	}
	return KexiDBFormBase::eventFilter(watched, e);
}

bool KexiDBForm::valueIsNull()
{
	return true;
}

bool KexiDBForm::valueIsEmpty()
{
	return true;
}

bool KexiDBForm::isReadOnly() const
{
	if (d->dataAwareObject)
		return d->dataAwareObject->isReadOnly();
//! @todo ?
	return false;
}

void KexiDBForm::setReadOnly( bool readOnly )
{
	if (d->dataAwareObject)
		d->dataAwareObject->setReadOnly( readOnly ); //???
}

QWidget* KexiDBForm::widget()
{
	return this;
}

bool KexiDBForm::cursorAtStart()
{
	return false;
}

bool KexiDBForm::cursorAtEnd()
{
	return false;
}

void KexiDBForm::clear()
{
	//! @todo clear all fields?
}

bool KexiDBForm::preview() const {
	return dynamic_cast<KexiScrollView*>(d->dataAwareObject)
		? dynamic_cast<KexiScrollView*>(d->dataAwareObject)->preview() : false;
}

void KexiDBForm::dragMoveEvent( QDragMoveEvent *e )
{
	KexiDBFormBase::dragMoveEvent( e );
	emit handleDragMoveEvent(e);
}

void KexiDBForm::dropEvent( QDropEvent *e ) 
{
	KexiDBFormBase::dropEvent( e );
	emit handleDropEvent(e);
}

void KexiDBForm::setCursor( const QCursor & cursor )
{
	//js: empty, to avoid fscking problems with random cursors!
	//! @todo?

	if (KFormDesigner::FormManager::self()->isInserting()) //exception
		KexiDBFormBase::setCursor(cursor);
}

//! @todo: Qt4? XORed resize rectangles instead of black widgets
/*
void KexiDBForm::paintEvent( QPaintEvent *e )
{
	QPainter p;
	p.begin(this);
	bool unclipped = testWFlags( WPaintUnclipped );
	setWFlags( WPaintUnclipped );

	p.setPen(white);
	p.setRasterOp(XorROP);
	p.drawLine(e->rect().topLeft(), e->rect().bottomRight());

	if (!unclipped)
		clearWFlags( WPaintUnclipped );
	p.end();
	KexiDBFormBase::paintEvent(e);
}
*/

#include "kexidbform.moc"
