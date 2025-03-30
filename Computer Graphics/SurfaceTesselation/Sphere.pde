class Sphere {
  int radius;
  
  int phiSteps; // number of steps/vertices when traversing sphere vertically from 0->pi
  
  int thetaSteps; // number of steps/vertices when traversing sphere horizontally from 0->2pi
  
  PVector[][] surfaceVertices = null; // [theta][phi]
  PVector[][] surfaceNorms = null; // one for each vertex
  
  Triangle[] surface = null;
  
  
  Sphere(int r, int lats, int lons){
    this.radius = r;
    this.phiSteps = lats; // 14 seems good
    this.thetaSteps = lons; // 28 seems good
    // decide size of this.surfaceVertices
    this.surfaceVertices = new PVector[this.thetaSteps+2][this.phiSteps+2];
    this.surfaceNorms = new PVector[this.thetaSteps+2][this.phiSteps+2];
  }
  
  // build PVector using: (r*sin(phi)cos(theta), r*cos(phi), r*sin(phi)sin(theta))
  PVector buildVertex(float theta, float phi){
    return new PVector(this.radius*sin(phi)*cos(theta), this.radius*cos(phi), this.radius*sin(phi)*sin(theta));
  }
  
  // build PVector using: (sin(φ) cos(θ), cos(φ), sin(φ) sin(θ))
  PVector buildNorm(float theta, float phi){
    return new PVector(sin(phi)*cos(theta), cos(phi), sin(phi)*sin(theta));
  }
  
  // create all the vertices for use in surface
  // sphere: x^2 + y^2 + z^2 = r^2
  // alternatively, using phi and theta:
  // a point on the sphere has the coords: (r*sin(phi)cos(theta), r*cos(phi), r*sin(phi)sin(theta))
  // want to loop from 'North' pole to 'South' pole to create a single arc
  // then rotate horizontally a step and repeat until you reach the start
  void buildVertices(){
    // horizontal loop
    float currentTheta = 0;
    float thetaStep = TWO_PI / (float)this.thetaSteps;
    println("thetaStep: PI/" + thetaSteps);
    float phiStep = PI / (float)this.phiSteps;
    println("phiStep: PI/" + phiSteps);
    int thetaIndex = 0;
    while(currentTheta <= TWO_PI + thetaStep){
      // vertical loop
      float currentPhi = 0;
      int phiIndex = 0;
      while(currentPhi <= PI + phiStep){
        this.surfaceVertices[thetaIndex][phiIndex] = buildVertex(currentTheta, currentPhi);
        this.surfaceNorms[thetaIndex][phiIndex] = buildNorm(currentTheta, currentPhi);
        currentPhi = currentPhi + phiStep;
        // println("phi index: " + phiIndex);
        phiIndex++;
      }
      currentTheta = currentTheta + thetaStep;
      // println("theta index: " + thetaIndex);
      thetaIndex++;
      
    }
    
  }
  
  // make sure to use CCW
  // technically, phi is always 0
  Triangle makeNorth(int theta, int phi){
    // 'root' vertex
    PVector v3 = this.surfaceVertices[theta][0];
    PVector n3 = this.surfaceNorms[theta][0];
    
    // 'down 1' vertex
    PVector v2 = this.surfaceVertices[theta][1];
    PVector n2 = this.surfaceNorms[theta][1];
    
    // diagonal vertex (down 1, right 1)
    PVector v1 = this.surfaceVertices[(theta+1)%this.thetaSteps][1]; // % so it loops back to [0][phi+1] vertex
    PVector n1 = this.surfaceNorms[(theta+1)%this.thetaSteps][1];
    
    PVector[] vertices = {v1, v2, v3};
    PVector[] normals = {n1, n2, n3};
    if(v1 == null | v2 == null | v3 == null){
      println("Null Vector in Sphere: North " + theta + " " + phi);
      println(v1 + " " + v2 + " " + v3 );
    }
    return new Triangle(vertices, normals);
  }
  
  Triangle makeSouth(int theta, int phi){
    // 'root' vertex
    PVector v3 = this.surfaceVertices[theta][phi];
    PVector n3 = this.surfaceNorms[theta][phi];
    
    // 'down 1' vertex
    PVector v2 = this.surfaceVertices[theta][phi+1];
    PVector n2 = this.surfaceNorms[theta][phi+1];
    
    // diagonal vertex (right 1)
    PVector v1 = this.surfaceVertices[(theta+1)%this.thetaSteps][phi]; // % so it loops back to [0][phi+1] vertex
    PVector n1 = this.surfaceNorms[(theta+1)%this.thetaSteps][phi];
    
    PVector[] vertices = {v1, v2, v3};
    PVector[] normals = {n1, n2, n3};
    if(v1 == null | v2 == null | v3 == null){
      println("Null Vector in Sphere: South " + theta + " " + phi); 
      println(v1 + " " + v2 + " " + v3 );
      println(this.surfaceVertices[theta][phi]);
      println(this.surfaceVertices[theta][phi+1]);
      println(this.surfaceVertices[theta].length);
    }
    return new Triangle(vertices, normals);
  }
  
  Triangle[] makeRect(int theta, int phi){
    Triangle [] list = new Triangle[2];
    // 'root' vertex
    PVector v3 = this.surfaceVertices[theta][phi];
    PVector n3 = this.surfaceNorms[theta][phi];
    
    // 'down 1' vertex
    PVector v2 = this.surfaceVertices[theta][phi+1];
    PVector n2 = this.surfaceNorms[theta][phi+1];
    
    // diagonal vertex (down 1, right 1)
    PVector v1 = this.surfaceVertices[(theta+1)%this.thetaSteps][phi+1]; // % so it loops back to [0][phi+1] vertex
    PVector n1 = this.surfaceNorms[(theta+1)%this.thetaSteps][phi+1];
    
    PVector[] vertices = {v1, v2, v3};
    PVector[] normals = {n1, n2, n3};
    list[0] = new Triangle(vertices, normals); // |/ , [0,0] [0][1] [1][1] Triangle
    
    // right 1 vertex
    PVector v4 = this.surfaceVertices[(theta+1)%this.thetaSteps][phi]; // % so it loops back to [0][phi+1] vertex
    PVector n4 = this.surfaceNorms[(theta+1)%this.thetaSteps][phi];
    
    PVector[] vertices2 = {v1, v3, v4};
    PVector[] normals2 = {n1, n3, n4};
    if(v1 == null | v2 == null | v3 == null| v4 == null){
      println("Null Vector in Sphere:  Rect" + theta + " " + phi); 
      println(v1 + " " + v2 + " " + v3 + " " + v4);
    }
    list[1] = new Triangle(vertices2, normals2); // /| , [0][0] [1][1] [0][1] Triangle
    
    return list;
  }
  
  
  // construct triangle objects from the built vertices
  void buildTriangles(){
    if(this.surfaceVertices == null){
      buildVertices();
    } else {
      //println("building sphere triangles");
      // build triangles, and pass normals to vectors
      ArrayList<Triangle> tri = new ArrayList<Triangle>(); // dynamic array, use .toArray() later
      // int leng = (this.thetaSteps) * (this.phiSteps);
      
      // horizontal orbit loop
      for(int theta = 0; theta < this.thetaSteps; theta++){
        // vertical arc loop
        for(int phi = 0; phi < this.phiSteps; phi++){ // don't loop to [this.phiSteps - 1], b/c it's the south Pole
          if(phi == 0){ // we're at the north pole
            tri.add(makeNorth(theta, phi));
          } else if(phi == this.phiSteps - 1){ // the vertex that connects to the south pole
            tri.add(makeSouth(theta, phi));
          } else { //we're at a 'rectangle' vertex, make two triangles
            Triangle[] list = makeRect(theta, phi);
            tri.add(list[0]);
            tri.add(list[1]);
          }
        }
      }
      
      this.surface = tri.toArray(new Triangle[tri.size()]);
      println("Total triangles: " + this.surface.length);
    }
  }
  
  Triangle[] getSurface(){
    if(this.surface != null){ // already have surface
      return this.surface; 
    } else { // no surface
      buildVertices();
      buildTriangles();
      return this.surface;
    }
  }
  
  // Takes a point 'p' and returns the point on the surface of the sphere that is closest
  // this is done by casting a ray from the sphere center to 'p', normalizing the vector and then scaling by the radius
  // since the sphere is centered on the origin, the vector to normalize is just 'p'
  // all points are 3D, not 2D projections
  PVector getSurfacePoint(PVector p){
    PVector res = p.copy().normalize().mult(this.radius);
    // println("origin dist: ", Math.sqrt(Math.pow(res.x,2) + Math.pow(res.y,2) + Math.pow(res.z,2)), "radius: " , this.radius); // should be close to this.radius
    return res;
  }
  
  // simplified with sphere centered on origin,
  // normal is just 'p' normalized
  PVector getSurfaceNormal(PVector p){
    PVector res = p.copy().normalize();
    return res;
  }
  
} // end of class
