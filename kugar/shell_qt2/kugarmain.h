/* This file is part of the KDE project
   Copyright (C) 2003 Alexander Dymo <cloudtemple@mksat.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this library; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KUGARMAIN_H
#define KUGARMAIN_H

#include "kugarmainbase.h"

class MReportViewer;

class KugarMain : public KugarMainBase
{
    Q_OBJECT

public:
    KugarMain(QString dataFile, QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~KugarMain();
    /*$PUBLIC_FUNCTIONS$*/

public slots:
    /*$PUBLIC_SLOTS$*/
    virtual void          viewLastPage();
    virtual void          viewFirstPage();
    virtual void          viewPrevPage();
    virtual void          viewNextPage();
    virtual void          helpAbout();
    virtual void          fileExit();
    virtual void          filePrint();
    virtual void          slotPreferredTemplate(const QString &tpl);

protected:
    /*$PROTECTED_FUNCTIONS$*/

protected slots:
    /*$PROTECTED_SLOTS$*/

private:
    MReportViewer* rptviewer;
    QString dataPath;
    bool gen_ok;
};

#endif

