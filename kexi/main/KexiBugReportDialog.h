/* This file is part of the KDE project
   Copyright (C) 2014 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIBUGREPORTDIALOG_H
#define KEXIBUGREPORTDIALOG_H

#include <KBugReport>

//! A bug report dialog dedicated for Kexi.
//! It reports proper app version, platform and OS, hides unnecessary information.
class KexiBugReportDialog : public KBugReport
{
    Q_OBJECT

public:
    explicit KexiBugReportDialog(QWidget *parent = 0);

public Q_SLOTS:
    virtual void accept();

private:
    KAboutData *copyAboutData();
    void collectData();

    KAboutData *m_aboutData;
    QString m_op_sys;
    QString m_rep_platform;
    Q_DISABLE_COPY(KexiBugReportDialog)
};

#endif
