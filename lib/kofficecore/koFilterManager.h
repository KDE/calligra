/* $Id:$
   This file is part of KOffice
    Copyright (C) 1998 Kalle Dalheimer <kalle@kde.org>

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

#ifndef __koffice_filter_manager_h__
#define __koffice_filter_manager_h__

#include <qobject.h>
#include <qstrlist.h>

class KoFilter;

// You are not supposed to use this.
struct KoFilterData
{
	QString name;
	QString extensions;
	int direction;
	KoFilter* filter;
};
typedef QList<KoFilterData> KoFilterDataList;

/**
 * This class manages all filters for a KOffice application. It handles filter 
 * registration and invoking.
 * This class follows the singleton pattern. You don't create objects of this
 * class directly, but rather use the static method instance().
 * @author Kalle Dalheimer <kalle@kde.org>
 * @version $Id:$
 */
class KoFilterManager : public QObject
{
public:
	enum Direction { Import, Export };

	/** 
	 * Returns a pointer to the only instance of the KoFilterManager.
	 * @return The pointer to the KoFilterManager instance
	 */
	static KoFilterManager* instance();

	/** 
	 * Frees a reference to the KoFilterManager. When there are no more
	 * references, all associated data will be deleted.
	 */
	void releaseRef();

	/**
	 * Registers a filter with the filter manager. For now, call this
	 * for each known filter in your application in your main() (or
	 * equivalent).
	 *
	 * @param name The name of the file format that this filter
	 * handles. Example: "FrameMaker (MIF)".
	 * @param extensions The filename extensions regexp that this filter
	 * expects. Example: "*.mif;*.MIF".
	 * @param direction Whether this is an input or output filter. Is
	 * either KoFilterManager::Import or KoFilterManager::Export.
	 * @param filter A pointer to the class that implements this
	 * filter. 
	 */
	void registerFilter( const char* name, const char* extensions, 
						 Direction direction, KoFilter* filter );

	/**
	 * Returns a string list that is suitable for passing to
	 * {Q,K}FileDialog::setFilters().
	 *
	 * @param direction Whether the dialog is for opening or for
	 * saving. Is either KoFilterManager::Import or
	 * KoFilterManager::Export.
	 * @param allfiles Whether a wildcard should be added to the list.
	 */
	QStrList fileSelectorList( Direction direction, bool allfiles ) const;
 
	/**
	 * Invokes the appropriate filter parser and returns an
	 * application-dependent structure of the parsed data.
	 * PENDING(kalle) Should this be a document object?
	 *
	 * @param direction Whether the filter should be importing or exporting.
	 * @param filename The file that contains the data to filter.
	 * @param filter This parameter will be filled with a pointer to the
	 * filter object used.
	 * @return A pointer to a filter-dependent data structure
	 */
	void* invokeFilterParser( Direction direction, const char* filename,
							  KoFilter*& filter );

protected:
	KoFilterManager();

private:
	static KoFilterManager* _instance;
	KoFilterDataList _filterlist;
};
	 

#endif
