/* This file is part of the KDE project
   Copyright (C) 2002 Till Busch <till@bux.at>
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003 Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2008 Jarosław Staniek <staniek@kde.org>

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

   Original Author:  Till Busch <till@bux.at>
   Original Project: buX (www.bux.at)
*/

#ifndef KEXITABLEVIEW_P_H
#define KEXITABLEVIEW_P_H

#include "kexitableview.h"

#include <kexidb/roweditbuffer.h>
#include <widget/utils/kexidisplayutils.h>

#include <QEvent>
#include <QTimer>
#include <QValidator>
#include <QPixmap>
#include <QLabel>
#include <QList>
#include <QHash>
#include <QRubberBand>

#include <KPushButton>
#include <KLineEdit>
#include <KMenu>

class KexiTableEdit;
class QLabel;

//! @short a dynamic tooltip for table view cells
/*! @internal */
class KexiTableViewCellToolTip : public QToolTip
{
public:
    KexiTableViewCellToolTip(KexiTableView * tableView);
    virtual ~KexiTableViewCellToolTip();
protected:
    virtual void maybeTip(const QPoint & p);

    KexiTableView *m_tableView;
};

/*! KexiTableView's internal structures
 @internal */
class KexiTableViewPrivate
{
public:

    KexiTableViewPrivate(KexiTableView* t);
    ~KexiTableViewPrivate();

    void clearVariables();

    KexiTableView *tv;

    //! editors: one for each column (indexed by KexiTableViewColumn)
    QHash<KexiTableViewColumn*, KexiTableEdit*> editors;

    int rowHeight;

//Qt4 QPixmap *pBufferPm;
    QTimer *pUpdateTimer;
    int menu_id_addRecord;
    int menu_id_removeRecord;

#if 0//(js) doesn't work!
    QTimer *scrollTimer;
#endif

    KexiTableView::ScrollDirection scrollDirection;

    bool editOnDoubleClick : 1;

    bool needAutoScroll : 1;

    bool disableDrawContents : 1;

    /*! true if the navigation panel is enabled (visible) for the view.
     True by default. */
    bool navigatorEnabled : 1;

    /*! true if the context menu is enabled (visible) for the view.
     True by default. */
    bool contextMenuEnabled : 1;

    /*! used to force single skip keyPress event. */
    bool skipKeyPress : 1;

    /*! Needed because m_horizontalHeader->isVisible() is not always accurate. True by default.  */
    bool horizontalHeaderVisible : 1;

    /*! true if cursor should be moved on mouse release evenr rather than mouse press
     in handleContentsMousePressOrRelease().
     False by default. Used by KeixComboBoxPopup. */
    bool moveCursorOnMouseRelease : 1;

    bool firstTimeEnsureCellVisible : 1;

    KexiTableView::Appearance appearance;

    //! brushes, fonts
    QBrush diagonalGrayPattern;

    //! Parameters for displaying autonumbers
    KexiDisplayUtils::DisplayParameters autonumberSignDisplayParameters;

    //! Parameters for displaying default values
    KexiDisplayUtils::DisplayParameters defaultValueDisplayParameters;

    //! Used by delayed mode of maximizeColumnsWidth()
    QList<int> maximizeColumnsWidthOnShow;

    /*! Used for delayed call of ensureCellVisible() after show().
     It's equal to (-1,-1) if ensureCellVisible() shouldn't e called. */
    QPoint ensureCellVisibleOnShow;

    /*! @internal Changes bottom margin settings, in pixels.
     At this time, it's used by KexiComboBoxPopup to decrease margin for popup's table. */
    int internal_bottomMargin;

    /*! Helper for "highlighted row" effect. */
    int highlightedRow;

    /*! Id of context menu key (cached). */
    int contextMenuKey;

    /*! Specifies currently displayed cell tooltip.
     Value of QPoint(-1,-1) means "no tooltip". */
    QPoint recentCellWithToolTip;

    /*! Table cell tooltip */
    KexiTableViewCellToolTip *cellToolTip;

    /*! A rubber band for displaying drag indicator. */
    QRubberBand *dragIndicatorRubberBand;
};

#endif
