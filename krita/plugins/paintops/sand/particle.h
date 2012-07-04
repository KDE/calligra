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
#include <iostream>
#include <math.h>

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
    ///Default constructor
     explicit Particle(QObject *parent = 0);

    ///Parameterized constructor
    Particle( bool life = true,
              float mass = 1.0,
              float radius = 1,
              int lifespan = 10000,
              float friction = 0.00005, //default value of friction
              float dissipation = 0.0, //dissipation constant
              QPoint * position = 0,
              QPointF * velocity = 0,
              QPointF * acceleration = 0
            );

    //Methods of the particle dynamics

    ///responsible for update the actual force interacting with this particle. The forces is applied by the mouse.
    void applyForce();

    ///rk4 implementation for this particle
    void integrationStep(double dt);
    QPointF accel(const State &state, double t);
    Derivative eval(const State & init, double dt, const Derivative & der);
    
    //Gets
    
    bool isAlive() const {return _life;}
    int lifespan() const {return _lifespan;}
    float mass() const {return _mass;}
    float force() const {return _force;}
    float radius() const {return _radius;}
    float friction() const {return _friction;}
    float dissipation() const {return _dissipation;}
    
    QPoint * pos(){return _pos;}
    QPointF * vel(){return _vel;}
    QPointF * accel(){return _accel;}

    //Sets
    void setLife(bool lf){ _life = lf;}
    void setMass(float m) { _mass = m;}
    void force(float f) {_force = f;}
    void setRadius(float r) { _radius = r;}
    void setLife(int l) { _lifespan = l;}
    void setFriction(float f) { _friction = f;}
    void setDissipation(float d) {_dissipation = d;}

    void setPos(QPoint * p){ _pos = p;}
    void setVel(QPointF * v){ _vel = v;}
    void setPos(QPointF * a){ _accel = a;}



signals:

public slots:

private:
    ///Informs if the particle will vanish if lifespan reaches 0
    bool _life;
    
    ///This particle mass.
    float _mass;

    ///The force been applied on this particle
    float _force;
    
    ///Size of this particle radius
    float _radius;

    ///Quantity of timesteps this particle endures
    int _lifespan;

    ///Young modulus (friction) (should be tunned)
    float _friction;

    /// The A dissipative constant (should be tunned)
    float _dissipation;

    ///Particle position on canvas
    QPoint  * _pos;

    ///Particle velocity
    QPointF * _vel;

    ///Particle acceleration
    QPointF * _accel;

};

#endif // PARTICLE_H
 
