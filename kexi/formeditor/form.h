/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>

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

#ifndef KFORMDESIGNERFORM_H
#define KFORMDESIGNERFORM_H


#include <qobject.h>
#include "objecttree.h"
#include "widgetlibrary.h"
#include "resizehandle.h"

class QWidget;
class QDomElement;

namespace KFormDesigner {

class Container;
class ObjectPropertyBuffer;
class FormManager;

/**
 * This class provides the base for accessing KFormDesigner over an "forign" api
 * and makes all parts of KFormDesigner working together.
 * it holds e.g. the toplevel container, the property buffer the widget library
 * (if no FormDesiger ptr was overgiven)
 **/
class KFORMEDITOR_EXPORT Form : public QObject
{
	Q_OBJECT

	public:
		Form(FormManager *manager, const char *name=0);
		~Form();

		/**
		 * creates a toplevel widget out of another widget
		 * @code QWidget *toplevel = new QWidget(this);
		 * @code form->createToplevel(toplevel);
		 */
		void	createToplevel(QWidget *container);

		/**
		 * creates a toplevel widget using the library
		 */
		QWidget* createToplevel(const QString &w, QWidget *parent=0, const char *name=0);

		/**
		 * @returns the TopLevel container or 0 if there isn't any
		 */
		Container			*toplevelContainer() { return m_toplevel; }

		/**
		 * creates an instance of this form by using
		 * its Property- and WidgetBuffer
		 * @param editmode = false you can't edit that form
		 */
		QWidget			*createInstance(QWidget *parent, bool editmode);

		/**
		 * creates an *empty* instance
		 */
		QWidget			*createEmptyInstance(const QString &c="QWidget", QWidget *parent=0);

		/**
		 * @returns a ptr of this form's objecttree
		 */
		ObjectTree		*objectTree() { return m_topTree; }
		FormManager*		manager() { return m_manager; }
		
		QWidget*		selectedWidget()  {return m_selWidget;}
		Container*		activeContainer();
		void			setCurrentWidget(QWidget *w);
		void			setInteractiveMode(bool interactive) { m_inter = interactive; }

		bool			interactiveMode() { return m_inter; }

		static int		gridX() { return 10;}
		static int		gridY() { return 10;}
		
		void			pasteWidget(QDomElement &widg, QPoint pos=QPoint());

	public slots:
		void			changeName(const char *oldname, const QString &newname);

	signals:
		void			selectionChanged(QWidget *w);

	protected:
		void  fixPos(QDomElement el, QPoint newpos);
		void  fixNames(QDomElement el);

	private:
		FormManager		*m_manager;
		Container		*m_toplevel;
		ObjectTree		*m_topTree;
		
		QWidget			*m_selWidget;
		ResizeHandleSet		*m_resizeHandles;

		bool			m_inter;
};

}
#endif
