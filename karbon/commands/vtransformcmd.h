/* This file is part of the KDE project          é
   Copyright (C) 2001, The Karbon Developers
   Copyright (C) 2002, The Karbon Developers

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef __VTRANSFORMCMD_H__
#define __VTRANSFORMCMD_H__

#include <q3valuevector.h>
#include <QMap>
#include "vcommand.h"
#include "vgroup.h"
#include <koffice_export.h>
// Transform object(s) with a specified matrix and allow undo.


class QMatrix;
class VPath;
class VSubpath;
class VSegment;
class VSelection;


class KARBONCOMMAND_EXPORT VTransformCmd : public VCommand
{
public:
	VTransformCmd( VDocument *doc, const QMatrix& mat, bool duplicate = false );
	virtual ~VTransformCmd();

	virtual void execute();
	virtual void unexecute();

	virtual void visitVPath( VPath& composite );
	virtual void visitVSubpath( VSubpath& path );
	virtual void visitVText( VText& text );
	virtual void visitVImage( VImage& img );
	virtual void visitVObject( VObject& object );

	void setMatrix( const QMatrix& m )
	{
		m_mat = m;
	}

protected:
	VTransformCmd( VDocument *doc, const QString& name, const QString& icon, bool duplicate = false );

	VSelection* m_selection;
	VObjectList m_duplicates;

	QMatrix m_mat;

	bool m_duplicate;
};


class KARBONCOMMAND_EXPORT VTranslateCmd : public VTransformCmd
{
public:
	VTranslateCmd( VDocument *doc, double d1, double d2, bool duplicate = false );
};


class KARBONCOMMAND_EXPORT VScaleCmd : public VTransformCmd
{
public:
	VScaleCmd( VDocument *doc, const KoPoint& p, double s1, double s2, bool duplicate = false );
};


class KARBONCOMMAND_EXPORT VShearCmd : public VTransformCmd
{
public:
	VShearCmd( VDocument *doc, const KoPoint& p, double s1, double s2, bool duplicate = false );
};


class KARBONCOMMAND_EXPORT VRotateCmd : public VTransformCmd
{
public:
	VRotateCmd( VDocument *doc, const KoPoint& p, double angle, bool duplicate = false );
};

class KARBONCOMMAND_EXPORT VTranslateBezierCmd : public VCommand
{
public:
	VTranslateBezierCmd( VDocument *doc, VSegment *segment, double d1, double d2, bool firstControl );
	virtual ~VTranslateBezierCmd();

	virtual void execute();
	virtual void unexecute();

	virtual void visitVSubpath( VSubpath& path );

protected:
	QMatrix m_mat;
	VSegment *m_segment;
	VSegment *m_segmenttwo;
	bool	  m_firstControl;
	VSubpath *m_subpath;
};

class KARBONCOMMAND_EXPORT VTranslatePointCmd : public VCommand
{
public:
	VTranslatePointCmd( VDocument *doc, double d1, double d2 );
	virtual ~VTranslatePointCmd();

	virtual void execute();
	virtual void unexecute();

	virtual void visitVSubpath( VSubpath& path );

protected:
	void translatePoints();

	QMatrix m_mat;
	QMap<VSegment*, Q3ValueVector<int> > m_segPnts;
	VObjectList m_subpaths;
};

#endif

