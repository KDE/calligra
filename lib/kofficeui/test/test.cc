// Test for libkoffice
// (c) by Reginald Stadlbauer
// <reggie@kde.org>

#include "test.h"
#include "test.moc"

void main(int argc,char **argv)
{
  KApplication a(argc,argv);
  MyTest *w = new MyTest();
  w->resize(220,325);
  w->setMaximumSize(w->size());
  w->setMinimumSize(w->size());
  layout = KoPageLayoutDia::standardLayout();

  QPushButton aboutKOffice("About Koffice...",w);
  aboutKOffice.move(10,10);
  aboutKOffice.resize(200,25);
  QObject::connect(&aboutKOffice,SIGNAL(clicked()),w,SLOT(aKOffice()));

  QPushButton aboutKDE("About KDE...",w);
  aboutKDE.move(10,40);
  aboutKDE.resize(200,25);
  QObject::connect(&aboutKDE,SIGNAL(clicked()),w,SLOT(aKDE()));

  QPushButton aboutKSpread("About KSpread...",w);
  aboutKSpread.move(10,70);
  aboutKSpread.resize(200,25);
  QObject::connect(&aboutKSpread,SIGNAL(clicked()),w,SLOT(aKSpread()));

  QPushButton aboutKCharts("About KCharts...",w);
  aboutKCharts.move(10,100);
  aboutKCharts.resize(200,25);
  QObject::connect(&aboutKCharts,SIGNAL(clicked()),w,SLOT(aKCharts()));

  QPushButton aboutKImage("About KImage...",w);
  aboutKImage.move(10,130);
  aboutKImage.resize(200,25);
  QObject::connect(&aboutKImage,SIGNAL(clicked()),w,SLOT(aKImage()));

  QPushButton aboutKPresenter("About KPresenter...",w);
  aboutKPresenter.move(10,160);
  aboutKPresenter.resize(200,25);
  QObject::connect(&aboutKPresenter,SIGNAL(clicked()),w,SLOT(aKPresenter()));

  QPushButton aboutKAutoformEdit("About KAutoformEdit...",w);
  aboutKAutoformEdit.move(10,190);
  aboutKAutoformEdit.resize(200,25);
  QObject::connect(&aboutKAutoformEdit,SIGNAL(clicked()),w,SLOT(aKAutoformEdit()));

  QPushButton pageLayout("Page Layout...",w);
  pageLayout.move(10,240);
  pageLayout.resize(200,25);
  QObject::connect(&pageLayout,SIGNAL(clicked()),w,SLOT(pgLayout()));

  QPushButton quit("Quit",w);
  quit.move(10,290);
  quit.resize(200,25);
  QObject::connect(&quit,SIGNAL(clicked()),&a,SLOT(quit()));

  w->show();
  return a.exec();
}
