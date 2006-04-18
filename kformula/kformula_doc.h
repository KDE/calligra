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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KFORMULA_DOC_H
#define KFORMULA_DOC_H

#include <q3ptrlist.h>
#include <qpainter.h>

#include <kcommand.h>
#include <KoDocument.h>
#include <kcommand.h>
#include <kformuladefs.h>

#include "kformula_view.h"

class KoXmlWriter;

KFORMULA_NAMESPACE_BEGIN

class FormulaCursor;
class Container;
class Document;
class DocumentWrapper;

KFORMULA_NAMESPACE_END


/**
 * The part's document. Forwards most of the requests.
 */
class KFormulaDoc : public KoDocument
{
    Q_OBJECT

public:

    KFormulaDoc(QWidget *parentWidget = 0,
                const char *widgetName = 0,
                QObject* parent = 0,
                const char* name = 0,
                bool singleViewMode = false);
    ~KFormulaDoc();

    virtual void paintContent( QPainter &painter, const QRect &rect, bool transparent = false, double zoomX = 1.0, double zoomY = 1.0 );

    virtual bool initDoc(InitDocFlags flags, QWidget* parentWidget=0);
    virtual void showStartUpWidget(KoMainWindow* parent, bool alwaysShow = false);
    virtual bool showEmbedInitDialog(QWidget* parent);
    virtual bool loadOasis( const QDomDocument& doc, KoOasisStyles& oasisStyles, const QDomDocument& settings, KoStore* );
    virtual bool loadXML(QIODevice *, const QDomDocument& doc);
    virtual QDomDocument saveXML();
    virtual bool saveOasis( KoStore* store, KoXmlWriter* manifestWriter );

    KFormula::Container* getFormula() const { return formula; }
    KFormula::Document* getDocument() const { return document; }

protected slots:

    void commandExecuted();
    void documentRestored();

protected:

    virtual QString configFile() const;
    virtual KoView* createViewInstance(QWidget* parent, const char* name);

private:

    /**
     * Our undo stack.
     */
    KCommandHistory* history;

    /**
     * The place where all formula related work is done.
     */
    KFormula::Container* formula;

    /**
     * The document that contains all the formulas.
     * Right now we only have one, but this might change.
     */
    KFormula::Document* document;

    KFormula::DocumentWrapper* wrapper;
};

#endif
