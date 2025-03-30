class Triangle {
  static final int NUM_VERTICES = 3;

  // positions of three triangle vertices in 3D space
  PVector[] vertices = new PVector[NUM_VERTICES];
  
  // normal vectors at vertices
  PVector[] vertexNormals = new PVector[NUM_VERTICES];
  
  // center point
  PVector center = new PVector();
  
  // normal vector at center point
  PVector centerNormal = new PVector();
  
  // edge vectors
  PVector[] edges = new PVector[NUM_VERTICES];
  
  // projected points
  PVector[] projVertices = new PVector[NUM_VERTICES];
  
  // projected edges
  PVector[] projEdges = new PVector[NUM_VERTICES];
  
  // projected center point <- might not change, idk
  PVector projCenter = new PVector();
  
  // for back culling
  Boolean frontFacing;
  
  // for single triangle
  // is the triangle part of a surface or not
  Boolean inSurface = true;

  Triangle(PVector[] vertices, PVector[] normals) {
    for (int j=0; j<NUM_VERTICES; j++) {
      this.vertices[j] = vertices[j].copy();
      this.vertexNormals[j] = normals[j].copy();
      this.vertexNormals[j].normalize();
      this.edges[j] = new PVector(); // initialize individual elements
      this.projVertices[j] = new PVector();
      this.projEdges[j] = new PVector();
    }
    
    updateAll();
  }
  
  Triangle(PVector[] vertices) {
    for (int j=0; j<NUM_VERTICES; j++) {
      this.vertices[j] = vertices[j].copy();
      this.vertexNormals[j] = new PVector();
      this.edges[j] = new PVector(); // initialize individual elements
      this.projVertices[j] = new PVector();
      this.projEdges[j] = new PVector();
      this.inSurface = false;
    }
    
    updateAll();
  }
  
  // does standard ortho normal compute for the triangle plane
  void computeNormals(){
   if(!inSurface){
     // treat as plane -> all normals will be the same
     PVector ortho = edges[0].cross(edges[1]).normalize();
     for(int i = 0; i < NUM_VERTICES; i++){
       vertexNormals[i].set(ortho.copy());
     }
   }
   
   
   
  }
  
  
  float[] getBarycentric(PVector point){    
    // point - vertices vectors
    PVector[] pointVec = getPointVec(point);
    
    
    // area of projected triangle
    float A = triAreaVector(this.projEdges[0], this.projEdges[1]);
    
    // area of sub-triangles
    float A1 = triAreaVector(this.projEdges[1], pointVec[1]);
    float A2 = triAreaVector(this.projEdges[2], pointVec[2]);
    float A3 = triAreaVector(this.projEdges[0], pointVec[0]);
    
    float[] list = {A1/A, A2/A, A3/A}; // u,v,w
    return list;
  }
  
  // make PVectors between given point and projected vertices
  PVector[] getPointVec(PVector point){
    PVector[] pointVec = new PVector[3];
    for(int i = 0; i < NUM_VERTICES; i++){
     pointVec[i] = point.copy().sub(this.projVertices[i]);
    }
    return pointVec;
  }
  
  
  // using projected coords
  Boolean isPointWithin(PVector point){
    int count = 0;
    PVector[] pointVec = getPointVec(point);
    for(int i = 0; i < NUM_VERTICES; i++){
     // cross prod with projected edge
     // all cross products must be positive
     PVector cross = this.projEdges[i].copy().cross(pointVec[i]);
     if(cross.z >= 0.0){
      count++; 
     }
    }
    return count == 3;
    
   }
   
  
  // checks if triangle is front facing, updates local value and returns it.
  Boolean isFrontFacing(){
    
    float error = 0.001;
    // camera is at (0,0,600)
    PVector L = EYE.copy().sub(this.center);
    this.frontFacing = L.dot(this.centerNormal) > error; // >= 0, acute, frontFacing
    // println("frontfacing: " + this.frontFacing);
    return this.frontFacing;
   }

  // if triangle vertices or vertex normals change, update remaining data
  void updateAll() {
    
    // compute projected vertices
    projVertices[0].set(projectVertex(this.vertices[0]));
    projVertices[1].set(projectVertex(this.vertices[1]));
    projVertices[2].set(projectVertex(this.vertices[2]));
    
    // any vertex projection return null? set flag
    Boolean nullProj = (projVertices[0] == null || projVertices[1] == null || projVertices[2] == null);
    
    // compute center point
    // center point P = u*p1 + v*p2 + w*p3
    // where (u,v,w) = (1/3, 1/3, 1/3) and pi is the triangle's vertices
    center = new PVector();
    center.set(PVector.mult(vertices[0], 1.0/3.0));
    center.add(PVector.mult(vertices[1], 1.0/3.0));
    center.add(PVector.mult(vertices[2], 1.0/3.0));
    
    // compute projected center point
    projCenter.set(projectVertex(center));
    // println("center: " + center);
    
    
    // compute edges
    // maintain order of edges -> CCW
    PVector e0 = PVector.sub(vertices[1], vertices[0]);
    PVector e1 = PVector.sub(vertices[2], vertices[1]);
    PVector e2 = PVector.sub(vertices[0], vertices[2]);
    
    // compute projected edges
    projEdges[0].set(PVector.sub(projVertices[1], projVertices[0]));
    projEdges[1].set(PVector.sub(projVertices[2], projVertices[1]));
    projEdges[2].set(PVector.sub(projVertices[0], projVertices[2]));
    
    // check CCW
    if(e0.cross(e1).mag() < 0){ // not CCW
      println("Not CCW!"); // if vertices are ordered in CCW initially, this should never trigger, probably
      // remake edge vectors in opposite winding
      e0 = PVector.sub(vertices[2], vertices[0]);
      e1 = PVector.sub(vertices[1], vertices[2]);
      e2 = PVector.sub(vertices[0], vertices[1]);
      
      // compute projected edges
      projEdges[0].set(PVector.sub(projVertices[2], projVertices[0]));
      projEdges[1].set(PVector.sub(projVertices[1], projVertices[2]));
      projEdges[2].set(PVector.sub(projVertices[0], projVertices[1]));
    }
    edges[0].set(e0); // set obj scope variable
    edges[1].set(e1); // ensures data doesn't fall out of scope
    edges[2].set(e2);

    
    // compute center normal
    // this is just the average of the vertex normals
    PVector avgNormal = vertexNormals[0].copy().add(vertexNormals[1]);
    avgNormal.add(vertexNormals[2]);
    //println("centerNorm1: " + avgNormal);
    avgNormal.mult(1.0/3.0);
    //println("centerNorm2: " + avgNormal);
    centerNormal.set(avgNormal).normalize();
    //println("centerNorm3: " + centerNormal);
    
    if(!nullProj) this.isFrontFacing(); // no null projection
    else this.frontFacing = false; // null projection, do not display triangle
    //println("center: ");
    //this.isPointWithin(this.projCenter);
    //println("test: ");
    //this.isPointWithin(projectVertex(new PVector(-100, -100, -100)));
  }

  void setVectors(PVector[] newVertices, PVector[] newNormals) {
    for (int j=0; j<Triangle.NUM_VERTICES; j++) {
      vertices[j] = newVertices[j].copy();
      vertexNormals[j] = newNormals[j].copy();
    }
    updateAll();
  }
  
  PVector[] getEdges(){
    return this.edges; 
  }
  
  void setProjVerts(PVector[] newVerts){
   for(int i = 0; i < NUM_VERTICES; i++){
     projVertices[i] = newVerts[i]; 
   }
  }
}

/*
Implementation of the rotation effect. 
Don't change anything below here.
*/

void rotateTriangles(Triangle[] original, Triangle[] rotated, float theta) {
  if (original == null || rotated == null) return;
  for (int i=0; i<original.length; i++) {
    PVector[] rotatedVertices = new PVector[Triangle.NUM_VERTICES];
    PVector[] rotatedNormals = new PVector[Triangle.NUM_VERTICES];
    for (int j=0; j<Triangle.NUM_VERTICES; j++) {
      rotatedVertices[j] = rotateVertex(original[i].vertices[j], theta);
      rotatedNormals[j] = rotateVertex(original[i].vertexNormals[j], theta);
    }
    rotated[i].setVectors(rotatedVertices, rotatedNormals);
  }
}

/*
Parameter v is a 3D vector. Return a copy of v after
 rotating by angle theta about the x, y and z axes in succession.
 This math will be covered later in the course.
 */
PVector rotateVertex(PVector v, float theta) {
  PVector r = v.copy();
  for (int axis=X; axis<=Z; axis++) {
    eulerRotate(r, theta, axis);
  }
  return r;
}

/*
Rotate 3D vector in place about the given axis
 */
void eulerRotate(PVector v, float theta, int rotateIndex) {
  float[] vectorArray = v.array();
  int ind1 = (rotateIndex+1) % NUM_DIMENSIONS;
  int ind2 = (rotateIndex+2) % NUM_DIMENSIONS;

  float tmp1, tmp2;

  tmp1 = vectorArray[ind1]*cos(theta) - vectorArray[ind2]*sin(theta);
  tmp2 = vectorArray[ind1]*sin(theta) + vectorArray[ind2]*cos(theta);
  vectorArray[ind1] = tmp1;
  vectorArray[ind2] = tmp2;
  v.set(vectorArray);
}

Triangle[] copyTriangleList(Triangle[] originalList) {
  if (originalList == null) return null;
  Triangle[] copyList = new Triangle[originalList.length];
  for (int i=0; i<originalList.length; i++) {
    copyList[i] = new Triangle(originalList[i].vertices, originalList[i].vertexNormals);
  }
  return copyList;
}
