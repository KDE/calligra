//   testFastPixmap - test program for fast QPixmap::convertFromImage
//     replacement for specific display visuals
//
//   Copyright 1999 Andrew Richards <A.Richards@phys.canterbury.ac.nz>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   For more information see at the file COPYING in this package

#include <qimageio.h>
#include <qlabel.h>
#include <qimage.h>
#include <qapp.h>
#include <stdlib.h>
#include <X11/Xlib.h>

#include "misc.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
	qInitImageIO();

	QLabel q(0);
	QImage img("images/camTile2.jpg");
	img=img.convertDepth(32);
	QPixmap p(128,128);
	p.setOptimization(QPixmap::NoOptim);

	Display *dpy    =p.x11Display();
	int displayDepth=p.x11Depth();
	Visual *vis     =(Visual*)p.x11Visual();
	bool trueColour = (vis->c_class == TrueColor);
	showi(trueColour);

	uint  red_mask    = (uint)vis->red_mask;
	uint  green_mask  = (uint)vis->green_mask;
	uint  blue_mask   = (uint)vis->blue_mask;
	printf("red=%8x green=%8x blue=%8x\n",red_mask,green_mask,blue_mask);

	enum dispVisual {unknown, rgb565, rgb888} visual;
	visual=unknown;
	if ((red_mask==0xf800) && (green_mask==0x7e0) && (blue_mask==0x1f))
		visual=rgb565;
	if ((red_mask==0xff0000) && (green_mask==0xff00) && (blue_mask==0xff))
		visual=rgb888;
	
	//visual=unknown;
	
	switch (visual) {
	case rgb565:  puts("using RGB565"); break;
	case rgb888:  puts("using RGB888"); break;
	case unknown: puts("using convertFromImage"); break;
	}
	
	if (visual!=unknown) {
		XImage *xi;
		HANDLE h=p.handle();
		printf("p=%p\n",p.handle());
		xi=XCreateImage( dpy, vis, displayDepth, ZPixmap, 0,0, 128,128, 32, 0 );
		uchar *imageData=(uchar*)malloc(xi->bytes_per_line*128);
		xi->data=(char*)imageData;
		printf("bytes_per_line=%d\n",xi->bytes_per_line);

		TIME_START;
		if (visual==rgb565) {
			for(int i=0;i<10;i++) {
				ushort s;
				ushort *ptr=(ushort *)imageData;
				uchar *qimg=img.bits();
				for(int y=0;y<128;y++)
					for(int x=0;x<128;x++) { 
						s =(*qimg++)>>3;
						s|=(*qimg++ & 252)<<3;
						s|=(*qimg++ & 248)<<8; 
						qimg++;
						*ptr++=s;
					}
				XPutImage(dpy, p.handle(), qt_xget_readonly_gc(), xi, 0,0, 0,0, 128,128);
			}
		}
		if (visual==rgb888) {
			xi->data=(char*)img.bits();
			for(int i=0;i<10;i++) {
				XPutImage(dpy, p.handle(), qt_xget_readonly_gc(), xi, 0,0,0,0,128,128);
			}
		}
		TIME_END("fast method");
	} 


	if (visual==unknown) {
		TIME_START;
		for(int i=0;i<10;i++)
			p.convertFromImage(img);
		TIME_END("convertFromImage");
	}
		
	q.setPixmap(p);
	q.resize(p.size());
	q.show();

	app.exec();
	
}
