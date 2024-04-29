/* This file is part of the Calligra project, made within the KDE community.
 *
 * SPDX-FileCopyrightText: 2013 Friedrich W. H. Kossebau <friedrich@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef TEXTDOCUMENTINSPECTIONDOCKER_H
#define TEXTDOCUMENTINSPECTIONDOCKER_H

// library
#include <KoCanvasObserverBase.h>
// Qt
#include <QDockWidget>

class TextDocumentStructureModel;
class KoCanvasBase;
class QTreeView;

class TextDocumentInspectionDocker : public QDockWidget, public KoCanvasObserverBase
{
    Q_OBJECT
public:
    explicit TextDocumentInspectionDocker(QWidget *parent = nullptr);
    ~TextDocumentInspectionDocker() override;
    /// reimplemented from KoCanvasObserverBase
    QString observerName() const override
    {
        return QStringLiteral("TextDocumentInspectionDocker");
    }
    void setCanvas(KoCanvasBase *canvas) override;
    void unsetCanvas() override;

private Q_SLOTS:
    void onShapeSelectionChanged();

private:
    KoCanvasBase *m_canvas;
    QTreeView *m_mainWidget;

    TextDocumentStructureModel *m_textDocumentStructureModel;
};

#endif // TEXTDOCUMENTINSPECTIONDOCKER_H
