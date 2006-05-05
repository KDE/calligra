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

#ifndef KEXIMACROPART_H
#define KEXIMACROPART_H

//#include <qcstring.h>

#include <kexi.h>
#include <kexipart.h>
#include <kexidialogbase.h>

/**
 * Kexi Macro Plugin.
 */
class KexiMacroPart : public KexiPart::Part
{
		Q_OBJECT

	public:

		/**
		* Constructor.
		*
		* \param parent The parent QObject this part is child of.
		* \param name The name this part has.
		* \param args Optional list of arguments passed to this part.
		*/
		KexiMacroPart(QObject *parent, const char *name, const QStringList& args);

		/**
		* Destructor.
		*/
		virtual ~KexiMacroPart();

		/**
		* Implementation of the KexiPart::Part::action method used to
		* provide scripts as KAction's to the outside world.
		*/
		virtual bool execute(KexiPart::Item* item, QObject* sender = 0);

		/**
		* \return the i18n message for the passed \p englishMessage string.
		*/
		virtual QString i18nMessage(const QCString& englishMessage) const;

	protected:

		/**
		* Create a new view.
		*
		* \param parent The parent QWidget the new view is displayed in.
		* \param dialog The \a KexiDialogBase the view is child of.
		* \param item The \a KexiPart::Item this view is for.
		* \param viewMode The viewmode we like to have a view for.
		*/
		virtual KexiViewBase* createView(QWidget *parent,
		                                 KexiDialogBase* dialog,
		                                 KexiPart::Item& item,
		                                 int viewMode = Kexi::DesignViewMode,
		                                 QMap<QString,QString>* staticObjectArgs = 0);

		/**
		* Initialize the part's actions.
		*/
		virtual void initPartActions();

		/**
		* Initialize the instance actions.
		*/
		virtual void initInstanceActions();

	private:
		/// \internal d-pointer class.
		class Private;
		/// \internal d-pointer instance.
		Private* const d;
};

#endif

