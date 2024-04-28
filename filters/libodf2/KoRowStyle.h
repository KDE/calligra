/*
 *  SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KOROWSTYLE_H
#define KOROWSTYLE_H

#include "KoStyle.h"
#include "koodf2_export.h"

#include <QColor>

/**
 * \class KoRowStyle
 * \brief This class represents a style of a row to be applied to one or more rows.
 *
 * As all the styles it can be shared
 */

class KOODF2_EXPORT KoRowStyle : public KoStyle
{
    KoRowStyle();

public:
    KOSTYLE_DECLARE_SHARED_POINTER(KoRowStyle)
    ~KoRowStyle() override;

    void setBackgroundColor(const QColor &color);
    QColor backgroundColor() const;

    enum HeightType { MinimumHeight, ExactHeight, OptimalHeight };
    void setHeight(qreal height);
    void setHeightType(HeightType type);
    qreal height() const;

    enum BreakType { NoBreak, AutoBreak, ColumnBreak, PageBreak };
    void setBreakBefore(BreakType breakBefore);
    BreakType breakBefore() const;

    void setBreakAfter(BreakType breakAfter);
    BreakType breakAfter() const;

    enum KeepTogetherType { DontKeepTogether, AutoKeepTogether, AlwaysKeeptogether };
    void setKeepTogether(KeepTogetherType keepTogether);
    KeepTogetherType keepTogether() const;

    //     void setBackgroundImage(Image image);
    //     Image backgroundImage() const;

protected:
    KoGenStyle::Type automaticstyleType() const override;
    QString defaultPrefix() const override;
    void prepareStyle(KoGenStyle &style) const override;
    const char *styleFamilyName() const override;
    KoGenStyle::Type styleType() const override;

private:
    QColor m_backgroundColor;
    //     Image* m_image;

    qreal m_height;
    HeightType m_heightType;
    BreakType m_breakAfter;
    BreakType m_breakBefore;
    KeepTogetherType m_keepTogether;
};

#endif
