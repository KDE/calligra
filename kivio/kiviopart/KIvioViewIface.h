/* This file is part of the KDE project
   Copyright (C) 2001 Laurent Montel <lmontel@mandrakesoft.com>

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

#ifndef KIVIO_VIEW_IFACE_H
#define KIVIO_VIEW_IFACE_H

#include <KoViewIface.h>

#include <QString>
#include <QColor>
class KivioView;

class KIvioViewIface : public KoViewIface
{
    K_DCOP
public:
    KIvioViewIface( KivioView *view_ );

k_dcop:
    virtual void paperLayoutDlg();
    virtual void insertPage();
    virtual void removePage();
    virtual void renamePage();
    virtual void hidePage();
    virtual void showPage();
    virtual void optionsDialog();

    virtual bool isSnapGuides()const;
    virtual bool isShowGuides()const;
    virtual bool isShowRulers()const;
    virtual bool isShowPageMargins()const;


    virtual int leftBorder() const;
    virtual int rightBorder() const;
    virtual int topBorder() const;
    virtual int bottomBorder() const;

    virtual void togglePageMargins(bool);
    virtual void toggleShowRulers(bool);
    virtual void toggleShowGrid(bool);
    virtual void toggleSnapGrid(bool);
    virtual void toggleShowGuides(bool);

    virtual void toggleViewManager(bool);

    virtual void alignStencilsDlg();

    virtual void groupStencils();
    virtual void ungroupStencils();

    virtual void selectAllStencils();
    virtual void unselectAllStencils();

    virtual double lineWidth()const;
    QColor foreGroundColor()const;
    QColor backGroundColor()const;
private:
    KivioView *view;

};

#endif
