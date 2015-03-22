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

#ifndef KWRDFDOCKERTREE_H
#define KWRDFDOCKERTREE_H

#include <QTreeWidget>
#include <KoCanvasObserverBase.h>

class KoDocumentRdf;

/**
 * @short A custom QTree subclass that allows D&D
 *
 * @author  Ben Martin <ben.martin@kogmbh.com>
 * @see KoDocumentRdf
 *
 * This QTree subclass allows SemanticItems to be dragged away from
 * Calligra and accepts data dropped from other applications.
 *
 * When data is dropped onto the QTree a new SemanticItem is added to
 * the document and will be inserted at the current cursor position.
 * The Rdf docker displays information about the SemanticItem at the
 * cursor position, so by adding the new semantic item at the cursor
 * position, the docker will update itself too and as a consequence
 * the new SemanticItem will be displayed into the QTree that the user
 * dropped the data onto.
 *
 */
class KWRdfDockerTree : public QTreeWidget, public KoCanvasObserverBase
{
    Q_OBJECT
public:
    explicit KWRdfDockerTree(QWidget *parent = 0);

    void setDocumentRdf(KoDocumentRdf *rdf);
    /// reimplemented from KoCanvasObserver
    virtual void setCanvas(KoCanvasBase *canvas);
    virtual void unsetCanvas() { m_canvas = 0; }

    // for dropping
    bool dropMimeData(QTreeWidgetItem *parent, int index, const
                      QMimeData *data, Qt::DropAction action);

protected:
    // for dragging
    virtual QStringList mimeTypes() const;
    virtual QMimeData *mimeData(const QList<QTreeWidgetItem *> items) const;

    // for dropping
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *e);

    // for dragging
    void mouseMoveEvent(QMouseEvent *event);

private:
    KoDocumentRdf *m_rdf;
    KoCanvasBase *m_canvas;
};

#endif
