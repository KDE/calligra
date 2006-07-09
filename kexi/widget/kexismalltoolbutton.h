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

#include <qtoolbutton.h>
#include <kaction.h>

class QIconSet;

//! A small tool button with icon and optional text
class KEXIEXTWIDGETS_EXPORT KexiSmallToolButton : public QToolButton
{
	Q_OBJECT

	public:
		KexiSmallToolButton(QWidget* parent, const QString& text, 
			const QString& icon = QString::null, const char* name = 0);

		KexiSmallToolButton(QWidget* parent, const QString& text, 
			const QIconSet& iconSet, const char* name = 0);

		KexiSmallToolButton(QWidget* parent, KAction *action);
		
		virtual ~KexiSmallToolButton();

		void updateAction();

		virtual void setIconSet( const QIconSet& iconSet );
		virtual void setIconSet( const QString& icon );
		virtual void setTextLabel( const QString & newLabel, bool tipToo );
		virtual void setTextLabel( const QString & newLabel ) { setTextLabel(newLabel, false); }

	protected:
		void update(const QString& text, const QIconSet& iconSet, bool tipToo = false);
		void init();
		virtual void drawButton( QPainter *_painter );

		QGuardedPtr<KAction> m_action;
};

#endif
