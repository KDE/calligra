/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Matus Hanzes <matus.hanzes@ixonos.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef ANCHORSTRATEGY_H_
#define ANCHORSTRATEGY_H_

#include "KoShapeAnchor.h"

#include <QRectF>

class KoTextShapeContainerModel;
class KoTextLayoutRootArea;

class AnchorStrategy : public KoShapeAnchor::PlacementStrategy
{
public:
    AnchorStrategy(KoShapeAnchor *anchor, KoTextLayoutRootArea *rootArea);
    ~AnchorStrategy() override;

    /**
     * Moves the subject to it's right position.
     *
     * @return true if subject was moved to a new position (or it it couldn't be calculated yet)
     */
    virtual bool moveSubject() = 0;

    void detachFromModel() override;

    /**
     * Reparent the anchored shape under the rootArea's container this AnchorStrategy acts for
     *
     * If needed changes the parent KoShapeContainerModel and KoShapeContainer of the anchored shape.
     * It is changed so the anchored shape is now under the rootArea
     */
    void updateContainerModel() override;

    /// get page rectangle coordinates to which this text anchor is anchored (needed for HPage)
    QRectF pageRect() const;

    /// set page rectangle coordinates to which this text anchor is anchored (needed for HPage)
    void setPageRect(const QRectF &pageRect);

    /// get content rectangle coordinates to which this text anchor is anchored (needed for
    /// HPageContent)
    QRectF pageContentRect() const;

    /// set content rectangle coordinates to which this text anchor is anchored (needed for
    /// HPageContent)
    void setPageContentRect(const QRectF &marginRect);

    /// get paragraph rectangle coordinates to which this text anchor is anchored (needed for
    /// HParagraphContent, HParagraphStartMargin, HParagraphEndMargin, VParagraph)
    QRectF paragraphRect() const;

    /// set paragraph rectangle to which this text anchor is anchored (needed for HParagraphContent,
    /// HParagraphStartMargin, HParagraphEndMargin, VParagraph)
    void setParagraphRect(const QRectF &paragraphRect);

    /// get paragraph rectangle coordinates to which this text anchor is anchored (needed for
    /// HParagraphContent, HParagraphStartMargin, HParagraphEndMargin)
    QRectF paragraphContentRect() const;

    /// set paragraph rectangle to which this text anchor is anchored (needed for HParagraphContent,
    /// HParagraphStartMargin, HParagraphEndMargin)
    void setParagraphContentRect(const QRectF &paragraphContentRect);

    /// get layout environment rectangle @see odf attribute style:flow-with-text
    QRectF layoutEnvironmentRect() const;

    /// set layout environment rect @see odf attribute style:flow-with-text
    void setLayoutEnvironmentRect(const QRectF &layoutEnvironmentRect);

    /// get number of page to which this text anchor is anchored (needed for HOutside, HInside,
    /// HFromInside)
    int pageNumber() const;

    /// set number of page to which this text anchor is anchored (needed for HOutside, HInside,
    /// HFromInside)
    void setPageNumber(int pageNumber);

protected:
    KoShapeAnchor *const m_anchor;
    KoTextLayoutRootArea *m_rootArea;

private:
    KoTextShapeContainerModel *m_model;
    QRectF m_pageRect;
    QRectF m_pageContentRect;
    QRectF m_paragraphRect;
    QRectF m_paragraphContentRect;
    QRectF m_layoutEnvironmentRect;
    int m_pageNumber;
};

#endif /* ANCHORSTRATEGY_H_ */
