/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KEXISIMPLEPRINTINGWINDOW_H
#define KEXISIMPLEPRINTINGWINDOW_H

#include "kexisimpleprintingengine.h"
#include <kexiviewbase.h>

class KexiSimplePrintingWindowBase;
class KoPageLayoutSize;
class KexiSimplePrintPreviewWindow;

//! @short A window for displaying settings for simple printing.
class KexiSimplePrintingWindow : public KexiViewBase
{
	Q_OBJECT

	public:
		KexiSimplePrintingWindow( KexiMainWindow *mainWin, QWidget *parent, QMap<QString,QString>* args );
		~KexiSimplePrintingWindow();

	public slots:
		void print();
		void printPreview();

	protected slots:
		void slotOpenData();
		void slotSaveSetup();
		void slotChangeTitleFont();
		void slotChangePageSizeAndMargins();
		void slotAddPageNumbersCheckboxToggled(bool set);
		void slotAddDateTimeCheckboxToggled(bool set);
		void slotAddTableBordersCheckboxToggled(bool set);

	protected:
		void loadSetup();
		void updatePageLayoutAndUnitInfo();
		void setDirty(bool set);

		bool m_preview;
		KexiSimplePrintingSettings m_settings;
		KexiSimplePrintingEngine *m_engine;
		KoUnit::Unit m_unit;
		KexiSimplePrintingWindowBase *m_contents;
		KoPageLayoutSize *m_pageLayoutWidget;
		KexiPart::Item *m_item;
		QString m_origCaptionLabelText;
		KexiSimplePrintPreviewWindow *m_previewWindow;
		bool m_printPreviewNeedsReloading;

};

#endif
