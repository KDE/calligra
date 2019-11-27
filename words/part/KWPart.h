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

#ifndef KWPART_H
#define KWPART_H

#include <KoPart.h>

#include "words_export.h"

class KoMainWindow;
class KWDocument;
class KWView;
class QGraphicsItem;

class WORDS_EXPORT KWPart : public KoPart
{
    Q_OBJECT

public:
    explicit KWPart(QObject *parent);
    // TODO: not nice, just added to allow CAuPart pass its own componentData
    explicit KWPart(const KoComponentData &componentData, QObject *parent);

    ~KWPart() override;

    QGraphicsItem *createCanvasItem(KoDocument *document) override; ///reimplemented

    void setDocument(KWDocument *document);
    KWDocument *document() const;

    /// reimplemented from super
    QList<KoPart::CustomDocumentWidgetItem> createCustomDocumentWidgets(QWidget *parent) override;

    /// reimplemented from super
    KoMainWindow *createMainWindow() override;

    void showStartUpWidget(KoMainWindow *parent, bool alwaysShow = false) override;

private Q_SLOTS:
     void showErrorAndDie();

protected:

    KoView *createViewInstance(KoDocument *document, QWidget *parent) override; ///reimplemented
    virtual void setupViewInstance(KoDocument *document, KWView *view);

    KWDocument *m_document;
};

#endif
