/*
 *  Copyright (c) 2012 Francisco fernandes <francisco.fernandes.j@gmail.com>
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
 * \file particle.h
 *
 * \brief Contains all information which characterizes a particle and all methods which
 * compute its dynamics.
 *
 * \attention THIS PARTICLE DOES NOT HAVE ANGULAR COMPONENTS. FOR THAT,
 * ADD THE ROTATION PROPERTIES angle, angular velocity AND THE ANGULAR
 * MATERIAL PROPERTIES mu AND gamma.
 * 
 * \version 0.0
 * \date May 2012
 *
 */

#ifndef PARTICLE_H
#define PARTICLE_H

#include <QObject>
#include <QPoint>
#include <QPointF>
#include <QColor>
#include <QDataStream>
#include <iostream>
#include <math.h>
#include <QDebug>
#include <kis_types.h>

using namespace std;

/**
 * Structs to do the derivation using the RK4 method in the
 * integrationStep() method.
 */

///Current state of this particle
typedef struct{
    QPoint pos; //current position
    QPointF vel; //current velocity
} State;

/// Derivatives of the current state
typedef struct{
    QPointF dpos; //derivative of the position : velocity
    QPointF dvel; //derivative of the velocity : acceleration
}Derivative;

class Particle : public QObject
{
    Q_OBJECT
public:
     explicit Particle(QObject *parent = 0);
     
    ///Construct a particle with arguments (or its default values)
    Particle( bool life = true,
              float mass = 1.0,
              int radius = 1,
              int lifespan = 10000,
              float friction = 0.00005, //default value of friction
              float dissipation = 0.0, //dissipation constant
              QPoint * position = 0,
              QPointF * velocity = 0,
              QPointF * acceleration = 0
            );

    ///Copy constructor
    Particle(const Particle & p);

    /*
     * Methods of the particle dynamics
     */

    /**
     * Responsible for update the actual force interacting with this particle. The force is applied by the mouse,
     * which should behave like there is a particle on it's position, so we could do a simple collision and move
     * the grains on the canvas.
     */
    void applyForce(const KisPaintInformation& info);

    ///Runge-Kutta 4 implementation for this particle

    /**
     * Advances the particle state in one timestep.
     */
    void integrationStep(double dt);

    /**
     * Calculate the particle acceleration based on applied forces
     */
    QPointF accel(const State &state, double t);

    /**
     * Evaluate the integration step to update the particle state
     */
    Derivative eval(const State & init, double dt, const Derivative & der);
    
    /*
     * Object methods
     */

    //Gets
    
    bool isAlive() const {return _life;}
    int lifespan() const {return _lifespan;}
    float mass() const {return _mass;}
    float force() const {return _force;}
    int radius() const {return _radius;}
    float friction() const {return _friction;}
    float dissipation() const {return _dissipation;}
    
    QPoint * pos() const {return _pos;}
    QPointF * vel() const {return _vel;}
    QPointF * accel()const {return _accel;}

    //Sets
    
    void setLife(bool lf){ _life = lf;}
    void setMass(float m) { _mass = m;}
    void setForce(float f) {_force = f;}
    void setRadius(int r) { _radius = r;}
    void setLifespan(int l) { _lifespan = l;}
    void setFriction(float f) { _friction = f;}
    void setDissipation(float d) {_dissipation = d;}
    void setPos(QPoint * p){ _pos = p;}
    void setVel(QPointF * v){ _vel = v;}
    void setAccel(QPointF * a){ _accel = a;}


    ///Operator overload to serialization of the Particle to use KisAnnotation
    
    friend QDataStream &operator<<(QDataStream &out, const Particle &particle);
    friend QDataStream &operator>>(QDataStream &in, Particle &particle);

signals:

public slots:

private:
    ///Informs if the particle will vanish if lifespan reaches 0
    bool _life;
    
    ///This particle mass.
    float _mass;

    ///The force that have been applied in this particle
    float _force;
    
    ///Particle radius
    int _radius;

    ///Quantity of timesteps this particle endures (not useful right now)
    int _lifespan;

    ///Young modulus (friction) (should be tunned to proper behaviour)
    float _friction;

    /// The A dissipative constant (should be tunned to proper behaviour)
    float _dissipation;

    ///Particle position on canvas
    QPoint  * _pos;

    ///Particle velocity
    QPointF * _vel;

    ///Particle acceleration
    QPointF * _accel;

};

#endif // PARTICLE_H
 
