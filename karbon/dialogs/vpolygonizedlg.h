/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VPOLYGONIZEDLG_H__
#define __VPOLYGONIZEDLG_H__

#include <kdialog.h>

class QLineEdit;

class VPolygonizeDlg : public KDialog
{
	Q_OBJECT

public:
	VPolygonizeDlg();

	double flatness() const;
	void setFlatness( double value );

private:
	QLineEdit* m_flatness;
};

#endif

