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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIVIEWBASE_H
#define KEXIVIEWBASE_H

#include <qwidget.h>

#include "kexiactionproxy.h"

class KexiMainWindow;
class KexiDialogBase;

class KEXICORE_EXPORT KexiViewBase : public QWidget, public KexiActionProxy
{
	Q_OBJECT

	public:
		KexiViewBase(KexiMainWindow *mainWin, QWidget *parent, const char *name = 0);
		virtual ~KexiViewBase();

		//! \return kexi main window that contain this view
		inline KexiMainWindow *mainWin() const { return m_mainWin; }

		//! \return parent KexiDialogBase that contains this view, or 0 if no dialog contain this view
		KexiDialogBase* parentDialog();

		/*! \return preferred size hint, that can be used to resize the view.
		 It is computed using maximum of (a) \a otherSize and (b) current KMDI dock area's size, 
		 so the view won't exceed this maximum size. The method is used e.g. in KexiDialogBase::sizeHint().
		 If you reimplement this method, dont forget to return value of 
		 yoursize.boundedTo( KexiViewBase::preferredSizeHint(otherSize) ). */
		virtual QSize preferredSizeHint(const QSize& otherSize);

	public slots:

	signals:
		//! emitted when the view is about to close
		void closing();

	protected:
		/*! called by KexiDialogBase::switchToViewMode() right before dialog is switched to new mode
		 By default does nothing. Reimplement this if you need to do something 
		 before switching to this view.
		 return true if you accept or false if a error occupied and view shouldn't change
		 */
		virtual bool beforeSwitchTo(int mode);

		/*! called by KexiDialogBase::switchToViewMode() right after dialog is switched to new mode
		 By default does nothing. Reimplement this if you need to do something 
		 after switching to this view.
		 return true if you accept or false if a error occupied and view shouldn't change
		*/
		virtual bool afterSwitchFrom(int mode);

		/*! True if contents (data) of the view is dirty and need to be saved
		 This may or not be used, depending if changes in the dialog 
		 are saved immediately (e.g. like in datatableview) or saved by hand (by user)
		 (e.g. like in alter-table dialog).
		 Default implementation always return false. Reimplement this if you e.g. want reuse "dirty" 
		 flag from internal structures that may be changed. */
		virtual bool dirty();

		virtual void closeEvent( QCloseEvent * e );

		QString m_defaultIconName;

	private:

		KexiMainWindow *m_mainWin;

	friend class KexiDialogBase;
};

#endif

