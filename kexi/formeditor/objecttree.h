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

namespace KFormDesigner {


class ObjectTreeItem;
class Container;
class EventEater;
typedef QPtrList<ObjectTreeItem> ObjectTreeC;
typedef QDict<ObjectTreeItem> TreeDict;
typedef QDictIterator<ObjectTreeItem> TreeDictIterator;
typedef QMap<QString, int> Names;

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
		ObjectTreeC*	children() { return &m_children; }
		QMap<QString, QVariant>* modifProp() { return &m_props;}
		Container*	container() const { return m_container;}

		void		setWidget(QWidget *w) { m_widget = w; }
		void 		setParent(ObjectTreeItem *parent)  { m_parent = parent;}

		void		debug(int ident);
		virtual void	rename(const QString &name);

		virtual void	addChild(ObjectTreeItem *it);
		void 		remChild(ObjectTreeItem *it);

		/*! Adds \a property in the list of the modified properties for this object.
		    These modified properties are written in the .ui files when saving the form.
		*/
		void		addModProperty(const QString &property, const QVariant &value);

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
		ObjectTreeC	m_children;
		QGuardedPtr<Container> m_container;
		QMap<QString, QVariant> m_props;
		QMap<QString, QString>  m_pixmapNames;
		ObjectTreeItem* m_parent;
		QGuardedPtr<QWidget> m_widget;
		QGuardedPtr<EventEater> m_eater;

		int m_row, m_col, m_rowspan, m_colspan;
		bool m_span;
};

//! A class representing all the objects in a Form
/*! This class holds ObjectTreeItem for each widget in a Form.
 */
class KFORMEDITOR_EXPORT ObjectTree : public ObjectTreeItem
{
	public:
		ObjectTree(const QString &className=QString::null, const QString &name=QString::null, QWidget *widget=0, Container *container=0);
		virtual ~ObjectTree();

		/**
		 * renames a item and returns false if name is doublicated
		 */
		virtual bool	rename(const QString &oldname, const QString &newname );
		/*! Sets \a newparent as new parent for the item whose name is \a name. */
		bool		reparent(const QString &name, const QString &newparent);

		ObjectTreeItem	*lookup(const QString &name);
		TreeDict	*dict() { return &m_treeDict; }

		void		addChild(ObjectTreeItem *parent, ObjectTreeItem *c);
		void		addChild(ObjectTreeItem *c);

		void		removeChild(const QString &);

		/*! Generates a new name with \a base as beginning (eg if base is "QLineEdit", it returns "QLineEdit1"). */
		QString		genName(const QString &base);

		void		debug();

	private:
		TreeDict	m_treeDict;
		Names		m_names;
};

}

#endif
