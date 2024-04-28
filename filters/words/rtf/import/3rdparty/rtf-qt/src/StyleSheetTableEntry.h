// SPDX-FileCopyrightText: 2010 Brad Hards <bradh@frogmouth.net>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef RTFREADER_STYLESHEETTABLEENTRY_H
#define RTFREADER_STYLESHEETTABLEENTRY_H
#include "rtfreader_export.h"
namespace RtfReader
{
enum TextAlignment { LeftAligned, CentreAligned, RightAligned, Justified, Distributed };

class RTFREADER_EXPORT StyleSheetTableEntry
{
public:
    StyleSheetTableEntry()
        : m_textAlignment(LeftAligned)
        , m_leftIndent(0)
        , m_rightIndent(0)
        , m_layoutDirection(Qt::LeftToRight)
    {
    }

    enum TextAlignment textAlignment() const
    {
        return m_textAlignment;
    }

    void setTextAlignment(enum TextAlignment textAlignment)
    {
        m_textAlignment = textAlignment;
    }

    int leftIndent() const
    {
        return m_leftIndent;
    }

    void setLeftIndent(const int leftIndent)
    {
        m_leftIndent = leftIndent;
    }

    int rightIndent() const
    {
        return m_rightIndent;
    }

    void setRightIndent(const int rightIndent)
    {
        m_rightIndent = rightIndent;
    }

    int topMargin() const
    {
        return m_topMargin;
    }

    void setTopMargin(const int topMargin)
    {
        m_topMargin = topMargin;
    }

    Qt::LayoutDirection layoutDirection() const
    {
        return m_layoutDirection;
    }

    void setLayoutDirection(const Qt::LayoutDirection layoutDirection)
    {
        m_layoutDirection = layoutDirection;
    }

    QString styleName() const
    {
        return m_styleName;
    }

    void setStyleName(const QString &styleName)
    {
        m_styleName = styleName;
    }

protected:
    QString m_styleName;
    enum TextAlignment m_textAlignment;
    int m_leftIndent;
    int m_rightIndent;
    Qt::LayoutDirection m_layoutDirection;
    int m_topMargin;
};
}

#endif
