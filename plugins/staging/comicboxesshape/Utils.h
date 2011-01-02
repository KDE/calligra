#include <QPoint>
#include <cmath>

inline bool near(qreal a, qreal b)
{
    return qAbs(a - b) < 1e-6;
}

inline bool near(const QPointF& a, const QPointF& b, qreal d)
{
    return (a - b).manhattanLength() < d;
}

inline qreal norm2_2(const QPointF& pt)
{
    return (pt.x() * pt.x() + pt.y() * pt.y());
}

inline qreal norm2(const QPointF& pt)
{
    return std::sqrt(norm2_2(pt));
}

// See http://local.wasp.uwa.edu.au/~pbourke/geometry/pointline/
inline qreal projection(const QLineF& l, const QPointF& pt_1 )
{
    qreal top = (pt_1.x() - l.p1().x()) * (l.p2().x() - l.p1().x()) + (pt_1.y() - l.p1().y()) * (l.p2().y() - l.p1().y());
    return top / norm2_2(l.p2() - l.p1());
}
