// $Header$

/*
   This file is part of the KDE project
   Copyright (C) 2001 Nicolas GOUTTE <nicog@snafu.de>

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

#ifndef IMPORT_DIALOG_H
#define IMPORT_DIALOG_H

#include <qwidget.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qradiobutton.h>
#include <qvbuttongroup.h>
#include <qlineedit.h>

#include <kdialogbase.h>

class ImportDialogUI;

class HtmlImportDialog : public KDialogBase
{
    Q_OBJECT
public:

    HtmlImportDialog(QWidget* parent=NULL);
    ~HtmlImportDialog(void);
public:
    void setHintCharset(const QString& strNew);
    QString getHintCharset(void);
    int getHint(void);
private:
    ImportDialogUI* m_dialog;
};

#endif /* IMPORT_DIALOG_H */
