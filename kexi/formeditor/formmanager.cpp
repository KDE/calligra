/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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

#include <kdebug.h>

#include <qworkspace.h>
#include <qcursor.h>
#include <qstring.h>
#include <qlabel.h>
#include <qstylefactory.h>
#include <qmetaobject.h>
#include <qregexp.h>
#include <q3valuevector.h>
#include <q3vbox.h>
//Added by qt3to4:
#include <Q3CString>
#include <Q3StrList>
#include <Q3PtrList>

#include <klocale.h>
#include <kiconloader.h>
#include <kmenu.h>
#include <kstandardaction.h>
#include <kaction.h>
#include <kxmlguiclient.h>
#include <kmainwindow.h>
#include <kmessagebox.h>
#include <kconfig.h>
#include <kstyle.h>
#include <ktoggleaction.h>
#include <kselectaction.h>
#include <kapplication.h>
#include <kglobal.h>
#include <kglobalsettings.h>
#include <kdialog.h>
#include <ktextedit.h>
#include <ktabwidget.h>
#include <kfontdialog.h>

#include <kdeversion.h>
#if KDE_VERSION >= KDE_MAKE_VERSION(3,1,9) && !defined(Q_WS_WIN)
# include <kactioncollection.h>
#endif

#include "widgetpropertyset.h"
#include "objecttree.h"
#include "widgetlibrary.h"
#include "form.h"
#include "container.h"
#include "formIO.h"
#include "objecttreeview.h"
#include "commands.h"
#include "tabstopdialog.h"
#include "connectiondialog.h"
#include "events.h"
#include "utils.h"
#include "kfdpixmapedit.h"
#include <koproperty/editor.h>
#include <koproperty/property.h>
#include <koproperty/factory.h>
#include <kexiutils/utils.h>

#include "formmanager.h"

#define KFD_NO_STYLES //disables; styles support needs improvements

#define KEXI_NO_PIXMAPCOLLECTION
#ifdef __GNUC__
#warning pixmapcollection
#endif
#ifndef KEXI_NO_PIXMAPCOLLECTION
#include "pixmapcollection.h"
#endif

namespace KFormDesigner {

//! @internal
class PropertyFactory : public KoProperty::CustomPropertyFactory
{
	public:
		PropertyFactory(QObject *parent) 
		: KoProperty::CustomPropertyFactory(parent)
//			m_manager(manager)
		{
		}
		virtual ~PropertyFactory() {}

		KoProperty::CustomProperty* createCustomProperty(KoProperty::Property *) { return 0;}

		KoProperty::Widget* createCustomWidget(KoProperty::Property *prop)
		{
			return new KFDPixmapEdit(prop);
		}
};

}

using namespace KFormDesigner;

static KStaticDeleter<FormManager> m_managerDeleter;
FormManager* FormManager::_self = 0L;

FormManager::FormManager(QObject *parent, int options, const char *name)
   : QObject(parent, name)
#ifdef KEXI_DEBUG_GUI
   , m_uiCodeDialog(0)
   , m_options(options)
#endif
   , m_objectBlockingPropertyEditorUpdating(0)
   , m_isRedoing(false)
{
	Q_UNUSED(options);
#ifdef KEXI_STANDALONE
	KGlobal::locale()->insertCatalog("standalone_kformdesigner");
#else
	KGlobal::locale()->insertCatalog("kformdesigner");
#endif

	connect( KGlobalSettings::self(), SIGNAL( settingsChanged(int) ), SLOT( slotSettingsChanged(int) ) );
	slotSettingsChanged(KGlobalSettings::SETTINGS_SHORTCUTS);

//moved to createWidgetLibrary()	m_lib = new WidgetLibrary(this, supportedFactoryGroups);
	m_propSet = new WidgetPropertySet(this);
	
	m_widgetActionGroup = new QActionGroup(this);

	//unused m_editor = 0;
	m_active = 0;
	m_inserting = false;
	m_drawingSlot = false;
	m_collection = 0;
	m_connection = 0;
	m_popup = 0;
	m_treeview = 0;
	m_emitSelectionSignalsUpdatesPropertySet = false;
	m_domDoc.appendChild(m_domDoc.createElement("UI"));

	m_deleteWidgetLater_list.setAutoDelete(true);
	connect( &m_deleteWidgetLater_timer, SIGNAL(timeout()), this, SLOT(deleteWidgetLaterTimeout()));
	connect( this, SIGNAL(connectionCreated(KFormDesigner::Form*, KFormDesigner::Connection&)),
		this, SLOT(slotConnectionCreated(KFormDesigner::Form*, KFormDesigner::Connection&)));

	// register kfd custom editors
	KoProperty::FactoryManager::self()->registerFactoryForEditor(KoProperty::Pixmap, 
		new PropertyFactory(KoProperty::FactoryManager::self()));
}

FormManager::~FormManager()
{
	m_managerDeleter.setObject(_self, 0, false); //safe
	delete m_popup;
	delete m_connection;
#ifdef KEXI_DEBUG_GUI
	delete m_uiCodeDialog;
#endif
//	delete m_propFactory;
}


FormManager* FormManager::self()
{
	return _self;
}

WidgetLibrary* 
FormManager::createWidgetLibrary(FormManager* m, const QStringList& supportedFactoryGroups)
{
	if(!_self)
		m_managerDeleter.setObject( _self, m );
	return new WidgetLibrary(_self, supportedFactoryGroups);
}

void
FormManager::setEditor(KoProperty::Editor *editor)
{
	m_editor = editor;

	if(editor)
		editor->changeSet(m_propSet->set());
}

void
FormManager::setObjectTreeView(ObjectTreeView *treeview)
{
	m_treeview = treeview;
	if (m_treeview)
		connect(m_propSet, SIGNAL(widgetNameChanged(const Q3CString&, const Q3CString&)),
			m_treeview, SLOT(renameItem(const Q3CString&, const Q3CString&)));
}

ActionList
FormManager::createActions(WidgetLibrary *lib, KActionCollection* collection, KXMLGUIClient* client)
{
	m_collection = collection;

	ActionList actions = lib->createWidgetActions(client, m_collection, 
		this, SLOT(insertWidget(const Q3CString &)));

	if (m_options & HideSignalSlotConnections)
		m_dragConnection = 0;
	else {
		m_dragConnection = new KToggleAction(
			KIcon("signalslot"), i18n("Connect Signals/Slots"), m_collection);
		m_dragConnection->setObjectName("drag_connection");
		m_widgetActionGroup->addAction( m_dragConnection );
		connect(m_dragConnection, SIGNAL(triggered()),
			this, SLOT(startCreatingConnection()));
		//to be exclusive with any 'widget' action
//kde4 not needed			m_dragConnection->setExclusiveGroup("LibActionWidgets");
		m_dragConnection->setChecked(false);
		actions.append(m_dragConnection);
	}

	m_pointer = new KToggleAction(
		KIcon("mouse_pointer"), i18n("Pointer"), m_collection);
	m_pointer->setObjectName("pointer");
	m_widgetActionGroup->addAction( m_pointer );
	connect(m_pointer, SIGNAL(triggered()),
		this, SLOT(slotPointerClicked()));
//kde4 not needed	m_pointer->setExclusiveGroup("LibActionWidgets"); //to be exclusive with any 'widget' action
	m_pointer->setChecked(true);
	actions.append(m_pointer);

	m_snapToGrid = new KToggleAction(
		i18n("Snap to Grid"), m_collection);
	m_snapToGrid->setObjectName("snap_to_grid");
	m_widgetActionGroup->addAction( m_snapToGrid );
	m_snapToGrid->setChecked(true);
	actions.append(m_snapToGrid);

	// Create the Style selection action (with a combo box in toolbar and submenu items)
	KSelectAction *styleAction = new KSelectAction(
			i18n("Style"), m_collection);
	styleAction->setObjectName("change_style");
	connect(styleAction, SIGNAL(triggered()),
		this, SLOT(slotStyle()));
	styleAction->setEditable(false);

	KGlobal::config()->setGroup("General");
	QString currentStyle( QString::fromLatin1(kapp->style()->name()).lower() );
	const QStringList styles = QStyleFactory::keys();
	styleAction->setItems(styles);
	styleAction->setCurrentItem(0);

	QStringList::ConstIterator endIt = styles.constEnd();
	int idx = 0;
	for (QStringList::ConstIterator it = styles.constBegin(); it != endIt; ++it, ++idx)
	{
		if ((*it).lower() == currentStyle) {
			styleAction->setCurrentItem(idx);
			break;
		}
	}
	styleAction->setToolTip(i18n("Set the current view style."));
	styleAction->setMenuAccelsEnabled(true);
	actions.append(styleAction);

	lib->addCustomWidgetActions(m_collection);

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

	m_isRedoing = true;
	activeForm()->commandHistory()->redo();
	m_isRedoing = false;
}

void
FormManager::insertWidget(const Q3CString &classname)
{
	if(m_drawingSlot)
		stopCreatingConnection();

	m_inserting = true;

	Form *form;
	for(form = m_forms.first(); form; form = m_forms.next())
	{
//		form->d->cursors = new QMap<QString, QCursor>();
		if (form->toplevelContainer())
			form->widget()->setCursor(QCursor(Qt::CrossCursor));
		QObjectList l( form->widget()->queryList( "QWidget" ) );
		foreach(QObject *o, l) {
			QWidget *w = static_cast<QWidget*>(o);
			if (w->ownCursor()) {
				form->d->cursors.insert(o, w->cursor());
				w->setCursor(QCursor(Qt::CrossCursor));
			}
		}
	}

	m_selectedClass = classname;
	m_pointer->setChecked(false);
}

void
FormManager::stopInsert()
{
	if(m_drawingSlot)
		stopCreatingConnection();
	if(!m_inserting)
		return;

//#ifndef KEXI_NO_CURSOR_PROPERTY
	Form *form;
	for(form = m_forms.first(); form; form = m_forms.next())
	{
		form->widget()->unsetCursor();
		QObjectList l( form->widget()->queryList( "QWidget" ) );
		foreach (QObject *o, l) {
			static_cast<QWidget*>(o)->unsetCursor();
#if 0
			if( ((QWidget*)o)->ownCursor()) {
				QMap<QObject*,QCursor>::ConstIterator curIt( form->d->cursors.find(o) );
				if (curIt!=form->d->cursors.constEnd())
					static_cast<QWidget*>(o)->setCursor( *curIt );
//				((QWidget*)o)->setCursor( (*(form->d->cursors))[o->name()] ) ;
			}
#endif
		}
	}
//#endif
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
	if (m_options & HideSignalSlotConnections)
		return;

	if(m_inserting)
		stopInsert();

	// We set a Pointing hand cursor while drawing the connection
	Form *form;
	for(form = m_forms.first(); form; form = m_forms.next())
	{
//		form->d->cursors = new QMap<QString, QCursor>();
		form->d->mouseTrackers = new QStringList();
		if (form->toplevelContainer())
		{
			form->widget()->setCursor(QCursor(Qt::PointingHandCursor));
			form->widget()->setMouseTracking(true);
		}
		QObjectList l( form->widget()->queryList( "QWidget" ) );
		foreach (QObject *o, l) {
			QWidget *w = static_cast<QWidget*>(o);
			if( w->ownCursor() ) {
				form->d->cursors.insert(w, w->cursor());
//				form->d->cursors->insert(w->name(), w->cursor());
				w->setCursor(QCursor(Qt::PointingHandCursor ));
			}
			if (w->hasMouseTracking())
				form->d->mouseTrackers->append(w->name());
			w->setMouseTracking(true);
		}
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
	if (m_options & HideSignalSlotConnections)
		return;

	delete m_connection;
	m_connection = new Connection();

	if(m_active && m_active->formWidget()) {
		Form *ff = (Form*)m_active;
		FormWidget *fw = 0;
		if (ff)
			fw = ff->formWidget();
		m_active->formWidget()->clearForm();
	}
	if (m_active && m_active->widget())
		m_active->widget()->repaint();
}

void
FormManager::stopCreatingConnection()
{
	if (m_options & HideSignalSlotConnections)
		return;
	if(!m_drawingSlot)
		return;

	if(m_active && m_active->formWidget())
		m_active->formWidget()->clearForm();

	Form *form;
	for(form = m_forms.first(); form; form = m_forms.next())
	{
		form->widget()->unsetCursor();
		form->widget()->setMouseTracking(false);
		QObjectList l( form->widget()->queryList( "QWidget" ) );
		foreach (QObject *o, l) {
			QWidget *w = static_cast<QWidget*>(o);
			if (w->ownCursor()) {
				QMap<QObject*,QCursor>::ConstIterator curIt( form->d->cursors.find(o) );
				if (curIt!=form->d->cursors.constEnd())
					w->setCursor( *curIt );
			}
			w->setMouseTracking( !form->d->mouseTrackers->grep(w->name()).isEmpty() );
		}
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
	kDebug() << "FormManager::windowChanged(" 
		<< (w ? (QString(w->className())+" "+w->name()) : QString("0")) << ")" << endl;

	if(!w)
	{
		m_active = 0;
		if(m_treeview)
			m_treeview->setForm(0);
		emit propertySetSwitched(0);
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
			//if(m_propSet)
			//	m_propList->setCollection(form->pixmapCollection());

			kDebug() << "FormManager::windowChanged() active form is " << form->objectTree()->name() << endl;

			if(m_collection)
			{
#ifndef KFD_NO_STYLES
				// update the 'style' action
				KSelectAction *m_style = (KSelectAction*)m_collection->action("change_style", "KSelectAction");
				const QString currentStyle = form->widget()->style().name();
				const QStringList styles = m_style->items();

				int idx = 0;
				QStringList::ConstIterator endIt = styles.constEnd();
				for (QStringList::ConstIterator it = styles.constBegin(); it != endIt; ++it, ++idx)
				{
					if ((*it).lower() == currentStyle) {
						kDebug() << "Updating the style to " << currentStyle << endl;
						m_style->setCurrentItem(idx);
						break;
					}
				}
#endif
			}

			if((form != previousActive) && isCreatingConnection())
				resetCreatedConnection();

			m_active = form;

			emit  dirty(form, form->isModified());
			// update actions state
			m_active->emitActionSignals();
			//update the buffer too
			form->emitSelectionSignals();
			if (!m_emitSelectionSignalsUpdatesPropertySet)
				showPropertySet( propertySet(), true );
			return;
		}
	}

	for(form = m_preview.first(); form; form = m_preview.next())
	{
		kDebug() << (form->widget() ? form->widget()->name() : "") << endl;
		if(form->toplevelContainer() && form->widget() == w) {
			kDebug() << "FormManager::windowChanged() active preview form is " << form->widget()->name() << endl;

			if(m_collection)
			{
#ifndef KFD_NO_STYLES
				// update the 'style' action
				KSelectAction *m_style = (KSelectAction*)m_collection->action("change_style", "KSelectAction");
				const QString currentStyle = form->widget()->style().name();
				const QStringList styles = m_style->items();

				int idx = 0;
				QStringList::ConstIterator endIt = styles.constEnd();
				for (QStringList::ConstIterator it = styles.constBegin(); it != endIt; ++it, ++idx)
				{
					if ((*it).lower() == currentStyle) {
						kDebug() << "Updating the style to " << currentStyle << endl;
						m_style->setCurrentItem(idx);
						break;
					}
				}
#endif

				resetCreatedConnection();
				m_active = form;

				emit dirty(form, false);
				emitNoFormSelected();
				showPropertySet(0);
				return;
			}
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
		emit propertySetSwitched(0);
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

	connect(form, SIGNAL(selectionChanged(QWidget*, bool, bool)), 
		m_propSet, SLOT(setSelectedWidgetWithoutReload(QWidget*, bool, bool)));
	if(m_treeview)
	{
		connect(form, SIGNAL(selectionChanged(QWidget*, bool, bool)), 
			m_treeview, SLOT(setSelectedWidget(QWidget*, bool)));
		connect(form, SIGNAL(childAdded(ObjectTreeItem* )), m_treeview, SLOT(addItem(ObjectTreeItem*)));
		connect(form, SIGNAL(childRemoved(ObjectTreeItem* )), m_treeview, SLOT(removeItem(ObjectTreeItem*)));
	}
	connect(m_propSet, SIGNAL(widgetNameChanged(const Q3CString&, const Q3CString&)),
		form, SLOT(changeName(const Q3CString&, const Q3CString&)));

	form->setSelectedWidget(form->widget());
	windowChanged(form->widget());
}

void
FormManager::previewForm(Form *form, QWidget *container, Form *toForm)
{
	if(!form || !container || !form->objectTree())
		return;
	QDomDocument domDoc;
	if (!FormIO::saveFormToDom(form, domDoc))
		return;

	Form *myform;
	if(!toForm)
		myform = new Form(form->library(), form->objectTree()->name().latin1(), 
			false/*!designMode, we need to set it early enough*/);
	else
		myform = toForm;
	myform->createToplevel(container);
	container->setStyle( form->widget()->style() );

	if (!FormIO::loadFormFromDom(myform, container, domDoc)) {
		delete myform;
		return;
	}

	myform->setDesignMode(false);
	m_preview.append(myform);
	container->show();
}

/*
bool
FormManager::loadFormFromDomInternal(Form *form, QWidget *container, QDomDocument &inBuf)
{
	return FormIO::loadFormFromDom(myform, container, domDoc);
}

bool
FormManager::saveFormToStringInternal(Form *form, QString &dest, int indent)
{
	return KFormDesigner::FormIO::saveFormToString(form, dest, indent);
}*/

bool
FormManager::isTopLevel(QWidget *w)
{
	if(!activeForm() || !activeForm()->objectTree())
		return false;

//	kDebug() << "FormManager::isTopLevel(): for: " << w->name() << " = "
//		<< activeForm()->objectTree()->lookup(w->name())<< endl;

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

	Q3PtrList<QWidget> *list = activeForm()->selectedWidgets();
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

	Q3PtrList<QWidget> *list = activeForm()->selectedWidgets();
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

	Q3PtrList<QWidget> *list = activeForm()->selectedWidgets();
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
	m_sigSlotMenu = new KMenu();
	m_sigSlotMenu->addTitle(SmallIcon("connection"), i18n("Signals"));

	const QList<QMetaMethod> list( 
		KexiUtils::methodsForMetaObjectWithParents(w->metaObject(),
		QMetaMethod::Signal, QMetaMethod::Public) );
//qt3:	Q3StrList list = w->metaObject()->signalNames(true);
	foreach (QMetaMethod method, list)
		m_sigSlotMenu->addAction( QString::fromLatin1(method.signature()) );

	QAction* result = m_sigSlotMenu->exec(QCursor::pos());
	if (result)
		menuSignalChosen(result);
	else
		resetCreatedConnection();

	delete m_sigSlotMenu;
	m_sigSlotMenu = 0;
}

void
FormManager::createSlotMenu(QWidget *w)
{
	m_sigSlotMenu = new KMenu();
	m_sigSlotMenu->addTitle(SmallIcon("connection"), i18n("Slots"));

	QString signalArg( m_connection->signal().remove( QRegExp(".*[(]|[)]") ) );

	const QList<QMetaMethod> list( 
		KexiUtils::methodsForMetaObjectWithParents(w->metaObject(),
		QMetaMethod::Slot, QMetaMethod::Public) );
//qt3:	Q3StrList list = w->metaObject()->slotNames(true);
	foreach (QMetaMethod method, list) {
		QString slotArg(method.signature());
		slotArg = slotArg.remove( QRegExp(".*[(]|[)]") );
		if (!signalArg.startsWith(slotArg))
			continue; // args not compatible
		m_sigSlotMenu->addAction( slotArg );
	}

	QAction* result = m_sigSlotMenu->exec(QCursor::pos());
	if (result)
		menuSignalChosen(result);
	else
		resetCreatedConnection();

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
	//const bool enableRemove = w != m_active->widget();
	// We only enablelayout creation if more than one widget with the same parent are selected
	const bool enableLayout = multiple || w == container->widget();

	m_menuWidget = w;
	QString n = container->form()->library()->displayName(w->className());
//	QValueVector<int> menuIds();

	if (!m_popup) {
		m_popup = new KMenu();
	}
	else {
		m_popup->clear();
	}

	//set title
	QIcon icon;
	QString titleText;
	if (!multiple) {
		if(w == container->form()->widget()) {
			icon = SmallIcon("form");
			titleText = i18n("%1 : Form").arg(w->name());
		}
		else {
			icon = SmallIcon(
				container->form()->library()->iconName(w->className()));
			titleText = QString(w->name()) + " : " + n;
		}
	}
	else {
		icon = SmallIcon("multiple_obj");
		titleText = i18n("Multiple Widgets") + QString(" (%1)").arg(widgetsCount);
	}
	
	m_popup->addTitle(icon, titleText);

	KAction *a;
#define PLUG_ACTION(_name, forceVisible) \
	{ a = action(_name); \
	if (a && (forceVisible || a->isEnabled())) { \
		if (separatorNeeded) \
			m_popup->insertSeparator(); \
		separatorNeeded = false; \
		m_popup->addAction(a); \
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
	if(!multiple && w->inherits("QLabel") && ((QLabel*)w)->text().contains("&") && (((QLabel*)w)->textFormat() != Qt::RichText))
	{
		if (separatorNeeded)
			m_popup->insertSeparator();

		KMenu *sub = new KMenu(w);
		QWidget *buddy = ((QLabel*)w)->buddy();

		sub->insertItem(i18n("No Buddy"), MenuNoBuddy);
		if(!buddy)
			sub->setItemChecked(MenuNoBuddy, true);
		sub->insertSeparator();

		// add all the widgets that can have focus
		for(ObjectTreeListIterator it( container->form()->tabStopsIterator() ); it.current(); ++it)
		{
			int index = sub->insertItem( 
				SmallIcon(container->form()->library()->iconName(it.current()->className().latin1())),
				it.current()->name());
			if(it.current()->widget() == buddy)
				sub->setItemChecked(index, true);
		}

		/*int id =*/ m_popup->insertItem(i18n("Choose Buddy..."), sub);
//		menuIds->append(id);
		connect(sub, SIGNAL(activated(int)), this, SLOT(buddyChosen(int)));

		separatorNeeded = true;
	}

	//int sigid=0;
#ifdef KEXI_DEBUG_GUI
	if(!multiple && !(m_options & HideEventsInPopupMenu))
	{
		if (separatorNeeded)
			m_popup->insertSeparator();

		// We create the signals menu
		KMenu *sigMenu = new KMenu();
		Q3StrList list = w->metaObject()->signalNames(true);
		QStrListIterator it(list);
		for(; it.current() != 0; ++it)
			sigMenu->insertItem(*it);

		int id = m_popup->insertItem(KIcon(""), i18n("Events"), sigMenu);
//		menuIds->append(id);
		if(list.isEmpty())
			m_popup->setItemEnabled(id, false);
		connect(sigMenu, SIGNAL(triggered(QAction*)), 
			this, SLOT(menuSignalChosen(QAction*)));
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
		const uint oldIndex = m_popup->count()-1;
		container->form()->library()
			->createMenuActions(w->className(), w, m_popup, container);
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
FormManager::buddyChosen(int id)
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
FormManager::menuSignalChosen(QAction* action)
{
	if (m_options & HideSignalSlotConnections)
		return;

	//if(!m_menuWidget)
	//	return;
	if (m_drawingSlot && m_sigSlotMenu && action)
	{
		if( m_connection->receiver().isNull() )
			m_connection->setSignal(action->text());
		else
		{
			m_connection->setSlot(action->text());
			kDebug() << "Finished creating the connection: sender=" << m_connection->sender() << "; signal=" << m_connection->signal() <<
			  "; receiver=" << m_connection->receiver() << "; slot=" << m_connection->slot() << endl;
			emit connectionCreated(activeForm(), *m_connection);
			stopCreatingConnection();
		}
	}
	else if(m_menuWidget)
		emit createFormSlot(m_active, m_menuWidget->name(), action->text());
}

void
FormManager::slotConnectionCreated(Form *form, Connection &connection)
{
	if (m_options & HideSignalSlotConnections)
		return;
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
FormManager::layoutHFlow()
{
	createLayout(Container::HFlow);
}

void
FormManager::layoutVFlow()
{
	createLayout(Container::VFlow);
}

void
FormManager::createLayout(int layoutType)
{
	WidgetList *list = m_active->selectedWidgets();
	// if only one widget is selected (a container), we modify its layout
	if (list->isEmpty()) {//sanity check
		kWarning() << "FormManager::createLayout(): list is empty!" << endl;
		return;
	}
	if(list->count() == 1)
	{
		ObjectTreeItem *item = m_active->objectTree()->lookup(list->first()->name());
		if(!item || !item->container() || !m_propSet->contains("layout"))
			return;
		(*m_propSet)["layout"] = Container::layoutTypeToString(layoutType);
		return;
	}

	QWidget *parent = list->first()->parentWidget();
	for(QWidget *w = list->first(); w; w = list->next())
	{
		kDebug() << "comparing widget " << w->name() << " whose parent is " << w->parentWidget()->name() << " insteaed of " << parent->name() << endl;
		if(w->parentWidget() != parent)
		{
			KMessageBox::sorry(m_active->widget()->topLevelWidget(), i18n("<b>Cannot create the layout.</b>\n"
		   "All selected widgets must have the same parent."));
			kDebug() << "FormManager::createLayout() widgets don't have the same parent widget" << endl;
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
	Q3CString c( container->widget()->className() );

	if((c == "Grid") || (c == "VBox") || (c == "HBox") || (c == "HFlow") || (c == "VFlow"))
	{
		KCommand *com = new BreakLayoutCommand(container);
		m_active->addCommand(com, true);
	}
	else // normal container
	{
		if(activeForm()->selectedWidgets()->count() == 1)
			(*m_propSet)["layout"] = "NoLayout";
		else
			container->setLayout(Container::NoLayout);
	}
}

void
FormManager::showPropertySet(WidgetPropertySet *set, bool forceReload, const Q3CString& propertyToSelect)
{
	if (m_objectBlockingPropertyEditorUpdating)
		return;

/*unused	if(m_editor) {
		if (propertyToSelect.isEmpty() && forceReload)
			m_editor->changeSet(set ? set->set() : 0, propertyToSelect);
		else
			m_editor->changeSet(set ? set->set() : 0);
	}*/

	emit propertySetSwitched(set ? set->set(): 0, /*preservePrevSelection*/forceReload, propertyToSelect);
}

void
FormManager::blockPropertyEditorUpdating(void *blockingObject)
{
	if (!blockingObject || m_objectBlockingPropertyEditorUpdating)
		return;
	m_objectBlockingPropertyEditorUpdating = blockingObject;
}

void
FormManager::unblockPropertyEditorUpdating(void *blockingObject, WidgetPropertySet *set)
{
	if (!blockingObject || m_objectBlockingPropertyEditorUpdating!=blockingObject)
		return;

	m_objectBlockingPropertyEditorUpdating = 0;
	showPropertySet(set, true/*forceReload*/);
}

void
FormManager::editTabOrder()
{
	if(!activeForm() || !activeForm()->objectTree())
		return;
	QWidget *topLevel = m_active->widget()->topLevelWidget();
	TabStopDialog dlg(topLevel);
	//const bool oldAutoTabStops = m_active->autoTabStops();
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

	KSelectAction *styleAction = qobject_cast<KSelectAction*>(
		m_collection->action("change_style"));
	QString style = styleAction->currentText();
	activeForm()->widget()->setStyle(style);

	QObjectList l( activeForm()->widget()->queryList( "QWidget" ) );
	foreach (QObject *o, l)
		static_cast<QWidget*>(o)->setStyle( style );
}

void
FormManager::editFormPixmapCollection()
{
	if(!activeForm() || !activeForm()->objectTree())
		return;

#ifdef __GNUC__
#warning pixmapcollection
#endif
#ifndef KEXI_NO_PIXMAPCOLLECTION
	PixmapCollectionEditor dialog(activeForm()->pixmapCollection(), activeForm()->widget()->topLevelWidget());
	dialog.exec();
#endif
}

void
FormManager::editConnections()
{
	if (m_options & HideSignalSlotConnections)
		return;
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
			kDebug() << "FormManager::alignWidgets() type ==" << type <<  " widgets don't have the same parent widget" << endl;
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

	activeForm()->selectFormWidget();
	uint count = activeForm()->objectTree()->children()->count();
	for(ObjectTreeItem *it = activeForm()->objectTree()->children()->first(); it; 
		it = activeForm()->objectTree()->children()->next(), count--)
	{
		activeForm()->setSelectedWidget(it->widget(), /*add*/true, /*raise*/false, /*moreWillBeSelected*/count>1);
	}
}

void
FormManager::clearWidgetContent()
{
	if(!activeForm() || !activeForm()->objectTree())
		return;

	for(QWidget *w = activeForm()->selectedWidgets()->first(); w; w = activeForm()->selectedWidgets()->next())
		activeForm()->library()->clearWidgetContent(w->className(), w);
}

void
FormManager::deleteWidgetLater( QWidget *w )
{
	w->hide();
	w->reparent(0, Qt::WType_TopLevel, QPoint(0,0));
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
#ifdef KEXI_DEBUG_GUI
	if(!activeForm())
		return;

	QString uiCode;
	if (!FormIO::saveFormToString(activeForm(), uiCode, 3)) {
		//! @todo show err?
		return;
	}

	if (!m_uiCodeDialog) {
		m_uiCodeDialog = new KPageDialog();
		m_uiCodeDialog->setFaceType(KPageDialog::Tabbed);
		m_uiCodeDialog->setObjectName("ui_dialog");
		m_uiCodeDialog->setModal(true)
		m_uiCodeDialog->setCaption(i18n("Form's UI Code"));
		m_uiCodeDialog->setButtons(KDialog::Close);
//kde4: needed?		m_uiCodeDialog->resize(700, 600);

		m_currentUICodeDialogEditor = new KTextEdit(
			QString::null, QString::null, m_uiCodeDialog);
		m_uiCodeDialog->addPage(m_currentUICodeDialogEditor, i18n("Current"));
		m_currentUICodeDialogEditor->setReadOnly(true);
		QFont f( m_currentUICodeDialogEditor->font() );
		f.setFamily("courier");
		m_currentUICodeDialogEditor->setFont(f);
		m_currentUICodeDialogEditor->setTextFormat(Qt::PlainText);

		m_originalUICodeDialogEditor = new KTextEdit(
			QString::null, QString::null, m_uiCodeDialog);
		m_uiCodeDialog->addPage(m_originalUICodeDialogEditor, i18n("Original"));
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
	if (category==KGlobalSettings::SETTINGS_SHORTCUTS) {
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

	WidgetList *wlist = form->selectedWidgets();
	bool fontEnabled = false;
	for (WidgetListIterator it(*wlist); it.current(); ++it) {
		if (-1!=KexiUtils::indexOfPropertyWithSuperclasses(it.current(), "font")) {
			fontEnabled = true;
			break;
		}
	}
	enableAction("format_font", fontEnabled);

	// If the widgets selected is a container, we enable layout actions
	bool containerSelected = false;
	if(!multiple)
	{
		KFormDesigner::ObjectTreeItem *item = 0;
		if (form->selectedWidgets()->first())
			form->objectTree()->lookup( form->selectedWidgets()->first()->name() );
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

	KFormDesigner::Container *container = activeForm() ? activeForm()->activeContainer() : 0;
	if (container)
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

	enableAction("format_font", false);

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
	if (!(m_options & HideSignalSlotConnections))
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
	if (!(m_options & HideSignalSlotConnections))
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

void
FormManager::changeFont()
{
	if (!m_active)
		return;
	WidgetList *wlist = m_active->selectedWidgets();
	WidgetList widgetsWithFontProperty;
	QWidget *widget;
	QFont font;
	bool oneFontSelected = true;
	for (WidgetListIterator it(*wlist); (widget = it.current()); ++it) {
		if (m_active->library()->isPropertyVisible(widget->className(), widget, "font")) {
			widgetsWithFontProperty.append(widget);
			if (oneFontSelected) {
				if (widgetsWithFontProperty.count()==1)
					font = widget->font();
				else if (font != widget->font())
					oneFontSelected = false;
			}
		}
	}
	if (widgetsWithFontProperty.isEmpty())
		return;
	if (!oneFontSelected) //many different fonts selected: pick a font from toplevel conatiner
		font = m_active->widget()->font();

	if (1==widgetsWithFontProperty.count()) {
		//single widget's settings
		widget = widgetsWithFontProperty.first();
		KoProperty::Property &fontProp = m_propSet->property("font");
		if (QDialog::Accepted != KFontDialog::getFont(font, false, m_active->widget()))
			return;
		fontProp = font;
		return;
	}
	//multiple widgets
	int diffFlags=0;
	if (QDialog::Accepted != KFontDialog::getFontDiff(font, diffFlags, false, m_active->widget())
		|| 0==diffFlags)
		return;
	//update font
	for (WidgetListIterator it(widgetsWithFontProperty); (widget = it.current()); ++it) {
		QFont prevFont( widget->font() );
		if (diffFlags & KFontChooser::FontDiffFamily)
			prevFont.setFamily( font.family() );
		if (diffFlags & KFontChooser::FontDiffStyle) {
			prevFont.setBold( font.bold() );
			prevFont.setItalic( font.italic() );
		}
		if (diffFlags & KFontChooser::FontDiffSize)
			prevFont.setPointSize( font.pointSize() );
/*! @todo this modification is not added to UNDO BUFFER:
          do it when KoProperty::Set supports multiple selections */
		widget->setFont( prevFont );
		//temporary fix for dirty flag:
		emit dirty(m_active, true);
	}
}

#include "formmanager.moc"
