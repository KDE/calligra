/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1998                   */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* KPresenter is under GNU GPL                                    */
/******************************************************************/
/* Module: KPresenter Utilities                                   */
/******************************************************************/
#include "kpresenter_utils.h"

#include <stdio.h>
#include <stdlib.h>

/*========================== convert string to pixmap ============*/
QPixmap string_to_pixmap(const char *_pixmap)
{
    if ( _pixmap == 0L || _pixmap[0] == 0 )
	return QPixmap();
    
    // Count lines in XPM
    char pixmap[ strlen( _pixmap ) + 1 ];
    strcpy( pixmap, _pixmap );
    int lines = 1;
    char* pos = &pixmap[0];
    char* start = 0L;
    char* pline = pixmap;
    char first = *pixmap;
    
    while (*pos)
      {
	if (*pos == 1)
	  *pos = '\"';

	pos++;
      }

    pos = pixmap;
    while ( *pos )
      {	
	if ( *pos++ == '\n' )
	  {
	    if ( strncmp( pos - 3, "\",\n", 3 ) == 0 )
	      { *(pos-1) = 0; *(pos-2) = 0; *(pos-3) = 0; }
	    else if ( strncmp( pos - 4, "\"};\n", 4 ) == 0 )
	      { *(pos-1) = 0; *(pos-2) = 0; *(pos-3) = 0; *(pos-4) = 0; }	    
	    else if ( strncmp( pos - 3, "\"};", 3 ) == 0 )
	      { *(pos-1) = 0; *(pos-2) = 0; *(pos-3) = 0; }	    
	    else
	      { *(pos-1) = 0; }	    
	    if ( first == '\"' && start == 0L )
	      start = pline;
	    if ( first == '\"' )
	      lines++;
	    pline = pos;
	    first = *pline;
	  }
      }

    if ( start == 0L )
      return QPixmap();

    const char* list[ lines ];
    int i;
    const char* p = start;
    for ( i = 0; i < lines; i++ )
    {
      list[i] = p + 1;
      p += strlen( p ) + 1;
      while ( *p == 0 ) p++;
    }
    list[i] = 0L;

    return QPixmap( list );
}

/*========================== load a pixmap =======================*/
QString load_pixmap(const char *_file)
{
    FILE *f = fopen( _file, "r" );
    if ( f == 0L )
    {
	warning(" Could not open pixmap file '%s\n", _file );
	return QString();
    }
    
    char buffer[ 2048 ];
    
    QString str( "" );
    while( !feof( f ) )
    {
	int i = fread( buffer, 1, 2047, f );
	if ( i > 0 )
	{
	    buffer[i] = 0;
	    str += buffer;
	}
    }
    
    fclose( f );
    
    return str;
}

/*= load pixmap saved in KPresenter's native format - make valid =*/
QString load_pixmap_native_format(const char *_file)
{
    FILE *f = fopen( _file, "r" );
    if ( f == 0L )
    {
	warning(" Could not open pixmap file '%s\n", _file );
	return QString();
    }
    
    char buffer[ 2048 ];
    
    QString str( "" );
    while( !feof( f ) )
    {
	int i = fread( buffer, 1, 2047, f );
	if ( i > 0 )
	{
	    buffer[i] = 0;
	    char* c = &buffer[0];
	    while(*c)
	      {
		if (*c == '\"')
		  *c = 1;

		c++;
	      }

	    str += buffer;
	}
    }
    
    fclose( f );
    
    return str;
}

/*========================== draw a figure =======================*/
void drawFigure(LineEnd figure,QPainter* painter,QPoint coord,QColor color,int _w,float angle)
{
  painter->setPen(NoPen);
  painter->setBrush(NoBrush);

  switch (figure)
    {
    case L_SQUARE:
      {
	int _h = _w;
	if (_h % 2 == 0) _h--;
	painter->save();
	painter->translate(coord.x(),coord.y());
	painter->rotate(angle);
	painter->scale(1,1);
	painter->fillRect(-3 - _w / 2,-3 - _h / 2,6 + _w,6 + _h,color);
	painter->restore();
      } break;
    case L_CIRCLE:
      {
	painter->save();
	painter->translate(coord.x(),coord.y());
	painter->setBrush(color);
	painter->drawEllipse(-3 - _w / 2,-3 - _w / 2,6 + _w,6 + _w);
	painter->restore();
      } break;
    case L_ARROW:
      {
  	QPoint p1(-5 - _w / 2,-3 - _w / 2);
  	QPoint p2(5 + _w / 2,0);
  	QPoint p3(-5 - _w / 2,3 + _w / 2);
	QPointArray pArray(3);
	pArray.setPoint(0,p1);
	pArray.setPoint(1,p2);
	pArray.setPoint(2,p3);
	
	painter->save();
	painter->translate(coord.x(),coord.y());
	painter->rotate(angle);
	painter->scale(1,1);
	painter->setBrush(color);
	painter->drawPolygon(pArray);
	painter->restore();
      } break;
    default: break;
    }
}

/*================== get bounding with of figure =================*/
QSize getBoundingSize(LineEnd figure,int _w)
{
  switch (figure)
    {
    case L_SQUARE:
      {
	int _h = _w;
	if (_h % 2 == 0) _h--;
	return QSize(6 + _w + 4,6 + _h + 4);
      } break;
    case L_CIRCLE:
      return QSize(6 + _w + 4,6 + _w + 4);
      break;
    case L_ARROW:
	return QSize(10 + _w + 4,6 + _w + 4);
	break;
    default: break;
    }

  return QSize(0,0);
}
