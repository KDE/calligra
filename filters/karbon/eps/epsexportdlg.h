/* This file is part of the KDE project
   Copyright (C) 2002, The Karbon Developers
*/

#ifndef __VEPSEXPORTDLG_H__
#define __VEPSEXPORTDLG_H__

#include <kdialogbase.h>


class QButtonGroup;


class EpsExportDlg : public KDialogBase
{
	Q_OBJECT

public:
	EpsExportDlg( QWidget* parent = 0L, const char* name = 0L );

	uint psLevel() const;

private:
	QButtonGroup* m_psLevelButtons;
};

#endif

