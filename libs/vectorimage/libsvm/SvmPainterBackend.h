/*
  SPDX-FileCopyrightText: 2009 Inge Wallin <inge@lysator.liu.se>

  SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef SVMPAINTERBACKEND_H
#define SVMPAINTERBACKEND_H

#include "SvmAbstractBackend.h"
#include "kovectorimage_export.h"

#include <QSize>
#include <QTransform>

#include "SvmEnums.h"
#include "SvmGraphicsContext.h"
#include "SvmStructs.h"

class QRect;
class QPolygon;
class QPainter;

/**
   \file

   Primary definitions for SVM output backend
*/

/**
   Namespace for StarView Metafile (SVM) classes
*/
namespace Libsvm
{

/**
    Painter output strategy for SVM Parser
*/
class KOVECTORIMAGE_EXPORT SvmPainterBackend : public SvmAbstractBackend
{
public:
    SvmPainterBackend(QPainter *painter, const QSize &outputSize);
    ~SvmPainterBackend() override;

    /**
       Initialisation routine

       \param header the SVM Header record
    */
    void init(const SvmHeader &header) override;

    /**
       Cleanup routine

       This function is called when the painting is done.  Any
       initializations that are done in init() can be undone here if
       necessary.
    */
    void cleanup() override;

    /**
       Close-out routine
    */
    void eof() override;

    void rect(SvmGraphicsContext &context, const QRect &rect) override;

    /**
       Handler META_POLYLINE_ACTION

       This action type specifies how to output a multi-segment line
       (unfilled polyline).

       \param context the graphics context to be used when drawing the polyline
       \param polyline the sequence of points that describe the line

       \note the line is not meant to be closed (i.e. do not connect
       the last point to the first point) or filled.
    */
    void polyLine(SvmGraphicsContext &context, const QPolygon &polyline) override;

    /**
       This action type specifies how to output a multi-segment line
       (unfilled polygon).

       \param context the graphics context to be used when drawing the polygon
       \param polygon the sequence of points that describe the polygon
    */
    void polygon(SvmGraphicsContext &context, const QPolygon &polygon) override;

    void polyPolygon(SvmGraphicsContext &context, const QList<QPolygon> &polyPolygon) override;

    void textArray(SvmGraphicsContext &context,
                   const QPoint &point,
                   const QString &string,
                   quint16 startIndex,
                   quint16 len,
                   quint32 dxArrayLen,
                   qint32 *dxArray) override;

private:
    void updateFromGraphicscontext(SvmGraphicsContext &context);

private:
    QPainter *m_painter;
    QSize m_outputSize;

    QTransform m_outputTransform;
};
}

#endif
