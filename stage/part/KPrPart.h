/* This file is part of the KDE project
   Copyright (C) 2012 C. Boemann <cbo@kogmbh.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
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
