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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __VCONFIGUREDLG_H__
#define __VCONFIGUREDLG_H__


#include <kpagedialog.h>
#include <kvbox.h>

class KarbonView;
class KConfig;
class KIntNumInput;
class KColorButton;
class KoUnitDoubleSpinBox;
class QCheckBox;
class QComboBox;

class VConfigInterfacePage : public KVBox
{
	Q_OBJECT

public:
	VConfigInterfacePage( KarbonView* view, char* name = 0L );

	void apply();

public slots:
	void slotDefault();

private:
	KarbonView* m_view;
	KConfig* m_config;

	KIntNumInput* m_recentFiles;
	int m_oldRecentFiles;

	QCheckBox* m_showStatusBar;

	KIntNumInput* m_copyOffset;
	int m_oldCopyOffset;

	KIntNumInput* m_dockerFontSize;
	int m_oldDockerFontSize;
};


class VConfigMiscPage : public KVBox
{
	Q_OBJECT

public:
	VConfigMiscPage( KarbonView* view, char* name = 0L );

	void apply();

signals:
	void unitChanged( int );

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

class VConfigDefaultPage : public KVBox
{
	Q_OBJECT

public:
	VConfigDefaultPage( KarbonView* view, char* name = 0L );

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
	QCheckBox *m_saveAsPath;
	bool m_oldSaveAsPath;
};

class VConfigGridPage : public QWidget
{
	Q_OBJECT

public:
	VConfigGridPage(
		KarbonView* view, char* name = 0L );

	void apply();

public slots:
	void slotDefault();
	void slotUnitChanged( int );

protected slots:
	void setMaxHorizSnap( double v );
	void setMaxVertSnap( double v );

private:
	KarbonView* m_view;
	KoUnitDoubleSpinBox* m_spaceHorizUSpin;
	KoUnitDoubleSpinBox* m_spaceVertUSpin;
	KoUnitDoubleSpinBox* m_snapHorizUSpin;
	KoUnitDoubleSpinBox* m_snapVertUSpin;
	QCheckBox* m_gridChBox;
	QCheckBox* m_snapChBox;
	KColorButton* m_gridColorBtn;
};

class VConfigureDlg : public KPageDialog
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
	VConfigGridPage* m_gridPage;
	VConfigDefaultPage* m_defaultDocPage;
};

#endif

