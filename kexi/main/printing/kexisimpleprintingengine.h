/* This file is part of the KDE project
   Copyright (C) 2005-2007 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXISIMPLEPRINTINGENGINE_H
#define KEXISIMPLEPRINTINGENGINE_H

class KexiSimplePrintingSettings;

#include <kexidb/connection.h>
#include <kexidb/tableschema.h>
#include <kexidb/cursor.h>
#include <kexidb/utils.h>
#include <kexidb/queryschema.h>
#include <widget/tableview/kexitableviewdata.h>
#include <KoPageLayoutDia.h>

#include <q3paintdevicemetrics.h>
#include <qfontmetrics.h>
#include <qfont.h>
#include <Q3PtrList>

//! @short Settings data for simple printing engine.
class KexiSimplePrintingSettings
{
	public:
		KexiSimplePrintingSettings();
		~KexiSimplePrintingSettings();

		static KexiSimplePrintingSettings load();
		void save();

		KoPageLayout pageLayout;
		KoUnit unit;
		QFont pageTitleFont;
		bool addPageNumbers : 1;
		bool addDateAndTime : 1;
		bool addTableBorders : 1;
};

/*! @short An engine painting data on pages using QPainter.
 The engine allows for random access to any page. */
class KexiSimplePrintingEngine : public QObject
{
	Q_OBJECT

	public:
		KexiSimplePrintingEngine( const KexiSimplePrintingSettings& settings, QObject* parent );
		~KexiSimplePrintingEngine();

		bool init(KexiDB::Connection& conn, KexiDB::TableOrQuerySchema& tableOrQuery,
			const QString& titleText, QString& errorMessage);

		void setTitleText(const QString& titleText);

		//! Calculates pafe count that can be later obtained using pagesCount().
		//! Page count can depend on \a painter (printer/screen) and on printing settings.
		void calculatePagesCount(QPainter& painter);

		bool done();
		void clear();
		const KexiSimplePrintingSettings* settings() const { return m_settings; }

		//! \return true when all records has been painted
		bool eof() const { return m_eof; }

		//! \return number of pages. Can be used after calculatePagesCount().
		uint pagesCount() const { return m_pagesCount; }

		//! \return number of painted pages so far. 
		//! If eof() is true, this number is equal to total page count.
		uint paintedPages() const { return m_dataOffsets.count(); }

	public slots:
		/*! Paints a page number \a pageNumber (counted from 0) on \a painter.
		 If \a paint is false, drawings are only computed but not painted, 
		 so this can be used for calculating page number before printing or previewing. */
		void paintPage(int pageNumber, QPainter& painter, bool paint = true);

	protected:
		void paintRecord(QPainter& painter, KexiTableItem *item, 
			int cellMargin, double &y, uint paintedRows, bool paint, bool printing);

		const KexiSimplePrintingSettings* m_settings;

//		QPainter* m_painter;
		QFont m_mainFont, m_headerFont;
		Q3PaintDeviceMetrics m_pdm;
		double m_dpiX, m_dpiY;
		uint m_pageWidth, m_pageHeight;
		uint m_SCALE;
		//QFontMetrics m_headerFM, m_mainFM;
		KexiDB::Cursor *m_cursor;
		KexiTableViewData *m_data;
//		KexiTableViewData::Iterator *m_dataIterator;
		Q3PtrList<uint> m_dataOffsets;
		QString m_headerText;
		QString m_dateTimeText;
		uint m_dateTimeWidth;
		QRect m_headerTextRect;
		int m_maxFieldNameWidth;
		int m_mainLineSpacing;
		int m_footerHeight;
		KexiDB::QueryColumnInfo::Vector m_fieldsExpanded;
		uint m_visibleFieldsCount;
		uint m_pagesCount;
		bool m_eof;
		bool m_paintInitialized; //!< used by paintPage()
		double m_leftMargin;
		double m_rightMargin;
		double m_topMargin;
		double m_bottomMargin;
		double m_fx, m_fy;
};

#endif
