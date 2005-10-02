/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

   This is temporary solution for Kexi CSV support,
   based on kspread/dialogs/kspread_dlg_csv.h

   Copyright (C) 2002-2003 Norbert Andres <nandres@web.de>
			 (C) 2002-2003 Ariya Hidayat <ariya@kde.org>
			 (C) 2002	  Laurent Montel <montel@kde.org>
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

#ifndef KEXI_CSVDIALOG_H
#define KEXI_CSVDIALOG_H

#include <qvaluevector.h>
#include <qvaluelist.h>
#include <qptrvector.h>
#include <qregexp.h>
#include <qbitarray.h>

#include <kdialogbase.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QButtonGroup;
class QComboBox;
class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QTable;

//class KSpreadView;

class KexiMainWindow;

/**
 * This is temporary solution for Kexi CSV support,
 * based on kspread/dialogs/kspread_dlg_csv.h, cc.
 *
 * Provides dialog for managing CSV (comma separated value) data.
 *
 * Currently KexiCSVDialog is used for converting text into columns,
 * inserting text file and pasting text from clipboard, where conversion
 * from CSV (comma separated value) data is is all required. 
 * The different purposed mentioned above is determined
 * using mode, which can be Column, File, or Clipboard respectively.
*/

class KexiCSVDialog : public KDialogBase
{
  Q_OBJECT

 public:

  enum Mode { Clipboard, File /*, Column*/ };
  enum Header { TEXT, NUMBER, DATE, CURRENCY };

//! @todo what about making it kexidb-independent?
  KexiCSVDialog( Mode mode, KexiMainWindow* mainWin, QWidget * parent, const char * name = 0/*, QRect const & rect*/);

  ~KexiCSVDialog();

  bool cancelled();

 protected:
  bool loadData();
  virtual void accept();


 private:
//  KexiView* m_pView;

  QGridLayout* MyDialogLayout;
  QHBoxLayout* Layout1;
  QGridLayout* m_delimiterBoxLayout;
  QGridLayout* m_formatBoxLayout;
  QTable* m_table;
  QButtonGroup* m_delimiterBox;
  QRadioButton* m_radioComma;
  QRadioButton* m_radioSemicolon;
  QRadioButton* m_radioTab;
  QRadioButton* m_radioSpace;
  QRadioButton* m_radioOther;
  QLineEdit* m_delimiterEdit;
  QButtonGroup* m_formatBox;
	QString m_formatBoxTitle;
  QRadioButton* m_radioNumber;
  QRadioButton* m_radioText;
  QRadioButton* m_radioCurrency;
  QRadioButton* m_radioDate;
  QComboBox* m_comboLine;
  QComboBox* m_comboQuote;
  QLabel* TextLabel3;
  QLabel* TextLabel2;
  QCheckBox* m_ignoreDuplicates;
  QCheckBox* m_1stRowForFieldNames;
  QCheckBox* m_primaryKeyField;

  KexiMainWindow* m_mainWin;

  void fillTable();
  void fillComboBox();
  void detectTypeAndUniqueness(int row, int col, const QString& text);
  void setText(int row, int col, const QString& text);
  void adjustRows(int iRows);
  int  getHeader(int col);
  QString getText(int row, int col);
  void updateColumnText(int col);

  bool m_cancelled;
  int   m_adjustRows;
  int   m_startline;
  QChar m_textquote;
  QString m_delimiter;
  QString m_data;
  QByteArray m_fileArray;
//  QRect m_targetRect;
	Mode m_mode;
	int m_prevSelectedCol;

	//vector of detected types, 0==text (the default), 1==number, 2==date
//! @todo ...
	QValueVector<int> m_detectedTypes;
	//! m_detectedUniqueColumns[i]==true means that i-th column has unique values
	//! (only for numeric type)
//	QBitArray m_detectedUniqueColumns;
	QPtrVector< QValueList<int> > m_uniquenessTest;
//! @todo tmp
	QRegExp m_dateRegExp1, m_dateRegExp2, m_fpNumberRegExp;
	QValueVector<QString> m_typeNames, m_columnNames;
	QBitArray m_changedColumnNames;
	bool m_columnsAdjusted : 1; //only once
	bool m_1stRowForFieldNamesDetected : 1; //!< used to force rerun fillTable() after 1st row
	bool m_firstFillTableCall : 1; //!< used to know whether it's 1st fillTable() call
	int m_primaryKeyColumn; //!< index of column with PK assigned (-1 if none)
	QPixmap m_pkIcon;

	QString m_fname;
	QString m_encoding;

 private slots:
  void returnPressed();
  void formatClicked(int id);
  void delimiterClicked(int id);
  void lineSelected(const QString& line);
  void textquoteSelected(const QString& mark);
  void currentCellChanged(int, int col);
  void textChanged ( const QString & );
  void ignoreDuplicatesChanged(int);
  void slot1stRowForFieldNamesChanged(int);
  void cellValueChanged(int row,int col);
  void optionsButtonClicked();
};

#endif
