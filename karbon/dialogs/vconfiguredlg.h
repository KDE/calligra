/* This file is part of the KDE project
   Copyright (C) 2002, Laurent Montel <lmontel@mandrakesoft.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef __VCONFIGUREDLG_H__
#define __VCONFIGUREDLG_H__


#include <kdialogbase.h>

class KarbonView;
class KColorButton;
class KConfig;
class KIntNumInput;
class KLineEdit;
class KSpellConfig;
class QCheckBox;
class QComboBox;


class VConfigInterfacePage : public QWidget
{
	Q_OBJECT

public:
	VConfigInterfacePage(
		KarbonView* view, QWidget* parent = 0L, char* name = 0L );

	void apply();

public slots:
	void slotDefault();

private:
	KarbonView* m_view;
	KConfig* m_config;

	KIntNumInput* m_recentFiles;
	int m_oldRecentFiles;

	QCheckBox* m_showStatusBar;
};


class VConfigMiscPage : public QObject
{
	Q_OBJECT

public:
	VConfigMiscPage(
		KarbonView* view, QVBox* box, char* name = 0L );

	void apply();

public slots:
	void slotDefault();

private:
	KarbonView* m_view;
	KConfig* m_config;

	KIntNumInput* m_undoRedo;
	int m_oldUndoRedo;
	int m_oldUnit;
	QComboBox *m_unit;
};


class VConfigDefaultPage : public QObject
{
	Q_OBJECT

public:
	VConfigDefaultPage(
		KarbonView* view, QVBox* box, char* name = 0L );

	void apply();

public slots:
	void slotDefault();

private:
	KarbonView* m_view;
	KConfig* m_config;

	KIntNumInput* m_autoSave;
	int m_oldAutoSave;
	QCheckBox *m_createBackupFile;
	bool m_oldBackupFile;
};


class VConfigureDlg : public KDialogBase
{
	Q_OBJECT

public:
	VConfigureDlg( KarbonView* parent );

public slots:
	void slotApply();
	void slotDefault();

private:
	VConfigInterfacePage* m_interfacePage;
	VConfigMiscPage* m_miscPage;
	VConfigDefaultPage* m_defaultDocPage;
};

#endif

