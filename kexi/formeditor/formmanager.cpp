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

#include <kdeversion.h>
#if KDE_IS_VERSION(3,1,9) && !defined(Q_WS_WIN)
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
#include "extrawidgets.h"
#include "pixmapcollection.h"

#include "formmanager.h"

using namespace KFormDesigner;

FormManager::FormManager(QWidget *container, QObject *parent=0, const char *name=0)
   : QObject(parent, name)
{
	m_lib = new WidgetLibrary(this);
	m_buffer = new ObjectPropertyBuffer(this, this, "buffer");
	m_parent = container;

	m_editor = 0;
	m_active = 0;
	m_inserting = false;
	m_count = 0;
	m_collection = 0;

	m_domDoc.appendChild(m_domDoc.createElement("UI"));

	m_popup = new KPopupMenu();
	m_popup->insertItem( SmallIconSet("editcopy"), i18n("&Copy"), this, SLOT(copyWidget()), 0, MenuCopy);
	m_popup->insertItem( SmallIconSet("editcut"), i18n("Cu&t"), this, SLOT(cutWidget()), 0, MenuCut);
	m_popup->insertItem( SmallIconSet("editpaste"), i18n("&Paste"), this, SLOT(pasteWidget()), MenuPaste);
	m_popup->insertItem( SmallIconSet("editdelete"), i18n("&Remove Item"), this, SLOT(deleteWidget()), 0, MenuDelete);
	m_popup->insertSeparator(MenuDelete + 1);

	m_popup->insertItem( i18n("&Lay Out Horizontally"), this, SLOT(layoutHBox()), 0, MenuHBox);
	m_popup->insertItem( i18n("&Lay Out Vertically"), this, SLOT(layoutVBox()), 0, MenuVBox);
	m_popup->insertItem( i18n("&Lay Out in Grid"), this, SLOT(layoutGrid()), 0, MenuGrid);
	m_popup->insertSeparator(MenuGrid + 1);

	m_treeview = 0;
	m_editor = 0;

	m_deleteWidgetLater_list.setAutoDelete(true);
	connect( &m_deleteWidgetLater_timer, SIGNAL(timeout()), this, SLOT(deleteWidgetLaterTimeout()));
}

void
FormManager::setEditors(KexiPropertyEditor *editor, ObjectTreeView *treeview)
{
	m_editor = editor;
	m_treeview = treeview;

	if(editor)
		editor->setBuffer(m_buffer);

	connect(treeview, SIGNAL(selectionChanged(QWidget*)), m_buffer, SLOT(setWidget(QWidget *)));
	connect(treeview, SIGNAL(selectionChanged(QWidget*)), this, SLOT(setSelWidget(QWidget*)));
	connect(m_buffer, SIGNAL(nameChanged(const QString&, const QString&)), treeview, SLOT(renameItem(const QString&, const QString&)));
}

Actions
FormManager::createActions(KActionCollection *parent, KMainWindow *client)
{
	m_collection = parent;
	m_client = client;

	Actions actions = m_lib->createActions(parent, this, SLOT(insertWidget(const QString &)));
	m_pointer = new KToggleAction(i18n("Pointer"), "mouse_pointer", KShortcut(0), this, SLOT(stopInsert()), parent, "pointer");
	m_pointer->setExclusiveGroup("LibActionWidgets"); //to be exclusive with any 'widget' action
	m_pointer->setChecked(true);
	actions.append(m_pointer);

	// Create the Style selection action (with a combo box in toolbar and submenu items)
	KSelectAction *m_style = new KSelectAction( i18n("Style"), CTRL + Key_S, this, SLOT(slotStyle()), parent, "change_style");
	m_style->setEditable(false);

	KGlobal::config()->setGroup("General");
	const QString currentStyle = kapp->style().name();
	const QStringList styles = QStyleFactory::keys();
	m_style->setItems(styles);
	m_style->setCurrentItem(0);

	QStringList::ConstIterator it = styles.begin();
	QStringList::ConstIterator end = styles.end();
	int idx = 0;
	for (; it != end; ++it, ++idx)
	{
		if ((*it).lower() == currentStyle) {
			m_style->setCurrentItem(idx);
			break;
        	}
	}

	m_style->setToolTip(i18n("Set the current style to view."));
	m_style->setMenuAccelsEnabled(true);
	actions.append(m_style);

	return actions;
}

void
FormManager::insertWidget(const QString &classname)
{
	Form *form;
	for(form = m_forms.first(); form; form = m_forms.next())
	{
		form->m_cursors = new QMap<QString, QCursor>();
		if (form->toplevelContainer())
			form->toplevelContainer()->widget()->setCursor(QCursor(CrossCursor));
		QObjectList *l = form->toplevelContainer()->widget()->queryList( "QWidget" );
		for(QObject *o = l->first(); o; o = l->next())
		{
			if( ((QWidget*)o)->ownCursor() )
			{
				form->m_cursors->insert(o->name(), ((QWidget*)o)->cursor());
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
	Form *form;
	for(form = m_forms.first(); form; form = m_forms.next())
	{
		form->toplevelContainer()->widget()->unsetCursor();
		QObjectList *l = form->toplevelContainer()->widget()->queryList( "QWidget" );
		for(QObject *o = l->first(); o; o = l->next())
		{
			if( ((QWidget*)o)->ownCursor())
				((QWidget*)o)->setCursor( (*(form->m_cursors))[o->name()] ) ;
		}
		delete l;
		delete form->m_cursors;
		form->m_cursors = 0;
	}
	m_inserting = false;
	m_pointer->setChecked(true);
}

void
FormManager::windowChanged(QWidget *w)
{
	if(!w)
	{
		m_active = 0;
		return;
	}

//	if(m_forms.count() >= 1)
//	{
	/*if(m_collection && m_collection->action( KStdAction::name(KStdAction::Undo)))
		m_collection->take( m_collection->action( KStdAction::name(KStdAction::Undo) ) );
	if(m_collection && m_collection->action( KStdAction::name(KStdAction::Redo)))
		m_collection->take( m_collection->action( KStdAction::name(KStdAction::Redo) ) );*/
//	}

	Form *form;
	for(form = m_forms.first(); form; form = m_forms.next())
	{
		if(form->toplevelContainer() && form->toplevelContainer()->widget() == w)
		{
			m_active = form;
			if(m_treeview)
				m_treeview->setForm(form);
			if(m_buffer)
				m_buffer->setCollection(activeForm()->pixmapCollection());

			kdDebug() << "FormManager::windowChanged() active form is " << form->objectTree()->name() << endl;
			//if(m_collection)
				//m_collection->addDocCollection(form->actionCollection());
			KSelectAction *m_style = (KSelectAction*)m_collection->action("change_style", "KSelectAction");
			const QString currentStyle = form->toplevelContainer()->widget()->style().name();
			const QStringList styles = m_style->items();

			int idx = 0;
			for (QStringList::ConstIterator it = styles.begin(); it != styles.end(); ++it, ++idx)
			{
				if ((*it).lower() == currentStyle) {
					kdDebug() << "Updating the style to " << currentStyle << endl;
					m_style->setCurrentItem(idx);
					break;
				}
			}

			//if(m_client)
			//	m_client->createGUI(m_client->xmlFile());
			/*Actions actions;
			actions.append(form->actionCollection()->action( KStdAction::name(KStdAction::Undo) ));
			actions.append(form->actionCollection()->action( KStdAction::name(KStdAction::Redo) ));
			m_client->unplugActionList("undo_actions");
			m_client->plugActionList("undo_actions", actions);*/
			return;
		}
	}
	//m_active = 0;

}

Form*
FormManager::activeForm() const
{
	return m_active;
}

void
FormManager::deleteForm(Form *form)
{
	if(m_forms.find(form) == -1)
		m_preview.remove(form);
	else
		m_forms.remove(form);

	if(m_forms.count() == 0)
		m_active = 0;
}

void
FormManager::setSelWidget(QWidget *w)
{
	if(activeForm())
		activeForm()->setSelWidget(w);
}

void
FormManager::createBlankForm()
{
	createBlankForm("QWidget",0);
}

QWidget *
FormManager::createBlankForm(const QString &classname, const char *name, QWidget *parent)
{
	if(!parent)
		parent = m_parent;

	Form *form = new Form(this, name);

	QString n;

	n = i18n("Form") + QString::number(m_count + 1);
	FormWidgetBase *w = new FormWidgetBase(parent, n.latin1(), Qt::WDestructiveClose);

	form->createToplevel(w, w, classname);
	w->setCaption(n);
	w->setIcon(SmallIcon("form"));
	w->resize(350, 300);
	w->show();
	w->setFocus();
	initForm(form);

	return 0;
}

void
FormManager::importForm(QWidget *w, Form *form, bool preview)
{
	if(!form)
		form = new Form(this, w->name());

	if(!form->toplevelContainer())
		form->createToplevel(w, 0, w->name());
	w->setCaption(w->name());
	w->setIcon(SmallIcon("kexi"));
	w->resize(350, 300);
	w->show();
	//w->setFocus();

	if(!preview)
		initForm(form);
	else
	{
		m_preview.append(form);
		form->setDesignMode(false);
	}
}

void
FormManager::loadForm(bool preview, const QString &filename)
{
//	QString n = "Form" + QString::number(m_count + 1);
	Form *form = new Form(this);//, n.latin1());
	//QWidget *w = new QWidget(m_parent, 0, Qt::WDestructiveClose);
	FormWidgetBase *w = new FormWidgetBase(m_parent, 0, Qt::WDestructiveClose);
	form->createToplevel(w, w);
	if(!FormIO::loadForm(form, w, filename))
	{
		delete form;
		return;
	}
	w->show();

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
	m_count++;

	m_buffer->setWidget(form->toplevelContainer()->widget());

	connect(form, SIGNAL(selectionChanged(QWidget*)), m_buffer, SLOT(setWidget(QWidget*)));
	connect(form, SIGNAL(addedSelectedWidget(QWidget*)), m_buffer, SLOT(addWidget(QWidget*)));
	connect(form, SIGNAL(selectionChanged(QWidget*)), m_treeview, SLOT(setSelWidget(QWidget*)));
	connect(form, SIGNAL(addedSelectedWidget(QWidget*)), m_treeview, SLOT(addSelWidget(QWidget*)));
	connect(form, SIGNAL(childAdded(ObjectTreeItem* )), m_treeview, SLOT(addItem(ObjectTreeItem*)));
	connect(form, SIGNAL(childRemoved(ObjectTreeItem* )), m_treeview, SLOT(removeItem(ObjectTreeItem*)));
	connect(m_buffer, SIGNAL(nameChanged(const QString&, const QString&)), form, SLOT(changeName(const QString&, const QString&)));
	connect(m_treeview, SIGNAL(selectionChanged(QWidget*)), m_buffer, SLOT(setWidget(QWidget*)));

	windowChanged(form->toplevelContainer()->widget());
}


void
FormManager::saveForm()
{
	m_buffer->checkModifiedProp();
	if (activeForm())
	{
		if(!activeForm()->filename().isNull())
			FormIO::saveForm(activeForm(), activeForm()->filename());
		else
			FormIO::saveForm(activeForm());
	}
}

void
FormManager::saveFormAs()
{
	m_buffer->checkModifiedProp();
	if (activeForm())
		FormIO::saveForm(activeForm());
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
	if(!activeForm())
		return;

	QPtrList<QWidget> *list = activeForm()->selectedWidgets();
	if(list->isEmpty())
		return;

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
}

void
FormManager::cutWidget()
{
	if(!activeForm())
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
	if(!activeForm())
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
FormManager::createContextMenu(QWidget *w, Container *container, bool enableRemove)
{
	m_menuWidget = w;
	QString n = m_lib->displayName(w->className());
	KPopupMenu *p = new KPopupMenu();

	int id;
	bool ok = m_lib->createMenuActions(w->className(), w, p, container);
	if(!ok) // the widget doesn't have menu items, so we disable it
	{
		id = m_popup->insertItem(SmallIconSet(m_lib->icon(w->className())), n);
		m_popup->setItemEnabled(id, false);
	}
	else
		id = m_popup->insertItem(SmallIconSet(m_lib->icon(w->className())), n, p);

	m_popup->setItemEnabled(MenuCopy, enableRemove);
	m_popup->setItemEnabled(MenuCut, enableRemove);
	m_popup->setItemEnabled(MenuDelete, enableRemove);

	// We only enablelayout creation if more than one widget with the same parent are selected
	bool enableLayout = false;
	if((container->form()->selectedWidgets()->count() > 1) || (w == container->widget()))
		enableLayout = true;

	m_popup->setItemEnabled(MenuHBox, enableLayout);
	m_popup->setItemEnabled(MenuVBox, enableLayout);
	m_popup->setItemEnabled(MenuGrid, enableLayout);

	// We create the buddy menu
	int subid;
	if(w->inherits("QLabel") && ((QLabel*)w)->text().contains("&") && (((QLabel*)w)->textFormat() != RichText))
	{
		KPopupMenu *sub = new KPopupMenu(w);
		QWidget *buddy = ((QLabel*)w)->buddy();

		sub->insertItem(i18n("No Buddy"), MenuNoBuddy);
		if(!buddy)
			sub->setItemChecked(MenuNoBuddy, true);
		sub->insertSeparator();

		// We add al the widgets that can have focus
		ObjectTreeC *list = container->form()->tabStops();
		for(ObjectTreeItem *item = list->first(); item; item = list->next())
		{
			int index = sub->insertItem(item->name());
			if(item->widget() == buddy)
				sub->setItemChecked(index, true);
		}
		subid = m_popup->insertItem(i18n("Choose Buddy..."), sub);
		connect(sub, SIGNAL(activated(int)), this, SLOT(buddyChoosed(int)));
	}

	// We create the signals menu
	KPopupMenu *sigMenu = new KPopupMenu();
	QStrList list = w->metaObject()->signalNames(true);
	QStrListIterator it(list);
	for(; it.current() != 0; ++it)
		sigMenu->insertItem(*it);
	int sigid = m_popup->insertItem(SmallIconSet(""), i18n("Events"), sigMenu);
	if(list.isEmpty())
		m_popup->setItemEnabled(sigid, false);
	connect(sigMenu, SIGNAL(activated(int)), this, SLOT(menuSignalChoosed(int)));

	m_insertPoint = container->widget()->mapFromGlobal(QCursor::pos());
	m_popup->exec(QCursor::pos());
	m_insertPoint = QPoint();

	m_popup->removeItem(id);
	m_popup->removeItem(subid);
	m_popup->removeItem(sigid);
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
	if(!m_menuWidget)
		return;
	emit(createFormSlot(m_active, m_menuWidget->name(), m_popup->text(id)));
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
FormManager::createLayout(int layoutType)
{
	QtWidgetList *list = m_active->selectedWidgets();
	// if only one widget is selected (a container), we modify its layout
	if(list->count() == 1)
	{
		ObjectTreeItem *item = m_active->objectTree()->lookup(list->first()->name());
		if(!item || !item->container() || !(*m_buffer)["layout"])
			return;
		(*m_buffer)["layout"]->setValue(Container::layoutTypeToString(layoutType));
		return;
	}

	QWidget *parent = list->first()->parentWidget();
	for(QWidget *w = list->first(); w; w = list->next())
	{
		kdDebug() << "comparing widget " << w->name() << " whose parent is " << w->parentWidget()->name() << " insteaed of " << parent->name() << endl;
		if(w->parentWidget() != parent)
		{
			KMessageBox::sorry(m_active->toplevelContainer()->widget()->topLevelWidget(), i18n("<b>Cannot create the layout.</b>\n"
		   "All selected widgets must have the same parent."));
			kdDebug() << "FormManager::createLayout() widgets don't have the same parent widget" << endl;
			return;
		}
	}

	KCommand *com = new CreateLayoutCommand(layoutType, *list, m_active);
	m_active->addCommand(com, true);
}

void
FormManager::debugTree()
{
	if (activeForm() && activeForm()->objectTree())
		activeForm()->objectTree()->debug();
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
	if(!m_active)
		return;
	TabStopDialog dlg(m_active->toplevelContainer()->widget()->topLevelWidget());
	dlg.exec(m_active);
}

void
FormManager::adjustWidgetSize()
{
	if(!m_active)
		return;
	if(m_active->selectedWidgets()->count() > 1)
		return;

	m_active->selectedWidgets()->first()->resize(m_active->selectedWidgets()->first()->sizeHint());
}

void
FormManager::slotStyle()
{
	if(!activeForm())
		return;

	KSelectAction *m_style = (KSelectAction*)m_collection->action("change_style", "KSelectAction");
	QString style = m_style->currentText();
	kdDebug() << "Changing style of the form to " << style << endl;
	activeForm()->toplevelContainer()->widget()->setStyle( style);

	QObjectList *l = activeForm()->toplevelContainer()->widget()->queryList( "QWidget" );
	for(QObject *o = l->first(); o; o = l->next())
		(static_cast<QWidget*>(o))->setStyle( style );
	delete l;
}

void
FormManager::editFormPixmapCollection()
{
	if(!activeForm())
		return;

	PixmapCollectionEditor dialog(activeForm()->pixmapCollection(), activeForm()->toplevelContainer()->widget()->topLevelWidget());
	dialog.exec();
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

FormManager::~FormManager()
{
	delete m_popup;
}

#include "formmanager.moc"

