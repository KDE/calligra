/* This file is part of the KDE project
   Copyright (C) 2000 Werner Trobin <trobin@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef gprimitivecmds_h
#define gprimitivecmds_h

#include <gobject.h>
#include <kcommand.h>

class QPoint;
class FxPoint;
class QRect;
class FxRect;
class QBrush;
class QPen;

template<class Property, void (GObject::* Function) (const Property &)> class GenericCmd : public KCommand {

public:
    GenericCmd(GObject *object, const QString &name) : KCommand(name), m_object(object) {}
    GenericCmd(GObject *object, const QString &name,
               const Property &oldProperty, const Property &newProperty) : KCommand(name),
        m_object(object), m_oldProperty(oldProperty), m_newProperty(newProperty) {}
    virtual ~GenericCmd() {}

    virtual void execute() { if(m_object) (m_object->*Function)(m_newProperty); }
    virtual void unexecute() { if(m_object) (m_object->*Function)(m_oldProperty); }

    void setOldProperty(const Property &oldProperty) { m_oldProperty=oldProperty; }
    const Property &oldProperty() const { return m_oldProperty; }
    void setNewProperty(const Property &newProperty) { m_newProperty=newProperty; }
    const Property &newProperty() const { return m_newProperty; }

private:
    // Don't copy or assign this stuff
    GenericCmd(const GenericCmd<Property, Function> &rhs);
    GenericCmd &operator=(const GenericCmd<Property, Function> &rhs);

    GObject *m_object;
    Property m_oldProperty, m_newProperty;
};

typedef GenericCmd<FxPoint, &GObject::setOrigin> GMoveCmd;
typedef GenericCmd<FxRect, &GObject::resize> GResizeCmd;
typedef GenericCmd<QBrush, &GObject::setBrush> GSetBrushCmd;
typedef GenericCmd<QPen, &GObject::setPen> GSetPenCmd;
typedef GenericCmd<QString, &GObject::setName> GSetNameCmd;
typedef GenericCmd<GObject::FillStyle, &GObject::setFillStyle> GSetFillStyleCmd;
typedef GenericCmd<Gradient, &GObject::setGradient> GSetGradientCmd;


class GRotateCmd : public KCommand {

public:
    GRotateCmd(GObject *object, const QString &name);
    GRotateCmd(GObject *object, const QString &name, const FxPoint &center,
               const double &angle);
    virtual ~GRotateCmd() {}

    virtual void execute();
    virtual void unexecute();

    void setCenter(const FxPoint &center) { m_center=center; }
    const FxPoint &center() const { return m_center; }
    void setAngle(const double &angle) { m_angle=angle; }
    const double &angle() const { return m_angle; }

private:
    GObject *m_object;
    FxPoint m_center;
    double m_angle;
};


class GScaleCmd : public KCommand {

public:
    GScaleCmd(GObject *object, const QString &name);
    GScaleCmd(GObject *object, const QString &name, const FxPoint &origin,
              const double &xfactor, const double &yfactor);
    virtual ~GScaleCmd() {}

    virtual void execute();
    virtual void unexecute();

    void setOrigin(const FxPoint &origin) { m_origin=origin; }
    const FxPoint &origin() const { return m_origin; }
    void setXFactor(const double &xfactor) { m_xfactor=xfactor; }
    const double &xfactor() const { return m_xfactor; }
    void setYFactor(const double &yfactor) { m_yfactor=yfactor; }
    const double &yfactor() const { return m_yfactor; }

private:
    GObject *m_object;
    FxPoint m_origin;
    double m_xfactor, m_yfactor;
};

#endif // gprimitivecmds_h
