/* This file is part of the KDE project
 * Made by Tomislav Lukman (tomislav.lukman@ck.tel.hr)
 * SPDX-FileCopyrightText: 2002 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
 * SPDX-FileCopyrightText: 2002-2003 Rob Buis <buis@kde.org>
 * SPDX-FileCopyrightText: 2005-2006 Tim Beaulen <tbscope@gmail.com>
 * SPDX-FileCopyrightText: 2005-2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2005-2006, 2011 Inge Wallin <inge@lysator.liu.se>
 * SPDX-FileCopyrightText: 2005-2008 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2006 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2006 Peter Simonsson <psn@linux.se>
 * SPDX-FileCopyrightText: 2006 Laurent Montel <montel@kde.org>
 * SPDX-FileCopyrightText: 2007, 2011 Thorsten Zachmann <t.zachmann@zagge.de>
 * SPDX-FileCopyrightText: 2011 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

// Own
#include "KoStrokeConfigWidget.h"

// Qt
#include <QButtonGroup>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QSizePolicy>
#include <QToolButton>
#include <QVBoxLayout>

// KF5
#include <KLocalizedString>

// Calligra
#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoCanvasResourceManager.h>
#include <KoColorPopupAction.h>
#include <KoDocumentResourceManager.h>
#include <KoIcon.h>
#include <KoLineStyleSelector.h>
#include <KoMarker.h>
#include <KoMarkerCollection.h>
#include <KoMarkerSelector.h>
#include <KoPathShape.h>
#include <KoPathShapeMarkerCommand.h>
#include <KoSelection.h>
#include <KoShapeController.h>
#include <KoShapeManager.h>
#include <KoShapeStroke.h>
#include <KoShapeStrokeCommand.h>
#include <KoShapeStrokeModel.h>
#include <KoToolManager.h>
#include <KoUnit.h>
#include <KoUnitDoubleSpinBox.h>

class CapNJoinMenu : public QMenu
{
    Q_OBJECT
public:
    CapNJoinMenu(QWidget *parent = nullptr);
    QSize sizeHint() const override;

    KoUnitDoubleSpinBox *miterLimit;
    QButtonGroup *capGroup;
    QButtonGroup *joinGroup;
};

CapNJoinMenu::CapNJoinMenu(QWidget *parent)
    : QMenu(parent)
{
    QGridLayout *mainLayout = new QGridLayout();
    mainLayout->setContentsMargins(2, 2, 2, 2);

    // The cap group
    capGroup = new QButtonGroup(this);
    capGroup->setExclusive(true);

    QToolButton *button = nullptr;

    button = new QToolButton(this);
    button->setIcon(koIcon("stroke-cap-butt"));
    button->setCheckable(true);
    button->setToolTip(i18n("Butt cap"));
    capGroup->addButton(button, Qt::FlatCap);
    mainLayout->addWidget(button, 2, 0);

    button = new QToolButton(this);
    button->setIcon(koIcon("stroke-cap-round"));
    button->setCheckable(true);
    button->setToolTip(i18n("Round cap"));
    capGroup->addButton(button, Qt::RoundCap);
    mainLayout->addWidget(button, 2, 1);

    button = new QToolButton(this);
    button->setIcon(koIcon("stroke-cap-square"));
    button->setCheckable(true);
    button->setToolTip(i18n("Square cap"));
    capGroup->addButton(button, Qt::SquareCap);
    mainLayout->addWidget(button, 2, 2, Qt::AlignLeft);

    // The join group
    joinGroup = new QButtonGroup(this);
    joinGroup->setExclusive(true);

    button = new QToolButton(this);
    button->setIcon(koIcon("stroke-join-miter"));
    button->setCheckable(true);
    button->setToolTip(i18n("Miter join"));
    joinGroup->addButton(button, Qt::MiterJoin);
    mainLayout->addWidget(button, 3, 0);

    button = new QToolButton(this);
    button->setIcon(koIcon("stroke-join-round"));
    button->setCheckable(true);
    button->setToolTip(i18n("Round join"));
    joinGroup->addButton(button, Qt::RoundJoin);
    mainLayout->addWidget(button, 3, 1);

    button = new QToolButton(this);
    button->setIcon(koIcon("stroke-join-bevel"));
    button->setCheckable(true);
    button->setToolTip(i18n("Bevel join"));
    joinGroup->addButton(button, Qt::BevelJoin);
    mainLayout->addWidget(button, 3, 2, Qt::AlignLeft);

    // Miter limit
    // set min/max/step and value in points, then set actual unit
    miterLimit = new KoUnitDoubleSpinBox(this);
    miterLimit->setMinMaxStep(0.0, 1000.0, 0.5);
    miterLimit->setDecimals(2);
    miterLimit->setUnit(KoUnit(KoUnit::Point));
    miterLimit->setToolTip(i18n("Miter limit"));
    mainLayout->addWidget(miterLimit, 4, 0, 1, 3);

    mainLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    setLayout(mainLayout);
}

QSize CapNJoinMenu::sizeHint() const
{
    return layout()->sizeHint();
}

class Q_DECL_HIDDEN KoStrokeConfigWidget::Private
{
public:
    Private()
        : canvas(nullptr)
        , active(true)
    {
    }

    KoLineStyleSelector *lineStyle;
    KoUnitDoubleSpinBox *lineWidth;
    KoMarkerSelector *startMarkerSelector;
    KoMarkerSelector *endMarkerSelector;

    CapNJoinMenu *capNJoinMenu;
    QToolButton *colorButton;
    KoColorPopupAction *colorAction;

    QWidget *spacer;

    KoCanvasBase *canvas;

    bool active;
};

KoStrokeConfigWidget::KoStrokeConfigWidget(QWidget *parent)
    : QWidget(parent)
    , d(new Private())
{
    setObjectName("Stroke widget");
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins({});

    QHBoxLayout *firstLineLayout = new QHBoxLayout();

    // Start marker
    QList<KoMarker *> markers;

    d->startMarkerSelector = new KoMarkerSelector(KoMarkerData::MarkerStart, this);
    d->startMarkerSelector->updateMarkers(markers);
    d->startMarkerSelector->setMaximumWidth(50);
    firstLineLayout->addWidget(d->startMarkerSelector);

    // Line style
    d->lineStyle = new KoLineStyleSelector(this);
    d->lineStyle->setMinimumWidth(70);
    firstLineLayout->addWidget(d->lineStyle);

    // End marker
    d->endMarkerSelector = new KoMarkerSelector(KoMarkerData::MarkerEnd, this);
    d->endMarkerSelector->updateMarkers(markers);
    d->endMarkerSelector->setMaximumWidth(50);
    firstLineLayout->addWidget(d->endMarkerSelector);

    QHBoxLayout *secondLineLayout = new QHBoxLayout();

    // Line width
    QLabel *l = new QLabel(this);
    l->setText(i18n("Thickness:"));
    l->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    l->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    secondLineLayout->addWidget(l);

    // set min/max/step and value in points, then set actual unit
    d->lineWidth = new KoUnitDoubleSpinBox(this);
    d->lineWidth->setMinMaxStep(0.0, 1000.0, 0.5);
    d->lineWidth->setDecimals(2);
    d->lineWidth->setUnit(KoUnit(KoUnit::Point));
    d->lineWidth->setToolTip(i18n("Set line width of actual selection"));
    secondLineLayout->addWidget(d->lineWidth);

    QToolButton *capNJoinButton = new QToolButton(this);
    capNJoinButton->setMinimumHeight(25);
    d->capNJoinMenu = new CapNJoinMenu(this);
    capNJoinButton->setMenu(d->capNJoinMenu);
    capNJoinButton->setText("...");
    capNJoinButton->setPopupMode(QToolButton::InstantPopup);

    secondLineLayout->addWidget(capNJoinButton);

    d->colorButton = new QToolButton(this);
    secondLineLayout->addWidget(d->colorButton);
    d->colorAction = new KoColorPopupAction(this);
    d->colorAction->setIcon(koIcon("format-stroke-color"));
    d->colorAction->setToolTip(i18n("Change the color of the line/border"));
    d->colorButton->setDefaultAction(d->colorAction);

    mainLayout->addLayout(firstLineLayout);
    mainLayout->addLayout(secondLineLayout);

    // Spacer
    d->spacer = new QWidget();
    d->spacer->setObjectName("SpecialSpacer");
    mainLayout->addWidget(d->spacer);

    // set sensitive defaults
    d->lineStyle->setLineStyle(Qt::SolidLine);
    d->lineWidth->changeValue(1);
    d->colorAction->setCurrentColor(Qt::black);

    // Make the signals visible on the outside of this widget.
    connect(d->lineStyle, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &KoStrokeConfigWidget::applyChanges);
    connect(d->lineWidth, &KoUnitDoubleSpinBox::valueChangedPt, this, &KoStrokeConfigWidget::applyChanges);
    connect(d->colorAction, &KoColorPopupAction::colorChanged, this, &KoStrokeConfigWidget::applyChanges);
    connect(d->capNJoinMenu->capGroup, &QButtonGroup::idClicked, this, &KoStrokeConfigWidget::applyChanges);
    connect(d->capNJoinMenu->joinGroup, &QButtonGroup::idClicked, this, &KoStrokeConfigWidget::applyChanges);
    connect(d->capNJoinMenu->miterLimit, &KoUnitDoubleSpinBox::valueChangedPt, this, &KoStrokeConfigWidget::applyChanges);
    connect(d->startMarkerSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &KoStrokeConfigWidget::startMarkerChanged);
    connect(d->endMarkerSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &KoStrokeConfigWidget::endMarkerChanged);
}

KoStrokeConfigWidget::~KoStrokeConfigWidget()
{
    delete d;
}

// ----------------------------------------------------------------
//                         getters and setters

Qt::PenStyle KoStrokeConfigWidget::lineStyle() const
{
    return d->lineStyle->lineStyle();
}

QVector<qreal> KoStrokeConfigWidget::lineDashes() const
{
    return d->lineStyle->lineDashes();
}

qreal KoStrokeConfigWidget::lineWidth() const
{
    return d->lineWidth->value();
}

QColor KoStrokeConfigWidget::color() const
{
    return d->colorAction->currentColor();
}

qreal KoStrokeConfigWidget::miterLimit() const
{
    return d->capNJoinMenu->miterLimit->value();
}

KoMarker *KoStrokeConfigWidget::startMarker() const
{
    return d->startMarkerSelector->marker();
}

KoMarker *KoStrokeConfigWidget::endMarker() const
{
    return d->endMarkerSelector->marker();
}

Qt::PenCapStyle KoStrokeConfigWidget::capStyle() const
{
    return static_cast<Qt::PenCapStyle>(d->capNJoinMenu->capGroup->checkedId());
}

Qt::PenJoinStyle KoStrokeConfigWidget::joinStyle() const
{
    return static_cast<Qt::PenJoinStyle>(d->capNJoinMenu->joinGroup->checkedId());
}

KoShapeStroke *KoStrokeConfigWidget::createShapeStroke() const
{
    KoShapeStroke *stroke = new KoShapeStroke();

    stroke->setColor(color());
    stroke->setLineWidth(lineWidth());
    stroke->setCapStyle(capStyle());
    stroke->setJoinStyle(joinStyle());
    stroke->setMiterLimit(miterLimit());
    stroke->setLineStyle(lineStyle(), lineDashes());

    return stroke;
}

// ----------------------------------------------------------------
//                         Other public functions

void KoStrokeConfigWidget::updateControls(KoShapeStrokeModel *stroke, KoMarker *startMarker, KoMarker *endMarker)
{
    blockChildSignals(true);

    const KoShapeStroke *lineStroke = dynamic_cast<const KoShapeStroke *>(stroke);
    if (lineStroke) {
        d->lineWidth->changeValue(lineStroke->lineWidth());
        QAbstractButton *button = d->capNJoinMenu->capGroup->button(lineStroke->capStyle());
        if (button) {
            button->setChecked(true);
        }
        button = d->capNJoinMenu->joinGroup->button(lineStroke->joinStyle());
        if (button) {
            button->setChecked(true);
        }
        d->capNJoinMenu->miterLimit->changeValue(lineStroke->miterLimit());
        d->capNJoinMenu->miterLimit->setEnabled(lineStroke->joinStyle() == Qt::MiterJoin);
        d->lineStyle->setLineStyle(lineStroke->lineStyle(), lineStroke->lineDashes());
        d->colorAction->setCurrentColor(lineStroke->color());
    } else {
        d->lineWidth->changeValue(0.0);
        d->capNJoinMenu->capGroup->button(Qt::FlatCap)->setChecked(true);
        d->capNJoinMenu->joinGroup->button(Qt::MiterJoin)->setChecked(true);
        d->capNJoinMenu->miterLimit->changeValue(0.0);
        d->capNJoinMenu->miterLimit->setEnabled(true);
        d->lineStyle->setLineStyle(Qt::NoPen, QVector<qreal>());
    }

    d->startMarkerSelector->setMarker(startMarker);
    d->endMarkerSelector->setMarker(endMarker);

    blockChildSignals(false);
}

void KoStrokeConfigWidget::setUnit(const KoUnit &unit)
{
    blockChildSignals(true);

    KoCanvasController *canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
    KoShape *shape = selection->firstSelectedShape();

    /**
     * KoStrokeShape knows nothing about the transformations applied
     * to the shape, which doesn't prevent the shape to apply them and
     * display the stroke differently. So just take that into account
     * and show the user correct values using the multiplier in KoUnit.
     */
    KoUnit newUnit(unit);
    if (shape) {
        newUnit.adjustByPixelTransform(shape->absoluteTransformation(nullptr));
    }

    d->lineWidth->setUnit(newUnit);
    d->capNJoinMenu->miterLimit->setUnit(newUnit);

    blockChildSignals(false);
}

void KoStrokeConfigWidget::updateMarkers(const QList<KoMarker *> &markers)
{
    d->startMarkerSelector->updateMarkers(markers);
    d->endMarkerSelector->updateMarkers(markers);
}

void KoStrokeConfigWidget::blockChildSignals(bool block)
{
    d->colorAction->blockSignals(block);
    d->lineWidth->blockSignals(block);
    d->capNJoinMenu->capGroup->blockSignals(block);
    d->capNJoinMenu->joinGroup->blockSignals(block);
    d->capNJoinMenu->miterLimit->blockSignals(block);
    d->lineStyle->blockSignals(block);
    d->startMarkerSelector->blockSignals(block);
    d->endMarkerSelector->blockSignals(block);
}

void KoStrokeConfigWidget::setActive(bool active)
{
    d->active = active;
}

//------------------------
void KoStrokeConfigWidget::applyChanges()
{
    KoCanvasController *canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();

    // FIXME d->canvas->resourceManager()->setActiveStroke( d->stroke );

    if (!selection || !selection->count()) {
        return;
    }

    KoShapeStroke *newStroke = new KoShapeStroke();
    KoShapeStroke *oldStroke = dynamic_cast<KoShapeStroke *>(selection->firstSelectedShape()->stroke());
    if (oldStroke) {
        newStroke->setLineBrush(oldStroke->lineBrush());
    }
    newStroke->setColor(color());
    newStroke->setLineWidth(lineWidth());
    newStroke->setCapStyle(static_cast<Qt::PenCapStyle>(d->capNJoinMenu->capGroup->checkedId()));
    newStroke->setJoinStyle(static_cast<Qt::PenJoinStyle>(d->capNJoinMenu->joinGroup->checkedId()));
    newStroke->setMiterLimit(miterLimit());
    newStroke->setLineStyle(lineStyle(), lineDashes());

    if (d->active) {
        KoShapeStrokeCommand *cmd = new KoShapeStrokeCommand(selection->selectedShapes(), newStroke);
        canvasController->canvas()->addCommand(cmd);
    }
}

void KoStrokeConfigWidget::applyMarkerChanges(KoMarkerData::MarkerPosition position)
{
    KoMarker *marker = nullptr;
    if (position == KoMarkerData::MarkerStart) {
        marker = startMarker();
    } else if (position == KoMarkerData::MarkerEnd) {
        marker = endMarker();
    }

    KoCanvasController *canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();

    if (!selection || !selection->count()) {
        return;
    }

    const QList<KoShape *> shapeList = selection->selectedShapes();
    QList<KoPathShape *> pathShapeList;
    for (auto itShape : shapeList) {
        KoPathShape *pathShape = dynamic_cast<KoPathShape *>(itShape);
        if (pathShape) {
            pathShapeList << pathShape;
        }
    }

    if (!pathShapeList.empty()) {
        KoPathShapeMarkerCommand *cmdMarker = new KoPathShapeMarkerCommand(pathShapeList, marker, position);
        canvasController->canvas()->addCommand(cmdMarker);
    }
}

void KoStrokeConfigWidget::startMarkerChanged()
{
    applyMarkerChanges(KoMarkerData::MarkerStart);
}

void KoStrokeConfigWidget::endMarkerChanged()
{
    applyMarkerChanges(KoMarkerData::MarkerEnd);
}
// ----------------------------------------------------------------

void KoStrokeConfigWidget::selectionChanged()
{
    // see a comment in setUnit()
    setUnit(d->canvas->unit());

    KoCanvasController *canvasController = KoToolManager::instance()->activeCanvasController();
    KoSelection *selection = canvasController->canvas()->shapeManager()->selection();
    KoShape *shape = selection->firstSelectedShape();
    if (shape && shape->stroke()) {
        KoPathShape *pathShape = dynamic_cast<KoPathShape *>(shape);
        if (pathShape) {
            updateControls(shape->stroke(), pathShape->marker(KoMarkerData::MarkerStart), pathShape->marker(KoMarkerData::MarkerEnd));
        } else {
            updateControls(shape->stroke(), nullptr, nullptr);
        }
    }
}

void KoStrokeConfigWidget::setCanvas(KoCanvasBase *canvas)
{
    if (canvas) {
        connect(canvas->shapeManager()->selection(), &KoSelection::selectionChanged, this, &KoStrokeConfigWidget::selectionChanged);
        connect(canvas->shapeManager(), &KoShapeManager::selectionContentChanged, this, &KoStrokeConfigWidget::selectionChanged);
        connect(canvas->resourceManager(), &KoCanvasResourceManager::canvasResourceChanged, this, &KoStrokeConfigWidget::canvasResourceChanged);
        setUnit(canvas->unit());

        KoDocumentResourceManager *resourceManager = canvas->shapeController()->resourceManager();
        if (resourceManager) {
            KoMarkerCollection *collection = resourceManager->resource(KoDocumentResourceManager::MarkerCollection).value<KoMarkerCollection *>();
            if (collection) {
                updateMarkers(collection->markers());
            }
        }
    }

    d->canvas = canvas;
}

void KoStrokeConfigWidget::canvasResourceChanged(int key, const QVariant &value)
{
    switch (key) {
    case KoCanvasResourceManager::Unit:
        setUnit(value.value<KoUnit>());
        break;
    }
}
#include "KoStrokeConfigWidget.moc"
