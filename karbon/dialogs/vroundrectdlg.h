/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VROUNDRECTDLG_H__
#define __VROUNDRECTDLG_H__

#include <kdialog.h>

class QLineEdit;

class VRoundRectDlg : public KDialog
{
	Q_OBJECT

public:
	VRoundRectDlg( QWidget* parent = 0L, const char* name = 0L );

	double width() const;
	double height() const;
	double round() const;
	void setWidth( double value );
	void setHeight( double value );
	void setRound( double value );

private:
	QLineEdit* m_width;
	QLineEdit* m_height;
	QLineEdit* m_round;
};

#endif

