/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and,or
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

   Loosely based on kdevelop/src/statusbar.h
   Copyright (C) 2001 by Bernd Gehrmann <bernd@kdevelop.org>
*/

#ifndef KEXISTATUSBAR_H
#define KEXISTATUSBAR_H

//temporary
#define KexiStatusBar_KTEXTEDITOR_USED 0

#include <kdeversion.h>
#include <kstatusbar.h>
#include <qmap.h>
//Added by qt3to4:
#include <QLabel>

class QLabel;

#if KexiStatusBar_KTEXTEDITOR_USED
namespace KTextEditor { class ViewStatusMsgInterface; }
namespace KTextEditor { class ViewCursorInterface; }
#endif
namespace KParts { class Part; }

class KexiStatusBar : public KStatusBar
{
	Q_OBJECT
	public:
		KexiStatusBar( QWidget *parent=0 );
		virtual ~KexiStatusBar();
//		virtual void addWidget( QWidget *widget, int stretch = 0, bool permanent = false);

	public slots:
		virtual void setStatus(const QString &str);
		virtual void setReadOnlyFlag(bool readOnly);

	protected slots:
		virtual void cursorPositionChanged();
		virtual void activePartChanged(KParts::Part *part);
		virtual void setCursorPosition(int line, int col);

	protected:
		int m_msgID, m_readOnlyID;
//		QLabel *m_status, *m_readOnlyStatus;

#if KexiStatusBar_KTEXTEDITOR_USED
	KTextEditor::ViewCursorInterface * m_cursorIface;
	KTextEditor::ViewStatusMsgInterface * m_viewmsgIface;
#endif
	KParts::Part *m_activePart;

// still hoping for a fix for KDE-3.1
#if defined(KDE_MAKE_VERSION)
# if KDE_VERSION < KDE_MAKE_VERSION(3,1,90)
	QMap<KParts::Part*, QString> m_map;
# endif
#endif
};

#endif

