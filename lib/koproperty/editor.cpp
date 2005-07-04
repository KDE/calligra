/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004  Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2004  Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "editor.h"
#include "editoritem.h"
#include "set.h"
#include "factory.h"
#include "property.h"
#include "widget.h"

#include <qpushbutton.h>
#include <qlayout.h>
#include <qmap.h>
#include <qguardedptr.h>
#include <qheader.h>
#include <qasciidict.h>
#include <qtooltip.h>
#include <qapplication.h>
#include <qeventloop.h>
#include <qtimer.h>

#ifdef QT_ONLY
#else
#include <kdebug.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kdeversion.h>
#endif

namespace KoProperty {

static bool kofficeAppDirAdded = false;

//! \return true if \a o has parent \a par.
inline bool hasParent(QObject* par, QObject* o)
{
    while (o && o != par)
        o = o->parent();
    return o == par;
}

class EditorPrivate
{
	public:
		EditorPrivate()
		: itemDict(101, false), justClickedItem(false)
		{
			currentItem = 0;
			undoButton = 0;
			topItem = 0;
			if (!kofficeAppDirAdded) {
				kofficeAppDirAdded = true;
				KGlobal::iconLoader()->addAppDir("koffice");
			}
		}
		~EditorPrivate()
		{
		}

		QGuardedPtr<Set> set;
		//! widget cache for property types, widget will be deleted
		QMap<Property*, Widget* >  widgetCache;
		QGuardedPtr<Widget> currentWidget;
		EditorItem *currentItem;
		EditorItem *topItem; //! The top item is used to control the drawing of every branches.
		QPushButton *undoButton; //! "Revert to defaults" button
		EditorItem::Dict itemDict;

		int baseRowHeight;
		bool sync : 1;
		bool insideSlotValueChanged : 1;

		//! Helpers for changeSetLater()
		bool setListLater_set : 1;
		bool preservePrevSelection_preservePrevSelection : 1;
		bool doNotSetFocusOnSelection : 1;
		//! Used in setFocus() to prevent scrolling to previously selected item on mouse click
		bool justClickedItem : 1;
		//! Helper for changeSet()
		Set* setListLater_list;
};
}

using namespace KoProperty;

Editor::Editor(QWidget *parent, bool autoSync, const char *name)
 : KListView(parent, name)
{
	d = new EditorPrivate();
	d->itemDict.setAutoDelete(false);

	d->set = 0;
	d->topItem = 0;
	d->currentItem = 0;
	d->sync = autoSync;
	d->insideSlotValueChanged = false;
	d->setListLater_set = false;
	d->preservePrevSelection_preservePrevSelection = false;
	d->setListLater_list = 0;

	d->undoButton = new QPushButton(viewport());
	d->undoButton->setFocusPolicy(QWidget::NoFocus);
	setFocusPolicy(QWidget::ClickFocus);
	d->undoButton->setMinimumSize(QSize(5,5)); // allow to resize undoButton even below pixmap size
	d->undoButton->setPixmap(SmallIcon("undo"));
	QToolTip::add(d->undoButton, i18n("Undo changes"));
	d->undoButton->hide();
	connect(d->undoButton, SIGNAL(clicked()), this, SLOT(undo()));

	addColumn(i18n("Name"));
	addColumn(i18n("Value"));
	setAllColumnsShowFocus(true);
	setColumnWidthMode(0, QListView::Maximum);
	setFullWidth(true);
	setShowSortIndicator(false);
#if KDE_VERSION >= KDE_MAKE_VERSION(3,3,9)
	setShadeSortColumn(false);
#endif
	setTooltipColumn(0);
	setSorting(0);
	setItemMargin(KPROPEDITOR_ITEM_MARGIN);
	header()->setMovingEnabled( false );
	setTreeStepSize(16 + 2/*left*/ + 1/*right*/);

	d->baseRowHeight = QFontMetrics(font()).height() + itemMargin()*2;

	connect(this, SIGNAL(selectionChanged(QListViewItem *)), this, SLOT(slotClicked(QListViewItem *)));
	connect(this, SIGNAL(currentChanged(QListViewItem *)), this, SLOT(slotCurrentChanged(QListViewItem *)));
	connect(this, SIGNAL(expanded(QListViewItem *)), this, SLOT(slotExpanded(QListViewItem *)));
	connect(this, SIGNAL(collapsed(QListViewItem *)), this, SLOT(slotCollapsed(QListViewItem *)));
	connect(header(), SIGNAL(sizeChange(int, int, int)), this, SLOT(slotColumnSizeChanged(int, int, int)));
	connect(header(), SIGNAL(clicked(int)), this, SLOT(updateEditorGeometry()));
	connect(header(), SIGNAL(sectionHandleDoubleClicked (int)), this, SLOT(slotColumnSizeChanged(int)));
}

Editor::~Editor()
{
	clearWidgetCache();
	delete d;
}

void
Editor::fill()
{
	hideEditor();
	KListView::clear();
	d->itemDict.clear();
	if(!d->set)
		return;

	d->topItem = new EditorDummyItem(this);

	int i = 0;
	StringListMap map = d->set->groups();
	if(map.count() == 1) { // just one group (default one), so don't show groups

		QValueList<QCString> props = map.begin().data();
		QValueList<QCString>::ConstIterator it = props.constBegin();
		for( ; it != props.constEnd(); ++it)
			addItem(*it, d->topItem);

	} else { // else create a groupItem for each group

		StringListMap::ConstIterator it = map.constBegin();
		for( ; it != map.constEnd(); ++it) {
			EditorGroupItem *groupItem = 0;
			if(!it.key().isEmpty() && !it.data().isEmpty() && map.count() > 1)
				groupItem = new EditorGroupItem(d->topItem, d->set->groupDescription(it.key()) );

			QValueList<QCString>::ConstIterator it2 = it.data().constBegin();
			for( ; it2 != it.data().constEnd(); ++it2)
					addItem(*it2, groupItem);
		}

	}

	if (firstChild())
	{
		setCurrentItem(firstChild());
		setSelected(firstChild(), true);
		slotClicked(firstChild());
	}
}

void
Editor::addItem(const QCString &name, EditorItem *parent)
{
	if(!d->set || !d->set->contains(name))
		return;

	Property *property = &(d->set->property(name));
	if(!property || !property->isVisible()) {
		kdDebug() << "Property is not visible " << name << property->name() << endl;
		return;
	}
	QListViewItem *last = parent ? parent->firstChild() : d->topItem->firstChild();
	while(last && last->nextSibling())
		last = last->nextSibling();

	EditorItem *item=0;
	if(parent)
		item = new EditorItem(this, parent, property, last);
	else
		item = new EditorItem(this, d->topItem, property, last);
	d->itemDict.insert(name, item);

	// Create child items
	item->setOpen(true);
	if(!property->children())
		return;

	last = 0;
	QValueList<Property*>::ConstIterator endIt = property->children()->constEnd();
	for(QValueList<Property*>::ConstIterator it = property->children()->constBegin(); it != endIt; ++it)
			last = new EditorItem(this, item, *it, last);
}

void
Editor::changeSet(Set *set, bool preservePrevSelection)
{
	if (d->insideSlotValueChanged) {
		//setBuffer() called from inside of slotValueChanged()
		//this is dangerous, because there can be pending events,
		//especially for the GUI stuff, so let's do delayed work
		d->setListLater_list = set;
		d->preservePrevSelection_preservePrevSelection = preservePrevSelection;
		qApp->eventLoop()->processEvents(QEventLoop::AllEvents);
		if (!d->setListLater_set) {
			d->setListLater_set = true;
			QTimer::singleShot(10, this, SLOT(changeSetLater()));
		}
		return;
	}

	if (d->set) {
		slotWidgetAcceptInput(d->currentWidget);
		//store prev. selection for this buffer
		if (d->currentItem)
			d->set->setPrevSelection( d->currentItem->property()->name() );
		d->set->disconnect(this);
	}

	QCString selectedPropertyName1, selectedPropertyName2;
	if (preservePrevSelection) {
		//try to find prev. selection:
		//1. in new list's prev. selection
		if(set)
			selectedPropertyName1 = set->prevSelection();
		//2. in prev. list's current selection
		if(d->set)
			selectedPropertyName2 = d->set->prevSelection();
	}

	d->set = set;
	if (d->set) {
		//receive property changes
		connect(d->set, SIGNAL(propertyChanged(KoProperty::Set&, KoProperty::Property&)),
			this, SLOT(slotPropertyChanged(KoProperty::Set&, KoProperty::Property&)));
		connect(d->set, SIGNAL(propertyReset(KoProperty::Set&, KoProperty::Property&)),
			this, SLOT(slotPropertyReset(KoProperty::Set&, KoProperty::Property&)));
		connect(d->set,SIGNAL(aboutToBeCleared()), this, SLOT(slotSetWillBeCleared()));
		connect(d->set,SIGNAL(aboutToBeDeleted()), this, SLOT(slotSetWillBeDeleted()));
	}

	fill();
	if (d->set) {
		//select prev. selecteed item
		EditorItem * item = 0;
		if (!selectedPropertyName2.isEmpty()) //try other one for old buffer
			item = d->itemDict[selectedPropertyName2];
		if (!item && !selectedPropertyName1.isEmpty()) //try old one for current buffer
			item = d->itemDict[selectedPropertyName1];
		if (item) {
			d->doNotSetFocusOnSelection = !hasParent(this, focusWidget());
			setSelected(item, true);
			d->doNotSetFocusOnSelection = false;
			ensureItemVisible(item);
		}
	}

	emit propertySetChanged(d->set);
}

//! @internal
void
Editor::changeSetLater()
{
	d->setListLater_set = false;
	if (!d->setListLater_list)
		return;

	bool b = d->insideSlotValueChanged;
	d->insideSlotValueChanged = false;
	changeSet(d->setListLater_list, d->preservePrevSelection_preservePrevSelection);
	d->insideSlotValueChanged = b;
}

void
Editor::clear(bool editorOnly)
{
	hideEditor();

	if(!editorOnly) {
		clearWidgetCache();
		KListView::clear();
		d->itemDict.clear();
		d->topItem = 0;
		if(d->set)
			d->set->disconnect(this);
	}
}

void
Editor::undo()
{
	if(!d->currentWidget || !d->currentItem)
		return;

	int propertySync = d->currentWidget->property()->autoSync();
	bool sync = (propertySync != 0 && propertySync != 1) ?
				 d->sync : bool(propertySync);

	if(sync)
		d->currentItem->property()->resetValue();
	d->currentWidget->setValue( d->currentItem->property()->value());
	repaintItem(d->currentItem);
}

void
Editor::slotPropertyChanged(Set& set, Property& property)
{
	if(&set != d->set)
		return;

	if (d->currentItem && d->currentItem->property() == &property) {
		d->currentWidget->setValue(property.value(), false);
		for(QListViewItem *item = d->currentItem->firstChild(); item; item = item->nextSibling())
			repaintItem(item);
	}
	else  {
		EditorItem *item = d->itemDict[property.name()];
		// prop not in the dict, might be a child prop.
		if(!item && property.parent())
			item = d->itemDict[property.parent()->name()];
		if (item) {
			repaintItem(item);
			for(QListViewItem *it = item->firstChild(); it; it = it->nextSibling())
				repaintItem(it);
		}
	}

	showUndoButton( property.isModified() );
}

void
Editor::slotPropertyReset(Set& set, Property& property)
{
	if(&set != d->set)
		return;

	if (d->currentItem && d->currentItem->property() == &property) {
		d->currentWidget->setValue(property.value(), false);
		for(QListViewItem *item = d->currentItem->firstChild(); item; item = item->nextSibling())
			repaintItem(item);
	}
	else  {
		EditorItem *item = d->itemDict[property.name()];
		// prop not in the dict, might be a child prop.
		if(!item && property.parent())
			item = d->itemDict[property.parent()->name()];
		repaintItem(item);
		for(QListViewItem *it = item->firstChild(); it; it = it->nextSibling())
			repaintItem(it);
	}

	showUndoButton( false );
}

void
Editor::slotWidgetValueChanged(Widget *widget)
{
	if(!widget || !d->set)
		return;
	d->insideSlotValueChanged = true;

	QVariant value = widget->value();
	int propertySync = widget->property()->autoSync();
	bool sync = (propertySync != 0 && propertySync != 1) ?
				 d->sync : bool(propertySync);

	if(sync) {
		widget->property()->setValue(value);
		showUndoButton( widget->property()->isModified() );
	}

	d->insideSlotValueChanged = false;
}

void
Editor::acceptInput()
{
	slotWidgetAcceptInput(d->currentWidget);
}

void
Editor::slotWidgetAcceptInput(Widget *widget)
{
	if(!widget || !d->set || !widget->property())
		return;

	widget->property()->setValue(widget->value());
}

void
Editor::slotWidgetRejectInput(Widget *widget)
{
	if(!widget || !d->set)
		return;

	undo();
}

void
Editor::slotClicked(QListViewItem *it)
{
	hideEditor();
	if(!it)
		return;

	EditorItem *item = static_cast<EditorItem*>(it);
	Property *p = item ? item->property() : 0;
	if(!p)
		return;

	d->currentItem = item;
	d->currentWidget = createWidgetForProperty(p);
	d->currentWidget->setFocus();

	updateEditorGeometry();
	showUndoButton( p->isModified() );
	d->currentWidget->show();

	d->justClickedItem = true;
}

void
Editor::slotCurrentChanged(QListViewItem *item)
{
	if (item == firstChild()) {
		QListViewItem *oldItem = item;
		while (item && (!item->isSelectable() || !item->isVisible()))
			item = item->itemBelow();
		if (item && item != oldItem) {
			setSelected(item,true);
			return;
		}
	}
}

void
Editor::slotSetWillBeCleared()
{
	if (d->currentWidget) {
		acceptInput();
		d->currentWidget->setProperty(0);
	}
	clear();
}

void
Editor::slotSetWillBeDeleted()
{
	clear();
	d->set = 0;
}

Widget*
Editor::createWidgetForProperty(Property *property, bool changeWidgetProperty)
{
	int type = property->type();
	Widget *widget = 0;

	if(d->widgetCache.contains(property) )
		widget = d->widgetCache[property];
	else {
		widget = Factory::getInstance()->widgetForProperty(property);
		d->widgetCache[property] = widget;
		widget->setProperty(0); // to force reloading property later
		widget->hide();
		connect(widget, SIGNAL(valueChanged(Widget*)),
			this, SLOT(slotWidgetValueChanged(Widget*)) );
		connect(widget, SIGNAL(acceptInput(Widget*)),
			this, SLOT(slotWidgetAcceptInput(Widget*)) );
		connect(widget, SIGNAL(rejectInput(Widget*)),
			this, SLOT(slotWidgetRejectInput(Widget*)) );
	}
	if(!widget->property() || changeWidgetProperty)
		widget->setProperty(property);

	if (!d->doNotSetFocusOnSelection)
		widget->setFocus();
	return widget;
}


void
Editor::clearWidgetCache()
{
	for(QMap<Property*, Widget*>::iterator it = d->widgetCache.begin(); it != d->widgetCache.end(); ++it)
		delete it.data();
	d->widgetCache.clear();
}

void
Editor::updateEditorGeometry(bool forceUndoButtonSettings, bool undoButtonVisible)
{
	if(!d->currentItem || !d->currentWidget)
		return;

	int placeForUndoButton;
	if (forceUndoButtonSettings ? undoButtonVisible : d->undoButton->isVisible())
		placeForUndoButton = d->undoButton->width();
	else
		placeForUndoButton = d->currentWidget->leavesTheSpaceForRevertButton() ? d->undoButton->width() : 0;

	QRect r;
	int y = itemPos(d->currentItem);
	r.setX(header()->sectionPos(1)-(d->currentWidget->hasBorders()?1:0)); //-1, to align to horizontal line
	r.setY(y-(d->currentWidget->hasBorders()?1:0));
	r.setWidth(header()->sectionSize(1)+(d->currentWidget->hasBorders()?1:0) //+1 because we subtracted 1 from X
		- placeForUndoButton);
	r.setHeight(d->currentItem->height()+(d->currentWidget->hasBorders()?1:-1));

	// check if the column is fully visible
	if (visibleWidth() < r.right())
		r.setRight(visibleWidth());

	moveChild(d->currentWidget, r.x(), r.y());
	d->currentWidget->resize(r.size());
}

void
Editor::hideEditor()
{
	d->currentItem = 0;
	QWidget *cw = d->currentWidget;
	if(cw) {
		d->currentWidget = 0;
		cw->hide();
	}
	d->undoButton->hide();
}

void
Editor::showUndoButton( bool show )
{
	if (!d->currentItem)
		return;

	int y = viewportToContents(QPoint(0, itemRect(d->currentItem).y())).y();
	QRect geometry(columnWidth(0), y, columnWidth(1) + 1, d->currentItem->height());
	d->undoButton->resize(d->baseRowHeight, d->baseRowHeight);

	updateEditorGeometry(true, show);

	if (!show) {
/*	  if (d->currentWidget) {
			if (d->currentWidget->leavesTheSpaceForRevertButton()) {
				geometry.setWidth(geometry.width()-d->undoButton->width());
			}
			d->currentWidget->resize(geometry.width(), geometry.height());
		}
		d->undoButton->hide();*/
		return;
	}

	QPoint p = contentsToViewport(QPoint(0, geometry.y()));
	d->undoButton->move(geometry.x() + geometry.width() -(d->currentWidget->hasBorders()?1:0)/*editor is moved by 1 to left*/
		- d->undoButton->width(), p.y());
//  if (d->currentWidget) {
//	  d->currentWidget->move(d->currentWidget->x(), p.y());
//	  d->currentWidget->resize(geometry.width()-d->undoButton->width(), geometry.height());
//  }
	d->undoButton->show();
}

void
Editor::slotExpanded(QListViewItem *item)
{
	if (!item)
		return;
	updateEditorGeometry();
}

void
Editor::slotCollapsed(QListViewItem *item)
{
	if (!item)
		return;
	updateEditorGeometry();
}

void
Editor::slotColumnSizeChanged(int section, int, int newS)
{
	updateEditorGeometry();
/*
	if(d->currentWidget) {
		if(section == 0)
			d->currentWidget->move(newS, d->currentWidget->y());
		else  {
			if(d->undoButton->isVisible())
				d->currentWidget->resize(newS - d->undoButton->width(), d->currentWidget->height());
			else
				d->currentWidget->resize(
					newS-(d->currentWidget->leavesTheSpaceForRevertButton() ? d->undoButton->width() : 0),
					d->currentWidget->height());
		}
	}*/
}

void
Editor::slotColumnSizeChanged(int section)
{
	setColumnWidth(1, viewport()->width() - columnWidth(0));
	slotColumnSizeChanged(section, 0, header()->sectionSize(section));

/*  if(d->currentWidget) {
		if(d->undoButton->isVisible())
			d->currentWidget->resize(columnWidth(1) - d->undoButton->width(), d->currentWidget->height());
		else
			d->currentWidget->resize(
				columnWidth(1)-(d->currentWidget->leavesTheSpaceForRevertButton() ? d->undoButton->width() : 0),
				d->currentWidget->height());
	}*/
	if(d->undoButton->isVisible())
		showUndoButton(true);
	else
		updateEditorGeometry();
}

QSize
Editor::sizeHint() const
{
	return QSize( QFontMetrics(font()).width(columnText(0)+columnText(1)+"   "),
		KListView::sizeHint().height());
}

void
Editor::setFocus()
{
	EditorItem *item = static_cast<EditorItem *>(selectedItem());
	if (item) {
		if (!d->justClickedItem)
			ensureItemVisible(item);
		d->justClickedItem = false;
	}
	else {
		//select an item before focusing
		item = static_cast<EditorItem *>(itemAt(QPoint(10,1)));
		if (item) {
			ensureItemVisible(item);
			setSelected(item, true);
		}
	}
	if (d->currentWidget)
		d->currentWidget->setFocus();
	else
		KListView::setFocus();
}

void
Editor::resizeEvent(QResizeEvent *ev)
{
	KListView::resizeEvent(ev);
	if(d->undoButton->isVisible())
		showUndoButton(true);
}

bool
Editor::handleKeyPress(QKeyEvent* ev)
{
	const int k = ev->key();
	const Qt::ButtonState s = ev->state();

	//selection moving
	QListViewItem *item = 0;

	if ( ((s == NoButton) && (k == Key_Up)) || (k==Key_BackTab) ) {
		//find prev visible
		item = selectedItem() ? selectedItem()->itemAbove() : 0;
		while (item && (!item->isSelectable() || !item->isVisible()))
			item = item->itemAbove();
		if (!item)
			return true;
	}
	else if( (s == NoButton) && ((k == Key_Down) || (k == Key_Tab)) ) {
		//find next visible
		item = selectedItem() ? selectedItem()->itemBelow() : 0;
		while (item && (!item->isSelectable() || !item->isVisible()))
			item = item->itemBelow();
		if (!item)
			return true;
	}
	else if( (s==NoButton) && (k==Key_Home) ) {
		if (d->currentWidget && d->currentWidget->hasFocus())
			return false;
		//find 1st visible
		item = firstChild();
		while (item && (!item->isSelectable() || !item->isVisible()))
			item = item->itemBelow();
	}
	else if( (s==NoButton) && (k==Key_End) ) {
		if (d->currentWidget && d->currentWidget->hasFocus())
			return false;
		//find last visible
		item = selectedItem();
		QListViewItem *lastVisible = item;
		while (item) { // && (!item->isSelectable() || !item->isVisible()))
			item = item->itemBelow();
			if (item && item->isSelectable() && item->isVisible())
				lastVisible = item;
		}
		item = lastVisible;
	}
	if(item) {
		ev->accept();
		ensureItemVisible(item);
		setSelected(item, true);
		return true;
	}
	return false;
}

#include "editor.moc"

