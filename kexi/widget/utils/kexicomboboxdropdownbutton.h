/* This file is part of the KDE project
   Copyright (C) 2006 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KexiComboBoxDropDownButton_H
#define KexiComboBoxDropDownButton_H

#include <kpushbutton.h>

class KComboBox;

//! @short A drop-down button for combo box widgets
/*! Used in KexiComboBoxTableEdit. 
*/
class KEXIGUIUTILS_EXPORT KexiComboBoxDropDownButton : public KPushButton
{
	public:
		KexiComboBoxDropDownButton( QWidget *parent );
		virtual ~KexiComboBoxDropDownButton();

	protected:
		/*! Reimplemented after @ref KPushButton to draw drop-down arrow. */
		virtual void drawButton(QPainter *p);

		/*! Reimplemented after @ref KPushButton to adapt size to style changes. */
		virtual void styleChange( QStyle & oldStyle );

		int m_fixForHeight;
		bool m_drawComplexControl : 1;
		KComboBox *m_paintedCombo; //!< fake combo used only to pass it as 'this' for QStyle 
		                           //!< (because styles use \<static_cast\>)
};

#endif
