/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VIMAGE_H__
#define __VIMAGE_H__

#include "vobject.h"

class QDomElement;
class QImage;

// all vobjects exist inside a layer.

class VImage : public VObject
{
public:
	VImage( VObject *parent, const QString &fname = "" );
	VImage( const VImage & );
	~VImage();

	virtual void draw( VPainter *painter, const KoRect *rect ) const;

    virtual void transform( const QWMatrix& m );
	virtual VObject* clone() const;

	virtual void save( QDomElement& element ) const;
	virtual void load( const QDomElement& element );

private:
	QImage *m_image;
	QString m_fname;
	QWMatrix m_matrix;
};

#endif
