/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005-2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_STYLE_COMMAND
#define CALLIGRA_SHEETS_STYLE_COMMAND

#include "AbstractRegionCommand.h"
#include "Style.h"

namespace Calligra
{
namespace Sheets
{

/**
 * \class StyleCommand
 * \ingroup Commands
 * \brief Manipulates the style of a cell region.
 */
class CALLIGRA_SHEETS_UI_EXPORT StyleCommand : public AbstractRegionCommand
{
public:
    explicit StyleCommand(KUndo2Command *parent = nullptr);
    ~StyleCommand() override;

    void setStyle(const Style &s)
    {
        m_style = s;
    }

    void setHorizontalPen(const QPen &pen)
    {
        m_horizontalPen = pen;
        m_horizontalPenChanged = true;
    }
    void setVerticalPen(const QPen &pen)
    {
        m_verticalPen = pen;
        m_verticalPenChanged = true;
    }

protected:
    bool process(Element *) override;

private:
    QPen m_horizontalPen;
    QPen m_verticalPen;
    bool m_horizontalPenChanged;
    bool m_verticalPenChanged;

    Style m_style;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_STYLE_COMMAND
