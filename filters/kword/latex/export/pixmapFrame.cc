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
/* analyse                                 */
/*******************************************/
/* Get information about a pixmap stored   */
/* in the tarred file.                     */
/*******************************************/
void PixmapFrame::analyse(const QDomNode balise)
{
	/* MARKUP TYPE : FRAMESET INFO = TEXTE, ENTETE CONNUE */

	/* Parameters Analyse */
	Element::analyse(balise);

	kDebug(30522) << "FRAME ANALYSE (Pixmap)" << endl;

	/* Chlidren markups Analyse */
	for(int index = 0; index < getNbChild(balise); index++)
	{
		if(getChildName(balise, index).compare("FRAME")== 0)
		{
			analyseParamFrame(balise);
		}
		else if(getChildName(balise, index).compare("PICTURE")== 0)
		{
			getPixmap(getChild(balise, "PICTURE"));
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
void PixmapFrame::getPixmap(const QDomNode balise_initiale)
{
	kDebug(30522) << "PIXMAP" << endl;
	setKeepAspectRatio(getAttr(balise_initiale, "keepAspectRatio"));
	QDomNode balise = getChild(balise_initiale, "KEY");
	setKey(getAttr(balise, "filename"));
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
/* analyseParamFrame                       */
/*******************************************/
void PixmapFrame::analyseParamFrame(const QDomNode balise)
{
	/*<FRAME left="28" top="42" right="566" bottom="798" runaround="1" />*/

	_left = getAttr(balise, "left").toInt();
	_top = getAttr(balise, "top").toInt();
	_right = getAttr(balise, "right").toInt();
	_bottom = getAttr(balise, "bottom").toInt();
	setRunAround(getAttr(balise, "runaround").toInt());
	setAroundGap(getAttr(balise, "runaroundGap").toInt());
	setAutoCreate(getAttr(balise, "autoCreateNewFrame").toInt());
	setNewFrame(getAttr(balise, "newFrameBehaviour").toInt());
	setSheetSide(getAttr(balise, "sheetside").toInt());
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
	// 8 characters are deleted when readign the file picture name
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
/* Generate the text formated (if needed). */
/*******************************************/
void PixmapFrame::generate(QTextStream &out)
{
	if(Config::instance()->convertPictures())
		convert();
	
	Config::instance()->writeIndent(out);
	out << "\\includegraphics{" << getFilenamePS()<< "}" << endl;

}

