#ifndef KIS_LINDENMAYER_LETTER_H
#define KIS_LINDENMAYER_LETTER_H

#include <Eigen/Core>
#include <QList>
#include <QPointF>

class KisLindenmayerLetter
{
private:
    Eigen::Vector2f m_position;
    Eigen::Vector2f m_growDirection;
    float m_age;

public:
    KisLindenmayerLetter(const Eigen::Vector2f& position, const Eigen::Vector2f& m_growDirection);
    KisLindenmayerLetter(const KisLindenmayerLetter& other);

    void grow(float time, QList<KisLindenmayerLetter*>& leafs);
    QPointF position() const {
        return QPointF(m_position.x(), m_position.y());
    }
};

#endif // KIS_LINDENMAYER_LETTER_H
