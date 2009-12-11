/* This file is part of the KDE project

   Copyright (C) 2006 Fredrik Edemar <f_edemar@linux.se>
   Copyright 2002-2003 Joseph Wenninger <jowenn@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 2001-2002 Laurent Montel <montel@kde.org>
   Copyright 2000 Simon Hausmann <hausmann@kde.org>
   Copyright 1999 Torben Weis <weis@kde.org>

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

#ifndef KSPREAD_VIEW_ADAPTOR
#define KSPREAD_VIEW_ADAPTOR

#include <QColor>
#include <QRect>
#include <QString>
#include <QtDBus/QtDBus>
#include "kspread_export.h"
namespace KSpread
{
class View;
class CellProxy;

/**
* The ViewAdaptor class provides access to a view on a KSpread document.
*/
class KSPREAD_EXPORT ViewAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.kde.koffice.spreadsheet.view")
public:
    explicit ViewAdaptor(View*);
    ~ViewAdaptor();

public Q_SLOTS: // METHODS

    /** Return the name of the document the view looks on. */
    virtual QString doc() const;
    /** Return the name of the map for the document. An map offers
    additional functionality to deal with the content of a document. */
    virtual QString map() const;
    /** Return the name of the active sheet. */
    virtual QString sheet() const;

    virtual void changeNbOfRecentFiles(int _nb);

    /** Hide the view. */
    virtual void hide();
    /** Show the view. */
    virtual void show();

    /** Set the range of cells that should be selected. */
    virtual void setSelection(const QRect& selection);
    /** Return the range of cells that is selected. */
    virtual QRect selection();

#if 0 // -> cell tool
    /** Display the find dialog to find something in the document. */
    virtual void find();
    /** Display the replace dialog to replace something in the document. */
    virtual void replace();
    /** Display the Conditional dialog that allows to set cell style based
    on certain conditions. */
    virtual void conditional();
    /** Display the Validity dialog that enables to set tests to confirm cell
    data is valid. */
    virtual void validity();
    /** Display the "Series" dialog that allows to insert series into cells. */
    virtual void insertSeries();
    /** Display the "Hyperlink" dialog that provides functionality to insert
    hyperlinks like URLs or E-Mails into a cell. */
    virtual void insertHyperlink();
    /** Display the "Go to..." dialog that enables to go to a defined cell. */
    virtual void gotoCell();
    /** Display the "Change Angle" dialog to offer to change the angle of a cell. */
    virtual void changeAngle();
#endif
    virtual void preference();
    /** Select the next sheet as active sheet. */
    virtual void nextSheet();
    /** Select the previous sheet as active sheet. */
    virtual void previousSheet();
    /** Select the sheet with name \p sheetName as active sheet. */
    virtual bool showSheet(const QString& sheetName);
#if 0 // -> cell tool
    /** Display the dialog to create a custom lists for sorting or autofill. */
    virtual void sortList();
    /** Set a name for a region of the spreadsheet. */
    virtual void setAreaName();
    /** Show the dialog that allows to edit or select named areas. */
    virtual void showAreaName();
    /** Merge the selected region. */
    virtual void mergeCell();
    /** Unmerge the selected region. */
    virtual void dissociateCell();
    /** Display the "Consolidate..." dialog. */
    virtual void consolidate();

    /** Delete a column from the sheet. */
    virtual void deleteColumn();
    /** Insert a new column into the sheet. */
    virtual void insertColumn();
    /** Delete a row from the sheet. */
    virtual void deleteRow();
    /** Insert a new row into the sheet. */
    virtual void insertRow();
    /** Hide a row in the sheet. */
    virtual void hideRow();
    /** Show a row in the sheet. */
    virtual void showRow();
    /** Hide a column in the sheet. */
    virtual void hideColumn();
    /** Show a column in the sheet. */
    virtual void showColumn();
    /** Convert all letters to upper case. */
    virtual void upper();
    /** Convert all letters to lower case. */
    virtual void lower();

    /** Equalize Column. */
    virtual void equalizeColumn();
    /** Equalize Row. */
    virtual void equalizeRow();

    /** Remove the contents of the current cell. */
    virtual void clearTextSelection();
    /** Remove comment from the selected cells. */
    virtual void clearCommentSelection();
    /** Remove the validity tests on this cell. */
    virtual void clearValiditySelection();
    /** Remove the conditional cell styles. */
    virtual void clearConditionalSelection();
    /** Displays the "Goal seek" dialog. */
    virtual void goalSeek();
    /** Calls the Insert/Database dialog. */
    virtual void insertFromDatabase();
    /** Calls the Insert/Textfile dialog. */
    virtual void insertFromTextfile();
    /** Calls the Insert/Clipboard dialog. */
    virtual void insertFromClipboard();
    /** Display the "Text to Columns..." dialog. */
    virtual void textToColumns();
#endif
    /** Copy the content of the range of cells that is selected. */
    virtual void copyAsText();

#if 0 // -> cell tool
    /** Set the cell formatting to look like your local currency. */
    virtual void setSelectionMoneyFormat(bool b);
    /** Set the cell formatting to look like a percentage. */
    virtual void setSelectionPercent(bool b);
    /** Make the cell text wrap onto multiple lines. */
    virtual void setSelectionMultiRow(bool enable);
    /** Set the font size of the selected cells to the defined size. */
    virtual void setSelectionSize(int size);
    /** Convert all letters to upper case. */
    virtual void setSelectionUpper();
    /** Convert all letters to lower case. */
    virtual void setSelectionLower();
    /** Convert First Letter to Upper Case. */
    virtual void setSelectionFirstLetterUpper();
    /** Print cell contents vertically. */
    virtual void setSelectionVerticalText(bool enable);
#endif
    /** Set the comment of all selected cells to the defined comment string. */
    virtual void setSelectionComment(const QString& comment);
#if 0 // -> cell tool
    /** Set the angle of all selected cells to the defined angle value. */
    virtual void setSelectionAngle(int value);
#endif
    /** Set the text color of all selected cells to the defined color value. */
    virtual void setSelectionTextColor(const QColor& txtColor);
    /** Set the background color of all selected cells to the defined color value. */
    virtual void setSelectionBgColor(const QColor& bgColor);
    /** Set the border color of all selected cells to the defined color value. */
    virtual void setSelectionBorderColor(const QColor& bdColor);

#if 0 // -> cell tool
    /** Delete content of the range of cells that is selected. */
    virtual void deleteSelection();
    /** Copy the content of the range of cells that is selected. */
    virtual void copySelection();
    /** Cut the content of the range of cells that is selected. */
    virtual void cutSelection();
#endif

    /** Set the color of the left border. */
    virtual void setLeftBorderColor(const QColor& color);
    /** Set the color of the top border. */
    virtual void setTopBorderColor(const QColor& color);
    /** Set the color of the right border. */
    virtual void setRightBorderColor(const QColor& color);
    /** Set the color of the bottom border. */
    virtual void setBottomBorderColor(const QColor& color);
    /** Set the color of the all borders. */
    virtual void setAllBorderColor(const QColor& color);
    /** Set the color of the outline border. */
    virtual void setOutlineBorderColor(const QColor& color);
#if 0 // -> cell tool
    /** Remove border. */
    virtual void removeBorder();

    /** Increase the indention. */
    virtual void increaseIndent();
    /** Decrease the indention. */
    virtual void decreaseIndent();
    /** Increase the precision. */
    virtual void increasePrecision();
    /** Decrease the precision. */
    virtual void decreasePrecision();

    /** Display the "Subtotals..." dialog. */
    void subtotals();
    /** Sort a group of cells in ascending (first to last) order. */
    void sortInc();
    /** Sort a group of cells in decreasing (last to first) order. */
    void sortDec();
    /** Display the "Layout..." dialog. */
    void layoutDlg();
    /** Increase the size of the font. */
    void increaseFontSize();
    /** Decrease the size of the font. */
    void decreaseFontSize();
#endif

Q_SIGNALS:

    /** This signal got emitted if this view got activated or deactivated. */
    void activated(bool active);

    /** This signal got emitted if this view got selected or unselected. */
    void selected(bool select);

    /** This signal got emitted if the selection changed. */
    void selectionChanged();

private:
    View* m_view;
    CellProxy* m_proxy;
};

} // namespace KSpread

#endif // KSPREAD_VIEW_ADAPTOR
