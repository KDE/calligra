/******************************************************************/
/* KCharSelect - (c) by Reginald Stadlbauer 1998                  */
/* Version: 0.0.4                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* needs c++ library Qt (http://www.troll.no)                     */
/* written for KDE (http://www.kde.org)                           */
/* KCharSelect is under GNU GPL                                   */
/******************************************************************/
/* Module: Character Selector (header)                            */
/******************************************************************/

#include "kcharselect.h"
#include "kcharselect.moc"

/******************************************************************/
/* Class: KCharSelect                                             */
/******************************************************************/

/*===================== constructor ==============================*/
KCharSelect::KCharSelect(QWidget *parent,const char *name,QFont __font,int __c)
  : QTableView(parent,name)
{
  _font = QFont(__font.family());
  _c = __c;

  currentC = KPoint(_c - 33 - ((_c - 33) / 28) * 28,(_c - 33) / 28);

  setBackgroundColor(colorGroup().base());

  setCellWidth(20);
  setCellHeight(25);

  setNumCols(28);
  setNumRows(8);
}

/*================================================================*/
void KCharSelect::setChar(int __c)
{
  _c = __c;
  currentC = KPoint(_c - 33 - ((_c - 33) / 28) * 28,(_c - 33) / 28);
  repaint(true);
}

/*======================= get size hint ==========================*/
KSize KCharSelect::sizeHint()
{
  int w = cellWidth();
  int h = cellHeight();

  w *= numCols();
  h *= numRows();

  return KSize(w,h);
}

/*====================== paint cell ==============================*/
void KCharSelect::paintCell(class QPainter* p,int row,int col)
{
  int w = cellWidth(col);
  int h = cellHeight(row);
  int x2 = w - 1;
  int y2 = h - 1;
  char chr[5];
  int c;

  c = 33 + row * numCols() + col;
  sprintf(chr,"%c",c);

  p->setPen(colorGroup().text());
  if (currentC.x() == col && currentC.y() == row)
    {
      p->setBrush(QBrush(colorGroup().highlight()));
      p->setPen(NoPen);
      p->drawRect(0,0,w,h);
      p->setPen(colorGroup().highlightedText());
      _c = c;
    }

  p->setFont(_font);
  p->drawText(0,0,x2,y2,AlignHCenter | AlignVCenter,chr);

  p->setPen(black);

  p->drawLine(x2,0,x2,y2);
  p->drawLine(0,y2,x2,y2);

  if (row == 0)
    p->drawLine(0,0,x2,0);
  if (col == 0)
    p->drawLine(0,0,0,y2);

}

/*======================== mouse move event =======================*/
void KCharSelect::mouseMoveEvent(QMouseEvent *e)
{
  if (findRow(e->y()) != -1 && findCol(e->x()) != -1)
    {
      KPoint oldC = currentC;

      currentC.setX(findCol(e->x()));
      currentC.setY(findRow(e->y()));

      updateCell(oldC.y(),oldC.x(),true);
      updateCell(currentC.y(),currentC.x(),true);
      emit activated(_c);
    }
}







