/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2002 Harri Porten <porten@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kppageeffects.h"
#include <kapplication.h>
#include <krandomsequence.h>
#include <qpixmap.h>
#include <qdatetime.h>
#include <qwidget.h>
#include <qwmatrix.h>

/*=========================== change pages =======================*/
void kPchangePages( QWidget *canv, const QPixmap &_pix1, const QPixmap &_pix2,
                    PageEffect effect, int speedFaktor )
{
    Q_ASSERT( _pix1.size() == _pix2.size() );
    int width = _pix1.width();
    int height = _pix1.height();
    QTime _time;
    int _step = 0, _steps = 0, _h = 0, _w = 0, _x = 0, _y = 0;

    switch ( effect )
    {
    case PEF_NONE:
        bitBlt( canv, 0, 0, &_pix2, 0, 0, width, height );
        break;
    case PEF_CLOSE_HORZ:
    {
        _steps = static_cast<int>( static_cast<float>( height ) / speedFaktor );
        _time.start();

        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _h = ( height/( 2 * _steps ) ) * _step;
                _h = _h > height / 2 ? height / 2 : _h;

                bitBlt( canv, 0, 0, &_pix2, 0, height / 2 - _h, width, _h );
                bitBlt( canv, 0, height - _h, &_pix2, 0, height / 2, width, _h );

                _time.restart();
            }
            if ( ( height/( 2 * _steps ) ) * _step >= height / 2 ) break;
        }
    } break;
    case PEF_CLOSE_VERT:
    {
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );
        _time.start();

        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _w = ( width/( 2 * _steps ) ) * _step;
                _w = _w > width / 2 ? width / 2 : _w;

                bitBlt( canv, 0, 0, &_pix2, width / 2 - _w, 0, _w, height );
                bitBlt( canv, width - _w, 0, &_pix2, width / 2, 0, _w, height );

                _time.restart();
            }
            if ( ( width/( 2 * _steps ) ) * _step >= width / 2 ) break;
        }
    } break;
    case PEF_CLOSE_ALL:
    {
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );
        _time.start();

        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _w = ( width/( 2 * _steps ) ) * _step;
                _w = _w > width / 2 ? width / 2 : _w;

                _h = ( height/( 2 * _steps ) ) * _step;
                _h = _h > height / 2 ? height / 2 : _h;

                bitBlt( canv, 0, 0, &_pix2, 0, 0, _w, _h );
                bitBlt( canv, width - _w, 0, &_pix2, width - _w, 0, _w, _h );
                bitBlt( canv, 0, height - _h, &_pix2, 0, height - _h, _w, _h );
                bitBlt( canv, width - _w, height - _h, &_pix2, width - _w, height - _h, _w, _h );

                _time.restart();
            }
            if ( ( width/( 2 * _steps ) ) * _step >= width / 2
                 && ( height/( 2 * _steps ) ) * _step >= height / 2 ) break;
        }
    } break;
    case PEF_OPEN_HORZ:
    {
        _steps = static_cast<int>( static_cast<float>( height ) / speedFaktor );
        _time.start();

        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _h = ( height / _steps ) * _step;
                _h = _h > height ? height : _h;

                _y = height / 2;

                bitBlt( canv, 0, _y - _h / 2, &_pix2, 0, _y - _h / 2, width, _h );

                _time.restart();
            }
            if ( ( height / _steps ) * _step >= height ) break;
        }
    } break;
    case PEF_OPEN_VERT:
    {
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );
        _time.start();

        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _w = ( width / _steps ) * _step;
                _w = _w > width ? width : _w;

                _x = width / 2;

                bitBlt( canv, _x - _w / 2, 0, &_pix2, _x - _w / 2, 0, _w, height );

                _time.restart();
            }
            if ( ( width / _steps ) * _step >= width ) break;
        }
    } break;
    case PEF_OPEN_ALL:
    {
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );
        _time.start();

        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _w = ( width / _steps ) * _step;
                _w = _w > width ? width : _w;

                _x = width / 2;

                _h = ( height / _steps ) * _step;
                _h = _h > height ? height : _h;

                _y = height / 2;

                bitBlt( canv, _x - _w / 2, _y - _h / 2, &_pix2, _x - _w / 2, _y - _h / 2, _w, _h );

                _time.restart();
            }
            if ( ( width / _steps ) * _step >= width &&
                 ( height / _steps ) * _step >= height ) break;
        }
    } break;
    case PEF_INTERLOCKING_HORZ_1:
    {
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );
        _time.start();

        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _w = ( width / _steps ) * _step;
                _w = _w > width ? width : _w;

                bitBlt( canv, 0, 0, &_pix2, 0, 0, _w, height / 4 );
                bitBlt( canv, 0, height / 2, &_pix2, 0, height / 2, _w, height / 4 );
                bitBlt( canv, width - _w, height / 4, &_pix2, width - _w, height / 4, _w, height / 4 );
                bitBlt( canv, width - _w, height / 2 + height / 4, &_pix2, width - _w,
                        height / 2 + height / 4, _w, height / 4 );

                _time.restart();
            }
            if ( ( width / _steps ) * _step >= width ) break;
        }
    } break;
    case PEF_INTERLOCKING_HORZ_2:
    {
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );
        _time.start();

        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _w = ( width / _steps ) * _step;
                _w = _w > width ? width : _w;

                bitBlt( canv, 0, height / 4, &_pix2, 0, height / 4, _w, height / 4 );
                bitBlt( canv, 0, height / 2 + height / 4, &_pix2, 0, height / 2 + height / 4, _w, height / 4 );
                bitBlt( canv, width - _w, 0, &_pix2, width - _w, 0, _w, height / 4 );
                bitBlt( canv, width - _w, height / 2, &_pix2, width - _w, height / 2, _w, height / 4 );

                _time.restart();
            }
            if ( ( width / _steps ) * _step >= width ) break;
        }
    } break;
    case PEF_INTERLOCKING_VERT_1:
    {
        _steps = static_cast<int>( static_cast<float>( height ) / speedFaktor );
        _time.start();

        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _h = ( height / _steps ) * _step;
                _h = _h > height ? height : _h;

                bitBlt( canv, 0, 0, &_pix2, 0, 0, width / 4, _h );
                bitBlt( canv, width / 2, 0, &_pix2, width / 2, 0, width / 4, _h );
                bitBlt( canv, width / 4, height - _h, &_pix2, width / 4, height - _h, width / 4, _h );
                bitBlt( canv, width / 2 + width / 4, height - _h, &_pix2, width / 2 + width / 4, height - _h,
                        width / 4, _h );

                _time.restart();
            }
            if ( ( height / _steps ) * _step >= height ) break;
        }
    } break;
    case PEF_INTERLOCKING_VERT_2:
    {
        _steps = static_cast<int>( static_cast<float>( height ) / speedFaktor );
        _time.start();

        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _h = ( height / _steps ) * _step;
                _h = _h > height ? height : _h;

                bitBlt( canv, width / 4, 0, &_pix2, width / 4, 0, width / 4, _h );
                bitBlt( canv, width / 2 + width / 4, 0, &_pix2, width / 2 + width / 4, 0, width / 4, _h );
                bitBlt( canv, 0, height - _h, &_pix2, 0, height - _h, width / 4, _h );
                bitBlt( canv, width / 2, height - _h, &_pix2, width / 2, height - _h, width / 4, _h );

                _time.restart();
            }
            if ( ( height / _steps ) * _step >= height ) break;
        }
    } break;
    case PEF_SURROUND1:
    {
        int wid = width / 10;
        int hei = height / 10;

        int curr = 1;
        int curr2 = 1;

        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );
        _time.start();

        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;

                if ( curr == 1 || curr == 5 || curr == 9 || curr == 13 || curr == 17 )
                {
                    int dx = ( curr2 / 4 ) * wid;
                    int dy = ( curr2 / 4 ) * hei;
                    _h = ( height / _steps ) * _step;
                    if ( _h >= height - 2 * dy )
                    {
                        _h = height - 2 * dy;
                        curr++;
                        _step = 0;
                    }
                    bitBlt( canv, dx, dy, &_pix2, dx, dy, wid, _h );
                }
                else if ( curr == 2 || curr == 6 || curr == 10 || curr == 14 || curr == 18 )
                {
                    int dx = ( curr2 / 4 ) * wid;
                    int dy = ( curr2 / 4 ) * hei;
                    _w = ( width / _steps ) * _step;
                    if ( _w >= width - wid - 2 * dx )
                    {
                        _w = width - wid - 2 * dx;
                        curr++;
                        _step = 0;
                    }
                    bitBlt( canv, dx + wid, height - hei - dy, &_pix2, dx + wid, height - hei - dy, _w, hei );
                }
                else if ( curr == 3 || curr == 7 || curr == 11 || curr == 15 || curr == 19 )
                {
                    int dx = ( curr2 / 4 ) * wid;
                    int dy = ( curr2 / 4 ) * hei;
                    _h = ( height / _steps ) * _step;
                    if ( _h >= height - hei - 2 * dy )
                    {
                        _h = height - hei - 2 * dy;
                        curr++;
                        _step = 0;
                    }
                    bitBlt( canv, width - wid - dx, height - hei - dy - _h, &_pix2,
                            width - wid - dx, height - hei - dy - _h, wid, _h );
                }
                else if ( curr == 4 || curr == 8 || curr == 12 || curr == 16 || curr == 20 )
                {
                    int dx = ( curr2 / 4 ) * wid;
                    int dy = ( curr2 / 4 ) * hei;
                    _w = ( width / _steps ) * _step;
                    if ( _w >= width - 2 * wid - 2 * dx )
                    {
                        _w = width - 2 * wid - 2 * dx;
                        _steps *= 2;
                        _steps = static_cast<int>( static_cast<float>( _steps ) / 1.5 );
                        curr++;
                        curr2 += 4;
                        _step = 0;
                    }
                    bitBlt( canv, width - dx - wid - _w, dy, &_pix2, width - dx - wid - _w,
                            dy, _w, hei );
                }
                _time.restart();
            }
            if ( curr == 21 )
            {
                bitBlt( canv, 0, 0, &_pix2, 0, 0, width, height );
                break;
            }
        }
    } break;
    case PEF_FLY1:
    {
        _steps = static_cast<int>( static_cast<float>( height ) / speedFaktor );
        _time.start();

        int _psteps = _steps / 5;
        QRect oldRect( 0, 0, width, height );
        QSize ps;
        QPixmap pix3;

        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                if ( _step < _psteps )
                {
                    pix3 = QPixmap( _pix1 );
                    QPixmap pix4( _pix2 );
                    float dw = static_cast<float>( _step * ( ( pix3.width() - ( pix3.width() / 10 ) ) /
                                                             ( 2 * _psteps ) ) );
                    float dh = static_cast<float>( _step * ( ( pix3.height() - ( pix3.height() / 10 ) ) /
                                                             ( 2 * _psteps ) ) );

                    dw *= 2;
                    dh *= 2;

                    QWMatrix m;
                    m.scale( static_cast<float>( pix3.width() - dw ) / static_cast<float>( pix3.width() ),
                             static_cast<float>( pix3.height() - dh ) / static_cast<float>( pix3.height() ) );
                    pix3 = pix3.xForm( m );
                    ps = pix3.size();

                    bitBlt( &pix4, ( pix4.width() - pix3.width() ) / 2, ( pix4.height() - pix3.height() ) / 2,
                            &pix3, 0, 0, pix3.width(), pix3.height() );
                    QRect newRect( ( pix4.width() - pix3.width() ) / 2, ( pix4.height() - pix3.height() ) / 2,
                                   pix3.width(), pix3.height() );
                    QRect r = newRect.unite( oldRect );
                    bitBlt( canv, r.x(), r.y(), &pix4, r.x(), r.y(), r.width(), r.height() );
                    oldRect = newRect;
                }
                if ( _step > _psteps && _step < _psteps * 2 )
                {
                    QPixmap pix4( _pix2 );
                    int yy = ( height - pix3.height() ) / 2 - ( ( ( height - pix3.height() ) / 2 ) /
                                                                        _psteps ) * ( _step - _psteps );

                    bitBlt( &pix4, ( pix4.width() - pix3.width() ) / 2, yy,
                            &pix3, 0, 0, pix3.width(), pix3.height() );
                    QRect newRect( ( pix4.width() - pix3.width() ) / 2, yy,
                                   pix3.width(), pix3.height() );
                    QRect r = newRect.unite( oldRect );
                    bitBlt( canv, r.x(), r.y(), &pix4, r.x(), r.y(), r.width(), r.height() );
                    oldRect = newRect;
                }
                if ( _step > 2 * _psteps && _step < _psteps * 3 )
                {
                    QPixmap pix4( _pix2 );
                    int xx = ( width - pix3.width() ) / 2 - ( ( ( width - pix3.width() ) / 2 ) /
                                                                      _psteps ) * ( _step - 2 * _psteps );
                    int yy = ( ( ( height - pix3.height() ) / 2 ) / _psteps ) * ( _step - 2 * _psteps );

                    bitBlt( &pix4, xx, yy, &pix3, 0, 0, pix3.width(), pix3.height() );
                    QRect newRect( xx, yy, pix3.width(), pix3.height() );
                    QRect r = newRect.unite( oldRect );
                    bitBlt( canv, r.x(), r.y(), &pix4, r.x(), r.y(), r.width(), r.height() );
                    oldRect = newRect;
                }
                if ( _step > 3 * _psteps && _step < _psteps * 5 )
                {
                    QPixmap pix4( _pix2 );
                    int xx = ( ( width - pix3.width() ) / _psteps ) * ( _step - 3 * _psteps );
                    int yy = ( ( height - pix3.height() ) / 2 ) +
                             ( ( ( height - pix3.height() ) / 2 ) / _psteps ) * ( _step - 3 * _psteps );

                    bitBlt( &pix4, xx, yy, &pix3, 0, 0, pix3.width(), pix3.height() );
                    QRect newRect( xx, yy, pix3.width(), pix3.height() );
                    QRect r = newRect.unite( oldRect );
                    bitBlt( canv, r.x(), r.y(), &pix4, r.x(), r.y(), r.width(), r.height() );
                    oldRect = newRect;
                }
                _time.restart();
            }
            if ( _step >= _steps )
            {
                bitBlt( canv, 0, 0, &_pix2, 0, 0, width, height );
                break;
            }
        }
    } break;

    // Blinds Horizontal
    case PEF_BLINDS_HOR:
    {
        int blockSize = height / 8;
        _steps = static_cast<int>( static_cast<float>( height ) / speedFaktor );
        if( _steps < 1 ) _steps = 1;

        _time.start();

        _h = 0;
        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _h = _step * blockSize / _steps;

                for ( int i=0; i < height; i += blockSize )
                  bitBlt( canv, 0, i, &_pix2, 0, i, width, _h );

                _time.restart();
            }
            if( _h >= blockSize ) break;
        }
    } break;

    // Blinds Vertical
    case PEF_BLINDS_VER:
    {
        int blockSize = width / 8;
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );        if( _steps < 1 ) _steps = 1;

        _time.start();

        _w = 0;
        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _w = _step * blockSize / _steps;

                for( int i=0; i < width; i += blockSize )
                  bitBlt( canv, i, 0, &_pix2, i, 0, _w, height );

                _time.restart();
            }
            if( _w >= blockSize ) break;
        }
    } break;

    // Box In
    case PEF_BOX_IN:
    {
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );
        _time.start();

        _w = _h = 0;
        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _w = ( width/( 2 * _steps ) ) * _step;
                _w = _w > width / 2 ? width / 2 : _w;
                _h = _w * height / width;
                _h = _h > height / 2 ? height / 2 : _h;

                bitBlt( canv, 0, 0, &_pix2, 0, 0, _w, height );
                bitBlt( canv, width-_w, 0, &_pix2, width-_w, 0, _w, height );
                bitBlt( canv, 0, 0, &_pix2, 0, 0, width, _h );
                bitBlt( canv, 0, height-_h, &_pix2, 0, height-_h, width, _h );
                _time.restart();
            }
            if( _w >= width/2 ) break;
        }
    } break;

    // Box Out
    case PEF_BOX_OUT:
    {
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );
        _time.start();

        _w = _h = 0;
        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _w = ( width/( 2 * _steps ) ) * _step;
                _w = _w > width / 2 ? width / 2 : _w;
                _h = _w * height / width;
                _h = _h > height / 2 ? height / 2 : _h;

                bitBlt( canv, width/2-_w, height/2-_h, &_pix2,
                   width/2-_w, height/2-_h, _w*2, _h*2 );
                _time.restart();
            }
            if( _w >= width/2 ) break;
        }
    } break;

    // Checkboard Across
    case PEF_CHECKBOARD_ACROSS:
    {
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );
        _time.start();

        int blocksize = height / 8;

        _steps /= 2;
        int w0 = 0;
        while ( w0 < 2 * blocksize )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _w = blocksize * _step / _steps;
                _w = kMin( _w, (blocksize * 2) );

                for( int yp = 0; yp < height; yp += blocksize )
                {
                  int xp = ((yp/blocksize) & 1) ? blocksize : 0;
                  if( ( xp > 0 ) && ( _w > blocksize ) )
                    bitBlt( canv, w0-blocksize, yp, &_pix2, w0-blocksize,
                            yp, _w-w0, blocksize );
                  for( ; xp < width; xp += 2*blocksize )
                    bitBlt( canv, xp + w0, yp,
                            &_pix2, xp + w0, yp, _w - w0, blocksize );
                }

                _time.restart();
                w0 = _w;
            }
        }
    } break;

    // Checkboard Down
    case PEF_CHECKBOARD_DOWN:
    {
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );
        _time.start();

        int blocksize = height / 8;

        _steps /= 2;
        int h0 = 0;
        while ( h0 < 2 * blocksize )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _h = blocksize * _step / _steps;
                _h = kMin ( _h, (blocksize * 2) );

                for( int xp = 0; xp < width; xp += blocksize )
                {
                  int yp = ((xp/blocksize) & 1) ? blocksize : 0;
                  if( ( yp > 0 ) && ( _h > blocksize ) )
                      bitBlt( canv, xp, h0-blocksize, &_pix2, xp, h0-blocksize,
                              blocksize, _h-h0 );
                  for( ; yp < height; yp += 2*blocksize )
                      bitBlt( canv, xp, yp + h0,
                              &_pix2, xp, yp + h0, blocksize, _h - h0 );
                }

                h0 = _h;
                _time.restart();
            }
        }
    } break;

    // Cover Down
    case PEF_COVER_DOWN:
    {
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );
        _time.start();

        int dheight = height;
        int dwidth = width;

        _h = 0;
        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _h = ( dheight/ _steps ) * _step;
                _h = _h > dheight ? dheight : _h;

                bitBlt( canv, 0, 0, &_pix2, 0, dheight-_h, dwidth, _h );

                _time.restart();
            }
            if( _h >= dheight ) break;
        }
    } break;

    // Uncover Down
    case PEF_UNCOVER_DOWN:
    {
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );
        _time.start();

        int dheight = height;

        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _h = ( dheight/ _steps ) * _step;
                _h = _h > dheight ? dheight : _h;

                bitBlt( canv, 0, 0, &_pix2, 0, 0, width, _h );
                bitBlt( canv, 0, _h, &_pix1, 0, 0, width, height-_h );

                _time.restart();
            }
            if( _h >= dheight ) break;
        }

    } break;

    // Cover Up
    case PEF_COVER_UP:
    {
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );
        _time.start();

        int dheight = height;
        int dwidth = width;

        _h = 0;
        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _h = ( dheight/ _steps ) * _step;
                _h = _h > dheight ? dheight : _h;

                bitBlt( canv, 0, dheight-_h, &_pix2, 0, 0, dwidth, _h );

                _time.restart();
            }
            if( _h >= dheight ) break;
        }
    } break;

    // Uncover Up
    case PEF_UNCOVER_UP:
    {
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );
        _time.start();

        int dheight = height;

        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _h = ( dheight/ _steps ) * _step;
                _h = _h > dheight ? dheight : _h;

                bitBlt( canv, 0, dheight-_h, &_pix2, 0, dheight-_h, width, _h );
                bitBlt( canv, 0, 0, &_pix1, 0, _h, width, height-_h );

                _time.restart();
            }
            if( _h >= dheight ) break;
        }

    } break;

    // Cover Left
    case PEF_COVER_LEFT:
    {
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );
        _time.start();

        int dheight = height;
        int dwidth = width;

        _w = 0;
        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _w = ( dwidth / _steps ) * _step;
                _w = _w > dwidth ? dwidth : _w;

                bitBlt( canv, dwidth-_w, 0, &_pix2, 0, 0, _w, dheight );

                _time.restart();
            }
            if( _w >= dwidth ) break;
        }
    } break;

    // Uncover Left
    case PEF_UNCOVER_LEFT:
    {
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );
        _time.start();

        int dheight = height;
        int dwidth = width;

        _w = 0;
        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _w = ( dwidth / _steps ) * _step;
                _w = _w > dwidth ? dwidth : _w;

                bitBlt( canv, 0, 0, &_pix1, _w, 0, dwidth-_w, dheight );
                bitBlt( canv, dwidth-_w, 0, &_pix2, dwidth-_w, 0, _w, dheight );

                _time.restart();
            }
            if( _w >= dwidth ) break;
        }
    } break;

    // Cover Right
    case PEF_COVER_RIGHT:
    {
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );
        _time.start();

        int dheight = height;
        int dwidth = width;;

        _w = 0;
        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _w = ( dwidth / _steps ) * _step;
                _w = _w > dwidth ? dwidth : _w;

                bitBlt( canv, 0, 0, &_pix2, dwidth-_w, 0, _w, dheight );

                _time.restart();
            }
            if( _w >= dwidth ) break;
        }
    } break;

    // Uncover Right
    case PEF_UNCOVER_RIGHT:
    {
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );
        _time.start();

        int dheight = height;
        int dwidth = width;;

        _w = 0;
        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _w = ( dwidth / _steps ) * _step;
                _w = _w > dwidth ? dwidth : _w;

                bitBlt( canv, _w, 0, &_pix1, 0, 0, dwidth-_w, dheight );
                bitBlt( canv, 0, 0, &_pix2, 0, 0, _w, dheight );

                _time.restart();
            }
            if( _w >= dwidth ) break;
        }
    } break;

    // Cover Left-Up
    case PEF_COVER_LEFT_UP:
    {
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );
        _time.start();

        int dheight = height;
        int dwidth = width;;

        _w = _h = 0;
        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _w = ( dwidth / _steps ) * _step;
                _w = _w > dwidth ? dwidth : _w;
                _h = ( dheight / _steps ) * _step;
                _h = _h > dheight ? dheight : _h;

                bitBlt( canv, dwidth-_w, dheight-_h, &_pix2, 0, 0, _w, _h );

                _time.restart();
            }
            if( _w >= dwidth ) break;
        }
    } break;

    // Uncover Left-Up
    case PEF_UNCOVER_LEFT_UP:
    {
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );
        _time.start();

        int dheight = height;
        int dwidth = width;;

        _w = _h = 0;
        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _w = ( dwidth / _steps ) * _step;
                _w = _w > dwidth ? dwidth : _w;
                _h = ( dheight / _steps ) * _step;
                _h = _h > dheight ? dheight : _h;

                bitBlt( canv, 0, 0, &_pix1, _w, _h, dwidth-_w, dheight-_h );
                bitBlt( canv, dwidth-_w, 0, &_pix2, dwidth-_w, 0, _w, dheight );
                bitBlt( canv, 0, dheight-_h, &_pix2, 0, dheight-_h, dwidth, _h );

                _time.restart();
            }
            if( _w >= dwidth ) break;
        }
    } break;

    // Cover Left-Down
    case PEF_COVER_LEFT_DOWN:
    {
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );
        _time.start();

        int dheight = height;
        int dwidth = width;;

        _w = _h = 0;
        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _w = ( dwidth / _steps ) * _step;
                _w = _w > dwidth ? dwidth : _w;
                _h = ( dheight / _steps ) * _step;
                _h = _h > dheight ? dheight : _h;

                bitBlt( canv, dwidth-_w, 0, &_pix2, 0, dheight-_h, _w, _h );

                _time.restart();
            }
            if( _w >= dwidth ) break;
        }
    } break;

    // Uncover Left-Down
    case PEF_UNCOVER_LEFT_DOWN:
    {
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );
        _time.start();

        int dheight = height;
        int dwidth = width;;

        _w = _h = 0;
        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _w = ( dwidth / _steps ) * _step;
                _w = _w > dwidth ? dwidth : _w;
                _h = ( dheight / _steps ) * _step;
                _h = _h > dheight ? dheight : _h;

                bitBlt( canv, 0, _h, &_pix1, _w, 0, dwidth-_w, dheight-_h );
                bitBlt( canv, dwidth-_w, 0, &_pix2, dwidth-_w, 0, _w, dheight );
                bitBlt( canv, 0, 0, &_pix2, 0, 0, dwidth, _h );

                _time.restart();
            }
            if( _w >= dwidth ) break;
        }
    } break;

    // Cover Right-Up
    case PEF_COVER_RIGHT_UP:
    {
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );
        _time.start();

        int dheight = height;
        int dwidth = width;;

        _w = _h = 0;
        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _w = ( dwidth / _steps ) * _step;
                _w = _w > dwidth ? dwidth : _w;
                _h = ( dheight / _steps ) * _step;
                _h = _h > dheight ? dheight : _h;

                bitBlt( canv, 0, dheight-_h, &_pix2, dwidth-_w, 0, _w, _h );

                _time.restart();
            }
            if( _w >= dwidth ) break;
        }
    } break;

    // Uncover Right-Up
    case PEF_UNCOVER_RIGHT_UP:
    {
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );
        _time.start();

        int dheight = height;
        int dwidth = width;;

        _w = _h = 0;
        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _w = ( dwidth / _steps ) * _step;
                _w = _w > dwidth ? dwidth : _w;
                _h = ( dheight / _steps ) * _step;
                _h = _h > dheight ? dheight : _h;

                bitBlt( canv, _w, 0, &_pix1, 0, _h, dwidth-_w, dheight-_h );
                bitBlt( canv, 0, 0, &_pix2, 0, 0, _w, dheight );
                bitBlt( canv, 0, dheight-_h, &_pix2, 0, dheight-_h, dwidth, _h );

                _time.restart();
            }
            if( _w >= dwidth ) break;
        }
    } break;

    // Cover Right-Down
    case PEF_COVER_RIGHT_DOWN:
    {
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );
        _time.start();

        int dheight = height;
        int dwidth = width;;

        _w = _h = 0;
        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _w = ( dwidth / _steps ) * _step;
                _w = _w > dwidth ? dwidth : _w;
                _h = ( dheight / _steps ) * _step;
                _h = _h > dheight ? dheight : _h;

                bitBlt( canv, 0, 0, &_pix2, dwidth-_w, dheight-_h, _w, _h );

                _time.restart();
            }
            if( _w >= dwidth ) break;
        }
    } break;

    // Uncover Right-Down
    case PEF_UNCOVER_RIGHT_DOWN:
    {
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );
        _time.start();

        int dheight = height;
        int dwidth = width;;

        _w = _h = 0;
        for ( ; ; )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                _w = ( dwidth / _steps ) * _step;
                _w = _w > dwidth ? dwidth : _w;
                _h = ( dheight / _steps ) * _step;
                _h = _h > dheight ? dheight : _h;

                bitBlt( canv, _w, _h, &_pix1, 0, 0, dwidth-_w, dheight-_h );
                bitBlt( canv, 0, 0, &_pix2, 0, 0, _w, dheight );
                bitBlt( canv, 0, 0, &_pix2, 0, 0, dwidth, _h );

                _time.restart();
            }
            if( _w >= dwidth ) break;
        }
    } break;

    // Dissolve
    case PEF_DISSOLVE:
    {
        KRandomSequence random;

        unsigned blockSize = height / 32; // small enough
        unsigned rowno = ( height + blockSize - 1 ) / blockSize;
        unsigned colno = ( width + blockSize - 1 ) / blockSize;
        unsigned cellno = rowno * colno;

        QValueList<int> cells;
        for( unsigned c = 0; c < cellno; c++ )
            cells.append( c );

        _steps = static_cast<int>( static_cast<float>( height ) / speedFaktor );

        _steps *= 10;

        while( !cells.isEmpty() )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                _step++;
                unsigned k = cellno - cellno * _step / _steps;

                while( cells.count() > k )
                {
                  unsigned index = random.getLong( cells.count() );
                  QValueListIterator<int> it = cells.at( index );
                  unsigned y = *it / colno;
                  unsigned x = *it % colno;
                  cells.remove( it );

                  bitBlt( canv, x * blockSize, y * blockSize, &_pix2,
                    x * blockSize, y * blockSize, blockSize, blockSize );
               }

              _time.restart();
            }
        }

    } break;

    // Strips Left-Up
    case PEF_STRIPS_LEFT_UP:
    {
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );

        unsigned blocks = _steps / 2;
        unsigned blockSize = width / blocks;

        _time.start();

        for ( ; _step < _steps;  )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                for ( int x = 0; x <= _step; ++x )
                {
                    int y = _step - x;

                    bitBlt( canv, width - x * blockSize, height - y * blockSize, &_pix2,
                        width - x * blockSize, height - y * blockSize,
                        blockSize, blockSize );
                }

                _step++;
                _time.restart();
            }
        }

    } break;

    // Strips Left-Down
    case PEF_STRIPS_LEFT_DOWN:
    {
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );

        unsigned blocks = _steps / 2;
        unsigned blockSize = width / blocks;

        _time.start();

        for ( ; _step < _steps;  )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                for ( int x = 0; x <= _step; ++x )
                {
                    int y = _step - x;

                    bitBlt( canv, width - x * blockSize, y * blockSize, &_pix2,
                        width - x * blockSize, y * blockSize, blockSize, blockSize );
                }

                _step++;
                _time.restart();
            }
        }

    } break;

    // Strips Right-Up
    case PEF_STRIPS_RIGHT_UP:
    {
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );

        unsigned blocks = _steps / 2;
        unsigned blockSize = width / blocks;

        _time.start();

        for ( ; _step < _steps;  )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                for ( int x = 0; x <= _step; ++x )
                {
                    int y = _step - x;
                    bitBlt( canv, x * blockSize, height - y * blockSize,
                            &_pix2, x * blockSize, height - y * blockSize,
                            blockSize, blockSize );
                }

                _step++;
                _time.restart();
            }
        }

    } break;

    // Strips Right-Down
    case PEF_STRIPS_RIGHT_DOWN:
    {
        _steps = static_cast<int>( static_cast<float>( width ) / speedFaktor );

        unsigned blocks = _steps / 2;
        unsigned blockSize = width / blocks;

        _time.start();

        for ( ; _step < _steps;  )
        {
            kapp->processEvents();
            if ( _time.elapsed() >= 1 )
            {
                for ( int x = 0; x <= _step; ++x )
                {
                    int y = _step - x;

                    bitBlt( canv, x * blockSize, y * blockSize, &_pix2,
                        x * blockSize, y * blockSize, blockSize, blockSize );
                }

                _step++;
                _time.restart();
            }
        }

    } break;

    // Random (just pick up one of the above effect)
    case PEF_RANDOM:
    case PEF_LAST_MARKER:
    {
        KRandomSequence random;

        kPchangePages( canv, _pix1, _pix2, static_cast<PageEffect>( random.getLong( PEF_LAST_MARKER ) ), speedFaktor );

    } break;

    }
}
