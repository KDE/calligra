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
#include <KoShapeControllerBase.h>
#include "KFormulaPartView.h"
#include <QPainter>
#include <kcommand.h>

class QIODevice;
class QDomDocument;
class FormulaShape;
class KoXmlWriter;

/**
 * @short The document class for the KFormulaPart
 * 
 * A KFormulaPartDocument will always be created by @ref KFormulaPartFactory.
 * Following the model-view-paradigm KFormulaPartDocument is the class holding
 * the data whereas KFormulaPartView does the GUI & rendering related tasks.
 * The actual data KFormulaPartDocument is maintaining is a list of FormulaShape.
 * 
 */
class KFormulaPartDocument : public KoDocument, public KoShapeControllerBase {
Q_OBJECT
public:
    /// The basic constructor
    KFormulaPartDocument( QWidget *parentWidget = 0, QObject* parent = 0,
		           bool singleViewMode = false );
    ~KFormulaPartDocument();

    // KoShapeControllerBase interface
    /// reimplemented from KoShapeControllerBase
    void addShape (KoShape *shape);
    /// reimplemented from KoShapeControllerBase
    void removeShape (KoShape *shape);


    // KoDocument interface
    /// reimplemented from KoDocument
    void paintContent( QPainter &painter, const QRect &rect);

    /// reimplemented from KoDocument
    void showStartUpWidget(KoMainWindow* parent, bool alwaysShow = false);

    /// reimplemented from KoDocument
    bool loadXML( QIODevice *, const KoXmlDocument & doc );

    /// reimplemented from KoDocument
    bool loadOasis( const KoXmlDocument& doc, KoOasisStyles& oasisStyles,
		            const KoXmlDocument& settings, KoStore* );

    /// reimplemented from KoDocument
    bool saveOasis( KoStore* store, KoXmlWriter* manifestWriter );

    /// reimplemented from KoDocument
    bool showEmbedInitDialog(QWidget* parent);

protected slots:
    void commandExecuted();
    void documentRestored();

protected:
    virtual QString configFile() const;
    virtual KoView* createViewInstance(QWidget* parent);

private:
    /// The undo stack
    KCommandHistory* m_commandHistory;
};

#endif
