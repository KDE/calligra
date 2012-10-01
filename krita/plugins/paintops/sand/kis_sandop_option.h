/*
 *  Copyright (c) 2012 Francisco Fernandes <francisco.fernandes.j@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef KIS_SANDOP_OPTION_H
#define KIS_SANDOP_OPTION_H

#include <kis_paintop_option.h>
#include <krita_export.h>

/*
 * Sand  strings
 */

const QString SAND_RADIUS = "Sand/radius";
const QString SAND_AMOUNT = "Sand/amount";
const QString SAND_DEPLETION = "Sand/sandDepletion";
const QString SAND_SIZE = "Sand/size";
const QString SAND_MASS = "Sand/mass";
const QString SAND_FRICTION = "Sand/friction";
const QString SAND_BMASS = "Sand/brushMass";
const QString SAND_BFRICTION = "Sand/brushFriction";    
const QString SAND_MODE = "Sand/mode";    //set the operation mode for this brush (pouring or spread)
const QString SAND_GRID_X = "Sand/gridWidth";
const QString SAND_GRID_Y = "Sand/gridHeight";
const QString SAND_GRID_RESIZE = "Sand/gridAutoResize";

class KisSandOpOptionsWidget;

class KisSandOpOption : public KisPaintOpOption
{
public:
    KisSandOpOption();
    ~KisSandOpOption();

    
    int radius() const;
    void setRadius(int radius) const;

    int amount() const;
    void setAmount(int amount) const;

    int size() const;
    void setSize(int size) const;
    
    double friction () const;
    void setFriction(double friction) const;
    
    double mass () const;
    void setMass(double mass) const;

    double bFriction () const;
    void setBfriction(double friction) const;

    double bMass () const;
    void setBmass(double mass) const;

    int gridWidth() const;
    void setGridWidth(int x) const;
    
    int gridHeight() const;
    void setGridHeight(int y) const;
    
    bool sandDepletion() const;
    bool mode() const;
    bool gridAutoResize() const;

    void writeOptionSetting(KisPropertiesConfiguration* setting) const;
    void readOptionSetting(const KisPropertiesConfiguration* setting);

private:

    KisSandOpOptionsWidget * m_options;

};

class SandProperties {
    
public:
    ///Brush size
    int radius;

    ///Amount of the sand per stroke
    int amount;

    ///Enable sand quantity limitation per stroke
    bool sandDepletion;

    ///Particle radius
    int size;

    ///Particle mass
    double mass;

    ///Particle friction
    double friction;

    ///Brush "particle" mass
    double b_mass;

    ///Brush "particle" friction
    double b_friction;

    ///Brush mode: pouring or spreading
    bool mode;

    ///Grid width
    int gridWidth;

    ///Grid height
    int gridHeight;

    ///Grid Auto Resize
    bool gridAutoResize;

    ///Energy dissipation of the particle (damping)
    //float dissipation;

    /**
     * Read the particle configuration from the brush settings widget
     */
    void readOptionSetting(const KisPropertiesConfiguration* settings){
        radius = settings->getInt(SAND_RADIUS);
        amount = settings->getInt(SAND_AMOUNT);
        sandDepletion = settings->getBool(SAND_DEPLETION);
        size = settings->getInt(SAND_SIZE);
        mass = settings->getFloat(SAND_MASS);
        friction = settings->getFloat(SAND_FRICTION);
        b_mass = settings->getFloat(SAND_BMASS);
        b_friction = settings->getFloat(SAND_BFRICTION);
        mode = settings->getBool(SAND_MODE); 
        gridWidth = settings->getInt(SAND_GRID_X);
        gridHeight = settings->getInt(SAND_GRID_Y);
        gridAutoResize = settings->getBool(SAND_GRID_RESIZE);
//         dissipation = settings->getFloat(SAND_DISSIPATION);
    }
};

#endif
