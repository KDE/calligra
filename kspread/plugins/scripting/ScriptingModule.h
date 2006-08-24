/*
 *  Copyright (c) 2005 Cyrille Berger <cberger@cberger.net>
 *  Copyright (c) 2006 Isaac Clerencia <isaac@warp.es>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Library General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Library General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef SCRIPTINGMODULE_H
#define SCRIPTINGMODULE_H

#include <QString>
#include <QStringList>
#include <QObject>

namespace KSpread {
	//class Doc;
	class View;
	class ViewAdaptor;
	//class Sheet;
	class SheetAdaptor;
}

/**
* The ScriptingModule class enables access to the KSpread
* functionality from within the scripting backends.
*/
class ScriptingModule : public QObject
{
		Q_OBJECT
	public:
		ScriptingModule(KSpread::View* view);
		virtual ~ScriptingModule();

	public slots:

		/**
		* Returns the \a KoApplicationAdaptor object.
		*/
		QObject* application();

		/**
		* Returns the \a KoDocumentAdaptor object.
		*/
		QObject* document();

		/**
		* Returns the \a KSpread::ViewAdaptor object in which
		* the document is displayed.
		*/
		QObject* view();

		/**
		* Returns the \a KSpread::SheetAdaptor object currently
		* active in the document.
		*/
		QObject* currentSheet();

		/**
		* Returns a \a KSpread::SheetAdaptor object by name. The
		* name should be listened in the list returned by the
		* \a sheetNames() method.
		*/
		QObject* sheetByName(const QString& name);

		/**
		* Returns a list of the sheet names. The \a sheetByName
		* method could then be used to access the sheet object
		* who's name is in the list.
		*/
		QStringList sheetNames();

		/**
		* Set the document-content to the as argument passed
		* XML string.
		*/
		bool fromXML(const QString& xml);

		/**
		* Return the document-content as XML string.
		*/
		QString toXML();

	private:
		class Private;
		Private* const d;
};

#endif
