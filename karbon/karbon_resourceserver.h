/* This file is part of the KDE project

   Original work :
   kis_resourceserver.h - part of KImageShop

   Copyright (c) 1999 Matthias Elter <elter@kde.org>
   Copyright (C) 2002-2004 Rob Buis <buis@kde.org>
   Copyright (C) 2002 Beno�t Vautrin <benoit.vautrin@free.fr>
   Copyright (C) 2002 Lennart Kudling <kudling@kde.org>
   Copyright (C) 2005-2006 Laurent Montel <montel@kde.org>
   Copyright (C) 2005 Thomas Zander <zander@kde.org>
   Copyright (C) 2006 Tim Beaulen <tbscope@gmail.com>
   Copyright (C) 2006 Casper Boemann <cbr@boemann.dk>
   Copyright (C) 2006 Dirk Mueller <mueller@kde.org>
   Copyright (C) 2007 David Faure <faure@kde.org>
   Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>

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

#include <karbon_export.h>

#include <QtGui/QTableWidgetItem>

class VGradient;
class VClipartIconItem;
class VObject;
class KoPattern;
class KoAbstractGradient;

class KARBONCOMMON_EXPORT KarbonResourceServer
{

public:
	KarbonResourceServer();
	virtual ~KarbonResourceServer();

    /// Returns number of loaded patterns
    int patternCount() const;

    /// Returns list of patterns.
    QList<KoPattern*> patterns();

    /// Adds new pattern from given file
    KoPattern * addPattern( const QString& tilename );

    /// Removes given pattern
    void removePattern( KoPattern * pattern );

    /// Returns number of loaded gradients
    int gradientCount();

    /// Returns list of gradients
    QList<KoAbstractGradient*> gradients();

    /// Adds a new gradient
    KoAbstractGradient* addGradient( QGradient* gradient );

    /// Removes given gradient
    void removeGradient( KoAbstractGradient* gradient );

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
    /// Loads pattern from given file name
    const KoPattern* loadPattern( const QString& filename );

	void loadGradient( const QString& filename );
    /// Saves gradient to given file
    bool saveGradient( QGradient* gradient, const QString& filename );

	void loadClipart( const QString& filename );
	void saveClipart( VObject* object, double width, double height, const QString& filename );

private:
    QList<KoPattern*> m_patterns; ///< the loaded patterns
    QList<KoAbstractGradient*> m_gradients; ///< the loaded gradients
	Q3PtrList<VClipartIconItem>* m_cliparts;
	Q3Dict<QPixmap> m_pixmaps;
};

class VClipartIconItem : public QTableWidgetItem
{
public:
	VClipartIconItem( const VObject* clipart, double width, double height, const QString & filename );
	VClipartIconItem( const VClipartIconItem& item );
	virtual ~VClipartIconItem();

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

	VClipartIconItem* clone() const;

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

