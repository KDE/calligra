/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VPOLYGONDLG_H__
#define __VPOLYGONDLG_H__

#include <kdialog.h>

class KDoubleNumInput;
class QSpinBox;

class VPolygonDlg : public KDialog
{
	Q_OBJECT

public:
	VPolygonDlg( QWidget* parent = 0L, const char* name = 0L );

	double radius() const;
	uint edges() const;
	void setRadius( double value );
	void setEdges( uint value );

private:
	KDoubleNumInput* m_radius;
	QSpinBox* m_edges;
};

#endif

