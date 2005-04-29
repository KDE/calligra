/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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

#include <kdebug.h>

#include <qworkspace.h>
#include <qcursor.h>
#include <qstring.h>
#include <qlabel.h>
#include <qobjectlist.h>
#include <qstylefactory.h>
#include <qmetaobject.h>
#include <qregexp.h>
#include <qvaluevector.h>
#include <qvbox.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kpopupmenu.h>
#include <kstdaction.h>
#include <kaction.h>
#include <kxmlguiclient.h>
#include <kmainwindow.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <kstyle.h>
#include <kactionclasses.h>
#include <kapplication.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kdialogbase.h>
#include <ktextedit.h>
#include <ktabwidget.h>

#include <kdeversion.h>
#if KDE_VERSION >= KDE_MAKE_VERSION(3,1,9) && !defined(Q_WS_WIN)
# include <kactioncollection.h>
#endif

#include "kexipropertyeditor.h"
#include "objpropbuffer.h"
#include "objecttree.h"
#include "widgetlibrary.h"
#include "form.h"
#include "container.h"
#include "formIO.h"
#include "objecttreeview.h"
#include "commands.h"
#include "tabstopdialog.h"
#include "connectiondialog.h"
#include "pixmapcollection.h"
#include "events.h"
#include "utils.h"

#include "formmanager.h"

using namespace KFormDesigner;

FormManager::FormManager(QObject *parent,
	const QStringList& supportedFactoryGroups, int options, const char *name)
   : QObject(parent, name)
#ifdef KEXI_SHOW_DEBUG_ACTIONS
   , m_uiCodeDialog(0)
   , m_options(options)
#endif
{
	connect( kapp, SIGNAL( settingsChanged(int) ), SLOT( slotSettingsChanged(int) ) );
	slotSettingsChanged(KApplication::SETTINGS_SHORTCUTS);

	m_lib = new WidgetLibrary(this, supportedFactoryGroups);
	m_buffer = new ObjectPropertyBuffer(this, this, "buffer");

	m_editor = 0;
	m_active = 0;
	m_inserting = false;
	m_drawingSlot = false;
	m_collection = 0;
	m_connection = 0;

	m_domDoc.appendChild(m_domDoc.createElement("UI"));

	m_popup = 0;

	m_treeview = 0;
	m_editor = 0;

	m_deleteWidgetLater_list.setAutoDelete(true);
	connect( &m_deleteWidgetLater_timer, SIGNAL(timeout()), this, SLOT(deleteWidgetLaterTimeout()));
	connect( this, SIGNAL(connectionCreated(KFormDesigner::Form*, KFormDesigner::Connection&)), 
		this, SLOT(slotConnectionCreated(KFormDesigner::Form*, KFormDesigner::Connection&)));
}

FormManager::~FormManager()
{
	delete m_popup;
	delete m_connection;
#ifdef KEXI_SHOW_DEBUG_ACTIONS
	delete m_uiCodeDialog;
#endif
}

void
FormManager::setEditors(KexiPropertyEditor *editor, ObjectTreeView *treeview)
{
	m_editor = editor;
	m_treeview = treeview;

	if(editor)
		editor->setBuffer(m_buffer);

	if(treeview)
		connect(m_buffer, SIGNAL(nameChanged(const QString&, const QString&)), treeview, SLOT(renameItem(const QString&, const QString&)));
}

ActionList
FormManager::createActions(KActionCollection *parent)
{
	m_collection = parent;

	ActionList actions = m_lib->addCreateWidgetActions(parent, this, SLOT(insertWidget(const QCString &)));

	if (m_options & HideSignalSlotConnections)
		m_dragConnection = 0;
	else {
		m_dragConnection = new KToggleAction(i18n("Connect Signals/Slots"), 
			"signalslot", KShortcut(0), this, SLOT(startCreatingConnection()), parent,
			"drag_connection");
		//to be exclusive with any 'widget' action
		m_dragConnection->setExclusiveGroup("LibActionWidgets"); 
		m_dragConnection->setChecked(false);
		actions.append(m_dragConnection);
	}

	m_pointer = new KToggleAction(i18n("Pointer"), "mouse_pointer", KShortcut(0), this, SLOT(slotPointerClicked()), parent, "pointer");
	m_pointer->setExclusiveGroup("LibActionWidgets"); //to be exclusive with any 'widget' action
	m_pointer->setChecked(true);
	actions.append(m_pointer);

	m_snapToGrid = new KToggleAction(i18n("Snap to Grid"), QString::null, KShortcut(0), 0, 0, parent, "snap_to_grid");
	m_snapToGrid->setChecked(true);
	actions.append(m_snapToGrid);

	// Create the Style selection action (with a combo box in toolbar and submenu items)
	KSelectAction *m_style = new KSelectAction( i18n("Style"), CTRL + Key_S, this, SLOT(slotStyle()), parent, "change_style");
	m_style->setEditable(false);

	KGlobal::config()->setGroup("General");
	QString currentStyle = QString::fromLatin1(kapp->style().name()).lower();
	const QStringList styles = QStyleFactory::keys();
	m_style->setItems(styles);
	m_style->setCurrentItem(0);

	QStringList::ConstIterator endIt = styles.constEnd();
	int idx = 0;
	for (QStringList::ConstIterator it = styles.constBegin(); it != endIt; ++it, ++idx)
	{
		if ((*it).lower() == currentStyle) {
			m_style->setCurrentItem(idx);
			break;
		}
	}

	m_style->setToolTip(i18n("Set the current view style."));
	m_style->setMenuAccelsEnabled(true);
	actions.append(m_style);

	m_lib->addCustomWidgetActions( parent );

	return actions;
}

bool
FormManager::isPasteEnabled()
{
	return m_domDoc.namedItem("UI").hasChildNodes();
}

void
FormManager::undo()
{
	if(!activeForm() || !activeForm()->objectTree())
		return;

	activeForm()->commandHistory()->undo();
}

void
FormManager::redo()
{
	if(!activeForm() || !activeForm()->objectTree())
		return;

	activeForm()->commandHistory()->redo();
}

void
FormManager::insertWidget(const QCString &classname)
{
	if(m_drawingSlot)
		stopCreatingConnection();

	Form *form;
	for(form = m_forms.first(); form; form = m_forms.next())
	{
		form->d->cursors = new QMap<QString, QCursor>();
		if (form->toplevelContainer())
			form->widget()->setCursor(QCursor(CrossCursor));
		QObjectList *l = form->widget()->queryList( "QWidget" );
		for(QObject *o = l->first(); o; o = l->next())
		{
			if( ((QWidget*)o)->ownCursor() )
			{
				form->d->cursors->insert(o->name(), ((QWidget*)o)->cursor());
				((QWidget*)o)->setCursor(QCursor(Qt::CrossCursor));
			}

		}
		delete l;
	}

	m_inserting = true;
	m_insertClass = classname;
	m_pointer->setChecked(false);
}

void
FormManager::stopInsert()
{
	if(m_drawingSlot)
		stopCreatingConnection();
	if(!m_inserting)
		return;

	Form *form;
	for(form = m_forms.first(); form; form = m_forms.next())
	{
		form->widget()->unsetCursor();
		QObjectList *l = form->widget()->queryList( "QWidget" );
		for(QObject *o = l->first(); o; o = l->next())
		{
			if( ((QWidget*)o)->ownCursor())
				((QWidget*)o)->setCursor( (*(form->d->cursors))[o->name()] ) ;
		}
		delete l;
		delete (form->d->cursors);
		form->d->cursors = 0;
	}
	m_inserting = false;
	m_pointer->setChecked(true);
}

void
FormManager::slotPointerClicked()
{
	if(m_inserting)
		stopInsert();
	else if(m_dragConnection)
		stopCreatingConnection();
}

void
FormManager::startCreatingConnection()
{
	if(m_inserting)
		stopInsert();

	// We set a Pointing hand cursor while drawing the connection
	Form *form;
	for(form = m_forms.first(); form; form = m_forms.next())
	{
		form->d->cursors = new QMap<QString, QCursor>();
		form->d->mouseTrackers = new QStringList();
		if (form->toplevelContainer())
		{
			form->widget()->setCursor(QCursor(PointingHandCursor));
			form->widget()->setMouseTracking(true);
		}
		QObjectList *l = form->widget()->queryList( "QWidget" );
		for(QObject *o = l->first(); o; o = l->next())
		{
			QWidget *w = (QWidget*)o;
			if( w->ownCursor() )
			{
				form->d->cursors->insert(w->name(), w->cursor());
				w->setCursor(QCursor(PointingHandCursor ));
			}
			if(w->hasMouseTracking())
				form->d->mouseTrackers->append(w->name());
			w->setMouseTracking(true);
		}
		delete l;
	}
	delete m_connection;
	m_connection = new Connection();
	m_drawingSlot = true;
	if (m_dragConnection)
		m_dragConnection->setChecked(true);
}

void
FormManager::resetCreatedConnection()
{
	delete m_connection;
	m_connection = new Connection();

	if(m_active && m_active->formWidget())
		m_active->formWidget()->clearForm();
	m_active->widget()->repaint();
}

void
FormManager::stopCreatingConnection()
{
	if(!m_drawingSlot)
		return;

	if(m_active && m_active->formWidget())
		m_active->formWidget()->clearForm();

	Form *form;
	for(form = m_forms.first(); form; form = m_forms.next())
	{
		form->widget()->unsetCursor();
		form->widget()->setMouseTracking(false);
		QObjectList *l = form->widget()->queryList( "QWidget" );
		for(QObject *o = l->first(); o; o = l->next())
		{
			QWidget *w = (QWidget*)o;
			if( w->ownCursor())
				w->setCursor( (*(form->d->cursors))[o->name()] ) ;
			w->setMouseTracking( !form->d->mouseTrackers->grep(w->name()).isEmpty() );
		}
		delete l;
		delete (form->d->cursors);
		form->d->cursors = 0;
		delete (form->d->mouseTrackers);
		form->d->mouseTrackers = 0;
	}

	if(m_connection->slot().isNull())
		emit connectionAborted(activeForm());
	delete m_connection;
	m_connection = 0;
	m_drawingSlot = false;
	m_pointer->setChecked(true);
}

bool
FormManager::snapWidgetsToGrid()
{
	return m_snapToGrid->isChecked();
}

void
FormManager::windowChanged(QWidget *w)
{
	if(!w)
	{
		m_active = 0;
		if(m_treeview)
			m_treeview->setForm(0);
		showPropertyBuffer(0);
		if(isCreatingConnection())
			stopCreatingConnection();

		emitNoFormSelected();
		return;
	}

	Form *previousActive = m_active;
	Form *form;
	for(form = m_forms.first(); form; form = m_forms.next())
	{
		if(form->toplevelContainer() && form->widget() == w)
		{
			if(m_treeview)
				m_treeview->setForm(form);
			if(m_buffer)
				m_buffer->setCollection(form->pixmapCollection());

			kdDebug() << "FormManager::windowChanged() active form is " << form->objectTree()->name() << endl;

			if(m_collection)
			{
				// update the 'style' action
				KSelectAction *m_style = (KSelectAction*)m_collection->action("change_style", "KSelectAction");
				const QString currentStyle = form->widget()->style().name();
				const QStringList styles = m_style->items();

				int idx = 0;
				QStringList::ConstIterator endIt = styles.constEnd();
				for (QStringList::ConstIterator it = styles.constBegin(); it != endIt; ++it, ++idx)
				{
					if ((*it).lower() == currentStyle) {
						kdDebug() << "Updating the style to " << currentStyle << endl;
						m_style->setCurrentItem(idx);
						break;
					}
				}
			}

			if((form != previousActive) && isCreatingConnection())
				resetCreatedConnection();

			m_active = form;

			emit  dirty(form, form->isModified());
			// update actions state
			m_active->emitActionSignals();
			//uodate the buffer too
			form->emitSelectionSignals();
			return;
		}
	}

	for(form = m_preview.first(); form; form = m_preview.next())
	{
		kdDebug() << "FormManager::windowChanged() active preview form is " << form->widget()->name() << endl;

		if(m_collection)
		{
			// update the 'style' action
			KSelectAction *m_style = (KSelectAction*)m_collection->action("change_style", "KSelectAction");
			const QString currentStyle = form->widget()->style().name();
			const QStringList styles = m_style->items();

			int idx = 0;
			QStringList::ConstIterator endIt = styles.constEnd();
			for (QStringList::ConstIterator it = styles.constBegin(); it != endIt; ++it, ++idx)
			{
				if ((*it).lower() == currentStyle) {
					kdDebug() << "Updating the style to " << currentStyle << endl;
					m_style->setCurrentItem(idx);
					break;
				}
			}

			resetCreatedConnection();
			m_active = form;

			emit dirty(form, false);
			emitNoFormSelected();
			showPropertyBuffer(0);
		}
	}
	//m_active = 0;
}

Form*
FormManager::activeForm() const
{
	return m_active;
}

Form*
FormManager::formForWidget(QWidget *w)
{
	for(Form *form = m_forms.first(); form; form = m_forms.next())  {
		if(form->toplevelContainer() && form->widget() == w)
			return form;
	}

	return 0; // not one of toplevel widgets
}

void
FormManager::deleteForm(Form *form)
{
	if (!form)
		return;
	if(m_forms.find(form) == -1)
		m_preview.remove(form);
	else
		m_forms.remove(form);

	if(m_forms.count() == 0) {
		m_active = 0;
		showPropertyBuffer(0);
	}
}

void
FormManager::importForm(Form *form, bool preview)
{
	if(!preview)
		initForm(form);
	else
	{
		m_preview.append(form);
		form->setDesignMode(false);
	}
}

void
FormManager::initForm(Form *form)
{
	m_forms.append(form);

	if(m_treeview)
		m_treeview->setForm(form);

	m_active = form;

	connect(form, SIGNAL(selectionChanged(QWidget*, bool)), m_buffer, SLOT(setSelectedWidget(QWidget*, bool)));
	if(m_treeview)
	{
		connect(form, SIGNAL(selectionChanged(QWidget*, bool)), m_treeview, SLOT(setSelectedWidget(QWidget*, bool)));
		connect(form, SIGNAL(childAdded(ObjectTreeItem* )), m_treeview, SLOT(addItem(ObjectTreeItem*)));
		connect(form, SIGNAL(childRemoved(ObjectTreeItem* )), m_treeview, SLOT(removeItem(ObjectTreeItem*)));
	}
	connect(m_buffer, SIGNAL(nameChanged(const QString&, const QString&)), form, SLOT(changeName(const QString&, const QString&)));

	form->setSelectedWidget(form->widget());
	windowChanged(form->widget());
}

void
FormManager::previewForm(Form *form, QWidget *container, Form *toForm)
{
	if(!form || !container || !form->objectTree())
		return;
	QDomDocument domDoc;
	FormIO::saveFormToDom(form, domDoc);

	Form *myform;
	if(!toForm)
		myform = new Form(this, form->objectTree()->name().latin1());
	else
		myform = toForm;
	myform->createToplevel(container);
	container->setStyle( &(form->widget()->style()) );
	FormIO::loadFormFromDom(myform, container, domDoc);

	myform->setDesignMode(false);
	m_preview.append(myform);
	container->show();
}

bool
FormManager::isTopLevel(QWidget *w)
{
	if(!activeForm() || !activeForm()->objectTree())
		return false;

	kdDebug() << "FormManager::isTopLevel(): for: " << w->name() << " = "
		<< activeForm()->objectTree()->lookup(w->name())<< endl;

	ObjectTreeItem *item = activeForm()->objectTree()->lookup(w->name());
	if(!item)
		return true;

	return (!item->parent());
}

void
FormManager::deleteWidget()
{
	if(!activeForm() || !activeForm()->objectTree())
		return;

	QPtrList<QWidget> *list = activeForm()->selectedWidgets();
	if(list->isEmpty())
		return;

	if (activeForm()->widget() == list->first()) {
		//toplevel form is selected, cannot delete it
		return;
	}

	KCommand *com = new DeleteWidgetCommand(*list, activeForm());
	activeForm()->addCommand(com, true);
}

void
FormManager::copyWidget()
{
	if (!activeForm() || !activeForm()->objectTree())
		return;

	QPtrList<QWidget> *list = activeForm()->selectedWidgets();
	if(list->isEmpty())
		return;

	removeChildrenFromList(*list);

	// We clear the current clipboard
	m_domDoc.setContent(QString(), true);
	QDomElement parent = m_domDoc.createElement("UI");
	m_domDoc.appendChild(parent);

	QWidget *w;
	for(w = list->first(); w; w = list->next())
	{
		ObjectTreeItem *it = activeForm()->objectTree()->lookup(w->name());
		if (!it)
			continue;

		FormIO::saveWidget(it, parent, m_domDoc);
	}

	FormIO::cleanClipboard(parent);

	activeForm()->emitActionSignals(); // to update 'Paste' item state
}

void
FormManager::cutWidget()
{
	if(!activeForm() || !activeForm()->objectTree())
		return;

	QPtrList<QWidget> *list = activeForm()->selectedWidgets();
	if(list->isEmpty())
		return;

	KCommand *com = new CutWidgetCommand(*list, activeForm());
	activeForm()->addCommand(com, true);
}

void
FormManager::pasteWidget()
{
	if(!m_domDoc.namedItem("UI").hasChildNodes())
		return;
	if(!activeForm() || !activeForm()->objectTree())
		return;

	KCommand *com = new PasteWidgetCommand(m_domDoc, activeForm()->activeContainer(), m_insertPoint);
	activeForm()->addCommand(com, true);
}

void
FormManager::setInsertPoint(const QPoint &p)
{
	m_insertPoint = p;
}

void
FormManager::createSignalMenu(QWidget *w)
{
	m_sigSlotMenu = new KPopupMenu();
	m_sigSlotMenu->insertTitle(SmallIcon("connection"), i18n("Signals"));

	QStrList list = w->metaObject()->signalNames(true);
	QStrListIterator it(list);
	for(; it.current() != 0; ++it)
		m_sigSlotMenu->insertItem(*it);

	int result = m_sigSlotMenu->exec(QCursor::pos());
	if(result == -1)
		resetCreatedConnection();
	else
		menuSignalChoosed(result);

	delete m_sigSlotMenu;
	m_sigSlotMenu = 0;
}

void
FormManager::createSlotMenu(QWidget *w)
{
	m_sigSlotMenu = new KPopupMenu();
	m_sigSlotMenu->insertTitle(SmallIcon("connection"), i18n("Slots"));

	QString signalArg( m_connection->signal().remove( QRegExp(".*[(]|[)]") ) );

	QStrList list = w->metaObject()->slotNames(true);
	QStrListIterator it(list);
	for(; it.current() != 0; ++it)
	{
		// we add the slot only if it is compatible with the signal
		QString slotArg(*it);
		slotArg = slotArg.remove( QRegExp(".*[(]|[)]") );
		if(!signalArg.startsWith(slotArg, true)) // args not compatible
			continue;

		m_sigSlotMenu->insertItem(*it);
	}

	int result = m_sigSlotMenu->exec(QCursor::pos());
	if(result == -1)
		resetCreatedConnection();
	else
		menuSignalChoosed(result);

	delete m_sigSlotMenu;
	m_sigSlotMenu = 0;
}

void
FormManager::createContextMenu(QWidget *w, Container *container, bool popupAtCursor)
{
	if(!activeForm() || !activeForm()->widget())
		return;
	const bool toplevelWidgetSelected = activeForm()->widget() == w;
	const uint widgetsCount = container->form()->selectedWidgets()->count();
	const bool multiple = widgetsCount > 1;
	const bool enableRemove = w != m_active->widget();
	// We only enablelayout creation if more than one widget with the same parent are selected
	const bool enableLayout = multiple || w == container->widget();

	m_menuWidget = w;
	QString n = m_lib->displayName(w->className());
//	QValueVector<int> menuIds();

	if (!m_popup) {
		m_popup = new KPopupMenu();
	}
	else {
		m_popup->clear();
	}

	//set title
	if(!multiple)
	{
		if(w == container->form()->widget())
			m_popup->insertTitle(SmallIcon("form"), i18n("Form: ") + w->name());
		else
			m_popup->insertTitle(SmallIcon(m_lib->icon(w->className())), n + ": " + w->name() );
	}
	else
		m_popup->insertTitle(SmallIcon("multiple_obj"), i18n("Multiple Widgets") 
		+ QString(" (%1)").arg(widgetsCount));

	KAction *a;
#define PLUG_ACTION(_name, forceVisible) \
	{ a = action(_name); \
	if (a && (forceVisible || a->isEnabled())) { \
		if (separatorNeeded) \
			m_popup->insertSeparator(); \
		separatorNeeded = false; \
		a->plug(m_popup); \
	} \
	}

	bool separatorNeeded = false;

	PLUG_ACTION("edit_cut", !toplevelWidgetSelected);
	PLUG_ACTION("edit_copy", !toplevelWidgetSelected);
	PLUG_ACTION("edit_paste", true);
	PLUG_ACTION("edit_delete", !toplevelWidgetSelected);
	separatorNeeded = true;
	PLUG_ACTION("layout_menu", enableLayout);
	PLUG_ACTION("break_layout", enableLayout);
	separatorNeeded = true;
	PLUG_ACTION("align_menu", !toplevelWidgetSelected);
	PLUG_ACTION("adjust_size_menu", !toplevelWidgetSelected);
	separatorNeeded = true;

	// We create the buddy menu
	if(!multiple && w->inherits("QLabel") && ((QLabel*)w)->text().contains("&") && (((QLabel*)w)->textFormat() != RichText))
	{
		if (separatorNeeded)
			m_popup->insertSeparator();

		KPopupMenu *sub = new KPopupMenu(w);
		QWidget *buddy = ((QLabel*)w)->buddy();

		sub->insertItem(i18n("No Buddy"), MenuNoBuddy);
		if(!buddy)
			sub->setItemChecked(MenuNoBuddy, true);
		sub->insertSeparator();

		// add all the widgets that can have focus
		for(ObjectTreeListIterator it( container->form()->tabStopsIterator() ); it.current(); ++it)
		{
			int index = sub->insertItem( SmallIcon(m_lib->icon(it.current()->className().latin1())),
				it.current()->name());
			if(it.current()->widget() == buddy)
				sub->setItemChecked(index, true);
		}

		int id = m_popup->insertItem(i18n("Choose Buddy..."), sub);
//		menuIds->append(id);
		connect(sub, SIGNAL(activated(int)), this, SLOT(buddyChoosed(int)));

		separatorNeeded = true;
	}

	//int sigid=0;
#ifdef KEXI_SHOW_DEBUG_ACTIONS
	if(!multiple && !(m_options & HideEventsInPopupMenu))
	{
		if (separatorNeeded)
			m_popup->insertSeparator();

		// We create the signals menu
		KPopupMenu *sigMenu = new KPopupMenu();
		QStrList list = w->metaObject()->signalNames(true);
		QStrListIterator it(list);
		for(; it.current() != 0; ++it)
			sigMenu->insertItem(*it);

		int id = m_popup->insertItem(SmallIconSet(""), i18n("Events"), sigMenu);
//		menuIds->append(id);
		if(list.isEmpty())
			m_popup->setItemEnabled(id, false);
		connect(sigMenu, SIGNAL(activated(int)), this, SLOT(menuSignalChoosed(int)));
		separatorNeeded = true;
	}
#endif

	// Other items
	if(!multiple)
	{
		int lastID = -1;
		if (separatorNeeded) {
			lastID = m_popup->insertSeparator();
		}
		const int oldIndex = m_popup->count()-1;
		m_lib->createMenuActions(w->className(), w, m_popup, container);
		if (oldIndex == (m_popup->count()-1)) {
//			for (uint i=oldIndex; i<m_popup->count(); i++) {
//				int id = m_popup->idAt( i );
//				if (id!=-1)
//					menuIds->append( id );
//			}
			//nothing added
			if (separatorNeeded) {
				m_popup->removeItem( lastID );
//				menuIds->pop_back();
			}
		}
	}
	
	//show the popup at the selected widget
	QPoint popupPos;
	if (popupAtCursor) {
		popupPos = QCursor::pos();
	}
	else {
		WidgetList *lst = container->form()->selectedWidgets();
		QWidget * sel_w = lst ? lst->first() : container->form()->selectedWidget();
		popupPos = sel_w ? sel_w->mapToGlobal(QPoint(sel_w->width()/2, sel_w->height()/2)) : QCursor::pos();
	}
	m_insertPoint = container->widget()->mapFromGlobal(popupPos);
	m_popup->exec(popupPos);//QCursor::pos());
	m_insertPoint = QPoint();

//	QValueVector<int>::iterator it;
//	for(it = menuIds->begin(); it != menuIds->end(); ++it)
//		m_popup->removeItem(*it);
}

void
FormManager::buddyChoosed(int id)
{
	if(!m_menuWidget)
		return;
	QLabel *label = static_cast<QLabel*>((QWidget*)m_menuWidget);

	if(id == MenuNoBuddy)
	{
		label->setBuddy(0);
		return;
	}

	ObjectTreeItem *item = activeForm()->objectTree()->lookup(m_popup->text(id));
	if(!item || !item->widget())
		return;
	label->setBuddy(item->widget());
}

void
FormManager::menuSignalChoosed(int id)
{
	//if(!m_menuWidget)
	//	return;
	if(m_drawingSlot && m_sigSlotMenu)
	{
		if( m_connection->receiver().isNull() )
			m_connection->setSignal(m_sigSlotMenu->text(id));
		else
		{
			m_connection->setSlot(m_sigSlotMenu->text(id));
			kdDebug() << "Finished creating the connection: sender=" << m_connection->sender() << "; signal=" << m_connection->signal() <<
			  "; receiver=" << m_connection->receiver() << "; slot=" << m_connection->slot() << endl;
			emit connectionCreated(activeForm(), *m_connection);
			stopCreatingConnection();
		}
	}
	else if(m_menuWidget)
		emit(createFormSlot(m_active, m_menuWidget->name(), m_popup->text(id)));
}

void
FormManager::slotConnectionCreated(Form *form, Connection &connection)
{
	if(!form)
		return;

	Connection *c = new Connection(connection);
	form->connectionBuffer()->append(c);

}

void
FormManager::layoutHBox()
{
	createLayout(Container::HBox);
}

void
FormManager::layoutVBox()
{
	createLayout(Container::VBox);
}

void
FormManager::layoutGrid()
{
	createLayout(Container::Grid);
}

void
FormManager::layoutHSplitter()
{
	createLayout(Container::HSplitter);
}

void
FormManager::layoutVSplitter()
{
	createLayout(Container::VSplitter);
}

void
FormManager::createLayout(int layoutType)
{
	WidgetList *list = m_active->selectedWidgets();
	// if only one widget is selected (a container), we modify its layout
	if(list->count() == 1)
	{
		ObjectTreeItem *item = m_active->objectTree()->lookup(list->first()->name());
		if(!item || !item->container() || !(*m_buffer)["layout"])
			return;
		(*m_buffer)["layout"] = Container::layoutTypeToString(layoutType);
		return;
	}

	QWidget *parent = list->first()->parentWidget();
	for(QWidget *w = list->first(); w; w = list->next())
	{
		kdDebug() << "comparing widget " << w->name() << " whose parent is " << w->parentWidget()->name() << " insteaed of " << parent->name() << endl;
		if(w->parentWidget() != parent)
		{
			KMessageBox::sorry(m_active->widget()->topLevelWidget(), i18n("<b>Cannot create the layout.</b>\n"
		   "All selected widgets must have the same parent."));
			kdDebug() << "FormManager::createLayout() widgets don't have the same parent widget" << endl;
			return;
		}
	}

	KCommand *com = new CreateLayoutCommand(layoutType, *list, m_active);
	m_active->addCommand(com, true);
}

void
FormManager::breakLayout()
{
	if(!activeForm() || !activeForm()->objectTree())
		return;

	Container *container = activeForm()->activeContainer();
	QCString c( container->widget()->className() );

	if((c == "Grid") || (c == "VBox") || (c == "HBox"))
	{
		KCommand *com = new BreakLayoutCommand(container);
		m_active->addCommand(com, true);
	}
	else // normal container
	{
		if(activeForm()->selectedWidgets()->count() == 1)
			(*m_buffer)["layout"] = "NoLayout";
		else
			container->setLayout(Container::NoLayout);
	}
}

void
FormManager::showPropertyBuffer(ObjectPropertyBuffer *buff)
{
	if(m_editor)
		m_editor->setBuffer(buff);

	emit bufferSwitched(buff);
}

void
FormManager::editTabOrder()
{
	if(!activeForm() || !activeForm()->objectTree())
		return;
	QWidget *topLevel = m_active->widget()->topLevelWidget();
	TabStopDialog dlg(topLevel);
	const bool oldAutoTabStops = m_active->autoTabStops();
	if (dlg.exec(m_active) == QDialog::Accepted) {
		//inform about changing "autoTabStop" property
		// -- this will be received eg. by Kexi, so custom "autoTabStop" property can be updated
		emit autoTabStopsSet(m_active, dlg.autoTabStops());
		//force set dirty
		emit dirty(m_active, true);
	}
}

void
FormManager::slotStyle()
{
	if(!activeForm())
		return;

	KSelectAction *m_style = (KSelectAction*)m_collection->action("change_style", "KSelectAction");
	QString style = m_style->currentText();
	activeForm()->widget()->setStyle( style);

	QObjectList *l = activeForm()->widget()->queryList( "QWidget" );
	for(QObject *o = l->first(); o; o = l->next())
		(static_cast<QWidget*>(o))->setStyle( style );
	delete l;
}

void
FormManager::editFormPixmapCollection()
{
	if(!activeForm() || !activeForm()->objectTree())
		return;

	PixmapCollectionEditor dialog(activeForm()->pixmapCollection(), activeForm()->widget()->topLevelWidget());
	dialog.exec();
}

void
FormManager::editConnections()
{
	if(!activeForm() || !activeForm()->objectTree())
		return;

	ConnectionDialog dialog(activeForm()->widget()->topLevelWidget());
	dialog.exec(activeForm());
}

void
FormManager::alignWidgets(int type)
{
	if(!activeForm() || !activeForm()->objectTree() || (activeForm()->selectedWidgets()->count() < 2))
		return;

	QWidget *parentWidget = activeForm()->selectedWidgets()->first()->parentWidget();

	for(QWidget *w = activeForm()->selectedWidgets()->first(); w; w = activeForm()->selectedWidgets()->next())
	{
		if(w->parentWidget() != parentWidget)
		{
			kdDebug() << "FormManager::alignWidgets() type ==" << type <<  " widgets don't have the same parent widget" << endl;
			return;
		}
	}

	KCommand *com = new AlignWidgetsCommand(type, *(activeForm()->selectedWidgets()), activeForm());
	activeForm()->addCommand(com, true);
}

void
FormManager::alignWidgetsToLeft()
{
	alignWidgets(AlignWidgetsCommand::AlignToLeft);
}

void
FormManager::alignWidgetsToRight()
{
	alignWidgets(AlignWidgetsCommand::AlignToRight);
}

void
FormManager::alignWidgetsToTop()
{
	alignWidgets(AlignWidgetsCommand::AlignToTop);
}

void
FormManager::alignWidgetsToBottom()
{
	alignWidgets(AlignWidgetsCommand::AlignToBottom);
}

void
FormManager::adjustWidgetSize()
{
	if(!activeForm() || !activeForm()->objectTree())
		return;

	KCommand *com = new AdjustSizeCommand(AdjustSizeCommand::SizeToFit, *(activeForm()->selectedWidgets()), activeForm());
	activeForm()->addCommand(com, true);
}

void
FormManager::alignWidgetsToGrid()
{
	if(!activeForm() || !activeForm()->objectTree())
		return;

	KCommand *com = new AlignWidgetsCommand(AlignWidgetsCommand::AlignToGrid, *(activeForm()->selectedWidgets()), activeForm());
	activeForm()->addCommand(com, true);
}

void
FormManager::adjustSizeToGrid()
{
	if(!activeForm() || !activeForm()->objectTree())
		return;

	KCommand *com = new AdjustSizeCommand(AdjustSizeCommand::SizeToGrid, *(activeForm()->selectedWidgets()), activeForm());
	activeForm()->addCommand(com, true);
}

void
FormManager::adjustWidthToSmall()
{
	if(!activeForm() || !activeForm()->objectTree())
		return;

	KCommand *com = new AdjustSizeCommand(AdjustSizeCommand::SizeToSmallWidth, *(activeForm()->selectedWidgets()), activeForm());
	activeForm()->addCommand(com, true);
}

void
FormManager::adjustWidthToBig()
{
	if(!activeForm() || !activeForm()->objectTree())
		return;

	KCommand *com = new AdjustSizeCommand(AdjustSizeCommand::SizeToBigWidth, *(activeForm()->selectedWidgets()), activeForm());
	activeForm()->addCommand(com, true);
}

void
FormManager::adjustHeightToSmall()
{
	if(!activeForm() || !activeForm()->objectTree())
		return;

	KCommand *com = new AdjustSizeCommand(AdjustSizeCommand::SizeToSmallHeight, *(activeForm()->selectedWidgets()), activeForm());
	activeForm()->addCommand(com, true);
}

void
FormManager::adjustHeightToBig()
{
	if(!activeForm() || !activeForm()->objectTree())
		return;

	KCommand *com = new AdjustSizeCommand(AdjustSizeCommand::SizeToBigHeight, *(activeForm()->selectedWidgets()), activeForm());
	activeForm()->addCommand(com, true);
}

void
FormManager::bringWidgetToFront()
{
	if(!activeForm() || !activeForm()->objectTree())
		return;

	for(QWidget *w = activeForm()->selectedWidgets()->first(); w; w = activeForm()->selectedWidgets()->next())
		w->raise();
}

void
FormManager::sendWidgetToBack()
{
	if(!activeForm() || !activeForm()->objectTree())
		return;

	for(QWidget *w = activeForm()->selectedWidgets()->first(); w; w = activeForm()->selectedWidgets()->next())
		w->lower();
}

void
FormManager::selectAll()
{
	if(!activeForm() || !activeForm()->objectTree())
		return;

	activeForm()->resetSelection();
	for(ObjectTreeItem *it = activeForm()->objectTree()->children()->first(); it; it = activeForm()->objectTree()->children()->next())
		activeForm()->setSelectedWidget(it->widget(), true);
}

void
FormManager::clearWidgetContent()
{
	if(!activeForm() || !activeForm()->objectTree())
		return;

	for(QWidget *w = activeForm()->selectedWidgets()->first(); w; w = activeForm()->selectedWidgets()->next())
		m_lib->clearWidgetContent(w->className(), w);
}

void
FormManager::deleteWidgetLater( QWidget *w )
{
	w->hide();
	w->reparent(0, WType_TopLevel, QPoint(0,0));
	m_deleteWidgetLater_list.append( w );
	m_deleteWidgetLater_timer.start( 100, true );
}

void
FormManager::deleteWidgetLaterTimeout()
{
	m_deleteWidgetLater_list.clear();
}

void
FormManager::showFormUICode()
{
#ifdef KEXI_SHOW_DEBUG_ACTIONS
	if(!activeForm())
		return;

	QString uiCode;
	KFormDesigner::FormIO::saveFormToString(activeForm(), uiCode, 3);

	if (!m_uiCodeDialog) {
		m_uiCodeDialog = new KDialogBase(0, "uiwindow", true, i18n("Form's UI Code"),
				KDialogBase::Close,	KDialogBase::Close);
		m_uiCodeDialog->resize(700, 600);
		QVBox *box = m_uiCodeDialog->makeVBoxMainWidget();
		KTabWidget* tab = new KTabWidget(box);

		m_currentUICodeDialogEditor = new KTextEdit(QString::null, QString::null, tab);
		tab->addTab( m_currentUICodeDialogEditor, i18n("Current"));
		m_currentUICodeDialogEditor->setReadOnly(true);
		QFont f( m_currentUICodeDialogEditor->font() );
		f.setFamily("courier");
		m_currentUICodeDialogEditor->setFont(f);
		m_currentUICodeDialogEditor->setTextFormat(Qt::PlainText);

		m_originalUICodeDialogEditor = new KTextEdit(QString::null, QString::null, tab);
		tab->addTab( m_originalUICodeDialogEditor, i18n("Original"));
		m_originalUICodeDialogEditor->setReadOnly(true);
		m_originalUICodeDialogEditor->setFont(f);
		m_originalUICodeDialogEditor->setTextFormat(Qt::PlainText);
	}
	m_currentUICodeDialogEditor->setText( uiCode );
	//indent and set our original doc as well:
	QDomDocument doc;
	doc.setContent( activeForm()->m_recentlyLoadedUICode );
	m_originalUICodeDialogEditor->setText( doc.toString( 3 ) );
	m_uiCodeDialog->show();
#endif
}

void
FormManager::slotSettingsChanged(int category)
{
	if (category==KApplication::SETTINGS_SHORTCUTS) {
		m_contextMenuKey = KGlobalSettings::contextMenuKey();
	}
}

void
FormManager::emitWidgetSelected( KFormDesigner::Form* form, bool multiple )
{
	enableFormActions();
	// Enable edit actions
	enableAction("edit_copy", true);
	enableAction("edit_cut", true);
	enableAction("edit_delete", true);
	enableAction("clear_contents", true);

	// 'Align Widgets' menu
	enableAction("align_menu", multiple);
	enableAction("align_to_left", multiple);
	enableAction("align_to_right", multiple);
	enableAction("align_to_top", multiple);
	enableAction("align_to_bottom", multiple);

	enableAction("adjust_size_menu", true);
	enableAction("adjust_width_small", multiple);
	enableAction("adjust_width_big", multiple);
	enableAction("adjust_height_small", multiple);
	enableAction("adjust_height_big", multiple);

	enableAction("format_raise", true);
	enableAction("format_lower", true);

	// If the widgets selected is a container, we enable layout actions
	bool containerSelected = false;
	if(!multiple)
	{
		KFormDesigner::ObjectTreeItem *item = form->objectTree()->lookup( form->selectedWidgets()->first()->name() );
		if(item && item->container())
			containerSelected = true;
	}
	const bool twoSelected = form->selectedWidgets()->count()==2;
	// Layout actions
	enableAction("layout_menu", multiple || containerSelected);
	enableAction("layout_hbox", multiple || containerSelected);
	enableAction("layout_vbox", multiple || containerSelected);
	enableAction("layout_grid", multiple || containerSelected);
	enableAction("layout_hsplitter", twoSelected);
	enableAction("layout_vsplitter", twoSelected);

	KFormDesigner::Container *container = activeForm()->activeContainer();
	enableAction("break_layout", (container->layoutType() != KFormDesigner::Container::NoLayout));

	emit widgetSelected(form, true);
}

void
FormManager::emitFormWidgetSelected( KFormDesigner::Form* form )
{
//	disableWidgetActions();
	enableAction("edit_copy", false);
	enableAction("edit_cut", false);
	enableAction("edit_delete", false);
	enableAction("clear_contents", false);

		// Disable format functions
	enableAction("align_menu", false);
	enableAction("align_to_left", false);
	enableAction("align_to_right", false);
	enableAction("align_to_top", false);
	enableAction("align_to_bottom", false);
	enableAction("adjust_size_menu", false);
	enableAction("format_raise", false);
	enableAction("format_lower", false);

	enableFormActions();

	const bool twoSelected = form->selectedWidgets()->count()==2;
	const bool hasChildren = !form->objectTree()->children()->isEmpty();

	// Layout actions
	enableAction("layout_menu", hasChildren);
	enableAction("layout_hbox", hasChildren);
	enableAction("layout_vbox", hasChildren);
	enableAction("layout_grid", hasChildren);
	enableAction("layout_hsplitter", twoSelected);
	enableAction("layout_vsplitter", twoSelected);
	enableAction("break_layout", (form->toplevelContainer()->layoutType() != KFormDesigner::Container::NoLayout));

	emit formWidgetSelected( form );
}

void
FormManager::emitNoFormSelected()
{
	disableWidgetActions();

	// Disable edit actions
//	enableAction("edit_paste", false);
//	enableAction("edit_undo", false);
//	enableAction("edit_redo", false);

	// Disable 'Tools' actions
	enableAction("pixmap_collection", false);
	enableAction("form_connections", false);
	enableAction("taborder", false);
	enableAction("change_style", activeForm()!=0);

	// Disable items in 'File'
	if (!(m_options & SkipFileActions)) {
		enableAction("file_save", false);
		enableAction("file_save_as", false);
		enableAction("preview_form", false);
	}

	emit noFormSelected();
}

void
FormManager::enableFormActions()
{
	// Enable 'Tools' actions
	enableAction("pixmap_collection", true);
	enableAction("form_connections", true);
	enableAction("taborder", true);
	enableAction("change_style", true);

	// Enable items in 'File'
	if (!(m_options & SkipFileActions)) {
		enableAction("file_save", true);
		enableAction("file_save_as", true);
		enableAction("preview_form", true);
	}

	enableAction("edit_paste", isPasteEnabled());
	enableAction("edit_select_all", true);
}

void
FormManager::disableWidgetActions()
{
	// Disable edit actions
	enableAction("edit_copy", false);
	enableAction("edit_cut", false);
	enableAction("edit_delete", false);
	enableAction("clear_contents", false);

	// Disable format functions
	enableAction("align_menu", false);
	enableAction("align_to_left", false);
	enableAction("align_to_right", false);
	enableAction("align_to_top", false);
	enableAction("align_to_bottom", false);
	enableAction("adjust_size_menu", false);
	enableAction("format_raise", false);
	enableAction("format_lower", false);

	enableAction("layout_menu", false);
	enableAction("layout_hbox", false);
	enableAction("layout_vbox", false);
	enableAction("layout_grid", false);
	enableAction("layout_hsplitter", false);
	enableAction("layout_vsplitter", false);
	enableAction("break_layout", false);
}

void
FormManager::emitUndoEnabled(bool enabled, const QString &text)
{
	enableAction("edit_undo", enabled);
	emit undoEnabled(enabled, text);
}

void
FormManager::emitRedoEnabled(bool enabled, const QString &text)
{
	enableAction("edit_redo", enabled);
	emit redoEnabled(enabled, text);
}

#include "formmanager.moc"
