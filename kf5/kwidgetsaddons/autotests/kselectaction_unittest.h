/* This file is part of the KDE libraries
    Copyright (c) 2009 Daniel Calviño Sánchez <danxuliu@gmail.com>

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
    Boston, MA 02110-1301, USA.
*/

#ifndef KSELECTACTION_UNITTEST_H
#define KSELECTACTION_UNITTEST_H

#include <QtCore/QObject>

class KSelectAction_UnitTest : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    // The next 8 are from bug 205293.
    void testSetToolTipBeforeRequestingComboBoxWidget();
    void testSetToolTipAfterRequestingComboBoxWidget();
    void testSetToolTipBeforeRequestingToolButtonWidget();
    void testSetToolTipAfterRequestingToolButtonWidget();

    void testSetWhatsThisBeforeRequestingComboBoxWidget();
    void testSetWhatsThisAfterRequestingComboBoxWidget();
    void testSetWhatsThisBeforeRequestingToolButtonWidget();
    void testSetWhatsThisAfterRequestingToolButtonWidget();

    // Test for the eventFilter code.
    void testChildActionStateChangeComboMode();

    // The next 6 are from bug 203114.
    void testRequestWidgetComboBoxModeWidgetParent();
    void testRequestWidgetComboBoxModeWidgetParentSeveralActions();

    void testRequestWidgetMenuModeWidgetParent();
    void testRequestWidgetMenuModeWidgetParentSeveralActions();
    void testRequestWidgetMenuModeWidgetParentAddActions();
    void testRequestWidgetMenuModeWidgetParentRemoveActions();
};

#endif
