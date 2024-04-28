/*
 *  SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KOCELLSTYLE_H
#define KOCELLSTYLE_H

#include <KoBorder.h>

#include "KoStyle.h"
#include "koodf2_export.h"

/**
 * \class KoCellStyle
 * \brief represents a style of a cell to be applied to one or more cells.
 *
 * As all the styles it can be shared
 */
class KOODF2_EXPORT KoCellStyle : public KoStyle
{
    KoCellStyle();

public:
    KOSTYLE_DECLARE_SHARED_POINTER(KoCellStyle)

    ~KoCellStyle() override;

    KoBorder *borders();

    void setBackgroundColor(const QColor &color);
    QColor backgroundColor() const;

    void setBackgroundOpacity(qreal opacity);
    qreal backgroundOpacity() const;

    qreal leftPadding() const;
    void setLeftPadding(qreal padding);

    qreal topPadding() const;
    void setTopPadding(qreal padding);

    qreal rightPadding() const;
    void setRightPadding(qreal padding);

    qreal bottomPadding() const;
    void setBottomPadding(qreal padding);

    QString verticalAlign() const;
    void setVerticalAlign(const QString &align);

    bool glyphOrientation() const;
    void setGlyphOrientation(bool orientation);

    void setTextStyle(const KoGenStyle &style);
    void setParagraphStyle(const KoGenStyle &style);
    KoGenStyle styleProperties() const;

protected:
    void prepareStyle(KoGenStyle &style) const override;
    QString defaultPrefix() const override;
    KoGenStyle::Type styleType() const override;
    KoGenStyle::Type automaticstyleType() const override;
    const char *styleFamilyName() const override;

private:
    KoBorder *m_borders;
    QColor m_backgroundColor;
    qreal m_backgroundOpacity;

    qreal m_leftPadding;
    qreal m_topPadding;
    qreal m_rightPadding;
    qreal m_bottomPadding;

    QString m_verticalAlign;
    bool m_glyphOrientation;

    KoGenStyle m_styleProperties;
};

#endif
