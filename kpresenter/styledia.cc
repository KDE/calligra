/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Version: 0.1.0                                                 */
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
#include <klocale.h>

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
  gradient = 0;
}

/*================== paint event =================================*/
void PBPreview::paintEvent(QPaintEvent*)
{
  QPainter painter;

  painter.begin(this);
  if (paintType == 0)
    {
      KSize diff1(0,0),diff2(0,0);
      int _w = pen.width();

      if (lineBegin != L_NORMAL)
	diff1 = getBoundingSize(lineBegin,_w);

      if (lineEnd != L_NORMAL)
	diff2 = getBoundingSize(lineEnd,_w);

      if (lineBegin != L_NORMAL)
	drawFigure(lineBegin,&painter,KPoint(diff1.width() / 2,height() / 2),pen.color(),_w,180.0);

      if (lineEnd != L_NORMAL)
	drawFigure(lineEnd,&painter,KPoint(width() - diff2.width() / 2,height() / 2),pen.color(),_w,0.0);

      painter.setPen(pen);
      painter.drawLine(diff1.width() / 2,height()/2,width() - diff2.width() / 2,height()/2);
   }
  else if (paintType == 1)
    {
      painter.fillRect(0,0,width(),height(),white);
      painter.fillRect(2,2,width() - 4,height() - 4,brush);
    }
  else if (paintType == 2 && gradient)
    {
      gradient->setSize(size());
      painter.drawPixmap(0,0,*gradient->getGradient());
    }

  painter.end();
}

/******************************************************************/
/* class StyleDia                                                 */
/******************************************************************/

/*==================== constructor ===============================*/
StyleDia::StyleDia(QWidget* parent = 0,const char* name = 0,int flags = SD_PEN | SD_BRUSH)
  : QTabDialog(parent,name,true)
{
  penFrame = new QWidget(this,"penGrp");

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

  choosePWidth = new QSpinBox(1,10,1,penFrame);
  choosePWidth->move(choosePCol->x(),penWidth->y()+penWidth->height()+10);
  choosePWidth->resize(choosePStyle->width(),choosePWidth->sizeHint().height());
  connect(choosePWidth,SIGNAL(valueChanged(int)),this,SLOT(changePWidth(int)));

  pen = QPen(black,1,SolidLine);

  llineBegin = new QLabel(penFrame);
  llineBegin->setText(i18n("Choose line begin:"));
  llineBegin->move(choosePCol->x() + choosePCol->width() + 10,penStyle->y());
  llineBegin->resize(llineBegin->sizeHint());

  clineBegin = new QComboBox(false,penFrame,"lineBegin");
  clineBegin->move(llineBegin->x(),llineBegin->y() + llineBegin->height() + 10);
  clineBegin->resize(choosePStyle->width(),clineBegin->sizeHint().height());
  clineBegin->insertItem("Normal");
  clineBegin->insertItem("Arrow");
  clineBegin->insertItem("Square");
  clineBegin->insertItem("Circle");
  connect(clineBegin,SIGNAL(activated(int)),this,SLOT(changeLineBegin(int)));

  llineEnd = new QLabel(penFrame);
  llineEnd->setText(i18n("Choose line end:"));
  llineEnd->move(llineBegin->x(),clineBegin->y() + clineBegin->height() + 20);
  llineEnd->resize(llineEnd->sizeHint());

  clineEnd = new QComboBox(false,penFrame,"lineEnd");
  clineEnd->move(llineBegin->x(),llineEnd->y()+llineEnd->height()+10);
  clineEnd->resize(choosePStyle->width(),clineEnd->sizeHint().height());
  clineEnd->insertItem("Normal");
  clineEnd->insertItem("Arrow");
  clineEnd->insertItem("Square");
  clineEnd->insertItem("Circle");
  connect(clineEnd,SIGNAL(activated(int)),this,SLOT(changeLineEnd(int)));

  penPrev = new PBPreview(penFrame,"penPrev",0);
  penPrev->move(choosePCol->x(),clineEnd->y()+clineEnd->height()+20);
  penPrev->resize(choosePCol->width() + clineEnd->width() + 10,25);
  penPrev->setPen(pen);

  choosePCol->resize(penPrev->width(),choosePCol->height());

  if (flags & SD_PEN)
    addTab(penFrame,i18n("Pen"));
  else
    penFrame->hide();

  brushFrame = new QWidget(this,"brushGrp");
  QButtonGroup *tmp = new QButtonGroup(brushFrame);
  tmp->hide();
  tmp->setExclusive(true);

  fillStyle = new QRadioButton(i18n("Fill with brush:"),brushFrame,"");
  fillStyle->resize(fillStyle->sizeHint());
  fillStyle->move(10,20);
  tmp->insert(fillStyle);
  connect(fillStyle,SIGNAL(clicked()),this,SLOT(rBrush()));

  chooseBCol = new QPushButton(brushFrame,"BCol");
  chooseBCol->setText(i18n("Choose color..."));
  chooseBCol->move(fillStyle->x(),fillStyle->y() + fillStyle->height() + 20);
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

  gradient1 = new KColorButton(red,brushFrame);
  gradient1->resize(chooseBCol->size());

  fillGradient = new QRadioButton(i18n("Fill with gradient:"),brushFrame,"");
  fillGradient->resize(fillGradient->sizeHint());
  fillGradient->move(penPrev->x() + penPrev->width() - gradient1->width(),fillStyle->y());
  tmp->insert(fillGradient);
  connect(fillGradient,SIGNAL(clicked()),this,SLOT(rGradient()));

  gColors = new QLabel(brushFrame);
  gColors->setText(i18n("Choose gradient colors:"));
  gColors->move(fillGradient->x(),fillGradient->y() + fillGradient->height() + 20);
  gColors->resize(gColors->sizeHint());

  gradient1->move(gColors->x(),gColors->y() + gColors->height() + 10);
  connect(gradient1,SIGNAL(changed(const QColor &)),this,SLOT(gColor1(const QColor &)));

  gradient2 = new KColorButton(green,brushFrame);
  gradient2->resize(chooseBCol->size());
  gradient2->move(gradient1->x(),gradient1->y() + gradient1->height() + 10);
  connect(gradient2,SIGNAL(changed(const QColor &)),this,SLOT(gColor2(const QColor &)));

  gStyle = new QLabel(brushFrame);
  gStyle->setText(i18n("Choose gradient style:"));
  gStyle->move(gradient2->x(),gradient2->y() + gradient2->height() + 20);
  gStyle->resize(gStyle->sizeHint());

  gradients = new QComboBox(false,brushFrame,"");
  gradients->move(gStyle->x(),gStyle->y() + gStyle->height() + 10);
  gradients->insertItem(i18n("Horizontal Gradient"),-1);
  gradients->insertItem(i18n("Vertical Gradient"),-1);
  gradients->insertItem(i18n("Diagonal Gradient 1"),-1);
  gradients->insertItem(i18n("Diagonal Gradient 2"),-1);
  gradients->insertItem(i18n("Circle Gradient"),-1);
  gradients->insertItem(i18n("Rectangle Gradient"),-1);
  gradients->resize(chooseBStyle->size());
  connect(gradients,SIGNAL(activated(int)),this,SLOT(gcStyle(int)));

  gradient = new KPGradient(red,green,BCT_GHORZ,KSize(chooseBCol->width(),25));

  gPrev = new PBPreview(brushFrame,"",2);
  gPrev->move(gradients->x(),gradients->y() + gradients->height() + 20);
  gPrev->resize(chooseBCol->width(),25);
  gPrev->setGradient(gradient);

  chooseBCol->move(chooseBCol->x(),gradient1->y());
  brushStyle->move(brushStyle->x(),gStyle->y());
  chooseBStyle->move(chooseBStyle->x(),gradients->y());
  brushPrev->move(brushPrev->x(),gPrev->y());

  if (flags & SD_BRUSH)
    addTab(brushFrame,i18n("Brush"));
  else
    brushFrame->hide();

  penFrame->setMinimumSize(penPrev->x() + penPrev->width() + 40,penPrev->y() + penPrev->height() + 40);
  penFrame->setMaximumSize(penPrev->x() + penPrev->width() + 40,penPrev->y() + penPrev->height() + 40);
  penFrame->resize(penPrev->x() + penPrev->width() + 40,penPrev->y() + penPrev->height() + 40);

  brushFrame->setMinimumSize(gPrev->x() + gPrev->width() + 40,gPrev->y() + gPrev->height() + 40);
  brushFrame->setMaximumSize(gPrev->x() + gPrev->width() + 40,gPrev->y() + gPrev->height() + 40);
  brushFrame->resize(gPrev->x() + gPrev->width() + 40,gPrev->y() + gPrev->height() + 40);

  resize(max(penFrame->size().width() + 30,brushFrame->size().width() + 30),
	 max(penFrame->size().height() + 60 + chooseBCol->height(),brushFrame->size().height() + 60 + chooseBCol->height()));

  setCancelButton(i18n("Cancel"));
  setOKButton(i18n("OK"));
  setApplyButton(i18n("Apply"));

  connect(this,SIGNAL(applyButtonPressed()),this,SLOT(styleDone()));
  connect(this,SIGNAL(cancelButtonPressed()),this,SLOT(reject()));
}

/*===================== destructor ===============================*/
StyleDia::~StyleDia()
{
  delete gradient;
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
  choosePWidth->setValue(pen.width());
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

/*================================================================*/
void StyleDia::setFillType(FillType ft)
{
  if (ft == FT_BRUSH)
    {
      fillStyle->setChecked(true);
      fillGradient->setChecked(false);
      rBrush();
    }
  else
    {
      fillStyle->setChecked(false);
      fillGradient->setChecked(true);
      rGradient();
    }
}

/*================================================================*/
void StyleDia::setGradient(QColor _c1,QColor _c2,BCType _t)
{
  gradient1->setColor(_c1);
  gradient2->setColor(_c2);
  gradients->setCurrentItem(static_cast<int>(_t - 1));

  gradient->setColor1(QColor(_c1));
  gradient->setColor2(QColor(_c2));
  gradient->setBackColorType(_t);
  gPrev->setGradient(gradient);
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
  pen.setWidth(item);
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

/*=================================================================*/
void StyleDia::gColor1(const QColor &newColor)
{
  gradient->setColor1(QColor(newColor));
  gPrev->setGradient(gradient);
}

/*=================================================================*/
void StyleDia::gColor2(const QColor &newColor)
{
  gradient->setColor2(QColor(newColor));
  gPrev->setGradient(gradient);
}

/*=================================================================*/
void StyleDia::gcStyle(int item)
{
  gradient->setBackColorType(static_cast<BCType>(item + 1));
  gPrev->setGradient(gradient);
}

/*=================================================================*/
void StyleDia::rBrush()
{
  gradients->setEnabled(false);
  gradient1->setEnabled(false);
  gradient2->setEnabled(false);

  chooseBCol->setEnabled(true);
  chooseBStyle->setEnabled(true);
}

/*=================================================================*/
void StyleDia::rGradient()
{
  gradients->setEnabled(true);
  gradient1->setEnabled(true);
  gradient2->setEnabled(true);

  chooseBCol->setEnabled(false);
  chooseBStyle->setEnabled(false);
}

