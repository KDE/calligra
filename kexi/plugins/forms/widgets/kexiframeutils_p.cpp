/* This file is part of the KDE project
   Copyright (C) 2005-2006 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

/* This file is included by KexiDBLabel and KexiFrame */

//! @todo add more frame types
void ClassName::drawFrame( QPainter *p )
{
  if (frameShape() == QFrame::Box) {
    if ( frameShadow() == Plain )
      qDrawPlainRect( p, frameRect(), d->frameColor, lineWidth() );
    else
      qDrawShadeRect( p, frameRect(), colorGroup(), frameShadow() == QFrame::Sunken, 
        lineWidth(), midLineWidth() );
  }
  else {
    SuperClassName::drawFrame(p);
  }
}

void ClassName::setPalette( const QPalette &pal )
{
  QPalette pal2(pal);
  QColorGroup cg( pal2.active() );
  cg.setColor(QColorGroup::Light, KexiUtils::bleachedColor( d->frameColor, 150 ));
  cg.setColor(QColorGroup::Mid, d->frameColor);
  cg.setColor(QColorGroup::Dark, d->frameColor.dark(150));
  pal2.setActive(cg);
  QColorGroup cg2( pal2.inactive() );
  cg2.setColor(QColorGroup::Light, cg.light() );
  cg2.setColor(QColorGroup::Mid, cg.mid());
  cg2.setColor(QColorGroup::Dark, cg.dark());
  pal2.setInactive(cg2);
  SuperClassName::setPalette(pal2);
}

const QColor& ClassName::frameColor() const
{
  return d->frameColor;
}

void ClassName::setFrameColor(const QColor& color)
{
  d->frameColor = color;
  //update light and dark colors
  setPalette( palette() );
}

#if 0
//todo
ClassName::Shape ClassName::frameShape() const
{
  return d->frameShape;
}

void ClassName::setFrameShape( ClassName::Shape shape )
{
  d->frameShape = shape;
  update();
}

ClassName::Shadow ClassName::frameShadow() const
{
  return d->frameShadow;
}

void ClassName::setFrameShadow( ClassName::Shadow shadow )
{
  d->frameShadow = shadow;
  update();
}
#endif

#if 0
void QFrame::drawFrame( QPainter *p )
{
    QPoint      p1, p2;
    QRect       r     = frameRect();
    int         type  = fstyle & MShape;
    int         cstyle = fstyle & MShadow;
#ifdef QT_NO_DRAWUTIL
    p->setPen( black ); // ####
    p->drawRect( r ); //### a bit too simple
#else
    const QColorGroup & g = colorGroup();

#ifndef QT_NO_STYLE
    QStyleOption opt(lineWidth(),midLineWidth());

    QStyle::SFlags flags = QStyle::Style_Default;
    if (isEnabled())
  flags |= QStyle::Style_Enabled;
    if (cstyle == Sunken)
  flags |= QStyle::Style_Sunken;
    else if (cstyle == Raised)
  flags |= QStyle::Style_Raised;
    if (hasFocus())
  flags |= QStyle::Style_HasFocus;
    if (hasMouse())
  flags |= QStyle::Style_MouseOver;
#endif // QT_NO_STYLE

    switch ( type ) {

    case Box:
        if ( cstyle == Plain )
            qDrawPlainRect( p, r, g.foreground(), lwidth );
        else
            qDrawShadeRect( p, r, g, cstyle == Sunken, lwidth,
                            midLineWidth() );
        break;

    case LineEditPanel:
  style().drawPrimitive( QStyle::PE_PanelLineEdit, p, r, g, flags, opt );
  break;

    case GroupBoxPanel:
  style().drawPrimitive( QStyle::PE_PanelGroupBox, p, r, g, flags, opt );
  break;

    case TabWidgetPanel:
  style().drawPrimitive( QStyle::PE_PanelTabWidget, p, r, g, flags, opt );
  break;

    case MenuBarPanel:
#ifndef QT_NO_STYLE
  style().drawPrimitive(QStyle::PE_PanelMenuBar, p, r, g, flags, opt);
  break;
#endif // fall through to Panel if QT_NO_STYLE

    case ToolBarPanel:
#ifndef QT_NO_STYLE
  style().drawPrimitive( QStyle::PE_PanelDockWindow, p, rect(), g, flags, opt);
        break;
#endif // fall through to Panel if QT_NO_STYLE

    case StyledPanel:
#ifndef QT_NO_STYLE
        if ( cstyle == Plain )
            qDrawPlainRect( p, r, g.foreground(), lwidth );
        else
      style().drawPrimitive(QStyle::PE_Panel, p, r, g, flags, opt);
        break;
#endif // fall through to Panel if QT_NO_STYLE

    case PopupPanel:
#ifndef QT_NO_STYLE
    {
  int vextra = style().pixelMetric(QStyle::PM_PopupMenuFrameVerticalExtra, this),
      hextra = style().pixelMetric(QStyle::PM_PopupMenuFrameHorizontalExtra, this);
  if(vextra > 0 || hextra > 0) {
      QRect fr = frameRect();
      int   fw = frameWidth();
      if(vextra > 0) {
    style().drawControl(QStyle::CE_PopupMenuVerticalExtra, p, this,
            QRect(fr.x() + fw, fr.y() + fw, fr.width() - (fw*2), vextra),
            g, flags, opt);
    style().drawControl(QStyle::CE_PopupMenuVerticalExtra, p, this,
            QRect(fr.x() + fw, fr.bottom() - fw - vextra, fr.width() - (fw*2), vextra),
            g, flags, opt);
      }
      if(hextra > 0) {
    style().drawControl(QStyle::CE_PopupMenuHorizontalExtra, p, this,
            QRect(fr.x() + fw, fr.y() + fw + vextra, hextra, fr.height() - (fw*2) - vextra),
            g, flags, opt);
    style().drawControl(QStyle::CE_PopupMenuHorizontalExtra, p, this,
            QRect(fr.right() - fw - hextra, fr.y() + fw + vextra, hextra, fr.height() - (fw*2) - vextra),
            g, flags, opt);
      }
  }

        if ( cstyle == Plain )
            qDrawPlainRect( p, r, g.foreground(), lwidth );
        else
      style().drawPrimitive(QStyle::PE_PanelPopup, p, r, g, flags, opt);
        break;
    }
#endif // fall through to Panel if QT_NO_STYLE

    case Panel:
        if ( cstyle == Plain )
            qDrawPlainRect( p, r, g.foreground(), lwidth );
        else
            qDrawShadePanel( p, r, g, cstyle == Sunken, lwidth );
        break;

    case WinPanel:
        if ( cstyle == Plain )
            qDrawPlainRect( p, r, g.foreground(), wpwidth );
        else
            qDrawWinPanel( p, r, g, cstyle == Sunken );
        break;
    case HLine:
    case VLine:
        if ( type == HLine ) {
            p1 = QPoint( r.x(), r.height()/2 );
            p2 = QPoint( r.x()+r.width(), p1.y() );
        }
        else {
            p1 = QPoint( r.x()+r.width()/2, 0 );
            p2 = QPoint( p1.x(), r.height() );
        }
        if ( cstyle == Plain ) {
            QPen oldPen = p->pen();
            p->setPen( QPen(g.foreground(),lwidth) );
            p->drawLine( p1, p2 );
            p->setPen( oldPen );
        }
        else
            qDrawShadeLine( p, p1, p2, g, cstyle == Sunken,
                            lwidth, midLineWidth() );
        break;
    }
#endif // QT_NO_DRAWUTIL

#endif
