/*
** A program to convert the XML rendered by KWord into LATEX.
**
** Copyright (C) 2000, 2002 Robert JACOLIN
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** To receive a copy of the GNU Library General Public License, write to the
** Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
**
*/

#include <QDir>
//Added by qt3to4:
#include <QTextStream>
#include <kdebug.h>		/* for kDebug() stream */
#include <config.h>
#include <config-filters.h>
/* Needed to convert picture in eps file. Use ImageMagick. */
#ifdef HAVE_MAGICK
#        include <stdio.h>
#        include <time.h>
#        include <sys/types.h>
#        include <magick/api.h>
#endif

#include "document.h"
#include "pixmapFrame.h"
#include "fileheader.h"

/*******************************************/
/* Constructor                             */
/*******************************************/
PixmapFrame::PixmapFrame()
{
}

/*******************************************/
/* Destructor                              */
/*******************************************/
PixmapFrame::~PixmapFrame()
{
	kDebug(30522) << "Destruction of a pixmap" << endl;
}
void PixmapFrame::setKeepAspectRatio(const QString ratio)
{
	if(ratio == "true")
		_keepAspectRatio = true;
	else
		_keepAspectRatio = false;
}

/*******************************************/
/* analyze                                 */
/*******************************************/
/* Get information about a pixmap stored   */
/* in the tarred file.                     */
/*******************************************/
void PixmapFrame::analyze(const QDomNode node)
{
	/* Markup type: Frameset info = text, heading known */

	/* Parameter analysis */
	Element::analyze(node);

	kDebug(30522) << "FRAME ANALYSIS (Pixmap)" << endl;

	/* Child markup analysis */
	for(int index = 0; index < getNbChild(node); index++)
	{
		if(getChildName(node, index).compare("FRAME")== 0)
		{
			analyzeParamFrame(node);
		}
		else if(getChildName(node, index).compare("PICTURE")== 0)
		{
			getPixmap(getChild(node, "PICTURE"));
		}

	}
	kDebug(30522) << "END OF A FRAME" << endl;
}

/*******************************************/
/* getPixmap                               */
/*******************************************/
/* Get information about a pixmap stored   */
/* in the tarred file.                     */
/*******************************************/
void PixmapFrame::getPixmap(const QDomNode node)
{
	kDebug(30522) << "PIXMAP" << endl;
	setKeepAspectRatio(getAttr(node, "keepAspectRatio"));
	QDomNode childNode = getChild(node, "KEY");
	setKey(getAttr(childNode, "filename"));
	FileHeader::instance()->useGraphics();
	QString file = getKey();
	/* Remove the extension */
	int posExt = file.findRev('.');
	file.truncate(posExt);
	/* Remove the path */
	file = file.section('/', -1);
	setFilenamePS(file + ".eps");
	kDebug(30522) << "PS : " << getFilenamePS() << endl;
	kDebug(30522) << "END PIXMAP" << endl;
}

/*******************************************/
/* analyzeParamFrame                       */
/*******************************************/
void PixmapFrame::analyzeParamFrame(const QDomNode node)
{
	/*<FRAME left="28" top="42" right="566" bottom="798" runaround="1" />*/

	_left = getAttr(node, "left").toInt();
	_top = getAttr(node, "top").toInt();
	_right = getAttr(node, "right").toInt();
	_bottom = getAttr(node, "bottom").toInt();
	setRunAround(getAttr(node, "runaround").toInt());
	setAroundGap(getAttr(node, "runaroundGap").toInt());
	setAutoCreate(getAttr(node, "autoCreateNewFrame").toInt());
	setNewFrame(getAttr(node, "newFrameBehaviour").toInt());
	setSheetSide(getAttr(node, "sheetside").toInt());
}

/**
 * Convert a pixmap file in eps file. Use ImageMagick
 */
void PixmapFrame::convert()
{
#ifdef HAVE_MAGICK
	kDebug(30522) << "CONVERT PICTURE IN EPS" << endl;
	ExceptionInfo exception;

	Image* image;

	ImageInfo
		*image_info;

	/*
		Initialize the image info structure and read an image.
	*/
	InitializeMagick(NULL);
	GetExceptionInfo(&exception);
	image_info = CloneImageInfo((ImageInfo *) NULL);
	// 8 characters are deleted when reading the file picture name
	QString filename = "file:///" + getRoot()->extractData(getKey());
	strncpy(image_info->filename, filename.latin1(), filename.length());
	image = ReadImage(image_info, &exception);
	if (image == (Image *) NULL)
		MagickError(exception.severity, exception.reason, exception.description);
	else
	{
		/*
			Write the image as EPS and destroy it.
		  Copy image file in the same directory than the tex file.
		*/
		QString dir = "";
		if( Config::instance()->getPicturesDir().isEmpty() || 
				Config::instance()->getPicturesDir() == NULL)
		{
			dir = getFilename();
			dir.truncate(getFilename().findRev('/'));
		}
		else
			dir = Config::instance()->getPicturesDir();
		kDebug(30522) << "file " << getFilename() << endl;
		kDebug(30522) << "path " << dir << endl;
		(void) strcpy(image->filename, (dir + "/" + getFilenamePS()).latin1());
		WriteImage(image_info, image);
		DestroyImage(image);
	}
	DestroyImageInfo(image_info);
	DestroyExceptionInfo(&exception);
	DestroyMagick();
	kDebug(30522) << "CONVERTION DONE" << endl;
#endif
}

/*******************************************/
/* generate                                */
/*******************************************/
/* Generate the text formatted (if needed).*/
/*******************************************/
void PixmapFrame::generate(QTextStream &out)
{
	if(Config::instance()->convertPictures())
		convert();
	
	Config::instance()->writeIndent(out);
	out << "\\includegraphics{" << getFilenamePS()<< "}" << endl;

}

