/* This file is part of the KDE project
   Copyright (C) 2001 George Staikos <staikos@kde.org>

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

#ifndef PWDDLG_H
#define PWDDLG_H

#include <kdialog.h>
class QString;
class KLineEdit;
class QLabel;
class QPushButton;
class PasswordPrompt;


class PasswordPrompt : public KDialog {
  Q_OBJECT

 public:
   PasswordPrompt(QWidget *parent = 0L, const char *name = 0L);
   virtual ~PasswordPrompt();

   QString& getPassword();

 signals:
   void setPassword(QString);

 private slots:
   void doOk(const QString&);
   void ok();
   void cancel();

 private:
   KLineEdit *_pwd;
   QLabel    *_prompt;
   QPushButton *_ok, *_cancel;
   QString    _password;
};


#endif

