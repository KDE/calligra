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
#include <kmenu.h>

#include <QApplication>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QEvent>
#include <QDropEvent>
#include <QToolBar>

class MailMergeDraggableLabel : public QToolButton
{
public:
    MailMergeDraggableLabel( KoMainWindow * mw, const QString & text, QWidget * parent = 0 )
        : QToolButton( parent ), m_mw(mw)
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

#if 0 // TODO QT4 PORTING
    void drawButton( QPainter * p )
    {
        // Draw the background
        style().drawComplexControl( QStyle::CC_ToolButton, p, this, rect(), colorGroup(),
                                    QStyle::Style_Enabled, QStyle::SC_ToolButton );
        // Draw the label
        style().drawControl( QStyle::CE_ToolButtonLabel, p, this, rect(), colorGroup(),
                             QStyle::Style_Enabled );
    }
#endif
    void enterEvent( QEvent* ) {};
    void leaveEvent( QEvent* ) {};
#if 0
    void dragEnterEvent( QDragEnterEvent *ev ) {
        if ( m_mw && KUrl::List::canDecode( ev->mimeData() ) )
            ev->acceptAction();
    }
    void dropEvent( QDropEvent* ev ) {
        KUrl::List lst = KUrl::List::fromMimeData( ev->mimeData() );
        if ( m_mw && !lst.isEmpty() ) {
            m_mw->openURL( 0L, lst.first() );
        }
    }
#endif
private:
    QPoint startDragPos;
    bool validDrag;
    KoMainWindow * m_mw;
};





KWMailMergeLabelAction::KWMailMergeLabelAction( const QString &text, KActionCollection* parent, const QString& name )
    : KAction( text, parent, name )
{
}

QWidget* KWMailMergeLabelAction::createToolBarWidget(QToolBar* tb)
{
    KoMainWindow* mw = qobject_cast<KoMainWindow *>( tb->parentWidget() );
    return new MailMergeDraggableLabel( mw, text(), tb );
}
