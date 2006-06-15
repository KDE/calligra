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

#include "KPrViewAdaptor.h"

#include "KPrView.h"
#include "KPrCanvas.h"
#include "KPrPage.h"
#include "KPrDocument.h"

#include <kapplication.h>
#include <dcopclient.h>

KPrViewAdaptor::KPrViewAdaptor( KPrView *view_ )
    : KoViewAdaptor( view_ )
{
    view = view_;
}

void KPrViewAdaptor::editCut()
{
    view->editCut();
}

void KPrViewAdaptor::editCopy()
{
    view->editCopy();
}

void KPrViewAdaptor::editPaste()
{
    view->editPaste();
}

void KPrViewAdaptor::editDelete()
{
    view->editDelete();
}

void KPrViewAdaptor::editSelectAll()
{
    view->editSelectAll();
}

void KPrViewAdaptor::editDuplicatePage()
{
    view->editDuplicatePage();
}

void KPrViewAdaptor::editDelPage()
{
    view->editDelPage();
}

void KPrViewAdaptor::editFind()
{
    view->editFind();
}

void KPrViewAdaptor::editReplace()
{
    view->editReplace();
}

void KPrViewAdaptor::newView()
{
    view->newView();
}

void KPrViewAdaptor::insertPage()
{
    view->insertPage();
}

void KPrViewAdaptor::insertPicture()
{
    view->insertPicture();
}

void KPrViewAdaptor::toolsMouse()
{
    view->toolsMouse();
}

void KPrViewAdaptor::toolsLine()
{
    view->toolsLine();
}

void KPrViewAdaptor::toolsRectangle()
{
    view->toolsRectangle();
}

void KPrViewAdaptor::toolsCircleOrEllipse()
{
    view->toolsCircleOrEllipse();
}

void KPrViewAdaptor::toolsPie()
{
    view->toolsPie();
}

void KPrViewAdaptor::toolsText()
{
    view->toolsText();
}

void KPrViewAdaptor::toolsAutoform()
{
    view->toolsAutoform();
}

void KPrViewAdaptor::toolsDiagramm()
{
    view->toolsDiagramm();
}

void KPrViewAdaptor::toolsTable()
{
    view->toolsTable();
}

void KPrViewAdaptor::toolsFormula()
{
    view->toolsFormula();
}

void KPrViewAdaptor::toolsObject()
{
    view->toolsObject();
}

void KPrViewAdaptor::toolsFreehand()
{
    view->toolsFreehand();
}

void KPrViewAdaptor::toolsPolyline()
{
    view->toolsPolyline();
}

void KPrViewAdaptor::toolsQuadricBezierCurve()
{
    view->toolsQuadricBezierCurve();
}

void KPrViewAdaptor::toolsCubicBezierCurve()
{
    view->toolsCubicBezierCurve();
}

void KPrViewAdaptor::toolsConvexOrConcavePolygon()
{
    view->toolsConvexOrConcavePolygon();
}

void KPrViewAdaptor::extraPenBrush()
{
    view->extraProperties();
}

void KPrViewAdaptor::extraProperties()
{
    view->extraProperties();
}

void KPrViewAdaptor::extraRaise()
{
    view->extraRaise();
}

void KPrViewAdaptor::extraLower()
{
    view->extraLower();
}

void KPrViewAdaptor::extraRotate()
{
    view->extraRotate();
}

void KPrViewAdaptor::extraShadow()
{
    view->extraShadow();
}

void KPrViewAdaptor::extraBackground()
{
    view->extraBackground();
}

void KPrViewAdaptor::extraLayout()
{
    view->extraLayout();
}

void KPrViewAdaptor::extraConfigure()
{
    view->extraConfigure();
}

void KPrViewAdaptor::extraLineBegin()
{
    view->extraLineBegin();
}

void KPrViewAdaptor::extraLineEnd()
{
    view->extraLineEnd();
}

void KPrViewAdaptor::extraWebPres()
{
    view->extraWebPres();
}

void KPrViewAdaptor::extraMSPres()
{
    view->extraMSPres();
}

void KPrViewAdaptor::extraAlignObjLeft()
{
    view->extraAlignObjLeft();
}

void KPrViewAdaptor::extraAlignObjCenterH()
{
    view->extraAlignObjCenterH();
}

void KPrViewAdaptor::extraAlignObjRight()
{
    view->extraAlignObjRight();
}

void KPrViewAdaptor::extraAlignObjTop()
{
    view->extraAlignObjTop();
}

void KPrViewAdaptor::extraAlignObjCenterV()
{
    view->extraAlignObjCenterV();
}

void KPrViewAdaptor::extraAlignObjBottom()
{
    view->extraAlignObjBottom();
}

void KPrViewAdaptor::extraGroup()
{
    view->extraGroup();
}

void KPrViewAdaptor::extraUnGroup()
{
    view->extraUnGroup();
}

void KPrViewAdaptor::extraPenStyle()
{
// FIXME
//     view->extraPenStyle();
}

void KPrViewAdaptor::extraPenWidth()
{
// FIXME
//     view->extraPenWidth();
}

void KPrViewAdaptor::screenTransEffect()
{
    view->screenTransEffect();
}

void KPrViewAdaptor::screenConfigPages()
{
    view->screenConfigPages();
}

void KPrViewAdaptor::screenAssignEffect()
{
    view->screenAssignEffect();
}

void KPrViewAdaptor::screenStart()
{
    view->screenStart();
}

void KPrViewAdaptor::screenStartFromFirst()
{
    view->screenStartFromFirst();
}

void KPrViewAdaptor::screenStop()
{
    view->screenStop();
}

void KPrViewAdaptor::screenPause()
{
    view->screenPause();
}

void KPrViewAdaptor::screenFirst()
{
    view->screenFirst();
}

void KPrViewAdaptor::screenPrev()
{
    view->screenPrev();
}

void KPrViewAdaptor::screenNext()
{
    view->screenNext();
}

void KPrViewAdaptor::screenLast()
{
    view->screenLast();
}

void KPrViewAdaptor::screenSkip()
{
    view->screenSkip();
}

void KPrViewAdaptor::sizeSelected( int size )
{
    view->sizeSelected( size );
}

void KPrViewAdaptor::fontSelected( const QString &fontFamily )
{
    view->fontSelected( fontFamily );
}

void KPrViewAdaptor::textBold()
{
    view->textBold();
}

void KPrViewAdaptor::textItalic()
{
    view->textItalic();
}

void KPrViewAdaptor::textUnderline()
{
    view->textUnderline();
}

void KPrViewAdaptor::textColor()
{
    view->textColor();
}

void KPrViewAdaptor::textAlignLeft()
{
    view->textAlignLeft();
}

void KPrViewAdaptor::textAlignCenter()
{
    view->textAlignCenter();
}

void KPrViewAdaptor::textAlignRight()
{
    view->textAlignRight();
}

void KPrViewAdaptor::mtextFont()
{
    view->mtextFont();
}

void KPrViewAdaptor::textDepthPlus()
{
    view->textDepthPlus();
}

void KPrViewAdaptor::textDepthMinus()
{
    view->textDepthMinus();
}

void KPrViewAdaptor::textContentsToHeight()
{
    view->textContentsToHeight();
}

void KPrViewAdaptor::textObjectToContents()
{
    view->textObjectToContents();
}

void KPrViewAdaptor::penChosen()
{
    view->penChosen();
}

void KPrViewAdaptor::brushChosen()
{
    view->brushChosen();
}

int KPrViewAdaptor::getCurrentPresPage() const
{
    return view->getCurrentPresPage();
}

int KPrViewAdaptor::getCurrentPresStep() const
{
    return view->getCurrentPresStep();
}

int KPrViewAdaptor::getPresStepsOfPage() const
{
    return view->getPresStepsOfPage();
}

int KPrViewAdaptor::getNumPresPages() const
{
    return view->getNumPresPages();
}

bool KPrViewAdaptor::gotoPresPage( int pg ) const
{
    return view->gotoPresPage( pg );
}

int KPrViewAdaptor::getCurrentPageNum() const
{
    return view->getCurrPgNum();
}

void KPrViewAdaptor::insertSpecialChar()
{
    view->insertSpecialChar();
}

void KPrViewAdaptor::insertLink()
{
    view->insertLink();
}

void KPrViewAdaptor::editCustomVars()
{
    view->editCustomVars();
}

void KPrViewAdaptor::insertVariable()
{
    view->insertVariable();
}

void KPrViewAdaptor::insertCustomVariable()
{
    view->insertCustomVariable();
}

void KPrViewAdaptor::insertNewCustomVariable()
{
    view->insertNewCustomVariable();
}

void KPrViewAdaptor::changeCaseOfText()
{
    view->changeCaseOfText();
}

void KPrViewAdaptor::viewShowSideBar()
{
    view->viewShowSideBar();
}

void KPrViewAdaptor::viewShowNoteBar()
{
    view->viewShowNoteBar();
}

void KPrViewAdaptor::viewSlideMaster()
{
    view->viewSlideMaster();
}


void KPrViewAdaptor::formatParagraph()
{
    view->formatParagraph();
}

void KPrViewAdaptor::extraAutoFormat()
{
    view->extraAutoFormat();
}

void KPrViewAdaptor::refreshAllVariable()
{
    view->extraAutoFormat();
}

bool KPrViewAdaptor::skipToPage( int _num )
{
    //don't try to skip to a not exist page
    if(_num>(int)view->kPresenterDoc()->getPageNums()-1)
        return false;
    view->skipToPage(_num);
    return true;
}

void KPrViewAdaptor::toolsRotate()
{
    view->toolsRotate();
}

void KPrViewAdaptor::toolsZoom()
{
    view->toolsZoom();
}

void KPrViewAdaptor::insertComment()
{
    view->insertComment();
}

void KPrViewAdaptor::configureCompletion()
{
    view->configureCompletion();
}

void KPrViewAdaptor::duplicateObj()
{
    view->duplicateObj();
}

void KPrViewAdaptor::extraSendBackward()
{
    view->extraSendBackward();
}

void KPrViewAdaptor::extraBringForward()
{
    view->extraBringForward();
}

void KPrViewAdaptor::applyAutoFormat()
{
    view->applyAutoFormat();
}

void KPrViewAdaptor::editDeSelectAll()
{
    view->editDeSelectAll();
}

void KPrViewAdaptor::createStyleFromSelection()
{
    view->createStyleFromSelection();
}

void KPrViewAdaptor::closeObject()
{
    view->closeObject();
}

void KPrViewAdaptor::savePicture()
{
    view->savePicture();
}

void KPrViewAdaptor::viewFooter()
{
    view->viewFooter();
}

void KPrViewAdaptor::viewHeader()
{
    view->viewHeader();
}


// note: _nPage is the user visible 1-based page number
// if 0 < _verbose exportPage() returns the title and notes of the page
// if not verbose it returns an empty string
QStringList KPrViewAdaptor::exportPage( int _nPage,
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

void KPrViewAdaptor::insertFile()
{
    view->insertFile();
}

void KPrViewAdaptor::importStyle()
{
    view->importStyle();
}

void KPrViewAdaptor::backgroundPicture()
{
    view->backgroundPicture();
}

void KPrViewAdaptor::insertFile( const QString & file )
{
    view->insertFile( file );
}

void KPrViewAdaptor::addWordToDictionary()
{
    view->addWordToDictionary();
}

void KPrViewAdaptor::customSlideShow()
{
    view->customSlideShow();
}

void KPrViewAdaptor::insertLineBreak()
{
    view->slotLineBreak();
}

void KPrViewAdaptor::increaseNumberingLevel()
{
    view->slotIncreaseNumberingLevel();
}

void KPrViewAdaptor::decreaseNumberingLevel()
{
    view->slotDecreaseNumberingLevel();
}

void KPrViewAdaptor::increaseFontSize()
{
    view->increaseFontSize();
}

void KPrViewAdaptor::decreaseFontSize()
{
    view->decreaseFontSize();
}

void KPrViewAdaptor::flipHorizontal()
{
    view->flipHorizontal();
}

void KPrViewAdaptor::flipVertical()
{
    view->flipVertical();
}
