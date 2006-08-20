/* This file is part of the KDE project
   Copyright (C) 2001 Andrea Rizzi <rizzi@kde.org>
	              Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
		 2006 Martin Pfeiffer <hubipete@gmx.net>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KFORMULAPARTDOCUMENT_H
#define KFORMULAPARTDOCUMENT_H

#include <KoDocument.h>
#include "KFormulaPartView.h"

#include <QPainter>

class QIODevice;
class QDomDocument;

#include <kcommand.h>

class KoXmlWriter;
/* that will come...
namespace KFormula {
class FormulaShape;
}
*/

/**
 * @short The document class for the KFormulaPart
 * 
 * A KFormulaPartDocument will always be created by @ref KFormulaPartFactory.
 * Following the model-view-paradigm KFormulaPartDocument is the class holding
 * the data whereas KFormulaPartView does the GUI & rendering related tasks.
 * The actual data KFormulaPartDocument is maintaining is a list of @ref FormulaShape.
 * 
 */
class KFormulaPartDocument : public KoDocument
{
    Q_OBJECT

public:
    /// The basic constructor
    KFormulaPartDocument( QWidget *parentWidget = 0, QObject* parent = 0,
		           bool singleViewMode = false);
    ~KFormulaPartDocument();

    virtual void paintContent( QPainter &painter, const QRect &rect,
		    bool transparent = false, double zoomX = 1.0, double zoomY = 1.0 );

    virtual void showStartUpWidget(KoMainWindow* parent, bool alwaysShow = false);
    virtual bool showEmbedInitDialog(QWidget* parent);
    virtual bool loadOasis( const QDomDocument& doc, KoOasisStyles& oasisStyles,
		            const QDomDocument& settings, KoStore* );
    virtual bool saveOasis( KoStore* store, KoXmlWriter* manifestWriter );
    virtual bool loadXML( QIODevice *, const QDomDocument & doc );

protected slots:
    void commandExecuted();
    void documentRestored();

protected:
    virtual QString configFile() const;
    virtual KoView* createViewInstance(QWidget* parent);

private:
    /// The undo stack
    KCommandHistory* m_commandHistory;

    /// The formula in form of a FormulaShape
//    FormulaShape* m_formulaShape;
};

#endif
