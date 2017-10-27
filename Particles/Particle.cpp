//This file is part of 1m Particles(GNU GPL) - https://www.youtube.com/user/Muzkaw.

#include "Particle.h"

Particle::Particle()
{
	m_mass = 50 ;
	m_position = Vector2f(50,50) ;
	m_speed = Vector2f(0,0) ;
	m_acceleration = Vector2f(0,0) ;
	m_movable = true ;
}
Particle::Particle(float mass, Vector2f position, Vector2f speed)
{
	m_mass = mass ;
	m_position = position ;
	m_speed = speed ;
}

float Particle::getMass() const
{
	return m_mass ;
}
void Particle::setMass(float const& mass)
{
	m_mass=mass ;
}

vector<Vector2f> Particle::getForce() const
{
	return m_force ;
}
Vector2f Particle::getTotalForce() const
{
	Vector2f totalForce(0,0) ;
	for(int i(0) ; i < m_force.size() ; i++)
	{
		totalForce += m_force[i] ;
	}
	return totalForce ;
}
void Particle::addForce(Vector2f const& force)
{
	m_force.push_back(force) ;
}
void Particle::clearForce()
{
	m_force.clear() ;
}

Vector2f Particle::getPosition() const
{
	return m_position ;
}
void Particle::setPosition(Vector2f const& position)
{
	m_position = position ;
}

Vector2f Particle::getSpeed() const
{
	return m_speed ;
}
void Particle::setSpeed(Vector2f const& speed)
{
	m_speed = speed ;
}

Vector2f Particle::getAcceleration() const
{
	return m_acceleration ;
}
void Particle::setAcceleration(Vector2f const& acceleration)
{
	m_acceleration = acceleration ;
}
void Particle::setStatic()
{
	m_movable = false ;
}
void Particle::setMovable()
{
	m_movable = true ;
}
bool Particle::isMovable() const
{
	return m_movable ;
}

void Particle::updatePosition(float const& dt)
{
	Vector2f previousPosition = m_position ; // if x is the position : we store x(t)
	/*	
		Newton's law states : F=m*a, that is to say a=F/m
		we integrate this equation with the following approximation : between each frame, the acceleration is constant (it's nearly true)
		Notation : x(t) is the position during the current frame, x(t+dt) is the position we want to find for the next frame. Same for v(t)...
		so we have v(t+dt)=(F/m)*dt + v(t) -> if we differentiate this equation, we indeed find back a=F/m
		we integrate again the equation above considering that v is constant aswell
		thus we get x(t+dt) = (1/2)*(F/m)*dt^2 + v(t)*dt + x(t) -> knowing F, m, dt, the speed of the particle and its position, we can guess its next position !
		this is known as the leapfrog numerical integration. I think that it is equivalent to the Runge Kutta order 2 (RK2)
	*/
	m_position = m_position + m_speed*dt + 0.5f*getTotalForce()/m_mass*(float)pow(dt,2); // the equation explained above
	m_speed = (m_position-previousPosition)/dt ; //now we need to update the speed by differentiating the position. (now we have the new position and the new speed : the particle moves !)
}