
#include <qapplication.h>
#include <QWidget>
#include <QtCore/QTimer>
#include <stdlib.h>
#include "kled.h"
#include "kledtest.h"



KLedTest::KLedTest(QWidget* parent)
  : QWidget(parent),
    LedWidth(16),
    LedHeight(10),
    Grid(3),
    ledcolor(0),
    ledlook(KLed::Flat),
    kled_round(true) // Switch HERE between rectangle and circular leds
{
  if (kled_round) {
    //KLed l(KLed::red, &qw);				// create lamp
    //KLed l(KLed::blue, &qw);				// create lamp
    l = new KLed(Qt::green, this);				// create lamp
    //KLed l(KLed::yellow, &qw);				// create lamp
    //KLed l(KLed::orange, &qw);				// create lamp


    l->resize(16,30);
    //l.setLook(KLed::flat);
    l->setShape(KLed::Circular);
    //l->setShape(KLed::Rectangular);

    //l->setLook(KLed::Flat);
    //l->setLook(KLed::Flat);
    //l->setLook(KLed::Flat);

    l->move(5,5);
    //    ktmp tmpobj(l);

    t_toggle.setSingleShot(false);
    t_toggle.start(1000);
    t_color.setSingleShot(false);
    t_color.start(3500);
    t_look.setSingleShot(false);
    t_look.start(3500);
    QObject::connect(&t_toggle, SIGNAL(timeout()), l, SLOT(toggle()));
    QObject::connect(&t_color, SIGNAL(timeout()), this, SLOT(nextColor()));
    QObject::connect(&t_look, SIGNAL(timeout()), this, SLOT(nextLook()));
    l->show();
    resize(240,140);
  }
  else {
    y=Grid; index=0;
    for( int shape=0; (int)shape<2; shape=(KLed::Shape)(shape+1)) {
      x=Grid;
      for( int look=0; (int)look<3; look=(KLed::Look)(look+1)) {
	for(state=KLed::Off; (int)state<2; state=(KLed::State)(state+1))
	    {
	      leds[index]=new KLed(Qt::yellow, state,
				   (KLed::Look)(look+1),
				   (KLed::Shape)(shape+1), this);
	      leds[index]->setGeometry(x, y, LedWidth, LedHeight);
	      ++index;
	      x+=Grid+LedWidth;
	    }
	}
      y+=Grid+LedHeight;
      }
    setFixedSize(x+Grid, y+Grid);
    connect(&timer, SIGNAL(timeout()), SLOT(timeout()));
    timer.start(500);
  }
}


KLedTest::~KLedTest()
{
  if (kled_round) {
    delete l;
  }
}


void
KLedTest::nextColor() {

  ledcolor++;
  ledcolor%=4;

  switch(ledcolor) {
  default:
  case 0: l->setColor(Qt::green); break;
  case 1: l->setColor(Qt::blue); break;
  case 2: l->setColor(Qt::red); break;
  case 3: l->setColor(Qt::yellow); break;
  }
}


void
KLedTest::nextLook() {
  register int tmp;
  if (kled_round) {
    tmp = (static_cast<int>(ledlook) +1 ) % 3 ;
  }
  else {
    tmp = (static_cast<int>(ledlook) + 1) % 3;
  }
  ledlook = static_cast<KLed::Look>(tmp);
  l->setLook(ledlook);
  //qDebug("painting look %i", ledlook);
  //l->repaint();
}


void
KLedTest::timeout()
{
  const int NoOfLeds=sizeof(leds)/sizeof(leds[0]);
  int count;
  // -----
  for(count=0; count<NoOfLeds; ++count)
    {
      if(leds[count]->state()==KLed::Off)
	{
	  leds[count]->setState(KLed::On);
	} else {
	  leds[count]->setState(KLed::Off);
	}
    }
}


/*#include <stdio.h>*/

int main( int argc, char **argv )
{
    QApplication a(argc, argv);
    KLedTest widget;
    // -----
    /*
    if (argc>1) { // look out for round or circular led command
      if (strncmp(argv[1],"-c",2)) {
	// paint circular
	printf("painting circular led\n");
	widget.kled_round = true;
      }
      else if (strncmp(argv[1],"-r",2)) {
	// paint rectangle
	printf("painting rectangular led\n");
	widget.kled_round = false;
      }
    }
    */
    widget.show();
    return a.exec();				// go
}


