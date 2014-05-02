#include "kis_radiaxial_stroke_shortcut.h"

class KisRadiaxialStrokeShortcut::Private
{
public:
    QPointF *initial, *current; // pixel position
    int thresholdRadius; // pixels; pre-squared distance
    double angleOffset; // degrees
    int numAxes;
    bool isAxisFinalized;
    QLineF *axis; // unit vector from initial towards current
    ~Private()
    {
        delete initial;
        delete current;
    }
};

KisRadiaxialStrokeShortcut::KisRadiaxialStrokeShortcut(KisAbstractInputAction* inputAction, int index)
    : KisStrokeShortcut(inputAction, index)
    , m_d(new Private)
{
    m_d->initial = new QPointF(0.0f, 0.0f);
    m_d->current = new QPointF(0.0f, 0.0f);
    m_d->numAxes = 2;
    m_d->thresholdRadius = -1;
    m_d->angleOffset = 0;
    m_d->isAxisFinalized = false;
}

KisRadiaxialStrokeShortcut::KisRadiaxialStrokeShortcut(KisAbstractInputAction* inputAction, int index,
                                                       QPointF const& initialPoint)
    : KisStrokeShortcut(inputAction, index)
    , m_d(new Private)
{
    m_d->initial = new QPointF(initialPoint.x(), initialPoint.y());
    m_d->current = new QPointF(initialPoint.x(), initialPoint.y());
    m_d->thresholdRadius = -1;
    m_d->angleOffset = 0;
    m_d->isAxisFinalized = false;
}

KisRadiaxialStrokeShortcut::~KisRadiaxialStrokeShortcut()
{
    delete m_d;
}

KisRadiaxialStrokeShortcut::KisRadiaxialStrokeShortcut
        (const KisRadiaxialStrokeShortcut& src)
    : KisStrokeShortcut(src)
    , m_d(new Private)
{
    m_d->angleOffset = src.m_d->angleOffset;
    m_d->current = new QPointF(*(src.m_d->current));
    m_d->initial = new QPointF(*(src.m_d->initial));
    m_d->isAxisFinalized = src.m_d->isAxisFinalized;
    m_d->thresholdRadius = src.m_d->thresholdRadius;
}

KisRadiaxialStrokeShortcut::KisRadiaxialStrokeShortcut
        (KisAbstractInputAction* inputAction, int index, QPointF const& initialPoint,
         int distanceThreshold)
    : KisStrokeShortcut(inputAction, index)
    , m_d(new Private)
{
    m_d->angleOffset = 0;
    m_d->current = new QPointF(initialPoint);
    m_d->initial = new QPointF(initialPoint);
    if (distanceThreshold >= 0)
    {
        m_d->thresholdRadius = distanceThreshold * distanceThreshold;
    }
    else
    {
        m_d->thresholdRadius = -1;
    }
    m_d->isAxisFinalized = false;
}

void KisRadiaxialStrokeShortcut::setInitialPoint(QPointF const& newPoint)
{
    m_d->initial->setX(newPoint.x());
    m_d->initial->setY(newPoint.y());
}

void KisRadiaxialStrokeShortcut::setCurrentPoint(QPointF const& newPoint)
{
    m_d->current->setX(newPoint.x());
    m_d->current->setY(newPoint.y());
    if (!isAxisFinalized())
    {
        // check to see if the threshold has been breached
        float dx = m_d->current->x() - m_d->initial->x();
        float dy = m_d->current->y() - m_d->initial->y();
        if (dx*dx + dy*dy >= m_d->thresholdRadius)
        {
            m_d->isAxisFinalized = true;
        }
    }
}

void KisRadiaxialStrokeShortcut::setDistanceThreshold(int distance /* px, not squared */)
{
    m_d->thresholdRadius = distance * distance;
}

QLineF const& KisRadiaxialStrokeShortcut::getAxis()
{
    m_d->axis->setP1(*(m_d->current));
    // naive algorithm: just go through each possible axis and see how far from each the
    // currently-defined line is
    QLineF testAxis;
    testAxis.setLine(m_d->current->x(), m_d->current->y(), m_d->current->x()+1, m_d->current->y());
    testAxis.setLength(1);
    for (int axis = 0; axis < )
    return *(m_d->axis);
}

bool KisRadiaxialStrokeShortcut::isAxisFinalized()
{
    return m_d->isAxisFinalized;
}
