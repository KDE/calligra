/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VTOOLCONTAINER_H__
#define __VTOOLCONTAINER_H__

#include <kaction.h>

class VToolContainer : public KAction
{
	Q_OBJECT
public:
	VToolContainer( const QString& text, QObject* parent = 0L, const char* name = 0L );

	virtual int plug( QWidget* widget, int index = -1 );
};

#endif
