/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VCDLGSTAR_H__
#define __VCDLGSTAR_H__

#include <kdialog.h>

class QLineEdit;
class QSpinBox;

class VCDlgStar : public KDialog
{
	Q_OBJECT
public:
	VCDlgStar();

	double valueInnerR() const;
	double valueOuterR() const;
	uint valueEdges() const;
	void setValueInnerR( const double value );
	void setValueOuterR( const double value );
	void setValueEdges( const uint value );

private:
	QLineEdit* m_innerR;
	QLineEdit* m_outerR;
	QSpinBox* m_edges;
};

#endif
