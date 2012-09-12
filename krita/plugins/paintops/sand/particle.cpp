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
Particle::Particle( bool life,
                    float mass,
                    float force,
                    int radius,
                    int lifespan,
                    float friction,
                    float dissipation, //dissipation constant
                    QPoint * position,
                    QPointF * velocity,
                    QPointF * acceleration,
                    KoColor * color
                  )
{
    _life   = life;
    _mass   = mass;
    _radius =  radius;
    _lifespan = lifespan;
    _friction = friction;
    _dissipation = dissipation;
    _pos = position;
    _vel = velocity;
    _accel = acceleration;
    _force = force;
    _forceVec = new QPointF(0.0,0.0);
    _old = new QPoint(0,0);
    _color = color;
}

///Copy constructor
Particle::Particle(const Particle & p)
{
    _force  = p.force();
    _life   = p.isAlive();
    _mass   = p.mass();
    _radius =  p.radius();
    _lifespan = p.lifespan();
    _friction = p.friction();
    _dissipation = p.dissipation();
    _pos = p.pos();
    _old = p.old();
    _vel = p.vel();
    _accel = p.accel();
    _forceVec = p.forceVec();
    _color = p.color();
}

//Methods for particle dynamics

void Particle::applyForce(QPointF &pos, QPointF &vel, const SandProperties * properties, int width, int height, int dt)
{
    /*
     * xi calculation is incomplete. It should be like this:
     *
     * dist_x = _position->x() - info.pos().x();   //distance between particle centers: X
     * dist_y = _position->y() - info.pos().x();   //distance between particle centers: Y
     * dist_centers = sqrt(dist_x^2 + dist_y^2);        //distance norm
     * float xi = _radius + m_properties->radius - dist_centers;
     *
     */

    float dist_x = (pos.x() - _pos->x());   //distance between particle centers: X
    float dist_y = (pos.y() - _pos->y());   //distance between particle centers: Y

     
    float dist_centers = sqrt(dist_x*dist_x + dist_y*dist_y);        //distance norm
    float xi = float(_radius + properties->radius) - dist_centers;

    //No collision? Exit function
    if( xi <= 0){
        return;
    }
    
    /*
     * normalized young modulus between the MOUSE (brush) particle and this particle
     */
    
    float Y = float(_friction * properties->b_friction) / float(_friction + properties->b_friction);
    
    /*
     * dissipative constant: function of the material viscosity
     * Maybe it won't be needed to set this for the MOUSE (BRUSH) PARTICLE
     */
    
    float A = 0.5*( _dissipation + 0.5 );
    
    float reff = ( float(_radius * properties->radius) )/( float( _radius + properties->radius) );


   /*
    * FOR THE VELOCITY DERIVATION:
    * Same as above: we need the velocity of the  MOUSE particle:
    */

   //obs
    float dvx = vel.x() - _vel->x();
    float dvy = vel.y() - _vel->y();
    
    float rr_rez = 1 /dist_centers;
    float ex = dist_x * rr_rez;
    float ey = dist_y * rr_rez;

    //Xi derivative (velocity based)
    float xidot = - (ex*dvx + ey*dvy);

//     qDebug() << "| e ; "  << ex << ", " << ey
//              << "| dv ; " << dvx << ", " << dvy
//              << "| dist ; " << dist_x << ", " << dist_y;

    //NORMAL FORCE ON THIS PARTICLE
    float fn = sqrt(xi)*Y*sqrt(reff)*(xi+A*xidot); //this is the original formula with the dissipation constant

    //Update forces
    
    _force += fn;

    _forceVec->rx() =  fn*ex;
    _forceVec->ry() =  fn*ey;

    //impulse
    _vel->rx() = _forceVec->x()*dt;
    _vel->ry() = _forceVec->y()*dt;

}

Derivative Particle::eval(const State & init, double dt, const Derivative & der)
{
    
    State st;
    st.pos.rx() = init.pos.x() + der.dpos.x()*dt;            //apply the position modification
    st.pos.ry() = init.pos.y() + der.dpos.y()*dt;

    st.vel.rx() = init.vel.x() + der.dvel.x()*dt;            //apply the velocity modification
    st.vel.ry() = init.vel.y() + der.dvel.y()*dt;

    Derivative out;                             //output derivative

    out.dpos.rx() = st.vel.x();                          //derivative velocity
    out.dpos.ry() = st.vel.y();

    out.dvel = accel(st, dt);                   //derivative acceleration

    return out;
}

/**
 * \attention This method should be tunned. The dumping it's behaving in a strange mode. Instead
 * of go to zero, it keeps to a negative value. Should fix this!!!
 */
QPointF Particle::accel(const State &state, double t)
{
    const float k = 0.1; //put this as a property in the brush configuration
    const float b = _mass;
    
    //inear momentum dumping (due to friction)
    QPointF result (qreal( -k*state.vel.x()/b ),
                    qreal( -k*state.vel.y()/b ) );
    return result;
}

void Particle::integrationStep(double dt, int width, int height)
{
    
    float p_length = sqrt(_pos->x()*_pos->x() + _pos->y()*_pos->y());

   //this particle current state
    State state;
    state.pos.setX( _pos->x()/p_length);
    state.pos.setY( _pos->y()/p_length);
    state.vel.setX( _vel->x());
    state.vel.setY( _vel->y());

   //derivatives for the RK terms

    Derivative k1 = eval(state, 0.0, Derivative());
    Derivative k2 = eval(state, dt*0.5, k1);
    Derivative k3 = eval(state, dt*0.5, k2);
    Derivative k4 = eval(state, dt, k3);

    float pos_x = (1.0/6.0 * (k1.dpos.x() + 2*(k2.dpos.x() + k3.dpos.x()) + k4.dpos.x()))*dt;
    float pos_y = (1.0/6.0 * (k1.dpos.y() + 2*(k2.dpos.y() + k3.dpos.y()) + k4.dpos.y()))*dt;
    float vel_x = (1.0/6.0 * (k1.dvel.x() + 2*(k2.dvel.x() + k3.dvel.x()) + k4.dvel.x()))*dt;
    float vel_y = (1.0/6.0 * (k1.dvel.y() + 2*(k2.dvel.y() + k3.dvel.y()) + k4.dvel.y()))*dt;

    float newPos_x = _pos->x() + pos_x;
    float newPos_y = _pos->y() + pos_y;

//     qDebug() << "nPos " << newPos_x << ", " << newPos_y;
    
    if( sqrt(newPos_x * newPos_x) < width && sqrt(newPos_y * newPos_y) < height ){
        _old->rx() = _pos->x();
        _old->ry() = _pos->y();
        _pos->rx() = newPos_x;
        _pos->ry() = newPos_y;
        _vel->rx() = vel_x;
        _vel->ry() = vel_y;
    }

}

/*
 * Serialization operation overload
 */

QDataStream &operator<<(QDataStream &out, const Particle &particle)
{   
    out << particle.isAlive()
        << quint32(particle.lifespan())
        << particle.mass()
        << particle.force()
        << quint32(particle.radius())
        << particle.friction()
        << particle.dissipation()
        << *particle.pos()
        << *particle.vel()
        << *particle.accel();
        
    return out;
}

QDataStream &operator>>(QDataStream &in, Particle &particle)
{
    bool life;
    float mass;
    float force;
    int radius;
    int lifespan;
    float friction;
    float dissipation;
    QPoint   pos;
    QPointF  vel;
    QPointF  accel;

    in >> life >> lifespan >> mass
       >> force >> radius >> friction
       >> dissipation >> pos >> vel >> accel;
    
    particle.setLife(life);
    particle.setMass(mass);
    particle.setForce(force);
    particle.setRadius(radius);
    particle.setLifespan(lifespan);
    particle.setFriction(friction);
    particle.setDissipation(dissipation);
    particle.setPos(new QPoint(pos));
    particle.setVel(new QPointF(vel));
    particle.setAccel(new QPointF(accel));

    return in;
}

