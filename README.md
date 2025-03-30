# University Projects

This is a collection of projects and assignments I did while getting my CS bachelor's degree at the University of Manitoba

---
### Computer Security
#### [Encryption and Decryption](./Computer%20Security/Encryption%20and%20Decryption/README.md)

A comparison of AES-ECB and AES-CBC encryption modes, and implementation of a key generation algortihm for symmetric encryption/decryption.
#### [Buffer Overflow Attack](./Computer%20Security/Buffer%20Overflow%20Attack/README.md)

A writeup about performing a buffer oveflow attack on a program to achive a root shell.
#### [Environment Variable Attack](./Computer%20Security/Environment%20Variable%20Attack/README.md)

A writeup about modifying a system's `PATH` environment variables to execute arbitrary malicious code.
#### [SQL Injection](./Computer%20Security/SQL%20Injection/README.md)

A basic SQL injection attack on an example website.
#### [SYN Flooding Attack](./Computer%20Security/SYN%20Flooding%20Attack/README.md)

A DOS attack performed using invalid SYN packets to spoof false incoming TCP connections.

---
### Operating Systems
#### [ELF File Reader](./Operating%20Systems/ELF%20File%20Reader/README.md)

A custom program designed to read and interpret ELF formatted binary files.
#### [Threads and Processes Management](./Operating%20Systems/Threads%20and%20Processes%20Management/README.md)

Two programs simulating a handler that manages threads/processes through signals.
#### [Custom Shell](./Operating%20Systems/Custom%20Shell/README.md)

A very basic custom shell that can execute other programs and perform basic redirects, piping, and process substitution.
#### [EXFAT File Reader](./Operating%20Systems/EXFAT%20File%20Reader/README.md)

A program that reads any `.exfat` formatted files that are disk images of EXFAT formated file systems.
#### [EXFAT Volume Checker](./Operating%20Systems/EXFAT%20Volume%20Checker/README.md)

A program that checks EXFAT formated disk images and validates the MBR and file systems consistency.
#### [Threads and Locking](./Operating%20Systems/Threads%20and%20Locking/README.md)

An implementation and comparison of various locking techniques used with multithreading.
#### [CPU Task Management Simulation](./Operating%20Systems/CPU%20Task%20Management%20Simulation/README.md)

A simulation of a CPU executing and switching between various tasks, organized via a multi-level feedback queue.

---

### Machine Learning
#### [K-Step Ahead Forecasting](./Machine%20Learning/K-Step%20Ahead%20Forecasting.ipynb)

A custom implementation of an echo state neural network to perform k-step ahead forecasting on `sin` and `lorenz` functions.

---

### Mathematics of Data Science
A collection of data science projects done in R. The links take you to the `.Rmd` files, but there's also knitted `.html` files that contain the execution results.
#### [Line of Best Fit](./Mathematics%20of%20Data%20Science/Line%20of%20Best%20Fit.Rmd)

Creating a line of best fit to predict future country population, optimizing line hyperparameters through a genetic algorithm and error function.

#### [Least Squares Problem](./Mathematics%20of%20Data%20Science/Least%20Squares%20Problem.Rmd)

Implementing Least Squares problem solver, along with some examples of it running.

#### [Gram-Schmidt and QR Factorization](./Mathematics%20of%20Data%20Science/Gram-Schmidt%20and%20QR%20Factorization.Rmd)

Performing QR Factorization using the Gram-Schmidt process, along with some examples.
#### [Singular Value Decomposition and Pseudo-Inverse](./Mathematics%20of%20Data%20Science/Singular%20Value%20Decomposition%20and%20Pseudo-Inverse.Rmd)

#### [Country Data Analysis](./Mathematics%20of%20Data%20Science/Country%20Data%20Analysis.Rmd)

Performing preliminary data cleaning along with a mathematical analysis on country data.

#### [Gershgorin Disks](./Mathematics%20of%20Data%20Science/Gershgorin%20Disks.Rmd)

Implement functions to calculate Greshgorin Disks and plot them on a cartesian plane.

#### [Graph Basics](./Mathematics%20of%20Data%20Science/Graph%20Basics.Rmd)

Some very simple graph functions just to re-familarize myself with basic graphs.

#### [Custom Dijkstras Algorithm](./Mathematics%20of%20Data%20Science/Custom%20Dijkstras%20Algorithm.Rmd)

An implementation of Dijkstras path finding algorithm.

#### [Graph Analysis with Writeup](./Mathematics%20of%20Data%20Science/Graph%20Analysis%20with%20Writeup.Rmd)

A mathematical and visual analysis of a graph representing the relationships between members of a university faculty.

#### [Markov Chains](./Mathematics%20of%20Data%20Science/Markov-Chains.Rmd)

Custom markov chain functions along with computing absorbing states and graph regularity.

---

### Computer Graphics

All computer graphics implemented in processing.

#### [Lines, Triangles, and Surface Tesselation](./Computer%20Graphics/SurfaceTesselation/)

Using only the built in method for setting an individual pixel to a color I implemented:
- Bresenham's Line algorithm to draw a line from one pixel to another.
- Used Bresenham's Line algorithm to draw a 2D triangle projection of a rotating triangle in 3D space.
- Used 2D triangle projections to tesselate the surface of a 3D rotating sphere.
- Implemented several lighting models for triangles to shade the sphere.

The following are controls used when running the code:
- `O`: toggles outlines.
- `Space`: toggles rotation.
- `D` and `E`: next and previous display respectively.
- `S` and `W`: next and previous shading mode respectively.

#### [Raytracer](./Computer%20Graphics/Raytracing/)

Using only the built in method for setting an individual pixel's color I implemented:
- A raytracer with Phong lighting
- Classes that calculate surface normals, ray intersects and reflections for spheres, circles, infinite planes, and infinite cylinders
- The ability for some surfaces to be mirrors

Whenever you press any key, the raytracer will switch between scenes

#### [Graphics Pipeline](./Computer%20Graphics/GraphicsPipeline/)

Implementation of a 2D/3D graphics transformation pipline with the following features:
- A matrix stack containing transformation matrices: Model, Camera, Projection, and Viewport
- Functions to manipulate the matrix stack
- Functions to apply transformations to the matrix stack
- A basic 2D scene constructed using custom transformation functions
- A basic animated 3D scene that extrapolates the previous 2D implementation of the graphics pipeline to 3D



The following controls are used when running the code:
- `[`: Rotate Clockwise
- `]`: Rotate Counter-Clockwise
- `=`: Zoom in
- `-`: Zoom out
- `O`: Switch between the following orthographic projection modes
    - Indentity: Projection, Camera and Model matrices are all identity
    - Center600: Centered on `(0,0)` with the side boundaries in the range `[-300,300]`
    - TopRight600: Bottom left is `(0,0)`, top right is `(600,600)`
    - FlipX: Same as Center600, but flipped through the Y-Axis
    - Aspect: Uneven aspect ratio, X-Range is `(-300,300)`, Y-Range is `(-100,100)`
- `D`: Switch between the following display modes
    - Pattern: A multicolored grid pattern
    - Scene: A basic drawing made using the custom graphics pipeline and functions for rotation, translation, etc.
    - Shapes: An animated 3D scene featuring squares moving away from the camera utilizing orthographic projection.

#### [Space Game](./Computer%20Graphics/BasicGame/)

A basic space game built using a particle system and basic texturing
Features:
- Basic scoring system and gameplay with `WASD` and `Space`
- Particle based animations for spaceship death debris and bullet trails
- Frame based animation for thruster flames and death explosion
- Particle system to keep track of playter, enemies, and bullets, along with collision detection
- Basic physics based movement for enemies and player

---

### Distributed Computing

#### [Distributed Database](./Distributed%20Computing/Distributed%20Database)

A distributed system that is designed to maintain consistency in a mock database across multiple workers through use of a central coordinator

#### [Custom API Webserver](./Distributed%20Computing/Webserver/)

A python based server that has both a website accessible in the browser along with a `C` client that tests it's API

#### [Custom Blockchain](/Distributed%20Computing/Blockchain/)

A blockchain client for a classroom run blockchain built with it's own custom network protocol

---

## TODO:
Comp 3380 project

DB2 query optimization?

Link to study buddy


research paper python code?

finish this readme, and add readmes for class folders(?)

comp 2280 assemby??

comp 2160 c??