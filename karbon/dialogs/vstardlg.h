/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSTARDLG_H__
#define __VSTARDLG_H__

#include <kdialog.h>

class QLineEdit;
class QSpinBox;

class VStarDlg : public KDialog
{
	Q_OBJECT

public:
	VStarDlg( QWidget* parent = 0L, const char* name = 0L );

	double innerR() const;
	double outerR() const;
	uint edges() const;
	void setInnerR( double value );
	void setOuterR( double value );
	void setEdges( uint value );

private:
	QLineEdit* m_innerR;
	QLineEdit* m_outerR;
	QSpinBox* m_edges;
};

#endif

