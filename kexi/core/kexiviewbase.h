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
class KexiPropertyBuffer;

namespace KexiDB {
	class SchemaData;
}

class KEXICORE_EXPORT KexiViewBase : public QWidget, public KexiActionProxy
{
	Q_OBJECT

	public:
		KexiViewBase(KexiMainWindow *mainWin, QWidget *parent, const char *name = 0);
		virtual ~KexiViewBase();

		//! \return kexi main window that contain this view
		inline KexiMainWindow *mainWin() const { return m_mainWin; }

		//! \return parent KexiDialogBase that contains this view, or 0 if no dialog contain this view
		KexiDialogBase* parentDialog() const { return m_dialog; }

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
		 \return true if you accept or false if a error occupied and view shouldn't change
		 Set \a cancelled to true and return true it there is no error but switching should 
		 be just cancelled (probably after showing some info messages).
		 */
		virtual bool beforeSwitchTo(int mode, bool &cancelled);

		/*! called by KexiDialogBase::switchToViewMode() right after dialog is switched to new mode
		 By default does nothing. Reimplement this if you need to do something 
		 after switching to this view.
		 \return true if you accept or false if a error occupied and view shouldn't change
		 Set \a cancelled to true and return true it there is no error but switching should 
		 be just cancelled (probably after showing some info messages).
		*/
		virtual bool afterSwitchFrom(int mode, bool &cancelled);

		/*! True if contents (data) of the view is dirty and need to be saved
		 This may or not be used, depending if changes in the dialog 
		 are saved immediately (e.g. like in datatableview) or saved by hand (by user)
		 (e.g. like in alter-table dialog).
		 "Dirty" flag is reused by KexiDialogBase::dirty().
		 Default implementation just uses internal m_dirty flag, that is false by default.
		 Reimplement this if you e.g. want reuse other "dirty" 
		 flag from internal structures that may be changed. */
		virtual bool dirty() const { return m_dirty; }

		/*! \sets dirty flag on or off. It the flag changes, 
		 dirty(bool) signal is emitted by parent dialog (KexiDialog),
		 to inform the world about that. 
		 Always use this function to update 'dirty' flag information. */
		void setDirty(bool set = true);

		virtual void closeEvent( QCloseEvent * e );

		/*! \return a property buffer for this view. For reimplementation. By default returns NULL. */
		virtual KexiPropertyBuffer *propertyBuffer();

		/*! Call this in your view's implementation whenever current property buffer 
		 (returned by propertyBuffer()) is switched to other,
		 so property editor contents need to be completely replaced. */
		void propertyBufferSwitched();

		/*! Call this in your view's implementation whenever current property buffer 
		 is changed that few properties are now visible and/or few other are invisible,
		 so property editor operating on this buffer should be completely reloaded. */
		void propertyBufferReloaded();

		/*! Tells this dialog to create and store data of the new object
		 pointed by \a sdata on the backend. 
		 Called by KexiDialogBase::storeNewData().
		 Default implementation:
		 - makes a deep copy of \a sdata
		 - stores object schema data \sdata in 'kexi__objects' internal table
		   using Connection::storeObjectSchemaData().
		 Reimpelment this for your needs. 
		 Requirements: 
		 - deep copy of \a sdata should be made
		 - schema data should be created at the backend
		   using Connection::storeObjectSchemaData() or more specialized 
		   method. For example, KexiAlterTableDialog 
		   uses Connection::createTable(TableSchema) for this 
		   (tableschema is SchemaData subclass) to store more information than 
		   just a schem adata. You should use such subclasses if needed too.
		 Should return newly created schema data obejct on success. */
		virtual KexiDB::SchemaData* storeNewData(const KexiDB::SchemaData& sdata);

		/*! Tells this view to store data changes on the backend. 
		 Called by KexiDialogBase::storeData().
		 Default implementation:
		 - makes a deep copy of \a sdata
		 - stores object schema data \sdata in 'kexi__objects' internal table
		   using Connection::storeObjectSchemaData().
		 Reimpelment this for your needs. Should return true on success. 
		 \sa storeNewData() */
		virtual bool storeData();

		QString m_defaultIconName;

		KexiMainWindow *m_mainWin;

		KexiDialogBase *m_dialog;
	private:
		bool m_dirty : 1;

	friend class KexiDialogBase;
};

#endif

