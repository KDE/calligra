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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KSPREAD_DAMAGES
#define KSPREAD_DAMAGES

#include <qvaluelist.h>

class KSpreadCell;
class KSpreadSheet;

namespace KSpread
{

class Damage
{
  public:
  
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
  
    CellDamage( KSpreadCell* cell );
    
    virtual ~CellDamage();
    
    virtual Type type() const { return Cell; }
    
    KSpreadCell* cell();
    
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
    
    SheetDamage( KSpreadSheet* sheet, int action );
    
    virtual ~SheetDamage();
  
    virtual Type type() const { return Sheet; }
    
    KSpreadSheet* sheet() const;
    
    int action() const;
    
  private:
    class Private;
    Private *d;
};


} // namespace KSpread

#endif // KSPREAD_DAMAGES
