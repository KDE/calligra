/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCDLGPOLYGON_H__
#define __VCDLGPOLYGON_H__

#include <kdialog.h>

class QLineEdit;
class QSpinBox;

class VCDlgPolygon : public KDialog
{
	Q_OBJECT
public:
	VCDlgPolygon();

	double valueRadius() const;
	uint valueEdges() const;
	void setValueRadius( const double value );
	void setValueEdges( const uint value );

private:
	QLineEdit* m_radius;
	QSpinBox* m_edges;
};

#endif
