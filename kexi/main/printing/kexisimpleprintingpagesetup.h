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

#ifndef KEXISIMPLEPRINTINGPAGESETUP_H
#define KEXISIMPLEPRINTINGPAGESETUP_H

#include "kexisimpleprintingengine.h"
#include <KexiView.h>

class KexiSimplePrintingPageSetupBase;
class KoPageLayoutSize;
class KexiSimplePrintPreviewWindow;

/*! @short A command for simple printing and print preview. 
 This class is instantiated in KexiMainWindowImpl so there's:
 - a single print preview window per part item regardless of a way how user invoked
    the 'print preview' command (using 'File->Print Preview' command or 'Print Preview' button 
    of the 'Page Setup' dialog)
 - a single printing engine per part item regardless of a way how user started
   (using 'File->Print' command or 'Print' button of the 'Page Setup' dialog)
*/
class KexiSimplePrintingCommand : public QObject
{
	Q_OBJECT

	public:
		KexiSimplePrintingCommand(KexiMainWindow* mainWin, int objectId, 
			QObject* parent = 0);
		~KexiSimplePrintingCommand();

	public slots:
		bool print(const KexiSimplePrintingSettings& settings, 
			const QString& aTitleText = QString::null);
		bool print(const QString& aTitleText = QString::null);
		bool showPrintPreview(const KexiSimplePrintingSettings& settings, 
			const QString& aTitleText = QString::null, bool reload = false);
//		void setPrintPreviewNeedsReloading();

	signals:
		//! connected to Kexi Main Window
		void showPageSetupRequested(KexiPart::Item* item);

	protected slots:
		void slotShowPageSetupRequested();

	protected:
		KexiSimplePrintingEngine* m_previewEngine;
		KexiMainWindow* m_mainWin;
		int m_objectId;
		KexiSimplePrintingSettings m_settings;
		KexiSimplePrintPreviewWindow *m_previewWindow;
		bool m_printPreviewNeedsReloading : 1;
};

//! @short A window for displaying settings for simple printing.
class KexiSimplePrintingPageSetup : public KexiView
{
	Q_OBJECT

	public:
		KexiSimplePrintingPageSetup( KexiMainWindow *mainWin, QWidget *parent, QMap<QString,QString>* args );
		~KexiSimplePrintingPageSetup();

	public slots:
		void print();
		void printPreview();

	signals:
		void printItemRequested(KexiPart::Item* item, 
			const KexiSimplePrintingSettings& settings, const QString& titleText);
		void printPreviewForItemRequested(KexiPart::Item* item, 
			const KexiSimplePrintingSettings& settings, const QString& titleText, bool reload);

	protected slots:
		void slotOpenData();
		void slotSaveSetup();
		void slotChangeTitleFont();
		void slotChangePageSizeAndMargins();
		void slotAddPageNumbersCheckboxToggled(bool set);
		void slotAddDateTimeCheckboxToggled(bool set);
		void slotAddTableBordersCheckboxToggled(bool set);
		void slotTitleTextChanged(const QString&);

	protected:
		void setupPrintingCommand();
		void updatePageLayoutAndUnitInfo();
		void setDirty(bool set);

		KexiSimplePrintingSettings m_settings;
//		KexiSimplePrintingEngine *m_engine;
		KoUnit m_unit;
		KexiSimplePrintingPageSetupBase *m_contents;
		KoPageLayoutSize *m_pageLayoutWidget;
		KexiPart::Item *m_item;
//		KexiSimplePrintingCommand *m_command;
		QString m_origCaptionLabelText;
		bool m_printPreviewNeedsReloading : 1;

};

#endif
