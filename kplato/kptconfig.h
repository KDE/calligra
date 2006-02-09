/* This file is part of the KDE project
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation;
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

#ifndef KPTCONFIG_H
#define KPTCONFIG_H

#include "kpttask.h"

namespace KPlato
{

// class Behavior {
// public:
//     enum CalculationMode { Manual, OnChange };
//     Behavior() {
//         calculationMode = Manual;
//         allowOverbooking = true;
//     }
//     int calculationMode;
//     bool allowOverbooking;
// };

class Config {
public:
    
    Config();
    ~Config();

    void setReadWrite(bool readWrite) { m_readWrite = readWrite; }
    void load();
    void save();

    Task &taskDefaults() { return m_taskDefaults; }
//    Behavior &behavior() { return m_behavior; }
    
private:
    bool m_readWrite;
//    Behavior m_behavior;  
    Task m_taskDefaults;

};

}  //KPlato namespace

#endif // CONFIG_H
