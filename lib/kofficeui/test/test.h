// Test for libkoffice
// (c) by Reginald Stadlbauer
// <reggie@kde.org>

#ifndef __kofficelib_test__
#define __kofficelib_test__

#include "../koAboutDia.h"
#include "../koPageLayoutDia.h"

#include <qpushbt.h>
#include <qwidget.h>
#include <qobject.h>

#include <kapp.h>

KoPageLayout layout;
KoHeadFoot hf;

class MyTest : public QWidget
{
  Q_OBJECT
  
public slots:
  void aKOffice() {KoAboutDia::about(KoAboutDia::KOffice,"0.0.1");}
  void aKDE() {KoAboutDia::about(KoAboutDia::KDE);}
  void aKSpread() {KoAboutDia::about(KoAboutDia::KSpread,"0.0.1");}
  void aKCharts() {KoAboutDia::about(KoAboutDia::KCharts,"0.0.1");}
  void aKImage() {KoAboutDia::about(KoAboutDia::KImage,"0.0.1");}
  void aKPresenter() {KoAboutDia::about(KoAboutDia::KPresenter,"0.0.1");}
  void aKAutoformEdit() {KoAboutDia::about(KoAboutDia::KAutoformEdit,"0.0.1");}
  void pgLayout() {KoPageLayoutDia::pageLayout(layout,hf,FORMAT_AND_BORDERS | HEADER_AND_FOOTER);}

};

#endif
