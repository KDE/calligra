/* This file is part of the KDE project
   Copyright (C) 2004-2009 Jarosław Staniek <staniek@kde.org>

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

#ifndef KEXIPROPERTYEDITORVIEWBASE_H
#define KEXIPROPERTYEDITORVIEWBASE_H

#include <QWidget>
#include <kexi_export.h>

namespace KoProperty
{
    class EditorView;
    class Set;
}

class KexiObjectInfoLabel;

//! @short A base class for propety pane's tabs.
/*! Defines vertical layout and offers info label on the top. 
 Use layout() to access the QVBoxLayout object.
*/
class KEXIEXTWIDGETS_EXPORT KexiPropertyPaneViewBase : public QWidget
{
    Q_OBJECT
public:
    KexiPropertyPaneViewBase(QWidget* parent);
    virtual ~KexiPropertyPaneViewBase();

    KexiObjectInfoLabel *infoLabel() const;

    /*! Helper function. Updates \a infoLabel widget by reusing properties provided
     by property set \a set.
     Read documentation of KexiPropertyEditorView class for information about accepted properties.
     If \a set is 0 and \a textToDisplayForNullSet string is not empty, this string is displayed
     (without icon or any other additional part).
     If \a set is 0 and \a textToDisplayForNullSet string is empty, the \a infoLabel widget becomes
     hidden. */
    void updateInfoLabelForPropertySet(
        KoProperty::Set* set, const QString& textToDisplayForNullSet = QString());
protected:
    class Private;
    Private * const d;
};

#endif
