// for accessing components of homogeneous 4-vectors
final int X = 0;
final int Y = 1;
final int Z = 2;
final int W = 3;
final int NUM_COORDS = 4;
final int NUM_VERTICES = 4; // reset as desired
final int NUM_SHAPES = 20; // reset as desired
final float MOVE_LIMIT = F_FAR; // how far the shapes travel away from camera
final float SPEED = 2.0;
Shape[] shapes = new Shape[NUM_SHAPES];

// return random integer in [min,max] range
int rand(int min, int max){
  return min + (int)(Math.random() * (max - min + 1));
}

int rand(float m, float m2){
  return rand(round(m), round(m2)); 
}



void createShapes(){
  // x,y in [-300,300]
  // z in [0,500]
  // random color
  // speed?
  // size?
  
  for(int i = 0; i < NUM_SHAPES; i++){
    int n = rand(0, all_colors.length-1);
    color c = all_colors[n];
    
    float x = (float)rand(F_LEFT, F_RIGHT);
    float y = (float)rand(F_BOTTOM, F_TOP);
    float z = (float)rand(F_NEAR, F_FAR);
    PVector p = new PVector(x,y,z);
    
    
    float size = (float)(rand(10, 35)) / 100.0; // range: [0.1, 0.35]
    
    shapes[i] = new Shape(c, p, SPEED, size);
  }
  
}

// construct projection matrix for view frustum
// perspective projection, not orthographic
PMatrix3D getViewFrustum(float left, float right, float bottom, float top, float near, float far) {
  return new PMatrix3D((2.0*near)/(right-left), 0, (right+left)/(right-left), 0,
                       0, (2.0*near)/(top-bottom), (top+bottom)/(top-bottom), 0, 
                       0, 0, -(far+near)/(far-near), -(2.0*near*far)/(far-near),
                       0, 0, -1, 0);
}

void moveShapes() {
  for(int i = 0; i < shapes.length; i++){
    shapes[i].step(); 
  }
}

// dont want to write a sorting algorithm
// use default w/ custom function
class sortShapes implements Comparator<Shape> {
  public int compare(Shape s1, Shape s2){
    return round(s2.pos.z - s1.pos.z); // reverse sort, larger 'z' drawn first
  }
}

void drawShapes() {
   // draw furthest shapes first
   // sort array
   Arrays.sort(shapes, new sortShapes());
   
   for(int i = 0; i < shapes.length; i++){
     myPush();
     shapes[i].drawShape();
     myPop();
   }
   
}


class Shape {
  PVector pos;
  color col;
  float speed;
  float size;
  float projSize;
  PVector NDC_pos;
  Shape(color c, PVector p, float s, float si){
    this.col = c;
    this.pos = p;
    this.NDC_pos = p;
    this.speed = s;
    this.size = si;
    this.projSize = si;
  }
  
  void step(){
    // move z
    this.pos.z = (this.pos.z + this.speed) % MOVE_LIMIT; // goes [F_NEAR, F_FAR)
    if(this.pos.z < F_NEAR) this.pos.z = F_NEAR;
    
    // multiply shape pos w/ perspective matrix //<>//
    float[] f1 = {this.pos.x, this.pos.y, this.pos.z, 1.0};
    float[] p = new float[4];
    F.mult(f1, p);
    
    this.NDC_pos = new PVector(p[X]/p[W], p[Y]/p[W], p[Z]/p[W]); // divide out 'w'
    
    float factor = F_NEAR / this.pos.z; // inverse scale wrt distance
    this.projSize = this.size * factor;
  }
  
  // its a square
  void drawShape(){
    fill(this.col);
    beginShape();
    
    myVertex(this.NDC_pos.x - this.projSize, this.NDC_pos.y + this.projSize);
    myVertex(this.NDC_pos.x + this.projSize, this.NDC_pos.y + this.projSize);
    myVertex(this.NDC_pos.x + this.projSize, this.NDC_pos.y - this.projSize);
    myVertex(this.NDC_pos.x - this.projSize, this.NDC_pos.y - this.projSize);
    endShape();
  }
}
