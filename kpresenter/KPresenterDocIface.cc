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

#include "KPresenterDocIface.h"
#include "kptextobject.h"
#include "kpresenter_doc.h"
#include "kpresenter_view.h"
#include "kprpage.h"

#include <kapplication.h>
#include <dcopclient.h>
#include <kovariable.h>
#include "kprvariable.h"
#include <koAutoFormat.h>

KPresenterDocIface::KPresenterDocIface( KPresenterDoc *doc_ )
    : KoDocumentIface( doc_ )
{
    doc = doc_;
}

int KPresenterDocIface::numPages() const
{
    return doc->getPageNums();
}

DCOPRef KPresenterDocIface::page( int num )
{
    if( num>= (int )doc->getPageNums())
        return DCOPRef();
    return DCOPRef( kapp->dcopClient()->appId(),
                    doc->pageList().at( num )->dcopObject()->objId() );
}

DCOPRef KPresenterDocIface::masterPage()
{
    return DCOPRef( kapp->dcopClient()->appId(),
                    doc->masterPage()->dcopObject()->objId() );
}

void KPresenterDocIface::setShowRuler(bool b)
{
    doc->setShowRuler(b );
    doc->reorganizeGUI();
}

bool KPresenterDocIface::showRuler() const
{
    return doc->showRuler();
}

void KPresenterDocIface::recalcAllVariables()
{
    //recalc all variable
    doc->recalcVariables(VT_ALL);
}

void KPresenterDocIface::recalcVariables(int _var)
{
    doc->recalcVariables(_var);
}

void KPresenterDocIface::recalcVariables(const QString &varName)
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

int KPresenterDocIface::startingPage()
{
    return doc->getVariableCollection()->variableSetting()->startingPage();
}

void KPresenterDocIface::setStartingPage(int nb)
{
    doc->getVariableCollection()->variableSetting()->setStartingPage(nb);
    doc->recalcVariables(VT_PGNUM);
}

bool KPresenterDocIface::displayLink() const
{
    return doc->getVariableCollection()->variableSetting()->displayLink();
}

void KPresenterDocIface::setDisplayLink(bool b)
{
    doc->getVariableCollection()->variableSetting()->setDisplayLink(b);
    doc->recalcVariables(VT_LINK);
}

bool KPresenterDocIface::setCustomVariableValue(const QString & varname, const QString & value)
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

QString KPresenterDocIface::customVariableValue(const QString & varname)const
{
    if(doc->getVariableCollection()->customVariableExist(varname))
        return doc->getVariableCollection()->getVariableValue( varname );
    return QString::null;
}

bool KPresenterDocIface::insertNewPage(int pos )
{
    if( pos < 0 || pos > (int)(doc->getPageNums())-1 )
        pos=doc->getPageNums()-1;
    int ret= doc->insertNewPage( i18n("Insert New Slide"), pos, IP_AFTER, false, QString::null );
    bool state = (ret !=-1);
    return state;
}

//return false if page number doesn't exist
bool KPresenterDocIface::selectPage( int page,bool select)
{
    if(page <0 || page> (int)(doc->getPageNums())-1 )
        return false;
    doc->selectPage( page, select );
    return true;
}

// return false when we can't remove page
bool KPresenterDocIface::deletePage( int _page )
{
    if( _page < 0 || _page > (int)(doc->getPageNums())-1 )
        return false;
    doc->deletePage(_page);
    return true;
}

void KPresenterDocIface::deSelectAllObj()
{
    doc->deSelectAllObj();
}

void KPresenterDocIface::recalcPageNum()
{
    doc->recalcPageNum();
}

void KPresenterDocIface::setHeader( bool b )
{
    doc->setHeader(b);
}

void KPresenterDocIface::setFooter( bool b )
{
    doc->setFooter(b);
}

void KPresenterDocIface::initConfig()
{
    doc->initConfig();
}

void KPresenterDocIface::saveConfig()
{
    doc->saveConfig();
}

QString KPresenterDocIface::selectedForPrinting()const
{
    return doc->selectedForPrinting();
}

bool KPresenterDocIface::isSlideSelected( int pgNum)
{
    if( pgNum>= (int)doc->getPageNums())
        return false;
    return doc->isSlideSelected(pgNum);
}

bool KPresenterDocIface::hasFooter() const
{
    return doc->hasFooter();
}

bool KPresenterDocIface::hasHeader() const
{
    return doc->hasHeader();
}

//Return a reference to header textobj
DCOPRef KPresenterDocIface::header()
{
    if(doc->header())
        return DCOPRef( kapp->dcopClient()->appId(),
                        doc->header()->dcopObject()->objId() );
    else
        return DCOPRef();
}

//Return a reference to footer textobj
DCOPRef KPresenterDocIface::footer()
{
    if(doc->footer())
        return DCOPRef( kapp->dcopClient()->appId(),
                        doc->footer()->dcopObject()->objId() );
    else
        return DCOPRef();
}

void KPresenterDocIface::startBackgroundSpellCheck()
{
    doc->startBackgroundSpellCheck();
}

void KPresenterDocIface::reactivateBgSpellChecking()
{
    doc->reactivateBgSpellChecking();
}

void KPresenterDocIface::setConfigUpperCase( bool _uc )
{
    doc->getAutoFormat()->configUpperCase(_uc);
}

void KPresenterDocIface::setConfigUpperUpper( bool _uu )
{
    doc->getAutoFormat()->configUpperUpper(_uu);
}

void KPresenterDocIface::setConfigAdvancedAutocorrect( bool _aa )
{
    doc->getAutoFormat()->configAdvancedAutocorrect( _aa );
}

void KPresenterDocIface::setConfigAutoDetectUrl(bool _au)
{
    doc->getAutoFormat()->configAutoDetectUrl(_au);
}

void KPresenterDocIface::setConfigIgnoreDoubleSpace( bool _ids)
{
    doc->getAutoFormat()->configIgnoreDoubleSpace(_ids);
}

void KPresenterDocIface::setConfigRemoveSpaceBeginEndLine( bool _space)
{
    doc->getAutoFormat()->configRemoveSpaceBeginEndLine(_space);
}

void KPresenterDocIface::setConfigUseBulletStyle( bool _ubs)
{
    doc->getAutoFormat()->configUseBulletStyle(_ubs);
}

bool KPresenterDocIface::configUpperCase() const
{
    return doc->getAutoFormat()->getConfigUpperCase();
}

bool KPresenterDocIface::configUpperUpper() const
{
    return doc->getAutoFormat()->getConfigUpperUpper();
}

bool KPresenterDocIface::configAdvancedAutoCorrect() const
{
    return doc->getAutoFormat()->getConfigAdvancedAutoCorrect();
}

bool KPresenterDocIface::configAutoDetectUrl() const
{
    return doc->getAutoFormat()->getConfigAutoDetectUrl();
}

bool KPresenterDocIface::configIgnoreDoubleSpace() const
{
    return doc->getAutoFormat()->getConfigIgnoreDoubleSpace();
}

bool KPresenterDocIface::configRemoveSpaceBeginEndLine() const
{
    return doc->getAutoFormat()->getConfigIgnoreDoubleSpace();
}

bool KPresenterDocIface::configUseBulletSyle() const
{
    return doc->getAutoFormat()->getConfigUseBulletSyle();
}

bool KPresenterDocIface::configAutoChangeFormat() const
{
    return doc->getAutoFormat()->getConfigAutoChangeFormat();
}

void KPresenterDocIface::setConfigAutoChangeFormat( bool _auto)
{
    doc->getAutoFormat()->configAutoChangeFormat(_auto);
}

bool KPresenterDocIface::configAutoReplaceNumber() const
{
    return doc->getAutoFormat()->getConfigAutoReplaceNumber();
}

void KPresenterDocIface::setConfigAutoReplaceNumber( bool b )
{
    doc->getAutoFormat()->configAutoReplaceNumber(b);
}

bool KPresenterDocIface::showStatusBar() const
{
    return doc->showStatusBar();
}

void KPresenterDocIface::setShowStatusBar( bool _status )
{
    doc->setShowStatusBar(_status);
    doc->reorganizeGUI();
}

void KPresenterDocIface::setConfigAutoNumberStyle( bool b )
{
    doc->getAutoFormat()->configAutoNumberStyle(b);
}

bool KPresenterDocIface::configAutoNumberStyle() const
{
    return doc->getAutoFormat()->getConfigAutoNumberStyle();
}

void KPresenterDocIface::setConfigCompletion( bool b )
{
    doc->getAutoFormat()->configCompletion( b );
}

bool KPresenterDocIface::configCompletion() const
{
    return doc->getAutoFormat()->getConfigCompletion();
}

void KPresenterDocIface::setConfigAppendSpace( bool b)
{
    doc->getAutoFormat()->configAppendSpace( b );
}

bool KPresenterDocIface::configAppendSpace() const
{
    return doc->getAutoFormat()->getConfigAppendSpace();
}

void KPresenterDocIface::setConfigMinWordLength( uint val )
{
    doc->getAutoFormat()->configMinWordLength( val );
}

uint KPresenterDocIface::configMinWordLength() const
{
    return doc->getAutoFormat()->getConfigMinWordLength();
}

void KPresenterDocIface::setConfigNbMaxCompletionWord( uint val )
{
    doc->getAutoFormat()->configNbMaxCompletionWord( val );
}

uint KPresenterDocIface::configNbMaxCompletionWord() const
{
    return doc->getAutoFormat()->getConfigNbMaxCompletionWord();
}

void KPresenterDocIface::setConfigAddCompletionWord( bool b )
{
    doc->getAutoFormat()->configAddCompletionWord( b );
}

bool KPresenterDocIface::configAddCompletionWord() const
{
    return doc->getAutoFormat()->getConfigAddCompletionWord();
}

bool KPresenterDocIface::configIncludeTwoUpperUpperLetterException() const
{
    return doc->getAutoFormat()->getConfigIncludeTwoUpperUpperLetterException();
}

void KPresenterDocIface::setConfigIncludeTwoUpperUpperLetterException( bool b)
{
    doc->getAutoFormat()->configIncludeTwoUpperUpperLetterException( b );
}

bool KPresenterDocIface::configIncludeAbbreviation() const
{
    return doc->getAutoFormat()->getConfigIncludeAbbreviation();
}

void KPresenterDocIface::setConfigIncludeAbbreviation( bool b)
{
    doc->getAutoFormat()->configIncludeAbbreviation( b );
}

bool KPresenterDocIface::displayComment() const
{
    return doc->getVariableCollection()->variableSetting()->displayComment();
}

void KPresenterDocIface::setDisplayComment( bool b)
{
    doc->getVariableCollection()->variableSetting()->setDisplayComment( b );
    doc->recalcVariables(VT_NOTE);
}

bool KPresenterDocIface::showHelplines() const
{
    return doc->showHelplines();
}

void KPresenterDocIface::setShowHelplines(bool b)
{
    doc->setShowHelplines(b);
    doc->updateHelpLineButton();
    doc->repaint( false );
}

void KPresenterDocIface::addVertHelpLine( double val)
{
    doc->addVertHelpline(val);
    doc->repaint( false );
}

void KPresenterDocIface::addHoriHelpLine( double val)
{
    doc->addHorizHelpline(val);
    doc->repaint( false );
}

double KPresenterDocIface::horizHelpLineValue(int index) const
{
    if ( index >= (int)doc->horizHelplines().count())
        return -1.0;
    return doc->horizHelplines()[index];
}

double KPresenterDocIface::vertHelpLineValue(int index) const
{
    if ( index >= (int)doc->vertHelplines().count())
        return -1.0;
    return doc->vertHelplines()[index];
}

bool KPresenterDocIface::removeVertHelpLine( int index )
{
    if ( index >= (int)doc->vertHelplines().count())
        return false;
    doc->removeVertHelpline( index );
    doc->repaint( false );
    return true;
}

bool KPresenterDocIface::removeHorizHelpLine( int index )
{
    if ( index >= (int)doc->horizHelplines().count())
        return false;
    doc->removeHorizHelpline( index );
    doc->repaint( false );
    return true;
}

unsigned int KPresenterDocIface::nbHorizontalHelpLine() const
{
    return doc->horizHelplines().count();
}

unsigned int KPresenterDocIface::nbVerticalHelpLine() const
{
    return doc->vertHelplines().count();
}

unsigned int KPresenterDocIface::nbHelpPoint() const
{
    return doc->helpPoints().count();
}

bool KPresenterDocIface::moveHorizontalHelpLine( int index, double newPos)
{
    if ( index >= (int)doc->horizHelplines().count() )
        return false;
    if( newPos < 0 )
        doc->removeHorizHelpline( index );
    else
        doc->updateHorizHelpline( index, newPos);
    doc->repaint( false );
    return true;
}

bool KPresenterDocIface::moveVerticalHelpLine( int index, double newPos)
{
    if ( index >= (int)doc->vertHelplines().count() )
        return false;
    if( newPos < 0 )
        doc->removeVertHelpline( index );
    else
        doc->updateVertHelpline( index, newPos);
    doc->repaint( false );
    return true;
}

bool KPresenterDocIface::showGrid() const
{
    return doc->showGrid();
}

void KPresenterDocIface::setShowGrid ( bool _grid )
{
    doc->setShowGrid( _grid);
    doc->updateGridButton();
    doc->repaint( false );
}

double KPresenterDocIface::gridX() const
{
    return doc->getGridX();
}

void KPresenterDocIface::setGridX(double _x)
{
    doc->setGridX( _x );
    if( showGrid() )
        doc->repaint( false );
}

double KPresenterDocIface::gridY() const
{
    return doc->getGridY();
}

void KPresenterDocIface::setGridY(double _y)
{
    doc->setGridY( _y );
    if( showGrid() )
        doc->repaint( false );
}

bool KPresenterDocIface::gridToFront() const
{
    return doc->gridToFront();
}

void KPresenterDocIface::setGridToFront( bool _front )
{
    doc->setGridToFront( _front);
    doc->updateGridButton();
    if( showGrid() )
        doc->repaint( false );
}

bool KPresenterDocIface::helpLineToFront() const
{
    return doc->helpLineToFront();
}

void KPresenterDocIface::setHelpLineToFront( bool _front )
{
    doc->setHelpLineToFront(_front );
    doc->updateHelpLineButton();
    if( showGrid() )
        doc->repaint( false );
}

bool KPresenterDocIface::updateHelpPoint( int idx, double posX, double posY ) const
{
    if( idx < 0 || idx >= (int)doc->helpPoints().count() )
        return false ;
    doc->updateHelpPoint( idx, KoPoint( posX, posY ));
    if( showHelplines() )
        doc->repaint( false);
    return true;

}

void KPresenterDocIface::addHelpPoint( double posX, double posY )
{
    doc->addHelpPoint( KoPoint( posX, posY ));
    if( showHelplines() )
        doc->repaint( false);
}

bool KPresenterDocIface::removeHelpPoint( int index )
{
    if( index < 0 || index >= (int)doc->helpPoints().count() )
        return false;

    doc->removeHelpPoint( index );
    if( showHelplines() )
        doc->repaint( false);
    return true;
}

bool KPresenterDocIface::configAutoSuperScript() const
{
    return doc->getAutoFormat()->getConfigAutoSuperScript();
}

void KPresenterDocIface::setConfigAutoSuperScript( bool b)
{
    doc->getAutoFormat()->configAutoSuperScript( b );
}

double KPresenterDocIface::helpPointPosX( int index ) const
{
    if( index < 0 || index >= (int)doc->helpPoints().count() )
        return -1.0;
    return doc->helpPoints()[index].x();
}

double KPresenterDocIface::helpPointPosY( int index ) const
{
    if( index < 0 || index >= (int)doc->helpPoints().count() )
        return -1.0;
    return doc->helpPoints()[index].y();
}

void KPresenterDocIface::addIgnoreWordAll( const QString &word)
{
    doc->addIgnoreWordAll( word );
}

void KPresenterDocIface::clearIgnoreWordAll( )
{
    doc->clearIgnoreWordAll();
}

QStringList KPresenterDocIface::spellListIgnoreAll() const
{
    return doc->spellListIgnoreAll();
}

bool KPresenterDocIface::displayFieldCode()const
{
    return doc->getVariableCollection()->variableSetting()->displayFieldCode();
}

void KPresenterDocIface::setDisplayFieldCode( bool b)
{
    doc->getVariableCollection()->variableSetting()->setDisplayFieldCode( b );
    doc->recalcVariables(VT_ALL);
}

QString KPresenterDocIface::configAutoFormatLanguage( )const
{
    return doc->getAutoFormat()->getConfigAutoFormatLanguage( );
}


bool KPresenterDocIface::configCapitalizeNameOfDays() const
{
    return doc->getAutoFormat()->getConfigCapitalizeNameOfDays();
}

void KPresenterDocIface::setConfigCapitalizeNameOfDays( bool b)
{
    doc->getAutoFormat()->configCapitalizeNameOfDays( b );
}
