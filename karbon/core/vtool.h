/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VTOOL_H__
#define __VTOOL_H__

// The abstract base class for all VObject-manipulating classes.

class KCommand;

class VObject;

class VTool
{
public:
	virtual ~VTool() = 0;

	virtual KCommand* manipulate( VObject* object );
};

#endif
