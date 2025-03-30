// don't change these keys
final char KEY_ROTATE_CW = ']';
final char KEY_ROTATE_CCW = '[';
final char KEY_ZOOM_IN = '='; // plus sign without the shift
final char KEY_ZOOM_OUT = '-';
final char KEY_ORTHO_MODE = 'o';
final char KEY_DISPLAY_MODE = 'd';

final int NUM_DISPLAY_MODES = 3;
final int NUM_ORTHO_MODES = 5;

enum OrthoMode {
  IDENTITY, // straight to viewport with no transformations (Pr, V and M are all the identity)
    CENTER600, // bottom left is (-300,-300), top right is (300,300), center is (0,0)
    TOPRIGHT600, // bottom left is (0,0), top right is (600,600)
    FLIPX, // same as CENTER600 but reflected through y axis (x -> -x)
    ASPECT // uneven aspect ratio: x is from -300 to 300, y is from -100 to 100
}
OrthoMode orthoMode = OrthoMode.IDENTITY;

enum DisplayMode {
    PATTERN, 
    SCENE,
    SHAPES
}
DisplayMode testMode = DisplayMode.PATTERN;


void keyPressed() {
  if (key == KEY_DISPLAY_MODE) {
    int nextDisplay = getNextOrdinal(testMode, NUM_DISPLAY_MODES);
    if(testMode == DisplayMode.SHAPES){
      // currently in shapes, re-setup camera matrix
      V.set(getCamera(new PVector(0.0,0.0), new PVector(0.0,1.0), new PVector(1.0,0.0), camera_zoom));
    }
    testMode = DisplayMode.values()[nextDisplay];
    println("Display mode: " + testMode);
    if(testMode == DisplayMode.SHAPES){
      // reset M, V, Pr, keep Vp
      M = new PMatrix2D();
      V = new PMatrix2D();
      Pr = new PMatrix2D();
    }
    
  } else if (key == KEY_ORTHO_MODE) {
    int nextOrtho = getNextOrdinal(orthoMode, NUM_ORTHO_MODES);
    orthoMode = OrthoMode.values()[nextOrtho];
    println("-> Ortho mode: " + orthoMode);
    // reset model matrix
    camera_zoom = 1.0;
    current_rotation = 0.0;
    M.set(new PMatrix2D());
    
    // change Pr matrix accordingly
    if(orthoMode == OrthoMode.IDENTITY){
      Pr = new PMatrix2D(); // identity matrix 
    } else if(orthoMode == OrthoMode.CENTER600){
      Pr.set(getOrtho(-300.0, 300.0, -300.0, 300.0)); // need to change V to have center at (0,0) ??
    } else if(orthoMode == OrthoMode.TOPRIGHT600){
      Pr.set(getOrtho(0.0, 600.0, 0.0, 600.0)); 
    } else if(orthoMode == OrthoMode.FLIPX){
      Pr.set(getOrtho(300.0, -300.0, -300.0, 300.0)); // center300, but flipped thru y-axis
    } else if(orthoMode == OrthoMode.ASPECT){
      Pr.set(getOrtho(-300.0, 300.0, -100.0, 100.0)); // LR=[-300,300], TB=[-100,100]
    }
  } else if (key == KEY_ROTATE_CW){
    current_rotation -= 1.0;
    current_rotation = current_rotation % 24.0;
    println("Current Rotation: (" + current_rotation + " PI)/" + delta_theta);
    myRotate(-(PI/delta_theta));
  } else if (key == KEY_ROTATE_CCW){
    current_rotation += 1.0;
    current_rotation = current_rotation % 24.0;
    println("Current Rotation: (" + current_rotation + " PI)/" + delta_theta);
    myRotate(PI/delta_theta);
  } else if (key == KEY_ZOOM_IN){
    camera_zoom *= (1.0 + zoom_factor);
    println("Zoom at: " + camera_zoom);
    myScale(1.0 + zoom_factor, 1.0 + zoom_factor); // scale model matrix?
  } else if (key == KEY_ZOOM_OUT){
    camera_zoom *= (1.0 / (1.0 + zoom_factor));
    println("Zoom at: " + camera_zoom);
    myScale(1.0 / (1.0 + zoom_factor), 1.0 / (1.0 + zoom_factor));
  }
}

int getNextOrdinal(Enum e, int enumLength) {
  return (e.ordinal() + 1) % enumLength;
}


// don't change anything below here

final int NUM_LINES = 11;
final int THIN_LINE = 1;
void drawTest(float scale) {
  float left, right, top, bottom;
  left = bottom = -scale/2;
  right = top = scale/2;

  strokeWeight(THIN_LINE);
  beginShape(LINES);
  for (int i=0; i<NUM_LINES; i++) {
    float x = left + i*scale/(NUM_LINES-1);
    float y = bottom + i*scale/(NUM_LINES-1);

    setHorizontalColor(i);
    myVertex(left, y);
    myVertex(right, y);

    setVerticalColor(i);
    myVertex(x, bottom);
    myVertex(x, top);
  }
  endShape(LINES);
}

void setHorizontalColor(int i) {
  int r, g, b;
  r = (i >= NUM_LINES/2) ? 0 : 1;
  g = (i >= NUM_LINES/2) ? 1 : 0;
  b = (i >= NUM_LINES/2) ? 1 : 0;
  stroke(r, g, b);
}

void setVerticalColor(int i) {
  int r, g, b;
  r = (i >= NUM_LINES/2) ? 1 : 0;
  g = (i >= NUM_LINES/2) ? 1 : 0;
  b = (i >= NUM_LINES/2) ? 0 : 1;
  stroke(r, g, b);
}
