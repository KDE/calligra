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
    virtual void editCut();
    virtual void editCopy();
    virtual void editPaste();
    virtual void editDelete();
    virtual void editSelectAll();
    virtual void editDuplicatePage();
    virtual void editDelPage();
    virtual void editFind();
    virtual void editReplace();
    virtual void editDeSelectAll();
    // view menu
    virtual void newView();
    virtual void viewShowSideBar();
    virtual void viewShowNoteBar();

    // insert menu
    virtual void insertPage();
    virtual void insertPicture();
    virtual void insertClipart();
    virtual void insertSpecialChar();
    virtual void insertLink();
    virtual void insertComment();


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
    virtual void toolsFreehand();
    virtual void toolsPolyline();
    virtual void toolsQuadricBezierCurve();
    virtual void toolsCubicBezierCurve();
    virtual void toolsConvexOrConcavePolygon();
    virtual void toolsRotate();
    virtual void toolsZoom();


    // extra menu
    virtual void extraPenBrush();
    virtual void extraRaise();
    virtual void extraLower();
    virtual void extraRotate();
    virtual void extraShadow();
    virtual void extraBackground();
    virtual void extraLayout();
    virtual void extraConfigure();
    virtual void extraLineBegin();
    virtual void extraLineEnd();
    virtual void extraWebPres();
    virtual void extraGroup();
    virtual void extraUnGroup();
    virtual void extraPenStyle();
    virtual void extraPenWidth();

    virtual void extraAlignObjLeft();
    virtual void extraAlignObjCenterH();
    virtual void extraAlignObjRight();
    virtual void extraAlignObjTop();
    virtual void extraAlignObjCenterV();
    virtual void extraAlignObjBottom();

    virtual void extraAlignObjs();

    virtual void extraAutoFormat();

    virtual void extraSendBackward();
    virtual void extraBringForward();

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
    virtual void screenPenColor();
    virtual void screenPenWidth( const QString &w );
    virtual void screenViewPage();

    // text toolbar
    virtual void sizeSelected( int size );
    virtual void fontSelected( const QString &fontFamily );
    virtual void textBold();
    virtual void textItalic();
    virtual void textUnderline();
    virtual void textColor();
    virtual void textAlignLeft();
    virtual void textAlignCenter();
    virtual void textAlignRight();
    virtual void mtextFont();
    virtual void textDepthPlus();
    virtual void textDepthMinus();
    virtual void textContentsToHeight();
    virtual void textObjectToContents();
    virtual void formatParagraph();

    // color bar
    virtual void penChosen();
    virtual void brushChosen();

    // in presentation mode
    virtual int getCurrentPresPage() const;
    virtual int getCurrentPresStep() const;
    virtual int getPresStepsOfPage() const;
    virtual int getNumPresPages() const;
    virtual bool gotoPresPage( int pg ) const;

    // in edit mode
    virtual int getCurrentPageNum() const;

    // in both modes
    virtual double getCurrentFaktor()const;

    virtual void insertCustomVariable();
    virtual void insertNewCustomVariable();
    virtual void insertVariable();

    virtual void editCustomVars ();
    virtual void changeCaseOfText();

    virtual void refreshAllVariable();

    //return false when we can't "skipToPage" (num > nbpage )
    virtual bool skipToPage( int _num );

    virtual void configureCompletion();
    virtual void duplicateObj();
    virtual void applyAutoFormat();
    virtual void createStyleFromSelection();

    virtual void closeObject();

    void saveClipart();
    void savePicture();
    void insertFile();
private:
    KPresenterView *view;

};

#endif
