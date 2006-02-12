/* This file is part of the KDE project

   Copyright (C) 2002 Joseph Wenninger <jowenn@kde.org>

   Large parts are taken from kdebase/konqueror/konq_actions.*
   Copyright (C) 2000 Simon Hausmann <hausmann@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KWMailMergeLabelAction.h"
#include "KWMailMergeLabelAction.moc"
#include <KoMainWindow.h>
#include <kstyle.h>
#include <kpopupmenu.h>

class MailMergeDraggableLabel : public QToolButton
{
public:
    MailMergeDraggableLabel( KoMainWindow * mw, const QString & text, QWidget * parent = 0, const char * name = 0 )
        : QToolButton( parent, name ), m_mw(mw)
    {
        setText(text);
        setAcceptDrops(true);
        validDrag = false;
    }
protected:
    void mousePressEvent( QMouseEvent * ev )
    {
        validDrag = true;
        startDragPos = ev->pos();
    }
    void mouseMoveEvent( QMouseEvent * ev )
    {
        if ((startDragPos - ev->pos()).manhattanLength() > QApplication::startDragDistance())
        {
            validDrag = false;

#if 0             // what was the goal here?
        KWTextDrag *drag=new KWTextDrag(m_mw);
//      drag->setKWord(" <!DOCTYPE PARAGRAPHS> <PARAGRAPHS> <PARAGRAPH>  <TEXT>fsfsd</TEXT>  <LAYOUT>   <NAME value=\"Standard\" />   <FLOW align=\"left\" />   <FORMAT id=\"1\" pos=\"0\" len=\"5\" >    <WEIGHT value=\"50\" />    <FONT name=\"helvetica\" />    <SIZE value=\"12\" />    <ITALIC value=\"0\" />    <UNDERLINE value=\"0\" />    <STRIKEOUT value=\"0\" />    <VERTALIGN value=\"0\" />   </FORMAT>  </LAYOUT> </PARAGRAPH></PARAGRAPHS>");
        drag->setKWord(" <!DOCTYPE PARAGRAPHS> <PARAGRAPHS> <PARAGRAPH>  <TEXT>fsfsd</TEXT> </PARAGRAPH> </PARAGRAPHS>");
//      drag->setKWord("<TEXT>fsfsd</TEXT>");

        drag->dragCopy();
#endif
        }
    }
    void mouseReleaseEvent( QMouseEvent * )
    {
        validDrag = false;
    }
    QSize sizeHint() const
    {
        int w = fontMetrics().width( text() );
        int h = fontMetrics().height();
        return QSize( w, h );
    }
    void drawButton( QPainter * p )
    {
        // Draw the background
        style().drawComplexControl( QStyle::CC_ToolButton, p, this, rect(), colorGroup(),
                                    QStyle::Style_Enabled, QStyle::SC_ToolButton );
        // Draw the label
        style().drawControl( QStyle::CE_ToolButtonLabel, p, this, rect(), colorGroup(),
                             QStyle::Style_Enabled );
    }
    void enterEvent( QEvent* ) {};
    void leaveEvent( QEvent* ) {};
#if 0
    void dragEnterEvent( QDragEnterEvent *ev ) {
        if ( KURLDrag::canDecode( ev ) )
            ev->acceptAction();
    }
    void dropEvent( QDropEvent* ev ) {
        KURL::List lst;
        if ( KURLDrag::decode( ev, lst ) ) {
            m_mw->openURL( 0L, lst.first() );
        }
    }
#endif
private:
    QPoint startDragPos;
    bool validDrag;
    KoMainWindow * m_mw;
};





KWMailMergeLabelAction::KWMailMergeLabelAction( const QString &text, int accel,
                    QObject* receiver, const char* slot, QObject *parent, const char *name )
    : KAction( text, accel, receiver, slot, parent, name ), m_label( 0L )
{
}

int KWMailMergeLabelAction::plug( QWidget *widget, int index )
{
  //do not call the previous implementation here

  if ( widget->inherits( "KToolBar" ) )
  {
    KToolBar *tb = (KToolBar *)widget;

    int id = KAction::getToolButtonID();

    m_label = new MailMergeDraggableLabel( static_cast<KoMainWindow*>(tb->mainWindow()), text(), widget );
    tb->insertWidget( id, m_label->width(), m_label, index );

    addContainer( tb, id );

    connect( tb, SIGNAL( destroyed() ), this, SLOT( slotDestroyed() ) );

    return containerCount() - 1;
  }

  return -1;
}

void KWMailMergeLabelAction::unplug( QWidget *widget )
{
  if ( widget->inherits( "KToolBar" ) )
  {
    KToolBar *bar = (KToolBar *)widget;

    int idx = findContainer( bar );

    if ( idx != -1 )
    {
      bar->removeItem( itemId( idx ) );
      removeContainer( idx );
    }

    m_label = 0;
    return;
  }
}

