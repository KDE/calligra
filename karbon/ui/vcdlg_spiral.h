/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VCDLGSPIRAL_H__
#define __VCDLGSPIRAL_H__

#include <kdialog.h>

class QComboBox;
class QLineEdit;
class QSpinBox;

class VCDlgSpiral : public KDialog
{
	Q_OBJECT
public:
	VCDlgSpiral();

	double valueRadius() const;
	uint valueSegments() const;
	double valueFade() const;
	bool valueClockwise() const;
	void setValueRadius( const double value );
	void setValueSegments( const uint value );
	void setValueFade( const double value );
	void setValueClockwise( const bool value );

private:
	QLineEdit* m_radius;
	QSpinBox* m_segments;
	QLineEdit* m_fade;
	QComboBox* m_clockwise;
};

#endif
