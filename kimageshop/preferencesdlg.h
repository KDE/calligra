/*
 *  preferencesdlg.h - part of KImageShop
 *
 *  Copyright (c) 1999 Michael Koch <koch@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __preferencesdlg_h__
#define __preferencesdlg_h__

#include <qlineedit.h>

#include <kdialog.h>

/**
 *  Preferences dialog of KImageShop
 */
class PreferencesDialog : public KDialog
{
  Q_OBJECT

public:

  static void editPreferences();

protected:

	PreferencesDialog( QWidget *_parent = 0, const char *_name = 0, WFlags f = 0 );
	~PreferencesDialog();

private:

	QLineEdit *m_pLineEdit;
};

#endif



