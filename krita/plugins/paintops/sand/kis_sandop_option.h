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

/**
 * These options are for the sand addition/deletion brush. We will have two ways for this brush work:
 *
 * (1) Add / Delete sand grains, where the sand addition/removal will follow the mouse
 * position and dynamics.
 * 
 *  a) Add an AMOUNT of particles with initial conditions (velocity, position and acceleration) based
 *     on the mouse movements. As the user will be adding some sand, the amount falling will be reduced as it moves.
 *  b) Remove all the particles under the brush RADIUS. It's independent of the mouse dynamics.
 *  c) The particle properties will be default. Perhaps in the future we can let a user modify the radius, friction
 *     and dissipation properties of a grain.
 *  
 *  
 * 
 */
#ifndef KIS_SANDOP_OPTION_H
#define KIS_SANDOP_OPTION_H

#include <kis_paintop_option.h>
#include <krita_export.h>

/*
 * Sand add/remove strings
 */
const QString SAND_RADIUS = "Sand/radius";
const QString SAND_AMOUNT = "Sand/amount";
const QString SAND_DEPLETION = "Sand/sandDepletion";
const QString SAND_SIZE = "Sand/size"; //size of a grain (particle)
const QString SAND_MASS = "Sand/mass";    //
const QString SAND_FRICTION = "Sand/friction";    //Young modulus -

/*
 * Sand spread strings
 * Obs.: Have to choose a better name for this functionality
 * Particle (*)
 * Brush
 * Spread
 * Stroke
 * (...)
 */

// const QString SAND_MODE = "Sand/mode"    //set the operation mode for this brush (drop or spread)
// const QString SAND_DISSIPATION = "Sand/dissipation" //living time of a particle (this is necessary?)

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
    
    bool sandDepletion() const;

//     void setDissipation(float dissipation) const;
//     float dissipation () const;
    
    void writeOptionSetting(KisPropertiesConfiguration* setting) const;
    void readOptionSetting(const KisPropertiesConfiguration* setting);

private:

    KisSandOpOptionsWidget * m_options;

};

class SandProperties {
    
public:
    int radius;
    int amount;
    bool sandDepletion;
    int size;
    double mass;
    double friction;
    
    //bool mode;
    //float dissipation;
    
    
    void readOptionSetting(const KisPropertiesConfiguration* settings){
        radius = settings->getInt(SAND_RADIUS);
        amount = settings->getInt(SAND_AMOUNT);
        sandDepletion = settings->getBool(SAND_DEPLETION);
        size = settings->getInt(SAND_SIZE);
        mass = settings->getFloat(SAND_MASS);
        friction = settings->getFloat(SAND_FRICTION);
//         mode = settings->getBool(SAND_MODE); //??
//         dissipation = settings->getFloat(SAND_DISSIPATION);
    }
};

#endif
