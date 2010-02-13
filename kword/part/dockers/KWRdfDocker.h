/* This file is part of the KDE project
 * Copyright (C) 2010 KO GmbH <ben.martin@kogmbh.com>
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
#include <rdf/KoDocumentRdf.h>
#include "KoCanvasObserverBase.h"

class QTimer;
class QTextDocument;
class KWView;
class KoSelection;
class KWDocument;

/**
 * Docker widget that shows a semantic view of the Rdf that is
 * around the cursor location. For example, if you are over a
 * person's name and there is FOAF Rdf associated with that
 * element, then the person will be shown in the docker giving
 * the user the ability to import that contact.
 */
class KWRdfDocker : public QDockWidget, public KoCanvasObserverBase
{
    Q_OBJECT

public:
    explicit KWRdfDocker(KWView *parent);
    ~KWRdfDocker();
    /// reimplemented from KoCanvasObserver
    virtual void setCanvas(KoCanvasBase *canvas);
    KoCanvasBase *canvas();

private slots:
    void updateDataForced();
    void updateData();
    void setAutoUpdate(int);
    void selectionChanged();
    void showSemanticViewContextMenu(const QPoint &);
    void semanticObjectAdded(RdfSemanticItem* item);
    void semanticObjectUpdated(RdfSemanticItem* item);


private:

    // KWView *m_view;
    KoCanvasBase *m_canvas;
    int m_lastCursorPosition;
    bool m_autoUpdate;
    KWDocument *m_document;
    KoSelection *m_selection;
    QTimer *m_timer;
    QTextDocument *m_textDocument;
    RdfSemanticTree m_rdfSemanticTree;

    QWidget* m_widget;
    Ui::KWRdfDocker widgetDocker;
};
#endif
