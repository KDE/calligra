/***************************************************************************
 * This file is part of the KDE project
 * copyright (C) 2006 by Tobi Krebs (tobi.krebs@gmail.com)
 * copyright (C) 2006 by Bernd Steindorff (bernd@itii.de)
 * copyright (C) 2006 by Sascha Kupper (kusato@kfnv.de)
 * copyright (C) 2006 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#ifndef KEXIMACROERROR_H
#define KEXIMACROERROR_H

#include <qwidget.h>
#include <qlabel.h>
#include <qlistview.h>
#include <qpushbutton.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <kdebug.h>

#include "../lib/context.h"
#include "../lib/exception.h"
#include "../lib/macro.h"
#include "../lib/macroitem.h"

#include "keximacroerrorbase.h"

/**
* An error dialog used to displayed more detailed informations about
* a @a KoMacro::Exception that got thrown within a @a KoMacro::Context
* during execution.
*/
class KexiMacroError : public KexiMacroErrorBase
{
		Q_OBJECT

	public:

		/**
		* Constructor.
		*
		* @param parent The parent QWidget this Dialog is child of.
		* @param context The @a KoMacro::Context where the error happened.
		*/
		KexiMacroError(QWidget* parent, KoMacro::Context::Ptr context);
		
		/**
		* Destructor.
		*/
		virtual ~KexiMacroError();

	private slots:

		/**
		* Called if the "Open Macrodesigner"-Button is clicked.
		*/
		void designbtnClicked();

		/**
		* Called if the "continue"-Button is clicked.
		*/
		void continuebtnClicked();
	
	private:
		/// \internal d-pointer class.
		class Private;
		/// \internal d-pointer instance.
		Private* const d;
	
};

#endif
