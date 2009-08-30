/* This file is part of the KDE project
   Copyright (C) 2006 Jarosław Staniek <staniek@kde.org>

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

#include <kexi_export.h>
#include <kpushbutton.h>

class KComboBox;

//! @short A drop-down button for combo box widgets
/*! Used in KexiComboBoxTableEdit.
*/
class KEXIGUIUTILS_EXPORT KexiComboBoxDropDownButton : public KPushButton
{
public:
    KexiComboBoxDropDownButton(QWidget *parent = 0);
    virtual ~KexiComboBoxDropDownButton();

protected:
    /*! Reimplemented after @ref KPushButton to draw drop-down arrow. */
    virtual void paintEvent(QPaintEvent *pe);

    /*! Reimplemented after @ref KPushButton to adapt size to style changes. */
    virtual bool event(QEvent *event);

    /*! Adjusts appearance for current style. */
    void styleChanged();

    int m_fixForHeight;
    bool m_drawComplexControl : 1;
    //! fake combo used only to pass it as 'this' for QStyle
    //! (because styles use \<static_cast\>)
    KComboBox *m_paintedCombo;
};

#endif
