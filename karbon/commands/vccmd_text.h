/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VCCMDTEXT_H__
#define __VCCMDTEXT_H__

#include "vccommand.h"
#include <qfont.h>

// create a text-object.

class VObject;

class VCCmdText : public VCCommand
{
public:
	VCCmdText( KarbonPart* part, const QFont &font, const QString &m_text );
	virtual ~VCCmdText() {}

	// for complex shapes. needed to draw while creation (creation tool):
	VObject* createPath();

private:
	QFont m_font;
	QString m_text;
};

#endif
