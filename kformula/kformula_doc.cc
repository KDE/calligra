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



KFormulaDoc::KFormulaDoc(QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, bool singleViewMode)
        : KoDocument(parentWidget, widgetName, parent, name, singleViewMode),
          history(actionCollection())
{
    setInstance(KFormulaFactory::global(), false);
    //kdDebug(39001) << "General Settings" << endl;
    
    formula = new KFormulaContainer(history);

    // the modify flag
    connect(&history, SIGNAL(commandExecuted()), this, SLOT(commandExecuted()));
    connect(&history, SIGNAL(documentRestored()), this, SLOT(documentRestored()));
    history.documentSaved();

    // copy&paste
    (void) KStdAction::cut(formula, SLOT(cut()), actionCollection());
    (void) KStdAction::copy(formula, SLOT(copy()), actionCollection());
    (void) KStdAction::paste(formula, SLOT(paste()), actionCollection());
    
    (void) new KAction(i18n("Add/change to integral"),
                       "mini-integral",
                       CTRL + Key_6 ,
                       formula, SLOT(addIntegral()),
                       actionCollection(), "addintegral");
    (void) new KAction(i18n("Add/change to symbol"),
                       "mini-symbol",
                       CTRL + Key_7 ,
                       formula, SLOT(addSum()),
                       actionCollection(), "addsymbol");
    (void) new KAction(i18n("Add/change to root"),
                       "mini-root",
                       CTRL + Key_2 ,
                       formula, SLOT(addRoot()),
                       actionCollection(), "addroot");
    (void) new KAction(i18n("Add/change to fraction"),
                       "mini-frac",
                       CTRL + Key_3 ,
                       formula, SLOT(addFraction()),
                       actionCollection(), "addfrac");
    (void) new KAction(i18n("Add/change to bracket"),
                       "mini-bra",
                       CTRL + Key_5 ,
                       this, SLOT(addBracket()),
                       actionCollection(),"addbra");

    
    //KAction* matrixElement     = new KAction(i18n("Matrix"),      CTRL+Key_M, formula, SLOT(addMatrix()), actionCollection());
}


KFormulaDoc::~KFormulaDoc()
{
}


QDomDocument KFormulaDoc::saveXML()
{
    QDomDocument doc("FORMULA");
    formula->save(doc);
    history.documentSaved();
    return doc;
}

bool KFormulaDoc::loadXML(QIODevice *, const QDomDocument& doc)
{
    if (doc.doctype().name() != "FORMULA") {
        return false;
    }

    if (formula->load(doc)) {
        history.clear();
        history.documentSaved();
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


void KFormulaDoc::addBracket()
{
    formula->addBracket('(', ')');
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

void KFormulaDoc::print(QPrinter *thePrt)
{
}


bool KFormulaDoc::initDoc()
{
    // If nothing is loaded, do initialize here
    return TRUE;
}

void KFormulaDoc::paintContent( QPainter& painter, const QRect& /*rect*/, bool /*transparent*/ )
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
