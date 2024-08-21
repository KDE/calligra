/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "SimpleTableWidget.h"
#include "TextTool.h"

#include <KoColor.h>
#include <KoColorPopupAction.h>
#include <KoIcon.h>
#include <KoTableCellStyle.h>
#include <KoTextLayoutCellHelper.h>
#include <KoZoomHandler.h>

#include <QAction>
#include <QDebug>

#include <QPixmap>
#include <QWidget>

SimpleTableWidget::SimpleTableWidget(TextTool *tool, QWidget *parent)
    : QWidget(parent)
    , m_blockSignals(false)
    , m_tool(tool)
    , m_lastStyleEmitted(2)
{
    widget.setupUi(this);
    widget.addRowAbove->setDefaultAction(tool->action("insert_tablerow_above"));
    widget.addRowBelow->setDefaultAction(tool->action("insert_tablerow_below"));
    widget.addColumnLeft->setDefaultAction(tool->action("insert_tablecolumn_left"));
    widget.addColumnRight->setDefaultAction(tool->action("insert_tablecolumn_right"));
    widget.deleteRow->setDefaultAction(tool->action("delete_tablerow"));
    widget.deleteColumn->setDefaultAction(tool->action("delete_tablecolumn"));
    widget.mergeCells->setDefaultAction(tool->action("merge_tablecells"));
    widget.splitCells->setDefaultAction(tool->action("split_tablecells"));

    connect(tool->action("activate_borderpainter"), &QAction::triggered, this, &SimpleTableWidget::restartPainting);
    widget.border->setDefaultAction(tool->action("activate_borderpainter"));

    fillBorderButton(QColor(0, 0, 0));

    KoColorPopupAction *actionBorderColor = new KoColorPopupAction(this);
    actionBorderColor->setIcon(koIcon("format-fill-color"));
    actionBorderColor->setText(i18n("Set Border Color..."));
    widget.border->addAction(actionBorderColor);
    connect(actionBorderColor, &KoColorPopupAction::colorChanged, this, &SimpleTableWidget::setBorderColor);

    connect(widget.addRowAbove, &QAbstractButton::clicked, this, &SimpleTableWidget::doneWithFocus);
    connect(widget.addRowBelow, &QAbstractButton::clicked, this, &SimpleTableWidget::doneWithFocus);
    connect(widget.addColumnLeft, &QAbstractButton::clicked, this, &SimpleTableWidget::doneWithFocus);
    connect(widget.addColumnRight, &QAbstractButton::clicked, this, &SimpleTableWidget::doneWithFocus);
    connect(widget.deleteRow, &QAbstractButton::clicked, this, &SimpleTableWidget::doneWithFocus);
    connect(widget.deleteColumn, &QAbstractButton::clicked, this, &SimpleTableWidget::doneWithFocus);
    connect(widget.mergeCells, &QAbstractButton::clicked, this, &SimpleTableWidget::doneWithFocus);
    connect(widget.splitCells, &QAbstractButton::clicked, this, &SimpleTableWidget::doneWithFocus);
    connect(widget.border, &FormattingButton::itemTriggered, this, &SimpleTableWidget::emitTableBorderDataUpdated);
    connect(widget.border, &QAbstractButton::clicked, this, &SimpleTableWidget::doneWithFocus);
    connect(widget.border, &FormattingButton::doneWithFocus, this, &SimpleTableWidget::doneWithFocus);
}

void SimpleTableWidget::restartPainting()
{
    emitTableBorderDataUpdated(m_lastStyleEmitted);
}

void SimpleTableWidget::emitTableBorderDataUpdated(int i)
{
    m_lastStyleEmitted = i;
    Q_EMIT tableBorderDataUpdated(m_cellStyles[i - 1]->getEdge(KoBorder::TopBorder));
}

void SimpleTableWidget::setStyleManager(KoStyleManager *sm)
{
    m_styleManager = sm;
}

void SimpleTableWidget::setBorderColor(const KoColor &koColor)
{
    fillBorderButton(koColor.toQColor());
}

void SimpleTableWidget::fillBorderButton(const QColor &color)
{
    ItemChooserAction *chooser = widget.border->addItemChooser(9);

    qDeleteAll(m_cellStyles);
    m_cellStyles.clear();

    qreal thickness[9] = {0.25, 0.5, 0.75, 1.0, 1.5, 2.25, 3.0, 4.5, 6.0};

    KoTableCellStyle cellStyle;
    qDeleteAll(m_cellStyles);
    m_cellStyles.append(KoTableCellStyle().clone());
    for (int i = 8; i < 9; i++) {
        cellStyle.setEdge(KoBorder::TopBorder, KoBorder::BorderDoubleWave, thickness[i], color);
        m_cellStyles.append(cellStyle.clone());
    }
    for (int i = 6; i < 8; i++) {
        cellStyle.setEdge(KoBorder::TopBorder, KoBorder::BorderWave, thickness[i], color);
        m_cellStyles.append(cellStyle.clone());
    }
    for (int i = 4; i < 9; i++) {
        cellStyle.setEdge(KoBorder::TopBorder, KoBorder::BorderDouble, thickness[i] * 1.5, color);
        cellStyle.setEdgeDoubleBorderValues(KoBorder::TopBorder, thickness[i], thickness[i] / 4);
        m_cellStyles.append(cellStyle.clone());
    }
    for (int i = 6; i < 7; i++) {
        cellStyle.setEdge(KoBorder::TopBorder, KoBorder::BorderSlash, thickness[i], color);
        m_cellStyles.append(cellStyle.clone());
    }
    m_cellStyles.append(nullptr);
    for (int i = 0; i < 7; i++) {
        cellStyle.setEdge(KoBorder::TopBorder, KoBorder::BorderDouble, thickness[i] * 3, color);
        m_cellStyles.append(cellStyle.clone());
    }
    for (int i = 0; i < 9; i++) {
        cellStyle.setEdge(KoBorder::TopBorder, KoBorder::BorderDouble, thickness[i] * 2, color);
        cellStyle.setEdgeDoubleBorderValues(KoBorder::TopBorder, thickness[i] / 2, thickness[i] / 2);
        m_cellStyles.append(cellStyle.clone());
    }
    for (int i = 0; i < 9; i++) {
        cellStyle.setEdge(KoBorder::TopBorder, KoBorder::BorderSolid, thickness[i], color);
        m_cellStyles.append(cellStyle.clone());
    }
    for (int i = 0; i < 9; i++) {
        cellStyle.setEdge(KoBorder::TopBorder, KoBorder::BorderDotted, thickness[i], color);
        m_cellStyles.append(cellStyle.clone());
    }
    for (int i = 0; i < 9; i++) {
        cellStyle.setEdge(KoBorder::TopBorder, KoBorder::BorderDashed, thickness[i], color);
        m_cellStyles.append(cellStyle.clone());
    }
    for (int i = 0; i < 9; i++) {
        cellStyle.setEdge(KoBorder::TopBorder, KoBorder::BorderDashedLong, thickness[i], color);
        m_cellStyles.append(cellStyle.clone());
    }
    for (int i = 0; i < 9; i++) {
        cellStyle.setEdge(KoBorder::TopBorder, KoBorder::BorderDashDot, thickness[i], color);
        m_cellStyles.append(cellStyle.clone());
    }
    for (int i = 0; i < 9; i++) {
        cellStyle.setEdge(KoBorder::TopBorder, KoBorder::BorderDashDotDot, thickness[i], color);
        m_cellStyles.append(cellStyle.clone());
    }

    int i = 1;
    KoZoomHandler zoomHandler;
    foreach (KoTableCellStyle *style, m_cellStyles) {
        if (style == nullptr) {
            widget.border->addBlanks(chooser, 1);
            i++;
            continue;
        }
        QPixmap pm(48, 16);

        pm.fill(Qt::transparent);
        QPainter p(&pm);
        p.setRenderHint(QPainter::Antialiasing);

        if (style->hasBorders()) {
            p.scale(zoomHandler.zoomedResolutionX(), zoomHandler.zoomedResolutionY());
            KoTextLayoutCellHelper cellStyleHelper(*style);
            qreal width = style->topBorderWidth();
            cellStyleHelper.drawTopHorizontalBorder(p,
                                                    0,
                                                    8 / zoomHandler.zoomedResolutionY() - width / 2,
                                                    pm.width() / zoomHandler.zoomedResolutionX(),
                                                    nullptr);

            widget.border->addItem(chooser, pm, i, KoUnit().toUserStringValue(style->topBorderWidth()) + "pt");
        } else {
            p.drawText(0, 0, 48, 16, Qt::AlignCenter, i18nc("No border - has to fit in 48pixels", "None"));
            widget.border->addItem(chooser, pm, i, i18n("No Border"));
        }
        i++;
    }
    widget.border->setItemsBackground(chooser, QColor(Qt::white));

    // widget.borderType->addItem("None");

    widget.border->addSeparator();

    /*
        //TODO: Uncomment the below line when the string freeze is over
        //action->setToolTip(i18n("Change the level the list is at"));

        QMenu *listLevelMenu = new QMenu();
        const int levelIndent = 13;
        for (int level = 0; level < 10; ++level) {
            QWidgetAction *wa = new QWidgetAction(listLevelMenu);
            ListLevelChooser *chooserWidget = new ListLevelChooser((levelIndent * level) + 5);
            wa->setDefaultWidget(chooserWidget);
            listLevelMenu->addAction(wa);
            m_mapper->setMapping(wa,level + 1);
            connect(chooserWidget, SIGNAL(clicked()), wa, SLOT(trigger()));
            connect(wa, SIGNAL(triggered()), m_mapper, SLOT(map()));
        }

        action->setMenu(listLevelMenu);
        */
}
