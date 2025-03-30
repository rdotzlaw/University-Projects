import java.util.Stack;  // for your matrix stack
import java.util.*; // for custom sort in flyingShapes
Stack<PMatrix2D> matrixStack = new Stack<PMatrix2D>();

PMatrix2D M = new PMatrix2D();
PMatrix2D V = new PMatrix2D();
PMatrix2D Pr = new PMatrix2D();
PMatrix2D Vp = new PMatrix2D();


final float F_LEFT = -300.0;
final float F_RIGHT = 300.0;
final float F_BOTTOM = -300.0;
final float F_TOP = 300.0;
final float F_FAR = 500;
final float F_NEAR = 50;

PMatrix3D F = getViewFrustum(F_LEFT, F_RIGHT, F_BOTTOM, F_TOP, F_NEAR, F_FAR);

final float BIG_TEST_PATTERN = 1000;
final float MED_TEST_PATTERN = 100;
final float SMALL_TEST_PATTERN = 1;

final color BLACK = color(0); 

final int CANVAS_HEIGHT = 600;
final int CANVAS_WIDTH = 600;

float camera_zoom = 1.0;
float zoom_factor = 0.25;

float current_rotation = 0.0;
float delta_theta = 12.0;

final float EPSI = 1e-6;

void setup() {
  
  println("Ryan Dotzlaw");
  println("All ortho modes and interactions work in all display modes");
  size(600, 600);  // don't change, and don't use the P3D renderer
  colorMode(RGB, 1.0f);

  // put additional setup here
  println("Display mode: " + testMode);
  println("-> Ortho mode: " + orthoMode);
  
  // create camera matrix
  V.set(getCamera(new PVector(0.0,0.0), new PVector(0.0,1.0), new PVector(1.0,0.0), camera_zoom));
  // create viewport matrix
  Vp.set(getViewPort());
  
  // do shapes setup
  createShapes();
}

void draw() {
  background(BLACK);

  switch (testMode) {
  case PATTERN:
    drawTest(BIG_TEST_PATTERN);
    drawTest(MED_TEST_PATTERN);
    drawTest(SMALL_TEST_PATTERN);
    break;

  case SCENE:
    drawScene();
    break;
    
  case SHAPES:
    moveShapes();
    drawShapes();
    break;
  }
}

void drawDebug(){
  strokeWeight(THIN_LINE);
  beginShape(LINES);
  stroke(1,0,0);
  myVertex(0,0);
  myVertex(0.5,0.5);
  endShape(LINES);
}

float totalX = 0.0;
float totalY = 0.0;
void mouseDragged() {
  /*
   how much the mouse has moved between this frame and the previous one,
   measured in viewport coordinates - you will have to do further
   calculations with these numbers
   */
  float xMove = mouseX - pmouseX;
  float yMove = mouseY - pmouseY;
  // implement click-and-drag panning here
  
  //println("x " + xMove);
  //println("y " + yMove);
  PVector moveDir;
  xMove *= (1.0/camera_zoom); // scale translate with zoom
  yMove *= (1.0/camera_zoom);
  // move direction
  if(orthoMode == OrthoMode.IDENTITY) {
    moveDir = new PVector((xMove/(CANVAS_WIDTH/2.0)), (yMove/(CANVAS_HEIGHT/2.0))); // works in identity Pr only
  } else if(orthoMode == OrthoMode.FLIPX) {
    moveDir = new PVector(-xMove, yMove);
  } else if(orthoMode == OrthoMode.ASPECT) {
    moveDir = new PVector(xMove, yMove / 3.0); // 3:1 aspect ratio
  } else {
    moveDir = new PVector(xMove, yMove);
  }

  
  // need to rotate this vector along with the model
  moveDir = getRotation((current_rotation * PI) / delta_theta).mult(moveDir, null);
  
  //println("vx " + moveDir.x);
  //println("vy " + moveDir.y);
  myTranslate(-moveDir.x, moveDir.y); 
  // x is negative so it follows the mouse properly, otherwise, doesn't follow in x-dir
}
