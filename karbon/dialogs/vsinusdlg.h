/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VSINUSDLG_H__
#define __VSINUSDLG_H__

#include <kdialog.h>

class KDoubleNumInput;
class QSpinBox;
class KarbonPart;
class QLabel;
class VSinusDlg : public KDialog
{
	Q_OBJECT

public:
	VSinusDlg(KarbonPart*part, QWidget* parent = 0L, const char* name = 0L );

	double width() const;
	double height() const;
	uint periods() const;
	void setWidth( double value );
	void setHeight( double value );
	void setPeriods( uint value );
    void refreshUnit ();
private:
	KDoubleNumInput* m_width;
	KDoubleNumInput* m_height;
	QSpinBox* m_periods;
    KarbonPart*m_part;
    QLabel *m_heightLabel;
    QLabel *m_widthLabel;

};

#endif

