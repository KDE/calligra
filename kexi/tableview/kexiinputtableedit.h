/* This file is part of the KDE project
   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
 */

#ifndef KEXIINPUTTABLEEDIT_H
#define KEXIINPUTTABLEEDIT_H

#include <klineedit.h>
#include <qvariant.h>

#include "kexitableedit.h"
#include "kexicelleditorfactory.h"

class KEXIDATATABLE_EXPORT KexiInputTableEdit : public KexiTableEdit
{
	Q_OBJECT

	public:
		KexiInputTableEdit(KexiDB::Field &f, QWidget *parent=0, const char* name = 0);

		virtual ~KexiInputTableEdit();

		virtual bool valueChanged();

		//! \return true is editor's value is null (not empty)
		virtual bool valueIsNull();
		//! \return true is editor's value is empty (not null). 
		//! Only few field types can accept "EMPTY" property 
		//! (check this with KexiDB::Field::hasEmptyProperty()), 
		virtual bool valueIsEmpty();

		virtual QVariant value(bool &ok);

		virtual bool cursorAtStart();
		virtual bool cursorAtEnd();

//		virtual bool eventFilter(QObject* watched, QEvent* e);
//js		void end(bool mark);
//js		void backspace();
		virtual void clear();

	protected slots:
		void setRestrictedCompletion();
		void completed(const QString &);

	protected:
		//! initializes this editor with \a add value
		virtual void init(const QString& add);

		void showHintButton();
		void init();

		virtual void paintEvent ( QPaintEvent *e );
	//virtual void drawFrame ( QPainter * p );


		bool		m_calculatedCell;
//js		QStringList	m_comp;

		QString m_decsym; //! decimal symbol
		QString m_origText; //! orig. Line Edit's text after conversion - for easy comparing

		KLineEdit	*m_lineedit;

	signals:
		void hintClicked();
};

class KexiInputEditorFactoryItem : public KexiCellEditorFactoryItem
{
	public:
		KexiInputEditorFactoryItem();
		virtual ~KexiInputEditorFactoryItem();

	protected:
		virtual KexiTableEdit* createEditor(KexiDB::Field &f, QWidget* parent = 0);
};

#endif
