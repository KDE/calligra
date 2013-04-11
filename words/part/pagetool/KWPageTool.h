/*
 *  Copyright (C) 2011 Jignesh Kakadiya <jigneshhk1992@gmail.com>
 *  
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
*/

#ifndef KWPAGETOOL_H
#define KWPAGETOOL_H

#include <KoToolBase.h>

//Size of the zone where we can select margin or page border in document px
#define SELECT_SPACE 10

class QTimer;
class KWCanvas;
class KWDocument;

class KWPageTool : public KoToolBase
{
    Q_OBJECT
public:
    explicit KWPageTool(KoCanvasBase *canvas);
    virtual ~KWPageTool();
    bool wantsAutoScroll() const;
public:
    virtual void paint(QPainter &painter, const KoViewConverter &converter);

public slots:
    virtual void activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes);

public: // Events

    virtual void mousePressEvent(KoPointerEvent *event);
    virtual void mouseMoveEvent(KoPointerEvent *event);
    virtual void mouseReleaseEvent(KoPointerEvent *event);

//  virtual void mouseDoubleClickEvent(KoPointerEvent *event);

//  virtual void keyPressEvent(QKeyEvent *event);

private slots:
    ///Force the remaining content on the page to next page.
    void insertPageBreak();
    void resizePage();
private:
    KWCanvas *getCanvas() const;
    KWDocument *getDocument() const;

protected:
    QList<QWidget *> createOptionWidgets();

private:
    enum Selection{NONE,MTOP,MBOTTOM,MLEFT,MRIGHT,HEADER,FOOTER,BLEFT,BRIGHT,BTOP,BBOTTOM};
    Selection m_selection;

    Margin margin;
    KWCanvas *m_canvas;
    KWDocument *m_document;
    QPoint *m_mousePosTmp;
    //Need to control the page resizing when cursor is out of canvas
    QTimer *m_resizeTimer;
    //Return or set the position x or y of the margin
    int marginInPx(Selection selection);
    void setMarginInPx(Selection selection,int postionX,int positionY);
    int xMouseInPage(int positionX);
    int yMouseInPage(int positionY);

    void createHeader();
    void createFooter();
    //To check if header/footer is already created
    bool header, footer;

};

#endif

