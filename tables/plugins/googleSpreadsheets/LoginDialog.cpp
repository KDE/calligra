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

#include "LoginDialog.h"


LoginDialog::LoginDialog(QWidget* parent)
    : QDialog(parent),
    m_loginDialog(new Ui_LoginDialog)
{
    setupDialog("Sign-In to Google-Documents-Service with your Google-Account:");
}

LoginDialog::LoginDialog(QWidget* parent, const QString& text)
    : QDialog(parent),
    m_loginDialog(new Ui_LoginDialog)
{
    setupDialog(text);
}

void LoginDialog::setupDialog(const QString& text)
{    
    m_loginDialog->setupUi(this);
    m_loginDialog->label->setText(text);
    m_loginDialog->usernameLineEdit->setCursorPosition(0);
}

LoginDialog::~LoginDialog()
{
    delete m_loginDialog;
}

void LoginDialog::accept()
{
    if ((m_loginDialog->usernameLineEdit->text().size() <= 10) || m_loginDialog->passwordLineEdit->text().isEmpty()) {
        m_loginDialog->label->setText("Incomplete input!\nPlease enter your Google-Email AND a password!");
        return;
    }

    QDialog::accept();
}

#include "LoginDialog.moc"
