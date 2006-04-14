/* This file is part of the KDE project
   Copyright (C) 2006 Sebastian Sauer <mail@dipe.org>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KEXIMACROTEXTVIEW_H
#define KEXIMACROTEXTVIEW_H

#include "keximacroview.h"

// Forward declaration.
namespace KoMacro {
	class Macro;
}

/**
 * The KexiMacroTextView implements \a KexiMacroView to provide
 * a simple texteditor to edit the XML document of a Macro.
 */
class KexiMacroTextView : public KexiMacroView
{
		Q_OBJECT
	public:

		/**
		* Constructor.
		*
		* \param mainwin The \a KexiMainWindow instance this \a KexiViewBase
		*        belongs to.
		* \param parent The parent widget this widget should be displayed in.
		* \param macro The \a KoMacro::Macro instance this view is for.
		*/
		KexiMacroTextView(KexiMainWindow *mainwin, QWidget *parent, ::KoMacro::Macro* const macro);

		/**
		* Destructor.
		*/
		virtual ~KexiMacroTextView();

		/**
		* Load the data and display it in the editor.
		*/
		virtual bool loadData();

		/**
		* Try to store the modified data in the already opened and
		* currently used \a KexiDB::SchemaData instance.
		*/
		virtual tristate storeData(bool dontAsk = false);

	private slots:

		/**
		* This slot got called if the text of the editor changed.
		*/
		void editorChanged();

	private:
		/// \internal d-pointer class.
		class Private;
		/// \internal d-pointer instance.
		Private* const d;
};

#endif
