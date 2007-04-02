/* This file is part of the KDE project
   Copyright (C) 2005 Frédéric Lambert <konkistadorr@gmail.com>

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

#ifndef KPTCHARTDIALOG_H
#define KPTCHARTDIALOG_H

#include <kdialog.h>
#include "kptchartpanel.h"
class QWidget;

class K3Command;

namespace KPlato
{

//class KChart;
class Part;

class ChartDialog : public KDialog {
    Q_OBJECT
public:
    explicit ChartDialog(Project &p,QWidget *parent=0, const char *name=0);
   ~ChartDialog();
    void resizeEvent(QResizeEvent* ev);
    ChartPanel * m_panel;
    
};

} //namespace KPlato

#endif
