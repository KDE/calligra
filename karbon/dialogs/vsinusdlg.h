/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSINUSDLG_H__
#define __VSINUSDLG_H__

#include <kdialog.h>

class QLineEdit;
class QSpinBox;

class VSinusDlg : public KDialog
{
	Q_OBJECT

public:
	VSinusDlg( QWidget* parent = 0L, const char* name = 0L );

	double width() const;
	double height() const;
	uint periods() const;
	void setWidth( double value );
	void setHeight( double value );
	void setPeriods( uint value );

private:
	QLineEdit* m_width;
	QLineEdit* m_height;
	QSpinBox* m_periods;
};

#endif

