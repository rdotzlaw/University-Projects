
color stateColor;

final int Red = 0;
final int Green = 1;
final int Blue = 2;

final int NUM_COLOR_CHANNELS = 3;

// when you store (ambient,diffuse,specular) values in an array,
// USE THESE NAMED CONSTANTS to access the entries
final int A = 0;
final int D = 1;
final int S = 2;
final int NUM_LIGHT_COMPONENTS = 3;


final float[][] PHONG_COLORS = {{0.15f, 0.65f, 1f}, {0.15f, 0.65f, 1f}, {1f, 1f, 1f}} ; // A, D, S colors
final float[] MATERIAL = {0.5, 0.5, 0.5}; // A, D, S
final float PHONG_SHININESS = 15; // exponent
final float[] BLACK = {0.0f, 0.0f, 0.0f};

final float REFLECTION_OFFSET = 0.0001;

void setColor(float[] col) {
  stateColor = color(col[Red], col[Green], col[Blue]);
}

void setColor(float red, float green, float blue) {
  stateColor = color(red, green, blue);
}

void setColor(color c) {
  stateColor = c;
}

// draw a pixel at the given location
void setPixel(float x, float y) {
  int index = indexFromXYCoord(x, y);
  if (0 <= index && index < buffer.pixels.length) {
    buffer.pixels[index] = stateColor;
  } else {
    println("ERROR:  this pixel is not within the raster.");
  }
}

void setPixel(PVector p) {
  setPixel(p.x, p.y);
}

// helper functions for pixel calculations
int indexFromXYCoord(float x, float y) {
  int col = colNumFromXCoord(x);
  int row = rowNumFromYCoord(y);
  return indexFromColRow(col, row);
}

int indexFromColRow(int col, int row) {
  return row*width + col;
}

int colNumFromXCoord(float x) {
  return (int)round(x + WIDTH/2);
}

int rowNumFromYCoord(float y) {
  return (int)round(HEIGHT/2 - y);
}

// get the center of the i,jth pixel on the raster
// fulstrum boundaries: L, R for left and right boundaries; T, B for top and bottom boundaries
// N is raster z-position in 3D space
// also works as vector from EYE to pixel
PVector pixelCenter(int i, int j){
  PVector center = new PVector(
    (float)L + (((float)(R-L) / (float)WIDTH) * ((float)i + 0.5)),
    (float)B + (((float)(T-B) / (float)HEIGHT) * ((float)j + 0.5)),
    (float)N);
  
  return center;
}


/*
Given a point p, unit normal vector n, eye location, light location, and various
 material properties, calculate the Phong lighting at that point (see course
 notes and the assignment instructions for more details).
 Return an array of length 3 containing the calculated RGB values.
 */
// needs normalized vectors
float[] phong(PVector p, PVector n, PVector eye, PVector l, float[] material, float[][] fillColor, float shininess, Shape f) {
  
  // ensure vectors are normalized
  PVector light = l.copy().normalize(); 
  n.copy().normalize();
  PVector viewer = p.copy().sub(EYE).normalize(); // EYE -> p
  
  // get material constants
  float mA = material[A];
  float mD = material[D];
  float mS = material[S];
  
  // get color constants
  float[] cA = fillColor[A];
  float[] cD = fillColor[D];
  float[] cS = fillColor[S];
  
  // ambient color channels
  float[] Ambient = {mA*cA[Red], mA*cA[Green], mA*cA[Blue]};

  // check for shadow
  if(shaded(p,f)){
    return Ambient; // return ambient only if shaded
  }
  
  // diffuse color
  float dR = cD[Red]*max(light.dot(n),0); // ensure no negative values
  float dG = cD[Green]*max(light.dot(n),0);
  float dB = cD[Blue]*max(light.dot(n),0);
  float[] Diffuse = {mD*dR, mD*dG, mD*dB}; //<>//
  
  // specular color
  // find 'r', the light reflected around 'n' at point 'p'
  PVector r = getReflection(n, light);
  float rPow = (float)pow(max(r.dot(viewer),0), shininess); // no negative values
  float sR = cS[Red]*rPow;
  float sG = cS[Green]*rPow;
  float sB = cS[Blue]*rPow;
  float[] Specular = {mS*sR, mS*sG, mS*sB};
  
  
  // combine colors
  float[] colors = new float[3];
  for(int i = 0; i < 3; i++){
    colors[i] = Ambient[i] + Diffuse[i] + Specular[i];
  }
  return colors;
}

// reflects incident vector about normal
// reflection vector = incident - 2(incident . norm)*norm
PVector getReflection(PVector norm, PVector incident){
  PVector i = incident.copy();
  PVector n = norm.copy().normalize();
  float IdotN = i.dot(n) * 2.0;
  n.mult(IdotN);
  return i.sub(n).normalize();
  
  
}

PVector getIncident(PVector ray, PVector from){
 return ray.copy().sub(from).normalize(); 
}

// checks if the 'point' of 's' is shaded
// so make a ray from 'point' in direction of light source, and see if there's an intersect
boolean shaded(PVector point, Shape s){
  // vector pointed from 'point' to 'LIGHT'
  PVector PtoL = LIGHT.copy().sub(point);
  float dist = PtoL.mag();
  
  Ray r = new Ray(point, PtoL.normalize(), s); // create ray from 'point' in direction of 'LIGHT'
  return r.iShape != null && r.iDist <= dist && enable_shadows; // check if ray had any intersect, and intersect happened before lightsource
  
}
