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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "KPresenterViewIface.h"

#include "kpresenter_view.h"
#include "kprcanvas.h"
#include "kprpage.h"
#include "kpresenter_doc.h"

#include <kapplication.h>
#include <dcopclient.h>

KPresenterViewIface::KPresenterViewIface( KPresenterView *view_ )
    : KoViewIface( view_ )
{
    view = view_;
}

void KPresenterViewIface::editCut()
{
    view->editCut();
}

void KPresenterViewIface::editCopy()
{
    view->editCopy();
}

void KPresenterViewIface::editPaste()
{
    view->editPaste();
}

void KPresenterViewIface::editDelete()
{
    view->editDelete();
}

void KPresenterViewIface::editSelectAll()
{
    view->editSelectAll();
}

void KPresenterViewIface::editDuplicatePage()
{
    view->editDuplicatePage();
}

void KPresenterViewIface::editDelPage()
{
    view->editDelPage();
}

void KPresenterViewIface::editFind()
{
    view->editFind();
}

void KPresenterViewIface::editReplace()
{
    view->editReplace();
}

void KPresenterViewIface::newView()
{
    view->newView();
}

void KPresenterViewIface::insertPage()
{
    view->insertPage();
}

void KPresenterViewIface::insertPicture()
{
    view->insertPicture();
}

void KPresenterViewIface::toolsMouse()
{
    view->toolsMouse();
}

void KPresenterViewIface::toolsLine()
{
    view->toolsLine();
}

void KPresenterViewIface::toolsRectangle()
{
    view->toolsRectangle();
}

void KPresenterViewIface::toolsCircleOrEllipse()
{
    view->toolsCircleOrEllipse();
}

void KPresenterViewIface::toolsPie()
{
    view->toolsPie();
}

void KPresenterViewIface::toolsText()
{
    view->toolsText();
}

void KPresenterViewIface::toolsAutoform()
{
    view->toolsAutoform();
}

void KPresenterViewIface::toolsDiagramm()
{
    view->toolsDiagramm();
}

void KPresenterViewIface::toolsTable()
{
    view->toolsTable();
}

void KPresenterViewIface::toolsFormula()
{
    view->toolsFormula();
}

void KPresenterViewIface::toolsObject()
{
    view->toolsObject();
}

void KPresenterViewIface::toolsFreehand()
{
    view->toolsFreehand();
}

void KPresenterViewIface::toolsPolyline()
{
    view->toolsPolyline();
}

void KPresenterViewIface::toolsQuadricBezierCurve()
{
    view->toolsQuadricBezierCurve();
}

void KPresenterViewIface::toolsCubicBezierCurve()
{
    view->toolsCubicBezierCurve();
}

void KPresenterViewIface::toolsConvexOrConcavePolygon()
{
    view->toolsConvexOrConcavePolygon();
}

void KPresenterViewIface::extraPenBrush()
{
    view->extraPenBrush();
}


void KPresenterViewIface::extraRaise()
{
    view->extraRaise();
}

void KPresenterViewIface::extraLower()
{
    view->extraLower();
}

void KPresenterViewIface::extraRotate()
{
    view->extraRotate();
}

void KPresenterViewIface::extraShadow()
{
    view->extraShadow();
}

void KPresenterViewIface::extraBackground()
{
    view->extraBackground();
}

void KPresenterViewIface::extraLayout()
{
    view->extraLayout();
}

void KPresenterViewIface::extraConfigure()
{
    view->extraConfigure();
}

void KPresenterViewIface::extraLineBegin()
{
    view->extraLineBegin();
}

void KPresenterViewIface::extraLineEnd()
{
    view->extraLineEnd();
}

void KPresenterViewIface::extraWebPres()
{
    view->extraWebPres();
}

void KPresenterViewIface::extraMSPres()
{
    view->extraMSPres();
}

void KPresenterViewIface::extraAlignObjLeft()
{
    view->extraAlignObjLeft();
}

void KPresenterViewIface::extraAlignObjCenterH()
{
    view->extraAlignObjCenterH();
}

void KPresenterViewIface::extraAlignObjRight()
{
    view->extraAlignObjRight();
}

void KPresenterViewIface::extraAlignObjTop()
{
    view->extraAlignObjTop();
}

void KPresenterViewIface::extraAlignObjCenterV()
{
    view->extraAlignObjCenterV();
}

void KPresenterViewIface::extraAlignObjBottom()
{
    view->extraAlignObjBottom();
}

void KPresenterViewIface::extraAlignObjs()
{
    view->extraAlignObjs();
}

void KPresenterViewIface::extraGroup()
{
    view->extraGroup();
}

void KPresenterViewIface::extraUnGroup()
{
    view->extraUnGroup();
}

void KPresenterViewIface::extraPenStyle()
{
    view->extraPenStyle();
}

void KPresenterViewIface::extraPenWidth()
{
    view->extraPenWidth();
}

void KPresenterViewIface::screenTransEffect()
{
    view->screenTransEffect();
}

void KPresenterViewIface::screenConfigPages()
{
    view->screenConfigPages();
}

void KPresenterViewIface::screenAssignEffect()
{
    view->screenAssignEffect();
}

void KPresenterViewIface::screenStart()
{
    view->screenStart();
}

void KPresenterViewIface::screenStartFromFirst()
{
    view->screenStartFromFirst();
}

void KPresenterViewIface::screenStop()
{
    view->screenStop();
}

void KPresenterViewIface::screenPause()
{
    view->screenPause();
}

void KPresenterViewIface::screenFirst()
{
    view->screenFirst();
}

void KPresenterViewIface::screenPrev()
{
    view->screenPrev();
}

void KPresenterViewIface::screenNext()
{
    view->screenNext();
}

void KPresenterViewIface::screenLast()
{
    view->screenLast();
}

void KPresenterViewIface::screenSkip()
{
    view->screenSkip();
}

void KPresenterViewIface::sizeSelected( int size )
{
    view->sizeSelected( size );
}

void KPresenterViewIface::fontSelected( const QString &fontFamily )
{
    view->fontSelected( fontFamily );
}

void KPresenterViewIface::textBold()
{
    view->textBold();
}

void KPresenterViewIface::textItalic()
{
    view->textItalic();
}

void KPresenterViewIface::textUnderline()
{
    view->textUnderline();
}

void KPresenterViewIface::textColor()
{
    view->textColor();
}

void KPresenterViewIface::textAlignLeft()
{
    view->textAlignLeft();
}

void KPresenterViewIface::textAlignCenter()
{
    view->textAlignCenter();
}

void KPresenterViewIface::textAlignRight()
{
    view->textAlignRight();
}

void KPresenterViewIface::mtextFont()
{
    view->mtextFont();
}

void KPresenterViewIface::textDepthPlus()
{
    view->textDepthPlus();
}

void KPresenterViewIface::textDepthMinus()
{
    view->textDepthMinus();
}

void KPresenterViewIface::textContentsToHeight()
{
    view->textContentsToHeight();
}

void KPresenterViewIface::textObjectToContents()
{
    view->textObjectToContents();
}

void KPresenterViewIface::penChosen()
{
    view->penChosen();
}

void KPresenterViewIface::brushChosen()
{
    view->brushChosen();
}

int KPresenterViewIface::getCurrentPresPage() const
{
    return view->getCurrentPresPage();
}

int KPresenterViewIface::getCurrentPresStep() const
{
    return view->getCurrentPresStep();
}

int KPresenterViewIface::getPresStepsOfPage() const
{
    return view->getPresStepsOfPage();
}

int KPresenterViewIface::getNumPresPages() const
{
    return view->getNumPresPages();
}

bool KPresenterViewIface::gotoPresPage( int pg ) const
{
    return view->gotoPresPage( pg );
}

int KPresenterViewIface::getCurrentPageNum() const
{
    return view->getCurrPgNum();
}

void KPresenterViewIface::insertSpecialChar()
{
    view->insertSpecialChar();
}

void KPresenterViewIface::insertLink()
{
    view->insertLink();
}

void KPresenterViewIface::editCustomVars()
{
    view->editCustomVars();
}

void KPresenterViewIface::insertVariable()
{
    view->insertVariable();
}

void KPresenterViewIface::insertCustomVariable()
{
    view->insertCustomVariable();
}

void KPresenterViewIface::insertNewCustomVariable()
{
    view->insertNewCustomVariable();
}

void KPresenterViewIface::changeCaseOfText()
{
    view->changeCaseOfText();
}

void KPresenterViewIface::viewShowSideBar()
{
    view->viewShowSideBar();
}

void KPresenterViewIface::viewShowNoteBar()
{
    view->viewShowNoteBar();
}

void KPresenterViewIface::formatParagraph()
{
    view->formatParagraph();
}

void KPresenterViewIface::extraAutoFormat()
{
    view->extraAutoFormat();
}

void KPresenterViewIface::refreshAllVariable()
{
    view->extraAutoFormat();
}

bool KPresenterViewIface::skipToPage( int _num )
{
    //don't try to skip to a not exist page
    if(_num>(int)view->kPresenterDoc()->getPageNums()-1)
        return false;
    view->skipToPage(_num);
    return true;
}

void KPresenterViewIface::toolsRotate()
{
    view->toolsRotate();
}

void KPresenterViewIface::toolsZoom()
{
    view->toolsZoom();
}

void KPresenterViewIface::insertComment()
{
    view->insertComment();
}

void KPresenterViewIface::configureCompletion()
{
    view->configureCompletion();
}

void KPresenterViewIface::duplicateObj()
{
    view->duplicateObj();
}

void KPresenterViewIface::extraSendBackward()
{
    view->extraSendBackward();
}

void KPresenterViewIface::extraBringForward()
{
    view->extraBringForward();
}

void KPresenterViewIface::applyAutoFormat()
{
    view->applyAutoFormat();
}

void KPresenterViewIface::editDeSelectAll()
{
    view->editDeSelectAll();
}

void KPresenterViewIface::createStyleFromSelection()
{
    view->createStyleFromSelection();
}

void KPresenterViewIface::closeObject()
{
    view->closeObject();
}

void KPresenterViewIface::savePicture()
{
    view->savePicture();
}

// note: _nPage is the user visible 1-based page number
// if 0 < _verbose exportPage() returns the title and notes of the page
// if not verbose it returns an empty string
QStringList KPresenterViewIface::exportPage( int _nPage,
                                             int _nWidth,
                                             int _nHeight,
                                             const QString & _fileName,
                                             const QString & _format,
                                             int _quality,
                                             int _verbose )const
{
    QStringList res;
    // we translate the user visible 1-based page number
    // to KPresenter's internal 0-based page number
    const int nPage = _nPage-1;
    if( 0 <= nPage &&
        view &&
        view->kPresenterDoc() &&
        nPage < (int)view->kPresenterDoc()->getPageNums() ){
        KPrCanvas* canvas = view->getCanvas();
        if( canvas ){
            if( canvas->exportPage( nPage,
                                    QMAX(8, _nWidth),
                                    QMAX(8, _nHeight),
                                    KURL::fromPathOrURL( _fileName ),
                                    _format.isEmpty() ? "PNG" : _format.latin1(),
                                    QMAX(-1, QMIN(100, _quality))) ){
                if( 0 < _verbose ){
                    KPrPage* page = view->kPresenterDoc()->pageList().at( nPage );
                    if( page ){
                        // Note: Do not i18n the following strings, they are prepared
                        //       to be written to an IndeView page information file,
                        //       see http://www.indeview.org for details.
                        // Note: We use the 1-based page number as fallback page title.
                        res << QString("Name=%1")
                                .arg( page->pageTitle( QString("Page%1").arg(_nPage) ) );
                        res << QString("Notes=%1")
                                .arg( page->noteText() );
                    }
                }
            }
        }
    }
    return res;
}

void KPresenterViewIface::insertFile()
{
    view->insertFile();
}

void KPresenterViewIface::importStyle()
{
    view->importStyle();
}

void KPresenterViewIface::backgroundPicture()
{
    view->backgroundPicture();
}

void KPresenterViewIface::insertFile( const QString & file )
{
    view->insertFile( file );
}

void KPresenterViewIface::addWordToDictionary()
{
    view->addWordToDictionary();
}

void KPresenterViewIface::customSlideShow()
{
    view->customSlideShow();
}

void KPresenterViewIface::insertLineBreak()
{
    view->slotLineBreak();
}

void KPresenterViewIface::increaseOutlineLevel()
{
    view->slotIncreaseOutlineLevel();
}

void KPresenterViewIface::decreaseOutlineLevel()
{
    view->slotDecreaseOutlineLevel();
}
