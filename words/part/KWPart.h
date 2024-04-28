/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012 C. Boemann <cbo@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
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

    QGraphicsItem *createCanvasItem(KoDocument *document) override; /// reimplemented

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
    KoView *createViewInstance(KoDocument *document, QWidget *parent) override; /// reimplemented
    virtual void setupViewInstance(KWDocument *document, KWView *view);

    KWDocument *m_document;
};

#endif
