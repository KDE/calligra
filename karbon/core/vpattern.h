/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VPATTERN_H__
#define __VPATTERN_H__

#include <koPoint.h>
#include <qimage.h>

class QDomElement;


class VPattern
{
public:
	VPattern();
	VPattern( const QString &tilename );

	unsigned char *pixels();
	unsigned int tileWidth() const;
	unsigned int tileHeight() const;

	KoPoint origin() const { return m_origin; }
	void setOrigin( const KoPoint &origin ) { m_origin = origin; }

	KoPoint vector() const { return m_vector; }
	void setVector( const KoPoint &vector ) { m_vector = vector; }

	void load( const QString &tilename );

	void save( QDomElement& element ) const;
	void load( const QDomElement& element );

	void transform( const QWMatrix& m );

private:
	// coordinates:
	KoPoint m_origin;
	KoPoint m_vector;
	QImage m_image;
	QString m_tilename;
};

#endif
