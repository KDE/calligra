/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VFILLDLG_H__
#define __VFILLDLG_H__

#include <kdialogbase.h>

class KarbonPart;
class VColorTab;

class VFillDlg : public KDialogBase
{
	Q_OBJECT

public:
	VFillDlg( KarbonPart* part, QWidget* parent = 0L, const char* name = 0L );

private:
	VColorTab* m_colortab;
	KarbonPart *m_part;

signals:
	void fillChanged( const VFill & );

private slots:
	void slotApplyButtonPressed();
};
#endif

