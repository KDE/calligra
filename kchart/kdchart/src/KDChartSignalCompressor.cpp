#include "KDChartSignalCompressor.h"

using namespace KDChart;

SignalCompressor::SignalCompressor( QObject* receiver, const char* signal,
                                    QObject* parent )
    : QObject( parent )
{
    connect( this, SIGNAL( finallyEmit() ), receiver, signal );
    connect( &m_timer, SIGNAL( timeout() ), SLOT( nowGoAlready() ) );
    m_timer.setSingleShot( true );
    // m_timer.setIntervall( 0 ); // default, just to know...
}

void SignalCompressor::emitSignal()
{
    if ( !m_timer.isActive() ) m_timer.start();
}

void SignalCompressor::nowGoAlready()
{
    emit finallyEmit();
}




