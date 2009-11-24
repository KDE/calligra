/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "reportpropertiesbutton.h"
#include <QPainter>
#include <KColorScheme>

ReportPropertiesButton::ReportPropertiesButton(QWidget* parent) :  QCheckBox(parent) {
  
}

void ReportPropertiesButton::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    KColorScheme colorScheme(QPalette::Active, KColorScheme::Window);
    
    painter.setPen(QPen(colorScheme.foreground(KColorScheme::InactiveText),2));
    painter.drawRect(4, 4, width() - 8, height() - 8);
    
    if (checkState()) {
      painter.fillRect(7, 7, width() - 14, height() - 14, colorScheme.foreground(KColorScheme::InactiveText));
    }
    //QCheckBox::paintEvent(event);
}

