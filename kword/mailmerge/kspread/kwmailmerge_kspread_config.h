/*
   This file is part of the KDE project
   Copyright (C) 2004 Tobias Koenig <tokoe@kde.org>

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

#ifndef _KWMAILMERGE_KSPREAD_CONFIG_H_
#define _KWMAILMERGE_KSPREAD_CONFIG_H_

#include <kdialog.h>

#include "kwmailmerge_kspread.h"

class KComboBox;
class KUrlRequester;

class KWMailMergeKSpread;

class KWMailMergeKSpreadConfig: public KDialog
{
  Q_OBJECT

  public:
    KWMailMergeKSpreadConfig( QWidget *parent, KWMailMergeKSpread *object );
    virtual ~KWMailMergeKSpreadConfig();


  protected slots:
    virtual void slotOk();

    void loadDocument();
    void documentLoaded();
    void slotTextChanged( const QString & _text );

  private:
    void initGUI();

    KWMailMergeKSpread *_object;
    KSpread::Doc *_document;

    KUrlRequester *_urlRequester;
    KComboBox *_pageNumber;

    int _initialPage;
};

#endif
