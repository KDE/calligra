/* This file is part of the KDE project
   Copyright (C) 2001, 2002 Laurent MONTEL <lmontel@mandrakesoft.com>

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
#include "KWordDocIface.h"
#include "KWTextFrameSet.h"
#include "KWordFrameSetIface.h"
#include "KWDocument.h"
#include "KWVariable.h"
#include "KWPageManager.h"

#include <kapplication.h>
#include <dcopclient.h>
#include <KoVariable.h>
#include <KoAutoFormat.h>

KWordDocIface::KWordDocIface( KWDocument *doc_ )
    : KoDocumentIface( doc_ )
{
   doc = doc_;
}

DCOPRef KWordDocIface::textFrameSet( int num )
{
    if( num>= (int)doc->frameSetCount())
        return DCOPRef();
    return DCOPRef( kapp->dcopClient()->appId(),
            doc->textFrameSet( num)->dcopObject()->objId() );
}

DCOPRef KWordDocIface::frameSet( int num )
{
    if( num>= (int)doc->frameSetCount())
        return DCOPRef();
    return DCOPRef( kapp->dcopClient()->appId(),
            doc->frameSet( num)->dcopObject()->objId() );
}

int KWordDocIface::numFrameSets()const
{
    return doc->frameSetCount();
}

int KWordDocIface::frameSetCount()const
{
    return doc->frameSetCount();
}

unsigned int KWordDocIface::nbColumns()const
{
    return doc->numColumns();
}

double KWordDocIface::ptPageTop( int pgNum )const
{
    return doc->pageManager()->topOfPage(pgNum);
}

QString KWordDocIface::unitName()const
{
    return doc->unitName();
}

void KWordDocIface::recalcAllVariables()
{
    //recalc all variable
    doc->recalcVariables(VT_ALL);
}

void KWordDocIface::recalcVariables(int _var)
{
    doc->recalcVariables(_var);
}

void KWordDocIface::recalcVariables(const QString &varName)
{
    if(varName=="VT_DATE")
        doc->recalcVariables(0);
    else if(varName=="VT_TIME")
        doc->recalcVariables(2);
    else if(varName=="VT_PGNUM")
        doc->recalcVariables(4);
    else if(varName=="VT_CUSTOM")
        doc->recalcVariables(6);
    else if(varName=="VT_MAILMERGE")
        doc->recalcVariables(7);
    else if(varName=="VT_FIELD")
        doc->recalcVariables(8);
    else if(varName=="VT_LINK")
        doc->recalcVariables(9);
    else if(varName=="VT_NOTE")
        doc->recalcVariables(10);
    else if(varName=="VT_FOOTNOTE")
        doc->recalcVariables(11);
    else if(varName=="VT_ALL")
        doc->recalcVariables(256);
}


bool KWordDocIface::showRuler() const
{
    return doc->showRuler();
}

bool KWordDocIface::showdocStruct() const
{
    return doc->showdocStruct();
}

bool KWordDocIface::viewFrameBorders() const
{
    return doc->viewFrameBorders();
}

void KWordDocIface::setHeaderVisible( bool b)
{
    doc->setHeaderVisible(b);
    doc->refreshGUIButton();
}

void KWordDocIface::setFooterVisible( bool b)
{
    doc->setFooterVisible( b);
    doc->refreshGUIButton();
}

void KWordDocIface::setViewFrameBorders( bool b )
{
    doc->setViewFrameBorders( b );
    doc->refreshGUIButton();
}

void KWordDocIface::setShowRuler(bool b)
{
    doc->setShowRuler(b);
    doc->refreshGUIButton();
    doc->reorganizeGUI();
}

bool KWordDocIface::viewFormattingChars() const
{
    return doc->viewFormattingChars();
}

void KWordDocIface::setViewFormattingChars(bool b)
{
    doc->setViewFormattingChars(b);
    doc->refreshGUIButton();
}

void KWordDocIface::setShowDocStruct(bool b)
{
    doc->setShowDocStruct(b);
    doc->refreshGUIButton();
    doc->reorganizeGUI();
}


bool KWordDocIface::showStatusBar() const
{
    return doc->showStatusBar();
}

void KWordDocIface::setShowStatusBar( bool _status )
{
    doc->setShowStatusBar(_status);
    doc->reorganizeGUI();
}

int KWordDocIface::startingPage()const
{
    return doc->variableCollection()->variableSetting()->startingPageNumber();
}

void KWordDocIface::setStartingPage(int nb)
{
    doc->variableCollection()->variableSetting()->setStartingPageNumber(nb);
    doc->recalcVariables(VT_PGNUM);
}

bool KWordDocIface::displayLink()const
{
    return doc->variableCollection()->variableSetting()->displayLink();
}

void KWordDocIface::setDisplayLink(bool b)
{
    doc->variableCollection()->variableSetting()->setDisplayLink(b);
    doc->recalcVariables(VT_LINK);
}

bool KWordDocIface::setCustomVariableValue(const QString & varname, const QString & value)
{
    bool exist=doc->variableCollection()->customVariableExist(varname);
    if(exist)
    {
        doc->variableCollection()->setVariableValue( varname, value );
        doc->recalcVariables(VT_CUSTOM);
    }
    else
        return false;
    return true;
}


QString KWordDocIface::customVariableValue(const QString & varname)const
{
    if(doc->variableCollection()->customVariableExist(varname))
        return doc->variableCollection()->getVariableValue( varname );
    return QString::null;
}

void KWordDocIface::initConfig()
{
    doc->initConfig();
}

void KWordDocIface::saveConfig()
{
    doc->saveConfig();
}

bool KWordDocIface::isHeaderVisible() const
{
    return doc->isHeaderVisible();
}

bool KWordDocIface::isFooterVisible() const
{
    return doc->isFooterVisible();
}

bool KWordDocIface::backgroundSpellCheckEnabled() const
{
    return doc->backgroundSpellCheckEnabled();
}

void KWordDocIface::enableBackgroundSpellCheck( bool b )
{
    doc->enableBackgroundSpellCheck(b);
}

void KWordDocIface::startBackgroundSpellCheck()
{
    doc->startBackgroundSpellCheck();
}

void KWordDocIface::reactivateBgSpellChecking()
{
    doc->reactivateBgSpellChecking();
}

void KWordDocIface::setConfigUpperCase( bool _uc )
{
    doc->autoFormat()->configUpperCase(_uc);
}

void KWordDocIface::setConfigUpperUpper( bool _uu )
{
    doc->autoFormat()->configUpperUpper(_uu);
}

void KWordDocIface::setConfigAdvancedAutocorrect( bool _aa )
{
    doc->autoFormat()->configAdvancedAutocorrect( _aa );

}

void KWordDocIface::setConfigAutoDetectUrl(bool _au)
{
    doc->autoFormat()->configAutoDetectUrl(_au);
}

void KWordDocIface::setConfigIgnoreDoubleSpace( bool _ids)
{
    doc->autoFormat()->configIgnoreDoubleSpace(_ids);
}

bool KWordDocIface::configUpperCase() const
{
    return doc->autoFormat()->getConfigUpperCase();
}

bool KWordDocIface::configUpperUpper() const
{
    return doc->autoFormat()->getConfigUpperUpper();
}

bool KWordDocIface::configAdvancedAutoCorrect() const
{
    return doc->autoFormat()->getConfigAdvancedAutoCorrect();

}
bool KWordDocIface::configAutoDetectUrl() const
{
    return doc->autoFormat()->getConfigAutoDetectUrl();

}

bool KWordDocIface::configIgnoreDoubleSpace() const
{
    return doc->autoFormat()->getConfigIgnoreDoubleSpace();
}

bool KWordDocIface::configRemoveSpaceBeginEndLine() const
{
    return doc->autoFormat()->getConfigIgnoreDoubleSpace();
}

bool KWordDocIface::configUseBulletSyle() const
{
    return doc->autoFormat()->getConfigUseBulletSyle();
}

void KWordDocIface::setConfigRemoveSpaceBeginEndLine( bool _space)
{
    doc->autoFormat()->configRemoveSpaceBeginEndLine(_space);
}

void KWordDocIface::setConfigUseBulletStyle( bool _ubs)
{
    doc->autoFormat()->configUseBulletStyle(_ubs);
}

bool KWordDocIface::configAutoChangeFormat() const
{
    return doc->autoFormat()->getConfigAutoChangeFormat();
}

void KWordDocIface::setConfigAutoChangeFormat( bool _auto)
{
    doc->autoFormat()->configAutoChangeFormat(_auto);
}

bool KWordDocIface::configAutoReplaceNumber() const
{
    return doc->autoFormat()->getConfigAutoReplaceNumber();
}

void KWordDocIface::setConfigAutoReplaceNumber( bool b )
{
    doc->autoFormat()->configAutoReplaceNumber(b);
}

void KWordDocIface::setConfigAutoNumberStyle( bool b )
{
    doc->autoFormat()->configAutoNumberStyle(b);
}

bool KWordDocIface::configAutoNumberStyle() const
{
    return doc->autoFormat()->getConfigAutoNumberStyle();
}

void KWordDocIface::setConfigCompletion( bool b )
{
    doc->autoFormat()->configCompletion( b );
}

bool KWordDocIface::configCompletion() const
{
    return doc->autoFormat()->getConfigCompletion();
}

void KWordDocIface::setConfigToolTipCompletion( bool b )
{
    doc->autoFormat()->configToolTipCompletion( b );
}

bool KWordDocIface::configToolTipCompletion() const
{
    return doc->autoFormat()->getConfigToolTipCompletion();
}

void KWordDocIface::setConfigAppendSpace( bool b)
{
    doc->autoFormat()->configAppendSpace( b );
}

bool KWordDocIface::configAppendSpace() const
{
    return doc->autoFormat()->getConfigAppendSpace();
}

void KWordDocIface::setConfigMinWordLength( uint val )
{
    doc->autoFormat()->configMinWordLength( val );
}

uint KWordDocIface::configMinWordLength() const
{
    return doc->autoFormat()->getConfigMinWordLength();
}

void KWordDocIface::setConfigNbMaxCompletionWord( uint val )
{
    doc->autoFormat()->configNbMaxCompletionWord( val );
}

uint KWordDocIface::configNbMaxCompletionWord() const
{
    return doc->autoFormat()->getConfigNbMaxCompletionWord();
}

void KWordDocIface::setConfigAddCompletionWord( bool b )
{
    doc->autoFormat()->configAddCompletionWord( b );
}

bool KWordDocIface::configAddCompletionWord() const
{
    return doc->autoFormat()->getConfigAddCompletionWord();
}

bool KWordDocIface::configIncludeTwoUpperUpperLetterException() const
{
    return doc->autoFormat()->getConfigIncludeTwoUpperUpperLetterException();
}

void KWordDocIface::setConfigIncludeTwoUpperUpperLetterException( bool b)
{
    doc->autoFormat()->configIncludeTwoUpperUpperLetterException( b );
}

bool KWordDocIface::configIncludeAbbreviation() const
{
    return doc->autoFormat()->getConfigIncludeAbbreviation();
}

void KWordDocIface::setConfigIncludeAbbreviation( bool b)
{
    doc->autoFormat()->configIncludeAbbreviation( b );
}

bool KWordDocIface::displayComment() const
{
    return doc->variableCollection()->variableSetting()->displayComment();
}

void KWordDocIface::setDisplayComment( bool b)
{
    doc->variableCollection()->variableSetting()->setDisplayComment( b );
    doc->recalcVariables(VT_NOTE);
}


bool KWordDocIface::displayFieldCode()const
{
    return doc->variableCollection()->variableSetting()->displayFieldCode();
}

void KWordDocIface::setDisplayFieldCode( bool b)
{
    doc->variableCollection()->variableSetting()->setDisplayFieldCode( b );
    doc->recalcVariables(VT_ALL);
}

bool KWordDocIface::configAutoSuperScript() const
{
    return doc->autoFormat()->getConfigAutoSuperScript();
}

void KWordDocIface::setConfigAutoSuperScript( bool b)
{
    doc->autoFormat()->configAutoSuperScript( b );
}

void KWordDocIface::addIgnoreWordAll( const QString &word )
{
    doc->addSpellCheckIgnoreWord( word );
}

void KWordDocIface::clearIgnoreWordAll( )
{
    doc->setSpellCheckIgnoreList( QStringList() );
}

QStringList KWordDocIface::spellListIgnoreAll() const
{
    return doc->spellCheckIgnoreList();
}


bool KWordDocIface::showScrollBar()const
{
    return doc->showScrollBar();
}

void KWordDocIface::setShowScrollBar( bool _status )
{
    doc->setShowScrollBar(_status );
    doc->reorganizeGUI();
}

double KWordDocIface::ptTabStopValue() const
{
    return doc->tabStopValue();
}

void KWordDocIface::setPtTabStopValue ( double _tabStop )
{
    doc->setTabStopValue (_tabStop );
}

int KWordDocIface::footNoteSeparatorLineLength() const
{
    return doc->footNoteSeparatorLineLength();
}

void KWordDocIface::setFootNoteSeparatorLineLength( int _length)
{
    doc->setFootNoteSeparatorLineLength( _length );
    doc->repaintAllViews();
}

void KWordDocIface::setFootNoteSeparatorLinePosition( const QString &pos)
{
    SeparatorLinePos tmp=SLP_LEFT;
    if ( pos.lower()=="left")
        tmp=SLP_LEFT;
    else if ( pos.lower()=="centered")
        tmp=SLP_CENTERED;
    else if ( pos.lower()=="right")
        tmp=SLP_RIGHT;
    doc->setFootNoteSeparatorLinePosition( tmp);
    doc->repaintAllViews();
}

QString KWordDocIface::footNoteSeparatorLinePosition()const
{
    QString tmp=QString::null;
    switch( doc->footNoteSeparatorLinePosition() )
    {
    case SLP_LEFT:
        return QString("left");
    case SLP_RIGHT:
        return QString("right");
    case SLP_CENTERED:
        return QString("centered");
    }
    return tmp;
}

double KWordDocIface::footNoteSeparatorLineWidth() const
{
    return doc->footNoteSeparatorLineWidth();
}

void KWordDocIface::setFootNoteSeparatorLineWidth( double _width)
{
    doc->setFootNoteSeparatorLineWidth( _width );
    doc->repaintAllViews();
}

void KWordDocIface::deleteBookMark(const QString &_name)
{
    doc->deleteBookmark( _name );
}

void KWordDocIface::renameBookMark(const QString &_oldname, const QString &_newName)
{
    doc->renameBookmark( _oldname, _newName);
}

QStringList KWordDocIface::listOfBookmarkName()const
{
    //return all list
    return doc->listOfBookmarkName(0L);
}

QString KWordDocIface::configAutoFormatLanguage( )const
{
    return doc->autoFormat()->getConfigAutoFormatLanguage( );
}

bool KWordDocIface::configCapitalizeNameOfDays() const
{
    return doc->autoFormat()->getConfigCapitalizeNameOfDays();
}

void KWordDocIface::setConfigCapitalizeNameOfDays( bool b)
{
    doc->autoFormat()->configCapitalizeNameOfDays( b );
}

bool KWordDocIface::showGrid() const
{
    return doc->showGrid();
}

bool KWordDocIface::snapToGrid() const
{
    return doc->snapToGrid();
}

double KWordDocIface::gridX() const
{
    return doc->gridX();
}

double KWordDocIface::gridY() const
{
    return doc->gridY();
}

void KWordDocIface::setGridX( double _gridX )
{

    doc->setGridX( QMAX( 0.1, _gridX ) );
}

void KWordDocIface::setGridY( double _gridY )
{
    doc->setGridY( QMAX( 0.1, _gridY ) );
}

void KWordDocIface::setShowGrid( bool _b )
{
    doc->setShowGrid( _b );
}

void KWordDocIface::setSnapToGrid( bool _b )
{
    doc->setSnapToGrid( _b );
}
