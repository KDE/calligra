/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2.
 
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
 
   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/


#include "kformula_doc.h"
#include "kformula_view.h"
#include "kformula_factory.h"

#include <qpopupmenu.h>
#include <qdom.h>
#include <qwmatrix.h>
#include <qcolor.h>
#include <qbitmap.h>
#include <qpainter.h>
#include <qprinter.h>

#include <unistd.h>

#include <kaboutdialog.h>
#include <kaction.h>
#include <kapp.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kstdaction.h>
#include <kstddirs.h>
#include <kurl.h>

#include <kformulacontainer.h>
#include <kformuladocument.h>



KFormulaDoc::KFormulaDoc(QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, bool singleViewMode)
        : KoDocument(parentWidget, widgetName, parent, name, singleViewMode)
{
    setInstance(KFormulaFactory::global(), false);
    //kdDebug(39001) << "General Settings" << endl;

    history = new KCommandHistory(actionCollection());
    document = new KFormulaDocument(actionCollection(), history);
    formula = new KFormulaContainer(document);

    // the modify flag
    connect(history, SIGNAL(commandExecuted()), this, SLOT(commandExecuted()));
    connect(history, SIGNAL(documentRestored()), this, SLOT(documentRestored()));
    history->documentSaved();
}


KFormulaDoc::~KFormulaDoc()
{
    delete formula;
    delete document;
    delete history;
}


QDomDocument KFormulaDoc::saveXML()
{
    QDomDocument doc("FORMULA");
    formula->save(doc);
    history->documentSaved();
    return doc;
}

bool KFormulaDoc::loadXML(QIODevice *, const QDomDocument& doc)
{
    if (doc.doctype().name() != "FORMULA") {
        return false;
    }

    if (formula->load(doc)) {
        history->clear();
        history->documentSaved();
        return true;
    }
    return false;
}


KoView* KFormulaDoc::createViewInstance(QWidget* _parent, const char *name)
{
    return new KFormulaView(this, _parent, name);
}

void KFormulaDoc::commandExecuted()
{
    if (formula->isEmpty()) {
        setEmpty();
    }
    setModified(true);
}

void KFormulaDoc::documentRestored()
{
    setModified(false);
}


void KFormulaDoc::enlarge()
{
    kdDebug(39001) <<"enlarge()\n";
}

void KFormulaDoc::reduce()
{
    kdDebug(39001) <<"reduce()\n";
}

void KFormulaDoc::enlargeRecur()
{
    kdDebug(39001) <<"enlargeRecur()\n";
}

void KFormulaDoc::reduceRecur()
{
    kdDebug(39001) <<"redureRecur()\n";
}

void KFormulaDoc::enlargeAll()
{
    kdDebug(39001) <<"enlargeAll()\n";
}

void KFormulaDoc::reduceAll()
{
    kdDebug(39001) <<"reduceAll()\n";
}

void KFormulaDoc::pro()
{
    kdDebug(39001) <<"pro()\n";
}

void KFormulaDoc::dele()
{
    kdDebug(39001) <<"delete()\n";
}

bool KFormulaDoc::initDoc()
{
    // If nothing is loaded, do initialize here
    return TRUE;
}

void KFormulaDoc::paintContent(QPainter& /*painter*/, const QRect& /*rect*/, bool /*transparent*/)
{
    // ####### handle transparency

    // Need to draw only the document rectangle described in the parameter rect.
}

QString KFormulaDoc::configFile() const
{
//    return readConfigFile( locate( "data", "kformula/kformula.rc",
//                                 KFormulaFactory::global() ) );

//    return readConfigFile( "kformula.rc" );
    return QString::null;
}

#include "kformula_doc.moc"
