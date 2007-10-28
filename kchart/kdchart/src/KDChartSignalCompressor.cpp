/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2005-2007 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Chart library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KD Chart licenses may use this file in
 ** accordance with the KD Chart Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.kdab.net/kdchart for
 **   information about KD Chart Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/

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




