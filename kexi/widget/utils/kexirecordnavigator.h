/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KEXIRECORDNAVIGATOR_H
#define KEXIRECORDNAVIGATOR_H

#include <qframe.h>

class QToolButton;
class QIntValidator;
class QScrollView;
class KLineEdit;
class KexiRecordNavigatorPrivate;
class QScrollBar;

//! \brief KexiRecordNavigator class provides a record navigator.
/*! Record navigator is usually used for data tables (e.g. KexiTableView)
 or data-aware forms.
 */
class KEXIGUIUTILS_EXPORT KexiRecordNavigator : public QFrame
{
	Q_OBJECT

	public:
		KexiRecordNavigator(QWidget *parent, int leftMargin = 0, const char *name=0);
		virtual ~KexiRecordNavigator();

		void setParentView(QScrollView *view);

		/*! \return true if data inserting is enabled (the default). */
		inline bool isInsertingEnabled() const { return m_isInsertingEnabled; }

		/*! \return current record number displayed for this navigator.
		 can return 0, if the 'text box's content is cleared. */
		uint currentRecordNumber() const;

		/*! \return record count displayed for this navigator. */
		uint recordCount() const;

		/*! Sets horizontal bar's \a hbar (at the bottom) geometry so this record navigator
		 is properly positioned together with horizontal scroll bar. This method is used 
		 in QScrollView::setHBarGeometry() implementations:
		 see KexiTableView::setHBarGeometry() and KexiFormScrollView::setHBarGeometry()
		 for usage examples. */
		void setHBarGeometry( QScrollBar & hbar, int x, int y, int w, int h );

		/*! @internal used for keyboard handling. */
		virtual bool eventFilter( QObject *o, QEvent *e );

	public slots:
		/*! Sets insertingEnabled flag. If true, "+" button will be enabled. */
		void setInsertingEnabled(bool set);

		virtual void setEnabled( bool set );

		/*! Sets current record number for this navigator, 
		 i.e. a value that will be displayed in the 'record number' text box. 
		 This can also affect button's enabling and disabling. 
		 If @p r is 0, 'record number' text box's content is cleared. */
		void setCurrentRecordNumber(uint r);

		/*! Sets record count for this navigator. 
		 This can also affect button's enabling and disabling. 
		 By default count is 0. */
		void setRecordCount(uint count);

		void updateGeometry(int leftMargin);

	signals:
		void prevButtonClicked();
		void nextButtonClicked();
		void lastButtonClicked();
		void firstButtonClicked();
		void newButtonClicked();
		void recordNumberEntered( uint r );

	protected slots:
		//void slotRecordNumberReturnPressed(const QString& text);

	protected:
		void updateButtons(uint recCnt);

		QToolButton *m_navBtnFirst;
		QToolButton *m_navBtnPrev;
		QToolButton *m_navBtnNext;
		QToolButton *m_navBtnLast;
		QToolButton *m_navBtnNew;
		KLineEdit *m_navRecordNumber;
		QIntValidator *m_navRecordNumberValidator;
		KLineEdit *m_navRecordCount; //!< readonly counter
		uint m_nav1DigitWidth;
//		uint m_recordCount;
		QScrollView *m_view;
		bool m_isInsertingEnabled : 1;

		KexiRecordNavigatorPrivate *d;
};

#endif
