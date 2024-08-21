/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2015 Camilla Boemann <cbo@boemann.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ListLevelWidget.h"

#include <kcharselect.h>

#include <KoListLevelProperties.h>

#include <QDebug>
#include <QGridLayout>
#include <QPaintEvent>
#include <QPainter>

static void drawMeasureArrow(QPainter &painter, int x, int y, bool pointRight)
{
    int const d = 6;

    painter.drawLine(x, y + d, x, y - d); // |

    if (pointRight) {
        --x; // makes arrow more pointy
        painter.drawLine(x, y, x - d, y + d);
        painter.drawLine(x, y, x - d, y - d);
    } else {
        ++x; // makes arrow more pointy
        painter.drawLine(x, y, x + d, y + d);
        painter.drawLine(x, y, x + d, y - d);
    }
}

class LabelDrawingWidget : public QWidget
{
public:
    LabelDrawingWidget()
        : QWidget()
        , m_align(2)
    {
    }

    void setLabelAlign(int a)
    {
        m_align = a;
        update();
    }

protected:
    void paintEvent(QPaintEvent *ev) override
    {
        QWidget::paintEvent(ev);
        QPainter painter(this);

        QPen pen;
        pen.setWidthF(2.0);
        painter.setPen(pen);

        int x = width() / 2 + 30 * (m_align - 1);

        // the |< arrow
        drawMeasureArrow(painter, x, 5, false);
        // the - line
        painter.drawLine(x, 5, width(), 5);
    }

    int m_align;
};

ListLevelWidget::ListLevelWidget(QWidget *parent)
    : QWidget(parent)
{
    widget.setupUi(this);

    widget.format->addItems(KoOdfNumberDefinition::userFormatDescriptions());

    QGridLayout *bulletLayout = new QGridLayout();
    m_charSelect =
        new KCharSelect(nullptr, nullptr, KCharSelect::FontCombo | KCharSelect::BlockCombos | KCharSelect::CharacterTable | KCharSelect::DetailBrowser);
    bulletLayout->addWidget(m_charSelect, 0, 0);
    widget.bulletTab->setLayout(bulletLayout);

    widget.geometryGrid->addWidget(m_label = new LabelDrawingWidget, 4, 0);

    connect(widget.format, &QComboBox::currentIndexChanged, this, &ListLevelWidget::numberFormatChanged);
    connect(widget.addTabStop, &QAbstractButton::toggled, widget.relativeTabStop, &QWidget::setEnabled);
    connect(widget.labelFollowedBy, &QComboBox::currentIndexChanged, this, &ListLevelWidget::labelFollowedByChanged);
    connect(widget.alignment, &QComboBox::currentIndexChanged, this, &ListLevelWidget::alignmentChanged);
}

void ListLevelWidget::paintEvent(QPaintEvent *ev)
{
    QWidget::paintEvent(ev);
    QPainter painter(this);

    QPen pen;
    pen.setWidthF(2.0);
    painter.setPen(pen);

    QRect quickRect = widget.loremIpsum->geometry();
    int x = 15;

    // the - line at the bottom
    painter.drawLine(x, quickRect.bottom() + 15, quickRect.left(), quickRect.bottom() + 15);
    // the < arrow aligned left side of dialog
    drawMeasureArrow(painter, x, quickRect.bottom() + 15, false);

    // the two >| arrows aligned left side of text
    x = quickRect.left();
    drawMeasureArrow(painter, x, widget.aboveLoremSpacer->geometry().y() + 5, true);
    drawMeasureArrow(painter, x, quickRect.bottom() + 15, true);

    // the | line aligned left side of text
    painter.drawLine(x, widget.aboveLoremSpacer->geometry().y() + 5, x, quickRect.bottom() + 15);

    // the |< arrow for tab distance
    drawMeasureArrow(painter, x, quickRect.bottom() + 15, false);
    // the - line for tab distance
    int tabwidth = 80;
    painter.drawLine(x, quickRect.bottom() + 15, x + tabwidth, quickRect.bottom() + 15);
    // the | line for tab distance
    painter.drawLine(x + tabwidth, quickRect.bottom() + 15, x + tabwidth, quickRect.top());
    // the >| for tab distance
    drawMeasureArrow(painter, x + tabwidth, quickRect.bottom() + 15, true);
}

void ListLevelWidget::labelFollowedByChanged(int i)
{
    widget.addTabStop->setEnabled(i == 0);
    widget.relativeTabStop->setEnabled(i == 0 && widget.addTabStop->isChecked());

    m_labelFollowedByModified = true;
}

void ListLevelWidget::alignmentChanged(int a)
{
    m_label->setLabelAlign(a);
    m_alignmentModified = true;
}

void ListLevelWidget::numberFormatChanged(int index)
{
    widget.synchro->setEnabled(index == KoOdfNumberDefinition::AlphabeticLowerCase || index == KoOdfNumberDefinition::AlphabeticUpperCase);
}

void ListLevelWidget::setDisplay(const KoListLevelProperties &props)
{
    widget.blockIndent->setValue(props.margin());
    widget.blockIndentIncrease->setValue(props.marginIncrease());
    widget.distance->setValue(-props.textIndent());
    widget.alignment->setCurrentIndex(int(props.alignment()));
    m_label->setLabelAlign(int(props.alignment()));
    widget.relativeTabStop->setValue(props.tabStopPosition() - props.margin());
    widget.labelFollowedBy->setCurrentIndex(props.labelFollowedBy());
    widget.addTabStop->setChecked(props.hasTabStopPosition());

    if (props.bulletCharacter() != QChar()) {
        m_charSelect->setCurrentChar(props.bulletCharacter());
    } else {
        m_charSelect->setCurrentChar(QChar(0x2022)); // default to bullet
    }

    widget.format->setCurrentIndex(props.numberFormat());
    numberFormatChanged(props.numberFormat()); // enable/disable letterSynch checkbox
    widget.synchro->setChecked(props.letterSynchronization());
    widget.prefix->setText(props.listItemPrefix());
    widget.suffix->setText(props.listItemSuffix());
    widget.displayLevels->setValue(props.displayLevel());

    switch (props.labelType()) {
    case KoListStyle::BulletCharLabelType:
        widget.tabWidget->setCurrentIndex(1);
        break;
    case KoListStyle::ImageLabelType:
        widget.tabWidget->setCurrentIndex(2);
        break;
    default:
        widget.tabWidget->setCurrentIndex(0);
        break;
    }
}

void ListLevelWidget::save(KoListLevelProperties &props) const
{
    props.setMargin(widget.blockIndent->value());
    props.setMarginIncrease(widget.blockIndentIncrease->value());
    props.setTextIndent(-widget.distance->value());
    props.setTabStopPosition(widget.relativeTabStop->value() + props.margin());
    props.setLabelFollowedBy(KoListStyle::ListLabelFollowedBy(widget.labelFollowedBy->currentIndex()));
    if (!widget.addTabStop->isChecked()) {
        props.clearTabStopPosition();
    }

    switch (widget.tabWidget->currentIndex()) {
    case 0:
        props.setLabelType(KoListStyle::NumberLabelType);
        props.setNumberFormat(KoOdfNumberDefinition::FormatSpecification(widget.format->currentIndex()));
        props.setListItemPrefix(widget.prefix->text());
        props.setListItemSuffix(widget.suffix->text());
        props.setLetterSynchronization(widget.synchro->isChecked());
        props.setDisplayLevel(widget.displayLevels->value());
        break;
    case 1:
        props.setLabelType(KoListStyle::BulletCharLabelType);
        props.setBulletCharacter(m_charSelect->currentChar());
        break;
    case 2:
        props.setLabelType(KoListStyle::ImageLabelType);
        break;
    }
}
