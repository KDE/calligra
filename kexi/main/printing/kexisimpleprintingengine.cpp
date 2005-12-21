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

#include "kexisimpleprintingengine.h"

#include <core/keximainwindow.h>
#include <kexiutils/utils.h>

#include <kapplication.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kfontdialog.h>
#include <kurllabel.h>
#include <kdebug.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qcheckbox.h>
#include <qwhatsthis.h>
#include <qpaintdevicemetrics.h>

#include <kprinter.h>

#include <kexiutils/tristate.h>
#include <kexidb/connection.h>
#include <kexidb/tableschema.h>
#include <kexidb/cursor.h>
#include <kexidb/utils.h>
#include <kexidb/queryschema.h>

KexiSimplePrintingSettings::KexiSimplePrintingSettings()
{
	pageLayout = KoPageLayout::standardLayout();
	addPageNumbers = true;
	addDateAndTime = true;
	addTableBorders = false;
	pageTitleFont = kapp->font();
	pageTitleFont.setPointSize( pageTitleFont.pointSize()*3/2 );
	pageTitleFont.setBold(true);
}

//------------------------

KexiSimplePrintingEngine::KexiSimplePrintingEngine(
	const KexiSimplePrintingSettings& settings, QObject* parent)
 : QObject(parent, "KexiSimplePrintingEngine")
 , m_settings(&settings)
 , m_pdm(0)
{
	m_cursor = 0;
	m_data = 0;
	m_dataOffsets.setAutoDelete(true);
	clear();
}

KexiSimplePrintingEngine::~KexiSimplePrintingEngine()
{
	done();
}

bool KexiSimplePrintingEngine::init(KexiDB::Connection& conn, 
	KexiDB::TableOrQuerySchema& tableOrQuery, QString& errorMessage)
{
	errorMessage = QString::null;
	done();
	m_headerText = tableOrQuery.captionOrName();

	//open data source
	KexiDB::QuerySchema *query = 0;
	if (tableOrQuery.table())
		query = tableOrQuery.table()->query(); //all rows
	else
		query = tableOrQuery.query();
	if (!query) {
		errorMessage = i18n("Could not load data from table or query.");
		return false;
	}

	m_cursor = conn.executeQuery(*query);
	if (!m_cursor) {
		conn.debugError();
		return false;
	}
	if (m_cursor->error()) {
		conn.debugError();
		conn.deleteCursor(m_cursor);
		m_cursor = 0;
		return false;
	}
//! @todo primitive: data should be loaded on demand
	m_data = new KexiTableViewData(m_cursor);
	m_data->preloadAllRows();
	m_fieldsExpanded = KexiDB::QueryColumnInfo::Vector( query->fieldsExpanded() );
	return true;
}

bool KexiSimplePrintingEngine::done()
{
	bool result = true;
	if (m_cursor && (m_cursor->error() || !m_cursor->connection()->deleteCursor(m_cursor))) {
		m_cursor->debugError();
		result = false;
	}
	m_cursor = 0;
	delete m_data;
	m_data = 0;
	m_paintInitialized = false;
	return result;
}

void KexiSimplePrintingEngine::clear()
{
	m_eof = false;
	m_dataOffsets.clear();
	m_dataOffsets.append(new uint(0));
	m_paintInitialized = false;
}

bool KexiSimplePrintingEngine::paintPage(int pageNumber, QPainter& painter)
{
	uint offset = 0;
	if (pageNumber < (int)m_dataOffsets.count()) {
		offset = *m_dataOffsets.at(pageNumber);
	}

	uint y = 0;

	m_painter = &painter;
	const bool printer = m_painter->device()->devType() == QInternal::Printer;

	int w = 0, h = 0;
	m_pdm = QPaintDeviceMetrics( m_painter->device() );
	
	if (dynamic_cast<QWidget*>(m_painter->device())) {
		w = dynamic_cast<QWidget*>(m_painter->device())->width();
		h = dynamic_cast<QWidget*>(m_painter->device())->height();
	}
	else if (dynamic_cast<QPixmap*>(m_painter->device())) {
		w = dynamic_cast<QPixmap*>(m_painter->device())->width();
		h = dynamic_cast<QPixmap*>(m_painter->device())->height();
	}
	else {//KPrinter...
		w = m_pdm.widthMM();
		h = m_pdm.heightMM();
	}

	if (!m_paintInitialized) {
		m_paintInitialized = true;

		double widthMM = KoPageFormat::width( 
			m_settings->pageLayout.format, m_settings->pageLayout.orientation);
		double heightMM = KoPageFormat::height( 
			m_settings->pageLayout.format, m_settings->pageLayout.orientation);

		m_dpiY = m_pdm.logicalDpiY();
		m_dpiX = m_pdm.logicalDpiX();
#ifdef Q_WS_WIN //fix for 120dpi
		if (!printer) {
			m_dpiY = 96;
			m_dpiX = 96;
		}
#endif
		int pdWidthMM = m_pdm.widthMM();
		int pdHeightMM = m_pdm.heightMM();

		double screenF;
		//if (printer)
			screenF = 1.0;
		//else
//todo			//screenF = (double)96.0/120.0;

		leftMargin = POINT_TO_INCH(m_settings->pageLayout.ptLeft)*m_dpiX* screenF;
		rightMargin = POINT_TO_INCH(m_settings->pageLayout.ptRight)*m_dpiX* screenF;
		topMargin = POINT_TO_INCH(m_settings->pageLayout.ptTop)*m_dpiY* screenF;
		bottomMargin = POINT_TO_INCH(m_settings->pageLayout.ptBottom)*m_dpiY* screenF;
		
		m_fx = widthMM / (pdWidthMM * screenF);
		m_fy = heightMM / (pdHeightMM * screenF);

	//screen only
	//	painter.fillRect(QRect(0,0,w,h), QBrush(white));
		m_pageWidth = int( m_fx*(double)m_pdm.width() - leftMargin - rightMargin);
		m_pageHeight = int( m_fy*(double)m_pdm.height() - topMargin - bottomMargin);
		m_headerFont = m_settings->pageTitleFont;
		if(!printer) {
			int pixelSize = (int)POINT_TO_INCH(m_headerFont.pointSizeFloat())*m_dpiX;
			m_headerFont.setPixelSize(pixelSize);
		}

//! @todo add setting
		m_mainFont = kapp->font(); 
		if(!printer) {
			int pixelSize = (int)POINT_TO_INCH(m_mainFont.pointSizeFloat())*m_dpiX;
			m_mainFont.setPixelSize(pixelSize);
		}
		m_painter->setFont(m_mainFont);

		m_dateTimeText = KGlobal::locale()->formatDateTime(QDateTime::currentDateTime(),
			true, false);
		m_dateTimeWidth = m_painter->fontMetrics().width(m_dateTimeText+"   ");
		m_mainLineSpacing = m_painter->fontMetrics().lineSpacing();
		m_footerHeight = m_mainLineSpacing * 2; //2 lines
		m_painter->setFont(m_headerFont);
		m_headerTextRect = m_painter->fontMetrics().boundingRect(
			(int)leftMargin, (int)topMargin,
			m_pageWidth - m_dateTimeWidth,
			m_pageHeight, Qt::AlignAuto|Qt::WordBreak, m_headerText);
		m_headerTextRect.setRight(m_headerTextRect.right()+10);

		//--compute max width of field names
		m_maxFieldNameWidth = 0;
		
		m_painter->setFont(m_mainFont);
		m_painter->setFont(m_mainFont);
		for (uint i=0; i < m_fieldsExpanded.count(); i++) {
			const int newW =
				m_painter->fontMetrics().width(m_fieldsExpanded[i]->captionOrAliasOrName()+":");
//			kdDebug() << "row"<<i<<": "<<m_fieldsExpanded[i]->captionOrAliasOrName()<<" " 
//				<< newW <<endl;
			if (m_maxFieldNameWidth < newW)
				m_maxFieldNameWidth = newW;
		}
		m_maxFieldNameWidth += m_painter->fontMetrics().width("ww"); //more space
	}

		//screen only
		if(!printer) {
			m_painter->setWindow(0, 0, int((double)w*m_fx), int((double)h*m_fy));
		}

	//paint header
	m_painter->setFont(m_headerFont);
	m_painter->drawText(m_headerTextRect, Qt::AlignAuto|Qt::WordBreak, m_headerText);
	m_painter->setFont(m_mainFont);
	m_painter->drawText((int)leftMargin + m_pageWidth - m_dateTimeWidth, 
		(int)topMargin, m_dateTimeWidth, 
		m_headerTextRect.height(), Qt::AlignRight, m_dateTimeText);
	//footer
	m_painter->drawText((int)leftMargin, 
		(int)topMargin + m_pageHeight - m_mainLineSpacing, 
		m_pageWidth, m_mainLineSpacing,
		Qt::AlignRight | Qt::AlignBottom, i18n("Page %1").arg(pageNumber+1));
	m_painter->drawLine((int)leftMargin, 
		(int)topMargin + m_pageHeight - m_mainLineSpacing*3/2, 
		(int)leftMargin + m_pageWidth,
		(int)topMargin + m_pageHeight - m_mainLineSpacing*3/2);
	y = (int)topMargin + m_headerTextRect.height() + m_mainLineSpacing/2;
	m_painter->drawLine((int)leftMargin, y, (int)leftMargin + m_pageWidth-1, y);
	y += m_mainLineSpacing;

	//--print records
	KexiDB::RowData row;
	KexiTableItem *item;
	const uint count = m_fieldsExpanded.count();
	const uint rows = m_data->count();
	const int cellMargin = m_settings->addTableBorders ? 
		m_painter->fontMetrics().width("i") : 0;
	for (;offset < rows; ++offset) {
		item = m_data->at(offset);
		if (int(count * m_mainLineSpacing) > int(topMargin + m_pageHeight-(int)y-m_footerHeight)) 
		{
			//do not break records between pages
			break;
		}
		kdDebug() << " -------- " << y << " / " << m_pageHeight << endl;
		for (uint i=0; i<count; i++) {
//			kdDebug() << "row"<<i<<": "<<row.at(i).toString()<<endl;
			m_painter->drawText(
				(int)leftMargin+cellMargin, y, m_maxFieldNameWidth-cellMargin*2, m_mainLineSpacing, 
				Qt::AlignTop, m_fieldsExpanded[i]->captionOrAliasOrName()
				+ (m_settings->addTableBorders ? "" : ":"));
			QString text;
			KexiDB::Field::Type ftype = m_fieldsExpanded[i]->field->type();
//! todo inherit format
			if (ftype==KexiDB::Field::DateTime)
				text = KGlobal::locale()->formatDateTime(item->at(i).toDateTime());
//! todo inherit format
			else if (ftype==KexiDB::Field::Date)
				text = KGlobal::locale()->formatDate(item->at(i).toDate());
//! todo inherit format
			else if (ftype==KexiDB::Field::Time)
				text = KGlobal::locale()->formatTime(item->at(i).toTime());
//! todo currency, decimal...
			else if (m_fieldsExpanded[i]->field->isFPNumericType())
				text = KGlobal::locale()->formatNumber(item->at(i).toDouble());
			else
				text = item->at(i).toString();
			QRect rect( m_painter->fontMetrics().boundingRect(
				(int)leftMargin + m_maxFieldNameWidth + cellMargin, y,
				m_pageWidth - m_maxFieldNameWidth - cellMargin*2, m_pageHeight - y, 
				Qt::AlignAuto|Qt::WordBreak, text) );
			m_painter->drawText(
				rect.x(), rect.y(), rect.width(), rect.height(),
				Qt::AlignTop|Qt::WordBreak, text);
			if (m_settings->addTableBorders) {
				m_painter->setPen(Qt::darkGray);
				m_painter->drawLine(
					(int)leftMargin, rect.top(), (int)leftMargin+m_pageWidth-1, rect.top());
				m_painter->drawLine(
					(int)leftMargin, rect.top(), (int)leftMargin, rect.bottom());
				m_painter->drawLine(
					(int)leftMargin+m_pageWidth-1, rect.top(), 
					(int)leftMargin+m_pageWidth-1, rect.bottom());
				m_painter->drawLine(
					(int)leftMargin+m_maxFieldNameWidth, rect.top(), 
					(int)leftMargin+m_maxFieldNameWidth, rect.bottom());
				m_painter->setPen(Qt::black);
			}
			y += rect.height();
		}
		if (m_settings->addTableBorders) {
			m_painter->setPen(Qt::darkGray);
			m_painter->drawLine(
				(int)leftMargin, y, (int)leftMargin+m_pageWidth-1, y);
			m_painter->setPen(Qt::black);
		}
		//record spacing
		y += m_mainLineSpacing;
		if (m_settings->addTableBorders)
			y += m_mainLineSpacing; //even more
	}

	if (int(m_dataOffsets.count()-1)==pageNumber) {//this was next page
		m_dataOffsets.append(new uint(offset));
	}
	m_eof = offset == rows;
	return true;
}

#include "kexisimpleprintingengine.moc"
