/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "kivio_ps_printer.h"

#include "kivio_line_style.h"
#include "kivio_fill_style.h"
#include "kivio_point.h"
#include "kivio_ps_printer.h"

#include <kdebug.h>

#define PS_CLOSEPATH	"cp"
#define PS_CURVETO		"ct"
#define PS_RCURVETO		"rct"
#define PS_FILL			"f"
#define PS_ARC			"a"
#define PS_EOFILL		"ef"
#define PS_EXCH			"ex"
#define PS_GRESTORE		"gr"
#define PS_GSAVE		"gs"
#define PS_SAVE			"sa"
#define PS_RECTFILL		"rectf"
#define PS_RECTSTROKE	"rects"
#define PS_RESTORE		"rs"
#define PS_LINETO		"l"
#define PS_MOVETO		"mt"
#define PS_RMOVETO		"rmt"
#define PS_NEWPATH		"n"
#define PS_STROKE		"s"
#define PS_SHOW			"sh"
#define PS_SETRGBCOLOR	"srgb"
#define PS_SETLINEWIDTH	"lw"
#define PS_TRANSLATE    "tr"
#define PS_SCALE        "sc"
#define PS_SHOWPAGE     "sp"

// This will convert a QColor element for use with postscript
#define PS_NORMALIZE_COLOR(x) (float(x)/255.0)

#define PS_CHECK_FP(x) { if( !m_fp ) { kdDebug() << "KivioPSPrinter::" << x << " - null m_fp"; return; } }

KivioPSPrinter::KivioPSPrinter()
    : KivioPainter()
{
    m_fileName = "";
    m_fp = NULL;
}

KivioPSPrinter::~KivioPSPrinter()
{
    if( m_fp )
    {
        fclose( m_fp );
        m_fp = NULL;
    }
}


void KivioPSPrinter::dumpColor( const QColor &c )
{
    PS_CHECK_FP("dumpColor");

    fprintf( m_fp, "%f %f %f %s\n", PS_NORMALIZE_COLOR(c.red()),
            PS_NORMALIZE_COLOR(c.green()), PS_NORMALIZE_COLOR(c.blue()), PS_SETRGBCOLOR );
}

bool KivioPSPrinter::start( const QString &_f, int numPages )
{
    m_fileName = QString(_f);

    m_fp = fopen( m_fileName.ascii(), "w+" );
    if( !m_fp )
        return false;

	fprintf(m_fp,
			"%%!PS-Adobe-2.0\n"
			"%%%%Creator: Kivio\n"
			"%%%%Title: %s\n"
			"%%%%Pages: %d\n"
			"%%%%PageOrder: Ascend\n"
			"%%%%BoundingBox: 0 0 596 842\n"
			"%%%%EndComments\n",
			m_fileName.ascii(), numPages );
			
	fprintf(m_fp,
			"%%BeginProlog\n"
			"/cp {closepath} bind def\n"
			"/ct {curveto} bind def\n"
			"/rct {rcurveto} bind def\n"
			"/f {fill} bind def\n"
			"/a {arc} bind def\n"
			"/ef {eofill} bind def\n"
			"/ex {exch} bind def\n"
			"/gr {grestore} bind def\n"
			"/gs {gsave} bind def\n"
			"/sa {save} bind def\n"
			"/rectf {rectfill} bind def\n"
			"/rects {rectstroke} bind def\n"
			"/rs {restore} bind def\n"
			"/l {lineto} bind def\n"
			"/mt {moveto} bind def\n"
			"/rmt {rmoveto} bind def\n"
			"/n {newpath} bind def\n"
			"/s {stroke} bind def\n"
			"/sh {show} bind def\n"
			"/srgb {setrgbcolor} bind def\n"
			"/lw {setlinewidth} bind def\n"
			"/sc {scale} bind def\n"
			"/tr {translate} bind def\n"
			"/sp {showpage} bind def\n"
			"%%EndProlog\n" );
			
	return true;
}

bool KivioPSPrinter::stop()
{
    if( !m_fp )
        return false;

    fclose( m_fp );
    m_fp = NULL;

    return true;
}

void KivioPSPrinter::drawLine( float x1, float y1, float x2, float y2 )
{
    PS_CHECK_FP("drawLine");

	fprintf( m_fp, "%f %s\n", m_pLineStyle->width(), PS_SETLINEWIDTH );
	dumpColor( m_pLineStyle->color() );
	

	fprintf(m_fp, "%f %f %s\n", x1, y1, PS_MOVETO );
	fprintf(m_fp, "%f %f %s\n", x2, y2, PS_LINETO );
	fprintf(m_fp, "%s\n", PS_STROKE);
}

void KivioPSPrinter::drawArc( float x, float y, float w, float h, float a1, float a2 )
{
	x=x; y=y; w=w; h=h; a1=a1; a2=a2;
}

void KivioPSPrinter::drawRect( float x, float y, float w, float h)
{
    PS_CHECK_FP("drawRect");

	fprintf( m_fp, "%f %s\n", m_pLineStyle->width(), PS_SETLINEWIDTH );
	dumpColor( m_pLineStyle->color() );

	fprintf(m_fp, "%f %f %f %f %s\n", x, y, w, h, PS_RECTSTROKE );
}

void KivioPSPrinter::fillRect( float x, float y, float w, float h )
{
    PS_CHECK_FP("fillRect");


	fprintf( m_fp, "%f %s\n", m_pLineStyle->width(), PS_SETLINEWIDTH );

	dumpColor( m_pFillStyle->color() );
	fprintf(m_fp, "%f %f %f %f %s\n", x,y,w,h, PS_RECTFILL );
	
	dumpColor( m_pLineStyle->color() );
	fprintf(m_fp, "%f %f %f %f %s\n", x, y, w, h, PS_RECTSTROKE );
}

void KivioPSPrinter::drawBezier( QPointArray &pArray )
{
    PS_CHECK_FP("drawBezier");
    	
	QPoint p1, p2, p3, p4;
	p1 = pArray.point(0);
	p2 = pArray.point(1);
	p3 = pArray.point(2);
	p4 = pArray.point(3);

	fprintf( m_fp, "%f %s\n", m_pLineStyle->width(), PS_SETLINEWIDTH );
	dumpColor( m_pLineStyle->color() );
	
	fprintf( m_fp, "%d %d %s\n", p1.x(), p1.y(), PS_MOVETO );
	fprintf( m_fp, "%d %d %d %d %d %d %s\n", p2.x(), p2.y(), p3.x(), p3.y(), p4.x(), p4.y(), PS_CURVETO );
	fprintf( m_fp, "%s\n", PS_STROKE );
}

void KivioPSPrinter::fillEllipse( float x, float y, float w, float h )
{
    PS_CHECK_FP("fillEllipse");

    fprintf( m_fp, "%f %s\n", m_pLineStyle->width(), PS_SETLINEWIDTH );

	switch( m_pFillStyle->colorStyle() )
	{
		case KivioFillStyle::kcsNone: // hollow
		   kdDebug() << "KivioPSPrinter::fillEllipse() - Cannot draw a hollow-filled-ellipse. Shape aborted.";
		   return;
			
		case KivioFillStyle::kcsSolid: // filled
		   dumpColor( m_pFillStyle->color() );
		   break;
			
		case KivioFillStyle::kcsGradient:
		default:
		   kdDebug() << "KivioPrinter::fillEllipse() - Gradient unsupported. Shape aborted.\n";
		   return;
	}

    fprintf( m_fp, "%s\n", PS_SAVE );
    fprintf( m_fp, "%f %f %s\n", x+w/2.0f, y+w/2.0f, PS_TRANSLATE );
    fprintf( m_fp, "%f %f %s\n", 1.0f, h / w, PS_SCALE );
    fprintf( m_fp, "0 0 %f 0 360 %s\n", w / 2.0f, PS_ARC );
    fprintf( m_fp, "%s\n", PS_FILL );
    fprintf( m_fp, "0 0 %f 0 360 %s\n", w / 2.0f, PS_ARC );
    dumpColor( m_pLineStyle->color() );
    fprintf( m_fp, "%s\n", PS_STROKE );
    fprintf( m_fp, "%s\n", PS_RESTORE );

}

void KivioPSPrinter::drawEllipse( float x, float y, float w, float h )
{
    PS_CHECK_FP("drawEllipse");

    dumpColor( m_pLineStyle->color() );
    fprintf( m_fp, "%f %s\n", m_pLineStyle->width(), PS_SETLINEWIDTH );


    fprintf( m_fp, "%s\n", PS_SAVE );
    fprintf( m_fp, "%f %f %s\n", x+w/2.0f, y+w/2.0f, PS_TRANSLATE );
    fprintf( m_fp, "%f %f %s\n", 1.0f, h / w, PS_SCALE );
    fprintf( m_fp, "0 0 %f 0 360 %s\n", w / 2.0f, PS_ARC );
    fprintf( m_fp, "%s\n", PS_FILL );
    fprintf( m_fp, "%s\n", PS_RESTORE );
}

void KivioPSPrinter::drawLineArray( QList< KivioPoint > *pList )
{
    PS_CHECK_FP("drawLines");

	KivioPoint *sPoint, *ePoint;

	fprintf( m_fp, "%f %s\n", m_pLineStyle->width(), PS_SETLINEWIDTH );
	dumpColor( m_pLineStyle->color() );
	
	sPoint = pList->first();
	while( sPoint )
	{
		ePoint = pList->next();
		if( !ePoint )
		{
		   kdDebug() << "KivioPSPrinter::drawLines() - sPoint without ePoint (shape aborted)\n";
			return;
		}
		
		fprintf( m_fp, "%f %f %s\n", sPoint->x(), sPoint->y(), PS_MOVETO );
		fprintf( m_fp, "%f %f %s\n", ePoint->x(), ePoint->y(), PS_LINETO );
		fprintf( m_fp, "%s\n", PS_STROKE );
		
		sPoint = pList->next();
	}
}

void KivioPSPrinter::drawPolyline( QList<KivioPoint> *pList )
{
    PS_CHECK_FP("drawPolyline");

	KivioPoint *pPoint;

	fprintf( m_fp, "%f %s\n", m_pLineStyle->width(), PS_SETLINEWIDTH );
	dumpColor( m_pLineStyle->color() );

	pPoint = pList->first();
	fprintf(m_fp,  "%f %f %s\n", pPoint->x(), pPoint->y(), PS_MOVETO );
	pPoint = pList->next();
	while( pPoint )
	{
		fprintf(m_fp,  "%f %f %s\n", pPoint->x(), pPoint->y(), PS_MOVETO );
		
		pPoint = pList->next();
	}
	
	fprintf( m_fp, "%s\n", PS_STROKE );
}

void KivioPSPrinter::drawPolygon( QList< KivioPoint> *pList )
{
    PS_CHECK_FP("drawPolygon");

	KivioPoint *pPoint;
	QColor bgColor = m_pFillStyle->color();

	fprintf( m_fp, "%f %s\n", m_pLineStyle->width(), PS_SETLINEWIDTH );
	
	switch( m_pFillStyle->colorStyle() )
	{
		case KivioFillStyle::kcsNone: // hollow
			dumpColor( m_pLineStyle->color() );
			break;
			
		case KivioFillStyle::kcsSolid: // filled
			dumpColor( bgColor );
			break;
			
		case KivioFillStyle::kcsGradient:
		default:
		   kdDebug() << "KivioPrinter::drawPolygon() - Gradient unsupported\n";
			break;
	}
	
	pPoint = pList->first();
	fprintf(m_fp,  "%f %f %s\n", pPoint->x(), pPoint->y(), PS_MOVETO );
	pPoint = pList->next();
	while( pPoint )
	{
		fprintf(m_fp,  "%f %f %s\n", pPoint->x(), pPoint->y(), PS_LINETO );
		
		pPoint = pList->next();
	}
	
	fprintf( m_fp, "%s\n", PS_CLOSEPATH );

		
	switch( m_pFillStyle->colorStyle() )
	{
		case KivioFillStyle::kcsNone: // hollow
			fprintf( m_fp, "%s\n", PS_STROKE );
			break;
			
		case KivioFillStyle::kcsSolid: // filled
			fprintf( m_fp, "%s\n", PS_GSAVE );
			fprintf( m_fp, "%s\n", PS_FILL );
			dumpColor( m_pLineStyle->color() );
			fprintf( m_fp, "%s\n", PS_GRESTORE );
			fprintf( m_fp, "%s\n", PS_STROKE );
			break;
			
		case KivioFillStyle::kcsGradient:
		default:
		   kdDebug() << "KivioPSPrinter::drawPolygon() - Gradient unsupported\n";
			break;
	}
}

void KivioPSPrinter::drawLineArray( QPointArray &pArray  )
{
    PS_CHECK_FP("drawLineArray");

	QPoint sPoint, ePoint;

	fprintf( m_fp, "%f %s\n", m_pLineStyle->width(), PS_SETLINEWIDTH );
	dumpColor( m_pLineStyle->color() );
	
	for( int i=0; i<(int)(pArray.count()/2); i++ )
	{
		sPoint = pArray[i<<1];
		ePoint = pArray[(i<<1)+1];
		
		fprintf( m_fp, "%d %d %s\n", sPoint.x(), sPoint.y(), PS_MOVETO );
		fprintf( m_fp, "%d %d %s\n", ePoint.x(), ePoint.y(), PS_MOVETO );
		fprintf( m_fp, "%s\n", PS_STROKE );
	}	
}

void KivioPSPrinter::drawPolyline( QPointArray &pArray )
{
    PS_CHECK_FP("drawPolyline");

	QPoint pPoint;
	int i=0;

	fprintf( m_fp, "%f %s\n", m_pLineStyle->width(), PS_SETLINEWIDTH );
	dumpColor( m_pLineStyle->color() );

	pPoint = pArray[i++];
	fprintf(m_fp,  "%d %d %s\n", pPoint.x(), pPoint.y(), PS_MOVETO );

	for( ; i<(int)pArray.count(); i++ )
	{
		pPoint = pArray[i];
		
		fprintf(m_fp,  "%d %d %s\n", pPoint.x(), pPoint.y(), PS_LINETO );
	}
	
	fprintf( m_fp, "%s\n", PS_STROKE );
}


void KivioPSPrinter::drawPolygon( QPointArray &pArray )
{
    PS_CHECK_FP("drawPolygon");

	QPoint pPoint;
	QColor bgColor = m_pFillStyle->color();
	int i;

	fprintf( m_fp, "%f %s\n", m_pLineStyle->width(), PS_SETLINEWIDTH );
	
	switch( m_pFillStyle->colorStyle() )
	{
		case KivioFillStyle::kcsNone: // hollow
			dumpColor( m_pLineStyle->color() );
			break;
			
		case KivioFillStyle::kcsSolid: // filled
			dumpColor( bgColor );
			break;
			
		case KivioFillStyle::kcsGradient:
		default:
		   kdDebug() << "KivioPSPrinter::drawPolygon() - Gradient unsupported\n";
		   break;
	}

	i=0;	
	pPoint = pArray[i++];
	fprintf(m_fp,  "%d %d %s\n", pPoint.x(), pPoint.y(), PS_MOVETO );

	for( ; i<(int)pArray.count(); i++ )
	{
		pPoint = pArray[i];
		fprintf(m_fp,  "%d %d %s\n", pPoint.x(), pPoint.y(), PS_LINETO );
	}
	
	fprintf( m_fp, "%s\n", PS_CLOSEPATH );
	
	switch( m_pFillStyle->colorStyle() )
	{
		case KivioFillStyle::kcsNone: // hollow
			fprintf( m_fp, "%s\n", PS_STROKE );
			break;
			
		case KivioFillStyle::kcsSolid: // filled
			fprintf( m_fp, "%s\n", PS_GSAVE );
			fprintf( m_fp, "%s\n", PS_FILL );
			fprintf( m_fp, "%s\n", PS_GRESTORE );
			dumpColor( m_pLineStyle->color() );
			fprintf( m_fp, "%s\n", PS_STROKE );

			break;
			
		case KivioFillStyle::kcsGradient:
		default:
		   kdDebug() << "KivioPSPrinter::drawPolygon() - Gradient unsupported\n";
			break;
	}
}

void KivioPSPrinter::drawOpenPath( QList< KivioPoint > * pList )
{
    PS_CHECK_FP("drawOpenPath");

	KivioPoint *pPoint, *pLastPoint, *p2, *p3, *p4;
	
	dumpColor( m_pLineStyle->color() );
	fprintf(m_fp, "%f %s\n", m_pLineStyle->width(), PS_SETLINEWIDTH );
	
	pPoint = pList->first();
	pLastPoint = NULL;
	while( pPoint )
	{
		switch( pPoint->pointType() )
		{
			case KivioPoint::kptNormal:
				if( !pLastPoint )
				{
					pLastPoint = pPoint;
					fprintf( m_fp, "%f %f %s\n", pPoint->x(), pPoint->y(), PS_MOVETO );
				}
				else
				{
					fprintf( m_fp, "%f %f %s\n", pPoint->x(), pPoint->y(), PS_LINETO );
				}
				break;
				
			case KivioPoint::kptBezier:
				p2 = pList->next();
				p3 = pList->next();
				p4 = pList->next();
				
				if( !pLastPoint )
				{
					pLastPoint = p4;
					fprintf( m_fp, "%f %f %s\n", pPoint->x(), pPoint->y(), PS_MOVETO );
				}
				else
				{
					fprintf( m_fp, "%f %f %s\n", pPoint->x(), pPoint->y(), PS_LINETO );
				}
				
				fprintf( m_fp, "%f %f %f %f %f %f %s\n", p2->x(), p2->y(), p3->x(), p3->y(),
						p4->x(), p4->y(), PS_CURVETO );
				break;
				
			case KivioPoint::kptArc:
			case KivioPoint::kptNone:
			case KivioPoint::kptLast:
				break;
		}
		
		pPoint = pList->next();
	}
	
	fprintf(m_fp, "%s\n", PS_STROKE );
}

void KivioPSPrinter::drawClosedPath( QList< KivioPoint > * pList )
{
    PS_CHECK_FP("drawClosedPath");

	KivioPoint *pPoint, *pLastPoint, *p2, *p3, *p4;
	
	
	pPoint = pList->first();
	pLastPoint = NULL;
	while( pPoint )
	{
		switch( pPoint->pointType() )
		{
			case KivioPoint::kptNormal:
				if( !pLastPoint )
				{
					pLastPoint = pPoint;
					fprintf( m_fp, "%f %f %s\n", pPoint->x(), pPoint->y(), PS_MOVETO );
				}
				else
				{
					fprintf( m_fp, "%f %f %s\n", pPoint->x(), pPoint->y(), PS_LINETO );
				}
				break;
				
			case KivioPoint::kptBezier:
				p2 = pList->next();
				p3 = pList->next();
				p4 = pList->next();
				
				if( !pLastPoint )
				{
					pLastPoint = p4;
					fprintf( m_fp, "%f %f %s\n", pPoint->x(), pPoint->y(), PS_MOVETO );
				}
				else
				{
					fprintf( m_fp, "%f %f %s\n", pPoint->x(), pPoint->y(), PS_LINETO );
				}
				
				fprintf( m_fp, "%f %f %f %f %f %f %s\n", p2->x(), p2->y(), p3->x(), p3->y(),
						p4->x(), p4->y(), PS_CURVETO );
				break;
				
			case KivioPoint::kptArc:
			case KivioPoint::kptNone:
			case KivioPoint::kptLast:
				break;
		}
		
		pPoint = pList->next();
	}
	
	fprintf(m_fp, "%s\n", PS_CLOSEPATH );
	fprintf(m_fp, "%f %s\n", m_pLineStyle->width(), PS_SETLINEWIDTH );
	
	switch( m_pFillStyle->colorStyle() )
	{
		case KivioFillStyle::kcsNone:
		    dumpColor( m_pLineStyle->color() );
			fprintf( m_fp, "%s\n", PS_STROKE );
			break;
			
		case KivioFillStyle::kcsSolid:
		    dumpColor( m_pFillStyle->color() );
			fprintf( m_fp, "%s\n", PS_GSAVE );
			fprintf( m_fp, "%s\n", PS_FILL );
			fprintf( m_fp, "%s\n", PS_GRESTORE );
			dumpColor( m_pLineStyle->color() );
			fprintf( m_fp, "%s\n", PS_STROKE );
			break;
			
		case KivioFillStyle::kcsGradient:
		default:
			break;
	}
}

void KivioPSPrinter::startPage( int num )
{
    PS_CHECK_FP("startPage");

    fprintf(m_fp, "%%%%Page: %d\n", num);
}

void KivioPSPrinter::stopPage()
{
    PS_CHECK_FP("stopPage");

    fprintf(m_fp, "%s\n", PS_SHOWPAGE);
}
