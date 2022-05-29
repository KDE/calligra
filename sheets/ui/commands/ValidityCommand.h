/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005, 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_VALIDITY_COMMAND
#define CALLIGRA_SHEETS_VALIDITY_COMMAND

#include "AbstractRegionCommand.h"
#include "engine/Validity.h"

namespace Calligra
{
namespace Sheets
{

/**
 * \class ValidityCommand
 * \ingroup Commands
 * \brief Adds/Removes validity checks to/of a cell region.
 */
class ValidityCommand : public AbstractRegionCommand
{
public:
    ValidityCommand();
    void setValidity(Validity validity);

protected:
    bool process(Element* element) override;

private:
    Validity m_validity;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_VALIDITY_COMMAND
