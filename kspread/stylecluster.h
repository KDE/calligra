/* This file is part of the KDE project
   Copyright (C) 2005 The KSpread Team
                              www.koffice.org/kspread

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KSPREADSTYLECLUSTER_H
#define KSPREADSTYLECLUSTER_H

#include <qobject.h>

#include <koffice_export.h>

class KSpreadStyle;
class KSpreadSheet;
class KSpreadRange;

namespace KSpread {

/**
@author Raphael Langerhorst
*/

class StyleClusterQuad;

// class StyleManipulator;


class KSPREAD_EXPORT StyleCluster : public QObject
{
  Q_OBJECT
  
  protected:
    StyleClusterQuad* m_topQuad;
    
    KSpreadSheet* m_sheet;
    
  public:
    StyleCluster(KSpreadSheet* sheet);

    ~StyleCluster();
    
    void insert( int x, int y, KSpreadStyle * style);
    
    /// TODO use manipulators
    
//     void insert( const KSpreadRange & range, const KSpreadStyle & style);
    
    const KSpreadStyle& lookup(int x, int y);

//     friend class StyleManipulator;

};

}

#endif
