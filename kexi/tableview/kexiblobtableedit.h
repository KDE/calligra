/* This file is part of the KDE project
   Copyright (C) 2002   Peter Simonsson <psn@linux.se>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef _KEXIBLOBTABLEEDIT_H_
#define _KEXIBLOBTABLEEDIT_H_

#include <qcstring.h>

#include "kexitableedit.h"
#include "kexicelleditorfactory.h"

class KTempFile;
class KProcess;
class QTextEdit;

class KexiBlobTableEdit : public KexiTableEdit
{
	Q_OBJECT
	public:
		KexiBlobTableEdit(KexiDB::Field &f, QWidget *parent=0);
		virtual ~KexiBlobTableEdit();

		bool valueIsNull();
		bool valueIsEmpty();

		virtual QVariant value(bool &ok);

		virtual void clear();
		virtual bool cursorAtStart();
		virtual bool cursorAtEnd();
	protected slots:
		void slotFinished(KProcess* p);
		void open();
		void openWith();
		void menu();
		void loadFile();
		void saveFile();

	protected:
		//! initializes this editor with \a add value
		virtual void init(const QString& add);

		QString openWithDlg(const QString& file);

		void execute(const QString& app, const QString& file);

		KTempFile* m_tempFile;
		KProcess* m_proc;
		QTextEdit *m_content;
};

class KexiBlobEditorFactoryItem : public KexiCellEditorFactoryItem
{
	public:
		KexiBlobEditorFactoryItem();
		virtual ~KexiBlobEditorFactoryItem();

	protected:
		virtual KexiTableEdit* createEditor(KexiDB::Field &f, QWidget* parent = 0);
};

#endif
