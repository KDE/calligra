/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
   Copyright (C) 1999 Montel Laurent <montell@club-internet.fr>

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
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qrect.h>
#include <qcheckbox.h>

class KSpreadView;
class KSpreadTable;
class KSpreadCell;

class internetAnchor : public QWidget
{
  Q_OBJECT
public:
  internetAnchor( KSpreadView* _view, QWidget *parent = 0, char *name = 0 );
  QString apply();
private:
    QString createLink();
    KSpreadView* m_pView;

    QLineEdit* text;
    QLineEdit* l_internet;
    QCheckBox *bold;
    QCheckBox *italic;
};

class mailAnchor : public QWidget
{
  Q_OBJECT
public:
  mailAnchor( KSpreadView* _view, QWidget *parent = 0, char *name = 0 );
  QString apply();
private:
    QString createLink();
    KSpreadView* m_pView;

    QLineEdit* text;
    QLineEdit* l_mail;
    QCheckBox *bold;
    QCheckBox *italic;
};

class cellAnchor : public QWidget
{
  Q_OBJECT
public:
  cellAnchor( KSpreadView* _view, QWidget *parent = 0, char *name = 0 );
  QString apply();
private:
    QString createLink();
    KSpreadView* m_pView;

    QLineEdit* text;
    QLineEdit* l_cell;
    QCheckBox *bold;
    QCheckBox *italic;
};

class KSpreadLinkDlg : public KDialogBase
{
    Q_OBJECT
public:
    KSpreadLinkDlg( KSpreadView* parent, const char* name );

public slots:
  void slotOk();
  void setCellText(const QString &_text);
 private:
    KSpreadView* m_pView;
    internetAnchor *_internetAnchor; 
    mailAnchor *_mailAnchor;
    cellAnchor *_cellAnchor;
};




#endif
