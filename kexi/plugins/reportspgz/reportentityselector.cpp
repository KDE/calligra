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
#include "reportentityselector.h"
#include <qlayout.h>
#include <kpushbutton.h>
#include <klocalizedstring.h>

ReportEntitySelector::ReportEntitySelector(QWidget *parent)
 : QWidget(parent)
{
	layout = new QVBoxLayout(this);
	itemLabel = new KPushButton ( KIcon ( "feed-subscribe" ),i18n ( "Label" ), this );
	itemField = new KPushButton ( KIcon ( "edit-rename" ),i18n ( "Field" ), this );
	itemText = new KPushButton ( KIcon ( "insert-text" ),i18n ( "Text" ), this );
	itemLine = new KPushButton ( KIcon ( "draw-freehand" ),i18n ( "Line" ), this );
	itemBarcode = new KPushButton ( KIcon ( "insert-barcode" ),i18n ( "Barcode" ), this );
	itemImage = new KPushButton ( KIcon ( "insert-image" ),i18n ( "Image" ), this );
	itemChart = new KPushButton ( KIcon ( "view-statistics" ),i18n ( "Chart" ), this );
	
	itemLabel->setFlat(true);
	layout->addWidget(itemLabel);
	layout->addWidget(itemField);
	layout->addWidget(itemText);
	layout->addWidget(itemLine);
	layout->addWidget(itemBarcode);
	layout->addWidget(itemImage);
	layout->addWidget(itemChart);
	layout->addStretch();
	setLayout(layout);
}

ReportEntitySelector::~ReportEntitySelector()
{
}


