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
class KColorButton;

class VGradientDlg : public KDialog
{
	Q_OBJECT

public:
	VGradientDlg( QWidget* parent = 0L, const char* name = 0L );

	int gradientRepeat() const;
	int gradientType() const;
	bool gradientFill() const;
	QColor startColor() const;
	QColor endColor() const;
	void setGradientType( int value );
	void setGradientRepeat( int value );
	void setGradientFill( bool b );
	void setStartColor( const QColor &c );
	void setEndColor( const QColor &c );

private:
	KColorButton *m_startColor;
	KColorButton *m_endColor;
	QComboBox* m_gradientFill;
	QComboBox* m_gradientRepeat;
	QComboBox* m_gradientType;
};

#endif

