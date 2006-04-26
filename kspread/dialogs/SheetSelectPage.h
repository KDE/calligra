/* This file is part of the KDE project
   Copyright (C) 2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_SHEETSELECTPAGE
#define KSPREAD_SHEETSELECTPAGE

#include <kdeprint/kprintdialogpage.h>

namespace KSpread
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
class SheetSelectPage : public KPrintDialogPage
{
  Q_OBJECT
  public:
    SheetSelectPage( QWidget *parent = 0 );
//     ~SheetSelectPage();

//     //reimplement virtual functions
    /**
     * @see printOptionPrefix()
     */
    void getOptions( QMap<QString,QString>& opts, bool incldef = false );

    /**
     * @see printOptionPrefix()
     */
    void setOptions( const QMap<QString,QString>& opts );

    /**
     * @return false if no sheet is selected for printing.
     */
    bool isValid( QString& msg );

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

    /**
     * @param prt the printer from which the options should be read.
     * @return list of sheets to print in correct order configured for given printer.
     */
    static QStringList selectedSheets(KPrinter &prt);

  public slots:

    /**
     * Inserts given sheet to the list of available sheets.
     */
    void prependAvailableSheet(const QString& sheetname);

    /**
     * Inserts given sheet to the list of sheets for printing at the top.
     */
    void prependSelectedSheet(const QString& sheetname);

  protected slots:

    // The following slots just implement the code for the buttons

    void selectAll();
    void select();
    void remove();
    void removeAll();

    void moveTop();
    void moveUp();
    void moveDown();
    void moveBottom();

  private:

    /**
     * The widget used, includes two lists of sheet names and
     * buttons to move sheets between and within the lists.
     */
    SheetSelectWidget* gui;
};

} // namespace KSpread

#endif // KSPREAD_SHEETSELECTPAGE
