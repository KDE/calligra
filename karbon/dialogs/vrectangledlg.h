/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VRECTANGLEDLG_H__
#define __VRECTANGLEDLG_H__

#include <kdialog.h>

class KDoubleNumInput;
class KarbonPart;
class VRectangleDlg : public KDialog
{
	Q_OBJECT

public:
	VRectangleDlg( KarbonPart*part, QWidget* parent = 0L, const char* name = 0L );

	double width() const;
	double height() const;
	void setWidth( double value );
	void setHeight( double value );

private:
	KDoubleNumInput* m_width;
	KDoubleNumInput* m_height;
    KarbonPart*m_part;
};

#endif

