/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSTARDLG_H__
#define __VSTARDLG_H__

#include <kdialog.h>

class KDoubleNumInput;
class QSpinBox;
class KarbonPart;

class VStarDlg : public KDialog
{
	Q_OBJECT

public:
	VStarDlg( KarbonPart*part, QWidget* parent = 0L, const char* name = 0L );

	double innerR() const;
	double outerR() const;
	uint edges() const;
	void setInnerR( double value );
	void setOuterR( double value );
	void setEdges( uint value );
    void refreshUnit ();
private:
	KDoubleNumInput* m_innerR;
	KDoubleNumInput* m_outerR;
	QSpinBox* m_edges;
    KarbonPart*m_part;
    QLabel *m_innerRLabel;
    QLabel *m_outerRLabel;

};

#endif

