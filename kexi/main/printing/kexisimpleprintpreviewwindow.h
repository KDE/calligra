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
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KEXISIMPLEPRINTPREVIEWWINDOW_H
#define KEXISIMPLEPRINTPREVIEWWINDOW_H

#include <qpainter.h>
#include <qscrollview.h>
#include <ktoolbar.h>
#include <3rdparty/kolibs/koPageLayoutDia.h>

class KexiSimplePrintPreviewScrollView;
class KexiSimplePrintPreviewView;
class KexiSimplePrintingSettings;
class KexiSimplePrintingEngine;

//! @short A window for displaying print preview for simple printing.
class KexiSimplePrintPreviewWindow : public QWidget
{
	Q_OBJECT

	public:
		KexiSimplePrintPreviewWindow(KexiSimplePrintingEngine &engine,
			const QString& previewName, QWidget *parent, WFlags f = 0);
		~KexiSimplePrintPreviewWindow();

		int currentPage() const { return m_pageNumber; }

		const KexiSimplePrintingSettings& settings() const { return m_settings; }

	public slots:
		void setPagesCount(int pagesCount);
		void goToPage(int pageNumber);
		void setFullWidth();

	signals:
		void printRequested();
//		void paintingPageRequested(int pageNumber, QPainter& painter);

	protected slots:
		void slotPrintClicked();
		void slotZoomInClicked();
		void slotZoomOutClicked();
		void slotFirstClicked();
		void slotPreviousClicked();
		void slotNextClicked();
		void slotLastClicked();

	protected:
		KexiSimplePrintingEngine &m_engine;
		const KexiSimplePrintingSettings& m_settings;
		KToolBar *m_toolbar, *m_navToolbar;
		int m_pageNumber, m_pagesCount;
		int m_idFirst, m_idLast, m_idPrevious, m_idNext;
		KexiSimplePrintPreviewScrollView *m_scrollView;
		KexiSimplePrintPreviewView *m_view;

		friend class KexiSimplePrintPreviewView;
};

#endif
