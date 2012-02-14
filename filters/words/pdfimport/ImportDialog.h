/*
   This file is part of the KDE project
   Copyright (C) 2011, 2012 Pankaj Kumar <me@panks.in>

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

#ifndef _IMPORT_DIALOG_H
#define _IMPORT_DIALOG_H

#include <QtGui/QWidget>
#include <QtGui/QButtonGroup>

#include <kdialog.h>
#include <ui_ImportDialogUI.h>

class PdfImportDialog : public KDialog
{
    Q_OBJECT
public:
    PdfImportDialog(const QString &encoding, QWidget *parent = 0);
    ~PdfImportDialog();
    QTextCodec *getCodec() const;
    int getParagraphStrategy() const;

private:
    Ui::ImportDialogUI m_ui;
    QButtonGroup m_radioGroup;
};

#endif /* _IMPORT_DIALOG_H */
