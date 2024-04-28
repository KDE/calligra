/*
 *  SPDX-FileCopyrightText: 2010 Carlos Licea <carlos@kdab.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef KOTBLSTYLE_H
#define KOTBLSTYLE_H

#include "KoStyle.h"
#include "koodf2_export.h"

#include <QColor>

/**
 * \class KoTblStyle
 * \brief This class represents a style for a Table in
 * a ODF document.
 *
 * As all the styles it can be shared.
 **/

class KOODF2_EXPORT KoTblStyle : public KoStyle
{
    KoTblStyle();

public:
    KOSTYLE_DECLARE_SHARED_POINTER(KoTblStyle)

    ~KoTblStyle() override;

    void setBackgroundColor(const QColor &color);
    QColor backgroundColor() const;

    enum BreakType { NoBreak, AutoBreak, ColumnBreak, PageBreak };
    void setBreakBefore(BreakType breakBefore);
    BreakType breakBefore() const;

    void setBreakAfter(BreakType breakAfter);
    BreakType breakAfter() const;

    void setAllowBreakBetweenRows(bool allow);
    bool allowBreakBetweenRows() const;

    void setMasterPageName(const QString &name);
    void setMasterPageName(const char *name);
    QString masterPageName() const;

    void setLeftMargin(qreal left);
    qreal leftMargin() const;

    void setTopMargin(qreal top);
    qreal topMargin() const;

    void setRightMargin(qreal right);
    qreal rightMargin() const;

    void setBottomMargin(qreal bottom);
    qreal bottomMargin() const;

    enum WidthUnit { PercentageUnit, PointsUnit };
    void setWidth(qreal width, WidthUnit unit = PointsUnit);
    qreal width() const;
    WidthUnit widthUnit() const;

    enum HorizontalAlign { CenterAlign, LeftAlign, MarginsAlign, RightAlign };
    void setHorizontalAlign(HorizontalAlign align);
    HorizontalAlign horizontalAlign() const;

    enum BorderModel { CollapsingModel, SeparatingModel };
    void setBorderModel(BorderModel bordelModel);
    BorderModel borderModel() const;

    void setDisplay(bool display);
    bool display() const;

    enum KeepWithNext { AutoKeepWithNext, AlwaysKeepWithNext };
    void setKeepWithNext(KeepWithNext keepWithNext);
    KeepWithNext keepWithNext() const;

    enum WritingMode { LrTbWritingMode, RlTbWritingMode, TbRlWritingMode, TbLrWritingMode, LrWritingMode, RlWritingMode, TbWritingMode, PageWritingMode };
    void setWritingMode(WritingMode writingMode);
    WritingMode writingMode() const;

protected:
    void prepareStyle(KoGenStyle &style) const override;
    KoGenStyle::Type automaticstyleType() const override;
    KoGenStyle::Type styleType() const override;
    const char *styleFamilyName() const override;
    QString defaultPrefix() const override;

private:
    QColor m_backgroundColor;
    BreakType m_breakAfter;
    BreakType m_breakBefore;
    bool m_allowBreakBetweenRows;

    QString m_masterPageName;

    qreal m_leftMargin;
    qreal m_topMargin;
    qreal m_rightMargin;
    qreal m_bottomMargin;

    qreal m_width;
    WidthUnit m_widthUnit;

    HorizontalAlign m_horizontalAlign;
    BorderModel m_borderModel;
    KeepWithNext m_keepWithNext;
    WritingMode m_writingMode;

    bool m_display;
    // TODO style:page-number
    // TODO style:shadow
    // TODO style:background-image
};

#endif
