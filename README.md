Flocking simulation
=====

Flocking simulation, with so many cores. 

Deploy 
===
Ansible => https://github.com/ansible/ansible

Requirements:
===
GCC 4.8.2
MPI 1.8.3
Log4cpp

Optional:
CUDA Toolkit 6.5
Glew, Glut, Qt4
QGLViewer

Initial code
===
Sequential flocking simulation meant to be a starting code for parallel  
programming class using MPI, OpenMP or CUDA/OpenCL.

The program is launch using the command
  ./boids

The program may take several options
  -agents number of particles in the simulation
  -steps number of time steps in the simulation
  -wa weight for alignment force between particles
  -ws weight for separation force between particles
  -wc weight for cohesion force between particles
  -ra cutoff radius for alignment force between particles
  -rs cutoff radius for separation force between particles
  -rc cutoff radius for cohesion force between particles

The results are saved every 20 time steps into the file boids.xyz
The file may be visualized in gnuplot using the command

splot 'boids.xyz'  every :::k::k pt 7 lc 3

where k is the iteration you want to visualize
