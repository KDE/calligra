/* This file is part of the KDE project
   Copyright (C) 1999 David Faure <faure@kde.org>
   Copyright (C) 2004 Nicolas GOUTTE <goutte@kde.org>
   
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef CSVEXPORTDIALOG_H
#define CSVEXPORTDIALOG_H

#include <kdialogbase.h>

class ExportDialogUI;
class KSpreadMap;

class CSVExportDialog : public KDialogBase
{
  Q_OBJECT
 public:
  CSVExportDialog(QWidget * parent);
  ~CSVExportDialog();

  QChar   getDelimiter() const;
  QChar   getTextQuote() const;
  bool    exportTable( QString const & tableName ) const;
  bool    printAlwaysTableDelimiter() const;
  QString getTableDelimiter() const;  
  bool    exportSelectionOnly() const;

  void  fillTable( KSpreadMap * map );
  QString getEndOfLine(void) const;
  QTextCodec* getCodec(void) const;

 private:
  ExportDialogUI * m_dialog;

  QString m_delimiter;
  QChar   m_textquote;

 private slots:
  void slotOk();
  void slotCancel();

  void returnPressed();
  void delimiterClicked( int id );
  void textChanged ( const QString & );
  void textquoteSelected( const QString & mark );
  void selectionOnlyChanged( bool );
};

#endif
