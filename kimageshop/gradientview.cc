// only for debug
#include <iostream.h>

#include <qpainter.h>

#include "gradientview.h"

GradientView::GradientView( QWidget *_parent, const char *_name )
  : QWidget( _parent, _name )
{
  addItem( red   , green , 0.10, 0.40 );
  addItem( green , blue  , 0.45, 0.50 );
  addItem( blue  , yellow, 0.52, 0.55 );
  addItem( yellow, red   , 0.68, 0.70 );
  addItem( red   , green , 0.90, 1.00 );
}

GradientView::~GradientView()
{
}

void GradientView::addItem( QColor _leftColor, QColor _rightColor, float _middle, float _right )
{
  GradientItem *item = new GradientItem;

  item->leftColor = _leftColor;
  item->rightColor = _rightColor;
  item->middle = _middle;
  item->right = _right;

  m_lstGradientItems.append( item );
}

void GradientView::paintEvent( QPaintEvent *_event )
{
  int w, pos;
  float index;
  QPainter p;

  pos = 0;
  index = 0.0;
  w = width();

  p.begin( this );

  GradientItem* item = m_lstGradientItems.first();

  for( int i = 0; i < m_lstGradientItems.count(); i++ )
  {
    w = (int) (( item->right - index ) * (float) width());

    cout << "Michael : processing gradient item, width : " << w << endl;

    p.fillRect( pos, 0, w, height(), item->leftColor );
    pos += w;
    index = item->right;

    item = m_lstGradientItems.next();
  }
//p.fillRect( 0, 0, width(), height(), QBrush( blue) );

  p.end();
}

#include "gradientview.moc"












