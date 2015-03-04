/* This file is part of the KDE project
 * Copyright (C) 2010 KO GmbH <ben.martin@kogmbh.com>
 * Copyright (C) 2010 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KWRdfDOCKER_H
#define KWRdfDOCKER_H

#include <QDockWidget>
#include <ui_KWRdfDocker.h>
#include <KoDocumentRdf.h>
#include <KoCanvasObserverBase.h>

class QTextDocument;
class KoSelection;
class KWDocument;

/**
 * Docker widget that shows a semantic view of the RDF that is
 * around the cursor location. For example, if you are over a
 * person's name and there is FOAF RDF associated with that
 * element, then the person will be shown in the docker giving
 * the user the ability to import that contact.
 */
class KWRdfDocker : public QDockWidget, public KoCanvasObserverBase
{
    Q_OBJECT

public:
    explicit KWRdfDocker();
    ~KWRdfDocker();
    
    /// reimplemented from KoCanvasObserver
    virtual void setCanvas(KoCanvasBase *canvas);
    virtual void unsetCanvas();
    
    KoCanvasBase *canvas();

private Q_SLOTS:
    void updateDataForced();
    void updateData();
    void showSemanticViewContextMenu(const QPoint &);
    void semanticObjectAdded(hKoRdfBasicSemanticItem item);
    void semanticObjectUpdated(hKoRdfBasicSemanticItem item);
    void canvasResourceChanged(int key, const QVariant &value);

private:
    KoCanvasBase *m_canvas;
    int m_lastCursorPosition;
    KWDocument *m_document;
    KoSelection *m_selection;
    QTextDocument *m_textDocument;
    KoRdfSemanticTree m_rdfSemanticTree;

    Ui::KWRdfDocker widgetDocker;
};
#endif
