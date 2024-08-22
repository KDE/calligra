/* This file is part of the KDE project
   Made by Tomislav Lukman (tomislav.lukman@ck.t-com.hr)
   SPDX-FileCopyrightText: 2005 Tomislav Lukman <tomislav.lukman@ck.t-com.hr>
   SPDX-FileCopyrightText: 2006 Tim Beaulen <tbscope@gmail.com>
   SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KarbonSmallStylePreview.h"
#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoCheckerBoardPainter.h>
#include <KoGradientBackground.h>
#include <KoGradientHelper.h>
#include <KoSelection.h>
#include <KoShape.h>
#include <KoShapeManager.h>
#include <KoShapePaintingContext.h>
#include <KoShapeStroke.h>
#include <KoToolManager.h>
#include <KoViewConverter.h>

#include <KLocalizedString>

#include <QFontDatabase>
#include <QHBoxLayout>
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>

#define FRAMEWIDTH 75
#define FRAMEHEIGHT 15

class KarbonFillStyleWidget : public QPushButton
{
public:
    KarbonFillStyleWidget(QWidget *parent)
        : QPushButton(parent)
        , m_fill(nullptr)
        , m_checkerPainter(5)
    {
        setCursor(Qt::PointingHandCursor);
        setToolTip(i18n("Press to apply fill to selection"));
    }

    ~KarbonFillStyleWidget() override = default;

    void setFill(QSharedPointer<KoShapeBackground> fill)
    {
        m_fill = fill;
        update();
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        QPainter painter(this);
        painter.setClipRect(event->rect());

        if (m_fill) {
            m_checkerPainter.paint(painter, rect());

            QSharedPointer<KoGradientBackground> gradientFill = qSharedPointerDynamicCast<KoGradientBackground>(m_fill);
            if (gradientFill) {
                const QGradient *gradient = gradientFill->gradient();
                QGradient *defGradient = KoGradientHelper::defaultGradient(gradient->type(), gradient->spread(), gradient->stops());
                QBrush brush(*defGradient);
                delete defGradient;
                painter.setBrush(brush);
                painter.setPen(Qt::NoPen);
                painter.drawRect(rect());
            } else {
                // use the background to draw
                painter.setPen(Qt::NoPen);
                QPainterPath p;
                p.addRect(rect());
                KoViewConverter converter;
                KoShapePaintingContext context;
                m_fill->paint(painter, converter, context, p);
            }
        } else {
            painter.setFont(QFontDatabase::systemFont(QFontDatabase::SmallestReadableFont));
            painter.setBrush(Qt::black);
            painter.setPen(Qt::black);
            painter.drawText(rect(), Qt::AlignCenter, i18nc("The style has no fill", "None"));
        }

        painter.end();

        // QPushButton::paintEvent( event );
    }

private:
    QSharedPointer<KoShapeBackground> m_fill; ///< the fill to preview
    KoCheckerBoardPainter m_checkerPainter;
};

class KarbonStrokeStyleWidget : public QPushButton
{
public:
    KarbonStrokeStyleWidget(QWidget *parent)
        : QPushButton(parent)
        , m_stroke(nullptr)
        , m_checkerPainter(5)
    {
        setCursor(Qt::PointingHandCursor);
        setToolTip(i18n("Press to apply stroke to selection"));
    }

    ~KarbonStrokeStyleWidget() override
    {
        if (m_stroke && !m_stroke->deref())
            delete m_stroke;
    }

    void setStroke(KoShapeStrokeModel *stroke)
    {
        if (stroke != m_stroke) {
            if (m_stroke && !m_stroke->deref())
                delete m_stroke;
            m_stroke = stroke;
            if (m_stroke)
                m_stroke->ref();
        }
        update();
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        QPainter painter(this);
        painter.setClipRect(event->rect());

        if (m_stroke) {
            m_checkerPainter.paint(painter, rect());
            const KoShapeStroke *line = dynamic_cast<const KoShapeStroke *>(m_stroke);
            if (line) {
                painter.setPen(Qt::NoPen);
                QBrush brush = line->lineBrush();
                if (brush.gradient()) {
                    QGradient *defGradient = KoGradientHelper::defaultGradient(brush.gradient()->type(), brush.gradient()->spread(), brush.gradient()->stops());
                    QBrush brush(*defGradient);
                    delete defGradient;
                    painter.setBrush(brush);
                    painter.setPen(Qt::NoPen);
                    painter.drawRect(rect());
                } else if (brush.style() == Qt::TexturePattern) {
                    painter.fillRect(rect(), brush);
                } else {
                    painter.fillRect(rect(), QBrush(line->color()));
                }
            } else {
                painter.setFont(QFontDatabase::systemFont(QFontDatabase::SmallestReadableFont));
                painter.setBrush(Qt::black);
                painter.setPen(Qt::black);
                painter.drawText(rect(), Qt::AlignCenter, i18nc("The style has a custom stroking", "Custom"));
            }
        } else {
            painter.setFont(QFontDatabase::systemFont(QFontDatabase::SmallestReadableFont));
            painter.setBrush(Qt::black);
            painter.setPen(Qt::black);
            painter.drawText(rect(), Qt::AlignCenter, i18nc("The style has no stroking", "None"));
        }

        painter.end();

        // QPushButton::paintEvent( event );
    }

private:
    KoShapeStrokeModel *m_stroke; ///< the stroke to preview
    KoCheckerBoardPainter m_checkerPainter;
};

KarbonSmallStylePreview::KarbonSmallStylePreview(QWidget *parent)
    : QWidget(parent)
{
    setFont(QFontDatabase::systemFont(QFontDatabase::SmallestReadableFont));

    /* Create widget layout */
    QHBoxLayout *layout = new QHBoxLayout(this);
    QLabel *strokeLabel = new QLabel(i18n("Stroke:"), this);
    strokeLabel->setMinimumHeight(FRAMEHEIGHT);
    m_strokeFrame = new KarbonStrokeStyleWidget(this);
    m_strokeFrame->setMinimumSize(QSize(FRAMEWIDTH, FRAMEHEIGHT));

    QLabel *fillLabel = new QLabel(i18n("Fill:"), this);
    fillLabel->setMinimumHeight(FRAMEHEIGHT);
    m_fillFrame = new KarbonFillStyleWidget(this);
    m_fillFrame->setMinimumSize(QSize(FRAMEWIDTH, FRAMEHEIGHT));

    layout->addWidget(strokeLabel);
    layout->addWidget(m_strokeFrame);
    layout->addWidget(fillLabel);
    layout->addWidget(m_fillFrame);
    layout->setContentsMargins(0, 0, 0, 0);

    setLayout(layout);

    connect(KoToolManager::instance(), &KoToolManager::changedCanvas, this, &KarbonSmallStylePreview::canvasChanged);
    connect(m_strokeFrame, &QAbstractButton::clicked, this, &KarbonSmallStylePreview::strokeApplied);
    connect(m_fillFrame, &QAbstractButton::clicked, this, &KarbonSmallStylePreview::fillApplied);
}

KarbonSmallStylePreview::~KarbonSmallStylePreview() = default;

void KarbonSmallStylePreview::canvasChanged(const KoCanvasBase *canvas)
{
    if (canvas) {
        connect(canvas->shapeManager(), &KoShapeManager::selectionChanged, this, &KarbonSmallStylePreview::selectionChanged);
        connect(canvas->shapeManager(), &KoShapeManager::selectionContentChanged, this, &KarbonSmallStylePreview::selectionChanged);
    }
    selectionChanged();
}

void KarbonSmallStylePreview::selectionChanged()
{
    KoCanvasController *controller = KoToolManager::instance()->activeCanvasController();
    if (!controller || !controller->canvas()) {
        m_fillFrame->setFill(QSharedPointer<KoShapeBackground>(nullptr));
        m_strokeFrame->setStroke(nullptr);
        QWidget::update();
        return;
    }

    KoShape *shape = controller->canvas()->shapeManager()->selection()->firstSelectedShape();
    if (shape) {
        m_fillFrame->setFill(shape->background());
        m_strokeFrame->setStroke(shape->stroke());
    } else {
        m_fillFrame->setFill(QSharedPointer<KoShapeBackground>(nullptr));
        m_strokeFrame->setStroke(nullptr);
    }
    QWidget::update();
}
