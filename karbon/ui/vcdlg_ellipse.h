/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#ifndef __VCDLGELLIPSE_H__
#define __VCDLGELLIPSE_H__

#include <kdialog.h>

class QLineEdit;

class VCDlgEllipse : public KDialog
{
	Q_OBJECT
public:
	VCDlgEllipse();

	double valueWidth() const;
	double valueHeight() const;
	void setValueWidth( const double value );
	void setValueHeight( const double value );

private:
	QLineEdit* m_width;
	QLineEdit* m_height;
};

#endif
