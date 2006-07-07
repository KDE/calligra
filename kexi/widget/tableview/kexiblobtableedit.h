/* This file is part of the KDE project
   Copyright (C) 2002 Peter Simonsson <psn@linux.se>
   Copyright (C) 2004, 2006 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _KEXIBLOBTABLEEDIT_H_
#define _KEXIBLOBTABLEEDIT_H_

#include <qcstring.h>
#include <qcache.h>

#include "kexitableedit.h"
#include "kexicelleditorfactory.h"

class KTempFile;
class KProcess;
class QTextEdit;

class KexiBlobTableEdit : public KexiTableEdit
{
	Q_OBJECT
	public:
		KexiBlobTableEdit(KexiTableViewColumn &column, QScrollView *parent=0);
		virtual ~KexiBlobTableEdit();

		bool valueIsNull();
		bool valueIsEmpty();

		virtual QVariant value();

		virtual void clear();
		virtual bool cursorAtStart();
		virtual bool cursorAtEnd();
//	protected slots:
//		void slotFinished(KProcess* p);
//		void open();
//		void openWith();
//		void menu();
//		void loadFile();
//		void saveFile();

		/*! Reimplemented: resizes a view(). */
		virtual void resize(int w, int h);

		virtual void showFocus( const QRect& r, bool readOnly );

		virtual void hideFocus();

		/*! \return total size of this editor, including popup button. */
		virtual QSize totalSize() const;

	protected:
		//! initializes this editor with \a add value
		virtual void setValueInternal(const QVariant& add, bool removeOld);

		virtual void setupContents( QPainter *p, bool focused, const QVariant& val, 
			QString &txt, int &align, int &x, int &y_offset, int &w, int &h );

		//todo QString openWithDlg(const QString& file);
		//todo void execute(const QString& app, const QString& file);

		//! internal
		void updateFocus( const QRect& r );

		class Private;
		Private *d;
//todo		KTempFile* m_tempFile;
//todo		KProcess* m_proc;
//todo		QTextEdit *m_content;
};

KEXI_DECLARE_CELLEDITOR_FACTORY_ITEM(KexiBlobEditorFactoryItem)


//=======================
//This class is temporarily here:

/*! @short Cell editor for displaying kde icon (using icon name provided as string).
 Read only.
*/
class KexiKIconTableEdit : public KexiTableEdit
{
	public:
		KexiKIconTableEdit(KexiTableViewColumn &column, QScrollView *parent=0);

		virtual ~KexiKIconTableEdit();

		//! \return true if editor's value is null (not empty)
		virtual bool valueIsNull();

		//! \return true if editor's value is empty (not null). 
		//! Only few field types can accept "EMPTY" property 
		//! (check this with KexiDB::Field::hasEmptyProperty()), 
		virtual bool valueIsEmpty();

		virtual QVariant value();

		virtual bool cursorAtStart();
		virtual bool cursorAtEnd();

		virtual void clear();

		virtual void setupContents( QPainter *p, bool focused, const QVariant& val, 
			QString &txt, int &align, int &x, int &y_offset, int &w, int &h );

	protected:
		//! initializes this editor with \a add value
		virtual void setValueInternal(const QVariant& add, bool removeOld);

		void showHintButton();
		void init();

		class Private;
		Private *d;
};

KEXI_DECLARE_CELLEDITOR_FACTORY_ITEM(KexiKIconTableEditorFactoryItem)

#endif
