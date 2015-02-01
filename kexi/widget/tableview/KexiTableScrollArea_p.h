/* This file is part of the KDE project
   Copyright (C) 2002 Till Busch <till@bux.at>
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003 Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2014 Jarosław Staniek <staniek@kde.org>

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

#include "KexiTableScrollArea.h"
#include "KexiTableScrollAreaHeader.h"
#include "KexiTableScrollAreaHeaderModel.h"

#include <db/roweditbuffer.h>
#include <widget/utils/kexidisplayutils.h>

#include <QEvent>
#include <QTimer>
#include <QValidator>
#include <QPixmap>
#include <QLabel>
#include <QList>
#include <QHash>
#include <QRubberBand>

class KexiTableScrollAreaWidget;
class KexiTableEdit;
class QLabel;

//! @short a dynamic tooltip for table view cells
/*! @internal */
class KexiTableViewCellToolTip : public QToolTip
{
public:
    explicit KexiTableViewCellToolTip(KexiTableScrollArea * tableView);
    virtual ~KexiTableViewCellToolTip();
protected:
    virtual void maybeTip(const QPoint & p);

    KexiTableScrollArea *m_tableView;
};

/*! KexiTableView's internal structures
 @internal */
class KexiTableScrollArea::Private
{
public:
    explicit Private(KexiTableScrollArea* t);
    ~Private();

    void clearVariables();

    KexiTableScrollArea *tv;
    KexiTableScrollAreaWidget *scrollAreaWidget;

    //! editors: one for each column (indexed by KexiDB::TableViewColumn)
    QHash<KexiDB::TableViewColumn*, KexiTableEdit*> editors;
    int rowHeight;
    QTimer *pUpdateTimer;
    int menu_id_addRecord;
    int menu_id_removeRecord;
    KexiTableScrollArea::ScrollDirection scrollDirection;
    bool editOnDoubleClick;
    bool needAutoScroll;
    bool disableDrawContents;

    /*! true if the navigation panel is enabled (visible) for the view.
     True by default. */
    bool navigatorEnabled;

    /*! true if the context menu is enabled (visible) for the view.
     True by default. */
    bool contextMenuEnabled;

    /*! used to force single skip keyPress event. */
    bool skipKeyPress;

    KexiTableScrollAreaHeaderModel *headerModel;

    KexiTableScrollAreaHeader *horizontalHeader;

    /*! Needed because horizontalHeader->isVisible() is not always accurate. True by default.  */
    bool horizontalHeaderVisible;

    KexiTableScrollAreaHeader *verticalHeader;

    /*! true, if certical header shouldn't be increased in
     KexiTableView::slotRowInserted() because it was already done
     in KexiTableView::createEditor(). */
    bool verticalHeaderAlreadyAdded;

    /*! true if cursor should be moved on mouse release evenr rather than mouse press
     in handleContentsMousePressOrRelease().
     False by default. Used by KeixComboBoxPopup. */
    bool moveCursorOnMouseRelease;

    bool firstTimeEnsureCellVisible;

    bool insideResizeEvent;

    KexiTableScrollArea::Appearance appearance;

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

    QMargins viewportMargins;

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

    //! true if this is the first call of showEvent()
    bool firstShowEvent;
};

#endif
