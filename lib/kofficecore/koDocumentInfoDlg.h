/* This file is part of the KDE project
   Copyright (C) 2000 Simon Hausmann <hausmann@kde.org>

   $Id$

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#ifndef __koDocumentInfoDlg_h__
#define __koDocumentInfoDlg_h__

#include <kdialogbase.h>

class KoDocumentInfo;
class KoDocumentInfoAuthor;
class KoDocumentInfoAbout;

class KoDocumentInfoDlg : public KDialogBase
{
  Q_OBJECT
public:
  KoDocumentInfoDlg( KoDocumentInfo *docInfo, QWidget *parent = 0, const char *name = 0 );
  virtual ~KoDocumentInfoDlg();

  void save();

private:
  void addAuthorPage( KoDocumentInfoAuthor *authorInfo );
  void addAboutPage( KoDocumentInfoAbout *aboutInfo );

  void save( KoDocumentInfoAuthor *authorInfo );
  void save( KoDocumentInfoAbout *aboutInfo );

  class KoDocumentInfoDlgPrivate;
  KoDocumentInfoDlgPrivate *d;
};

#endif
