/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
                 2001 Werner Trobin <trobin@kde.org>

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

#include <ruler.h>

#include <qpainter.h>
#include <qpopupmenu.h>
#include <klocale.h>

Ruler::Ruler(QWidget *parent, QWidget *canvas, Qt::Orientation orientation,
             const Graphite::PageLayout &layout, const double &zoomedRes) :
    QFrame(parent, "ruler", Qt::WResizeNoErase | Qt::WRepaintNoErase),
    m_canvas(canvas), m_orientation(orientation), m_layout(layout),
    m_zoomedRes(zoomedRes), m_buffer(1, 1), m_unit(Graphite::MM), m_dx(0),
    m_dy(0), m_MX(0), m_MY(0), m_oldMX(0), m_oldMY(0), m_action(A_NONE),
    m_mousePressed(false), m_showMPos(false), m_haveToDelete(false),
    m_movingFirstBorder(false), m_movingSecondBorder(false), m_editable(true) {

    setFrameStyle(QFrame::Box | QFrame::Raised);
    setMouseTracking(true);

    m_1_zoomedRes=1/m_zoomedRes;

    m_menu=new QPopupMenu();
    m_MMIndex=m_menu->insertItem(i18n("&Millimeters (mm)"), this, SLOT(rbMM()));
    m_PTIndex=m_menu->insertItem(i18n("&Points (pt)"), this, SLOT(rbPT()));
    m_INCHIndex=m_menu->insertItem(i18n("&Inches (inch)"), this, SLOT(rbINCH()));
    m_menu->insertSeparator();
    m_menu->insertItem(i18n("Page &Layout..."), this, SIGNAL(openPageLayoutDia()));
    m_menu->setCheckable(false);
    m_menu->setItemChecked(m_MMIndex, true);
}

Ruler::~Ruler() {
    delete m_menu;
}

void Ruler::setUnit(const Graphite::Unit &unit) {

    if(m_unit==unit)
        return;

    m_unit=unit;
    m_menu->setItemChecked(m_MMIndex, false);
    m_menu->setItemChecked(m_PTIndex, false);
    m_menu->setItemChecked(m_INCHIndex, false);

    if(unit==Graphite::MM)
        m_menu->setItemChecked(m_MMIndex, true);
    else if(unit==Graphite::Pt)
        m_menu->setItemChecked(m_PTIndex, true);
    else if(unit==Graphite::Inch)
        m_menu->setItemChecked(m_INCHIndex, true);
    repaint(false);
}

void Ruler::showMousePos(bool showMPos) {

    m_showMPos=showMPos;
    m_haveToDelete=false;
    m_MX=-1;
    m_MY=-1;
    repaint(false);
}

void Ruler::setMousePos(int mx, int my) {

    if(!m_showMPos || (mx==m_MX && my==m_MY))
        return;

    QPainter p(this);
    p.setRasterOp(Qt::NotROP);

    if(m_orientation==Qt::Horizontal) {
        if(m_haveToDelete)
            p.drawLine(m_MX, 1, m_MX, height()-1);
        p.drawLine(mx, 1, mx, height()-1);
        m_haveToDelete=true;
    }
    else {  // vertical
        if(m_haveToDelete)
            p.drawLine(1, m_MY, width()-1, m_MY);
        p.drawLine(1, my, width()-1, my);
        m_haveToDelete=true;
    }
    p.end();
    m_MX=mx;
    m_MY=my;
}

void Ruler::setZoomedRes(const double &zoomedRes) {

    if(m_zoomedRes==zoomedRes)
        return;
    m_zoomedRes=zoomedRes;
    m_1_zoomedRes=1/m_zoomedRes;
    repaint(false);
}

void Ruler::drawHorizontal(QPainter *painter) {

    QPainter p(&m_buffer);
    p.fillRect(0, 0, width(), height(), colorGroup().brush(QColorGroup::Background));

    double pw=m_layout.width()*m_zoomedRes;
    double zoom=m_zoomedRes/GraphiteGlobal::self()->resolution();
    int i_pw=Graphite::double2Int(pw);

    p.setBrush(colorGroup().brush(QColorGroup::Light));

    QRect r;
    if(!m_movingFirstBorder)
        r.setLeft(-m_dx+Graphite::double2Int(m_layout.borders.left*m_zoomedRes));
    else
        r.setLeft(m_oldMX);
    r.setTop(0);
    if(!m_movingSecondBorder)
        r.setRight(-m_dx+i_pw-Graphite::double2Int(m_layout.borders.right*m_zoomedRes));
    else
        r.setRight(m_oldMX);
    r.setBottom(height());
    p.drawRect(r);

    QFont font=QFont(QString::fromLatin1("helvetica"), 7);
    QFontMetrics fm(font);
    p.setFont(font);

    double dist;
    if(m_unit==Graphite::Inch)
        dist=QPaintDevice::x11AppDpiY()*zoom;
    else if(m_unit==Graphite::Pt)
        dist=QPaintDevice::x11AppDpiY()*1.38888888888889*zoom;
    else
        dist=10.0*m_zoomedRes;

    QString str;
    int j=0;
    for(double i=0.0; i<=pw; i+=dist) {
        str=QString::number(j++);
        if(m_unit==Graphite::MM && j!=1) // x10
            str+='0';
        if(m_unit==Graphite::Pt && j!=1) // x100
            str+=QString::fromLatin1("00");
        p.drawText(Graphite::double2Int(i)-m_dx-Graphite::double2Int(fm.width(str)*0.5),
                    Graphite::double2Int((height()-fm.height())*0.5),
                    fm.width(str), height(), AlignLeft | AlignTop, str);
    }

    for(double i=dist*0.5; i<=pw; i+=dist) {
        int ii=Graphite::double2Int(i);
        p.drawLine(ii-m_dx, 5, ii-m_dx, height()-5);
    }

    for(double i=dist*0.25; i<=pw; i+=dist*0.5) {
        int ii=Graphite::double2Int(i);
        p.drawLine(ii-m_dx, 7, ii-m_dx, height()-7);
    }

    p.drawLine(i_pw-m_dx+1, 1, i_pw-m_dx+1, height()-1);
    p.drawLine(-m_dx, 1, -m_dx, height()-1);
    p.setPen(Qt::white);
    p.drawLine(i_pw-m_dx, 1, i_pw-m_dx, height()-1);
    p.drawLine(-m_dx-1, 1, -m_dx-1, height()-1);
    p.setPen(Qt::black);

    if(m_action==A_NONE && m_showMPos)
        p.drawLine(m_MX, 1, m_MX, height()-1);
    m_haveToDelete=false;

    p.end();
    painter->drawPixmap(0, 0, m_buffer);
}

void Ruler::drawVertical(QPainter *painter) {

    QPainter p(&m_buffer);
    p.fillRect(0, 0, width(), height(), QBrush(colorGroup().brush(QColorGroup::Background)));

    double ph=m_layout.height()*m_zoomedRes;
    double zoom=m_zoomedRes/GraphiteGlobal::self()->resolution();
    int i_ph=Graphite::double2Int(ph);

    p.setBrush(colorGroup().brush(QColorGroup::Light));

    QRect r;
    if(!m_movingFirstBorder)
        r.setTop(-m_dy+Graphite::double2Int(m_layout.borders.top*m_zoomedRes));
    else
        r.setTop(m_oldMY);
    r.setLeft(0);
    if(!m_movingSecondBorder)
        r.setBottom(-m_dy+i_ph-Graphite::double2Int(m_layout.borders.bottom*m_zoomedRes));
    else
        r.setBottom(m_oldMY);
    r.setRight(width());
    p.drawRect(r);

    QFont font=QFont(QString::fromLatin1("helvetica"), 7);
    QFontMetrics fm(font);
    p.setFont(font);

    double dist;
    if(m_unit==Graphite::Inch)
        dist=QPaintDevice::x11AppDpiY()*zoom;
    else if(m_unit==Graphite::Pt)
        dist=QPaintDevice::x11AppDpiY()*1.38888888888889*zoom;
    else
        dist=10.0*m_zoomedRes;

    QString str;
    int j=0;
    for(double i=0.0; i<=ph; i+=dist) {
        str=QString::number(j++);
        if(m_unit==Graphite::MM && j!=1)  // x10
            str+='0';
        if(m_unit==Graphite::Pt && j!=1)  // x100
            str+=QString::fromLatin1("00");
        p.drawText(Graphite::double2Int((width()-fm.width(str))*0.5),
                    Graphite::double2Int(i)-m_dy-Graphite::double2Int(fm.height()*0.5),
                    width(), fm.height(), AlignLeft | AlignTop, str);
    }

    for(double i=dist*0.5; i<=ph; i+=dist) {
        int ii=Graphite::double2Int(i);
        p.drawLine(5, ii-m_dy, width()-5, ii-m_dy);
    }

    for(double i=dist*0.25; i<=ph; i+=dist*0.5) {
        int ii=Graphite::double2Int(i);
        p.drawLine(7, ii-m_dy, width()-7, ii-m_dy);
    }

    p.drawLine(1, i_ph-m_dy+1, width()-1, i_ph-m_dy+1);
    p.drawLine(1, -m_dy, width()-1, -m_dy);
    p.setPen(Qt::white);
    p.drawLine(1, i_ph-m_dy, width()-1, i_ph-m_dy);
    p.drawLine(1, -m_dy-1, width()-1, -m_dy-1);
    p.setPen(Qt::black);

    if(m_action==A_NONE && m_showMPos)
        p.drawLine(1, m_MY, width()-1, m_MY);
    m_haveToDelete=false;

    p.end();
    painter->drawPixmap(0, 0, m_buffer);
}

void Ruler::mousePressEvent(QMouseEvent *e) {

    if(!m_editable)
        return;

    m_oldMX=e->x();
    m_oldMY=e->y();
    m_mousePressed=true;

    if(e->button()==RightButton) {
        m_menu->popup(QCursor::pos());
        m_action=A_NONE;
        m_mousePressed=false;
        return;
    }

    if(m_action==A_BR_RIGHT || m_action==A_BR_LEFT) {
        if(m_action==A_BR_RIGHT)
            m_movingSecondBorder=true;
        else
            m_movingFirstBorder=true;

        if(m_canvas) {
            QPainter p(m_canvas);
            p.setRasterOp(Qt::NotROP);
            p.drawLine(m_oldMX, 0, m_oldMX, m_canvas->height());
            p.end();
        }
        repaint(false);
    } else if(m_action==A_BR_TOP || m_action==A_BR_BOTTOM) {
        if(m_action==A_BR_TOP)
            m_movingFirstBorder=true;
        else
            m_movingSecondBorder=true;

        if (m_canvas) {
            QPainter p(m_canvas);
            p.setRasterOp(Qt::NotROP);
            p.drawLine(0, m_oldMY, m_canvas->width(), m_oldMY);
            p.end();
        }
        repaint(false);
    }
}

void Ruler::mouseReleaseEvent(QMouseEvent *) {

    if(!m_editable)
        return;

    m_mousePressed=false;

    if(m_action==A_BR_RIGHT || m_action==A_BR_LEFT) {
        m_movingFirstBorder=false;
        m_movingSecondBorder=false;

        if(m_canvas) {
            QPainter p(m_canvas);
            p.setRasterOp(Qt::NotROP);
            p.drawLine(m_oldMX, 0, m_oldMX, m_canvas->height());
            p.end();
        }
        repaint(false);
        emit pageBordersChanged(m_layout.borders);
    } else if(m_action==A_BR_TOP || m_action==A_BR_BOTTOM) {
        m_movingFirstBorder=false;
        m_movingSecondBorder=false;

        if(m_canvas) {
            QPainter p(m_canvas);
            p.setRasterOp(Qt::NotROP);
            p.drawLine(0, m_oldMY, m_canvas->width(), m_oldMY);
            p.end();
        }
        repaint(false);
        emit pageBordersChanged(m_layout.borders);
    }
}

void Ruler::mouseMoveEvent(QMouseEvent *e) {

    if(!m_editable)
        return;

    m_haveToDelete=false;

    int pw=Graphite::double2Int(m_layout.width()*m_zoomedRes);
    int ph=Graphite::double2Int(m_layout.height()*m_zoomedRes);
    int left=Graphite::double2Int(m_layout.borders.left*m_zoomedRes)-m_dx;
    int top=Graphite::double2Int(m_layout.borders.top*m_zoomedRes)-m_dy;
    int right=pw-Graphite::double2Int(m_layout.borders.right*m_zoomedRes)-m_dx;
    int bottom=ph-Graphite::double2Int(m_layout.borders.bottom*m_zoomedRes)-m_dy;

    int mx=e->x()+m_dx<0 ? 0 : e->x();
    int my=e->y()+m_dy<0 ? 0 : e->y();

    if(m_orientation==Qt::Horizontal) {
        if(!m_mousePressed) {
            setCursor(Qt::arrowCursor);
            m_action=A_NONE;
            if(mx>left-5 && mx<left+5) {
                setCursor(Qt::sizeHorCursor);
                m_action=A_BR_LEFT;
            }
            else if(mx>right-5 && mx<right+5) {
                setCursor(Qt::sizeHorCursor);
                m_action=A_BR_RIGHT;
            }
        }
        else {
            // Note: All limits should be 0, but KWord crashes currently, when the
            // page is too small (infinite loop in the line breaking algorithm)! (Werner)
            switch(m_action) {
                case A_BR_LEFT:
                    if(m_canvas && mx<right-10) {
                        QPainter p(m_canvas);
                        p.setRasterOp(Qt::NotROP);
                        p.drawLine(m_oldMX, 0, m_oldMX, m_canvas->height());
                        p.drawLine(mx, 0, mx, m_canvas->height());
                        p.end();
                        m_layout.borders.left=static_cast<double>(mx+m_dx)*m_1_zoomedRes;
                        m_oldMX=mx;
                        m_oldMY=my;
                        repaint(false);
                    }
                    else
                        return;
                    break;
                case A_BR_RIGHT:
                    if(m_canvas && mx>left+10 && mx+m_dx<=pw) {
                        QPainter p(m_canvas);
                        p.setRasterOp(Qt::NotROP);
                        p.drawLine(m_oldMX, 0, m_oldMX, m_canvas->height());
                        p.drawLine(mx, 0, mx, m_canvas->height());
                        p.end();
                        m_layout.borders.right=static_cast<double>(pw-(mx+m_dx))*m_1_zoomedRes;
                        m_oldMX=mx;
                        m_oldMY=my;
                        repaint(false);
                    }
                    else
                        return;
                    break;
                default:
                    break;
            }
        }
    }
    else {  // vertical
        if(!m_mousePressed) {
            setCursor(Qt::arrowCursor);
            m_action=A_NONE;
            if(my>top-5 && my<top+5) {
                setCursor(Qt::sizeVerCursor);
                m_action=A_BR_TOP;
            } else if(my>bottom-5 && my<bottom+5) {
                setCursor(Qt::sizeVerCursor);
                m_action=A_BR_BOTTOM;
            }
        }
        else {
            switch(m_action) {
                case A_BR_TOP:
                    if(m_canvas && my<bottom-20) {
                        QPainter p(m_canvas);
                        p.setRasterOp(Qt::NotROP);
                        p.drawLine(0, m_oldMY, m_canvas->width(), m_oldMY);
                        p.drawLine(0, my, m_canvas->width(), my);
                        p.end();
                        m_layout.borders.top=static_cast<double>(my+m_dy)*m_1_zoomedRes;
                        m_oldMX=mx;
                        m_oldMY=my;
                        repaint(false);
                    }
                    else
                        return;
                    break;
                case A_BR_BOTTOM:
                    if(m_canvas && my>top+20 && my+m_dy<ph-2) {
                        QPainter p(m_canvas);
                        p.setRasterOp(Qt::NotROP);
                        p.drawLine(0, m_oldMY, m_canvas->width(), m_oldMY);
                        p.drawLine(0, my, m_canvas->width(), my);
                        p.end();
                        m_layout.borders.bottom=static_cast<double>(ph-(my+m_dy))*m_1_zoomedRes;
                        m_oldMX=mx;
                        m_oldMY=my;
                        repaint(false);
                    }
                    else
                        return;
                    break;
                default:
                    break;
            }
        }
    }
    m_oldMX=mx;
    m_oldMY=my;
}

void Ruler::mouseDoubleClickEvent(QMouseEvent *) {
    if(m_editable)
        emit openPageLayoutDia();
}

void Ruler::resizeEvent(QResizeEvent *e) {
    QFrame::resizeEvent(e);
    m_buffer.resize(size());
}

#include <ruler.moc>
