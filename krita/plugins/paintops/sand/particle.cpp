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
 * \file particle.cpp
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

#include "particle.h"

///Parameterized constructor
Particle::Particle( bool life = true, float mass = 1.0; float radius = 1, int lifespan = 10000, float friction = 0.00005, float dissipation = 0.0, //dissipation constant
            QVector2D * position = 0, QVector2D * velocity = 0, QVector2D * acceleration = 0, QObject * parent = 0)
{
    _life   = life,
    _mass   = mass;
    _radius =  radius = 1;
    _lifespan = lifespan;
    _friction = friction;
    _dissipation = dissipation;
    _pos = position;
    _vel = velocity;
    _accel = acceleration;
}

//Methods of the particle dynamics

/** Responsible for update the actual force interacting with this particle. The force is applied by the mouse,
 *  which should behave like there is a particle on it's position, so we could do a simple collision and move
 *  the grains on the canvas.
 *
 * The emission of grains will be responsible by the ParticleEmitter, but will use this method as well.
 * ?: Perhaps I should set this method as a slot
 */ 

{

    /*
     * xi calculation is imcomplete. It should be like this:
     * 
     * dist_x = _position->x() - mouse_position->x();   //distance between particle centers: X
     * dist_y = _position->y() - mouse_position->y();   //distance between particle centers: Y
     * dist_centers = sqrt(dist_x^2 + dist_y^2);        //distance norm
     * double xi = _radius + brush_size - dist_centers
     * 
     * COMPLETE THIS EQUATION WHEN GET THE MOUSE INFORMATION
     */
    double xi = _radius ;

    //this is a simple formality, the collision detection will be made before the force application
    if( xi > 0){
        
        /* normalized young modulus between the MOUSE (brush) particle and this particle
         * double Y = _friction * brush_friction / (friction + brush_friction)
         */
        double Y = _friction;  // Young modulus
        /*
         * dissipative constant: function of the material viscosity
         * Maybe it won't be needed to set this for the MOUSE (BRUSH) PARTICLE
         * double A = 0.5*( _dissipation + brush_dissipation );
         */
        double A =  _dissipation;

        /*
         * ATTENTION: IN A SITUATION WHERE PARTICLES HAVE ANGULAR MOVEMENT (ROTATIONS), WE WOULD NEED THE
         * mu AND gamma COEFFICIENTS
         */

        /*
         * double reff = ( _radius * brush_size )/( _radius + brush_size );
         */
        double reff = _radius;

        /*
         * FOR THE VELOCITY DERIVATION. Same as above: we need the velocity of the
         * MOUSE particle:
         *
         * double dvx = _velocity->x() - _brush_velocity->x()
         * double dvy = _velocity->y() - _brush_velocity->y()
         * 
         */
        double dvx = _velocity->x();
        double dvy = _velocity->y();


        /*
         * rr_rez = 1 /dist_centers 
         */
        double rr_rez = 1 / radius;

        /*
         * dist_x = _position->x() - mouse_position->x();   //distance between particle centers: X
         * dist_y = _position->y() - mouse_position->y();   //distance between particle centers: Y
         *
         * double ex = dist_x * rr_rez;
         * double ey = dist_y * rr_rez;
         */

        double ex = 1;  //modify
        double ey = 1;  //modify
        double xidot = - (ex*dvx + ey*dvy);

        //NORMAL FORCE ON THIS PARTICLE
        double fn = sqrt(xi)*Y*sqrt(reff)*(xi+A*xidot);

        //PERHAPS I HAVE TO MODIFY THIS, SINCE WE CAN HAVE A FORCE IN A DIFFERENT DIRECTION OF THE MOVEMENT
         if( fn< 0)
            fn = 0;
         else
             _force += fn; 
        
    }
}

///rk4 implementation for this particle
void Particle::integrationStep();

Derivative Particle::eval(const State & init, double dt, const Derivative & der)
{
    State st;
    st.pos = init.pos + der.dpos*dt;            //apply the position modification
    st.vel = init.pos + der.dvel*dt;            //apply the velocity modification

    Derivative out;                             //output derivative

    out.dpos = st.vel;                          //derivative velocity
    out.dvel = accel(st, dt);                   //derivative acceleration

    return out;
}

/**
 * \attention This method should be tunned. The dumping it's behaving in a strange mode. Instead
 * of go to zero, it keeps to a negative value. Should fix this!!!
 */
QPointF Particle::accel(const State &state, double t)
{
    const double k = 3.2; //dumping
    const double b = _mass;
    return QPointF( qreal(-k * state.pos.x() - b*state.vel.x() ), qreal(-k * state.pos.y() - b*state.vel.y()));
}

void Particle::integrationStep(double dt)
{
    //this particle current state
    State state;
    state.pos = _pos;
    state.vel = _vel;

    //derivatives for the RK terms
    Derivative k1 = eval(state, 0.0, Derivative());
    Derivative k2 = eval(state, dt*0.5, k1);
    Derivative k3 = eval(state, dt*0.5, k2);
    Derivative k4 = eval(state, dt, k3);

    QPointF d_pos = 1.0/6.0 * (k1.dpos + 2*(k2.dpos + k3.dpos) + k4.dpos);
    QPointF d_vel = 1.0/6.0 * (k1.dvel + 2*(k2.dvel + k3.dvel) + k4.dvel);

    _pos += d_pos*dt;
    _vel += d_vel*dt;
}