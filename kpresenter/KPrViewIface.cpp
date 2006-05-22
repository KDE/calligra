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

#include "KPrViewIface.h"

#include "KPrView.h"
#include "KPrCanvas.h"
#include "KPrPage.h"
#include "KPrDocument.h"

#include <kapplication.h>
#include <dcopclient.h>

KPrViewIface::KPrViewIface( KPrView *view_ )
    : KoViewIface( view_ )
{
    view = view_;
}

void KPrViewIface::editCut()
{
    view->editCut();
}

void KPrViewIface::editCopy()
{
    view->editCopy();
}

void KPrViewIface::editPaste()
{
    view->editPaste();
}

void KPrViewIface::editDelete()
{
    view->editDelete();
}

void KPrViewIface::editSelectAll()
{
    view->editSelectAll();
}

void KPrViewIface::editDuplicatePage()
{
    view->editDuplicatePage();
}

void KPrViewIface::editDelPage()
{
    view->editDelPage();
}

void KPrViewIface::editFind()
{
    view->editFind();
}

void KPrViewIface::editReplace()
{
    view->editReplace();
}

void KPrViewIface::newView()
{
    view->newView();
}

void KPrViewIface::insertPage()
{
    view->insertPage();
}

void KPrViewIface::insertPicture()
{
    view->insertPicture();
}

void KPrViewIface::toolsMouse()
{
    view->toolsMouse();
}

void KPrViewIface::toolsLine()
{
    view->toolsLine();
}

void KPrViewIface::toolsRectangle()
{
    view->toolsRectangle();
}

void KPrViewIface::toolsCircleOrEllipse()
{
    view->toolsCircleOrEllipse();
}

void KPrViewIface::toolsPie()
{
    view->toolsPie();
}

void KPrViewIface::toolsText()
{
    view->toolsText();
}

void KPrViewIface::toolsAutoform()
{
    view->toolsAutoform();
}

void KPrViewIface::toolsDiagramm()
{
    view->toolsDiagramm();
}

void KPrViewIface::toolsTable()
{
    view->toolsTable();
}

void KPrViewIface::toolsFormula()
{
    view->toolsFormula();
}

void KPrViewIface::toolsObject()
{
    view->toolsObject();
}

void KPrViewIface::toolsFreehand()
{
    view->toolsFreehand();
}

void KPrViewIface::toolsPolyline()
{
    view->toolsPolyline();
}

void KPrViewIface::toolsQuadricBezierCurve()
{
    view->toolsQuadricBezierCurve();
}

void KPrViewIface::toolsCubicBezierCurve()
{
    view->toolsCubicBezierCurve();
}

void KPrViewIface::toolsConvexOrConcavePolygon()
{
    view->toolsConvexOrConcavePolygon();
}

void KPrViewIface::extraPenBrush()
{
    view->extraProperties();
}

void KPrViewIface::extraProperties()
{
    view->extraProperties();
}

void KPrViewIface::extraRaise()
{
    view->extraRaise();
}

void KPrViewIface::extraLower()
{
    view->extraLower();
}

void KPrViewIface::extraRotate()
{
    view->extraRotate();
}

void KPrViewIface::extraShadow()
{
    view->extraShadow();
}

void KPrViewIface::extraBackground()
{
    view->extraBackground();
}

void KPrViewIface::extraLayout()
{
    view->extraLayout();
}

void KPrViewIface::extraConfigure()
{
    view->extraConfigure();
}

void KPrViewIface::extraLineBegin()
{
    view->extraLineBegin();
}

void KPrViewIface::extraLineEnd()
{
    view->extraLineEnd();
}

void KPrViewIface::extraWebPres()
{
    view->extraWebPres();
}

void KPrViewIface::extraMSPres()
{
    view->extraMSPres();
}

void KPrViewIface::extraAlignObjLeft()
{
    view->extraAlignObjLeft();
}

void KPrViewIface::extraAlignObjCenterH()
{
    view->extraAlignObjCenterH();
}

void KPrViewIface::extraAlignObjRight()
{
    view->extraAlignObjRight();
}

void KPrViewIface::extraAlignObjTop()
{
    view->extraAlignObjTop();
}

void KPrViewIface::extraAlignObjCenterV()
{
    view->extraAlignObjCenterV();
}

void KPrViewIface::extraAlignObjBottom()
{
    view->extraAlignObjBottom();
}

void KPrViewIface::extraGroup()
{
    view->extraGroup();
}

void KPrViewIface::extraUnGroup()
{
    view->extraUnGroup();
}

void KPrViewIface::extraPenStyle()
{
// FIXME
//     view->extraPenStyle();
}

void KPrViewIface::extraPenWidth()
{
// FIXME
//     view->extraPenWidth();
}

void KPrViewIface::screenTransEffect()
{
    view->screenTransEffect();
}

void KPrViewIface::screenConfigPages()
{
    view->screenConfigPages();
}

void KPrViewIface::screenAssignEffect()
{
    view->screenAssignEffect();
}

void KPrViewIface::screenStart()
{
    view->screenStart();
}

void KPrViewIface::screenStartFromFirst()
{
    view->screenStartFromFirst();
}

void KPrViewIface::screenStop()
{
    view->screenStop();
}

void KPrViewIface::screenPause()
{
    view->screenPause();
}

void KPrViewIface::screenFirst()
{
    view->screenFirst();
}

void KPrViewIface::screenPrev()
{
    view->screenPrev();
}

void KPrViewIface::screenNext()
{
    view->screenNext();
}

void KPrViewIface::screenLast()
{
    view->screenLast();
}

void KPrViewIface::screenSkip()
{
    view->screenSkip();
}

void KPrViewIface::sizeSelected( int size )
{
    view->sizeSelected( size );
}

void KPrViewIface::fontSelected( const QString &fontFamily )
{
    view->fontSelected( fontFamily );
}

void KPrViewIface::textBold()
{
    view->textBold();
}

void KPrViewIface::textItalic()
{
    view->textItalic();
}

void KPrViewIface::textUnderline()
{
    view->textUnderline();
}

void KPrViewIface::textColor()
{
    view->textColor();
}

void KPrViewIface::textAlignLeft()
{
    view->textAlignLeft();
}

void KPrViewIface::textAlignCenter()
{
    view->textAlignCenter();
}

void KPrViewIface::textAlignRight()
{
    view->textAlignRight();
}

void KPrViewIface::mtextFont()
{
    view->mtextFont();
}

void KPrViewIface::textDepthPlus()
{
    view->textDepthPlus();
}

void KPrViewIface::textDepthMinus()
{
    view->textDepthMinus();
}

void KPrViewIface::textContentsToHeight()
{
    view->textContentsToHeight();
}

void KPrViewIface::textObjectToContents()
{
    view->textObjectToContents();
}

void KPrViewIface::penChosen()
{
    view->penChosen();
}

void KPrViewIface::brushChosen()
{
    view->brushChosen();
}

int KPrViewIface::getCurrentPresPage() const
{
    return view->getCurrentPresPage();
}

int KPrViewIface::getCurrentPresStep() const
{
    return view->getCurrentPresStep();
}

int KPrViewIface::getPresStepsOfPage() const
{
    return view->getPresStepsOfPage();
}

int KPrViewIface::getNumPresPages() const
{
    return view->getNumPresPages();
}

bool KPrViewIface::gotoPresPage( int pg ) const
{
    return view->gotoPresPage( pg );
}

int KPrViewIface::getCurrentPageNum() const
{
    return view->getCurrPgNum();
}

void KPrViewIface::insertSpecialChar()
{
    view->insertSpecialChar();
}

void KPrViewIface::insertLink()
{
    view->insertLink();
}

void KPrViewIface::editCustomVars()
{
    view->editCustomVars();
}

void KPrViewIface::insertVariable()
{
    view->insertVariable();
}

void KPrViewIface::insertCustomVariable()
{
    view->insertCustomVariable();
}

void KPrViewIface::insertNewCustomVariable()
{
    view->insertNewCustomVariable();
}

void KPrViewIface::changeCaseOfText()
{
    view->changeCaseOfText();
}

void KPrViewIface::viewShowSideBar()
{
    view->viewShowSideBar();
}

void KPrViewIface::viewShowNoteBar()
{
    view->viewShowNoteBar();
}

void KPrViewIface::viewSlideMaster()
{
    view->viewSlideMaster();
}


void KPrViewIface::formatParagraph()
{
    view->formatParagraph();
}

void KPrViewIface::extraAutoFormat()
{
    view->extraAutoFormat();
}

void KPrViewIface::refreshAllVariable()
{
    view->extraAutoFormat();
}

bool KPrViewIface::skipToPage( int _num )
{
    //don't try to skip to a not exist page
    if(_num>(int)view->kPresenterDoc()->getPageNums()-1)
        return false;
    view->skipToPage(_num);
    return true;
}

void KPrViewIface::toolsRotate()
{
    view->toolsRotate();
}

void KPrViewIface::toolsZoom()
{
    view->toolsZoom();
}

void KPrViewIface::insertComment()
{
    view->insertComment();
}

void KPrViewIface::configureCompletion()
{
    view->configureCompletion();
}

void KPrViewIface::duplicateObj()
{
    view->duplicateObj();
}

void KPrViewIface::extraSendBackward()
{
    view->extraSendBackward();
}

void KPrViewIface::extraBringForward()
{
    view->extraBringForward();
}

void KPrViewIface::applyAutoFormat()
{
    view->applyAutoFormat();
}

void KPrViewIface::editDeSelectAll()
{
    view->editDeSelectAll();
}

void KPrViewIface::createStyleFromSelection()
{
    view->createStyleFromSelection();
}

void KPrViewIface::closeObject()
{
    view->closeObject();
}

void KPrViewIface::savePicture()
{
    view->savePicture();
}

void KPrViewIface::viewFooter()
{
    view->viewFooter();
}

void KPrViewIface::viewHeader()
{
    view->viewHeader();
}


// note: _nPage is the user visible 1-based page number
// if 0 < _verbose exportPage() returns the title and notes of the page
// if not verbose it returns an empty string
QStringList KPrViewIface::exportPage( int _nPage,
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
                                    qMax(8, _nWidth),
                                    qMax(8, _nHeight),
                                    KUrl::fromPathOrUrl( _fileName ),
                                    _format.isEmpty() ? "PNG" : _format.toLatin1(),
                                    qMax(-1, qMin(100, _quality))) ){
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

void KPrViewIface::insertFile()
{
    view->insertFile();
}

void KPrViewIface::importStyle()
{
    view->importStyle();
}

void KPrViewIface::backgroundPicture()
{
    view->backgroundPicture();
}

void KPrViewIface::insertFile( const QString & file )
{
    view->insertFile( file );
}

void KPrViewIface::addWordToDictionary()
{
    view->addWordToDictionary();
}

void KPrViewIface::customSlideShow()
{
    view->customSlideShow();
}

void KPrViewIface::insertLineBreak()
{
    view->slotLineBreak();
}

void KPrViewIface::increaseNumberingLevel()
{
    view->slotIncreaseNumberingLevel();
}

void KPrViewIface::decreaseNumberingLevel()
{
    view->slotDecreaseNumberingLevel();
}

void KPrViewIface::increaseFontSize()
{
    view->increaseFontSize();
}

void KPrViewIface::decreaseFontSize()
{
    view->decreaseFontSize();
}

void KPrViewIface::flipHorizontal()
{
    view->flipHorizontal();
}

void KPrViewIface::flipVertical()
{
    view->flipVertical();
}
