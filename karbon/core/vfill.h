/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VFILL_H__
#define __VFILL_H__

class QDomElement;


class VFill
{
public:
	VFill();

	void save( QDomElement& element ) const;
	void load( const QDomElement& element );

private:

};

#endif

