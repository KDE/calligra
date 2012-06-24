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
void Particle::applyForce()
{
    /*
     * Calculate the distance of between the particle centers
     */
    double dx = normalize( p1.x() - p2.x(), lx);
    double dy = normalize( p1.y() - p2.y(), ly);
    double rr = sqrt( dx*dx + dy*dy );

    /*
     * Particles radius
     */
    double r1 = p1.r();
    double r2 = p2.r();

    /*
     * Mutual compression of the particles:
     */
    double xi = r1 + r2 - rr;

    /*
     * Two particles are in contact if the sum of it's radius exceeds
     * the distance of their centers.
     * i.e. :
     *
     * xi > 0
     */
    if(xi>0){
//         cout << " The particles colide." << endl;

        /*
         * Material coefficients of the particles
         */

        //normal coefficients
        double Y = p1.Y * p2.Y / ( p1.Y + p2.Y);  // Young modulus
        double A = 0.5*( p1.A + p2.A );           //dissipative constant: function of the material viscosity

        //tangencial coefficients
        double mu = ( p1.mu < p2.mu ? p1.mu : p2.mu ); //friction parameter for Coulomb's friction law
        double gamma = ( p1.gamma < p2.gamma ? p1.gamma : p2.gamma ); //damping constant

//         cout << " Calculating the normal forces..." << endl;

        /**
         *
         * Normal force calculation terms for viscoelastic spheres
         *
         */

        /*
         * effective radius of colliding spheres
         */
        double reff = ( r1 * r2 )/( r1 + r2 );

        /*
         * velocity difference between the particles in the
         * x and y coordinates. Used to obtain the derivation
         * of the xi coefficient
         */
        double dvx = p1.vx() - p2.vx();
        double dvy = p1.vy() - p2.vy();

        /*
         * xi derivation :
         */

        // the derivation of the mutual compression coefficient (xi) means the
        // variation of this value. The variation of the xi it's due to the velocity
        // and the positions of the particle through the time. So this is why we use
        // the velocities in the x and y coordinates and the position through the time.

        double rr_rez = 1/rr;

        double ex = dx * rr_rez;
        double ey = dy * rr_rez;

        //first derivative of the xi coefficient
        double xidot = -( ex*dvx + ey*dvy);

        /*
         * Normal force : responsible for the translational aspect of the
         * particle motion.
         *
         * (?) : have to search for the reasons why the Poisson's ratio was removed .
         *
         */

        // Obs.: have to try to implement a more simple normal force equation, since
        // we don't need a accurate data simulation, just the visual aspects of
        // the viscoelastic particles simulation

        cout << "xi =  " << xi << endl;
        cout << "Y  =  " << Y << endl;
        cout << "reff =  " << reff << endl;
        cout << "A =  " << A << endl;
        cout << "xidot =  " << xidot << endl;
        double fn = sqrt(xi)*Y*sqrt(reff)*(xi+A*xidot);

//         cout << " Calculating the tangencial forces..." << endl;

        /**
         *
         * Tangencial force calculation terms
         *
         */

        // Relative velocity of the spheres : difference between the velocities
        // and the relative center positions
        double vtrel = -dvx*ey + dvy*ex + p1.omega()*p1.r() - p2.omega()*p2.r();

        /*
         * Tangencial force : responsible for the rotational aspect of the
         * particle motion
         */
        double ft = -gamma*vtrel;



        if(fn<0)
            fn = 0;

        /*
         * Coulomb's friction law : The shear force is limited by this law :
         *
         * |ft| <= mu|fn|
         *
         * for large relative velocity (vtrel) or small normal force
         * the tangencial force assume the value of the coefficient of
         * friction (mu) times the normal force
         *
         */
        if(ft < -mu*fn)
            ft = -mu*fn;

        if(ft > mu*fn)
            ft = mu*fn;

        /*
         * Update the particle forces for spherical shapes (_ptype == 0 )
         */
        if(p1.ptype()==0) {
            p1.add_force(Vector(fn*ex-ft*ey, fn*ey+ft*ex, r1*ft));
        }

        if(p2.ptype()==0) {
            p2.add_force(Vector(-fn*ex+ft*ey, -fn*ey-ft*ex, -r2*ft));
        }
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