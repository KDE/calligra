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

#include "kpresenter_doc.h"
#include "kpresenter_view.h"

#include <kapplication.h>
#include <dcopclient.h>
#include <koVariable.h>

KPresenterDocIface::KPresenterDocIface( KPresenterDoc *doc_ )
    : KoDocumentIface( doc_ )
{
   doc = doc_;
}

int KPresenterDocIface::getNumObjects()
{
    return doc->objNums();
}

DCOPRef KPresenterDocIface::getObject( int num )
{
    return DCOPRef( kapp->dcopClient()->appId(),
		    doc->objectList()->at( num )->dcopObject()->objId() );
}

int KPresenterDocIface::getNumPages()
{
    return doc->getPageNums();
}

DCOPRef KPresenterDocIface::getPage( int num )
{
    return DCOPRef( kapp->dcopClient()->appId(),
		    doc->backgroundList()->at( num )->dcopObject()->objId() );
}

int KPresenterDocIface::getPageOfObj( int obj, double faktor )
{
    return doc->getPageOfObj( obj, 0, 0, faktor ) - 1;
}

double KPresenterDocIface::getIndentValue()
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
    else if(varName=="VT_ALL")
        doc->recalcVariables(256);
}
