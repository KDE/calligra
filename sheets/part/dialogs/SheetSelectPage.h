/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_SHEETSELECTPAGE
#define CALLIGRA_SHEETS_SHEETSELECTPAGE

#include "ui_SheetSelectWidget.h"

namespace Calligra
{
namespace Sheets
{
class SheetSelectWidget;

/**
 * \class SheetSelectPage
 * \brief Print dialog page for selecting sheets to print.
 * @author raphael.langerhorst@kdemail.net
 * @see SheetSelectWidget
 *
 * This dialog is shown in the print dialog and allows the user
 * to select the sheets that should be printed and in which order
 * they should be printed.
 */
class SheetSelectPage : public QWidget
{
    Q_OBJECT
public:
    explicit SheetSelectPage(QWidget *parent = nullptr);
    //     ~SheetSelectPage();

    /**
     * @see printOptionPrefix()
     */
    void setAvailableSheets(const QStringList &sheetlist);

    /**
     * @return false if no sheet is selected for printing.
     */
    bool isValid(QString &msg);

    /**
     * @return list of sheets that will be printed, in correct order.
     */
    QStringList selectedSheets();

    /**
     * Removes all sheets from the list of selected sheets.
     */
    void clearSelection();

    /**
     * The print order of the sheets is stored in the option map,
     * using a prefix plus the index of the sheet, like the following:
     * \li sheetprintorder0
     * \li sheetprintorder1
     * \li sheetprintorder2
     * Please note that this is just the key to the value, not the value
     * itself. The value of the option is the sheetname itself.
     * @param index the index of the print order, starting at 0
     * @return the string that is used in the printoption for given index
     */
    static QString printOptionForIndex(unsigned int index);

public Q_SLOTS:

    /**
     * Inserts given sheet to the list of available sheets.
     */
    void prependAvailableSheet(const QString &sheetname);

    /**
     * Inserts given sheet to the list of sheets for printing at the top.
     */
    void prependSelectedSheet(const QString &sheetname);

protected Q_SLOTS:

    // The following slots just implement the code for the buttons

    void selectAll();
    void select();
    void remove();
    void removeAll();

    void moveTop();
    void moveUp();
    void moveDown();
    void moveBottom();

public:
    ::Ui::SheetSelectWidget m_ui;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_SHEETSELECTPAGE
