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

class PropertyCommand : public KCommand
{
	public:
		PropertyCommand(ObjectPropertyBuffer *buf, const QString &name, const QVariant &oldValue, const QVariant &value, const QString &property);
		PropertyCommand(ObjectPropertyBuffer *buf, const QMap<QString, QVariant> &oldvalues, const QVariant &value, const QString &property);

		virtual void execute();
		virtual void unexecute();
		virtual QString name() const;
		QString property() { return m_property; }

		void  setValue(const QVariant &value);

	protected:
		ObjectPropertyBuffer *m_buffer;
		QVariant   m_value;
		QMap<QString, QVariant> m_oldvalues;
		QString    m_property;
};

class GeometryPropertyCommand : public KCommand
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

class LayoutPropertyCommand : public PropertyCommand
{
	public:
		LayoutPropertyCommand(ObjectPropertyBuffer *buf, const QString &name, const QVariant &oldValue, const QVariant &value);

		virtual void execute();
		virtual void unexecute();
		virtual QString name() const;

	protected:
		Form *m_form;
		QMap<QString,QRect>  m_geometries;
};


class InsertWidgetCommand : public KCommand
{
	public:
		InsertWidgetCommand(Container *container, QPoint point);

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

class PasteWidgetCommand : public KCommand
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

class DeleteWidgetCommand : public KCommand
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
};

class CutWidgetCommand : public DeleteWidgetCommand
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


