/* This file is part of the KDE libraries
   SPDX-FileCopyrightText: 1998 Stefan Taferner
                 2001/2003 thierry lorthiois (lorthioist@wanadoo.fr)
                 2011 Inge Wallin (inge@lysator.liu.se)

   SPDX-License-Identifier: LGPL-2.0-only
*/
#ifndef _WMFSTACK_H_
#define _WMFSTACK_H_

#include <QBrush>
#include <QColor>
#include <QFont>
#include <QImage>
#include <QPen>

/**
   Namespace for Windows Metafile (WMF) classes
*/
namespace Libwmf
{

class WmfAbstractBackend;
class WmfDeviceContext;

/**
 * WMF file allows manipulation on a stack of object.
 * It's possible to create, delete or select an object.
 */
class KoWmfHandle
{
public:
    virtual ~KoWmfHandle() = default;
    virtual void apply(WmfDeviceContext *) = 0;
};

class KoWmfBrushHandle : public KoWmfHandle
{
public:
    void apply(WmfDeviceContext *) override;
    QBrush brush;
};

class KoWmfPenHandle : public KoWmfHandle
{
public:
    void apply(WmfDeviceContext *) override;
    QPen pen;
};

class KoWmfPatternBrushHandle : public KoWmfHandle
{
public:
    void apply(WmfDeviceContext *) override;
    QBrush brush;
    QImage image;
};

class KoWmfFontHandle : public KoWmfHandle
{
public:
    void apply(WmfDeviceContext *) override;
    QFont font;
    int escapement;
    int orientation;
    int height; // Can be negative. In 'font' above, we store the absolute value.
};
}

#endif
