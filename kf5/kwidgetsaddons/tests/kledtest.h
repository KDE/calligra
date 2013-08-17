#ifndef kledtest_h
#define kledtest_h

#include <QWidget>
#include <QTimer>
#include <stdlib.h>
#include <kled.h>

class KLedTest : public QWidget
{
  Q_OBJECT
protected:
  QTimer timer;
  KLed *leds[/*KLed::NoOfShapes*/2* /*KLed::NoOfLooks*/3* /*KLed::NoOfStates*/2];
  const int LedWidth;
  const int LedHeight;
  const int Grid;
  KLed::Shape shape;
  KLed::Look look;
  KLed::State state;
  int x, y, index;


  QTimer t_toggle, t_color, t_look;
  //KLed *l;				// create lamp
  //KLed *l;				// create lamp
  KLed *l;				// create lamp
  //KLed *l;				// create lamp
  //KLed *l;				// create lamp
  int ledcolor;
  KLed::Look  ledlook;

public:

  KLedTest(QWidget* parent=0);
  ~KLedTest();

  bool kled_round;


public Q_SLOTS:
  void timeout();

  void nextColor();
  void nextLook();


};

#endif

