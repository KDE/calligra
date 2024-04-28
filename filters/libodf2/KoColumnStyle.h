/*
 *  SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KOCOLUMNSTYLE_H
#define KOCOLUMNSTYLE_H

#include "KoStyle.h"
#include "koodf2_export.h"

/**
 * \class KoColumnStyle
 * \brief represents a style to be applied to one or more columns.
 *
 * As all the styles it can be shared
 */

class KOODF2_EXPORT KoColumnStyle : public KoStyle
{
    KoColumnStyle();

public:
    KOSTYLE_DECLARE_SHARED_POINTER(KoColumnStyle)

    ~KoColumnStyle() override;

    enum BreakType { NoBreak, AutoBreak, ColumnBreak, PageBreak };
    void setBreakBefore(BreakType breakBefore);
    BreakType breakBefore() const;

    void setBreakAfter(BreakType breakAfter);
    BreakType breakAfter() const;

    enum WidthType { MinimumWidth, ExactWidth, OptimalWidth };
    void setWidth(qreal width);
    qreal width() const;
    void setWidthType(WidthType type);
    WidthType widthType() const;

protected:
    void prepareStyle(KoGenStyle &style) const override;
    QString defaultPrefix() const override;
    KoGenStyle::Type styleType() const override;
    KoGenStyle::Type automaticstyleType() const override;
    const char *styleFamilyName() const override;

private:
    BreakType m_breakAfter;
    BreakType m_breakBefore;
    qreal m_width;
    WidthType m_widthType;
};

#endif
