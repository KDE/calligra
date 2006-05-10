/*
*
* Copyright (C) 2005  Fredrik Edemar
*                     f_edemar@linux.se
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*
*/

#ifndef kpresenterprintdlg_included
#define kpresenterprintdlg_included

#include <kdeprint/kprintdialogpage.h>
#include <knuminput.h>

#include <QCheckBox>
#include <QLabel>

class KPrPrinterDlg : public KPrintDialogPage
{
  Q_OBJECT
  public:
    /// Getting the common arguments.
    KPrPrinterDlg( QWidget *parent = 0, const char *name = 0 );

    /// Reimplemented.
    void getOptions( QMap<QString, QString>& opts, bool include_def = false );
    /// Reimplemented
    void setOptions( const QMap<QString, QString>& opts );
    /// Reimplemented.
    bool isValid( const QString& msg );

  private slots:
    void txtRows_valueChanged( int );
    void txtColumns_valueChanged( int );

  private:
    KIntNumInput *txtRows;
    KIntNumInput *txtColumns;
    QCheckBox *drawBorder;
};

#endif //kpresenterprinterdlg_included
