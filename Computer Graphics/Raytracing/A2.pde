// Ryan Dotzlaw
// COMP 3490 - Assignment 2

final boolean enable_phong = true;
final boolean enable_shadows = true;
final boolean mirror_debug = false; // gives some outline to mirrored spheres, doesn't work well
int shape_count = 0;
int scene = 0;
final int NUM_SCENES = 6;
final float draw_distance = 10000.0;

final int backdrop = color(0);
final int HEIGHT = 800;
final int WIDTH = 800;
Shape[] shapes;

// raster settings
final int N = -16; // z-plane slice for raster
final int T = 20; // top
final int B = 0; // bottom
final int L = 0; // left
final int R = 20; // right

// light source
PVector LIGHT;

// camera
final PVector EYE = new PVector(0,0,0);
final PVector UP_DIR = new PVector(0,1,0);
final PVector LOOK_DIR = new PVector(0,0,-1);

PGraphics buffer;

void setup() {
  
  println("Ryan Dotzlaw");
  println("Enable Phong?: " + enable_phong + ", Enable Shadows?: " + (enable_shadows && enable_phong));
  println("Press any key to cycle the through the scenes");
  
  
  // RGB values range over 0..1 rather than 0..255
  colorMode(RGB, 1.0f);

  buffer = createGraphics(HEIGHT, WIDTH);
  
  
  // create all the shapes
  createSnowman();
}

void keyPressed(){
  scene = (scene + 1) % NUM_SCENES;
  if(scene == 0){
    println("Snowman");
    createSnowman();
  } else if(scene == 1){
     println("Shadow test 1");
    createShadow1();
  } else if(scene == 2){
    println("Shadow test 2");
    createShadow2();
  } else if(scene == 3){
    println("Shadow test 3");
    createPlaneShadow();
  } else if(scene == 4){
    println("Mirror test 1");
    createMirror1();
  } else if(scene == 5){
    println("Mirror test 2");
    createMirror2();
  }
}

void createPlaneShadow(){
  LIGHT = new PVector(0, 55, -30); // above scene
  shapes = new Shape[3];
  shapes[0] = new Plane(new PVector(-2,0,-30), enable_phong, new float[]{0.2f, 0.5f, 0.7f},new PVector(0.5,1,0));
  shapes[1] = new Sphere(new PVector(12,0,-30), enable_phong, new float[]{0.7f, 0.2f, 0.2f}, 5);
  shapes[2] = new Circle(new PVector(0, 4, -30), enable_phong, new float[]{0.7f, 0.2f, 0.2f}, new PVector(0,1,0), 3);
}

void createSnowman(){
  LIGHT = new PVector(15, 10, 18); // above, behind & right of camera
  shapes = new Shape[7];
  shapes[0] = new Sphere(new PVector(2,4,-30), enable_phong, new float[]{0.7f, 0.2f, 0.2f}, 4);
  shapes[1] = new Sphere(new PVector(1.5,8.2,-29), enable_phong, new float[]{0.7f, 0.2f, 0.7f}, 2.1);
  shapes[2] = new Sphere(new PVector(1.3,-1.5,-32), enable_phong, new float[]{0.2f, 0.2f, 0.7f}, 6);
  shapes[3] = new Plane(new PVector(0,-5,-10), enable_phong, new float[]{0.2f, 0.5f, 0.7f}, new PVector(0,1,0));
  shapes[4] = new Cylinder(new PVector(18, 0, -42), enable_phong, new float[]{0.7f, 0.5f, 0.4f}, 1.5, true);
  shapes[5] = new Cylinder(new PVector(-19, 0, -36), enable_phong, new float[]{0.7f, 0.5f, 0.4f}, 1.5, true);
  shapes[6] = new Sphere(new PVector(10,3,-30), enable_phong, new float[]{0.7f, 0.2f, 0.7f}, 2.1, true);
}

void createShadow1() {
  LIGHT = new PVector(55, 0, -30);
  shapes = new Shape[3];
  shapes[0] = new Sphere(new PVector(-9,0,-30), enable_phong, new float[]{0.7f, 0.2f, 0.2f}, 6);
  shapes[1] = new Sphere(new PVector(2,0,-30), enable_phong, new float[]{0.7f, 0.2f, 0.2f}, 4);
  shapes[2] = new Sphere(new PVector(9,0,-30), enable_phong, new float[]{0.7f, 0.2f, 0.2f}, 2);
}

void createShadow2() {
  LIGHT = new PVector(55, 1, -30);
  shapes = new Shape[3];
  shapes[0] = new Sphere(new PVector(-10,0,-30), enable_phong, new float[]{0.7f, 0.2f, 0.2f}, 2);
  shapes[1] = new Sphere(new PVector(-3,0,-30), enable_phong, new float[]{0.7f, 0.2f, 0.2f}, 4);
  shapes[2] = new Sphere(new PVector(8,0,-30), enable_phong, new float[]{0.7f, 0.2f, 0.2f}, 6);
}

void createMirror1(){
 LIGHT = new PVector(20, 10, 10);
 shapes = new Shape[4];
 shapes[0] = new Sphere(new PVector(0,0,-60), enable_phong, new float[]{0.7f, 0.2f, 0.2f}, 20, true); // mirror shape
 shapes[1] = new Sphere(new PVector(-5,-5,-30), enable_phong, new float[]{0.7f, 0.2f, 0.2f}, 4);
 shapes[2] = new Sphere(new PVector(12,6,-30), enable_phong, new float[]{0.7f, 0.2f, 0.2f}, 4);
 shapes[3] = new Plane(new PVector(0,-10,-30), enable_phong, new float[]{0.2f, 0.5f, 0.7f}, new PVector(0,1,0));
}

void createMirror2(){
 LIGHT = new PVector(20, 10, 10);
 shapes = new Shape[3];
 shapes[0] = new Sphere(new PVector(0,3,-30), enable_phong, new float[]{0.7f, 0.2f, 0.2f}, 6, true); // mirror shape
 shapes[1] = new Sphere(new PVector(0,3,10), enable_phong, new float[]{0.7f, 0.2f, 0.2f}, 4);
 shapes[2] = new Plane(new PVector(0,-5,-30), enable_phong, new float[]{0.2f, 0.5f, 0.7f}, new PVector(0,1,0));
}

void settings(){
  size(HEIGHT, WIDTH);
}

void draw(){
  buffer.beginDraw();
  buffer.colorMode(RGB, 1.0f);
  buffer.background(backdrop);
  
  buffer.loadPixels();  

  
  // cast rays here
  rayCaster();
  
  buffer.updatePixels();
  buffer.endDraw();
  image(buffer, 0, 0); // draw our raster image on the screen
}

// loop through every pixel on the raster and cast a ray
void rayCaster(){
  for(int j = -(HEIGHT/2) + 1; j < HEIGHT/2; j++){
    for(int i = -(WIDTH/2) + 1; i < WIDTH/2; i++){
      
      PVector pixelCenter = pixelCenter(i,j); // vector from EYE to the center of the (i,j) pixel in raster
      PVector d_ij = pixelCenter.copy().normalize();
      
      Ray r = new Ray(EYE, d_ij); // ray from EYE in d_ij direction
      
      if(r.iShape != null){ // 'r' intersects an object
        setColor(r.iShape.getColor(r.iPoint, r.dir));
        setPixel(i,j);
      }
      
      // no intersect, do nothing, pixel remains background color
    }
  }
}
