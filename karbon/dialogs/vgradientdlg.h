/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VGRADIENTDLG_H__
#define __VGRADIENTDLG_H__

#include <kdialog.h>

class QComboBox;
class QLineEdit;
class QSpinBox;

class VGradientDlg : public KDialog
{
	Q_OBJECT

public:
	VGradientDlg( QWidget* parent = 0L, const char* name = 0L );

	int gradientRepeat() const;
	int gradientType() const;
	bool gradientFill() const;
	void setGradientType( int value );
	void setGradientRepeat( int value );
	void setGradientFill( bool b );

private:
	QComboBox* m_gradientFill;
	QComboBox* m_gradientRepeat;
	QComboBox* m_gradientType;
};

#endif

