/* This file is part of the KDE project
   Copyright (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002-2003 Ariya Hidayat <ariya@kde.org>
             (C) 2002      Laurent Montel <montel@kde.org>
             (C) 1999 David Faure <faure@kde.org>

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

#include <kdialogbase.h>
//Added by qt3to4:
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGridLayout>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class Q3ButtonGroup;
class QComboBox;
class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class Q3Table;


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
class CSVDialog : public KDialogBase
{
  Q_OBJECT

 public:

  enum Mode { Clipboard, File, Column };
  enum Header { TEXT, NUMBER, DATE, CURRENCY };

  CSVDialog( View * parent, const char * name, QRect const & rect, Mode mode);

  ~CSVDialog();

  bool cancelled();

 protected:
  void accept();


 private:
  View* m_pView;

  QGridLayout* MyDialogLayout;
  QHBoxLayout* Layout1;
  QGridLayout* m_delimiterBoxLayout;
  QGridLayout* m_formatBoxLayout;
  Q3Table* m_sheet;
  Q3ButtonGroup* m_delimiterBox;
  QRadioButton* m_radioComma;
  QRadioButton* m_radioSemicolon;
  QRadioButton* m_radioTab;
  QRadioButton* m_radioSpace;
  QRadioButton* m_radioOther;
  QLineEdit* m_delimiterEdit;
  Q3ButtonGroup* m_formatBox;
  QRadioButton* m_radioNumber;
  QRadioButton* m_radioText;
  QRadioButton* m_radioCurrency;
  QRadioButton* m_radioDate;
  QComboBox* m_comboLine;
  QComboBox* m_comboQuote;
  QLabel* TextLabel3;
  QLabel* TextLabel2;
  QCheckBox * m_ignoreDuplicates;

  void fillSheet();
  void fillComboBox();
  void setText(int row, int col, const QString& text);
  void adjustRows(int iRows);
  int  getHeader(int col);
  QString getText(int row, int col);

  bool m_cancelled;
  int   m_adjustRows;
  int   m_startline;
  QChar m_textquote;
  QString m_delimiter;
  QString m_data;
  QByteArray m_fileArray;
  QRect m_targetRect;
  Mode  m_mode;

 private slots:
  void returnPressed();
  void formatClicked(int id);
  void delimiterClicked(int id);
  void lineSelected(const QString& line);
  void textquoteSelected(const QString& mark);
  void currentCellChanged(int, int col);
  void textChanged ( const QString & );
  void ignoreDuplicatesChanged(int);
};

} // namespace KSpread

#endif // CVSDIALOG_H
