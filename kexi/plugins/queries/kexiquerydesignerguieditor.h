/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIQUERYDESIGNERGUIEDITOR_H
#define KEXIQUERYDESIGNERGUIEDITOR_H

#include <qwidget.h>
#include <qvaluelist.h>
#include <qmap.h>

#include "kexiquerydesigner.h"

class QGridLayout;
class QFrame;
class QComboBox;
class QPushButton;
class KexiDB;
class KexiTableView;
class KexiTableItem;
class KexiRelationDialog;

struct InvolvedTable
{
	QString		table;
	QStringList	relationList;
	int		involveCount;
};

struct JoinField
{
	QString		sourceField;
	QString		eqLeft;
	QString		eqRight;
};

struct Condition
{
	QString		field;
	QString		orCondition;
	QString		andCondition;
};

typedef QMap<QString, InvolvedTable> InvolvedTables;
typedef QValueList<JoinField> JoinFields;
typedef QValueList<Condition> ConditionList;

class KexiParameterListEditor;

class KexiQueryDesignerGuiEditor : public QWidget
{
	Q_OBJECT

	public:
		KexiQueryDesignerGuiEditor(KexiView *view,QWidget *parent, KexiQueryDesigner *myparent, const char *name=0);
		~KexiQueryDesignerGuiEditor();

		QString			getQuery();
		QString			getParam(const QString &name, bool escape);

		KexiTableView		*table()const { return m_designTable; };

		QStringList		sourceList() { return m_sourceList; };

		void clear();
		void appendLine(const QString &source, const QString &field, bool show, const QString &andC, 
				const QString &orC);


	protected slots:
		void			slotDropped(QDropEvent *ev);
		void			slotItemChanged(KexiTableItem *item, int col);
		void			slotAddParameter();
		void			slotRemoveParameter();

	private:
		KexiDB			*m_db;

		KexiView		*m_view;
		QWidget			*m_tables;
		KexiTableView		*m_designTable;
		KexiParameterListEditor *m_paramList;


		KexiQueryDesigner	*m_parent;

		KexiTableItem		*m_insertItem;
		QStringList		m_sourceList;
//		ConditionList		m_conditions;

//		InvolvedTables		m_involvedTables;
};

#endif
