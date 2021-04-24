/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_AUTO_FORMAT_COMMAND
#define CALLIGRA_SHEETS_AUTO_FORMAT_COMMAND

#include "AbstractRegionCommand.h"

#include <QList>

namespace Calligra
{
namespace Sheets
{
class Style;

/**
 * \ingroup Commands
 * \brief Formats a cell range using a pre-defined style for the table layout.
 */
class AutoFormatCommand : public AbstractRegionCommand
{
public:
    /**
     * Constructor.
     */
    AutoFormatCommand();

    /**
     * Destructor.
     */
    ~AutoFormatCommand() override;

    void setStyles(const QList<Style>& styles);

protected:
    bool process(Element* element) override;
    bool preProcessing() override;
    bool mainProcessing() override;

private:
    QList<Style> m_styles;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_AUTO_FORMAT_COMMAND
