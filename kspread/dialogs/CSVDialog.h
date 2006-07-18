/* This file is part of the KDE project
   Copyright 2006      Stefan Nikolaus <stefan.nikolaus@kdemail.net>
             2002-2003 Norbert Andres <nandres@web.de>
             2002-2003 Ariya Hidayat <ariya@kde.org>
             2002      Laurent Montel <montel@kde.org>
             1999      David Faure <faure@kde.org>

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

#ifndef CSVDIALOG_H
#define CSVDIALOG_H

#include <KoCsvImportDialog.h>

namespace KSpread
{
class View;

/**
 * Provides dialog for managing CSV (comma separated value) data.
 *
 * Currently CSVDialog is used for converting text into columns,
 * inserting text file and pasting text from clipboard, where conversion
 * from CSV (comma separated value) data is is all required.
 * The different purposed mentioned above is determined
 * using mode, which can be Column, File, or Clipboard respectively.
 *
*/
class CSVDialog : public KoCsvImportDialog
{
  Q_OBJECT

public:
  enum Mode { Clipboard, File, Column };

  CSVDialog( View * parent, QRect const & rect, Mode mode);
  ~CSVDialog();

  bool canceled();

protected:
  void accept();

private:
  QString getText(int row, int col);

  View*   m_pView;
  bool    m_canceled;
  QRect   m_targetRect;
  Mode    m_mode;
};

} // namespace KSpread

#endif // CVSDIALOG_H
