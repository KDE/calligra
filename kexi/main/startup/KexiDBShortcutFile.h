/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIDBSHORTCUTFILE_H
#define KEXIDBSHORTCUTFILE_H

class KexiProjectData;

/*! Loads and saved "shortcut" information
 to a Kexi project databases and connections.
*/
class KexiDBShortcutFile
{
	public:
		/*! Creates a new object for \a fileName. */
		KexiDBShortcutFile( const QString& fileName );

		~KexiDBShortcutFile();

		/*! Loads connection data into \a data.
		 Database name and caption can be set there but these are optional. 
		 \a groupKey, if provided will be set to a group key,
		 so you can later use it in saveConnectionData(). */
		bool loadConnectionData(KexiProjectData& data, QString* groupKey = 0);

		/*! Saves connection data \a data to a shortcut file. 
		 Existing data is merged with new data. 
		 \a groupKey is reused, if specified. 
		 If \a storePassword is true, password will be saved in the file. */
		bool saveConnectionData(const KexiProjectData& data, bool savePassword, QString* groupKey = 0);

		QString fileName() const;

	protected:
		class Private;
		Private *d;
};

#endif
