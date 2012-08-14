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
                    QPointF * acceleration
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
}

//Methods of the particle dynamics

void Particle::applyForce(QPointF &pos, QPointF &vel, const SandProperties * properties, int width, int height)
{
//     qDebug() << "bf. _force :" << _force;

    /*
     * xi calculation is incomplete. It should be like this:
     *
     * dist_x = _position->x() - info.pos().x();   //distance between particle centers: X
     * dist_y = _position->y() - info.pos().x();   //distance between particle centers: Y
     * dist_centers = sqrt(dist_x^2 + dist_y^2);        //distance norm
     * float xi = _radius + m_properties->radius - dist_centers;
     *
     */
//    qDebug() << " _pos :" << *_pos << " radius : " << _radius ;
    float dist_x = (_pos->x() - pos.x())/width;   //distance between particle centers: X
    float dist_y = (_pos->y() - pos.y())/height;   //distance between particle centers: Y

     
    float dist_centers = sqrt(dist_x*dist_x + dist_y*dist_y);        //distance norm
    float xi = float(_radius + properties->radius) - dist_centers;

    //No collision? Exit function
    if( xi <= 0)
        return;

    
    /*
     * normalized young modulus between the MOUSE (brush) particle and this particle
     */
    
    float Y = float(_friction * properties->friction) / float(_friction + properties->friction);
    /*
     * dissipative constant: function of the material viscosity
     * Maybe it won't be needed to set this for the MOUSE (BRUSH) PARTICLE
     */
    
//     float A =  _dissipation;
    float A = 0.5*( _dissipation + 0.5 );

    float reff = ( float(_radius * properties->radius) )/( float( _radius + properties->radius) );
//     qDebug() << "reff = ( " << _radius << " * " << properties->radius << " )/( " << _radius << " + " << properties->radius << ") = " << reff;

   /*
    * FOR THE VELOCITY DERIVATION. Same as above: we need the velocity of the
    * MOUSE particle:
    *
    */

    float dvx = _vel->x() - vel.x();
    float dvy = _vel->y() - vel.y();
//     qDebug() << "vel :" << *_vel;

    float rr_rez = 1 /dist_centers;

    float ex = dist_x * rr_rez;
    float ey = dist_y * rr_rez;

    //Xi derivative (velocity based)
    float xidot = - (ex*dvx + ey*dvy);

    //NORMAL FORCE ON THIS PARTICLE
    float fn = sqrt(xi)*Y*sqrt(reff)*(xi+A*xidot); //this is the original formula with the dissipation constant
//     float fn = sqrt(xi)*Y*sqrt(reff)*(xi+xidot);
//     qDebug() << _force << " + fn = sqrt( " << xi << " )* " << Y <<" *sqrt( " << reff << " )*( " << xi << " + "<< A << "*" << xidot << ") = " << fn;
//     qDebug() << "ex, ey : " << ex << ", " << ey;
    //PERHAPS I HAVE TO MODIFY THIS, SINCE WE CAN HAVE A FORCE IN A DIFFERENT DIRECTION OF THE MOVEMENT
//     if( fn< 0)
//         fn = 0;
//     else{
        _force += fn;
//         _forceVec->rx() = _forceVec->rx() + fn*ex;
//         _forceVec->ry() = _forceVec->ry() + fn*ey;
        _forceVec->rx() =  fn*ex;
        _forceVec->ry() =  fn*ey;
//     }

//     qDebug() << "af. _force : " << *_forceVec;
}

Derivative Particle::eval(const State & init, double dt, const Derivative & der)
{
    
    State st;
    st.pos.rx() = init.pos.x() + der.dpos.x()*dt;            //apply the position modification
    st.pos.ry() = init.pos.y() + der.dpos.y()*dt;

    st.vel.rx() = init.pos.x() + der.dvel.x()*dt;            //apply the velocity modification
    st.vel.ry() = init.pos.y() + der.dvel.y()*dt;

    Derivative out;                             //output derivative

    out.dpos.rx() = st.vel.x();                          //derivative velocity
    out.dpos.ry() = st.vel.y();

//     qDebug() << "end_pos "  << st.pos.x() << ", "<< st.pos.y();
//     qDebug() << "end_vel "  << st.vel.x() << ", "<< st.vel.y();
    out.dvel = accel(st, dt);                   //derivative acceleration

    return out;
}

/**
 * \attention This method should be tunned. The dumping it's behaving in a strange mode. Instead
 * of go to zero, it keeps to a negative value. Should fix this!!!
 */
QPointF Particle::accel(const State &state, double t)
{
    const float k = 0.1; //dumping, provavelmente usar a "dissipation"
    const float b = _mass;
    //Acceleration due to force minus linear momentum dumping (due to friction)
    QPointF result ( qreal( _forceVec->x()/_mass + (-k * state.pos.x() - b*state.vel.x())),
                     qreal( _forceVec->y()/_mass + (-k * state.pos.y() - b*state.vel.y())));

    return result;
}

void Particle::integrationStep(double dt, int width, int height)
{
//     qDebug() << "bef.: pos :" << *_pos << " vel :" << *_vel ;
    
    float p_length = sqrt(_pos->x()*_pos->x() + _pos->y()*_pos->y());

    float v_length = sqrt(_vel->x()*_vel->x() + _vel->y()*_vel->y());

    //gambiarra... tenho q ver uma melhor maneira de assegurar a normalizacao
//     if(p_length == 0)
//         p_length = 1;
    if(v_length == 0.0)
        v_length = 1.0;

//    //this particle current state
    State state;
    state.pos.setX( _pos->x()/p_length);
    state.pos.setY( _pos->y()/p_length);
    state.vel.setX( _vel->x()/v_length);
    state.vel.setY( _vel->y()/v_length);


//    //derivatives for the RK terms

    Derivative k1 = eval(state, 0.0, Derivative());
    Derivative k2 = eval(state, dt*0.5, k1);
    Derivative k3 = eval(state, dt*0.5, k2);
    Derivative k4 = eval(state, dt, k3);

////     QPointF d_pos = 1.0/6.0 * (k1.dpos + 2*(k2.dpos + k3.dpos) + k4.dpos);
////     QPointF d_vel = 1.0/6.0 * (k1.dvel + 2*(k2.dvel + k3.dvel) + k4.dvel);
    float pos_x = (1.0/6.0 * (k1.dpos.x() + 2*(k2.dpos.x() + k3.dpos.x()) + k4.dpos.x()))*dt;
    float pos_y = (1.0/6.0 * (k1.dpos.y() + 2*(k2.dpos.y() + k3.dpos.y()) + k4.dpos.y()))*dt;
    float vel_x = (1.0/6.0 * (k1.dvel.x() + 2*(k2.dvel.x() + k3.dvel.x()) + k4.dvel.x()))*dt;
    float vel_y = (1.0/6.0 * (k1.dvel.y() + 2*(k2.dvel.y() + k3.dvel.y()) + k4.dvel.y()))*dt;

//     qDebug() << "vals. pos: " << pos_x << ", " << pos_y << " vel.: " << vel_x << ", " << vel_y;
    float newPos_x = _pos->x() + pos_x;
    float newPos_y = _pos->y() + pos_y;
    
    if( sqrt(newPos_x*newPos_x) < width && sqrt(newPos_y*newPos_y) < height ){
        _old->rx() = _pos->x();
        _old->ry() = _pos->y();
        _pos->rx() = newPos_x;
        _pos->ry() = newPos_y;
        _vel->rx() = vel_x;
        _vel->ry() = vel_y;
    }


//     qDebug() << "aft.: pos :" << *_pos << " vel :" << *_vel ;

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

