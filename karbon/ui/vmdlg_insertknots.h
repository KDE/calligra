/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VMDLGINSERTKNOTS_H__
#define __VMDLGINSERTKNOTS_H__

#include <kdialog.h>

class QSpinBox;

class VMDlgInsertKnots : public KDialog
{
	Q_OBJECT
public:
	VMDlgInsertKnots();

	uint valueKnots() const;
	void setValueKnots( uint value );

private:
	QSpinBox* m_knots;
};

#endif
