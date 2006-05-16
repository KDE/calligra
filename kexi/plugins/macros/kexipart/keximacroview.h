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

#ifndef KEXIMACROVIEW_H
#define KEXIMACROVIEW_H

#include <kexiviewbase.h>

// Forward declarations.
namespace KoMacro {
	class Macro;
}
namespace KoProperty {
	class Property;
}
namespace KexiDB {
	class ResultInfo;
}
class KexiTableItem;

/**
 * The KexiMacroView implements \a KexiViewBase to provide
 * a base KexiView instance for Macros.
 *
 * The \a KexiMacroDesignView and the \a KexiMacroTextView
 * are inherited from this class.
 */
class KexiMacroView : public KexiViewBase
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
		KexiMacroView(KexiMainWindow *mainwin, QWidget *parent, ::KoMacro::Macro* const macro, const char* name = 0);

		/**
		* Destructor.
		*/
		virtual ~KexiMacroView();

		/**
		* \return the Macro instance.
		*/
		KSharedPtr<KoMacro::Macro> macro() const;

		/**
		* Load the data from XML source and fill the internally
		* used \a KoMacro::Macro instance.
		*/
		virtual bool loadData();

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

	public slots:

		/**
		* This slot will be invoked if Kexi's menuitem Data=>Execute
		* got activated and will execute the Macro.
		*/
		void execute(QObject* sender = 0);

	protected:

		/**
		* Called by \a KexiDialogBase::switchToViewMode() right before dialog
		* is switched to new mode.
		*
		* \param mode The viewmode to which should be switched. This
		*        could be either Kexi::DataViewMode, Kexi::DesignViewMode
		*        or Kexi::TextViewMode.
		* \param donstore This call-by-reference boolean value defines
		*        if \a storeData should be called for the old but still
		*        selected viewmode. Set \a dontstore to true (it's false
		*        by default) if you want to avoid data storing.
		* \return true if you accept or false if a error occupied and view
		*         shouldn't change. If there is no error but switching 
		*         should be just cancelled (probably after showing some 
		*         info messages), you need to return cancelled.
		*/
		virtual tristate beforeSwitchTo(int mode, bool& dontstore);

		/**
		* Called by \a KexiDialogBase::switchToViewMode() right after dialog
		* is switched to new mode.
		*
		* \param mode The viewmode to which we switched. This could
		*        be either Kexi::DataViewMode, Kexi::DesignViewMode
		*        or Kexi::TextViewMode.
		* \return true if you accept or false if a error occupied and view
		*         shouldn't change. If there is no error but switching
		*         should be just cancelled (probably after showing
		*         some info messages), you need to return cancelled.
		*/
		virtual tristate afterSwitchFrom(int mode);

	private:
		/// \internal d-pointer class.
		class Private;
		/// \internal d-pointer instance.
		Private* const d;
};

#endif
