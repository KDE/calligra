/* This file is part of the KDE project
   Copyright (C) 2003 Norbert Andres <nandres@web.de>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 1999-2002 Laurent Montel <montel@kde.org>
             (C) 1999 Stephan Kulow <coolo@kde.org>
             (C) 1998-1999 Torben Weis <weis@kde.org>

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

#ifndef __kspread_dlg_anchor__
#define __kspread_dlg_anchor__

#include <kdialogbase.h>
#include <qrect.h>

class KSpreadView;
class KSpreadSheet;
class KSpreadCell;
class KURLRequester;
class QLineEdit;
class QPushButton;

class internetAnchor : public QWidget
{
  Q_OBJECT
public:
  internetAnchor( KSpreadView* _view, QWidget *parent = 0, char *name = 0 );
  QString text() const;
  QString link() const;
private:
    QString createLink() const;
    KSpreadView* m_pView;

    QLineEdit* textEdit;
    QLineEdit* l_internet;
};

class mailAnchor : public QWidget
{
  Q_OBJECT
public:
  mailAnchor( KSpreadView* _view, QWidget *parent = 0, char *name = 0 );
  QString text() const;
  QString link() const;
private:
    KSpreadView* m_pView;

    QLineEdit* textEdit;
    QLineEdit* l_mail;
};

class fileAnchor : public QWidget
{
  Q_OBJECT
public:
  fileAnchor( KSpreadView* _view, QWidget *parent = 0, char *name = 0 );
  QString text() const;
  QString link() const;
private slots:
    void slotSelectRecentFile( const QString &_file );
private:
    KSpreadView* m_pView;

    QLineEdit* textEdit;
    KURLRequester* l_file;
};

class cellAnchor : public QWidget
{
  Q_OBJECT
public:
  cellAnchor( KSpreadView* _view, QWidget *parent = 0, char *name = 0 );
  QString text() const;
  QString link() const;
private:
    KSpreadView* m_pView;

    QLineEdit* textEdit;
    QLineEdit* l_cell;
};

class KSpreadLinkDlg : public KDialogBase
{
    Q_OBJECT
public:
    KSpreadLinkDlg( KSpreadView* parent, const char* name );
    QString text() const;
    QString link() const;

protected slots:
  void slotOk();

private:
    KSpreadView* m_pView;
    QString m_text;
    QString m_link;
    internetAnchor *_internetAnchor;
    mailAnchor *_mailAnchor;
    fileAnchor *_fileAnchor;
    cellAnchor *_cellAnchor;
};


#endif
