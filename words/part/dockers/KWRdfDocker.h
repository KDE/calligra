/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 KO GmbH <ben.martin@kogmbh.com>
 * SPDX-FileCopyrightText: 2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWRdfDOCKER_H
#define KWRdfDOCKER_H

#include <KoCanvasObserverBase.h>
#include <KoDocumentRdf.h>
#include <QDockWidget>
#include <ui_KWRdfDocker.h>

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
