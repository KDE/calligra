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

#ifndef FORMMANAGER_H
#define FORMMANAGER_H

#include <qobject.h>
#include <qdom.h>

template<class type> class QPtrList;
class QWidget;
class QWorkspace;
class KPopupMenu;
class KexiPropertyEditor;
class KActionCollection;
class KAction;

namespace KFormDesigner {

class ObjectPropertyBuffer;
class Form;
class WidgetLibrary;
class ObjectTreeView;
typedef QPtrList<KAction> Actions;

/** Form Manager
 **/
class KFORMEDITOR_EXPORT FormManager : public QObject
{
	Q_OBJECT

	public:
		FormManager(QWorkspace *workspace, QObject *parent, const char *name);
		~FormManager(){;}

		Actions createActions(KActionCollection *parent);
		//void createForm(QWidget *toplevel);
		void loadForm(const QString &filename);

		void setEditors(KexiPropertyEditor *editor, ObjectTreeView *treeview);

		WidgetLibrary*    lib() { return m_lib; }
		bool              inserting() { return m_inserting; }
		QString           insertClass() { return m_insertClass; }

		KPopupMenu*       popupMenu() { return m_popup; }
		void              setInsertPoint(const QPoint &p);

		Form*             activeForm();
		bool              isTopLevel(QWidget *w);

		KexiPropertyEditor* editor() { return m_editor; }

		void createBlankForm(const QString &classname, const char *name);

	public slots:
		void createBlankForm();
		void loadForm();
		void saveForm();

		void deleteWidget();
		void copyWidget();
		void cutWidget();
		void pasteWidget();

		void insertWidget(const QString &classname);
		void stopInsert();

		void debugTree();
		void updateTreeView(QWidget *);

	private:
		ObjectPropertyBuffer	*m_buffer;
		WidgetLibrary		*m_lib;
		KexiPropertyEditor	*m_editor;
		ObjectTreeView		*m_treeview;
		QPtrList<Form>		m_forms;
		QWorkspace		*m_workspace;

		QDomDocument		m_domDoc;
		KPopupMenu		*m_popup;
		QPoint			m_insertPoint;

		bool			m_inserting;
		QString			m_insertClass;
};

}

#endif

