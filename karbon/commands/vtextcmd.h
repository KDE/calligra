/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VTEXTCMD_H__
#define __VTEXTCMD_H__

#include <qfont.h>

#include "vshapecmd.h"

// create a text-object.

class VObject;

class VTextCmd : public VShapeCmd
{
public:
	VTextCmd( KarbonPart* part, const QFont& font, const QString& m_text );
	virtual ~VTextCmd() {}

	virtual VObject* createPath();

private:
	QFont m_font;
	QString m_text;
};

#endif

