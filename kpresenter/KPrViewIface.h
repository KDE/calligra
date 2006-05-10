// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPRESENTER_VIEW_IFACE_H
#define KPRESENTER_VIEW_IFACE_H

#include <KoViewIface.h>

#include <QString>
#include <qstringlist.h>

class KPrView;

class KPrViewIface : public KoViewIface
{
    K_DCOP
public:
    KPrViewIface( KPrView *view_ );

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
    virtual void viewSlideMaster();
    virtual void viewFooter();
    virtual void viewHeader();

    // insert menu
    virtual void insertPage();
    virtual void insertPicture();
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
    virtual void extraProperties();
    virtual void extraPenBrush(); /** * @deprecated */
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
    virtual void extraMSPres();
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

    virtual void extraAutoFormat();

    virtual void extraSendBackward();
    virtual void extraBringForward();

    // screen menu
    virtual void screenConfigPages();
    virtual void screenAssignEffect();
    virtual void screenStart();
    virtual void screenStartFromFirst();
    virtual void screenStop();
    virtual void screenPause();
    virtual void screenFirst();
    virtual void screenPrev();
    virtual void screenNext();
    virtual void screenLast();
    virtual void screenSkip();
    virtual void screenTransEffect();


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

    void savePicture();

    /**
     \brief Save page to bitmap file.

     Export a page of the currently open presentation to disk
     using a bitmap format like e.g. PNG.
     This method uses a QPixmap::save() call.

     \param _nPage the user visible <b>1-based</b> page number
     \param _nWidth the desired image width in px
     \param _nHeight the desired image height in px
     \param _fileName the name of the image file to be created (see QPixmap::save())
     \param _format the format of the image file (see QPixmap::save())
     \param _quality the quality of the image (see QPixmap::save())
     \param _verbose the verbosity of the method's return value:
       if 0 < _verbose exportPage() returns the title and the notes of the page
       if not _verbose it returns an empty string

     \returns Page title and page notes if the file was written successfully.

     \sa KPrCanvas::exportPage
     */
    QStringList exportPage( int _nPage,
                            int _nWidth,
                            int _nHeight,
                            const QString & _fileName,
                            const QString & _format,
                            int _quality,
                            int _verbose )const;
    void insertFile();
    void importStyle();
    void backgroundPicture();
    void insertFile( const QString & file );
    void addWordToDictionary();

    void customSlideShow();

    void insertLineBreak();
    void increaseNumberingLevel();
    void decreaseNumberingLevel();

    void increaseFontSize();
    void decreaseFontSize();

    void flipHorizontal();
    void flipVertical();



private:
    KPrView *view;

};

#endif
