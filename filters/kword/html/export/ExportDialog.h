// 

/*
   This file is part of the KDE project
   Copyright (C) 2001, 2002 Nicolas GOUTTE <goutte@kde.org>

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

#ifndef EXPORT_DIALOG_H
#define EXPORT_DIALOG_H

#include <qwidget.h>
#include <kdialogbase.h>

class ExportDialogUI;

class HtmlExportDialog : public KDialogBase
{
    Q_OBJECT
public:

    enum Mode {
      Light =0,
      Basic,
      CSS
    };

    HtmlExportDialog(QWidget* parent=NULL);
    ~HtmlExportDialog(void);
    bool isXHtml(void) const;
    QTextCodec* getCodec(void) const;
    Mode getMode(void) const;
private:
    ExportDialogUI* m_dialog;
};

#endif /* EXPORT_DIALOG_H */
