/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCDLGRECTANGLE_H__
#define __VCDLGRECTANGLE_H__

#include <kdialog.h>

class QLineEdit;

class VCDlgRectangle : public KDialog
{
	Q_OBJECT

public:
	VCDlgRectangle();

private:
	QLineEdit* m_width;
	QLineEdit* m_height;
	QLineEdit* m_edges;
};

#endif
