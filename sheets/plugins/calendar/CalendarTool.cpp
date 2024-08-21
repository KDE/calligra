/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "CalendarTool.h"

#include "CalendarToolWidget.h"

#include "core/CellStorage.h"
#include "core/Sheet.h"
#include "engine/CalculationSettings.h"
#include "engine/Localization.h"
#include "engine/MapBase.h"
#include "engine/Value.h"
#include "ui/Selection.h"

#include <KoCanvasBase.h>
#include <kundo2command.h>

#include <KMessageBox>

using namespace Calligra::Sheets;

class CalendarTool::Private
{
public:
};

CalendarTool::CalendarTool(KoCanvasBase *canvas)
    : CellTool(canvas)
    , d(new Private)
{
    setObjectName(QLatin1String("CalendarTool"));
    /*
        QAction* importAction = new QAction(koIcon("document-import"), i18n("Import OpenDocument Spreadsheet File"), this);
        importAction->setIconText(i18n("Import"));
        addAction("import", importAction);
        connect(importAction, SIGNAL(triggered()), this, SLOT(importDocument()));*/
}

CalendarTool::~CalendarTool()
{
    delete d;
}

void CalendarTool::activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes)
{
    CellTool::activate(toolActivation, shapes);
}

void CalendarTool::deactivate()
{
    CellTool::deactivate();
}

void CalendarTool::insertCalendar(const QDate &start, const QDate &end)
{
    if (end < start) {
        KMessageBox::error(nullptr, i18n("End date is before start date! Please make sure that end date comes after start date."), i18n("Error"));
        return;
    }

    if (start.daysTo(end) > 3652) {
        KMessageBox::error(nullptr,
                           i18n("Calendars should not be longer than 10 years. If you really need such long periods you need to split them up."),
                           i18n("Error"));
        return;
    }

    if (start == end) {
        if (KMessageBox::SecondaryAction
            == KMessageBox::warningTwoActions(nullptr,
                                              i18n("Start and end dates are equal! Only one day will be inserted, do you want to continue?"),
                                              i18n("Warning"),
                                              KStandardGuiItem::cont(),
                                              KStandardGuiItem::cancel()))
            return;
    }

    if (start.daysTo(end) > 366) {
        if (KMessageBox::SecondaryAction
            == KMessageBox::warningTwoActions(nullptr,
                                              i18n("Creating a calendar for a longer period than a year can take up a lot of space, do you want to continue?"),
                                              i18n("Warning"),
                                              KStandardGuiItem::cont(),
                                              KStandardGuiItem::cancel()))
            return;
    }

    const QPoint marker = this->selection()->lastRange().topLeft();
    Sheet *const sheet = this->selection()->activeSheet();
    Localization *locale = sheet->map()->calculationSettings()->locale();

    // now let's check if the area is really empty...
    // we use two columns per day and one column for the week number
    int sizeX = 15;
    // we use two rows per week, some additional space between months...
    // so that should be ok, but can be improved of course
    //@todo improve calendar size prediction!
    int sizeY = 4 + (int)(0.5 * (float)(start.daysTo(end)));

    if (!sheet->areaIsEmpty(Region(QRect(marker, QSize(sizeX, sizeY))))) {
        if (KMessageBox::SecondaryAction
            == KMessageBox::warningTwoActions(
                nullptr,
                i18n("The area where the calendar is inserted is NOT empty, are you sure you want to continue, overwriting existing data? If you choose No the "
                     "area that would be required for the desired calendar will be selected so you can see what data would be overwritten."),
                i18n("Warning"),
                KStandardGuiItem::cont(),
                KStandardGuiItem::cancel())) {
            // select the area so the user knows what's in the way
            selection()->initialize(QRect(marker.x(), marker.y(), sizeX, sizeY)); //,sheet);
            return;
        }
    }

    KUndo2Command *cmd = new KUndo2Command(kundo2_i18n("Insert Calendar"));
    sheet->fullCellStorage()->startUndoRecording();

    int row = marker.y();
    int col = marker.x();
    int colstart = col; // this is where we get back after each week
    setText(sheet, row, colstart, i18n("Calendar from %1 to %2", locale->formatDate(start), locale->formatDate(end)));

    QDate current(start);
    //   QDate previous(current);
    bool yearheader = true;
    bool monthheader = true;
    bool weekheader = true;

    // this loop creates the actual calendar
    //@todo formatting of cells - each day occupies QRect(row,col,row,col+1)
    while (current <= end) {
        // let's see if any header is required
        if (current.dayOfWeek() == 1) {
            col = colstart;
            row++;
            weekheader = true;
        }
        if (current.day() == 1) {
            row += 2;
            col = colstart + (current.dayOfWeek() - 1) * 2;
            monthheader = true;
            weekheader = true;
            if (current.month() == 1) {
                row++;
                yearheader = true;
            }
        }

        if (yearheader) {
            debugSheets << "inserting year" + QString::number(current.year());
            setText(sheet, row, colstart + 6, locale->formatDate(current, true));

            row += 2;
            yearheader = false;
        }
        if (monthheader) {
            debugSheets << "inserting month" + QString::number(current.month());
            setText(sheet, row, colstart + 6, locale->monthName(current.month()));
            row += 2;
            // we always have the week number in the first column
            setText(sheet, row, colstart, i18n("week"));
            for (int i = 1; i < 8; i++) {
                setText(sheet, row, colstart + (i - 1) * 2 + 1, locale->dayName(i));
            }
            row++;
            monthheader = false;
        }
        if (weekheader) {
            setText(sheet, row, colstart, QString::number(current.weekNumber()));
            col++;
            weekheader = false;

            // if we are at the beginning of the month we might need an offset
            if (current.day() == 1) {
                col = colstart + (current.dayOfWeek() - 1) * 2 + 1;
            }
        }

        setText(sheet, row, col, QString::number(current.day()));
        // go to the next date
        //@todo isn't there a better way, like current++ or something??
        QDate next = current.addDays(1);
        current.setDate(next.year(), next.month(), next.day());
        col += 2;
    }

    sheet->fullCellStorage()->stopUndoRecording(cmd);
    selection()->canvas()->addCommand(cmd);

    debugSheets << "inserting calendar completed";
}

QList<QPointer<QWidget>> CalendarTool::createOptionWidgets()
{
    // Create the main cell tool widget. It is not visible, but the CellTool makes heavy use
    // of it, and it refuses to work correctly if it does not exist
    CellTool::createOptionWidgets();

    CalendarToolWidget *widget = new CalendarToolWidget(canvas()->canvasWidget());
    connect(widget, &CalendarToolWidget::insertCalendar, this, &CalendarTool::insertCalendar);
    QList<QPointer<QWidget>> ow;
    ow.append(widget);
    return ow;
}

void CalendarTool::setText(Sheet *sheet, int _row, int _column, const QString &_text, bool asString)
{
    Cell cell(sheet, _column, _row);
    if (asString) {
        cell.setCellValue(Value(_text));
    } else {
        cell.parseUserInput(_text);
    }
}
