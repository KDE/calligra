/* This file is part of the KDE project
   Copyright (C) 2001, 2002, 2003 The Karbon Developers

   Original work :
   kis_resourceserver.h - part of KImageShop

   Copyright (c) 1999 Matthias Elter <elter@kde.org>

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

#ifndef __KARBONRESOURCESERVER_H__
#define __KARBONRESOURCESERVER_H__


#include <q3ptrlist.h>
#include <q3dict.h>
#include <QString>
//Added by qt3to4:
#include <QPixmap>

//#include "vgradient.h"
#include "vpattern.h"
#include <koffice_export.h>

class VGradient;
class VGradientListItem;
class VClipartIconItem;
class VObject;


class KARBONCOMMON_EXPORT KarbonResourceServer
{

public:
	KarbonResourceServer();
	virtual ~KarbonResourceServer();

	int patternCount()
	{
		return m_patterns.count();
	}

	Q3PtrList<KoIconItem> patterns()
	{
		return m_patterns;
	}

	VPattern* addPattern( const QString& tilename );
	void removePattern( VPattern* pattern );

	int gradientCount()
	{
		return m_gradients->count();
	}

	Q3PtrList<VGradientListItem>* gradients()
	{
		return m_gradients;
	}

	VGradientListItem* addGradient( VGradient* gradient );
	void removeGradient( VGradientListItem* gradient );

	int clipartCount()
	{
		return m_cliparts->count();
	}

	Q3PtrList<VClipartIconItem>* cliparts()
	{
		return m_cliparts;
	}

	VClipartIconItem* addClipart( VObject* clipart, double width, double height );
	void removeClipart( VClipartIconItem* clipartIcon );

	QPixmap *cachePixmap( const QString &key, int group_or_size );

protected:
	const VPattern* loadPattern( const QString& filename );

	void loadGradient( const QString& filename );
	void saveGradient( VGradient* gradient, const QString& filename );

	void loadClipart( const QString& filename );
	void saveClipart( VObject* object, double width, double height, const QString& filename );

private:
	Q3PtrList<KoIconItem> m_patterns;
	Q3PtrList<VGradientListItem>* m_gradients;
	Q3PtrList<VClipartIconItem>* m_cliparts;
	Q3Dict<QPixmap> m_pixmaps;
};

class VClipartIconItem : public KoIconItem
{
public:
	VClipartIconItem( const VObject* clipart, double width, double height, QString filename );
	VClipartIconItem( const VClipartIconItem& item );
	~VClipartIconItem();

	virtual QPixmap& thumbPixmap() const
	{
		return ( QPixmap& ) m_thumbPixmap;
	}

	virtual QPixmap& pixmap() const
	{
		return ( QPixmap& ) m_pixmap;
	}

	const VObject* clipart() const
	{
		return m_clipart;
	}

	QString filename() const
	{
		return m_filename;
	}

	bool canDelete() const
	{
		return m_delete;
	}

	double originalWidth() const
	{
		return m_width;
	}

	double originalHeight() const
	{
		return m_height;
	}

	VClipartIconItem* clone();

private:
	QPixmap m_pixmap;
	QPixmap m_thumbPixmap;
	VObject* m_clipart;
	QString m_filename;
	bool m_delete;
	double m_width;
	double m_height;
}

; // VClipartIconItem

#endif

