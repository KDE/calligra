#include "KPrPageEffectRunner.h"

#include "KPrPageEffect.h"

KPrPageEffectRunner::KPrPageEffectRunner( const QPixmap &oldPage, const QPixmap &newPage, QWidget *w, KPrPageEffect *effect )
: m_effect( effect )
, m_data( oldPage, newPage, w )
{
    m_effect->setup( m_data, m_data.m_timeLine );
}

KPrPageEffectRunner::~KPrPageEffectRunner()
{
}

bool KPrPageEffectRunner::paint( QPainter &painter )
{
    return m_effect->paint( painter, m_data );
}

void KPrPageEffectRunner::next( int currentTime )
{
    m_data.m_lastTime = m_data.m_currentTime;
    m_data.m_currentTime = currentTime;
    m_effect->next( m_data );
}

void KPrPageEffectRunner::finish()
{
    m_data.m_finished = true;
    m_effect->finish( m_data );
}

bool KPrPageEffectRunner::isFinished()
{
    return m_data.m_finished;
}
