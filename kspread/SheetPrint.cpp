/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2005 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
   Copyright 2003 Philipp Müller <philipp.mueller@gmx.de>
   Copyright 1998, 1999 Torben Weis <weis@kde.org>

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
   Boston, MA 02110-1301, USA.
*/

// Local
#include "SheetPrint.h"

#include <kdebug.h>
#include <kmessagebox.h>
#include "Limits.h"

#include "ApplicationSettings.h"
#include "part/Doc.h" // FIXME detach from part
#include "Localization.h"
#include "Map.h"
#include "RowColumnFormat.h"
#include "Sheet.h"
#include "ui/SheetView.h"
#include "Selection.h"

#include "part/commands/DefinePrintRangeCommand.h" // FIXME detach from part

#include <KoDocumentInfo.h>
#include <KoZoomHandler.h>

#include <QLinkedList>
#include <QPainter>
#include <QPixmap>
#include <QPointF>
#include <QRectF>

#include <pwd.h>
#include <unistd.h>

#include "SheetPrint.moc"

#define NO_MODIFICATION_POSSIBLE \
    do { \
        KMessageBox::error( 0, i18n ( "You cannot change a protected sheet" ) ); return; \
    } while(0)

namespace KSpread
{

class PrintNewPageEntry
{
public:
    explicit PrintNewPageEntry(int startItem, int endItem = 0, double size = 0, double offset = 0)
            : m_iStartItem(startItem)
            , m_iEndItem(endItem)
            , m_dSize(size)
            , m_dOffset(offset) {}

    int startItem() const {
        return m_iStartItem;
    }
    void setStartItem(int startItem) {
        m_iStartItem = startItem;
    }

    int endItem() const {
        return m_iEndItem;
    }
    void setEndItem(int endItem) {
        m_iEndItem = endItem;
    }

    double size() const {
        return m_dSize;
    }
    void setSize(double size) {
        m_dSize = size;
    }

    double offset() const {
        return m_dOffset;
    }
    void setOffset(double offset) {
        m_dOffset = offset;
    }

    bool operator==(PrintNewPageEntry const & entry) const;


private:
    int m_iStartItem;
    int m_iEndItem;
    double m_dSize;
    double m_dOffset;
};


#if 0 // KSPREAD_KOPART_EMBEDDING
class PrintObject
{
public:
    PrintObject() : obj(0), p(0) {}

    EmbeddedObject *obj;
    QPixmap *p;
};
#endif

} // namespace KSpread

using namespace KSpread;

SheetPrint::SheetPrint(Sheet* sheet)
{
    m_pSheet = sheet;
    m_pSheetView = new SheetView(sheet);
    m_pDoc = m_pSheet->doc();

    m_settings = new PrintSettings();

    m_printRange = QRect(QPoint(1, 1), QPoint(KS_colMax, KS_rowMax));
    m_maxCheckedNewPageX = 1;
    m_maxCheckedNewPageY = 1;
    m_dPrintRepeatColumnsWidth = 0.0;
    m_dPrintRepeatRowsHeight = 0.0;
    m_printRepeatColumns = qMakePair(0, 0);
    m_printRepeatRows = qMakePair(0, 0);
    m_zoomHandler = new KoZoomHandler();
    m_iPageLimitX = 0;
    m_iPageLimitY = 0;
}

SheetPrint::~SheetPrint()
{
    delete m_zoomHandler;
    delete m_settings;
    delete m_pSheetView;
}

QString SheetPrint::saveOdfSheetStyleLayout(KoGenStyles &mainStyles)
{
    KoGenStyle pageLayout = m_settings->pageLayout().saveOdf();
    //pageLayout.addAttribute( "style:page-usage", "all" ); FIXME
    //necessary for print setup
    m_pSheet->saveOdfPrintStyleLayout(pageLayout);
    // this is called from Sheet::saveOdfSheetStyleName for writing the SytleMaster so
    // the style has to be in the styles.xml file and only there
    pageLayout.setAutoStyleInStylesDotXml(true);

    return mainStyles.insert(pageLayout, "pm");
}


QRect SheetPrint::cellsPrintRange()
{
    // Find maximum right/bottom cell with content
    QRect cell_range(m_pSheet->usedArea());
    return cell_range;
}

int SheetPrint::pagesX(const QRect& cellsPrintRange)
{
    int pages = 0;

    updateNewPageX(cellsPrintRange.right());

    for (int i = cellsPrintRange.left(); i <= cellsPrintRange.right(); i++) {
        if (isColumnOnNewPage(i))
            pages++;
    }
    return pages;
}

int SheetPrint::pagesY(const QRect& cellsPrintRange)
{
    int pages = 0;

    updateNewPageY(cellsPrintRange.bottom());

    for (int i = cellsPrintRange.top(); i <= cellsPrintRange.bottom(); i++) {
        if (isRowOnNewPage(i))
            pages++;
    }
    return pages;
}


bool SheetPrint::pageNeedsPrinting(QRect& page_range)
{
    // bool filled = false;

    // Look at the cells
    for (int r = page_range.top();  r <= page_range.bottom() ; ++r)
        for (int c = page_range.left();  c <= page_range.right() ; ++c)
            if (Cell(m_pSheet, c, r).needsPrinting()) {
                return true;
            }
    // filled = true;

    //Page has no visible content on it, so we don't need to paint it
    return false;
}

void SheetPrint::generateThumbnail(QPainter &painter)
{
    painter.scale(m_zoomHandler->zoomedResolutionX(), m_zoomHandler->zoomedResolutionY());
    painter.setClipRect(0, 0, (int)paperLayout().width, (int)paperLayout().height);
    QRect range = cellsPrintRange();
    if (range.height() > 100) range.setHeight(100);
    if (range.width() > 100) range.setWidth(100);
    printPage(painter, range, QRect(0, 0, paperLayout().width, paperLayout().height), QPointF(0,0));
}

bool SheetPrint::print(QPainter &painter, QPrinter *_printer)
{
    kDebug(36001) << "PRINTING ....";

    // Override the current grid pen setting, when set to disable
    QPen gridPen;
    bool oldShowGrid = m_pSheet->getShowGrid();
    m_pSheet->setShowGrid(m_settings->printGrid());
    if (!m_settings->printGrid()) {
        gridPen = QPen(m_pSheet->map()->settings()->gridColor(), 1, Qt::SolidLine);
        QPen nopen;
        nopen.setStyle(Qt::NoPen);
        m_pSheet->map()->settings()->setGridColor(Qt::white);
    }

    //Update m_dPrintRepeatColumnsWidth for repeated columns
    //just in case it isn't done yet
    if (!m_pSheet->isShowPageBorders() && m_printRepeatColumns.first != 0)
        updatePrintRepeatColumnsWidth();

    //Update m_dPrintRepeatRowsHeight for repeated rows
    //just in case it isn't done yet
    if (!m_pSheet->isShowPageBorders() && m_printRepeatRows.first != 0)
        updatePrintRepeatRowsHeight();

    //Calculate the range to be printed
    QRect cell_range = cellsPrintRange();
    kDebug() << "cellsPrintRange() :" << cellsPrintRange();
    //Ensure, that our newPage lists are generated for the whole sheet to print
    //For this we add to the lists the width/height of 1 page
    updateNewPageX(cell_range.right());
    updateNewPageY(cell_range.bottom());

    // Find out how many pages need printing
    // and which cells to print on which page.
    QLinkedList<QRect> page_list;  //contains the cols and rows of a page
    QLinkedList<QRectF> page_frame_list;  //contains the coordinate range of a page
    QLinkedList<QPointF> page_frame_list_offset;  //contains the offset of the not repeated area

    QList<PrintNewPageEntry>::iterator itX;
    QList<PrintNewPageEntry>::iterator itY;
    for (itX = m_lnewPageListX.begin(); itX != m_lnewPageListX.end(); ++itX) {
        for (itY = m_lnewPageListY.begin(); itY != m_lnewPageListY.end(); ++itY) {
            QRect page_range(QPoint((*itX).startItem(), (*itY).startItem()),
                             QPoint((*itX).endItem(), (*itY).endItem()));
            kDebug() << " page_range :" << page_range;
            //Append page when there is something to print
            if (pageNeedsPrinting(page_range)) {
                QRectF view = QRectF(QPointF(0.0, 0.0),
                                     QSizeF(m_pSheet->columnPosition(page_range.right())
                                            + m_pSheet->columnFormat(page_range.right())->width()
                                            - m_pSheet->columnPosition(page_range.left()),
                                            m_pSheet->rowPosition(page_range.bottom())
                                            + m_pSheet->rowFormat(page_range.bottom())->height()
                                            - m_pSheet->rowPosition(page_range.top())));
                page_list.append(page_range);
                page_frame_list.append(view);
                page_frame_list_offset.append(QPointF((*itX).offset(), (*itY).offset()));
            }
        }
    }


    kDebug(36001) << "PRINTING" << page_list.count() << " pages";
    m_uprintPages = page_list.count();


    if (page_list.count() == 0) {
        // nothing to print
        painter.setPen(QPen(Qt::black, 1));
        painter.drawPoint(1, 1);
    } else {

        int pageNo = 1;

        //
        // Print all pages in the list
        //
        QLinkedList<QRect>::Iterator it = page_list.begin();
        QLinkedList<QRectF>::Iterator fit = page_frame_list.begin();
        QLinkedList<QPointF>::Iterator fito = page_frame_list_offset.begin();

        painter.translate(paperLayout().leftMargin, paperLayout().topMargin);
        painter.scale(m_zoomHandler->zoomedResolutionX(), m_zoomHandler->zoomedResolutionY());
        painter.setClipRect(0, 0, (int)paperLayout().width, (int)paperLayout().height);

        for (; it != page_list.end(); ++it, ++fit, ++fito, ++pageNo) {
            printHeaderFooter(painter, pageNo);

            // Print the page
            printPage(painter, *it, *fit, *fito);

            if (pageNo < (int)page_list.count())
                _printer->newPage();
        }
    }

    if (!m_settings->printGrid()) {
        // Restore the grid pen
        m_pSheet->map()->settings()->setGridColor(gridPen.color());
    }
    m_pSheet->setShowGrid(oldShowGrid);

#if 0 // KSPREAD_KOPART_EMBEDDING
    QList<PrintObject *>::iterator it;
    for (it = m_printObjects.begin(); it != m_printObjects.end(); ++it)
        delete(*it)->p;
    m_printObjects.clear();
#endif

    return (page_list.count() > 0);
}

void SheetPrint::printPage(QPainter &_painter, const QRect& page_range,
                           const QRectF& view, const QPointF _childOffset)
{
    kDebug(36001) << "Rect x=" << page_range.left() << " y=" << page_range.top() << ", r="
    << page_range.right() << " b="  << page_range.bottom() << "  offsetx: " << _childOffset.x()
    << "  offsety: " << _childOffset.y() << "  view-x: " << view.x() << endl;

    //Don't paint on the page borders
    QRegion clipRegion((int)_childOffset.x(),
                       (int)_childOffset.y(),
                       (int)(view.width() + _childOffset.x()),
                       (int)(view.height() + _childOffset.y()));
//     _painter.setClipRegion( clipRegion );

    //
    // Draw the cells.
    //
    //Check if we have to repeat some rows and columns (top left rect)
    if ((_childOffset.x() != 0.0) && (_childOffset.y() != 0.0)) {
        //QRect(left,top,width,height)  <<<< WIDTH AND HEIGHT!!!
        QRect _printRect(m_printRepeatColumns.first, m_printRepeatRows.first,
                         m_printRepeatColumns.second - m_printRepeatColumns.first + 1,
                         m_printRepeatRows.second - m_printRepeatRows.first + 1);
        QPointF _topLeft(0.0, 0.0);

        printRect(_painter, _topLeft, _printRect, view, clipRegion);
    }

    //Check if we have to repeat some rows (left rect)
    if (_childOffset.y() != 0) {
        QRect _printRect(page_range.left(), m_printRepeatRows.first,
                         page_range.right() - page_range.left() + 1,
                         m_printRepeatRows.second - m_printRepeatRows.first + 1);
        QPointF _topLeft(_childOffset.x(), 0.0);

        printRect(_painter, _topLeft, _printRect, view, clipRegion);
    }

    //Check if we have to repeat some columns (top right rect)
    if (_childOffset.x() != 0) {
        QRect _printRect(m_printRepeatColumns.first, page_range.top(),
                         m_printRepeatColumns.second - m_printRepeatColumns.first + 1,
                         page_range.bottom() - page_range.top() + 1);
        QPointF _topLeft(0.0, _childOffset.y());

        printRect(_painter, _topLeft, _printRect, view, clipRegion);
    }


    //Print the cells (right data rect)
    printRect(_painter, _childOffset, page_range, view, clipRegion);
}


void SheetPrint::printRect(QPainter& painter, const QPointF& topLeft,
                           const QRect& cellRange, const QRectF& view,
                           QRegion& clipRegion)
{
    Q_UNUSED(view);
//     kDebug() <<"topLeft:" << topLeft <<" cellRange:" << cellRange;
    // topLeft: starting coordinate (document coordinate system)
    // cellRange: cell range to be printed

    //
    // Draw the cells.
    //
    double xpos =  0;
    double ypos =  topLeft.y();

    int regionBottom = cellRange.bottom();
    int regionRight  = cellRange.right();
    int regionLeft   = cellRange.left();
    int regionTop    = cellRange.top();

    //Calculate the output rect
    QPointF bottomRight(topLeft);
    for (int x = regionLeft; x <= regionRight; ++x)
        bottomRight.setX(bottomRight.x()
                         + m_pSheet->columnFormat(x)->width());
    for (int y = regionTop; y <= regionBottom; ++y)
        bottomRight.setY(bottomRight.y()
                         + m_pSheet->rowFormat(y)->height());
    QRectF paintRect(topLeft, bottomRight);
//     kDebug() <<"view:" << view;
//     kDebug() <<"paintRect:" << paintRect;
//     kDebug() <<"clipRegion:" << painter.clipRegion();

//     painter.fillRect( painter.clipRegion().boundingRect(), Qt::lightGray );
//     painter.drawRect( paintRect );

    m_pSheetView->setPaintDevice(painter.device());
    m_pSheetView->setViewConverter(m_zoomHandler);
    m_pSheetView->setPaintCellRange(cellRange);
    m_pSheetView->paintCells(painter.device(), painter, paintRect, topLeft);

    //Don't let obscuring cells and children overpaint this area
    clipRegion -= QRegion((int)(paperLayout().leftMargin + topLeft.x()),
                          (int)(paperLayout().topMargin + topLeft.y()),
                          (int)xpos,
                          (int)ypos);
//     painter.setClipRegion( clipRegion );
}


void SheetPrint::printHeaderFooter(QPainter &painter, int pageNo)
{
    double w;
    double headFootDistance = MM_TO_POINT(5.0 /*mm*/);
    double leftMarginDistance = MM_TO_POINT(5.0 /*mm*/);
    QFont font("Times");
    font.setPointSizeF(/* Font size of 10 */ 10.0);
    painter.setFont(font);
    QFontMetrics fm = painter.fontMetrics();
    const double scale = POINT_TO_INCH(painter.device()->logicalDpiY());

    // print head line left
    w = fm.width(headLeft(pageNo, m_pSheet->sheetName()));
    if (w > 0)
        painter.drawText((int)leftMarginDistance, (int)headFootDistance,
                         headLeft(pageNo, m_pSheet->sheetName()));
    // print head line middle
    w = fm.width(headMid(pageNo, m_pSheet->sheetName()));
    if (w > 0)
        painter.drawText((int)(paperLayout().leftMargin + (m_settings->printWidth() - w) / 2.0),
                         (int)headFootDistance,
                         headMid(pageNo, m_pSheet->sheetName()));
    // print head line right
    w = fm.width(headRight(pageNo, m_pSheet->sheetName()));
    if (w > 0)
        painter.drawText((int)(paperLayout().leftMargin + m_settings->printWidth() - w),
                         (int)headFootDistance ,
                         headRight(pageNo, m_pSheet->sheetName()));

    // print foot line left
    w = fm.width(footLeft(pageNo, m_pSheet->sheetName()));
    if (w > 0)
        painter.drawText((int)leftMarginDistance,
                         (int)(m_settings->printHeight() * scale  + headFootDistance),
                         footLeft(pageNo, m_pSheet->sheetName()));
    // print foot line middle
    w = fm.width(footMid(pageNo, m_pSheet->sheetName()));
    if (w > 0)
        painter.drawText((int)(leftMarginDistance + (paperLayout().width - w) / 2.0),
                         (int)(m_settings->printHeight() * scale  + headFootDistance),
                         footMid(pageNo, m_pSheet->sheetName()));
    // print foot line right
    w = fm.width(footRight(pageNo, m_pSheet->sheetName()));
    if (w > 0)
        painter.drawText((int)(leftMarginDistance + paperLayout().width - w),
                         (int)(m_settings->printHeight() * scale  + headFootDistance),
                         footRight(pageNo, m_pSheet->sheetName()));
}


bool SheetPrint::isColumnOnNewPage(int _column)
{
    if (_column > m_maxCheckedNewPageX)
        updateNewPageX(_column);

    //Are these the edges of the print range?
    if (_column == m_printRange.left() || _column == m_printRange.right() + 1) {
        return true;
    }

    //beyond the print range it's always false
    if (_column < m_printRange.left() || _column > m_printRange.right()) {
        return false;
    }

    //Now check if we find the column already in the list
    if (m_lnewPageListX.indexOf(PrintNewPageEntry(_column)) != -1) {
        if (_column > m_maxCheckedNewPageX)
            m_maxCheckedNewPageX = _column;
        return true;
    }
    return false;
}


void SheetPrint::updateNewPageX(int _column)
{
    float offset = 0.0;

    //Are these the edges of the print range?
    if (_column == m_printRange.left() || _column == m_printRange.right() + 1) {
        if (_column > m_maxCheckedNewPageX)
            m_maxCheckedNewPageX = _column;
        return;
    }

    //We don't check beyond the print range
    if (_column < m_printRange.left() || _column > m_printRange.right()) {
        if (_column > m_maxCheckedNewPageX)
            m_maxCheckedNewPageX = _column;
        if (_column > m_printRange.right()) {
            if (m_lnewPageListX.last().endItem() == 0)
                m_lnewPageListX.last().setEndItem(m_printRange.right());
        }
        return;
    }

    //If we start, then add the left printrange
    if (m_lnewPageListX.empty())
        m_lnewPageListX.append(PrintNewPageEntry(m_printRange.left()));     //Add the first entry

    //If _column is greater than the last entry, we need to calculate the result
    if (_column > m_lnewPageListX.last().startItem() &&
            _column > m_maxCheckedNewPageX) { //this columns hasn't been calculated before
        int startCol = m_lnewPageListX.last().startItem();
        int col = startCol;
        double x = m_pSheet->columnFormat(col)->width();

        //Add repeated column width, when necessary
        if (col > m_printRepeatColumns.first) {
            x += m_dPrintRepeatColumnsWidth;
            offset = m_dPrintRepeatColumnsWidth;
        }

        while ((col <= _column) && (col < m_printRange.right())) {
            if (x > m_settings->printWidth()) { //end of page?
                //We found a new page, so add it to the list
                m_lnewPageListX.append(PrintNewPageEntry(col));

                //Now store into the previous entry the enditem and the width
                QList<PrintNewPageEntry>::iterator it;
                it = findNewPageColumn(startCol);
                (*it).setEndItem(col - 1);
                (*it).setSize(x - m_pSheet->columnFormat(col)->width());
                (*it).setOffset(offset);

                //start a new page
                startCol = col;
                if (col == _column) {
                    if (_column > m_maxCheckedNewPageX)
                        m_maxCheckedNewPageX = _column;
                    return;
                } else {
                    x = m_pSheet->columnFormat(col)->width();
                    if (col >= m_printRepeatColumns.first) {
                        x += m_dPrintRepeatColumnsWidth;
                        offset = m_dPrintRepeatColumnsWidth;
                    }
                }
            }

            col++;
            x += m_pSheet->columnFormat(col)->width();
        }
    }

    if (_column > m_maxCheckedNewPageX) {
        m_maxCheckedNewPageX = _column;
        m_lnewPageListX.last().setEndItem(_column);
    }
}


bool SheetPrint::isRowOnNewPage(int _row)
{
    if (_row > m_maxCheckedNewPageY)
        updateNewPageY(_row);

    //Are these the edges of the print range?
    if (_row == m_printRange.top() || _row == m_printRange.bottom() + 1) {
        return true;
    }

    //beyond the print range it's always false
    if (_row < m_printRange.top() || _row > m_printRange.bottom()) {
        return false;
    }

    //Now check if we find the row already in the list
    if (m_lnewPageListY.indexOf(PrintNewPageEntry(_row)) != -1) {
        if (_row > m_maxCheckedNewPageY)
            m_maxCheckedNewPageY = _row;
        return true;
    }

    return false;
}


void SheetPrint::updateNewPageY(int _row)
{
    float offset = 0.0;

    //Are these the edges of the print range?
    if (_row == m_printRange.top() || _row == m_printRange.bottom() + 1) {
        if (_row > m_maxCheckedNewPageY)
            m_maxCheckedNewPageY = _row;
        return;
    }

    //beyond the print range it's always false
    if (_row < m_printRange.top() || _row > m_printRange.bottom()) {
        if (_row > m_maxCheckedNewPageY)
            m_maxCheckedNewPageY = _row;
        if (_row > m_printRange.bottom()) {
            if (m_lnewPageListY.last().endItem() == 0)
                m_lnewPageListY.last().setEndItem(m_printRange.bottom());
        }
        return;
    }

    //If we start, then add the top printrange
    if (m_lnewPageListY.empty())
        m_lnewPageListY.append(PrintNewPageEntry(m_printRange.top()));     //Add the first entry

    //If _column is greater than the last entry, we need to calculate the result
    if (_row > m_lnewPageListY.last().startItem() &&
            _row > m_maxCheckedNewPageY) { //this columns hasn't been calculated before
        int startRow = m_lnewPageListY.last().startItem();
        int row = startRow;
        double y = m_pSheet->rowFormat(row)->height();

        //Add repeated row height, when necessary
        if (row > m_printRepeatRows.first) {
            y += m_dPrintRepeatRowsHeight;
            offset = m_dPrintRepeatRowsHeight;
        }

        while ((row <= _row) && (row < m_printRange.bottom())) {
            if (y > m_settings->printHeight()) {
                //We found a new page, so add it to the list
                m_lnewPageListY.append(PrintNewPageEntry(row));

                //Now store into the previous entry the enditem and the width
                QList<PrintNewPageEntry>::iterator it;
                it = findNewPageRow(startRow);
                (*it).setEndItem(row - 1);
                (*it).setSize(y - m_pSheet->rowFormat(row)->height());
                (*it).setOffset(offset);

                //start a new page
                startRow = row;
                if (row == _row) {
                    if (_row > m_maxCheckedNewPageY)
                        m_maxCheckedNewPageY = _row;
                    return;
                } else {
                    y = m_pSheet->rowFormat(row)->height();
                    if (row >= m_printRepeatRows.first) {
                        y += m_dPrintRepeatRowsHeight;
                        offset = m_dPrintRepeatRowsHeight;
                    }
                }
            }

            row++;
            y += m_pSheet->rowFormat(row)->height();
        }
    }

    if (_row > m_maxCheckedNewPageY) {
        m_maxCheckedNewPageY = _row;
        m_lnewPageListY.last().setEndItem(_row);
    }
}


void SheetPrint::updateNewPageListX(int _col)
{
    //If the new range is after the first entry, we need to delete the whole list
    if (m_lnewPageListX.isEmpty() || m_lnewPageListX.first().startItem() != m_printRange.left() || _col == 0) {
        m_lnewPageListX.clear();
        m_maxCheckedNewPageX = m_printRange.left();
        m_lnewPageListX.append(PrintNewPageEntry(m_printRange.left()));
        return;
    }

    if (_col < m_lnewPageListX.last().startItem()) {
        //Find the page entry for this column
        int index = m_lnewPageListX.indexOf(PrintNewPageEntry(_col));
        while ((index == -1) && _col > 0) {
            _col--;
            index = m_lnewPageListX.indexOf(PrintNewPageEntry(_col));
        }

        //Remove later pages
        while (index != m_lnewPageListX.count())
            m_lnewPageListX.removeAt(index);

        //Add default page when list is now empty
        if (m_lnewPageListX.empty())
            m_lnewPageListX.append(PrintNewPageEntry(m_printRange.left()));
    }

    m_maxCheckedNewPageX = _col;
}

void SheetPrint::updateNewPageListY(int _row)
{
    //If the new range is after the first entry, we need to delete the whole list
    if (m_lnewPageListY.isEmpty() || m_lnewPageListY.first().startItem() != m_printRange.top() || _row == 0) {
        m_lnewPageListY.clear();
        m_maxCheckedNewPageY = m_printRange.top();
        m_lnewPageListY.append(PrintNewPageEntry(m_printRange.top()));
        return;
    }

    if (_row < m_lnewPageListY.last().startItem()) {
        //Find the page entry for this row
        int index = m_lnewPageListY.indexOf(PrintNewPageEntry(_row));
        while ((index == -1) && _row > 0) {
            _row--;
            index = m_lnewPageListY.indexOf(PrintNewPageEntry(_row));
        }

        //Remove later pages
        while (index != m_lnewPageListY.count())
            m_lnewPageListY.removeAt(index);

        //Add default page when list is now empty
        if (m_lnewPageListY.empty())
            m_lnewPageListY.append(PrintNewPageEntry(m_printRange.top()));
    }

    m_maxCheckedNewPageY = _row;
}

void SheetPrint::replaceHeadFootLineMacro(QString &_text, const QString &_search, const QString &_replace)
{
    if (_search != _replace)
        _text.replace(QString('<' + _search + '>'), '<' + _replace + '>');
}

QString SheetPrint::localizeHeadFootLine(const QString &_text)
{
    QString tmp = _text;

    /*
      i18n:
      Please use the same words (even upper/lower case) as in
      KoPageLayoutDia.cc function setupTab2(), without the brakets "<" and ">"
    */
    replaceHeadFootLineMacro(tmp, "page",   i18n("page"));
    replaceHeadFootLineMacro(tmp, "pages",  i18n("pages"));
    replaceHeadFootLineMacro(tmp, "file",   i18n("file"));
    replaceHeadFootLineMacro(tmp, "name",   i18n("name"));
    replaceHeadFootLineMacro(tmp, "time",   i18n("time"));
    replaceHeadFootLineMacro(tmp, "date",   i18n("date"));
    replaceHeadFootLineMacro(tmp, "author", i18n("author"));
    replaceHeadFootLineMacro(tmp, "email",  i18n("email"));
    replaceHeadFootLineMacro(tmp, "org",    i18n("org"));
    replaceHeadFootLineMacro(tmp, "sheet",  i18n("sheet"));

    return tmp;
}


QString SheetPrint::delocalizeHeadFootLine(const QString &_text)
{
    QString tmp = _text;

    /*
      i18n:
      Please use the same words (even upper/lower case) as in
      KoPageLayoutDia.cc function setupTab2(), without the brakets "<" and ">"
    */
    replaceHeadFootLineMacro(tmp, i18n("page"),   "page");
    replaceHeadFootLineMacro(tmp, i18n("pages"),  "pages");
    replaceHeadFootLineMacro(tmp, i18n("file"),   "file");
    replaceHeadFootLineMacro(tmp, i18n("name"),   "name");
    replaceHeadFootLineMacro(tmp, i18n("time"),   "time");
    replaceHeadFootLineMacro(tmp, i18n("date"),   "date");
    replaceHeadFootLineMacro(tmp, i18n("author"), "author");
    replaceHeadFootLineMacro(tmp, i18n("email"),  "email");
    replaceHeadFootLineMacro(tmp, i18n("org"),    "org");
    replaceHeadFootLineMacro(tmp, i18n("sheet"),  "sheet");

    return tmp;
}


HeadFoot SheetPrint::headFootLine() const
{
    HeadFoot hf;
    hf.headLeft  = m_headLeft;
    hf.headRight = m_headRight;
    hf.headMid   = m_headMid;
    hf.footLeft  = m_footLeft;
    hf.footRight = m_footRight;
    hf.footMid   = m_footMid;

    return hf;
}


void SheetPrint::setHeadFootLine(const QString &_headl, const QString &_headm, const QString &_headr,
                                 const QString &_footl, const QString &_footm, const QString &_footr)
{
    if (m_pSheet->isProtected())
        NO_MODIFICATION_POSSIBLE;

    m_headLeft  = _headl;
    m_headRight = _headr;
    m_headMid   = _headm;
    m_footLeft  = _footl;
    m_footRight = _footr;
    m_footMid   = _footm;
    if (m_pDoc) m_pDoc->setModified(true);
}

void SheetPrint::setPaperOrientation(KoPageFormat::Orientation _orient)
{
    if (m_pSheet->isProtected())
        NO_MODIFICATION_POSSIBLE;

    m_settings->setPageOrientation(_orient);
    updatePrintRepeatColumnsWidth();
    updatePrintRepeatRowsHeight();
    updateNewPageListX(m_printRange.left());   //Reset the list
    updateNewPageListY(m_printRange.top());   //Reset the list

    if (m_pSheet->isShowPageBorders())
        emit sig_updateView(m_pSheet);
}


const KoPageLayout &SheetPrint::paperLayout() const
{
    return m_settings->pageLayout();
}

QList<PrintNewPageEntry>::iterator SheetPrint::findNewPageColumn(int col)
{
    QList<PrintNewPageEntry>::iterator it;
    for (it = m_lnewPageListX.begin(); it != m_lnewPageListX.end(); ++it) {
        if ((*it).startItem() == col)
            return it;
    }
    return it;
//                QValueList<PrintNewPageEntry>::iterator it;
//                it = m_lnewPageListX.indexOf( startCol );
}

QList<PrintNewPageEntry>::iterator SheetPrint::findNewPageRow(int row)
{
    QList<PrintNewPageEntry>::iterator it;
    for (it = m_lnewPageListY.begin(); it != m_lnewPageListY.end(); ++it) {
        if ((*it).startItem() == row)
            return it;
    }
    return it;
}


QString SheetPrint::paperFormatString() const
{
    if (paperLayout().format == KoPageFormat::CustomSize) {
        QString tmp;
        tmp.sprintf("%fx%f", paperLayout().width, paperLayout().height);
        return tmp;
    }

    return KoPageFormat::formatString(paperLayout().format);
}

const char* SheetPrint::orientationString() const
{
    switch (paperLayout().orientation) {
    case QPrinter::Portrait:
        return "Portrait";
    case QPrinter::Landscape:
        return "Landscape";
    }

    kWarning(36001) << "SheetPrint: Unknown orientation, using now portrait";
    return 0;
}

QString SheetPrint::completeHeading(const QString &_data, int _page, const QString &_sheet) const
{
    QString page(QString::number(_page));
    QString pages(QString::number(m_uprintPages));

    QString pathFileName(m_pDoc->url().path());
    if (pathFileName.isNull())
        pathFileName = "";

    QString fileName(m_pDoc->url().fileName());
    if (fileName.isNull())
        fileName = "";

    QString t(QTime::currentTime().toString());
    QString d(QDate::currentDate().toString());
    QString ta;
    if (!_sheet.isEmpty())
        ta = _sheet;

    KoDocumentInfo* info = m_pDoc->documentInfo();
    QString full_name;
    QString email_addr;
    QString organization;
    QString tmp;
    if (!info)
        kWarning() << "Author information not found in Document Info !";
    else {
        full_name = info->authorInfo("creator");
        email_addr = info->authorInfo("email");
        organization = info->authorInfo("company");
    }

    char hostname[80];
    struct passwd *p;

    p = getpwuid(getuid());
    gethostname(hostname, sizeof(hostname));

    if (full_name.isEmpty())
        full_name = p->pw_gecos;

    if (email_addr.isEmpty())
        email_addr = QString("%1@%2").arg(p->pw_name).arg(hostname);

    tmp = _data;
    int pos = 0;
    while ((pos = tmp.indexOf("<page>", pos)) != -1)
        tmp.replace(pos, 6, page);
    pos = 0;
    while ((pos = tmp.indexOf("<pages>", pos)) != -1)
        tmp.replace(pos, 7, pages);
    pos = 0;
    while ((pos = tmp.indexOf("<file>", pos)) != -1)
        tmp.replace(pos, 6, pathFileName);
    pos = 0;
    while ((pos = tmp.indexOf("<name>", pos)) != -1)
        tmp.replace(pos, 6, fileName);
    pos = 0;
    while ((pos = tmp.indexOf("<time>", pos)) != -1)
        tmp.replace(pos, 6, t);
    pos = 0;
    while ((pos = tmp.indexOf("<date>", pos)) != -1)
        tmp.replace(pos, 6, d);
    pos = 0;
    while ((pos = tmp.indexOf("<author>", pos)) != -1)
        tmp.replace(pos, 8, full_name);
    pos = 0;
    while ((pos = tmp.indexOf("<email>", pos)) != -1)
        tmp.replace(pos, 7, email_addr);
    pos = 0;
    while ((pos = tmp.indexOf("<org>", pos)) != -1)
        tmp.replace(pos, 5, organization);
    pos = 0;
    while ((pos = tmp.indexOf("<sheet>", pos)) != -1)
        tmp.replace(pos, 7, ta);

    return tmp;
}

void SheetPrint::setPrintRange(const QRect &_printRange)
{
    if (m_pSheet->isProtected())
        NO_MODIFICATION_POSSIBLE;


    if (m_printRange == _printRange)
        return;

    int oldLeft = m_printRange.left();
    int oldTop = m_printRange.top();
    m_printRange = _printRange;

    //Refresh calculation of stored page breaks, the lower one of old and new
    if (oldLeft != _printRange.left())
        updateNewPageListX(qMin(oldLeft, _printRange.left()));
    if (oldTop != _printRange.top())
        updateNewPageListY(qMin(oldTop, _printRange.top()));

    if (m_pDoc) m_pDoc->setModified(true);

    emit sig_updateView(m_pSheet);

}

void SheetPrint::setPageLimitX(int pages)
{
    //We do want an update in any case because the sheet content
    //could have changed, thus we need to recalculate although
    //it's the same setting!
//     if( m_iPageLimitX == pages )
//         return;

    m_iPageLimitX = pages;

    if (pages == 0)
        return;

    calculateZoomForPageLimitX();
}

void SheetPrint::setPageLimitY(int pages)
{
    //We do want an update in any case because the sheet content
    //could have changed, thus we need to recalculate although
    //it's the same setting!
//     if( m_iPageLimitY == pages )
//         return;

    m_iPageLimitY = pages;

    if (pages == 0)
        return;

    calculateZoomForPageLimitY();
}

void SheetPrint::calculateZoomForPageLimitX()
{
    kDebug() << "Calculating zoom for X limit";
    if (m_iPageLimitX == 0)
        return;

    double origZoom = (0.01 * m_zoomHandler->zoomInPercent());

    if ((0.01*m_zoomHandler->zoomInPercent()) < 1.0)
        m_zoomHandler->setZoom(1.0);

    QRect printRange = cellsPrintRange();
    updateNewPageX(printRange.right());
    int currentPages = pagesX(printRange);

    if (currentPages <= m_iPageLimitX)
        return;

    //calculating a factor for scaling the zoom down makes it lots faster
    double factor = (double)m_iPageLimitX / (double)currentPages +
                    1 - (double)currentPages / ((double)currentPages + 1); //add possible error;
    kDebug() << "Calculated factor for scaling (0.01*m_zoomHandler->zoomInPercent()):" << factor;
    m_zoomHandler->setZoom((0.01*m_zoomHandler->zoomInPercent())*factor);

    kDebug() << "New exact zoom:" << (0.01*m_zoomHandler->zoomInPercent());

    if ((0.01*m_zoomHandler->zoomInPercent()) < 0.01)
        m_zoomHandler->setZoom(0.01);
    if ((0.01*m_zoomHandler->zoomInPercent()) > 1.0)
        m_zoomHandler->setZoom(1.0);

    m_zoomHandler->setZoom((((int)((0.01*m_zoomHandler->zoomInPercent())*100 + 0.5)) / 100.0));

    kDebug() << "New rounded zoom:" << (0.01*m_zoomHandler->zoomInPercent());

    updatePrintRepeatColumnsWidth();
    updateNewPageListX(0);
    updateNewPageX(printRange.right());
    currentPages = pagesX(printRange);

    kDebug() << "Number of pages with this zoom:" << currentPages;

    while ((currentPages > m_iPageLimitX) && ((0.01*m_zoomHandler->zoomInPercent()) > 0.01)) {
        m_zoomHandler->setZoom((0.01*m_zoomHandler->zoomInPercent()) - 0.01);
        updatePrintRepeatColumnsWidth();
        updateNewPageListX(0);
        updateNewPageX(printRange.right());
        currentPages = pagesX(printRange);
        kDebug() << "Looping -0.01; current zoom:" << (0.01*m_zoomHandler->zoomInPercent());
    }

    if ((0.01*m_zoomHandler->zoomInPercent()) < origZoom) {
        double newZoom = (0.01 * m_zoomHandler->zoomInPercent());
        m_zoomHandler->setZoom((0.01*m_zoomHandler->zoomInPercent()) + 1.0);   //set it to something different
        setZoom(newZoom, false);
    } else
        m_zoomHandler->setZoom(origZoom);
}

void SheetPrint::calculateZoomForPageLimitY()
{
    kDebug() << "Calculating zoom for Y limit";
    if (m_iPageLimitY == 0)
        return;

    double origZoom = (0.01 * m_zoomHandler->zoomInPercent());

    if ((0.01*m_zoomHandler->zoomInPercent()) < 1.0)
        m_zoomHandler->setZoom(1.0);

    QRect printRange = cellsPrintRange();
    updateNewPageY(printRange.bottom());
    int currentPages = pagesY(printRange);

    if (currentPages <= m_iPageLimitY)
        return;

    double factor = (double)m_iPageLimitY / (double)currentPages +
                    1 - (double)currentPages / ((double)currentPages + 1); //add possible error
    kDebug() << "Calculated factor for scaling (0.01*m_zoomHandler->zoomInPercent()):" << factor;
    m_zoomHandler->setZoom((0.01*m_zoomHandler->zoomInPercent())*factor);

    kDebug() << "New exact zoom:" << (0.01*m_zoomHandler->zoomInPercent());

    if ((0.01*m_zoomHandler->zoomInPercent()) < 0.01)
        m_zoomHandler->setZoom(0.01);
    if ((0.01*m_zoomHandler->zoomInPercent()) > 1.0)
        m_zoomHandler->setZoom(1.0);

    m_zoomHandler->setZoom((((int)((0.01*m_zoomHandler->zoomInPercent())*100 + 0.5)) / 100.0));

    kDebug() << "New rounded zoom:" << (0.01*m_zoomHandler->zoomInPercent());

    updatePrintRepeatRowsHeight();
    updateNewPageListY(0);
    updateNewPageY(printRange.bottom());
    currentPages = pagesY(printRange);

    kDebug() << "Number of pages with this zoom:" << currentPages;

    while ((currentPages > m_iPageLimitY) && ((0.01*m_zoomHandler->zoomInPercent()) > 0.01)) {
        m_zoomHandler->setZoom((0.01*m_zoomHandler->zoomInPercent()) - 0.01);
        updatePrintRepeatRowsHeight();
        updateNewPageListY(0);
        updateNewPageY(printRange.bottom());
        currentPages = pagesY(printRange);
        kDebug() << "Looping -0.01; current zoom:" << (0.01*m_zoomHandler->zoomInPercent());
    }

    if ((0.01*m_zoomHandler->zoomInPercent()) < origZoom) {
        double newZoom = (0.01 * m_zoomHandler->zoomInPercent());
        m_zoomHandler->setZoom((0.01*m_zoomHandler->zoomInPercent()) + 1.0);   //set it to something different
        setZoom(newZoom, false);
    } else
        m_zoomHandler->setZoom(origZoom);
}

void SheetPrint::updatePrintRepeatColumnsWidth()
{
    m_dPrintRepeatColumnsWidth = 0.0;
    if (m_printRepeatColumns.first != 0) {
        for (int i = m_printRepeatColumns.first; i <= m_printRepeatColumns.second; i++) {
            m_dPrintRepeatColumnsWidth += m_pSheet->columnFormat(i)->width();
        }
    }
}

void SheetPrint::updatePrintRepeatRowsHeight()
{
    m_dPrintRepeatRowsHeight = 0.0;
    if (m_printRepeatRows.first != 0) {
        for (int i = m_printRepeatRows.first; i <= m_printRepeatRows.second; i++) {
            m_dPrintRepeatRowsHeight += m_pSheet->rowFormat(i)->height();
        }
    }
}


void SheetPrint::setPrintRepeatColumns(QPair<int, int> _printRepeatColumns)
{
    //Bring arguments in order
    if (_printRepeatColumns.first > _printRepeatColumns.second) {
        int tmp = _printRepeatColumns.first;
        _printRepeatColumns.first = _printRepeatColumns.second;
        _printRepeatColumns.second = tmp;
    }

    //If old are equal to the new setting, nothing is to be done at all
    if (m_printRepeatColumns == _printRepeatColumns)
        return;

    int oldFirst = m_printRepeatColumns.first;
    m_printRepeatColumns = _printRepeatColumns;

    //Recalcualte the space needed for the repeated columns
    updatePrintRepeatColumnsWidth();

    //Refresh calculation of stored page breaks, the lower one of old and new
    updateNewPageListX(qMin(oldFirst, _printRepeatColumns.first));

    //Refresh view, if page borders are shown
    if (m_pSheet->isShowPageBorders())
        emit sig_updateView(m_pSheet);

    if (m_pDoc) m_pDoc->setModified(true);
}

void SheetPrint::setPrintRepeatRows(QPair<int, int> _printRepeatRows)
{
    //Bring arguments in order
    if (_printRepeatRows.first > _printRepeatRows.second) {
        int tmp = _printRepeatRows.first;
        _printRepeatRows.first = _printRepeatRows.second;
        _printRepeatRows.second = tmp;
    }

    //If old are equal to the new setting, nothing is to be done at all
    if (m_printRepeatRows == _printRepeatRows)
        return;

    int oldFirst = m_printRepeatRows.first;
    m_printRepeatRows = _printRepeatRows;

    //Recalcualte the space needed for the repeated rows
    updatePrintRepeatRowsHeight();

    //Refresh calculation of stored page breaks, the lower one of old and new
    updateNewPageListY(qMin(oldFirst, _printRepeatRows.first));

    //Refresh view, if page borders are shown
    if (m_pSheet->isShowPageBorders())
        emit sig_updateView(m_pSheet);

    if (m_pDoc) m_pDoc->setModified(true);
}

void SheetPrint::insertColumn(int col, int nbCol)
{
    //update print range, when it has been defined
    if (m_printRange != QRect(QPoint(1, 1), QPoint(KS_colMax, KS_rowMax))) {
        int left = m_printRange.left();
        int right = m_printRange.right();

        for (int i = 0; i < nbCol; i++) {
            if (left >= col) left++;
            if (right >= col) right++;
        }
        //Validity checks
        if (left > KS_colMax) left = KS_colMax;
        if (right > KS_colMax) right = KS_colMax;
        setPrintRange(QRect(QPoint(left, m_printRange.top()),
                            QPoint(right, m_printRange.bottom())));
    }
}

void SheetPrint::insertRow(int row, int nbRow)
{
    //update print range, when it has been defined
    if (m_printRange != QRect(QPoint(1, 1), QPoint(KS_colMax, KS_rowMax))) {
        int top = m_printRange.top();
        int bottom = m_printRange.bottom();

        for (int i = 0; i < nbRow; i++) {
            if (top >= row) top++;
            if (bottom >= row) bottom++;
        }
        //Validity checks
        if (top > KS_rowMax) top = KS_rowMax;
        if (bottom > KS_rowMax) bottom = KS_rowMax;
        setPrintRange(QRect(QPoint(m_printRange.left(), top),
                            QPoint(m_printRange.right(), bottom)));
    }
}

void SheetPrint::removeColumn(int col, int nbCol)
{
    //update print range, when it has been defined
    if (m_printRange != QRect(QPoint(1, 1), QPoint(KS_colMax, KS_rowMax))) {
        int left = m_printRange.left();
        int right = m_printRange.right();

        for (int i = 0; i < nbCol; i++) {
            if (left > col) left--;
            if (right >= col) right--;
        }
        //Validity checks
        if (left < 1) left = 1;
        if (right < 1) right = 1;
        setPrintRange(QRect(QPoint(left, m_printRange.top()),
                            QPoint(right, m_printRange.bottom())));
    }

    //update repeat columns, when it has been defined
    if (m_printRepeatColumns.first != 0) {
        int left = m_printRepeatColumns.first;
        int right = m_printRepeatColumns.second;

        for (int i = 0; i < nbCol; i++) {
            if (left > col) left--;
            if (right >= col) right--;
        }
        //Validity checks
        if (left < 1) left = 1;
        if (right < 1) right = 1;
        setPrintRepeatColumns(qMakePair(left, right));
    }
}

void SheetPrint::removeRow(int row, int nbRow)
{
    //update print range, when it has been defined
    if (m_printRange != QRect(QPoint(1, 1), QPoint(KS_colMax, KS_rowMax))) {
        int top = m_printRange.top();
        int bottom = m_printRange.bottom();

        for (int i = 0; i < nbRow; i++) {
            if (top > row) top--;
            if (bottom >= row) bottom--;
        }
        //Validity checks
        if (top < 1) top = 1;
        if (bottom < 1) bottom = 1;
        setPrintRange(QRect(QPoint(m_printRange.left(), top),
                            QPoint(m_printRange.right(), bottom)));
    }

    //update repeat rows, when it has been defined
    if (m_printRepeatRows.first != 0) {
        int top = m_printRepeatRows.first;
        int bottom = m_printRepeatRows.second;

        for (int i = 0; i < nbRow; i++) {
            if (top > row) top--;
            if (bottom >= row) bottom--;
        }
        //Validity checks
        if (top < 1) top = 1;
        if (bottom < 1) bottom = 1;
        setPrintRepeatRows(qMakePair(top, bottom));
    }
}

void SheetPrint::setZoom(double _zoom, bool checkPageLimit)
{
    if ((0.01*m_zoomHandler->zoomInPercent()) == _zoom) {
        return;
    }

    m_zoomHandler->setZoom(_zoom);
    updatePrintRepeatColumnsWidth();
    updatePrintRepeatRowsHeight();
    updateNewPageListX(0);
    updateNewPageListY(0);
    if (m_pSheet->isShowPageBorders())
        emit sig_updateView(m_pSheet);

    if (checkPageLimit) {
        calculateZoomForPageLimitX();
        calculateZoomForPageLimitY();
    }

    if (m_pDoc) m_pDoc->setModified(true);
}

double SheetPrint::zoom() const
{
    return m_zoomHandler->zoomInPercent()*0.01;
}

bool PrintNewPageEntry::operator==(PrintNewPageEntry const & entry) const
{
    return m_iStartItem == entry.m_iStartItem;
}

