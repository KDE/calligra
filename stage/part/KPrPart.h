/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012 C. Boemann <cbo@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRPART_H
#define KPRPART_H

#include <KoPart.h>

#include "stage_export.h"

class KPrDocument;

class STAGE_EXPORT KPrPart : public KoPart
{
    Q_OBJECT

public:
    explicit KPrPart(QObject *parent);

    ~KPrPart() override;

    void setDocument(KPrDocument *document);

    /**
     * Creates and shows the start up widget. Reimplemented from KoDocument.
     *
     * @param parent the KoMainWindow used as parent for the widget.
     * @param alwaysShow always show the widget even if the user has configured it to not show.
     */
    void showStartUpWidget(KoMainWindow *parent, bool alwaysShow) override;

    /// reimplemented
    KoView *createViewInstance(KoDocument *document, QWidget *parent) override;
    /// reimplemented
    QGraphicsItem *createCanvasItem(KoDocument *document) override;
    /// reimplemented
    KoMainWindow *createMainWindow() override;
protected Q_SLOTS:
    /// Quits Stage with error message from m_errorMessage.
    void showErrorAndDie();

protected:
    QString m_errorMessage;
    KPrDocument *m_document;
};

#endif
