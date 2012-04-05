/* This file is part of the KDE project
   Copyright (C) 2005-2007 Jarosław Staniek <staniek@kde.org>

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

#include <QLabel>
#include <QLayout>
#include <QPainter>
#include <QCheckBox>
#include <QWhatsThis>
#include <q3paintdevicemetrics.h>
#include <QPixmap>
#include <QImage>

#include <kexiutils/tristate.h>
#include <kexidb/connection.h>
#include <kexidb/tableschema.h>
#include <kexidb/cursor.h>
#include <kexidb/utils.h>
#include <kexidb/queryschema.h>
#include <kglobal.h>

KexiSimplePrintingSettings::KexiSimplePrintingSettings()
{
    addPageNumbers = true;
    addDateAndTime = true;
    addTableBorders = false;
    pageTitleFont = kapp->font();
    pageTitleFont.setPointSizeFloat((double)QFontInfo(pageTitleFont).pointSize()*1.5);
    pageTitleFont.setBold(true);
}

KexiSimplePrintingSettings::~KexiSimplePrintingSettings()
{
}

KexiSimplePrintingSettings KexiSimplePrintingSettings::load()
{
    KexiSimplePrintingSettings settings; //this will set defaults

    KSharedConfig::Ptr config = KGlobal::config();
    config->setGroup("Simple Printing");
    if (config->hasKey("pageTitleFont"))
        settings.pageTitleFont = config->readEntry("pageTitleFont", QFont());
//! @todo system default?
    if (config->hasKey("pageFormat"))
        settings.pageLayout.format = KoPageFormat::formatFromString(
                                         config->readEntry("pageFormat"));
    if (config->readEntry("pageOrientation", "portrait").toLower() == "landscape")
        settings.pageLayout.orientation = PG_LANDSCAPE;
    else
        settings.pageLayout.orientation = PG_PORTRAIT;
    if (config->hasKey("pageWidth"))
        settings.pageLayout.ptWidth = config->readEntry("pageWidth");
    if (config->hasKey("pageHeight"))
        settings.pageLayout.ptHeight = config->readEntry("pageHeight");
    if (config->hasKey("pageLeftMargin"))
        settings.pageLayout.ptLeft = config->readEntry("pageLeftMargin");
    if (config->hasKey("pageRightMargin"))
        settings.pageLayout.ptRight = config->readEntry("pageRightMargin");
    if (config->hasKey("pageTopMargin"))
        settings.pageLayout.ptTop = config->readEntry("pageTopMargin");
    if (config->hasKey("pageBottomMargin"))
        settings.pageLayout.ptBottom = config->readEntry("pageBottomMargin");
    settings.addPageNumbers = config->readBoolEntry("addPageNumbersToPage", true);
    settings.addDateAndTime = config->readBoolEntry("addDateAndTimePage", true);
    settings.addTableBorders = config->readBoolEntry("addTableBorders", false);
    return settings;
}

void KexiSimplePrintingSettings::save()
{
    KSharedConfig::Ptr config = KGlobal::config();
    config->setGroup("Simple Printing");
    config->writeEntry("pageTitleFont", pageTitleFont);
    config->writeEntry("pageFormat", KoPageFormat::formatString(pageLayout.format));
    config->writeEntry("pageOrientation",
                       pageLayout.orientation == PG_PORTRAIT ? "portrait" : "landscape");
    config->writeEntry("pageWidth", pageLayout.ptWidth);
    config->writeEntry("pageHeight", pageLayout.ptHeight);
    config->writeEntry("pageLeftMargin", pageLayout.ptLeft);
    config->writeEntry("pageRightMargin", pageLayout.ptRight);
    config->writeEntry("pageTopMargin", pageLayout.ptTop);
    config->writeEntry("pageBottomMargin", pageLayout.ptBottom);
    config->writeEntry("addPageNumbersToPage", addPageNumbers);
    config->writeEntry("addDateAndTimePage", addDateAndTime);
    config->writeEntry("addTableBorders", addTableBorders);
    config->sync();
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
    m_visibleFieldsCount = 0;
//qt 4 m_dataOffsets.setAutoDelete(true);
    clear();
}

KexiSimplePrintingEngine::~KexiSimplePrintingEngine()
{
    done();
}

bool KexiSimplePrintingEngine::init(KexiDB::Connection& conn,
                                    KexiDB::TableOrQuerySchema& tableOrQuery, const QString& titleText, QString& errorMessage)
{
    errorMessage.clear();
    done();
    m_headerText = titleText; //tableOrQuery.captionOrName();

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
    bool ok = !m_cursor->error();
    if (ok) {
        m_data = new KexiTableViewData(m_cursor);
//! @todo primitive: data should be loaded on demand
        m_data->preloadAllRows();
        m_fieldsExpanded = query->fieldsExpanded(KexiDB::QuerySchema::WithInternalFields);
        m_visibleFieldsCount = m_cursor->query()->fieldsExpanded().count(); //real fields count without internals
    } else {
        conn.debugError();
    }
    m_eof = !ok || m_data->count() == 0;
    conn.deleteCursor(m_cursor);
    m_cursor = 0;
    return ok;
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
    m_pagesCount = 0;
    m_paintInitialized = false;
    m_fieldsExpanded.clear();
    m_visibleFieldsCount = 0;
    return result;
}

void KexiSimplePrintingEngine::clear()
{
    m_eof = false;
    m_pagesCount = 0;
    m_dataOffsets.clear();
    DataOffset *offset = new DataOffset;
    offset->record = 0;
    m_dataOffsets.append(offset);
    m_paintInitialized = false;
}

void KexiSimplePrintingEngine::paintPage(int pageNumber, QPainter& painter, bool paint)
{
    double y = 0.0;

    const bool printing = painter.device()->devType() == QInternal::Printer;
    m_SCALE = printing ? 1 : 20;

    double w, h;
    m_pdm = Q3PaintDeviceMetrics(painter.device());

    if (dynamic_cast<QWidget*>(painter.device())) {
        w = dynamic_cast<QWidget*>(painter.device())->width() * m_SCALE;
        h = dynamic_cast<QWidget*>(painter.device())->height() * m_SCALE;
    } else if (dynamic_cast<QPixmap*>(painter.device())) {
        w = dynamic_cast<QPixmap*>(painter.device())->width() * m_SCALE;
        h = dynamic_cast<QPixmap*>(painter.device())->height() * m_SCALE;
    } else {//KPrinter...
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
        if (!printing) {
            m_dpiY = 96;
            m_dpiX = 96;
//   m_dpiY = 86;
//   m_dpiX = 86;
        }
#endif
        double pdWidthMM = m_pdm.widthMM();
        double pdHeightMM = m_pdm.heightMM();

//  double screenF;
//   screenF = 1.0;

        m_leftMargin = POINT_TO_INCH(m_settings->pageLayout.ptLeft) * m_dpiX * (double)m_SCALE;//* screenF;
        m_rightMargin = POINT_TO_INCH(m_settings->pageLayout.ptRight) * m_dpiX * (double)m_SCALE;//* screenF;
        m_topMargin = POINT_TO_INCH(m_settings->pageLayout.ptTop) * m_dpiY * (double)m_SCALE;//* screenF;
        m_bottomMargin = POINT_TO_INCH(m_settings->pageLayout.ptBottom) * m_dpiY * (double)m_SCALE;//* screenF;

        m_fx = widthMM / (pdWidthMM);// * screenF);
        m_fy = heightMM / (pdHeightMM);// * screenF);

        //screen only
        // painter.fillRect(QRect(0,0,w,h), QBrush(white));
        m_pageWidth = uint(m_fx * double(m_pdm.width()) * m_SCALE - m_leftMargin - m_rightMargin);
        m_pageHeight = uint(m_fy * double(m_pdm.height()) * m_SCALE - m_topMargin - m_bottomMargin);
        m_headerFont = m_settings->pageTitleFont;
        if (!printing) {
            int pixelSize = int(POINT_TO_INCH((double)QFontInfo(m_headerFont).pointSize()) * m_dpiX) * m_SCALE;
            m_headerFont.setPixelSize(pixelSize);
        }

//! @todo add setting
        m_mainFont = kapp->font();
        if (!printing) {
            int pixelSize = int(POINT_TO_INCH(m_mainFont.pointSizeFloat()) * m_dpiX)
                            * m_SCALE;
            m_mainFont.setPixelSize(pixelSize);
        }
        painter.setFont(m_mainFont);

        m_dateTimeText = KGlobal::locale()->formatDateTime(QDateTime::currentDateTime(),
                         true, false);
        m_dateTimeWidth = painter.fontMetrics().width(m_dateTimeText + "   ");
        m_mainLineSpacing = painter.fontMetrics().lineSpacing();
        m_footerHeight = m_mainLineSpacing * 2; //2 lines
        painter.setFont(m_headerFont);
        m_headerTextRect = painter.fontMetrics().boundingRect(
                               (int)m_leftMargin, (int)m_topMargin,
                               m_pageWidth - m_dateTimeWidth,
                               m_pageHeight, Qt::AlignLeft | Qt::TextWordWrap, m_headerText);
        m_headerTextRect.setRight(m_headerTextRect.right() + 10);
        m_headerTextRect.setWidth(
            qMin(int(m_pageWidth - m_dateTimeWidth), m_headerTextRect.width()));

        //--compute max width of field names
        m_maxFieldNameWidth = 0;

        painter.setFont(m_mainFont);
        for (uint i = 0; i < m_visibleFieldsCount; i++) {
            const int newW =
                painter.fontMetrics().width(m_fieldsExpanded[i]->captionOrAliasOrName() + ":");
//   kDebug() << "row"<<i<<": "<<m_fieldsExpanded[i]->captionOrAliasOrName()<<" "
//    << newW;
            if (m_maxFieldNameWidth < newW)
                m_maxFieldNameWidth = newW;
        }
        m_maxFieldNameWidth += painter.fontMetrics().width("ww"); //more space
    }

    //screen only
    if (!printing) {
        painter.setWindow(0, 0, int(w*m_fx), int(h*m_fy));
    }

    //paint header
    painter.setFont(m_headerFont);
    if (paint) {
        painter.drawText(m_headerTextRect, Qt::AlignLeft | Qt::TextWordWrap, m_headerText);
    }
    painter.setFont(m_mainFont);
    if (paint) {
        painter.drawText((int)m_leftMargin + m_pageWidth - m_dateTimeWidth,
                         (int)m_topMargin, m_dateTimeWidth,
                         m_headerTextRect.height(), Qt::AlignRight, m_dateTimeText);
        //footer
        QString pageNumString;
        if (m_pagesCount > 0)
            pageNumString = i18nc("Page (number) of (total)", "Page %1 of %2",
                                  pageNumber + 1, m_pagesCount);
        else
            pageNumString = i18n("Page %1", pageNumber + 1);
        painter.drawText((int)m_leftMargin,
                         (int)m_topMargin + m_pageHeight - m_mainLineSpacing,
                         m_pageWidth, m_mainLineSpacing,
                         Qt::AlignRight | Qt::AlignBottom, pageNumString);
        painter.drawLine((int)m_leftMargin,
                         (int)m_topMargin + m_pageHeight - m_mainLineSpacing*3 / 2,
                         (int)m_leftMargin + m_pageWidth,
                         (int)m_topMargin + m_pageHeight - m_mainLineSpacing*3 / 2);
    }
    y = (double)m_topMargin + (double)m_headerTextRect.height() + double(m_mainLineSpacing) / 2;
    if (!m_settings->addTableBorders) {
        //separator line
        if (paint)
            painter.drawLine((int)m_leftMargin, (int)y, (int)m_leftMargin + m_pageWidth - 1, (int)y);
        y += (double)m_mainLineSpacing;
    }

    //--print records
// const uint count = m_fieldsExpanded.count();
// const uint count = m_cursor->query()->fieldsExpanded().count(); //real fields count without internals
    const uint records = m_data->count();
    const int cellMargin = m_settings->addTableBorders ?
                           painter.fontMetrics().width("i") : 0;
    uint paintedRecords = 0;
    DataOffset offset;
    if (pageNumber < (int)m_dataOffsets.count())
        offset = *m_dataOffsets.at(pageNumber);
    bool continuedRecord = false; // used for drawing |> arrow for continued records
    if (pageNumber < ((int)m_dataOffsets.count() - 1) && pageNumber > 0)
        continuedRecord = m_dataOffsets.at(pageNumber - 1)->record == offset.record;

    uint record = offset.record == -1 ? 0 : offset.record;
    while (record < records) {
        //compute height of this record
        double newY = y;
        DataOffset newOffset;
        paintRecord(painter, offset, /*item,*/ cellMargin, newY, paintedRecords,
                    false/* !paint */, printing, continuedRecord, newOffset);
//  if ((int(m_topMargin + m_pageHeight-newY-m_footerHeight)) < 0 /*(1)*/ && paintedRecords > 0/*(2)*/) {
#if 0
        if (
            newY > (m_topMargin + m_pageHeight - m_mainLineSpacing*2 + m_mainLineSpacing) /*(1)*/ &&
            paintedRecords > 0/*(2)*/) {
            //[disabled](1) do not break records between pages
            //(2) but paint at least one record
//! @todo break large records anyway...
            break;
        }
#endif
        if ((newY - y) < (m_mainLineSpacing * 3)) // jump to another page if there are less than 3 rows for use
            break;
        /*  if (int(count * m_mainLineSpacing) > int(m_topMargin + m_pageHeight-(int)y-m_footerHeight))
            {
              //do not break records between pages
              break;
            }*/
//  kDebug() << " -------- " << y << " / " << m_pageHeight;
        if (paint)
            paintRecord(painter, offset, /*item,*/ cellMargin, y, paintedRecords,
                        paint, printing, continuedRecord, newOffset);
        else
            y = newY; //speedup
        if (newOffset.record >= 0) {
            offset = newOffset;
            break;
        }
        paintedRecords++;
        record++;
        // next record will be painted from the beginning
        offset.record = record;
        offset.field = 0;
        offset.textOffset = 0;
        continuedRecord = false;
    } // while

    if (int(m_dataOffsets.count() - 1) == pageNumber) {//this was the next page
        m_dataOffsets.append(new DataOffset(offset));
    }
    m_eof = record == records;
}

inline QRect getRealRect(const QFontMetrics& fontMetrics,
                         QRect& rect, int alignFlags, const QString& text)
{
    return fontMetrics.boundingRect(
               rect.x(), rect.y(), rect.width(), rect.height(),
               alignFlags, text);
}

uint KexiSimplePrintingEngine::cutTextIfTooLarge(const QFontMetrics& fontMetrics,
        QRect& rect, int alignFlags, const QString& text)
{
    QRect realRect;
    uint delta = text.length();
    kDebug() << "\ntext==" << text << "==text\n";
    uint cut = delta;
    const bool updateRect = delta > 0;
    while (delta > 0) {
        realRect = getRealRect(fontMetrics, rect, alignFlags, text.left(cut));
        if (realRect.height() > rect.height()) {
            cut -= delta / 2;
            delta /= 2;
        } else {//this fragment fits in the rect
            if (delta == text.length()) {
                realRect = getRealRect(fontMetrics, rect, alignFlags, text.left(cut)); //.trimmed());
                break;
            }
            cut += delta / 2;
            delta /= 2;
        }
        if (delta == 0) { //final cut
            if (text.length() == cut || text.length() == 0 || cut == 0)
                break;
            if (text.at(cut).isSpace() && !text.at(cut - 1).isSpace())
                break;
            if (text.at(cut - 1).isSpace()) {
                // we're at whitespace: eat it
                cut--;
                for (;cut > 0 && text.at(cut).isSpace();cut--)
                    ;
            } else {
                // we're at a word: eat it
                for (;cut > 0 && !text.at(cut).isSpace();cut--)
                    ;
                if (cut > 0) {
                    // we're at whitespace: eat it
                    for (;cut > 0 && text.at(cut).isSpace();cut--)
                        ;
                    cut++;
                }
            }
            rect.setHeight(rect.height() - m_mainLineSpacing);   // add some pace for the 'more' arrow
            delta = 1; // just one more step to compute realRect
        }
    }
    if (updateRect)
        rect.setHeight(realRect.height());
    return cut;
}

void paintArrow(QPainter& painter, int x, int y, int arrowSize)
{
    QPen oldPen(painter.pen());
    painter.save();
    QPointArray arrow;
    arrow.putPoints(0, 4,
                    0, 0,
                    arrowSize, arrowSize / 2,
                    0, arrowSize,
                    0, 0);
// painter.setBrush(QBrush(Qt::darkGray));
    painter.setPen(Qt::darkGray);
    painter.save();
    painter.translate(x, y);
    painter.drawPolyline(arrow);
    painter.restore();
    painter.setPen(oldPen);
}

void KexiSimplePrintingEngine::paintRecord(QPainter& painter, const DataOffset& offset,
        int cellMargin, double &y, uint paintedRecord, bool paint, bool printing,
        bool continuedRecord, DataOffset& newOffset)
{
    KexiTableItem *item = m_data->at(offset.record);

    if (paintedRecord > 0 && !m_settings->addTableBorders) {//separator
        if (paint) {
            painter.setPen(Qt::darkGray);
            painter.drawLine(
                int(m_leftMargin), int(y - (double)m_mainLineSpacing),
                int(m_leftMargin) + m_pageWidth - 1, int(y - (double)m_mainLineSpacing));
            painter.setPen(Qt::black);
        }
    }

    const int arrowSize = m_mainLineSpacing / 2;
    if (continuedRecord)
        y += arrowSize;
    uint cut = 0;
    uint i = offset.field;
    QRect rect;
    QString text;
    for (; i < m_visibleFieldsCount; i++) {
//   kDebug() << "row"<<i<<": "<<row.at(i).toString();
        if (paint) {
            painter.drawText(
                (int)m_leftMargin + cellMargin, (int)y, m_maxFieldNameWidth - cellMargin*2, m_mainLineSpacing,
                Qt::AlignTop, m_fieldsExpanded[i]->captionOrAliasOrName()
                + (m_settings->addTableBorders ? "" : ":"));
        }
        text.clear();
//! @todo optimize like in KexiCSVExport::exportData()
        //get real column and real index to get the visible value
        KexiDB::QueryColumnInfo* ci;
        int indexForVisibleLookupValue = m_fieldsExpanded[i]->indexForVisibleLookupValue();
        if (-1 != indexForVisibleLookupValue && indexForVisibleLookupValue < (int)record->count()/*sanity*/)
            ci = m_fieldsExpanded[ indexForVisibleLookupValue ];
        else {
            ci = m_fieldsExpanded[ i ];
            indexForVisibleLookupValue = i;
        }

        QVariant v(record->at(indexForVisibleLookupValue));
        KexiDB::Field::Type ftype = ci->field->type();
//  QRect rect( (int)m_leftMargin + m_maxFieldNameWidth + cellMargin, (int)y,
//   m_pageWidth - m_maxFieldNameWidth - cellMargin*2, m_pageHeight - (int)y);
        rect = QRect(
                   (int)m_leftMargin + m_maxFieldNameWidth + cellMargin,
                   (int)y,
                   m_pageWidth - m_maxFieldNameWidth - cellMargin * 2,
                   int(m_pageHeight - ((int)y - m_topMargin) - m_mainLineSpacing * 2)
                   - (continuedRecord ? arrowSize * 2 : 0)
               );
        if (v.isNull() || !v.isValid()) {
            //nothing to do
        }
//! todo inherit format
        else if (ftype == KexiDB::Field::DateTime) {
            QDateTime dt(v.toDateTime());
            if (dt.isValid())
                text = KGlobal::locale()->formatDateTime(dt);
        }
//! todo inherit format
        else if (ftype == KexiDB::Field::Date) {
            QDate date(v.toDate());
            if (date.isValid())
                text = KGlobal::locale()->formatDate(date, KLocale::ShortDate);
        }
//! todo inherit format
        else if (ftype == KexiDB::Field::Time) {
            QTime time(v.toTime());
            if (time.isValid())
                text = KGlobal::locale()->formatTime(time);
        }
//! todo currency, decimal...
        else if (ci->field->isFPNumericType())
            text = KGlobal::locale()->formatNumber(v.toDouble());
        else if (ftype == KexiDB::Field::Boolean)
            text = v.toBool()
                   ? i18n("Boolean Yes (true)", "Yes") : i18n("Boolean No (false)", "No");
        else if (ftype == KexiDB::Field::BLOB) {
            const QByteArray ba(v.toByteArray());
            if (!ba.isEmpty()) {
                QPixmap pixmap(ba);
#define MAX_PIXMAP_HEIGHT (m_mainLineSpacing * 5)
                double pixmapHeight = MAX_PIXMAP_HEIGHT;
                double pixmapWidth = double(MAX_PIXMAP_HEIGHT) * pixmap.width() / (double)pixmap.height();
                if (pixmapWidth > (double)rect.width()) { //too wide
                    pixmapHeight = pixmapHeight * (double)rect.width() / pixmapWidth;
                    pixmapWidth = rect.width();
                }
                rect.setHeight(int(pixmapHeight + m_mainLineSpacing / 2));
                if (paint && !pixmap.isNull()) {
                    if (printing) {
                        painter.drawPixmap(
                            QRect(rect.x(), rect.y() + m_mainLineSpacing / 4,
                                  int(pixmapWidth), int(pixmapHeight)), pixmap);
                    } else {// we're just previewing the pixmap, so let's resize it and cache
                        // so redrawing will be faster
                        painter.save();
                        painter.setWindow( // set 1:1 scale to avoid unnecessary image scaling
                            QRect(painter.window().topLeft(),
                                  painter.window().size() / (int)m_SCALE));
                        painter.drawImage(
                            int(rect.x() / m_SCALE),
                            int((rect.y() + m_mainLineSpacing / 4) / m_SCALE),
                            pixmap.convertToImage().smoothScale(
                                int(pixmapWidth / m_SCALE), int(pixmapHeight / m_SCALE),
                                QImage::ScaleMin));
                        painter.restore(); // back to m_SCALE:1 scale
                    }
                }
            }
        } else
            text = v.toString(); //.trimmed();

        cut = 0;
        const int textOffsetForThisField = (i == offset.field) ? offset.textOffset : 0;
        if (ftype != KexiDB::Field::BLOB || v.isNull() || !v.isValid()) {
            /*   rect = QRect( painter.fontMetrics().boundingRect(
                    rect.x(), rect.y(), rect.width(), rect.height(),
                    Qt::AlignAuto|Qt::WordBreak, text) );*/
            cut = textOffsetForThisField + cutTextIfTooLarge(
                      painter.fontMetrics(), rect, Qt::AlignTop | Qt::WordBreak,
                      ((textOffsetForThisField > 0) ? text.mid(textOffsetForThisField) : text));
        }
        if (!text.isEmpty() && paint) {
//   kdDebug() << "print engine: painter.drawText: "
//    << rect.x() <<" "<< rect.y() <<" "<< m_pageWidth - m_maxFieldNameWidth - cellMargin*2
//    <<" "<< m_topMargin + m_pageHeight - (int)y <<" "<<m_pageHeight<<" "<<y<<" "<< text;
//   QString firstPartOfText(text), remainingPartOfText;
            //QRect realRect( rect.x(),rect.y(),
//    m_pageWidth - m_maxFieldNameWidth - cellMargin*2,
            //   int(m_pageHeight - ((int)y - m_topMargin) - m_mainLineSpacing*2 ) );
//   uint cut = cutTextIfTooLarge(
//    painter.fontMetrics(), realRect, Qt::AlignTop|Qt::WordBreak,text);

            QString aa(((textOffsetForThisField > 0) ? text.mid(textOffsetForThisField) : text).left(cut).trimmed());
            kDebug() << "-----" << aa << endl << "=====";
            kDebug() << "-----text.mid(textOffsetForThisField):" << text.mid(textOffsetForThisField) << endl << "=====";
            kDebug() << "-----text.mid(textOffsetForThisField).left(cut):" << text.mid(textOffsetForThisField).left(cut) << endl << "=====";

            QString xxxx_start, xxxx_end;
            if (textOffsetForThisField > 0) {
                xxxx_start = text.mid(textOffsetForThisField).left(cut - textOffsetForThisField).left(20);
                xxxx_end = text.mid(textOffsetForThisField).left(cut - textOffsetForThisField).right(20);
            } else {
                xxxx_start = text.left(cut - textOffsetForThisField).left(20);
                xxxx_end = text.left(cut - textOffsetForThisField).right(20);
            }
            painter.drawText(
//    rect.x(), rect.y(), m_pageWidth - m_maxFieldNameWidth - cellMargin*2,
//    int(m_pageHeight - ((int)y - m_topMargin)),
                rect, Qt::AlignTop | Qt::WordBreak,
                ((textOffsetForThisField > 0) ? text.mid(textOffsetForThisField) : text).left(cut - textOffsetForThisField).trimmed());
        }
        if (m_settings->addTableBorders) {
            if (paint) {
                painter.setPen(Qt::darkGray);
                painter.drawLine(
                    (int)m_leftMargin, rect.top(), (int)m_leftMargin + m_pageWidth - 1, rect.top());
                painter.drawLine(
                    (int)m_leftMargin, rect.top(), (int)m_leftMargin, rect.bottom());
                painter.drawLine(
                    (int)m_leftMargin + m_pageWidth - 1, rect.top(),
                    (int)m_leftMargin + m_pageWidth - 1, rect.bottom());
                painter.drawLine(
                    (int)m_leftMargin + m_maxFieldNameWidth, rect.top(),
                    (int)m_leftMargin + m_maxFieldNameWidth, rect.bottom());
                painter.setPen(Qt::black);
            }
        }
        y += (double)rect.height();
        if (cut < text.length())
            break;
    } //for
    if (m_settings->addTableBorders) {
        if (paint) {
            painter.setPen(Qt::darkGray);
            painter.drawLine(
                (int)m_leftMargin, (int)y, (int)m_leftMargin + m_pageWidth - 1, (int)y);
            painter.setPen(Qt::black);
        }
    }
    //record spacing
    y += double(m_mainLineSpacing) * 3.0 / 2.0;

    if (cut < text.length()) {
        newOffset.record = offset.record;
        newOffset.field = i;
        newOffset.textOffset = cut;
        if (paint) { //add |> arrow at the bottom
            int bottomRightX = (int)m_leftMargin + m_pageWidth - 1 - m_mainLineSpacing / 4;
            int bottomRightY = rect.bottom() - m_mainLineSpacing + arrowSize / 3;
            paintArrow(painter, bottomRightX - 3*arrowSize / 2, bottomRightY, arrowSize);
        }
    }
    if (continuedRecord && paint) { //add |> arrow on the top
        paintArrow(painter, m_leftMargin + arrowSize / 2, rect.top() - 3*arrowSize / 2, arrowSize);
    }
// if (m_settings->addTableBorders)
//  y -= m_mainLineSpacing; //a bit less
}

void KexiSimplePrintingEngine::calculatePagesCount(QPainter& painter)
{
    if (m_eof || !m_data) {
        m_pagesCount = 0;
        return;
    }

    uint pageNumber = 0;
    for (;!m_eof; ++pageNumber) {
        paintPage(pageNumber, painter, false /* !paint */);
    }
    m_pagesCount = pageNumber;
}

void KexiSimplePrintingEngine::setTitleText(const QString& titleText)
{
    m_headerText = titleText;
}

#include "kexisimpleprintingengine.moc"
