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
class KWPage;
class KoPageLayout;
class KWPageStyle;
enum Selection{NONE,MTOP,MBOTTOM,MLEFT,MRIGHT,HEADER,FOOTER,BLEFT,BRIGHT,BTOP,BBOTTOM};

class KWPageTool : public KoToolBase
{
    Q_OBJECT
public:
    explicit KWPageTool(KoCanvasBase *canvas);
    virtual ~KWPageTool();
    bool wantsAutoScroll() const;
    void setStyleFromWidget(QString style);
    void applyStyle(int page, QString style);
    void applyStyle(int page, KWPageStyle style);
public:
    virtual void paint(QPainter &painter, const KoViewConverter &converter);
    //Get the page under the mouse
    KWPage pageUnderMouse();
public slots:
    virtual void activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes);

public: // Events

    virtual void mousePressEvent(KoPointerEvent *event);
    virtual void mouseMoveEvent(KoPointerEvent *event);
    virtual void mouseReleaseEvent(KoPointerEvent *event);

    virtual void mouseDoubleClickEvent(KoPointerEvent *event);

//  virtual void keyPressEvent(QKeyEvent *event);
public slots:
    void enableHeader();
    void enableFooter();

private slots:
    ///Force the remaining content on the page to next page.
    void insertPageBreak();
    void resizePage();

private:
    KWCanvas *getCanvas() const;
    KWDocument *getDocument() const;
    //Return or set the position x or y of the margin
    int marginInPx(Selection selection);
    //Set position of margin in px
    void setMarginInPx(Selection selection, int postionX, int positionY);
    //Get the position of the mouse
    int xMouseInPage();
    int yMouseInPage();
    //Get the y position of the mouse in the document
    qreal yMouseInDocument();
    //Get the distance to the beginning of the document over the page in parameter
    int distanceOverPage(int pageNumber);
    //Change the layout of a page style
    void changeLayoutInStyle(KoPageLayout layout, KWPageStyle style);

protected:
    QList<QWidget *> createOptionWidgets();
    void refreshCanvas();

private:
    Selection m_selection;

    //Style from SimplePagesStyles
    QString m_styleFomWiget;
    int m_numberPageClicked;
    KWCanvas *m_canvas;
    KWDocument *m_document;
    QPoint *m_mousePosTmp;
    //Need to control the page resizing when cursor is out of canvas
    QTimer *m_resizeTimer;

    KAction *m_actionViewHeader;
    KAction *m_actionViewFooter;
};

#endif

