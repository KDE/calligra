/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VINSERTKNOTSDLG_H__
#define __VINSERTKNOTSDLG_H__

#include <kdialog.h>

class QSpinBox;

class VInsertKnotsDlg : public KDialog
{
	Q_OBJECT

public:
	VInsertKnotsDlg( QWidget* parent = 0L, const char* name = 0L );

	uint knots() const;
	void setKnots( uint value );

private:
	QSpinBox* m_knots;
};

#endif

