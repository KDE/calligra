/* This file is part of the KDE project
   Copyright (C) 2001 Robert JACOLIN <rjacolin@ifrance.com>

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

/*
   This file is based on the file :
    koffice/filters/kword/html/htmlexportdia.h
	Copyright (C) 2001 Nicolas Goutte <nicog@snafu.de>

   which was based on the old file:
    /home/kde/koffice/filters/kspread/csv/csvfilterdia.h

   The old file was copyrighted by
    Copyright (C) 1999 David Faure <faure@kde.org>

   The old file was licensed under the terms of the GNU Library General Public
   License version 2.
*/

#ifndef __LATEXEXPORTDIA_H__
#define __LATEXEXPORTDIA_H__

#ifndef USE_QFD

#include <qwidget.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qradiobutton.h>
#include <qvbuttongroup.h>

#include <klocale.h>
#include <koFilterDialog.h>

class LATEXExportDia : public KoFilterDialog
{

	Q_OBJECT

	public:
		LATEXExportDia(QWidget *parent=0L, const char *name=0L);

		virtual ~LATEXExportDia() {}

		virtual QString state();

	private:
		QVButtonGroup* styleBox,       *langBox,        *docBox;
		QRadioButton*  latexStyleRBtn, *kwordStyleRBtn;	/* Document style */
		QRadioButton*  unicodeRBtn,    *latin1RBtn;	/* Language       */
		QRadioButton*  newDocRBtn,     *embededRBtn;	/* Language       */
};
#endif
#endif /* __LATEXEXPORTDIA_H__ */
