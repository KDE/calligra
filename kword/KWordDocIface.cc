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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "KWordDocIface.h"
#include "kwtextframeset.h"
#include "KWordFrameSetIface.h"
#include "kwdoc.h"
#include "kwvariable.h"
#include <kapplication.h>
#include <dcopclient.h>
#include <kovariable.h>
#include <koAutoFormat.h>
KWordDocIface::KWordDocIface( KWDocument *doc_ )
    : KoDocumentIface( doc_ )
{
   doc = doc_;
}

DCOPRef KWordDocIface::textFrameSet( int num )
{
    if( num>= (int)doc->numFrameSets())
        return DCOPRef();
    return DCOPRef( kapp->dcopClient()->appId(),
		    doc->textFrameSet( num)->dcopObject()->objId() );
}

DCOPRef KWordDocIface::frameSet( int num )
{
    if( num>= (int)doc->numFrameSets())
        return DCOPRef();
    return DCOPRef( kapp->dcopClient()->appId(),
		    doc->frameSet( num)->dcopObject()->objId() );
}

int KWordDocIface::numFrameSets()const
{
    return doc->numFrameSets();
}

unsigned int KWordDocIface::nbColumns()const
{
    return doc->numColumns();
}

double KWordDocIface::ptPageTop( int pgNum )const
{
    return doc->ptPageTop(pgNum);
}

QString KWordDocIface::unitName()const
{
    return doc->getUnitName();
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
    return doc->getVariableCollection()->variableSetting()->startingPage();
}

void KWordDocIface::setStartingPage(int nb)
{
    doc->getVariableCollection()->variableSetting()->setStartingPage(nb);
    doc->recalcVariables(VT_PGNUM);
}

bool KWordDocIface::displayLink()const
{
    return doc->getVariableCollection()->variableSetting()->displayLink();
}

void KWordDocIface::setDisplayLink(bool b)
{
    doc->getVariableCollection()->variableSetting()->setDisplayLink(b);
    doc->recalcVariables(VT_LINK);
}

bool KWordDocIface::setCustomVariableValue(const QString & varname, const QString & value)
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


QString KWordDocIface::customVariableValue(const QString & varname)const
{
    if(doc->getVariableCollection()->customVariableExist(varname))
        return doc->getVariableCollection()->getVariableValue( varname );
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

void KWordDocIface::initUnit()
{
    doc->initUnit();
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
    doc->getAutoFormat()->configUpperCase(_uc);
}

void KWordDocIface::setConfigUpperUpper( bool _uu )
{
    doc->getAutoFormat()->configUpperUpper(_uu);
}

void KWordDocIface::setConfigAdvancedAutocorrect( bool _aa )
{
    doc->getAutoFormat()->configAdvancedAutocorrect( _aa );

}

void KWordDocIface::setConfigAutoDetectUrl(bool _au)
{
    doc->getAutoFormat()->configAutoDetectUrl(_au);
}

void KWordDocIface::setConfigIgnoreDoubleSpace( bool _ids)
{
    doc->getAutoFormat()->configIgnoreDoubleSpace(_ids);
}

bool KWordDocIface::configUpperCase() const
{
    return doc->getAutoFormat()->getConfigUpperCase();
}

bool KWordDocIface::configUpperUpper() const
{
    return doc->getAutoFormat()->getConfigUpperUpper();
}

bool KWordDocIface::configAdvancedAutoCorrect() const
{
    return doc->getAutoFormat()->getConfigAdvancedAutoCorrect();

}
bool KWordDocIface::configAutoDetectUrl() const
{
    return doc->getAutoFormat()->getConfigAutoDetectUrl();

}

bool KWordDocIface::configIgnoreDoubleSpace() const
{
    return doc->getAutoFormat()->getConfigIgnoreDoubleSpace();
}

bool KWordDocIface::configRemoveSpaceBeginEndLine() const
{
    return doc->getAutoFormat()->getConfigIgnoreDoubleSpace();
}

bool KWordDocIface::configUseBulletSyle() const
{
    return doc->getAutoFormat()->getConfigUseBulletSyle();
}

void KWordDocIface::setConfigRemoveSpaceBeginEndLine( bool _space)
{
    doc->getAutoFormat()->configRemoveSpaceBeginEndLine(_space);
}

void KWordDocIface::setConfigUseBulletStyle( bool _ubs)
{
    doc->getAutoFormat()->configUseBulletStyle(_ubs);
}

bool KWordDocIface::configAutoChangeFormat() const
{
    return doc->getAutoFormat()->getConfigAutoChangeFormat();
}

void KWordDocIface::setConfigAutoChangeFormat( bool _auto)
{
    doc->getAutoFormat()->configAutoChangeFormat(_auto);
}

bool KWordDocIface::configAutoReplaceNumber() const
{
    return doc->getAutoFormat()->getConfigAutoReplaceNumber();
}

void KWordDocIface::setConfigAutoReplaceNumber( bool b )
{
    doc->getAutoFormat()->configAutoReplaceNumber(b);
}

void KWordDocIface::setConfigAutoNumberStyle( bool b )
{
    doc->getAutoFormat()->configAutoNumberStyle(b);
}

bool KWordDocIface::configAutoNumberStyle() const
{
    return doc->getAutoFormat()->getConfigAutoNumberStyle();
}

void KWordDocIface::setConfigCompletion( bool b )
{
    doc->getAutoFormat()->configCompletion( b );
}

bool KWordDocIface::configCompletion() const
{
    return doc->getAutoFormat()->getConfigCompletion();
}

void KWordDocIface::setConfigAppendSpace( bool b)
{
    doc->getAutoFormat()->configAppendSpace( b );
}

bool KWordDocIface::configAppendSpace() const
{
    return doc->getAutoFormat()->getConfigAppendSpace();
}

void KWordDocIface::setConfigMinWordLength( uint val )
{
    doc->getAutoFormat()->configMinWordLength( val );
}

uint KWordDocIface::configMinWordLength() const
{
    return doc->getAutoFormat()->getConfigMinWordLength();
}

void KWordDocIface::setConfigNbMaxCompletionWord( uint val )
{
    doc->getAutoFormat()->configNbMaxCompletionWord( val );
}

uint KWordDocIface::configNbMaxCompletionWord() const
{
    return doc->getAutoFormat()->getConfigNbMaxCompletionWord();
}

void KWordDocIface::setConfigAddCompletionWord( bool b )
{
    doc->getAutoFormat()->configAddCompletionWord( b );
}

bool KWordDocIface::configAddCompletionWord() const
{
    return doc->getAutoFormat()->getConfigAddCompletionWord();
}

bool KWordDocIface::configIncludeTwoUpperUpperLetterException() const
{
    return doc->getAutoFormat()->getConfigIncludeTwoUpperUpperLetterException();
}

void KWordDocIface::setConfigIncludeTwoUpperUpperLetterException( bool b)
{
    doc->getAutoFormat()->configIncludeTwoUpperUpperLetterException( b );
}

bool KWordDocIface::configIncludeAbbreviation() const
{
    return doc->getAutoFormat()->getConfigIncludeAbbreviation();
}

void KWordDocIface::setConfigIncludeAbbreviation( bool b)
{
    doc->getAutoFormat()->configIncludeAbbreviation( b );
}

bool KWordDocIface::displayComment() const
{
    return doc->getVariableCollection()->variableSetting()->displayComment();
}

void KWordDocIface::setDisplayComment( bool b)
{
    doc->getVariableCollection()->variableSetting()->setDisplayComment( b );
    doc->recalcVariables(VT_NOTE);
}


bool KWordDocIface::displayFieldCode()const
{
    return doc->getVariableCollection()->variableSetting()->displayFieldCode();
}

void KWordDocIface::setDisplayFieldCode( bool b)
{
    doc->getVariableCollection()->variableSetting()->setDisplayFieldCode( b );
    doc->recalcVariables(VT_ALL);
}

bool KWordDocIface::configAutoSuperScript() const
{
    return doc->getAutoFormat()->getConfigAutoSuperScript();
}

void KWordDocIface::setConfigAutoSuperScript( bool b)
{
    doc->getAutoFormat()->configAutoSuperScript( b );
}

void KWordDocIface::addIgnoreWordAll( const QString &word )
{
    doc->addIgnoreWordAll( word );
}

void KWordDocIface::clearIgnoreWordAll( )
{
    doc->clearIgnoreWordAll();
}

QStringList KWordDocIface::spellListIgnoreAll() const
{
    return doc->spellListIgnoreAll();
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
    doc->deleteBookMark( _name );
}

void KWordDocIface::renameBookMark(const QString &_oldname, const QString &_newName)
{
    doc->renameBookMark( _oldname, _newName);
}

QStringList KWordDocIface::listOfBookmarkName()const
{
    //return all list
    return doc->listOfBookmarkName(0L);
}

QString KWordDocIface::configAutoFormatLanguage( )const
{
    return doc->getAutoFormat()->getConfigAutoFormatLanguage( );
}

bool KWordDocIface::configCapitalizeNameOfDays() const
{
    return doc->getAutoFormat()->getConfigCapitalizeNameOfDays();
}

void KWordDocIface::setConfigCapitalizeNameOfDays( bool b)
{
    doc->getAutoFormat()->configCapitalizeNameOfDays( b );
}

