/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers

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

#ifndef __VSTROKEDLG_H__
#define __VSTROKEDLG_H__

#include <kdialogbase.h>

class QComboBox;
class QVButtonGroup;

class KarbonPart;
class KoUnitDoubleSpinBox;
class VStroke;
class VColorTab;

class VStrokeDlg : public KDialogBase
{
	Q_OBJECT

public:
	VStrokeDlg( KarbonPart* part, QWidget* parent = 0L, const char* name = 0L );

private:
	VColorTab* m_colortab;
	KarbonPart *m_part;
	KoUnitDoubleSpinBox *m_setLineWidth;
	QComboBox *m_styleCombo;
	QVButtonGroup *m_typeOption;
	QVButtonGroup *m_capOption;
	QVButtonGroup *m_joinOption;

protected:
	VStroke m_stroke;

signals:
	void strokeChanged( const VStroke & );
	
private slots:
	void slotTypeChanged( int ID );
	void slotCapChanged( int ID );
	void slotJoinChanged( int ID );
	void slotOKClicked();
	void slotUpdateDialog();

};

#endif

