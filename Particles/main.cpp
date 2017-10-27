/*********************************************************************

This file is part of 1m Particles (GNU GPL) - https://www.youtube.com/user/Muzkaw).

1m Particles is free software : you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

1m Particles is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with 1m Particles. If not, see <http://www.gnu.org/licenses/>.

1m Particles codded by Muzkaw : https://www.youtube.com/user/Muzkaw
You need to link SFML 2.0 or above to run the program

********************************************************************/

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include "Particle.h"
#include "Utility.h"
#include <math.h>

#define GRAVITY 2000 //some constants I need
#define DRAG 0

#define RESTITUTION_COEFFICIENT 1 // how much energy must be absorbed when bouncing off a wall
#define INITIAL_DISTANCE 0.5 // how far particles are one another initially

#define MOUSE_FORCE -20000000

#define CHUNK_NB 10 // separating particles in smaller chunks to avoid having HUGE arrays (1 billion particles) : else we might face stack overflow or framerate drops. To understand the code faster, you can think that CHUNK_NB = 1
#define VERTEX_CHUNK 100000 // how much particles are in each chunk

using namespace std ;
using namespace sf ;

int main()
{
	srand(time(NULL));

	int width = 1920 ; // window definition
	int height = 1080 ;
	Window window(sf::VideoMode(width, height, 32), "OpenGL particles");
	window.setFramerateLimit(60) ;

	glViewport(0,0,width,height) ; // viewport definition
	glMatrixMode(GL_PROJECTION) ;
	glLoadIdentity() ;
	glOrtho(0,width, height,0,-1,1) ;

	vector<Particle> particles ; // an array storing Particle instances (that we'll move)
	int particleSize = CHUNK_NB*VERTEX_CHUNK ; // avoid repeating particles.size() during the for loop to save some time (remember that the for loop is done 1 billion time per frame !
	
	for(int i(0) ; i < 1000 ; i++) // storing Particle instances in the particles array
	{
		for(int j(0) ; j < 1000 ; j++)
		{
			Particle particle ; // see Particle.h and Particle.cpp
			particle.setPosition( Vector2f(j*INITIAL_DISTANCE,i*INITIAL_DISTANCE) + Vector2f(20,20) ); // we place the particles in a square shape
			particle.setMass(10);
			particles.push_back(particle) ;
		}
	}

	Clock deltaTime ; // SFML clock to keep track of our framerate
	float dt=0.00000001 ; // the time a frame requires to be drawn (=1/framerate). I set it at 0.0000001 to avoid getting undefined behaviors during the very first frame
	Vector2f previousPosition ; // a SFML Vector2f to check whether a particle passed a wall during this frame or not
		
	vector<Vector2f> wallPoints ; // an array of points that represent my 4 walls that'll block my particles if I want to
	wallPoints.push_back(Vector2f(10,10)) ;
	wallPoints.push_back(Vector2f(11,height-10)) ;

	wallPoints.push_back(Vector2f(11,height-10)) ;
	wallPoints.push_back(Vector2f(width-10,height-10)) ;

	wallPoints.push_back(Vector2f(width-10,height-10)) ;
	wallPoints.push_back(Vector2f(width-11,10)) ;

	wallPoints.push_back(Vector2f(width-11,10)) ;
	wallPoints.push_back(Vector2f(10,10)) ;

	float vertexCoords[CHUNK_NB][2*VERTEX_CHUNK] ; // OpenGL requires a static array of floats to represent coords : [x1][y1][x2][y2]...[xn][yn] while I use Vector2f to manipulate my particles : I need to convert that
	
	unsigned char colors[CHUNK_NB][3*VERTEX_CHUNK] ; // OpenGL requires a static array to represent the colors of the dots : [r1][g1][b1][r2][g2][b2]...[rn][gn][bn]
	
	glEnable( GL_POINT_SMOOTH ); // allow to have rounded dots
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glPointSize( 1 );

	bool LMB = false ; // is left mouse button hit ?
	float zoom = 1 ; // zoom factor controled by Z and S keys
	Vector2f camPos(0,0) ; // camera position controled with arrow keys

	while(window.isOpen()) // main loop, each time this loop is finished, we produce a new frame (so this while loop must run at least 20 times per seconds)
	{
		Event event; // Check if the user wants to close the window
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			window.close();
		}	

		glClearColor(0,0,0,0); // we clear the screen with black (else, frames would overlay...)
		glClear(GL_COLOR_BUFFER_BIT); // clear the buffer

		// CONTROLS (click, zoom, scroll) ////////////////////

		if(Mouse::isButtonPressed(Mouse::Left)) 
			LMB = true ;
		else
			LMB = false ;

		if(Keyboard::isKeyPressed(Keyboard::Z))
			zoom+=1*dt*zoom ;
		else if(Keyboard::isKeyPressed(Keyboard::S))
			zoom-=1*dt*zoom ;
		if(Keyboard::isKeyPressed(Keyboard::Left))
			camPos.x+=500*dt/zoom ;
		else if(Keyboard::isKeyPressed(Keyboard::Right))
			camPos.x-=500*dt/zoom ;
		if(Keyboard::isKeyPressed(Keyboard::Up))
			camPos.y+=500*dt/zoom ;
		else if(Keyboard::isKeyPressed(Keyboard::Down))
			camPos.y-=500*dt/zoom ;

		Vector2f mousePos = (Vector2f(Mouse::getPosition(window).x,Mouse::getPosition(window).y)/zoom-Vector2f(width/2,height/2)/zoom-camPos) ; // we store the current mouse position in this variable

		for(int i(0) ; i < particleSize ; i++) // now, each frame, we want to update each particle's position according to the newton's law, color according to its speed, and maybe make it collide with walls (this for loop is executed 1 billion times each frame)
		{
			//particles[i].addForce(Vector2f(0,GRAVITY)) ; // example for adding gravity force
			particles[i].addForce( (Vector2f(mousePos - particles[i].getPosition()) * (float)(LMB*500000/pow(Distance(mousePos , particles[i].getPosition())+10,2 )))) ; // if the user clicks we add a force proportionnal to the inverse of the distance squared
			particles[i].addForce(-particles[i].getSpeed()*(float)DRAG) ; // we add a drag force proportionnal to the speed
			
			//previousPosition = particles[i].getPosition() ; // uncomment this line if you want to perform collision detection
			particles[i].updatePosition(dt) ; // we update the position of the particle according to the Newton's law (see Particle.h and Particle.cpp)
			
			particles[i].clearForce() ; // we don't want forces to add over time so we clear them before adding them the next frame

			/*for(int j(0) ; j < wallPoints.size() ; j+=2) // uncomment these lines if you want to perform collision detection
			{
				if(determinant(wallPoints[j+1] - wallPoints[j], wallPoints[j+1]-particles[i].getPosition())*determinant(wallPoints[j+1] - wallPoints[j], wallPoints[j+1]-previousPosition)<0) // if we crossed a wall during this frame
				{
					Vector2f v = wallPoints[j+1] - wallPoints[j] ; // vector directing the wall
					Vector2f n = Vector2f(-v.y,v.x) ; // vector normal to the wall        
					n/=Norm(n) ; // we want the normal vector to be a unit vector (length = 1)
					particles[i].setPosition(previousPosition) ; // we put the particle in its previous position (in front of the wall, since it passed it)
					float j = -(1+RESTITUTION_COEFFICIENT)*dotProduct(particles[i].getSpeed(), n) ; // we compute the speed after bouncing off

					particles[i].setSpeed(particles[i].getSpeed() + j*n) ; // we change the speed
				}
			}*/
			
		}

		for(int j(0) ; j < CHUNK_NB ; j++) // we convert Vector2f positions to the OpenGL's way of storing positions : static arrays of floats
		{
			for(int i(0) ; i < VERTEX_CHUNK ; i++)
			{
				colors[j][3*i] = 255 ;
				colors[j][3*i+1] = clamp(255-Norm(particles[i + j*VERTEX_CHUNK].getSpeed()),0,255) ; // we change the particle's colors according to their speed
				colors[j][3*i+2]= 0 ;
			
				vertexCoords[j][2*i] = particles[i + j*VERTEX_CHUNK].getPosition().x ;
				vertexCoords[j][2*i+1] = particles[i + j*VERTEX_CHUNK].getPosition().y ;
			}
		}

		glPushMatrix() ; // time to draw the frame

		glTranslatef(width/2.f,height/2.f,0) ; // apply zoom
		glScaled(zoom, zoom, zoom);
				
		glTranslated(camPos.x,camPos.y,0) ; // apply scroll
		
		glEnableClientState(GL_VERTEX_ARRAY) ; // we are using VBAs : here's how to draw them
		glEnableClientState(GL_COLOR_ARRAY) ;

		for(int j(0) ; j < CHUNK_NB ; j++)
		{
			glVertexPointer(2,GL_FLOAT,0,vertexCoords[j]) ;
			glColorPointer(3,GL_UNSIGNED_BYTE,0,colors[j]) ;
			glDrawArrays(GL_POINTS,0,VERTEX_CHUNK) ;
		}

		glDisableClientState(GL_VERTEX_ARRAY) ;
		glDisableClientState(GL_COLOR_ARRAY) ;

		glPopMatrix();
				
		glFlush();
		window.display() ; // show the window with its frame drawn in it

		dt = deltaTime.restart().asSeconds() ; // measure the time it took to complete all the calculations for this particular frame (dt=1/framerate) 
	}

	return 0 ;
}