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

#include "KPrDocumentIface.h"
#include "KPrTextObject.h"
#include "KPrDocument.h"
#include "KPrView.h"
#include "KPrPage.h"

#include <kapplication.h>
#include <dcopclient.h>
#include <KoVariable.h>
#include "KPrVariableCollection.h"
#include <KoAutoFormat.h>

KPrDocumentIface::KPrDocumentIface( KPrDocument *doc_ )
    : KoDocumentIface( doc_ )
{
    doc = doc_;
}

int KPrDocumentIface::numPages() const
{
    return doc->getPageNums();
}

DCOPRef KPrDocumentIface::page( int num )
{
    if( num>= (int )doc->getPageNums())
        return DCOPRef();
    return DCOPRef( kapp->dcopClient()->appId(),
                    doc->pageList().at( num )->dcopObject()->objId() );
}

DCOPRef KPrDocumentIface::masterPage()
{
    return DCOPRef( kapp->dcopClient()->appId(),
                    doc->masterPage()->dcopObject()->objId() );
}

void KPrDocumentIface::setShowRuler(bool b)
{
    doc->setShowRuler(b );
    doc->reorganizeGUI();
}

bool KPrDocumentIface::showRuler() const
{
    return doc->showRuler();
}

void KPrDocumentIface::recalcAllVariables()
{
    //recalc all variable
    doc->recalcVariables(VT_ALL);
}

void KPrDocumentIface::recalcVariables(int _var)
{
    doc->recalcVariables(_var);
}

void KPrDocumentIface::recalcVariables(const QString &varName)
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

int KPrDocumentIface::startingPage()
{
    return doc->getVariableCollection()->variableSetting()->startingPageNumber();
}

void KPrDocumentIface::setStartingPage(int nb)
{
    doc->getVariableCollection()->variableSetting()->setStartingPageNumber(nb);
    doc->recalcVariables(VT_PGNUM);
}

bool KPrDocumentIface::displayLink() const
{
    return doc->getVariableCollection()->variableSetting()->displayLink();
}

void KPrDocumentIface::setDisplayLink(bool b)
{
    doc->getVariableCollection()->variableSetting()->setDisplayLink(b);
    doc->recalcVariables(VT_LINK);
}

bool KPrDocumentIface::setCustomVariableValue(const QString & varname, const QString & value)
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

QString KPrDocumentIface::customVariableValue(const QString & varname)const
{
    if(doc->getVariableCollection()->customVariableExist(varname))
        return doc->getVariableCollection()->getVariableValue( varname );
    return QString::null;
}

bool KPrDocumentIface::insertNewPage(int pos )
{
    if( pos < 0 || pos > (int)(doc->getPageNums())-1 )
        pos=doc->getPageNums()-1;
    int ret= doc->insertNewPage( i18n("Insert New Slide"), pos, IP_AFTER, false, QString::null );
    bool state = (ret !=-1);
    return state;
}

//return false if page number doesn't exist
bool KPrDocumentIface::selectPage( int page,bool select)
{
    if(page <0 || page> (int)(doc->getPageNums())-1 )
        return false;
    doc->selectPage( page, select );
    return true;
}

// return false when we can't remove page
bool KPrDocumentIface::deletePage( int _page )
{
    if( _page < 0 || _page > (int)(doc->getPageNums())-1 )
        return false;
    doc->deletePage(_page);
    return true;
}

void KPrDocumentIface::deSelectAllObj()
{
    doc->deSelectAllObj();
}

void KPrDocumentIface::recalcPageNum()
{
    doc->recalcPageNum();
}


void KPrDocumentIface::initConfig()
{
    doc->initConfig();
}

void KPrDocumentIface::saveConfig()
{
    doc->saveConfig();
}

QString KPrDocumentIface::selectedForPrinting()const
{
    return doc->selectedForPrinting();
}

bool KPrDocumentIface::isSlideSelected( int pgNum)
{
    if( pgNum>= (int)doc->getPageNums())
        return false;
    return doc->isSlideSelected(pgNum);
}

//Return a reference to header textobj
DCOPRef KPrDocumentIface::header()
{
    if(doc->header())
        return DCOPRef( kapp->dcopClient()->appId(),
                        doc->header()->dcopObject()->objId() );
    else
        return DCOPRef();
}

//Return a reference to footer textobj
DCOPRef KPrDocumentIface::footer()
{
    if(doc->footer())
        return DCOPRef( kapp->dcopClient()->appId(),
                        doc->footer()->dcopObject()->objId() );
    else
        return DCOPRef();
}

void KPrDocumentIface::startBackgroundSpellCheck()
{
    doc->startBackgroundSpellCheck();
}

void KPrDocumentIface::reactivateBgSpellChecking()
{
    doc->reactivateBgSpellChecking();
}

void KPrDocumentIface::setConfigUpperCase( bool _uc )
{
    doc->getAutoFormat()->configUpperCase(_uc);
}

void KPrDocumentIface::setConfigUpperUpper( bool _uu )
{
    doc->getAutoFormat()->configUpperUpper(_uu);
}

void KPrDocumentIface::setConfigAdvancedAutocorrect( bool _aa )
{
    doc->getAutoFormat()->configAdvancedAutocorrect( _aa );
}

void KPrDocumentIface::setConfigAutoDetectUrl(bool _au)
{
    doc->getAutoFormat()->configAutoDetectUrl(_au);
}

void KPrDocumentIface::setConfigIgnoreDoubleSpace( bool _ids)
{
    doc->getAutoFormat()->configIgnoreDoubleSpace(_ids);
}

void KPrDocumentIface::setConfigRemoveSpaceBeginEndLine( bool _space)
{
    doc->getAutoFormat()->configRemoveSpaceBeginEndLine(_space);
}

void KPrDocumentIface::setConfigUseBulletStyle( bool _ubs)
{
    doc->getAutoFormat()->configUseBulletStyle(_ubs);
}

bool KPrDocumentIface::configUpperCase() const
{
    return doc->getAutoFormat()->getConfigUpperCase();
}

bool KPrDocumentIface::configUpperUpper() const
{
    return doc->getAutoFormat()->getConfigUpperUpper();
}

bool KPrDocumentIface::configAdvancedAutoCorrect() const
{
    return doc->getAutoFormat()->getConfigAdvancedAutoCorrect();
}

bool KPrDocumentIface::configAutoDetectUrl() const
{
    return doc->getAutoFormat()->getConfigAutoDetectUrl();
}

bool KPrDocumentIface::configIgnoreDoubleSpace() const
{
    return doc->getAutoFormat()->getConfigIgnoreDoubleSpace();
}

bool KPrDocumentIface::configRemoveSpaceBeginEndLine() const
{
    return doc->getAutoFormat()->getConfigIgnoreDoubleSpace();
}

bool KPrDocumentIface::configUseBulletSyle() const
{
    return doc->getAutoFormat()->getConfigUseBulletSyle();
}

bool KPrDocumentIface::configAutoChangeFormat() const
{
    return doc->getAutoFormat()->getConfigAutoChangeFormat();
}

void KPrDocumentIface::setConfigAutoChangeFormat( bool _auto)
{
    doc->getAutoFormat()->configAutoChangeFormat(_auto);
}

bool KPrDocumentIface::configAutoReplaceNumber() const
{
    return doc->getAutoFormat()->getConfigAutoReplaceNumber();
}

void KPrDocumentIface::setConfigAutoReplaceNumber( bool b )
{
    doc->getAutoFormat()->configAutoReplaceNumber(b);
}

bool KPrDocumentIface::showStatusBar() const
{
    return doc->showStatusBar();
}

void KPrDocumentIface::setShowStatusBar( bool _status )
{
    doc->setShowStatusBar(_status);
    doc->reorganizeGUI();
}

void KPrDocumentIface::setConfigAutoNumberStyle( bool b )
{
    doc->getAutoFormat()->configAutoNumberStyle(b);
}

bool KPrDocumentIface::configAutoNumberStyle() const
{
    return doc->getAutoFormat()->getConfigAutoNumberStyle();
}

void KPrDocumentIface::setConfigCompletion( bool b )
{
    doc->getAutoFormat()->configCompletion( b );
}

bool KPrDocumentIface::configCompletion() const
{
    return doc->getAutoFormat()->getConfigCompletion();
}

void KPrDocumentIface::setConfigAppendSpace( bool b)
{
    doc->getAutoFormat()->configAppendSpace( b );
}

bool KPrDocumentIface::configAppendSpace() const
{
    return doc->getAutoFormat()->getConfigAppendSpace();
}

void KPrDocumentIface::setConfigMinWordLength( uint val )
{
    doc->getAutoFormat()->configMinWordLength( val );
}

uint KPrDocumentIface::configMinWordLength() const
{
    return doc->getAutoFormat()->getConfigMinWordLength();
}

void KPrDocumentIface::setConfigNbMaxCompletionWord( uint val )
{
    doc->getAutoFormat()->configNbMaxCompletionWord( val );
}

uint KPrDocumentIface::configNbMaxCompletionWord() const
{
    return doc->getAutoFormat()->getConfigNbMaxCompletionWord();
}

void KPrDocumentIface::setConfigAddCompletionWord( bool b )
{
    doc->getAutoFormat()->configAddCompletionWord( b );
}

bool KPrDocumentIface::configAddCompletionWord() const
{
    return doc->getAutoFormat()->getConfigAddCompletionWord();
}

bool KPrDocumentIface::configIncludeTwoUpperUpperLetterException() const
{
    return doc->getAutoFormat()->getConfigIncludeTwoUpperUpperLetterException();
}

void KPrDocumentIface::setConfigIncludeTwoUpperUpperLetterException( bool b)
{
    doc->getAutoFormat()->configIncludeTwoUpperUpperLetterException( b );
}

bool KPrDocumentIface::configIncludeAbbreviation() const
{
    return doc->getAutoFormat()->getConfigIncludeAbbreviation();
}

void KPrDocumentIface::setConfigIncludeAbbreviation( bool b)
{
    doc->getAutoFormat()->configIncludeAbbreviation( b );
}

bool KPrDocumentIface::displayComment() const
{
    return doc->getVariableCollection()->variableSetting()->displayComment();
}

void KPrDocumentIface::setDisplayComment( bool b)
{
    doc->getVariableCollection()->variableSetting()->setDisplayComment( b );
    doc->recalcVariables(VT_NOTE);
}

bool KPrDocumentIface::showGuideLines() const
{
    return doc->showGuideLines();
}

void KPrDocumentIface::setShowGuideLines( bool b )
{
    doc->setShowGuideLines( b );
    doc->updateGuideLineButton();
    doc->repaint( false );
}

void KPrDocumentIface::addGuideLine( bool horizontal, double pos )
{
    doc->addGuideLine( horizontal ? Qt::Horizontal: Qt::Vertical, pos );
    doc->repaint( false );
}

unsigned int KPrDocumentIface::nbHorizontalHelpLine() const
{
    return doc->horizontalGuideLines().count();
}

unsigned int KPrDocumentIface::nbVerticalHelpLine() const
{
    return doc->verticalGuideLines().count();
}

bool KPrDocumentIface::showGrid() const
{
    return doc->showGrid();
}

void KPrDocumentIface::setShowGrid ( bool _grid )
{
    doc->setShowGrid( _grid);
    doc->updateGridButton();
    doc->repaint( false );
}

double KPrDocumentIface::gridX() const
{
    return doc->getGridX();
}

void KPrDocumentIface::setGridX(double _x)
{
    doc->setGridX( _x );
    if( showGrid() )
        doc->repaint( false );
}

double KPrDocumentIface::gridY() const
{
    return doc->getGridY();
}

void KPrDocumentIface::setGridY(double _y)
{
    doc->setGridY( _y );
    if( showGrid() )
        doc->repaint( false );
}

bool KPrDocumentIface::configAutoSuperScript() const
{
    return doc->getAutoFormat()->getConfigAutoSuperScript();
}

void KPrDocumentIface::setConfigAutoSuperScript( bool b)
{
    doc->getAutoFormat()->configAutoSuperScript( b );
}

void KPrDocumentIface::addIgnoreWordAll( const QString &word)
{
    doc->addSpellCheckIgnoreWord( word );
}

void KPrDocumentIface::clearIgnoreWordAll( )
{
    doc->setSpellCheckIgnoreList( QStringList() );
}

QStringList KPrDocumentIface::spellListIgnoreAll() const
{
    return doc->spellCheckIgnoreList();
}

bool KPrDocumentIface::displayFieldCode()const
{
    return doc->getVariableCollection()->variableSetting()->displayFieldCode();
}

void KPrDocumentIface::setDisplayFieldCode( bool b)
{
    doc->getVariableCollection()->variableSetting()->setDisplayFieldCode( b );
    doc->recalcVariables(VT_ALL);
}

QString KPrDocumentIface::configAutoFormatLanguage( )const
{
    return doc->getAutoFormat()->getConfigAutoFormatLanguage( );
}


bool KPrDocumentIface::configCapitalizeNameOfDays() const
{
    return doc->getAutoFormat()->getConfigCapitalizeNameOfDays();
}

void KPrDocumentIface::setConfigCapitalizeNameOfDays( bool b)
{
    doc->getAutoFormat()->configCapitalizeNameOfDays( b );
}

QString KPrDocumentIface::presentationName() const
{
    return doc->presentationName();
}

void KPrDocumentIface::setPresentationName( const QString &_name )
{
    doc->setPresentationName( _name );
}


QStringList KPrDocumentIface::presentationList()
{
    return doc->presentationList();
}

void KPrDocumentIface::repaint()
{
    doc->repaint( false );
}

void KPrDocumentIface::setConfigToolTipCompletion( bool b )
{
    doc->getAutoFormat()->configToolTipCompletion( b );
}

bool KPrDocumentIface::configToolTipCompletion() const
{
    return doc->getAutoFormat()->getConfigToolTipCompletion();
}
