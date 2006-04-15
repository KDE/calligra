/* This file is part of the KDE project
   Copyright 2004 Ariya Hidayat <ariya@kde.org>

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

    CellDamage( KSpread::Cell* cell );

    virtual ~CellDamage();

    virtual Type type() const { return Damage::Cell; }

    KSpread::Cell* cell();

  private:
    class Private;
    Private *d;
};

class SheetDamage : public Damage
{
  public:

    enum
    {
      None = 0,
      ContentChanged,
      PropertiesChanged,
      Hidden,
      Shown
    };

    SheetDamage( KSpread::Sheet* sheet, int action );

    virtual ~SheetDamage();

    virtual Type type() const { return Damage::Sheet; }

    KSpread::Sheet* sheet() const;

    int action() const;

  private:
    class Private;
    Private *d;
};


} // namespace KSpread

#endif // KSPREAD_DAMAGES
