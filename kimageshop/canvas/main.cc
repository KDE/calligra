//   test program for canvas widget
//
//   Copyright 1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For more information see at the file COPYING in this package

#include <kimgio.h>
#include <qlabel.h>
#include <qimage.h>
#include "channel.h"
#include "layer.h"
#include "numSlider.h"
#include "layerlist.h"
#include "brush.h"
#include "canvas.h"

// #include <ktopwidget.h>
// #include <kwm.h>
// #include <kapp.h>
#include <qapp.h>



int main(int argc, char **argv)
{

  QApplication app(argc, argv);
	
	kimgioRegister();

	dbg=true;

 	canvas can(510,515);
	can.addRGBLayer("images/cam9b.jpg");
	can.setLayerOpacity(200);
// 	can.moveLayer(50,10);
// 	can.setLayerOpacity(255);
//  	can.setLayerOpacity(128);
	// 	can.addRGBLayer("images/cam9.jpg");

	can.addRGBLayer("images/cambw12.jpg");
	can.moveLayer(256,384);
	can.setLayerOpacity(180);
 	can.addRGBLayer("images/cam05.jpg");
	can.setLayerOpacity(255);
 	can.addRGBLayer("images/cam6.jpg");
	can.moveLayer(240,280);
 	can.setLayerOpacity(255);
	can.addRGBLayer("images/img2.jpg");
 	can.setLayerOpacity(80);

	brush br("images/brush.jpg");
	br.setHotSpot(QPoint(25,25));
	//	can.paintBrush(QPoint(10,10), &br);
	can.currentBrush=&br;


	//for(int i=0;i<10;i++)
	can.compositeImage(QRect());
	can.show();
	app.setMainWidget(&can);	

	layerList ll(&can);
  ll.show();
	ll.resize(150,200);

	return app.exec();  
}
