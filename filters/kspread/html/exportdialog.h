/* This file is part of the KDE project
   Copyright (C) 2005 Bram Schoenmakers <bramschoenmakers@kde.nl>

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

#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <kdialogbase.h>

class ExportWidget;

class ExportDialog : public KDialogBase
{
  Q_OBJECT
  public:
    ExportDialog( QWidget *parent = 0, const char *name = 0 );
    ~ExportDialog();

    /**
      Returns preferred encoding. Defaults to UTF-8.
     */
    QTextCodec *encoding() const;

    /**
      Returns a valid URL if the custom button was selected.
      Else, it will return QString::null.
    */
    QString customStyleURL() const;

    /**
      Returns true if borders should be shown, false if borders
      should be hidden.
     */
    bool useBorders() const;
  private:
    ExportWidget *m_mainwidget;
};

#endif
