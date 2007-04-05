/* This file is part of the KDE project
   Copyright (C) 2005 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004-2007 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KexiDBTextEdit_H
#define KexiDBTextEdit_H

#include "kexiformdataiteminterface.h"
#include "kexidbtextwidgetinterface.h"
#include "kexidbutils.h"
#include <ktextedit.h>
//Added by qt3to4:
#include <Q3CString>
#include <QPaintEvent>

//! @short Multiline edit widget for Kexi forms
class KEXIFORMUTILS_EXPORT KexiDBTextEdit :
	public KTextEdit,
	protected KexiDBTextWidgetInterface,
	public KexiFormDataItemInterface
{
	Q_OBJECT
	Q_PROPERTY(QString dataSource READ dataSource WRITE setDataSource DESIGNABLE true)
	Q_PROPERTY(Q3CString dataSourceMimeType READ dataSourceMimeType WRITE setDataSourceMimeType DESIGNABLE true)

	public:
		KexiDBTextEdit(QWidget *parent, const char *name=0);
		virtual ~KexiDBTextEdit();

		inline QString dataSource() const { return KexiFormDataItemInterface::dataSource(); }
		inline Q3CString dataSourceMimeType() const { return KexiFormDataItemInterface::dataSourceMimeType(); }
		virtual QVariant value();
		virtual void setInvalidState( const QString& displayText );

		//! \return true if editor's value is null (not empty)
		//! Used for checking if a given constraint within table of form is met.
		virtual bool valueIsNull();

		//! \return true if editor's value is empty (not necessary null).
		//! Only few data types can accept "EMPTY" property
		//! (use KexiDB::Field::hasEmptyProperty() to check this).
		//! Used for checking if a given constraint within table or form is met.
		virtual bool valueIsEmpty();

		/*! \return 'readOnly' flag for this widget. */
		virtual bool isReadOnly() const;

		/*! \return the view widget of this item, e.g. line edit widget. */
		virtual QWidget* widget();

		virtual bool cursorAtStart();
		virtual bool cursorAtEnd();
		virtual void clear();

		virtual void setColumnInfo(KexiDB::QueryColumnInfo* cinfo);

		/*! If \a displayDefaultValue is true, the value set by KexiDataItemInterface::setValue() 
		 is displayed in a special way. Used by KexiFormDataProvider::fillDataItems(). 
		 \a widget is equal to 'this'.
		 Reimplemented after KexiFormDataItemInterface. */
		virtual void setDisplayDefaultValue(QWidget* widget, bool displayDefaultValue);

		//! Windows uses Ctrl+Tab for moving between tabs, so do not steal this shortcut
		virtual void keyPressEvent( QKeyEvent *ke );

	public slots:
		inline void setDataSource(const QString &ds) { KexiFormDataItemInterface::setDataSource(ds); }
		inline void setDataSourceMimeType(const Q3CString &ds) { KexiFormDataItemInterface::setDataSourceMimeType(ds); }
		virtual void setReadOnly( bool readOnly );
		virtual void setText( const QString & text, const QString & context );

		//! Reimplemented, so "undo" means the same as "cancelEditor" action
//! @todo enable "real" undo internally so user can use ctrl+z while editing
		virtual void undo();

		//! Implemented for KexiDataItemInterface
		virtual void moveCursorToEnd();

		//! Implemented for KexiDataItemInterface
		virtual void moveCursorToStart();

		//! Implemented for KexiDataItemInterface
		virtual void selectAll();

	protected slots:
		void slotTextChanged();

	protected:
		virtual void paintEvent ( QPaintEvent * );
		virtual void setValueInternal(const QVariant& add, bool removeOld);
		QPopupMenu * createPopupMenu(const QPoint & pos);

		//! Used for extending context menu
		KexiDBWidgetContextMenuExtender m_menuExtender;

		//! Used to disable slotTextChanged()
		bool m_slotTextChanged_enabled : 1;
};

#endif
