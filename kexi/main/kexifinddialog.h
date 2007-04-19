/* This file is part of the KDE project
   Copyright (C) 2004-2007 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXIFINDDIALOG_H
#define KEXIFINDDIALOG_H

#include "kexifinddialogbase.h"
#include <core/kexisearchandreplaceiface.h>

class KAction;

//! @short A Kexi-specific "Find text" dialog.
/*! Also used for as replace dialog. 

 @todo replace m_textToFind and m_textToReplace KComboBoxes with Kexi's db-aware comboboxes,
       so we ca adapt to datatype being searched, e.g. date, time and numbers
*/
class KexiFindDialog : public KexiFindDialogBase
{
	Q_OBJECT
	public:
		//! Creates a new find dialog. Replace mode is off by default.
		KexiFindDialog(QWidget* parent);
		virtual ~KexiFindDialog();

		//! Sets actions that receive button clicks and shortcuts within the dialog. Should be called once.
		void setActions( KAction *findnext, KAction *findprev,
			KAction *replace, KAction *replaceall );

		//! Shows the dialog as a modal dialog. 
		virtual void show();

		//! \return current find and replace options set within the dialog
//! @todo should we have setOptions() too?
		KexiSearchAndReplaceViewInterface::Options options() const;

		/*! \return a list of column names for 'look in column' combo box. 
		 Neither "(All fields)" nor "(Current field)" items are prepended. */
		QStringList lookInColumnNames() const;

		/*! \return a list of column captions (i.e. visible values) for 'look in column' combo box. 
		 Neither "(All fields)" nor "(Current field)" items are prepended. */
		QStringList lookInColumnCaptions() const;

		/*! \return column name selected in "look in column" combo box.
		 If "(All fields)" item is selected, empty string is returned. 
		 If "(Current field)" item is selected, "(field)" string is returned. */
		QString currentLookInColumnName() const;

		//! \return value that to be used for searching
		QVariant valueToFind() const;

		//! \return value that to be used as a replacement
		QVariant valueToReplaceWith() const;

	public slots:
		/*! Sets \a columnNames list and \a columnCaptions for 'look in column' combo box. 
		 \a columnCaptions are visible values, while \a columnNames are used for returning
		 in currentLookInColumn().
		 "(All fields)" and "(Current field)" items are also prepended. */
		void setLookInColumnList(const QStringList& columnNames, 
			const QStringList& columnCaptions);

		/*! Selects \a columnName to be selected 'look in column'.
		 By default "(All fields)" item is selected. To select this item, 
		 pass empty string as \a columnName.
		 To select "(Current field)" item, "(field)" string should be passed 
		 as \a columnName. */
		void setCurrentLookInColumnName(const QString& columnName);

		/*! Sets or clears replace mode. 
		 For replace mode 'prompt or replace' option is visible. */
		void setReplaceMode(bool set);

		/*! Sets object name for caption, so for example it will be set 
		 to i18n("Find \"Persons\"")). */
		void setObjectNameForCaption(const QString& name);

		/*! Enables of disables the find/replace/replace all buttons. 
		 This is used if for the current context the dialog could not be used. 
		 If \a enable is false, object name for caption is cleared 
		 using setObjectNameForCaption() too. */
		void setButtonsEnabled(bool enable);

		/*! Sets message at the bottom to \a message. */
		void setMessage(const QString& message);

		/*! Updates message at the bottom; "The search item was not found" is set if \a found is true,
		 else the message is cleared. */
//! @todo add "Search again" hyperlink
		void updateMessage( bool found = true );

	signals:
		//! Emitted after clicking "Find next" button or pressing appropriate shortcut set by setActions()
		void findNext();
		
		//! Emitted after pressing appropriate shortcut set by setActions()
		void findPrevious();

		//! Emitted after clicking "Replace" button or pressing appropriate shortcut set by setActions()
		void replaceNext();

		//! Emitted after clicking "Replace All" button or pressing appropriate shortcut set by setActions()
		void replaceAll();

	protected slots:
		void slotCloseClicked();

	protected:
		class Private;
		Private * const d;
};

#endif
