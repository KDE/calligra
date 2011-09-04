/* This file is part of the KDE project
 Copyright (C) 2011 Adam Pigg <adam@piggz.co.uk>
 
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

#ifndef KEXIRECORDNAVIGATORIFACE_H
#define KEXIRECORDNAVIGATORIFACE_H

#include <kexi_export.h>
#include <QScrollBar>

class KexiRecordNavigatorHandler;

class KEXICORE_EXPORT KexiRecordNavigatorIface
{
public:
    KexiRecordNavigatorIface();
    virtual ~KexiRecordNavigatorIface();
    
    /*! Sets current record number for this navigator,
     *   i .e. a value that will be displ*ayed in the 'record number' text box.
     *   This can also affect button's enabling and disabling.
     *   If @p r is 0, 'record number' text box's content is cleared. */
    virtual void setCurrentRecordNumber(uint r) = 0;
    
    /*! Sets record count for this navigator.
     *   T his can also affect button's e*nabling and disabling.
     *   By default count is 0. */
    virtual void setRecordCount(uint count) = 0;
    
    /*! Sets insertingEnabled flag. If true, "+" button will be enabled. */
    virtual void setInsertingEnabled(bool set) = 0;
    
    /*! Sets visibility of "inserting" button. */
    virtual void setInsertingButtonVisible(bool set) = 0;
    
    /*! Sets record navigator handler. This allows to react
     *   o n actions performed within navigator and vice versa. */
    virtual void setRecordHandler(KexiRecordNavigatorHandler *handler) = 0;
    
    /*! Shows or hides "editing" indicator. */
    virtual void showEditingIndicator(bool show) = 0;
    
    /*! Sets horizontal bar's \a hbar (at the bottom) geometry so this record navigator
     *   i s properly positioned together with horizontal scroll bar. This me*thod is used
     *   in QScrollView::setHBarGeometry() implementations:
     *   see KexiTableView::setHBarGeometry() and KexiFormScrollView::setHBarGeometry()
     *  for usage examples. */
    virtual void setHBarGeometry(QScrollBar & hbar, int x, int y, int w, int h) = 0;
    
    virtual void updateGeometry(int leftMargin) = 0;
    
    /*! Sets label text at the left of the for record navigator's button.
     *   By default this label contains transla*ted "Row:" text. */
    virtual void setLabelText(const QString& text) = 0;
};

#endif
