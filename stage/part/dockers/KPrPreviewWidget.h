/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Sven Langkamp <sven.langkamp@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRPREVIEWWIDGET_H
#define KPRPREVIEWWIDGET_H

#include "stage_export.h"

#include <KoShape.h>
#include <QTimeLine>
#include <QWidget>

class KPrPage;
class KPrPageEffect;
class KPrPageEffectRunner;

/**
 * A widget for page effect preview. It shows a transition from a black page to the current page
 */
class STAGE_EXPORT KPrPreviewWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KPrPreviewWidget(QWidget *parent = nullptr);
    ~KPrPreviewWidget() override;

    /**
     * Set a page effect
     *
     * @param pageEffect The effect which will be previewed.
     * @param page The current page used in the preview. If 0 the preview will be x
     * @param prevpage The page coming before @p page
     */
    void setPageEffect(KPrPageEffect *pageEffect, KPrPage *page, KPrPage *prevpage);

    /**
     * Run the current page effect. Does nothing if no page effect was set.
     */
    void runPreview();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

protected Q_SLOTS:
    void animate();

private:
    void updatePixmaps();

    QTimeLine m_timeLine;

    KPrPageEffect *m_pageEffect;
    KPrPageEffectRunner *m_pageEffectRunner;
    KPrPage *m_page;
    KPrPage *m_prevpage;

    QPixmap m_oldPage;
    QPixmap m_newPage;
};

#endif /* KPRPREVIEWWIDGET_H */
