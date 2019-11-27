/* This file is part of the KDE project
   Copyright 2005,2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef CALLIGRA_SHEETS_VALIDITY_COMMAND
#define CALLIGRA_SHEETS_VALIDITY_COMMAND

#include "Validity.h"

#include "AbstractRegionCommand.h"

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
    bool mainProcessing() override;

private:
    Validity m_validity;
    QList< QPair<QRectF, Validity> > m_undoData;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_VALIDITY_COMMAND
