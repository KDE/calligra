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

#ifndef KFORMDESIGNEROBJECTTREE_H
#define KFORMDESIGNEROBJECTTREE_H

#include <qptrlist.h>
#include <qmap.h>
#include <qdict.h>
#include <qvariant.h>
#include <qstring.h>
#include <qguardedptr.h>

class QWidget;
class QDomElement;

namespace KFormDesigner {

class ObjectTreeItem;
class Container;
class EventEater;
typedef QPtrList<ObjectTreeItem> ObjectTreeList;
typedef QDict<ObjectTreeItem> ObjectTreeDict;
typedef QDictIterator<ObjectTreeItem> ObjectTreeDictIterator;
typedef QMap<QString, QVariant> QVariantMap;

/*! This class holds the properties of a widget (classname, name, parent, children ..).
    \author Lucijan Busch <lucijan@kde.org>
 */
 //! An item representing a widget
class KFORMEDITOR_EXPORT ObjectTreeItem
{

	public:
		ObjectTreeItem(const QString &className, const QString &name, QWidget *widget, Container *parentContainer, Container *container=0);
		virtual ~ObjectTreeItem();

		QString		name() const { return m_name; }
		QString		className() const { return m_className; }
		QWidget*	widget() const { return m_widget; }
		EventEater*     eventEater() const { return m_eater; }
		ObjectTreeItem* parent() const { return m_parent; }
		ObjectTreeList*	children() { return &m_children; }
		/*! \return a QMap<QString, QVariant> of all modified properties for this widget.
		  The QVariant is the old value (ie first value) of the property whose name is the QString. */
		const QVariantMap* modifiedProperties() { return &m_props;}
		//! \return the widget's Container, or 0if the widget is not a Container.
		Container*	container() const { return m_container;}

		void		setWidget(QWidget *w) { m_widget = w; }
		void 		setParent(ObjectTreeItem *parent)  { m_parent = parent;}

		void		debug(int ident);
		void		rename(const QString &name);

		void		addChild(ObjectTreeItem *it);
		void 		removeChild(ObjectTreeItem *it);

		/*! Adds \a property in the list of the modified properties for this object.
		    These modified properties are written in the .ui files when saving the form.
		*/
		void		addModifiedProperty(const QString &property, const QVariant &value);
		void		storeUnknownProperty(QDomElement &el);

		void		addPixmapName(const QString &property, const QString &name);
		QString		pixmapName(const QString &property);

		int gridRow() const { return m_row; }
		int gridCol() const { return m_col; }
		int gridRowSpan() const { return m_rowspan; }
		int gridColSpan() const { return m_colspan; }
		bool spanMultipleCells() const { return m_span; }
		void setGridPos(int row, int col, int rowspan, int colspan);

	protected:
		QString		m_className;
		QString		m_name;
		ObjectTreeList	m_children;
		QGuardedPtr<Container> m_container;
		QMap<QString, QVariant> m_props;
		QString  m_unknownProps;
		QMap<QString, QString>  m_pixmapNames;
		ObjectTreeItem* m_parent;
		QGuardedPtr<QWidget> m_widget;
		QGuardedPtr<EventEater> m_eater;

		int m_row, m_col, m_rowspan, m_colspan;
		bool m_span;

		friend class ObjectTree;
		friend class FormIO;
};

//! A class representing all the objects in a Form
/*! This class holds ObjectTreeItem for each widget in a Form.
 */
class KFORMEDITOR_EXPORT ObjectTree : public ObjectTreeItem
{
	public:
		ObjectTree(const QString &className=QString::null, const QString &name=QString::null, QWidget *widget=0, Container *container=0);
		virtual ~ObjectTree();

		/*! Renames the item named \a oldname to \a newname. \return false if widget named \a newname
		 already exists and renaming failed. */
		bool		rename(const QString &oldname, const QString &newname );
		/*! Sets \a newparent as new parent for the item whose name is \a name. */
		bool		reparent(const QString &name, const QString &newparent);

		/*! \return the ObjectTreeItem named \a name, or 0 if doesn't exist. */
		ObjectTreeItem*	lookup(const QString &name);
		/*! \return a dict containing all ObjectTreeItem in this ObjectTree. If you want to iterate on
		this dict, iterate on a copy. */
		const ObjectTreeDict*	dict() { return &m_treeDict; }

		void		addItem(ObjectTreeItem *parent, ObjectTreeItem *c);
		void		removeItem(const QString &name);

		/*! Generates a new name with \a base as beginning (eg if base is "QLineEdit", it returns "QLineEdit1"). */
		QString		genName(const QString &base);

	private:
		ObjectTreeDict	m_treeDict;
		//QMap<QString, int>	m_names;
};

}

#endif
