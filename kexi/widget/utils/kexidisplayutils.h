/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.

   Original Author:  Till Busch <till@bux.at>
   Original Project: buX (www.bux.at)
*/

#ifndef KEXIDISPUTILS_H
#define KEXIDISPUTILS_H

#include <qfont.h>
#include <qcolor.h>
class QWidget;

//! \brief A set of utilities related to displaying common elements in Kexi, like e.g. (autonumber) sign
class KEXIGUIUTILS_EXPORT KexiDisplayUtils
{
	public:
		class DisplayParameters
		{
			public:
				QColor textColor;
			protected:
				QFont font;
				int textWidth, textHeight;
			
			friend class KexiDisplayUtils;
		};

		static void initDisplayForAutonumberSign(DisplayParameters& par, QWidget *widget);

		static void drawAutonumberSign(const DisplayParameters& par, QPainter* painter, 
			int x, int y, int width, int height, int align, bool overrideColor = false);
};

#endif
