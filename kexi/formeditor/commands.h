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

#ifndef COMMANDS_H
#define COMMANDS_H

#include <qmap.h>
#include <qdict.h>
#include <qptrlist.h>
#include <qvariant.h>

#include <kcommand.h>

typedef QPtrList<QWidget> WidgetList;
class QWidget;
class QRect;
class QPoint;
class QStringList;
class QCString;
class QDomDocument;

namespace KFormDesigner {

class Container;
class Form;

/*! This command is used when changing a property for one or more widgets. \a oldvalues is a QMap
 of the old values of the property for every widget, to allow reverting the change. \a value is
 the new value of the property. You can use the simpler constructor for a single widget.
 */
class KFORMEDITOR_EXPORT PropertyCommand : public KCommand
{
	public:
		PropertyCommand(ObjectPropertyBuffer *buf, const QString &name, const QVariant &oldValue,
			const QVariant &value, const QCString &property);
		PropertyCommand(ObjectPropertyBuffer *buf, const QMap<QString, QVariant> &oldvalues,
			 const QVariant &value, const QCString &property);

		virtual void execute();
		virtual void unexecute();
		virtual QString name() const;
		QCString property() { return m_property; }

		void  setValue(const QVariant &value);

	protected:
		ObjectPropertyBuffer *m_buffer;
		QVariant   m_value;
		QMap<QString, QVariant> m_oldvalues;
		QCString    m_property;
};

/*! This command is used when moving multiples widgets at the same time, while holding Ctrl or Shift.
 You need to supply a list of widget names, and the position of the cursor before moving. Use setPos()
  to tell the new cursor pos every time it changes.*/
class KFORMEDITOR_EXPORT GeometryPropertyCommand : public KCommand
{
	public:
		GeometryPropertyCommand(ObjectPropertyBuffer *buf, const QStringList &names, QPoint oldPos);

		virtual void execute();
		virtual void unexecute();
		virtual QString name() const;

		void setPos(QPoint pos);

	protected:
		ObjectPropertyBuffer *m_buffer;
		QStringList  m_names;
		QPoint m_oldPos;
		QPoint m_pos;
};

/*! This command is used when an item in 'Align Widgets position' is selected. You just need
to give the list of widget names (the selected ones), and the
  type of alignment (see the enum for possible values). */
class KFORMEDITOR_EXPORT AlignWidgetsCommand : public KCommand
{
	public:
		AlignWidgetsCommand(int type, WidgetList &list, Form *form);

		virtual void execute();
		virtual void unexecute();
		virtual QString name() const;

		enum { AlignToGrid = 100, AlignToLeft, AlignToRight, AlignToTop, AlignToBottom };

	protected:
		Form  *m_form;
		int    m_type;
		QMap<QString, QPoint>  m_pos;
};

/*! This command is used when an item in 'Adjust Widgets Size' is selected. You just need
 to give the list of widget names (the selected ones), and the
  type of size modification (see the enum for possible values). */
class KFORMEDITOR_EXPORT AdjustSizeCommand : public KCommand
{
	public:
		AdjustSizeCommand(int type, WidgetList &list, Form *form);

		virtual void execute();
		virtual void unexecute();
		virtual QString name() const;

		enum { SizeToGrid = 200, SizeToFit, SizeToSmallWidth, SizeToBigWidth,
			SizeToSmallHeight, SizeToBigHeight };

	protected:
		Form  *m_form;
		int    m_type;
		QMap<QString, QPoint>  m_pos;
		QMap<QString, QSize>  m_sizes;
};

/*! This command is used when switching the layout of a Container. It remembers the old pos
 of every widget inside the Container. */
class KFORMEDITOR_EXPORT LayoutPropertyCommand : public PropertyCommand
{
	public:
		LayoutPropertyCommand(ObjectPropertyBuffer *buf, const QString &name,
			const QVariant &oldValue, const QVariant &value);

		virtual void execute();
		virtual void unexecute();
		virtual QString name() const;

	protected:
		Form *m_form;
		QMap<QString,QRect>  m_geometries;
};

/*! This command is used when inserting a widger using toolbar or menu. You only need to give
the parent Container and the widget pos.
 The other informations are taken from FormManager. */
class KFORMEDITOR_EXPORT InsertWidgetCommand : public KCommand
{
	public:
		InsertWidgetCommand(Container *container/*, QPoint point*/);

		virtual void execute();
		virtual void unexecute();
		virtual QString name() const;

	protected:
		Form *m_form;
		QString m_containername;
		QPoint m_point;
		QString  m_name;
		QString  m_class;
		QRect  m_insertRect;
};

/*! This command is used when creating a layout from some widgets using "Lay out in..." menu item.
 It remembers the old pos of every widget, and takes care of updating ObjectTree too. You need
 to supply a WidgetList of the selected widgets. */
class KFORMEDITOR_EXPORT CreateLayoutCommand : public KCommand
{
	public:
		CreateLayoutCommand(int layoutType, WidgetList &list, Form *form);
		CreateLayoutCommand() {;} // for BreakLayoutCommand

		virtual void execute();
		virtual void unexecute();
		virtual QString name() const;

	protected:
		Form  *m_form;
		QString  m_containername;
		QString  m_name;
		QMap<QString,QRect>  m_pos;
		int  m_type;
};

/*! This command is used when the 'Break Layout' menu item is selected. It does exactly the
 opposite of CreateLayoutCommand. */
class KFORMEDITOR_EXPORT BreakLayoutCommand : public CreateLayoutCommand
{
	public:
		BreakLayoutCommand(Container *container);

		virtual void execute();
		virtual void unexecute();
		virtual QString name() const;
};

/*! This command is used when pasting widgets. You need to give the QDomDocument containing
the widget(s) to paste, and optionnally the point where to paste widgets. */
class KFORMEDITOR_EXPORT PasteWidgetCommand : public KCommand
{
	public:
		PasteWidgetCommand(QDomDocument &domDoc, Container *container, QPoint p = QPoint());

		virtual void execute();
		virtual void unexecute();
		virtual QString name() const;

	protected:
		Form *m_form;
		QCString    m_data;
		QString     m_containername;
		QPoint      m_point;
		QStringList m_names;
};

/*! This command is used when deleting a widget using the "Delete" menu item.
You need to give a WidgetList of the selected widgets. */
class KFORMEDITOR_EXPORT DeleteWidgetCommand : public KCommand
{
	public:
		DeleteWidgetCommand(WidgetList &list, Form *form);

		virtual void execute();
		virtual void unexecute();
		virtual QString name() const;

	protected:
		QDomDocument  m_domDoc;
		Form       *m_form;
		QMap<QString, QString>  m_containers;
		QMap<QString, QString>  m_parents;
};

/*! This command is used when cutting widgets. It is basically a DeleteWidgetCommand
which also updates the clipboard contents. */
class KFORMEDITOR_EXPORT CutWidgetCommand : public DeleteWidgetCommand
{
	public:
		CutWidgetCommand(WidgetList &list, Form *form);

		virtual void execute();
		virtual void unexecute();
		virtual QString name() const;

	protected:
		QCString  m_data;
};

}

#endif


