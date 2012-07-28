#ifndef MOCKSHAPEANIMATION_H
#define MOCKSHAPEANIMATION_H

#include <animations/KPrShapeAnimation.h>

class MockShapeAnimation : public KPrShapeAnimation
{
public:
    MockShapeAnimation(KoShape *shape, KoTextBlockData *textBlockData)
        : KPrShapeAnimation(shape, textBlockData)
        , m_beginTime(0)
        , m_duration(1)
    {}

    /**
      * Return the begin and end time of the animation as a QPair
      */
    QPair<int, int> timeRange() {return QPair<int, int>(m_beginTime, m_beginTime + m_duration);}
    /**
      * Return global duration of the shape animation
      */
    int globalDuration() {return m_duration;}

    /**
     * @brief Set the begin time for the animation
     *
     * @param timeMS time in miliseconds
     */
    void setBeginTime(int timeMS) {m_beginTime = timeMS;}

    /**
     * @brief Set duration time for the animation
     *
     * @param timeMS time in miliseconds
     */
    void setGlobalDuration(int timeMS) {m_duration = timeMS;}

private:
    int m_beginTime;
    int m_duration;

};

#endif // MOCKSHAPEANIMATION_H
