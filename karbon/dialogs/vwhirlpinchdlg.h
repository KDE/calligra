/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VWHIRLPINCHDLG_H__
#define __VWHIRLPINCHDLG_H__

#include <kdialog.h>

class KDoubleNumInput;

class VWhirlPinchDlg : public KDialog
{
	Q_OBJECT

public:
	VWhirlPinchDlg( QWidget* parent = 0L, const char* name = 0L );

	double angle() const;
	double pinch() const;
	double radius() const;
	void setAngle( double value );
	void setPinch( double value );
	void setRadius( double value );

private:
	KDoubleNumInput* m_angle;
	KDoubleNumInput* m_pinch;
	KDoubleNumInput* m_radius;
};

#endif

