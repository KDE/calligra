/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSPIRALDLG_H__
#define __VSPIRALDLG_H__

#include <kdialog.h>

class QComboBox;
class QLineEdit;
class QSpinBox;

class VSpiralDlg : public KDialog
{
	Q_OBJECT

public:
	VSpiralDlg( QWidget* parent = 0L, const char* name = 0L );

	double radius() const;
	uint segments() const;
	double fade() const;
	bool clockwise() const;
	void setRadius( double value );
	void setSegments( uint value );
	void setFade( double value );
	void setClockwise( bool value );

private:
	QLineEdit* m_radius;
	QSpinBox* m_segments;
	QLineEdit* m_fade;
	QComboBox* m_clockwise;
};

#endif

