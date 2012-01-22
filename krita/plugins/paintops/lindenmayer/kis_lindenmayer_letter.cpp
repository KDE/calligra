#include "kis_lindenmayer_letter.h"


#include <Eigen/Geometry>
USING_PART_OF_NAMESPACE_EIGEN

#include <QDebug>

#include "kis_lindenmayer_paintop.h"


KisLindenmayerLetter::KisLindenmayerLetter(QPointF position, float angle, const KisLindenmayerPaintOp* paintop) : m_paintOp(paintop) {
    setParameter("position", position);
    setParameter("angle", angle);
    setParameter("age", 0.0f);
    setParameter("length", 0.0f);
    m_markedForRemoving = false;
}

KisLindenmayerLetter::KisLindenmayerLetter(const KisLindenmayerLetter& other) :
    m_paintOp(other.m_paintOp),
    m_parameters(other.m_parameters),
    m_markedForRemoving(other.m_markedForRemoving) {
}

// computed parameters: endPosition, angleToSun (points towards the mouse position), distanceToSun
QVariant KisLindenmayerLetter::getComputedParameter(QString key) const {

    if(key == "endPosition") {
        Eigen::Vector2f pos = eigenPosition();

        float angleInRadians = (((float) M_PI)/180.0f)*getParameter("angle").toFloat();
        Eigen::Rotation2D<float> rot = Eigen::Rotation2D<float>(angleInRadians);

        Eigen::Vector2f upVector(0.0f, -length()); // coord system is negative at the top

        Eigen::Vector2f result = pos + rot*upVector;
        return QPointF(result.x(), result.y());
    }

    if (key == "angleToSun" || key=="distanceToSun") {
//        QPointF qEndPos = getComputedParameter("endPosition").toPointF();
//        Eigen::Vector2f pos(qEndPos.x(), qEndPos.y());
        Eigen::Vector2f pos = eigenPosition();

        QPointF qSunPos = m_paintOp->getSunInformations().pos();
        Eigen::Vector2f sunPos(qSunPos.x(), qSunPos.y());

        Eigen::Vector2f direction = sunPos-pos;

        if(key=="angleToSun") {
            if(direction.norm() < 2)
                return 0.0f;
            direction.normalize();
//            qDebug() << "dot product: " << direction.dot(Eigen::Vector2f(0.0f, -1.0f));
            float radianAngle = acos(direction.dot(Eigen::Vector2f(0.0f, -1.0f)));
            if(direction.x() < 0) {
                radianAngle*=-1.0f;
            }
            return (180.0f/((float) M_PI))*radianAngle;
        }
        if(key=="distanceToSun") {
            return direction.norm();
        }
    }


    QString message = "unimplemented parameter key: %1" + key;
    Q_ASSERT_X(false, "getComputedParameter", message.toAscii().data());

    return QVariant();
}
