/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef KPRESENTER_VIEW_IFACE_H
#define KPRESENTER_VIEW_IFACE_H

#include <KoViewIface.h>

#include <qstring.h>

class KPresenterView;

class KPresenterViewIface : public KoViewIface
{
    K_DCOP
public:
    KPresenterViewIface( KPresenterView *view_ );

k_dcop:
    // edit menu
    virtual void editUndo();
    virtual void editRedo();
    virtual void editCut();
    virtual void editCopy();
    virtual void editPaste();
    virtual void editDelete();
    virtual void editSelectAll();
    virtual void editDuplicatePage();
    virtual void editDelPage();
    virtual void editFind();
    virtual void editHeaderFooter();

    // view menu
    virtual void newView();

    // insert menu
    virtual void insertPage();
    virtual void insertPicture();
    virtual void insertClipart();

    // tools menu
    virtual void toolsMouse();
    virtual void toolsLine();
    virtual void toolsRectangle();
    virtual void toolsCircleOrEllipse();
    virtual void toolsPie();
    virtual void toolsText();
    virtual void toolsAutoform();
    virtual void toolsDiagramm();
    virtual void toolsTable();
    virtual void toolsFormula();
    virtual void toolsObject();

    // extra menu
    virtual void extraPenBrush();
    virtual void extraConfigPie();
    virtual void extraConfigRect();
    virtual void extraRaise();
    virtual void extraLower();
    virtual void extraRotate();
    virtual void extraShadow();
    virtual void extraBackground();
    virtual void extraLayout();
    virtual void extraOptions();
    virtual void extraLineBegin();
    virtual void extraLineEnd();
    virtual void extraWebPres();
    virtual void extraGroup();
    virtual void extraUnGroup();

    virtual void extraAlignObjLeft();
    virtual void extraAlignObjCenterH();
    virtual void extraAlignObjRight();
    virtual void extraAlignObjTop();
    virtual void extraAlignObjCenterV();
    virtual void extraAlignObjBottom();

    virtual void extraAlignObjs();

    // screen menu
    virtual void screenConfigPages();
    virtual void screenPresStructView();
    virtual void screenAssignEffect();
    virtual void screenStart();
    virtual void screenStop();
    virtual void screenPause();
    virtual void screenFirst();
    virtual void screenPrev();
    virtual void screenNext();
    virtual void screenLast();
    virtual void screenSkip();
    virtual void screenFullScreen();
    virtual void screenPenColor();
    virtual void screenPenWidth( const QString &w );

    // text toolbar
    virtual void sizeSelected();
    virtual void fontSelected();
    virtual void textBold();
    virtual void textItalic();
    virtual void textUnderline();
    virtual void textColor();
    virtual void textAlignLeft();
    virtual void textAlignCenter();
    virtual void textAlignRight();
    virtual void mtextFont();
    virtual void textEnumList();
    virtual void textUnsortList();
    virtual void textNormalText();
    virtual void textDepthPlus();
    virtual void textDepthMinus();
    virtual void textSettings();
    virtual void textContentsToHeight();
    virtual void textObjectToContents();

    // in presentation mode
    virtual int getCurrentPresPage();
    virtual int getCurrentPresStep();
    virtual int getPresStepsOfPage();
    virtual int getNumPresPages();
    virtual bool gotoPresPage( int pg );

    // in edit mode
    virtual int getCurrentPageNum();

    // in both modes
    virtual double getCurrentFaktor();

private:
    KPresenterView *view;

};

#endif
