/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCDLGSINUS_H__
#define __VCDLGSINUS_H__

#include <kdialog.h>

class QLineEdit;
class QSpinBox;

class VCDlgSinus : public KDialog
{
	Q_OBJECT
public:
	VCDlgSinus();

	double valueWidth() const;
	double valueHeight() const;
	uint valuePeriods() const;
	void setValueWidth( const double value );
	void setValueHeight( const double value );
	void setValuePeriods( const uint value );

private:
	QLineEdit* m_width;
	QLineEdit* m_height;
	QSpinBox* m_periods;
};

#endif
