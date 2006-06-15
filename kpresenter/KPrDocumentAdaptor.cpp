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

#include "KPrDocumentAdaptor.h"
#include "KPrPageAdaptor.h"
#include "KPrObjectAdaptor.h"
#include "KPrTextObject.h"
#include "KPrDocument.h"
#include "KPrView.h"
#include "KPrPage.h"

#include <kapplication.h>
#include <KoVariable.h>
#include "KPrVariableCollection.h"
#include <KoAutoFormat.h>

KPrDocumentAdaptor::KPrDocumentAdaptor( KPrDocument *doc_ )
    : KoDocumentAdaptor( doc_ )
{
    doc = doc_;
}

int KPrDocumentAdaptor::numPages() const
{
    return doc->getPageNums();
}

QString KPrDocumentAdaptor::page( int num )
{
    if( num>= (int )doc->getPageNums())
        return QString();
    return doc->pageList().at( num )->dbusObject()->objectName();
}

QString KPrDocumentAdaptor::masterPage()
{
    return doc->masterPage()->dbusObject()->objectName();
}

void KPrDocumentAdaptor::setShowRuler(bool b)
{
    doc->setShowRuler(b );
    doc->reorganizeGUI();
}

bool KPrDocumentAdaptor::showRuler() const
{
    return doc->showRuler();
}

void KPrDocumentAdaptor::recalcAllVariables()
{
    //recalc all variable
    doc->recalcVariables(VT_ALL);
}

void KPrDocumentAdaptor::recalcVariables(int _var)
{
    doc->recalcVariables(_var);
}

void KPrDocumentAdaptor::recalcVariables(const QString &varName)
{
    if(varName=="VT_DATE")
        doc->recalcVariables(0);
    else if(varName=="VT_TIME")
        doc->recalcVariables(2);
    else if(varName=="VT_PGNUM")
        doc->recalcVariables(4);
    else if(varName=="VT_CUSTOM")
        doc->recalcVariables(6);
    else if(varName=="VT_SERIALLETTER")
        doc->recalcVariables(7);
    else if(varName=="VT_FIELD")
        doc->recalcVariables(8);
    else if(varName=="VT_LINK")
        doc->recalcVariables(9);
    else if(varName=="VT_NOTE")
        doc->recalcVariables(10);
    else if(varName=="VT_ALL")
        doc->recalcVariables(256);
}

int KPrDocumentAdaptor::startingPage()
{
    return doc->getVariableCollection()->variableSetting()->startingPageNumber();
}

void KPrDocumentAdaptor::setStartingPage(int nb)
{
    doc->getVariableCollection()->variableSetting()->setStartingPageNumber(nb);
    doc->recalcVariables(VT_PGNUM);
}

bool KPrDocumentAdaptor::displayLink() const
{
    return doc->getVariableCollection()->variableSetting()->displayLink();
}

void KPrDocumentAdaptor::setDisplayLink(bool b)
{
    doc->getVariableCollection()->variableSetting()->setDisplayLink(b);
    doc->recalcVariables(VT_LINK);
}

bool KPrDocumentAdaptor::setCustomVariableValue(const QString & varname, const QString & value)
{
    bool exist=doc->getVariableCollection()->customVariableExist(varname);
    if(exist)
    {
        doc->getVariableCollection()->setVariableValue( varname, value );
        doc->recalcVariables(VT_CUSTOM);
    }
    else
        return false;
    return true;
}

QString KPrDocumentAdaptor::customVariableValue(const QString & varname)const
{
    if(doc->getVariableCollection()->customVariableExist(varname))
        return doc->getVariableCollection()->getVariableValue( varname );
    return QString::null;
}

bool KPrDocumentAdaptor::insertNewPage(int pos )
{
    if( pos < 0 || pos > (int)(doc->getPageNums())-1 )
        pos=doc->getPageNums()-1;
    int ret= doc->insertNewPage( i18n("Insert New Slide"), pos, IP_AFTER, false, QString::null );
    bool state = (ret !=-1);
    return state;
}

//return false if page number doesn't exist
bool KPrDocumentAdaptor::selectPage( int page,bool select)
{
    if(page <0 || page> (int)(doc->getPageNums())-1 )
        return false;
    doc->selectPage( page, select );
    return true;
}

// return false when we can't remove page
bool KPrDocumentAdaptor::deletePage( int _page )
{
    if( _page < 0 || _page > (int)(doc->getPageNums())-1 )
        return false;
    doc->deletePage(_page);
    return true;
}

void KPrDocumentAdaptor::deSelectAllObj()
{
    doc->deSelectAllObj();
}

void KPrDocumentAdaptor::recalcPageNum()
{
    doc->recalcPageNum();
}


void KPrDocumentAdaptor::initConfig()
{
    doc->initConfig();
}

void KPrDocumentAdaptor::saveConfig()
{
    doc->saveConfig();
}

QString KPrDocumentAdaptor::selectedForPrinting()const
{
    return doc->selectedForPrinting();
}

bool KPrDocumentAdaptor::isSlideSelected( int pgNum)
{
    if( pgNum>= (int)doc->getPageNums())
        return false;
    return doc->isSlideSelected(pgNum);
}

//Return a reference to header textobj
QString KPrDocumentAdaptor::header()
{
    if(doc->header())
        return doc->header()->dbusObject()->objectName();
    else
        return QString();
}

//Return a reference to footer textobj
QString KPrDocumentAdaptor::footer()
{
    if(doc->footer())
        return doc->footer()->dbusObject()->objectName();
    else
        return QString();
}

void KPrDocumentAdaptor::startBackgroundSpellCheck()
{
    doc->startBackgroundSpellCheck();
}

void KPrDocumentAdaptor::reactivateBgSpellChecking()
{
    doc->reactivateBgSpellChecking();
}

void KPrDocumentAdaptor::setConfigUpperCase( bool _uc )
{
    doc->getAutoFormat()->configUpperCase(_uc);
}

void KPrDocumentAdaptor::setConfigUpperUpper( bool _uu )
{
    doc->getAutoFormat()->configUpperUpper(_uu);
}

void KPrDocumentAdaptor::setConfigAdvancedAutocorrect( bool _aa )
{
    doc->getAutoFormat()->configAdvancedAutocorrect( _aa );
}

void KPrDocumentAdaptor::setConfigAutoDetectUrl(bool _au)
{
    doc->getAutoFormat()->configAutoDetectUrl(_au);
}

void KPrDocumentAdaptor::setConfigIgnoreDoubleSpace( bool _ids)
{
    doc->getAutoFormat()->configIgnoreDoubleSpace(_ids);
}

void KPrDocumentAdaptor::setConfigRemoveSpaceBeginEndLine( bool _space)
{
    doc->getAutoFormat()->configRemoveSpaceBeginEndLine(_space);
}

void KPrDocumentAdaptor::setConfigUseBulletStyle( bool _ubs)
{
    doc->getAutoFormat()->configUseBulletStyle(_ubs);
}

bool KPrDocumentAdaptor::configUpperCase() const
{
    return doc->getAutoFormat()->getConfigUpperCase();
}

bool KPrDocumentAdaptor::configUpperUpper() const
{
    return doc->getAutoFormat()->getConfigUpperUpper();
}

bool KPrDocumentAdaptor::configAdvancedAutoCorrect() const
{
    return doc->getAutoFormat()->getConfigAdvancedAutoCorrect();
}

bool KPrDocumentAdaptor::configAutoDetectUrl() const
{
    return doc->getAutoFormat()->getConfigAutoDetectUrl();
}

bool KPrDocumentAdaptor::configIgnoreDoubleSpace() const
{
    return doc->getAutoFormat()->getConfigIgnoreDoubleSpace();
}

bool KPrDocumentAdaptor::configRemoveSpaceBeginEndLine() const
{
    return doc->getAutoFormat()->getConfigIgnoreDoubleSpace();
}

bool KPrDocumentAdaptor::configUseBulletSyle() const
{
    return doc->getAutoFormat()->getConfigUseBulletSyle();
}

bool KPrDocumentAdaptor::configAutoChangeFormat() const
{
    return doc->getAutoFormat()->getConfigAutoChangeFormat();
}

void KPrDocumentAdaptor::setConfigAutoChangeFormat( bool _auto)
{
    doc->getAutoFormat()->configAutoChangeFormat(_auto);
}

bool KPrDocumentAdaptor::configAutoReplaceNumber() const
{
    return doc->getAutoFormat()->getConfigAutoReplaceNumber();
}

void KPrDocumentAdaptor::setConfigAutoReplaceNumber( bool b )
{
    doc->getAutoFormat()->configAutoReplaceNumber(b);
}

bool KPrDocumentAdaptor::showStatusBar() const
{
    return doc->showStatusBar();
}

void KPrDocumentAdaptor::setShowStatusBar( bool _status )
{
    doc->setShowStatusBar(_status);
    doc->reorganizeGUI();
}

void KPrDocumentAdaptor::setConfigAutoNumberStyle( bool b )
{
    doc->getAutoFormat()->configAutoNumberStyle(b);
}

bool KPrDocumentAdaptor::configAutoNumberStyle() const
{
    return doc->getAutoFormat()->getConfigAutoNumberStyle();
}

void KPrDocumentAdaptor::setConfigCompletion( bool b )
{
    doc->getAutoFormat()->configCompletion( b );
}

bool KPrDocumentAdaptor::configCompletion() const
{
    return doc->getAutoFormat()->getConfigCompletion();
}

void KPrDocumentAdaptor::setConfigAppendSpace( bool b)
{
    doc->getAutoFormat()->configAppendSpace( b );
}

bool KPrDocumentAdaptor::configAppendSpace() const
{
    return doc->getAutoFormat()->getConfigAppendSpace();
}

void KPrDocumentAdaptor::setConfigMinWordLength( uint val )
{
    doc->getAutoFormat()->configMinWordLength( val );
}

uint KPrDocumentAdaptor::configMinWordLength() const
{
    return doc->getAutoFormat()->getConfigMinWordLength();
}

void KPrDocumentAdaptor::setConfigNbMaxCompletionWord( uint val )
{
    doc->getAutoFormat()->configNbMaxCompletionWord( val );
}

uint KPrDocumentAdaptor::configNbMaxCompletionWord() const
{
    return doc->getAutoFormat()->getConfigNbMaxCompletionWord();
}

void KPrDocumentAdaptor::setConfigAddCompletionWord( bool b )
{
    doc->getAutoFormat()->configAddCompletionWord( b );
}

bool KPrDocumentAdaptor::configAddCompletionWord() const
{
    return doc->getAutoFormat()->getConfigAddCompletionWord();
}

bool KPrDocumentAdaptor::configIncludeTwoUpperUpperLetterException() const
{
    return doc->getAutoFormat()->getConfigIncludeTwoUpperUpperLetterException();
}

void KPrDocumentAdaptor::setConfigIncludeTwoUpperUpperLetterException( bool b)
{
    doc->getAutoFormat()->configIncludeTwoUpperUpperLetterException( b );
}

bool KPrDocumentAdaptor::configIncludeAbbreviation() const
{
    return doc->getAutoFormat()->getConfigIncludeAbbreviation();
}

void KPrDocumentAdaptor::setConfigIncludeAbbreviation( bool b)
{
    doc->getAutoFormat()->configIncludeAbbreviation( b );
}

bool KPrDocumentAdaptor::displayComment() const
{
    return doc->getVariableCollection()->variableSetting()->displayComment();
}

void KPrDocumentAdaptor::setDisplayComment( bool b)
{
    doc->getVariableCollection()->variableSetting()->setDisplayComment( b );
    doc->recalcVariables(VT_NOTE);
}

bool KPrDocumentAdaptor::showGuideLines() const
{
    return doc->showGuideLines();
}

void KPrDocumentAdaptor::setShowGuideLines( bool b )
{
    doc->setShowGuideLines( b );
    doc->updateGuideLineButton();
    doc->repaint( false );
}

void KPrDocumentAdaptor::addGuideLine( bool horizontal, double pos )
{
    doc->addGuideLine( horizontal ? Qt::Horizontal: Qt::Vertical, pos );
    doc->repaint( false );
}

unsigned int KPrDocumentAdaptor::nbHorizontalHelpLine() const
{
    return doc->horizontalGuideLines().count();
}

unsigned int KPrDocumentAdaptor::nbVerticalHelpLine() const
{
    return doc->verticalGuideLines().count();
}

bool KPrDocumentAdaptor::showGrid() const
{
    return doc->showGrid();
}

void KPrDocumentAdaptor::setShowGrid ( bool _grid )
{
    doc->setShowGrid( _grid);
    doc->updateGridButton();
    doc->repaint( false );
}

double KPrDocumentAdaptor::gridX() const
{
    return doc->getGridX();
}

void KPrDocumentAdaptor::setGridX(double _x)
{
    doc->setGridX( _x );
    if( showGrid() )
        doc->repaint( false );
}

double KPrDocumentAdaptor::gridY() const
{
    return doc->getGridY();
}

void KPrDocumentAdaptor::setGridY(double _y)
{
    doc->setGridY( _y );
    if( showGrid() )
        doc->repaint( false );
}

bool KPrDocumentAdaptor::configAutoSuperScript() const
{
    return doc->getAutoFormat()->getConfigAutoSuperScript();
}

void KPrDocumentAdaptor::setConfigAutoSuperScript( bool b)
{
    doc->getAutoFormat()->configAutoSuperScript( b );
}

void KPrDocumentAdaptor::addIgnoreWordAll( const QString &word)
{
    doc->addSpellCheckIgnoreWord( word );
}

void KPrDocumentAdaptor::clearIgnoreWordAll( )
{
    doc->setSpellCheckIgnoreList( QStringList() );
}

QStringList KPrDocumentAdaptor::spellListIgnoreAll() const
{
    return doc->spellCheckIgnoreList();
}

bool KPrDocumentAdaptor::displayFieldCode()const
{
    return doc->getVariableCollection()->variableSetting()->displayFieldCode();
}

void KPrDocumentAdaptor::setDisplayFieldCode( bool b)
{
    doc->getVariableCollection()->variableSetting()->setDisplayFieldCode( b );
    doc->recalcVariables(VT_ALL);
}

QString KPrDocumentAdaptor::configAutoFormatLanguage( )const
{
    return doc->getAutoFormat()->getConfigAutoFormatLanguage( );
}


bool KPrDocumentAdaptor::configCapitalizeNameOfDays() const
{
    return doc->getAutoFormat()->getConfigCapitalizeNameOfDays();
}

void KPrDocumentAdaptor::setConfigCapitalizeNameOfDays( bool b)
{
    doc->getAutoFormat()->configCapitalizeNameOfDays( b );
}

QString KPrDocumentAdaptor::presentationName() const
{
    return doc->presentationName();
}

void KPrDocumentAdaptor::setPresentationName( const QString &_name )
{
    doc->setPresentationName( _name );
}


QStringList KPrDocumentAdaptor::presentationList()
{
    return doc->presentationList();
}

void KPrDocumentAdaptor::repaint()
{
    doc->repaint( false );
}

void KPrDocumentAdaptor::setConfigToolTipCompletion( bool b )
{
    doc->getAutoFormat()->configToolTipCompletion( b );
}

bool KPrDocumentAdaptor::configToolTipCompletion() const
{
    return doc->getAutoFormat()->getConfigToolTipCompletion();
}
