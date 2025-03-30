/*
COMP 3490 Assignment 1
Ryan Dotzlaw
 */

final color BLACK = color(0);
final Boolean debug = false; // true == disable culling

// for the test mode with one triangle
Triangle[] singleTriangle;
Triangle[] rotatedSingleTriangle;

// for drawing and rotating the 3D shape
Sphere s;
Triangle[] surfaceTessellation;
Triangle[] rotatedSurfaceTessellation;
// constants for surface tesselation
final int nTheta = 32; // 32
final int nPhi = 16; // 16
final int radius = 200;

// to make the image rotate - don't change these values
float theta = 0.0;  // rotation angle
float dtheta = 0.01; // rotation speed

PGraphics buffer;

void setup() {
  println("Ryan Dotzlaw");
  
  
  // RGB values range over 0..1 rather than 0..255
  colorMode(RGB, 1.0f);

  buffer = createGraphics(600, 600);
  PVector[] singleVerts = new PVector[Triangle.NUM_VERTICES];
  
  singleVerts[0] = new PVector(0,125,200);
  singleVerts[1] = new PVector(-125,0,200);
  singleVerts[2] = new PVector(125,0,200);
  

  singleTriangle = new Triangle[1];
  singleTriangle[0] = new Triangle(singleVerts);
  singleTriangle[0].computeNormals();
  rotatedSingleTriangle = copyTriangleList(singleTriangle);
  
  s = new Sphere(radius, nPhi, nTheta);
  surfaceTessellation = s.getSurface();
  rotatedSurfaceTessellation = copyTriangleList(surfaceTessellation);
  

  printSettings();
}

void settings() {
  size(600, 600); // hard-coded canvas size, same as the buffer
}

/*
You should read this function carefully and understand how it works,
 but you should not need to edit it
 */
void draw() {
  buffer.beginDraw();
  buffer.colorMode(RGB, 1.0f);
  buffer.background(BLACK);

  /*
  CAUTION: none of your functions should call loadPixels() or updatePixels().
   This is already done in the template. Extra calls will probably break things.
   */
  buffer.loadPixels();

  if (doRotate) {
    theta += dtheta;
    if (theta > TWO_PI) {
      theta -= TWO_PI;
    }
  }

  //do not change these blocks: rotation is already set up for you
  if (displayMode == DisplayMode.TEST_LINES) {
    testBresenham();
  } else if (displayMode == DisplayMode.SINGLE_TRIANGLE) {
    rotateTriangles(singleTriangle, rotatedSingleTriangle, theta);
    drawTriangles(rotatedSingleTriangle);
  } else if (displayMode == DisplayMode.SURFACE) {
    rotateTriangles(surfaceTessellation, rotatedSurfaceTessellation, theta);
    drawTriangles(rotatedSurfaceTessellation);
  }

  buffer.updatePixels();
  buffer.endDraw();
  image(buffer, 0, 0); // draw our raster image on the screen
}


/*
 Receives an array of triangles and draws them on the raster by
 calling draw2DTriangle()
 */
void drawTriangles(Triangle[] triangles) {
  for(int i = 0; i < triangles.length; i++){
    draw2DTriangle(triangles[i]);
  }
}

/*
Use the projected vertices to draw the 2D triangle on the raster.
Draw in the following order:
  1. Shading
  2. Outline
  3. Normals
 */
void draw2DTriangle(Triangle t) {
  // want counter-clockwise winding (CCW)
  // CCW implemented in Triangle.updateAll
 
  
  if(t.frontFacing | debug){ // triangle is front facing //<>//
    // cull degenerate triangles w/ area close to 0
    if(triArea(t.projVertices) > 1.0 | debug){ // now actually draw everything
      // do shading (if needed)
      if(shadingMode != ShadingMode.NONE){
        fillTriangle(t);
      }
      // draw outlines (if needed)
      if(doOutline){
        setColor(OUTLINE_COLOR); // red
        bresenhamLine((int)t.projVertices[0].x, (int)t.projVertices[0].y, (int)t.projVertices[1].x, (int)t.projVertices[1].y);
        bresenhamLine((int)t.projVertices[1].x, (int)t.projVertices[1].y, (int)t.projVertices[2].x, (int)t.projVertices[2].y);
        bresenhamLine((int)t.projVertices[2].x, (int)t.projVertices[2].y, (int)t.projVertices[0].x, (int)t.projVertices[0].y);
      }
      
      
      
      
      
      // draw normals after filling (if needed)
      if(doNormals){
        drawNormals(t);
      }
      
    } else {
     //println("Cull Degenerate"); 
    }
    
    
  } else {
   // println("Not Front Facing."); 
  }
  
}

/*
 Draw the normal vectors at each vertex and triangle center
 */
final int NORMAL_LENGTH = 20;
final float[] FACE_NORMAL_COLOR = {0f, 1f, 1f}; // cyan
final float[] VERTEX_NORMAL_COLOR = {1f, 0.6f, 0.1f}; // orange

// uses projected points
void drawNormals(Triangle t) {
  // from point to point + NORMAL_LENGTH * normal vector
  setColor(VERTEX_NORMAL_COLOR);
  for(int i = 0; i < Triangle.NUM_VERTICES; i++){
    // vertex normals are normalized, so scale by NORMAL_LENGTH
    PVector endpoint = new PVector();
    endpoint.set(t.projVertices[i].copy().add(t.vertexNormals[i].copy().mult(NORMAL_LENGTH)));
    bresenhamLine((int)t.projVertices[i].x, (int)t.projVertices[i].y, (int)endpoint.x, (int)endpoint.y);
  }
  setColor(FACE_NORMAL_COLOR);
  PVector endpoint = new PVector();
  endpoint.set(t.projCenter.copy().add(t.centerNormal.copy().mult(NORMAL_LENGTH)));
  bresenhamLine(round(t.projCenter.x), round(t.projCenter.y), round(endpoint.x), round(endpoint.y));
  setColor(OUTLINE_COLOR);
  
}

/*
Fill the 2D triangle on the raster, using a scanline algorithm.
 Modify the raster using setColor() and setPixel() ONLY.
 */
void fillTriangle(Triangle t) { //<>//
  // check each pixel in bounding box defined by min/max of x/y coords of triangle vertices
  // for an arbitrary pixel P = (x,y)
  // given the triangle T, consisting of the points v1->3 and edges e1->3
  // create the vectors p1->3 where pi = p - vi (pi is the vector from vertex v1 to point P)
  // then calculate 2D cross-products: ei x pi for all i 1->3
  // if the sign of all cross-products are the same (- XOR +), then pixel P is within the triangle
  
  float[] xList = {t.projVertices[0].x, t.projVertices[1].x, t.projVertices[2].x};
  float[] yList = {t.projVertices[0].y, t.projVertices[1].y, t.projVertices[2].y};
  
  int xMax = ceil(max(xList));
  int xMin = floor(min(xList));
  int yMax = ceil(max(yList));
  int yMin = floor(min(yList));
  
  int xDif = abs(xMax - xMin);
  int yDif = abs(yMax - yMin);
  
  // if(xDif == 0 | yDif == 0){
   // println(t.projCenter.x + " " + t.projCenter.y); 
  // }
  
  
  setColor(FLAT_FILL_COLOR); // default color //<>//
  
  
  float[][] gouraud = new float[3][3];
  if(shadingMode == ShadingMode.PHONG_FACE){
    // calculate face color from center
    setColor(phong(t.center, t.centerNormal, EYE, LIGHT, MATERIAL, PHONG_COLORS, PHONG_SHININESS));
  } else if(shadingMode == ShadingMode.PHONG_VERTEX){
    // calculate color at each vertex, then set as the average between them
    float[] c0 = phong(t.vertices[0], t.vertexNormals[0], EYE, LIGHT, MATERIAL, PHONG_COLORS, PHONG_SHININESS);
    float[] c1 = phong(t.vertices[1], t.vertexNormals[1], EYE, LIGHT, MATERIAL, PHONG_COLORS, PHONG_SHININESS);
    float[] c2 = phong(t.vertices[2], t.vertexNormals[2], EYE, LIGHT, MATERIAL, PHONG_COLORS, PHONG_SHININESS);
    float[] average = {(1.0/3.0)*c0[0] + (1.0/3.0)*c1[0] + (1.0/3.0)*c2[0],
                       (1.0/3.0)*c0[1] + (1.0/3.0)*c1[1] + (1.0/3.0)*c2[1],
                       (1.0/3.0)*c0[2] + (1.0/3.0)*c1[2] + (1.0/3.0)*c2[2],
    };
    setColor(average);
  } else if(shadingMode == ShadingMode.PHONG_GOURAUD){
    // calculate color at each vertex
    gouraud[0] = phong(t.vertices[0], t.vertexNormals[0], EYE, LIGHT, MATERIAL, PHONG_COLORS, PHONG_SHININESS);
    gouraud[1] = phong(t.vertices[1], t.vertexNormals[1], EYE, LIGHT, MATERIAL, PHONG_COLORS, PHONG_SHININESS);
    gouraud[2] = phong(t.vertices[2], t.vertexNormals[2], EYE, LIGHT, MATERIAL, PHONG_COLORS, PHONG_SHININESS);
  }
  
  int count = 0;
  // loop through bounding box
  for(int y = 0; y < yDif; y++){ // loop down
    for(int x = 0; x < xDif; x++){ // loop horizontally in a line
      PVector point = new PVector(xMin + x,yMin + y,0);
     if(t.isPointWithin(point)){
       // decide color
       if(shadingMode == ShadingMode.BARYCENTRIC){
         setColor(t.getBarycentric(point));
       } else if(shadingMode == ShadingMode.PHONG_GOURAUD){
         // use 'gouraud' and point's barycentric coords to perform a weighted average
         // setColor(gouraud avg weighted by t.getBaryCentric(point) coords)
         float[] b = t.getBarycentric(point);
         float[] weighted = {
           b[0]*gouraud[0][0] + b[1]*gouraud[1][0] + b[2]*gouraud[2][0],
           b[0]*gouraud[0][1] + b[1]*gouraud[1][1] + b[2]*gouraud[2][1],
           b[0]*gouraud[0][2] + b[1]*gouraud[1][2] + b[2]*gouraud[2][2],
         };
         setColor(weighted);
       } else if(shadingMode == ShadingMode.PHONG_SHADING){
         // per pixel calculation
         // get un-projected pixel
         float[] b = t.getBarycentric(point);
         PVector v1 = t.vertices[0].copy();
         PVector v2 = t.vertices[1].copy();
         PVector v3 = t.vertices[2].copy();
         // u*v1 + v*v2 + w*v3 = un-projected pixel
         PVector pTri = v1.mult(b[0]).add(v2.mult(b[1]).add(v3.mult(b[2]))); // stupid .add chain
         // now we have the point on the triangle, but we need the point & normal on the sphere
         PVector p = s.getSurfacePoint(pTri);
         PVector n = s.getSurfaceNormal(pTri);
         float[] c = phong(p, n, EYE, LIGHT, MATERIAL, PHONG_COLORS, PHONG_SHININESS);
         setColor(c);
       }
       setPixel((float)(xMin + x),(float)(yMin + y));
       count++;
     }
    }
   }
   // if(count > 0 ){ // debug stuff
     // println("Drew " + count + " pixels for " + t.vertices[0]);
   // } else if(t.frontFacing & count == 0){
     // println("what the heck!!!"); 
   // }


}

/*
Given a point p, unit normal vector n, eye location, light location, and various
 material properties, calculate the Phong lighting at that point (see course
 notes and the assignment instructions for more details).
 Return an array of length 3 containing the calculated RGB values.
 */
// needs normalized vectors

float[] phong(PVector p, PVector n, PVector eye, PVector light, float[] material, float[][] fillColor, float shininess) {
  
  // ensure vectors are normalized
  light.normalize();
  n.normalize();
  PVector viewer = eye.copy().sub(p).normalize(); // eye <-> p
  
  // get material constants
  float mA = material[0];
  float mD = material[1];
  float mS = material[2];
  
  // get color constants
  float[] cA = fillColor[0];
  float[] cD = fillColor[1];
  float[] cS = fillColor[2];
  
  // ambient color channels
  float[] Ambient = {mA*cA[0], mA*cA[1], mA*cA[2]};
  // println(Ambient);
  
  // diffuse color
  float dR = cD[0]*max(light.dot(n),0); // ensure no negative values
  float dG = cD[1]*max(light.dot(n),0);
  float dB = cD[2]*max(light.dot(n),0);
  float[] Diffuse = {mD*dR, mD*dG, mD*dB};
  
  // specular color
  // find 'r', the light reflected around 'n' at point 'p'
  float LdotN = light.dot(n) * 2;
  PVector nMult = n.mult(LdotN);
  PVector r = nMult.sub(light).normalize();
  float rPow = (float)pow(max(r.dot(viewer),0), shininess); // no negative values
  float sR = cS[0]*rPow;
  float sG = cS[1]*rPow;
  float sB = cS[2]*rPow;
  float[] Specular = {mS*sR, mS*sG, mS*sB};
  
  
  // combine colors
  float[] colors = new float[3];
  for(int i = 0; i < 3; i++){
    colors[i] = Ambient[i] + Diffuse[i] + Specular[i];
  }
   return colors;
}
