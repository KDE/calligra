/* This file is part of the KDE project
   Copyright (C) 2005-2006 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexidbtextwidgetinterface.h"
#include "kexiformdataiteminterface.h"
#include <kexidb/queryschema.h>
#include <kexiutils/utils.h>
#include <q3frame.h>
#include <qpainter.h>
//Added by qt3to4:
#include <QEvent>

KexiDBTextWidgetInterface::KexiDBTextWidgetInterface()
 : m_autonumberDisplayParameters(0)
{
}

KexiDBTextWidgetInterface::~KexiDBTextWidgetInterface()
{
	delete m_autonumberDisplayParameters;
}

void KexiDBTextWidgetInterface::setColumnInfo(KexiDB::QueryColumnInfo* cinfo, QWidget *w)
{
	if (cinfo->field->isAutoIncrement()) {
		if (!m_autonumberDisplayParameters)
			m_autonumberDisplayParameters = new KexiDisplayUtils::DisplayParameters();
		KexiDisplayUtils::initDisplayForAutonumberSign(*m_autonumberDisplayParameters, w);
	}
}

void KexiDBTextWidgetInterface::paintEvent( Q3Frame *w, bool textIsEmpty, int alignment, bool hasFocus  )
{
	KexiFormDataItemInterface *dataItemIface = dynamic_cast<KexiFormDataItemInterface*>(w);
	KexiDB::QueryColumnInfo *columnInfo = dataItemIface ? dataItemIface->columnInfo() : 0;
	if (columnInfo && columnInfo->field && dataItemIface->cursorAtNewRow() && textIsEmpty) {
		QPainter p(w);
		const int margin = w->lineWidth() + w->midLineWidth();
		if (columnInfo->field->isAutoIncrement() && m_autonumberDisplayParameters) {
			if (w->hasFocus()) {
				p.setPen(
					KexiUtils::blendedColors(
						m_autonumberDisplayParameters->textColor, w->palette().active().base(), 1, 3));
			}
			KexiDisplayUtils::paintAutonumberSign(*m_autonumberDisplayParameters, &p,
				2 + margin + w->margin(), margin, w->width() - margin*2 -2-2, 
				w->height() - margin*2 -2, alignment, hasFocus);
		}
	}
}

void KexiDBTextWidgetInterface::event( QEvent * e, QWidget *w, bool textIsEmpty )
{
	if (e->type()==QEvent::FocusIn || e->type()==QEvent::FocusOut) {
		if (m_autonumberDisplayParameters && textIsEmpty)
			w->repaint();
	}
}
