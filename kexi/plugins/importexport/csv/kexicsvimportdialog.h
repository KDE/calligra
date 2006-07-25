/* This file is part of the KDE project
   Copyright (C) 2005-2006 Jaroslaw Staniek <js@iidea.pl>

   This work is based on kspread/dialogs/kspread_dlg_csv.cc
   and will be merged back with KOffice libraries.

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

#include <kexiutils/tristate.h>
#include <kexidb/connection.h>

#include "kexicsvimportoptionsdlg.h"

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QButtonGroup;
class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QTable;
class QFile;
class KComboBox;
class KIntSpinBox;
class KProgressDialog;

class KexiMainWindow;
class KexiCSVDelimiterWidget;
class KexiCSVTextQuoteComboBox;
class KexiCSVInfoLabel;

/**
 * @short Kexi CSV import dialog
 *
 * This is temporary solution for Kexi CSV import,
 * based on kspread/dialogs/kspread_dlg_csv.h, cc.
 *
 * Provides dialog for managing CSV (comma separated value) data.
 *
 * Currently KexiCSVImportDialog is used for converting text into columns,
 * inserting text file and pasting text from clipboard, where conversion
 * from CSV (comma separated value) data is is all required. 
 * The different purposed mentioned above is determined
 * using mode, which can be Column, File, or Clipboard respectively.
*/
class KexiCSVImportDialog : public KDialogBase
{
  Q_OBJECT

 public:

  enum Mode { Clipboard, File /*, Column*/ };
  enum Header { TEXT, NUMBER, DATE, CURRENCY };

//! @todo what about making it kexidb-independent?
  KexiCSVImportDialog( Mode mode, KexiMainWindow* mainWin, QWidget * parent, 
	  const char * name = 0/*, QRect const & rect*/);

  ~KexiCSVImportDialog();

  bool cancelled() const;
  virtual bool eventFilter ( QObject * watched, QEvent * e );

 protected:
  bool openData();
  virtual void accept();


 private:
  QGridLayout* MyDialogLayout;
  QHBoxLayout* Layout1;
  QTable* m_table;
  KexiCSVDelimiterWidget* m_delimiterWidget;
  QString m_formatComboText;
  QLabel* m_formatLabel;
  KComboBox* m_formatCombo;
  KIntSpinBox *m_startAtLineSpinBox;
  KexiCSVTextQuoteComboBox* m_comboQuote;
  QLabel* m_startAtLineLabel;
  QLabel* TextLabel2;
  QCheckBox* m_ignoreDuplicates;
  QCheckBox* m_1stRowForFieldNames;
  QCheckBox* m_primaryKeyField;

  KexiMainWindow* m_mainWin;

  void detectTypeAndUniqueness(int row, int col, const QString& text);
  void setText(int row, int col, const QString& text, bool inGUI);

	/*! Parses date from \a text and stores into \a date. 
	 m_dateRegExp is used for clever detection; 
	 if '/' separated is found, it's assumed the format is american mm/dd/yyyy.
	 This function supports ommited zeros, so 1/2/2006 is parsed properly too.
	 \return true on success. */
	bool parseDate(const QString& text, QDate& date);

	/*! Parses time from \a text and stores into \a date. 
	 m_timeRegExp1 and m_timeRegExp2 are used for clever detection; 
	 both hh:mm:ss and hh:mm are supported.
	 This function supports ommited zeros, so 1:2:3 is parsed properly too.
	 \return true on success. */
	bool parseTime(const QString& text, QTime& time);

	/*! Called after the first fillTable() when number of rows is unknown. */
  void adjustRows(int iRows);

  int  getHeader(int col);
  QString getText(int row, int col);
  void updateColumnText(int col);
  void updateRowCountInfo();
  tristate loadRows(QString &field, int &row, int &columnm, int &maxColumn, bool inGUI);
  //! Used by loadRows().
  QString detectDelimiterByLookingAtFirstBytesOfFile(QTextStream& inputStream);

  /*! Callback, called whenever row is loaded in loadRows(). When inGUI is true, 
   nothing is performed, else database buffer is written back to the database. */
  bool saveRow(bool inGUI);

  bool m_cancelled;
  int m_adjustRows;
  int m_startline;
  QChar m_textquote;
  QString m_clipboardData;
  QByteArray m_fileArray;
	Mode m_mode;
	int m_prevSelectedCol;

	//! vector of detected types, 0==text (the default), 1==number, 2==date
//! @todo more types
	QValueVector<int> m_detectedTypes;

	//! m_detectedUniqueColumns[i]==true means that i-th column has unique values
	//! (only for numeric type)
	QPtrVector< QValueList<int> > m_uniquenessTest;

	QRegExp m_dateRegExp, m_timeRegExp1, m_timeRegExp2, m_fpNumberRegExp;
	QValueVector<QString> m_typeNames, m_columnNames;
	QBitArray m_changedColumnNames;
	bool m_columnsAdjusted : 1; //only once
	bool m_1stRowForFieldNamesDetected : 1; //!< used to force rerun fillTable() after 1st row
	bool m_firstFillTableCall : 1; //!< used to know whether it's 1st fillTable() call
	bool m_blockUserEvents : 1;
	int m_primaryKeyColumn; //!< index of column with PK assigned (-1 if none)
	int m_maximumRowsForPreview;
	int m_maximumBytesForPreview;
	QPixmap m_pkIcon;
	QString m_fname;
	QFile* m_file;
	QTextStream *m_inputStream; //!< used in loadData()
	KexiCSVImportOptions m_options;
	KProgressDialog *m_loadingProgressDlg, *m_importingProgressDlg;
	bool m_dialogCancelled;
	KexiCSVInfoLabel *m_infoLbl;
	KexiDB::Connection *m_conn; //!< (temp) database connection used for importing
	KexiDB::TableSchema *m_destinationTableSchema;  //!< (temp) dest. table schema used for importing
	KexiDB::PreparedStatement::Ptr m_importingStatement;
	QValueList<QVariant> m_dbRowBuffer; //!< (temp) used for importing
	bool m_implicitPrimaryKeyAdded; //!< (temp) used for importing
	bool m_allRowsLoadedInPreview; //!< we need to know whether all rows were loaded or it's just a partial data preview
	bool m_stoppedAt_MAX_BYTES_TO_PREVIEW; //!< used to compute m_allRowsLoadedInPreview

 private slots:
  void fillTable();
  void initLater();
  void formatChanged(int id);
  void delimiterChanged(const QString& delimiter);
  void startlineSelected(int line);
  void textquoteSelected(int);
  void currentCellChanged(int, int col);
  void ignoreDuplicatesChanged(int);
  void slot1stRowForFieldNamesChanged(int);
  void cellValueChanged(int row,int col);
  void optionsButtonClicked();
  void slotPrimaryKeyFieldToggled(bool on);
};

#endif
