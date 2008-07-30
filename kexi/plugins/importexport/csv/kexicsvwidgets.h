/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXI_CSVWIDGETS_H
#define KEXI_CSVWIDGETS_H

#include <KComboBox>

class KLineEdit;
class QLabel;
class QFrame;

#define KEXICSV_DEFAULT_FILE_TEXT_QUOTE "\""
#define KEXICSV_DEFAULT_CLIPBOARD_TEXT_QUOTE ""
#define KEXICSV_DEFAULT_FILE_DELIMITER ","
#define KEXICSV_DEFAULT_CLIPBOARD_DELIMITER "\t"
#define KEXICSV_DEFAULT_FILE_DELIMITER_INDEX 0

//! \return a list of mimetypes usable for handling CSV format handling
QStringList csvMimeTypes();

/*! @short A helper widget showing a short text information with an icon.
 See ctor for details.
 Used by CSV import and export dialogs. */
class KexiCSVInfoLabel : public QWidget
{
  public:
    /* Sets up a new info label \a labelText label with text like "Preview of data from file:".
     setFileName() can be used to display filename and setCommentAfterFileName() to display 
     additional comment.

     The widget's layout can look like this:

     \pre [icon] [labeltext] [filename] [comment]
    */
    KexiCSVInfoLabel( const QString& labelText, QWidget* parent );

    void setFileName( const QString& fileName );
    void setLabelText( const QString& text );
    void setCommentText( const QString& text );
//		void setIconForFileName();

    //! sets icon pixmap to \a iconName. Used wher setIconForFilename was false in ctor.
    void setIcon(const QString& iconName);

    QLabel* leftLabel() const { return m_leftLabel; }
    QLabel* fileNameLabel() const { return m_fnameLbl; }
    QLabel* commentLabel() const { return m_commentLbl; }
    QFrame* separator() const { return m_separator; }

  protected:
    QLabel *m_leftLabel, *m_iconLbl, *m_fnameLbl, *m_commentLbl;
    QFrame* m_separator;
};

//! @short A combo box widget providing a list of possible delimiters
//! Used by CSV import and export dialogs
class KexiCSVDelimiterWidget : public QWidget
{
  Q_OBJECT

  public:
    KexiCSVDelimiterWidget( bool lineEditOnBottom, QWidget * parent = 0 );
    ~KexiCSVDelimiterWidget();

    QString delimiter() const;
    void setDelimiter(const QString& delimiter);

  signals:
    void delimiterChanged(const QString& delimiter);

  protected slots:
    //! only called when a delimiter was set by user directly
    void slotDelimiterChanged(int idx);
    void slotDelimiterChangedInternal(int idx);
    void slotDelimiterLineEditTextChanged( const QString & );
    void slotDelimiterLineEditReturnPressed();

  protected:
    class Private;
    Private * const d;
};

//! @short A combo box widget providing a list of possible quote characters
//! Used by CSV import and export dialogs
class KexiCSVTextQuoteComboBox : public KComboBox
{
  public:
    KexiCSVTextQuoteComboBox( QWidget * parent = 0 );

    QString textQuote() const;

    //! Sets text quote. Only available are: ", ', and empty string.
    void setTextQuote(const QString& textQuote);
};

#endif
