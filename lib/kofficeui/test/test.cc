// Test for libkoffice
// (c) by Reginald Stadlbauer
// <reggie@kde.org>

#include "test.h"
#include "test.moc"

#include <op_app.h>

class MyApp : public OPApplication
{
public:
  MyApp( int argc, char **argv ) : OPApplication( argc, argv )
  {
  }
  
  void start();
};

void MyApp::start()
{
  MyTest *w = new MyTest();
  w->resize(220,385);
  w->setMaximumSize(w->size());
  w->setMinimumSize(w->size());
  layout = KoPageLayoutDia::standardLayout();

  cl.columns = 2;
  cl.columnSpacing = 2;

  QPushButton* aboutKOffice = new QPushButton("About Koffice...",w);
  aboutKOffice->move(10,10);
  aboutKOffice->resize(200,25);
  QObject::connect( aboutKOffice,SIGNAL(clicked()),w,SLOT(aKOffice()));

  QPushButton* aboutKDE = new QPushButton("About KDE...",w);
  aboutKDE->move(10,40);
  aboutKDE->resize(200,25);
  QObject::connect( aboutKDE,SIGNAL(clicked()),w,SLOT(aKDE()));

  QPushButton* aboutKSpread = new QPushButton("About KSpread...",w);
  aboutKSpread->move(10,70);
  aboutKSpread->resize(200,25);
  QObject::connect( aboutKSpread,SIGNAL(clicked()),w,SLOT(aKSpread()));

  QPushButton* aboutKCharts = new QPushButton("About KCharts...",w);
  aboutKCharts->move(10,100);
  aboutKCharts->resize(200,25);
  QObject::connect( aboutKCharts,SIGNAL(clicked()),w,SLOT(aKCharts()));

  QPushButton* aboutKImage = new QPushButton("About KImage...",w);
  aboutKImage->move(10,130);
  aboutKImage->resize(200,25);
  QObject::connect( aboutKImage,SIGNAL(clicked()),w,SLOT(aKImage()));

  QPushButton* aboutKPresenter = new QPushButton("About KPresenter...",w);
  aboutKPresenter->move(10,160);
  aboutKPresenter->resize(200,25);
  QObject::connect( aboutKPresenter,SIGNAL(clicked()),w,SLOT(aKPresenter()));

  QPushButton* aboutKAutoformEdit = new QPushButton("About KAutoformEdit...",w);
  aboutKAutoformEdit->move(10,190);
  aboutKAutoformEdit->resize(200,25);
  QObject::connect( aboutKAutoformEdit,SIGNAL(clicked()),w,SLOT(aKAutoformEdit()));

  QPushButton* pageLayout = new QPushButton("Page Layout...",w);
  pageLayout->move(10,240);
  pageLayout->resize(200,25);
  QObject::connect( pageLayout,SIGNAL(clicked()),w,SLOT(pgLayout()));

  QPushButton* selectPart = new QPushButton("Select Part...",w);
  selectPart->move(10,270);
  selectPart->resize(200,25);
  QObject::connect( selectPart,SIGNAL(clicked()),w,SLOT(selectPart()));

  QPushButton* printBtn = new QPushButton("Print...",w);
  printBtn->move(10,300);
  printBtn->resize(200,25);
  QObject::connect( printBtn,SIGNAL(clicked()),w,SLOT(print()));

  QPushButton* quit = new QPushButton("Quit",w);
  quit->move(10,350);
  quit->resize(200,25);
  QObject::connect( quit,SIGNAL(clicked()), SLOT(quit()));

  w->show();
}

int main(int argc,char **argv)
{
  MyApp a(argc,argv);
  a.exec();
  return 0;
}
