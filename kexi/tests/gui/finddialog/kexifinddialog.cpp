/* This file is part of the KDE project
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexifinddialog.h"

#include <kstandardguiitem.h>
#include <kpushbutton.h>
#include <kcombobox.h>
#include <klocale.h>

#include <qcheckbox.h>
#include <qlabel.h>

KexiFindDialog::KexiFindDialog( bool replaceMode, QWidget* parent, const char* name, bool modal )
 : KexiFindDialogBase(parent, name, modal)
 , m_replaceMode(true)
{
	m_btnFind->setIconSet(KStandardGuiItem::find().iconSet());
	m_btnClose->setIconSet(KStandardGuiItem::close().iconSet());
	setReplaceMode(replaceMode);
	m_lookIn->insertItem(i18n("(All columns)"));
}

void KexiFindDialog::setReplaceMode(bool set)
{
	if (m_replaceMode == set)
		return;
	m_replaceMode = set;
	if (m_replaceMode) {
		m_promptOnReplace->show();
		m_replaceLbl->show();
		m_textToReplace->show();
		m_btnReplace->show();
		m_btnReplaceAll->show();
	}
	else {
		m_promptOnReplace->hide();
		m_replaceLbl->hide();
		m_textToReplace->hide();
		m_btnReplace->hide();
		m_btnReplaceAll->hide();
		resize(width(),height()-30);
	}
	updateGeometry();
}

KexiFindDialog::~KexiFindDialog()
{
}

#include "kexifinddialog.moc"
