/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VMDLGPOLYGONIZE_H__
#define __VMDLGPOLYGONIZE_H__

#include <kdialog.h>

class QLineEdit;

class VMDlgPolygonize : public KDialog
{
	Q_OBJECT
public:
	VMDlgPolygonize();

	double valueFlatness() const;
	void setValueFlatness( double value );

private:
	QLineEdit* m_flatness;
};

#endif
