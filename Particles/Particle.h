//This file is part of 1m Particles(GNU GPL) - https://www.youtube.com/user/Muzkaw.

#pragma once

#include <SFML/Graphics.hpp>

using namespace std ;
using namespace sf ;

class Particle // class that manages particles following the Newton's law
{
public:

	Particle() ;
	Particle(float mass, Vector2f position, Vector2f speed) ;

	float getMass() const ;
	void setMass(float const& mass) ;
	vector<Vector2f> getForce() const;
	Vector2f getTotalForce() const ;
	void addForce(Vector2f const& force) ;
	void clearForce() ;
	Vector2f getPosition() const ;
	void setPosition(Vector2f const& position) ;
	Vector2f getSpeed() const ;
	void setSpeed(Vector2f const& speed) ;
	Vector2f getAcceleration() const ;
	void setAcceleration(Vector2f const& acceleration) ;
	void setStatic() ;
	void setMovable() ;
	bool isMovable() const ;

	void updatePosition(float const& dt) ; // the only important method in this class !
	
private:
	float m_mass ;

	vector<Vector2f> m_force ; // forces applied to this particle
	Vector2f m_position ; // position of the particle
	Vector2f m_speed ; // speed of the particle : speed v= ( x(t+dt) - x(t) ) / dt is the finite manner to say v=dx/dt
	Vector2f m_acceleration ; // acceleration of the particle : acceleration a= ( v(t+dt) - v(t) ) / dt is the finite manner to say a=dv/dt

	bool m_movable ;
};
