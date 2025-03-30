// full 2D pipeline:
// P` = [Vp][Pr][V][M]P

// construct viewport matrix using width and height of canvas
PMatrix2D getViewPort() { // Vp
  float w2 = (float)CANVAS_WIDTH / 2.0;
  float h2 = (float)CANVAS_HEIGHT / 2.0;
  return new PMatrix2D(w2, 0.0, w2, 0.0, -h2, h2); // 3x3 matrix, last row is always [0,0,1]
  
}

PMatrix2D getViewPortInv() { // Vp^-1
  
  float w = 2.0 / (float)CANVAS_WIDTH;
  float h = 2.0 / (float)CANVAS_HEIGHT;
  return new PMatrix2D(w, 0.0, -1.0, 0.0, -h, 1.0);
  
}

// hold these values for calculating inverse
float orthoLeft;
float orthoRight;
float orthoBottom;
float orthoTop;

// construct projection matrix using 2D boundaries
PMatrix2D getOrtho(float left, float right, float bottom, float top) { // Pr
  orthoLeft = left;
  orthoRight = right;
  orthoBottom = bottom;
  orthoTop = top;
  return new PMatrix2D(2.0/(right - left), 0.0, -(right + left)/(right - left),  //<>//
                       0.0, 2.0/(top - bottom), -(top + bottom)/(top - bottom));
  
}



PMatrix2D getOrthoInv(float left, float right, float bottom, float top) {
  if(orthoMode == OrthoMode.IDENTITY){
    return new PMatrix2D(); 
  }
  return new PMatrix2D((right-left)/2.0, 0.0, (right+left)/2.0, 0.0, (top-bottom)/2.0, (top+bottom)/2.0);
  
}

PVector cameraCenter;
PVector cameraUp;
PVector cameraPerp;


// construct camera matrix using camera position, up vector, and zoom setting
PMatrix2D getCamera(PVector center, PVector up, PVector perp, float zoom) {
  if(zoom != 1.0){ // there is a scale factor
    myScale(zoom, zoom); // will ensure it's not 0
  }
  cameraCenter = center;
  cameraUp = up;
  cameraPerp = perp;
  return new PMatrix2D(perp.x, up.x, -center.x, perp.y, up.y, -center.y);
}

PMatrix2D getCameraInv(PVector center, PVector up, PVector perp, float zoom) {
  float x1 = perp.x;
  float x2 = up.x;
  float y1 = perp.y;
  float y2 = up.y;
  float cx = center.x;
  float cy = center.y;
  
  float d1 = (y2 * x1) - (x2 * y1);
  float d2 = -(x2 * y1) + (y2 * x1);
  
  return new PMatrix2D(y2/d1, -x2/d1, ((-x2 * cy)/d2) + ((cx * y2)/d2),
                       -y1/d1, x1/d1, ((x1 * cy)/d2) + ((-cx * y1)/d2));
  
}

/*
Functions that manipulate the matrix stack
 */

// adds current model matrix to the stack
void myPush() {
  PMatrix2D toPush = new PMatrix2D();
  toPush.set(M);
  matrixStack.push(toPush);
}

// pops a model matrix off the stack, and sets the current model matrix to it
void myPop() {
  M.set(matrixStack.pop());
}

/*
Functions that update the model matrix
 */

void myScale(float sx, float sy) {
  // [M] = [M][S], S is scale matrix
  if(sx > 0 && sy > 0){
    PMatrix2D S = getScale(sx, sy);
    S.preApply(M); // [M][S]
    M.set(S);
  }
}

PMatrix2D getScale(float sx, float sy){
  return new PMatrix2D(sx, 0.0, 0.0, 0.0, sy, 0.0);
}

void myTranslate(float tx, float ty) {
  PMatrix2D T = getTranslate(tx, ty);
  T.preApply(M);
  M.set(T);
}

PMatrix2D getTranslate(float tx, float ty) {
  return new PMatrix2D(1.0, 0.0, -tx, 0.0, 1.0, -ty);
}

// rotates by theta CCW
void myRotate(float theta) {
  PMatrix2D R = getRotation(theta);
  R.preApply(M); // [M][R]
  M.set(R);
}

PMatrix2D getRotation(float theta){
  return new PMatrix2D(cos(theta), -sin(theta), 0.0, sin(theta), cos(theta), 0.0);
}

/*
Receives a point in object space and applies the complete transformation
 pipeline, Vp.Pr.V.M.point, to put the point in viewport coordinates.
 Then calls vertex to plot this point on the raster
 */
void myVertex(float x, float y) {
  // apply transformations here
  PVector v = applyPipeline(new PVector(x,y,1.0));
  // this is the only place in your program where you are allowed
  // to use the vertex command
  vertex(v.x, v.y);
}

// overload for convenience
void myVertex(PVector vertex) {
  myVertex(vertex.x, vertex.y);
}

PVector applyPipeline(PVector v){
  v = M.mult(v,null);
  v = V.mult(v,null);
  v = Pr.mult(v,null);
  v = Vp.mult(v,null);
  return v;
}
