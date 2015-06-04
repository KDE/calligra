/* This file is part of the KDE project
   Copyright (C) 2005-2015 Jarosław Staniek <staniek@kde.org>

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
*/

#ifndef KEXITABLESCROLLAREAHEADER_H
#define KEXITABLESCROLLAREAHEADER_H

#include "kexidatatable_export.h"
#include <kexi_global.h>

#include <QHeaderView>

class KexiTableScrollArea;

//! @short A header view with additional actions for use with table scroll area.
/*! Displays field description (Field::description()) text as a tool tip, if available.
 Displays tool tips if a pointed section is not wide enough to fit its label text.

 \todo react on indexChange ( int section, int fromIndex, int toIndex ) signal
*/
class KEXIDATATABLE_EXPORT KexiTableScrollAreaHeader : public QHeaderView
{
    Q_OBJECT

public:
    explicit KexiTableScrollAreaHeader(Qt::Orientation orientation, KexiTableScrollArea* parent = 0);

    virtual ~KexiTableScrollAreaHeader();

    //! @todo Support entire QBrush here?
    QColor selectionBackgroundColor() const;

    void setSelectionBackgroundColor(const QColor &color);

    //! @return the total width of all the header columns.
    int headerWidth() const;

    //! @return a suitable size hint for this header.
    virtual QSize sizeHint() const;

    //! @return preferred size for section @a logicalIndex.
    //! Preferred size is a minimal one that has still visible text and sort indicator
    //! or icons (if they are present). his method does not alanyze size of contents.
    //! @todo Currently only works for horizontal orientation.
    int preferredSectionSize(int logicalIndex) const;

    //! @return scroll area (parent) of this header.
    KexiTableScrollArea* scrollArea() const;

public Q_SLOTS:
    //! Updates the section specified by the given \a logicalIndex.
    //! Made public for use in the scroll view.
    void updateSection(int logicalIndex);

    //! Set sorting flags to @a set. Does not perform sorting, just changes visual feedback
    //! and whether clicking on section is enabled.
    void setSortingEnabled(bool set);

protected:
    virtual void changeEvent(QEvent *e);

    virtual bool viewportEvent(QEvent *e);

    //! For vertical header. We do not use sectionClicked() because we want to handle clicking on any pixel.
    virtual void mousePressEvent(QMouseEvent *e);

    virtual void mouseMoveEvent(QMouseEvent *e);

    virtual void leaveEvent(QEvent *e);

    virtual QSize sectionSizeFromContents(int logicalIndex) const;

private:
    void styleChanged();

    class Private;
    Private * const d;
};

#endif
