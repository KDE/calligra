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

class QWidget;

namespace KFormDesigner {

class Container;
class ObjectPropertyBuffer;

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
		Form(QObject *parent=0, const char *name=0, WidgetLibrary *lib=0, ObjectPropertyBuffer *buf=0);
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
		 * @returns the @ref WidgetLibrary<br>
		 * this library is shared if you constructed this class with a pointer
		 * to @ref FormDesigner
		 */
		WidgetLibrary		*widgetLibrary() { return m_widgetLib; }

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

		/**
		 * @returns a list of actions assisiated with this form
		 */
		Actions			createActions(KActionCollection *parent);
		
		QWidget*		selectedWidget()  {return m_selWidget;}
		void			setSelectedWidget(QWidget *w);
		
		void			preparePaste(QWidget*w, bool cut)  {m_copiedw = w; m_cut=cut; }
		QWidget*		copiedWidget()  {return m_copiedw;}
		bool			isCutting() { return m_cut;}


	public slots:
		void			insertWidget(const QString &c);
		void			changeName(const char *oldname, const QString &newname);

	private:
		Container		*m_toplevel;
		WidgetLibrary		*m_widgetLib;
		ObjectTree		*m_topTree;
		QWidget			*m_selWidget;
		ObjectPropertyBuffer	*m_buffer;
		
		QWidget 		*m_copiedw;
		bool			m_cut;
};

}
#endif
