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

#include <kapplication.h>
#include <dcopclient.h>
#include <koVariable.h>
#include "kprvariable.h"

KPresenterDocIface::KPresenterDocIface( KPresenterDoc *doc_ )
    : KoDocumentIface( doc_ )
{
   doc = doc_;
}

int KPresenterDocIface::numPages()
{
    return doc->getPageNums();
}

DCOPRef KPresenterDocIface::page( int num )
{
    if( num>= doc->getPageNums())
      return DCOPRef();
    return DCOPRef( kapp->dcopClient()->appId(),
		    doc->pageList().at( num )->dcopObject()->objId() );
}

double KPresenterDocIface::indentValue()
{
    return doc->getIndentValue();
}

void KPresenterDocIface::setIndentValue(double _ind)
{
    doc->setIndentValue(_ind);
}

bool KPresenterDocIface::dontCheckUpperWord()
{
    return doc->dontCheckUpperWord();
}

void KPresenterDocIface::setDontCheckUpperWord(bool _b)
{
    doc->setDontCheckUpperWord(_b);
}

bool KPresenterDocIface::dontCheckTitleCase()
{
    return doc->dontCheckTitleCase();
}

void KPresenterDocIface::setDontCheckTitleCase(bool _b)
{
    doc->setDontCheckTitleCase(_b);
}

int KPresenterDocIface::maxRecentFiles()
{
    return doc->maxRecentFiles();
}

void KPresenterDocIface::setUndoRedoLimit(int val)
{
    doc->setUndoRedoLimit(val);
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

bool KPresenterDocIface::displayLink()
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
  if( pos < 0 || pos > doc->getPageNums()-1 )
    pos=doc->getPageNums()-1;
  int ret= doc->insertNewPage( i18n("Insert New Page"), pos, IP_AFTER, false, QString::null );
  bool state = (ret !=-1);
  return state;
}

//return false if page number doesn't exist
bool KPresenterDocIface::selectPage( int page,bool select)
{
    if(page <0 || page> doc->getPageNums()-1 )
        return false;
    doc->selectPage( page, select );
    return true;
}

// return false when we can't remove page
bool KPresenterDocIface::deletePage( int _page )
{
  if( _page < 0 || _page > doc->getPageNums()-1 )
      return false;
  doc->deletePage(_page);
}

void KPresenterDocIface::deSelectAllObj()
{
    doc->deSelectAllObj();
}

void KPresenterDocIface::recalcPageNum()
{
    doc->recalcPageNum();
}

bool KPresenterDocIface::presentationDuration() const
{
    return doc->presentationDuration();
}

void KPresenterDocIface::setPresentationDuration( bool pd )
{
    doc->setPresentationDuration(pd);
}

bool KPresenterDocIface::spInfinitLoop() const
{
    return doc->spInfinitLoop();
}

bool KPresenterDocIface::spManualSwitch() const
{
    return doc->spManualSwitch();
}

void KPresenterDocIface::setInfinitLoop( bool il )
{
    doc->setInfinitLoop(il);
}

void KPresenterDocIface::setManualSwitch( bool ms )
{
    doc->setManualSwitch(ms);
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

int KPresenterDocIface::leftBorder() const
{
    return doc->getLeftBorder();
}

int KPresenterDocIface::topBorder() const
{
    return doc->getTopBorder();
}

int KPresenterDocIface::bottomBorder() const
{
    return doc->getBottomBorder();
}

bool KPresenterDocIface::isSlideSelected( int pgNum)
{
    if( pgNum>= doc->getPageNums())
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
    {
        return DCOPRef( kapp->dcopClient()->appId(),
                       doc->header()->dcopObject()->objId() );
    }
    else
        return DCOPRef();
}

//Return a reference to footer textobj
DCOPRef KPresenterDocIface::footer()
{
    if(doc->footer())
    {
        return DCOPRef( kapp->dcopClient()->appId(),
                       doc->footer()->dcopObject()->objId() );
    }
    else
        return DCOPRef();
}

bool KPresenterDocIface::backgroundSpellCheckEnabled() const
{
    return doc->backgroundSpellCheckEnabled();
}

void KPresenterDocIface::enableBackgroundSpellCheck( bool b )
{
    doc->enableBackgroundSpellCheck(b);
}

void KPresenterDocIface::startBackgroundSpellCheck()
{
    doc->startBackgroundSpellCheck();
}

void KPresenterDocIface::reactivateBgSpellChecking()
{
    doc->reactivateBgSpellChecking();
}
