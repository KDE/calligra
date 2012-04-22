#ifndef KPRANIMTRANSITIONFILTEREFFECT_H
#define KPRANIMTRANSITIONFILTEREFFECT_H

#include <QPixmap>
#include <QTimeLine>

#include <KoXmlReaderForward.h>
#include "stage_export.h"

class QPainter;
class QGraphicsScene;
class QGraphicsPixmapItem;
class QGraphicsView;
class KoXmlWriter;
class KoGenStyle;
class KPrAnimTransitionFilterStrategy;

class STAGE_EXPORT KPrAnimTransitionFilterEffect
{
public:
    KPrAnimTransitionFilterEffect( int duration, const QString & id, KPrAnimTransitionFilterStrategy * strategy);

    struct Data
    {
        Data( const QPixmap &oldPage, const QPixmap &newPage, QWidget *w )
        : m_oldPage( oldPage )
        , m_newPage( newPage )
        , m_widget( w )
        , m_scene( 0 )
        , m_graphicsView( 0 )
        , m_finished( false )
        , m_currentTime( 0 )
        , m_lastTime( 0 )
        {}

        QPixmap m_oldPage;
        QPixmap m_newPage;
        QWidget * m_widget;
        QTimeLine m_timeLine;
        QGraphicsScene *m_scene;
        QGraphicsView *m_graphicsView;
        QGraphicsPixmapItem *m_oldPageItem;
        QGraphicsPixmapItem *m_newPageItem;
        bool m_finished;
        int m_currentTime;
        int m_lastTime;
    };
protected:
    int m_duration;
    QString m_id;
    KPrAnimTransitionFilterStrategy * m_strategy;
};

#endif // KPRANIMTRANSITIONFILTEREFFECT_H
