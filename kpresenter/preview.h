/******************************************************************/
/* KPresenter - (c) by Reginald Stadlbauer 1997-1998              */
/* Used and modified Perview Implementation by                    */
/*                  Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de) */
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
/* Module: Filedialog Preview                                     */
/******************************************************************/

#ifndef preview_h
#define preview_h

#include <kfiledialog.h>

#include <qstring.h>

// WMF preview
bool wmfPreviewHandler (const KFileInfo*,const QString fileName,QString&,QPixmap& pixmap);

// Pixmap preview
bool pixmapPreviewHandler(const KFileInfo*,const QString fileName,QString&,QPixmap& pixmap);

#endif
