/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
             2004 Ariya Hidayat <ariya@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_DAMAGES
#define KSPREAD_DAMAGES

#include "QFlags"

namespace KSpread
{
class Cell;
class Sheet;

class Damage
{
  public:
    virtual ~Damage() {}

    typedef enum
    {
      Nothing = 0,
      Document,
      Workbook,
      Sheet,
      Range,
      Cell
    } Type;

    virtual Type type() const { return Nothing; }
};

class CellDamage : public Damage
{
  public:
    enum Change
    {
      Appearance = 0x01,
      Formula    = 0x02,
      Layout     = 0x04,
      TextFormat = 0x10,
      Value      = 0x20
    };
    Q_DECLARE_FLAGS( Changes, Change )

    CellDamage( KSpread::Cell* cell, Changes changes );

    virtual ~CellDamage();

    virtual Type type() const { return Damage::Cell; }

    KSpread::Cell* cell() const;

    Changes changes() const;

  private:
    class Private;
    Private * const d;
};
Q_DECLARE_OPERATORS_FOR_FLAGS( CellDamage::Changes )


class SheetDamage : public Damage
{
  public:

    enum Change
    {
      None              = 0x01,
      ContentChanged    = 0x02,
      PropertiesChanged = 0x04,
      Hidden            = 0x10,
      Shown             = 0x20
    };
    Q_DECLARE_FLAGS( Changes, Change )

    SheetDamage( KSpread::Sheet* sheet, Changes changes );

    virtual ~SheetDamage();

    virtual Type type() const { return Damage::Sheet; }

    KSpread::Sheet* sheet() const;

    Changes changes() const;

  private:
    class Private;
    Private * const d;
};
Q_DECLARE_OPERATORS_FOR_FLAGS( SheetDamage::Changes )


} // namespace KSpread

#endif // KSPREAD_DAMAGES
