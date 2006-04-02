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

#ifndef KEXIMACRODESIGNVIEW_H
#define KEXIMACRODESIGNVIEW_H

#include <kexiviewbase.h>

//#include <koproperty/set.h>
//#include <koproperty/property.h>

// Forward declaration.
class KexiMacroDesignViewPrivate;

/**
 * The KexiScriptDesignView implements \a KexiViewBase to provide
 * a KexiView instance for Macros.
 */
class KexiMacroDesignView : public KexiViewBase
{
		Q_OBJECT

	public:

		/**
		* Constructor.
		*/
		KexiMacroDesignView(KexiMainWindow *mainWin, QWidget *parent);

		/**
		* Destructor.
		*/
		virtual ~KexiMacroDesignView();

		/**
		* Try to call \a storeData with new data we like to store. On
		* success the matching \a KexiDB::SchemaData is returned.
		*
		* \param sdata The source \a KexiDB::SchemaData instance.
		* \param cancel Cancel on failure and don't try to clean
		*       possible temporary created data up.
		* \return The matching \a KexiDB::SchemaData instance or NULL
		*        if storing failed.
		*/
		virtual KexiDB::SchemaData* storeNewData(const KexiDB::SchemaData& sdata, bool &cancel);

		/**
		* Try to store the modified data in the already opened and
		* currently used \a KexiDB::SchemaData instance.
		*/
		virtual tristate storeData(bool dontAsk = false);

	private:
		KexiMacroDesignViewPrivate* const d;

		/**
		* Load the data from XML source and fill the internally
		* used \a Kross::Api::ScriptContainer instance.
		*/
		bool loadData();
};

#endif
