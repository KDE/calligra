/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2010 KO GmbH <boud@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOTOOLBASE_P_H
#define KOTOOLBASE_P_H

#include "KoDocumentResourceManager.h"
#include "KoCanvasResourceManager.h"
#include "KoCanvasBase.h"
#include "KoShapeController.h"
#include <QMap>
#include <QHash>
#include <QWidget>
#include <QString>
#include <QSet>
#include <QPointer>
#include <string.h> // for the qt version check

class QWidget;
class QAction;
class KoToolBase;
class KoCanvasBase;

class KoToolBasePrivate
{
public:
    KoToolBasePrivate(KoToolBase *qq, KoCanvasBase *canvas_)
        : currentCursor(Qt::ArrowCursor),
        q(qq),
        canvas(canvas_),
        isInTextMode(false)
    {
    }

    ~KoToolBasePrivate()
    {
        foreach(QPointer<QWidget> optionWidget, optionWidgets) {
            if (optionWidget) {
                optionWidget->setParent(0);
                delete optionWidget;
            }
        }
        optionWidgets.clear();
    }

    void connectSignals()
    {
        if (canvas) { // in the case of KoToolManagers dummytool it can be zero :(
            KoCanvasResourceManager * crp = canvas->resourceManager();
            Q_ASSERT_X(crp, "KoToolBase::KoToolBase", "No Canvas KoResourceManager");
            if (crp)
                q->connect(crp, &KoCanvasResourceManager::canvasResourceChanged,
                        q, &KoToolBase::canvasResourceChanged);

            // can be 0 in the case of Calligra Sheets
            KoDocumentResourceManager *scrm = canvas->shapeController()->resourceManager();
            if (scrm) {
                q->connect(scrm, &KoDocumentResourceManager::resourceChanged,
                        q, &KoToolBase::documentResourceChanged);
            }
        }
    }

    QList<QPointer<QWidget> > optionWidgets; ///< the optionwidgets associated with this tool
    QCursor currentCursor;
    QHash<QString, QAction *> actionCollection;
    QString toolId;
    QList<QAction*> popupActionList;
    QSet<QAction *> readOnlyActions;
    KoToolBase *q;
    KoCanvasBase *canvas; ///< the canvas interface this tool will work for.
    bool isInTextMode;
};

#endif
