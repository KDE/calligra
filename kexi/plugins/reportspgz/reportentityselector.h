/*
 * Kexi Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)                  
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */
#ifndef REPORTENTITYSELECTOR_H
#define REPORTENTITYSELECTOR_H

#include <QWidget>
class KPushButton;
class QVBoxLayout;

//This is a temporary solution until something better is found
/**
	@author 
*/
class ReportEntitySelector : public QWidget
{
Q_OBJECT
public:
    ReportEntitySelector(QWidget *parent = 0);

    ~ReportEntitySelector();

    QVBoxLayout *layout;
    KPushButton *itemLabel;
    KPushButton *itemField;
    KPushButton *itemText;
    KPushButton *itemLine;
    KPushButton *itemBarcode;
    KPushButton *itemImage;
    KPushButton *itemChart;
    
};

#endif
