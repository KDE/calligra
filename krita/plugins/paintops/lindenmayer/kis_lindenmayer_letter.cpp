#include "kis_lindenmayer_letter.h"

#include <Eigen/Geometry>
USING_PART_OF_NAMESPACE_EIGEN


KisLindenmayerLetter::KisLindenmayerLetter(const Eigen::Vector2f& position, const Eigen::Vector2f& growDirection) : m_position(position), m_growDirection(growDirection) {
    m_age = 0;
}

KisLindenmayerLetter::KisLindenmayerLetter(const KisLindenmayerLetter& other) : m_position(other.m_position), m_growDirection(other.m_growDirection), m_age(other.m_age) {
}

void KisLindenmayerLetter::grow(float time, QList<KisLindenmayerLetter*>& leafs) {
    m_age+=time;
    if(m_age>40) {
//        Eigen::Transform<float> rot(Eigen::Rotation2D<float>(0.1));
        Eigen::Rotation2D<float> rot = Eigen::Rotation2D<float>(0.1);

        leafs.append(new KisLindenmayerLetter(m_position, rot * m_growDirection));
        m_age = 0;
    }

    m_position += m_growDirection*time;
}
