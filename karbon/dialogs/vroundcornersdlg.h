/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VROUNDCORNERSDLG_H__
#define __VROUNDCORNERSDLG_H__

#include <kdialog.h>

class QLineEdit;

class VRoundCornersDlg : public KDialog
{
	Q_OBJECT

public:
	VRoundCornersDlg( QWidget* parent = 0L, const char* name = 0L );

	double radius() const;
	void setRadius( double value );

private:
	QLineEdit* m_radius;
};

#endif

