/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXISMALLTOOLBUTTON_H
#define KEXISMALLTOOLBUTTON_H

#include <QToolButton>
#include <QPointer>
#include <KAction>
#include <kexi_export.h>

class KIcon;

//! A small tool button with icon and optional text
class KEXIEXTWIDGETS_EXPORT KexiSmallToolButton : public QToolButton
{
	Q_OBJECT

	public:
		KexiSmallToolButton(QWidget* parent = 0);

		KexiSmallToolButton(const QString& text, QWidget* parent = 0);

		KexiSmallToolButton(const KIcon& icon, const QString& text, QWidget* parent = 0);

		KexiSmallToolButton(KAction *action, QWidget* parent = 0);
		
		virtual ~KexiSmallToolButton();

		void updateAction();

		virtual void setIcon( const KIcon& icon );
		virtual void setIcon( const QString& icon );
		virtual void setText( const QString& text );

	protected:
		void update(const QString& text, const KIcon& icon, bool tipToo = false);
		void init();
		virtual void paintEvent(QPaintEvent *pe);

		QPointer<KAction> m_action;
};

#endif
