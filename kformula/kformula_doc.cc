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

#include <qbitmap.h>
#include <qcolor.h>
#include <qdom.h>
#include <qpainter.h>
#include <qpopupmenu.h>
#include <qprinter.h>
#include <qstring.h>
#include <qwmatrix.h>

#include <config.h>
#include <unistd.h>

#include <kaboutdialog.h>
#include <kaction.h>
#include <kapplication.h>
#include <kdebug.h>
#include <koGlobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kstdaction.h>
#include <kstandarddirs.h>
#include <kurl.h>

#include <kformulacontainer.h>
#include <kformuladocument.h>


KFormulaDoc::KFormulaDoc(QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, bool singleViewMode)
        : KoDocument(parentWidget, widgetName, parent, name, singleViewMode)
{
    setInstance(KFormulaFactory::global(), false);
    //kdDebug(39001) << "General Settings" << endl;

    history = new KoCommandHistory( actionCollection() );
    wrapper = new KFormula::DocumentWrapper( kapp->config(),
                                             actionCollection(),
                                             history );
    document = new KFormula::Document;
    wrapper->document( document );
    formula = document->createFormula();

    document->setEnabled( true );

    // the modify flag
    connect(history, SIGNAL(commandExecuted()), this, SLOT(commandExecuted()));
    connect(history, SIGNAL(documentRestored()), this, SLOT(documentRestored()));
}


KFormulaDoc::~KFormulaDoc()
{
    delete history;
    delete wrapper;
}


QDomDocument KFormulaDoc::saveXML()
{
    QDomDocument doc = document->saveXML();
    history->documentSaved();
    return doc;
}

bool KFormulaDoc::loadOasis( const QDomDocument&, KoOasisStyles&, const QDomDocument&, KoStore* )
{
    //todo
    return false;
}

bool KFormulaDoc::loadXML(QIODevice *, const QDomDocument& doc)
{
    if ( document->loadXML( doc ) ) {
        history->clear();
        history->documentSaved();
        return true;
    }
    return false;
}


KoView* KFormulaDoc::createViewInstance(QWidget* _parent, const char *name)
{
    return new KFormulaPartView(this, _parent, name);
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


bool KFormulaDoc::initDoc()
{
    // If nothing is loaded, do initialize here
    return TRUE;
}

void KFormulaDoc::paintContent(QPainter& painter, const QRect& rect, bool transparent, double zoomX, double zoomY)
{
    // ####### handle transparency and zoom
    // Need to draw only the document rectangle described in the parameter rect.

    bool forPrint = painter.device() && painter.device()->devType() == QInternal::Printer;
    document->setZoomAndResolution( 100, zoomX, zoomY, true, forPrint );
    if ( !transparent ) {
        painter.fillRect( rect, Qt::white );
    }
    formula->draw( painter, rect );
}

QString KFormulaDoc::configFile() const
{
//    return readConfigFile( locate( "data", "kformula/kformula.rc",
//                                 KFormulaFactory::global() ) );

//    return readConfigFile( "kformula.rc" );
    return QString::null;
}

#include "kformula_doc.moc"
