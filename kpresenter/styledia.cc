/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.0.1                                                 */
/* Author: Reginald Stadlbauer                                    */
/* E-Mail: reggie@kde.org                                         */
/* Homepage: http://boch35.kfunigraz.ac.at/~rs                    */
/* needs c++ library Qt (http://www.troll.no)                     */
/* needs mico (http://diamant.vsb.cs.uni-frankfurt.de/~mico/)     */
/* needs OpenParts and Kom (weis@kde.org)                         */
/* written for KDE (http://www.kde.org)                           */
/* License: GNU GPL                                               */
/******************************************************************/
/* Module: Pen and Brush style Dialog                             */
/******************************************************************/

#include "styledia.h"
#include "styledia.moc"
#include <kapp.h>

/******************************************************************/
/* class Pen and Brush preview                                    */
/******************************************************************/

/*====================== constructor =============================*/
PBPreview::PBPreview(QWidget* parent=0,const char* name=0,int _paintType=0)
  : QWidget(parent,name)
{
  paintType = _paintType;
  pen = QPen(black,1,SolidLine);
  brush = QBrush(white,SolidPattern);
}

/*================== paint event =================================*/
void PBPreview::paintEvent(QPaintEvent*)
{
  QPainter painter;
    
  painter.begin(this);
  if (paintType == 0)
    {
      QSize diff1(0,0),diff2(0,0);
      int _w = pen.width();

      if (lineBegin != L_NORMAL)
	diff1 = getBoundingSize(lineBegin,_w);

      if (lineEnd != L_NORMAL)
	diff2 = getBoundingSize(lineEnd,_w);

      if (lineBegin != L_NORMAL)
	drawFigure(lineBegin,&painter,QPoint(diff1.width() / 2,height() / 2),pen.color(),_w,180.0);

      if (lineEnd != L_NORMAL)
	drawFigure(lineEnd,&painter,QPoint(width() - diff2.width() / 2,height() / 2),pen.color(),_w,0.0);

      painter.setPen(pen);
      painter.drawLine(diff1.width() / 2,height()/2,width() - diff2.width() / 2,height()/2);
   }
  else
    painter.fillRect(0,0,width(),height(),brush);

  painter.end();
}

/******************************************************************/
/* class StyleDia                                                 */
/******************************************************************/

/*==================== constructor ===============================*/
StyleDia::StyleDia(QWidget* parent=0,const char* name=0)
  : QDialog(parent,name,true)
{
  int butW,butH;

  penFrame = new QButtonGroup(this,"penGrp");
  penFrame->setFrameStyle(QFrame::Box|QFrame::Sunken);
  penFrame->move(20,20);
  penFrame->setTitle(i18n("Pen"));

  choosePCol = new QPushButton(penFrame,"PCol");
  choosePCol->setText(i18n("Choose color..."));
  choosePCol->move(10,20);
  choosePCol->resize(choosePCol->sizeHint());
  connect(choosePCol,SIGNAL(clicked()),this,SLOT(changePCol()));

  penStyle = new QLabel(penFrame);
  penStyle->setText(i18n("Choose style:"));
  penStyle->move(choosePCol->x(),choosePCol->y()+choosePCol->height()+20);
  penStyle->resize(penStyle->sizeHint());

  choosePStyle = new QComboBox(false,penFrame,"PStyle");
  choosePStyle->move(choosePCol->x(),penStyle->y()+penStyle->height()+10);
  choosePStyle->insertItem(i18n("solid line"));
  choosePStyle->insertItem(i18n("dash line (----)"));
  choosePStyle->insertItem(i18n("dot line (****)"));
  choosePStyle->insertItem(i18n("dash dot line (-*-*)"));
  choosePStyle->insertItem(i18n("dash dot dot line (-**-)"));
  choosePStyle->insertItem(i18n("no pen"));
  choosePStyle->resize(choosePStyle->sizeHint());
  choosePCol->resize(choosePStyle->width(),choosePCol->height());  
  connect(choosePStyle,SIGNAL(activated(int)),this,SLOT(changePStyle(int)));

  penWidth = new QLabel(penFrame);
  penWidth->setText(i18n("Choose width:"));
  penWidth->move(choosePCol->x(),choosePStyle->y()+choosePStyle->height()+20);
  penWidth->resize(penWidth->sizeHint());

  choosePWidth = new QComboBox(false,penFrame,"PWidth");
  choosePWidth->move(choosePCol->x(),penWidth->y()+penWidth->height()+10);
  choosePWidth->resize(choosePStyle->width(),choosePWidth->sizeHint().height());
  choosePWidth->insertItem("1");
  choosePWidth->insertItem("2");
  choosePWidth->insertItem("3");
  choosePWidth->insertItem("4");
  choosePWidth->insertItem("5");
  choosePWidth->insertItem("6");
  choosePWidth->insertItem("7");
  choosePWidth->insertItem("8");
  choosePWidth->insertItem("9");
  choosePWidth->insertItem("10");
  connect(choosePWidth,SIGNAL(activated(int)),this,SLOT(changePWidth(int)));

  pen = QPen(black,1,SolidLine);
  
  llineBegin = new QLabel(penFrame);
  llineBegin->setText(i18n("Choose line begin:"));
  llineBegin->move(choosePCol->x(),choosePWidth->y()+choosePWidth->height()+20);
  llineBegin->resize(llineBegin->sizeHint());

  clineBegin = new QComboBox(false,penFrame,"lineBegin");
  clineBegin->move(choosePCol->x(),llineBegin->y()+llineBegin->height()+10);
  clineBegin->resize(choosePStyle->width(),clineBegin->sizeHint().height());
  clineBegin->insertItem("Normal");
  clineBegin->insertItem("Arrow");
  clineBegin->insertItem("Square");
  clineBegin->insertItem("Circle");
  connect(clineBegin,SIGNAL(activated(int)),this,SLOT(changeLineBegin(int)));

  llineEnd = new QLabel(penFrame);
  llineEnd->setText(i18n("Choose line end:"));
  llineEnd->move(choosePCol->x(),clineBegin->y()+clineBegin->height()+20);
  llineEnd->resize(llineEnd->sizeHint());

  clineEnd = new QComboBox(false,penFrame,"lineEnd");
  clineEnd->move(choosePCol->x(),llineEnd->y()+llineEnd->height()+10);
  clineEnd->resize(choosePStyle->width(),clineEnd->sizeHint().height());
  clineEnd->insertItem("Normal");
  clineEnd->insertItem("Arrow");
  clineEnd->insertItem("Square");
  clineEnd->insertItem("Circle");
  connect(clineEnd,SIGNAL(activated(int)),this,SLOT(changeLineEnd(int)));

  penPrev = new PBPreview(penFrame,"penPrev",0);
  penPrev->move(choosePCol->x(),clineEnd->y()+clineEnd->height()+20);
  penPrev->resize(choosePCol->width(),25);
  penPrev->setPen(pen);

  penFrame->resize(choosePCol->x()*2+choosePCol->width(),penPrev->y()+penPrev->height()+10);

  brushFrame = new QButtonGroup(this,"brushGrp");
  brushFrame->setFrameStyle(QFrame::Box|QFrame::Sunken);
  brushFrame->move(penFrame->x()+penFrame->width()+20,20);
  brushFrame->setTitle(i18n("Brush"));

  chooseBCol = new QPushButton(brushFrame,"BCol");
  chooseBCol->setText(i18n("Choose color..."));
  chooseBCol->move(10,20);
  chooseBCol->resize(chooseBCol->sizeHint());
  connect(chooseBCol,SIGNAL(clicked()),this,SLOT(changeBCol()));

  brushStyle = new QLabel(brushFrame);
  brushStyle->setText(i18n("Choose style:"));
  brushStyle->move(chooseBCol->x(),chooseBCol->y()+chooseBCol->height()+20);
  brushStyle->resize(brushStyle->sizeHint());

  chooseBStyle = new QComboBox(false,brushFrame,"BStyle");
  chooseBStyle->move(brushStyle->x(),brushStyle->y()+brushStyle->height()+10);
  chooseBStyle->insertItem(i18n("100% fill pattern"));
  chooseBStyle->insertItem(i18n("94% fill pattern"));
  chooseBStyle->insertItem(i18n("88% fill pattern"));
  chooseBStyle->insertItem(i18n("63% fill pattern"));
  chooseBStyle->insertItem(i18n("50% fill pattern"));
  chooseBStyle->insertItem(i18n("37% fill pattern"));
  chooseBStyle->insertItem(i18n("12% fill pattern"));
  chooseBStyle->insertItem(i18n("6% fill pattern"));
  chooseBStyle->insertItem(i18n("horizontal lines"));
  chooseBStyle->insertItem(i18n("vertical lines"));
  chooseBStyle->insertItem(i18n("crossing lines"));
  chooseBStyle->insertItem(i18n("diagonal lines (/)"));
  chooseBStyle->insertItem(i18n("diagonal lines (\\)"));
  chooseBStyle->insertItem(i18n("diagonal crossing lines"));
  chooseBStyle->insertItem(i18n("no brush"));
  chooseBStyle->resize(chooseBStyle->sizeHint());
  chooseBCol->resize(chooseBStyle->width(),chooseBCol->height());
  connect(chooseBStyle,SIGNAL(activated(int)),this,SLOT(changeBStyle(int)));

  brush = QBrush(white,SolidPattern);

  brushPrev = new PBPreview(brushFrame,"brushPrev",1);
  brushPrev->move(brushStyle->x(),chooseBStyle->y()+chooseBStyle->height()+20);
  brushPrev->resize(chooseBStyle->width(),25);
  brushPrev->setBrush(brush);

  brushFrame->resize(2*chooseBStyle->x()+chooseBStyle->width(),penFrame->height());

  cancelBut = new QPushButton(this,"BCancel");
  cancelBut->setText(i18n("Cancel"));
  cancelBut->move(brushFrame->x()+brushFrame->width(),brushFrame->y()+brushFrame->height()+20);
 
  applyBut = new QPushButton(this,"BApply");
  applyBut->setText(i18n("Apply"));

  okBut = new QPushButton(this,"BOK");
  okBut->setText(i18n("OK"));
  okBut->setAutoRepeat(false);
  okBut->setAutoResize(false);
  okBut->setAutoDefault(true);
  okBut->setDefault(true);

  butW = max(cancelBut->sizeHint().width(),
	     max(applyBut->sizeHint().width(),okBut->sizeHint().width()));
  butH = cancelBut->sizeHint().height();

  cancelBut->resize(butW,butH);
  applyBut->resize(butW,butH);
  okBut->resize(butW,butH);

  cancelBut->move(brushFrame->x()+brushFrame->width()-cancelBut->width(),
		  brushFrame->y()+brushFrame->height()+20);
  applyBut->move(brushFrame->x()+brushFrame->width()-cancelBut->width()-5-applyBut->width(),
		  brushFrame->y()+brushFrame->height()+20);
  okBut->move(brushFrame->x()+brushFrame->width()-cancelBut->width()-5-applyBut->width()-10-okBut->width(),
		  brushFrame->y()+brushFrame->height()+20);

  resize(brushFrame->x()+brushFrame->width()+20,okBut->y()+okBut->height()+10);

  connect(okBut,SIGNAL(clicked()),this,SLOT(styleDone()));
  connect(applyBut,SIGNAL(clicked()),this,SLOT(styleDone()));
  connect(cancelBut,SIGNAL(clicked()),this,SLOT(reject()));
  connect(okBut,SIGNAL(clicked()),this,SLOT(accept()));
}

/*===================== destructor ===============================*/
StyleDia::~StyleDia()
{
}

/*=========================== set pen =============================*/
void StyleDia::setPen(QPen _pen)
{
  pen = _pen;
  penPrev->setPen(pen);
  switch (pen.style())
    {
    case NoPen: choosePStyle->setCurrentItem(5); break; 
    case SolidLine: choosePStyle->setCurrentItem(0); break; 
    case DashLine: choosePStyle->setCurrentItem(1); break; 
    case DotLine: choosePStyle->setCurrentItem(2); break;  
    case DashDotLine: choosePStyle->setCurrentItem(3); break;  
    case DashDotDotLine: choosePStyle->setCurrentItem(4); break;  
    }
  choosePWidth->setCurrentItem(pen.width()-1);
}

/*========================= set brush =============================*/
void StyleDia::setBrush(QBrush _brush)
{
  brush = _brush;
  brushPrev->setBrush(brush);
  switch (brush.style())
    {
    case SolidPattern: chooseBStyle->setCurrentItem(0); break;
    case Dense1Pattern: chooseBStyle->setCurrentItem(1); break;
    case Dense2Pattern: chooseBStyle->setCurrentItem(2); break;
    case Dense3Pattern: chooseBStyle->setCurrentItem(3); break;
    case Dense4Pattern: chooseBStyle->setCurrentItem(4); break;
    case Dense5Pattern: chooseBStyle->setCurrentItem(5); break;
    case Dense6Pattern: chooseBStyle->setCurrentItem(6); break;
    case Dense7Pattern: chooseBStyle->setCurrentItem(7); break;
    case HorPattern: chooseBStyle->setCurrentItem(8); break;
    case VerPattern: chooseBStyle->setCurrentItem(9); break;
    case CrossPattern: chooseBStyle->setCurrentItem(10); break; 
    case BDiagPattern: chooseBStyle->setCurrentItem(11); break; 
    case FDiagPattern: chooseBStyle->setCurrentItem(12); break;
    case DiagCrossPattern: chooseBStyle->setCurrentItem(13); break;
    case NoBrush: chooseBStyle->setCurrentItem(14); break;
    case CustomPattern: break;
    }
}

/*======================== set line beginning =====================*/
void StyleDia::setLineBegin(LineEnd lb)
{
  lineBegin = lb;
  penPrev->setLineBegin(lineBegin);
  clineBegin->setCurrentItem(static_cast<int>(lineBegin));
}

/*======================== set line end ===========================*/
void StyleDia::setLineEnd(LineEnd le)
{
  lineEnd = le;
  penPrev->setLineEnd(lineEnd);
  clineEnd->setCurrentItem(static_cast<int>(lineEnd));
}

/*====================== change pen-color =========================*/
void StyleDia::changePCol()
{
  QColor currColor;
  
  currColor = pen.color();
  if (KColorDialog::getColor(currColor))
    {
      pen.setColor(currColor);
      penPrev->setPen(pen);
    }
}

/*==================== change brush-color =========================*/
void StyleDia::changeBCol()
{
  QColor currColor;
  
  currColor = brush.color();
  if (KColorDialog::getColor(currColor))
    {
      brush.setColor(currColor);
      brushPrev->setBrush(brush);
    }
}

/*====================== change pen-style =========================*/
void StyleDia::changePStyle(int item)
{
  switch (item)
    {
    case 5: pen.setStyle(NoPen); break; 
    case 0: pen.setStyle(SolidLine); break; 
    case 1: pen.setStyle(DashLine); break; 
    case 2: pen.setStyle(DotLine); break;  
    case 3: pen.setStyle(DashDotLine); break;  
    case 4: pen.setStyle(DashDotDotLine); break;  
    }
  penPrev->setPen(pen);
}

/*====================== change brush-style =======================*/
void StyleDia::changeBStyle(int item)
{
  switch (item)
    {
    case 0: brush.setStyle(SolidPattern); break;
    case 1: brush.setStyle(Dense1Pattern); break;
    case 2: brush.setStyle(Dense2Pattern); break;
    case 3: brush.setStyle(Dense3Pattern); break;
    case 4: brush.setStyle(Dense4Pattern); break;
    case 5: brush.setStyle(Dense5Pattern); break;
    case 6: brush.setStyle(Dense6Pattern); break;
    case 7: brush.setStyle(Dense7Pattern); break;
    case 8: brush.setStyle(HorPattern); break;
    case 9: brush.setStyle(VerPattern); break;
    case 10: brush.setStyle(CrossPattern); break; 
    case 11: brush.setStyle(BDiagPattern); break; 
    case 12: brush.setStyle(FDiagPattern); break;
    case 13: brush.setStyle(DiagCrossPattern); break;
    case 14: brush.setStyle(NoBrush); break;
    }
  brushPrev->setBrush(brush);
}

/*====================== change pen-width =========================*/
void StyleDia::changePWidth(int item)
{
  pen.setWidth(item+1);
  penPrev->setPen(pen);
}

/*====================== change line beginning ====================*/
void StyleDia::changeLineBegin(int item)
{
  lineBegin = (LineEnd)item;
  penPrev->setLineBegin(lineBegin);
}

/*====================== change line end ==========================*/
void StyleDia::changeLineEnd(int item)
{
  lineEnd = (LineEnd)item;
  penPrev->setLineEnd(lineEnd);
}

