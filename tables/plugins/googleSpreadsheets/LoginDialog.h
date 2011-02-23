/* This file is part of the KDE project
   Copyright (C) 2010-2011 Christoph Goerlich <chgoerlich@gmx.de>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

#include "ui_LoginDialog.h"


class LoginDialog : public QDialog
{
    Q_OBJECT
    
public:
    LoginDialog(QWidget* parent = 0);
    explicit LoginDialog(QWidget* parent, const QString &text);
    virtual ~LoginDialog();
    
    QString username() { return m_loginDialog->usernameLineEdit->text(); }
    QString password() { return m_loginDialog->passwordLineEdit->text(); }
    
private Q_SLOTS:
    void accept();
    
private:
    void setupDialog(const QString &text = 0);
    
    Ui_LoginDialog* m_loginDialog;

};

#endif //LOGINDIALOG_H
