/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
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

#ifndef KFORMDESIGNERWIDGETFACTORY_H
#define KFORMDESIGNERWIDGETFACTORY_H


#include <qobject.h>
#include <qptrlist.h>
#include <qguardedptr.h>
#include <qpixmap.h>
#include <qpopupmenu.h>

// class QPixmap;

class QWidget;
class KLineEdit;
class QDomElement;
class QDomDocument;
class QVariant;
class QListView;

namespace KFormDesigner {

class WidgetFactory;
class Container;
class ResizeHandleSet;
class ObjectTreeItem;

/**
 * this class holds properties of widgets
 */
class KFORMEDITOR_EXPORT Widget
{
	public:
		Widget(WidgetFactory *f=0) {m_factory = f; }
		virtual ~Widget() { }

		/**
		 * returns a pixmap associated with the widget
		 */
		virtual QString	pixmap() { return m_pixmap; }

		/**
		 * returns the class name of a widget e.g. 'QLineEdit'
		 */
		virtual QString	className() { return m_class; }

		/**
		 * returns the real name e.g. 'Line Edit'
		 */
		virtual QString	name() { return m_name; }

		virtual QString description() { return m_desc; }

		virtual QString  includeFile() { return m_include; }

		virtual QString  alternateClassName() { return m_alternate; }

		virtual WidgetFactory *factory() { return m_factory; }

		void		setPixmap(const QString &p) { m_pixmap = p; }
		void		setClassName(const QString &s) { m_class = s; }
		void		setName(const QString &n) { m_name = n; }
		void		setDescription(const QString &desc) { m_desc = desc;}
		/*! Sets the C++ include file corresponding to this class, that uic will need to add when creating the file. */
		void		setInclude(const QString &include) { m_include = include;}
		/*! Sets an alternate name for this class. If this name is found when loading a .ui file, the className()
		 will be used instead. It allows to support both KDE and Qt versions of widget, without duplicating code. */
		void		setAlternateClassName(const QString &alternate) { m_alternate = alternate; }

	private:
		QString		m_pixmap;
		QString		m_class;
		QString		m_name;
		QString		m_desc;
		QString		m_include;
		QString		m_alternate;
		WidgetFactory	*m_factory;

};

typedef QPtrList<Widget> WidgetList;

/*! This helper function install an event filter on \a object and all of its children, directed to \a container.
  This is necessary to filter events for composed widgets. */
void KFORMEDITOR_EXPORT installRecursiveEventFilter(QObject *object, QObject *container);
/**
 * this is a poor virtual class, used for making widgets
 * avaible to the WidgetLibrary.
 * you can either make a KPart module out of it or call @ref WidgetLibrary::addFactory()
 */
class KFORMEDITOR_EXPORT WidgetFactory : public QObject
{
	Q_OBJECT
	public:
		WidgetFactory(QObject *parent=0, const char *name=0);
		virtual ~WidgetFactory();

		/**
		 * returns the name of the factory
		 */
		virtual QString		name()=0;

		/**
		 * retruns all classes, which are provided by this factory
		 */
		virtual	WidgetList	classes()=0;

		/**
		 * creates a widget (and if needed a container)
		 * @returns the created widgets
		 * @param classname the classname of the widget, which should get created
		 * @param parent the parent for the created widgets
		 * @param name the name of the created widget
		 * @param toplevel the toplevelcontainer (if a container should get created)
		 */
		virtual QWidget*	create(const QString &classname, QWidget *parent, const char *name,
					 KFormDesigner::Container *container)=0;

		virtual bool		createMenuActions(const QString &classname, QWidget *w, QPopupMenu *menu, KFormDesigner::Container *container)=0;

		/*! Creates (if necessary) an editor to edit the contents of the widget directly in the Form
		   (eg creates a line edit to change the text of a label). \a classname is the class the widget belongs to, \a w is the widget to edit
		   and \a container is the parent container of this widget (to access Form etc.).
		 */
		virtual void		startEditing(const QString &classname, QWidget *w, Container *container)=0;
		/*! This function is called just before the Form is previewed. It allows widgets to make changes before switching
		 (ie for a Spacer, hiding the cross) */
		virtual void		previewWidget(const QString &classname, QWidget *widget, Container *container)=0;

		/*! This function is called when FormIO finds a property, at save time, that it cannot handle (ie not a normal property).
		This way you can save special properties, for example the contents of a listbox.
		  \sa readSpecialProperty()
		 */
		virtual void	saveSpecialProperty(const QString &classname, const QString &name, const QVariant &value, QWidget *w,
		         QDomElement &parentNode,  QDomDocument &parent)=0;
		/*! This function is called when FormIO fins a property or an unknown element in a .ui file. You can this way load a special property, for
		  example the contents of a listbox.
		   \sa saveSpecialProperty()
		*/
		virtual void            readSpecialProperty(const QString &classname, QDomElement &node, QWidget *w, ObjectTreeItem *item)=0;
		/*! This function is used to know whether the \a property for the widget \a w should be shown or not in the PropertyEditor.
		  If \a multiple is true, then multiple widgets of the same class are selected, and you should only show properties shared by widgets
		  (eg font, color). By default, all properties are shown if multiple == true, and none if multiple == false.
		*/
		virtual bool		showProperty(const QString &classname, QWidget *w, const QString &property, bool multiple) { return !multiple; }
		/*! You need to return here a list of the properties that should automatically be saved for a widget belonging to \a classname,
		 and your custom properties (eg "text" for label or button, "contents" for combobox...). */
		virtual QStringList     autoSaveProperties(const QString &classname)=0;

	protected:
		/*! This function creates a KLineEdit to input some text and edit a widget's contents. This can be used in startEditing().
		   \a text is the text to display by default in the line edit, \a w is the edited widget, \a geometry is the geometry the new line
		   edit should have, and \a align is Qt::AlignmentFlags of the new line edit.
		 */
		virtual KLineEdit*	createEditor(const QString &text, QWidget *w, QRect geometry, int align,  bool useFrame=false,
		     BackgroundMode background = Qt::NoBackground);
		/*! This function provides a simple editing mode : it justs disable event filtering for the widget, and it install it again when
		  the widget loose focus or Enter is pressed.
		*/
		virtual void     disableFilter(QWidget *w, Container *container);
		/*! This function creates a little dialog (a KEditListBox) to modify the contents of a list (of strings). It can be used to modify the contents
		 of a combo box for instance. The modified list is copied into \a list when the user presses "Ok".*/
		virtual bool     editList(QWidget *w, QStringList &list);
		/* This function creates a little editor to modify rich text. It supports alignment, subscript and superscript and all basic formatting properties.
		  If the user presses "Ok", the edited text is put in \a text. If he presses "Cancel", nothing happens. */
		bool  editRichText(QWidget *w, QString &text);
		/*! This function creates a dialog to modify the contents of a ListView. You can modify both columns and list items. The listview is
		 automatically  updated if the user presses "Ok".*/
		void  editListView(QListView *listview);

		/*! This function destroys the editor when it looses focus or Enter is pressed. */
		virtual bool  eventFilter(QObject *obj, QEvent *ev);
		/*! This function is used to modify a property of a widget (eg after editing it). Please use it instead of w->setProperty()
		 to allow sync inside PropertyEditor.
		*/
		virtual void  changeProperty(const char *name, const QVariant &value, Container *container);
		/*! Adds the i18n'ed description of a property, which will be shown in PropertyEditor. */

		virtual void  addPropertyDescription(Container *container, const char *prop, const QString &desc);
		/*! Adds the i18n'ed description of a property value, which will be shown in PropertyEditor. */
		virtual void  addValueDescription(Container *container, const char *value, const QString &desc);

	protected slots:
		/*! You have to implement this function for editing inside the Form to work. This slot is called when the line edit text changes,
		  and you have to make it really change the good property of the widget using changeProperty() (text, or title, etc.).
		 */
		virtual void  changeText(const QString &newText){;}
		/*! This slot is called when the editor has lost focus or the user pressed Enter. It destroys the editor or installs
		 again the event filter on the widget. */
		virtual void  resetEditor();
		/*! This slot is called when the editor is destroyed.*/
		virtual void  editorDeleted();

	protected:
		QGuardedPtr<QWidget> m_widget;
		QGuardedPtr<KLineEdit>  m_editor;
		QGuardedPtr<ResizeHandleSet>  m_handles;
		QGuardedPtr<Container>      m_container;
};

}
#endif
