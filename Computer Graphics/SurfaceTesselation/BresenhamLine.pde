/*
Use Bresenham's line algorithm to draw a line on the raster between
 the two given points. Modify the raster using setPixel() ONLY.
 */
void bresenhamLine(int fromX, int fromY, int toX, int toY) {
  //println("------------");
  
  // find direction of deltaX and deltaY
  int xStepDir;
  int deltaX = toX - fromX;
  if(deltaX >= 0){ // deltaX is pos
    xStepDir = 1;
  } else { // deltaX is negative
    xStepDir = -1;
  }
  //println("deltaX: " + deltaX);
  
  int deltaY = toY - fromY;
  int yStepDir;
  if(deltaY >= 0){ // deltaY is pos
    yStepDir = 1;
  } else { // deltaY is negative
    yStepDir = -1;
  }
  //println("deltaY: " + deltaY);
  
  // decide the fast direction
  boolean fastX = abs(deltaX) >= abs(deltaY); // named after the movie
  //println("fastX?: " + fastX);
  
  
  int x = fromX;
  int y = fromY;
  float slope;
  float errorInc; // error incrementor
  if(deltaX == 0){ // undef slope, vertical line
    //println("vert line");
    slope = 0;
    errorInc = 0;
  } else { // non-vertical line
    //println("non-vert line");
    slope = (float)deltaY / (float)deltaX; // cast as float, otherwise non-int results = 0.0
    if(fastX){
      errorInc = abs(slope);
    } else {
      errorInc = 1/abs(slope); // inverse
    }
  }
  //println("slope: " + slope);
  //println("errorInc: " + errorInc);
  
  
  float error = errorInc;
  
  // main loop
  // increment over X or Y, and in which direction?
  if(fastX){
    // going to do a total of abs(deltaX) increments
    int count = 0;
    int max = abs(deltaX);
    // println(max);
    
    while(count <= max){
      setPixel(x, y); // draw pixel
      // check Y step
      if(error > 0.5){ // too much error, step in Y dir
         y = y + yStepDir; // increment Y
         error--; // decrement error by 1
      }
      x = x + xStepDir; // increment X
      error = error + errorInc; // increase error w/ step
      // increment counter
      count = count + 1;
    }
  } else { // !fastX
    int count = 0;
    int max = abs(deltaY);
    
    while(count <= max){
     setPixel(x,y);
     // check X step
     if(error > 0.5){ // step in X dir
       x = x + xStepDir; // increment X
       error--; // decrement error by 1
     }
     y = y + yStepDir; // increment Y
     error = error + errorInc; // increase error w/ step
     count++;
    }
  }
   
}

/*
Don't change anything below here
 */

final int LENGTH_X = 125;
final int LENGTH_Y = 125;
final int LENGTH_DIAGONAL = 52;

void testBresenham() {
  final color WHITE = color(1f, 1f, 1f);
  final color RED = color(1f, 0f, 0f);

  final int CENTER_OFFSET_X = 125;
  final int CENTER_OFFSET_Y = 125;

  buffer.updatePixels(); // display everything drawn so far

  buffer.stroke(RED);
  ComparisonLines comp = new ComparisonLines();
  comp.drawAllQuadrants(CENTER_OFFSET_X, CENTER_OFFSET_Y);

  buffer.loadPixels(); // switch back to editing the raster
  setColor(WHITE);

  // use the implementation of Bresenham's algorithm
  BresenhamLines bres = new BresenhamLines();
  bres.drawAllQuadrants(CENTER_OFFSET_X, CENTER_OFFSET_Y);
}

abstract class TestPattern {
  void drawAllQuadrants(int centerOffsetX, int centerOffsetY) {
    for (int signX=-1; signX<=1; signX+=2) {
      int centerX = signX*centerOffsetX;
      for (int signY=-1; signY<=1; signY+=2) {
        int centerY = signY*centerOffsetY;
        drawPattern(centerX, centerY);
      }
    }
  }

  void drawPattern(int centerX, int centerY) {
    drawAxes(centerX, centerY);
    drawDiagonals(centerX, centerY);
  }

  void drawAxes(int startX, int startY) {
    for (int sign=-1; sign<=1; sign+=2) {
      int endXHorizontal = startX + sign*LENGTH_X;
      int endYVertical = startY + sign*LENGTH_Y;
      drawLine(startX, startY, endXHorizontal, startY);
      drawLine(startX, startY, startX, endYVertical);
    }
  }

  void drawDiagonals(int startX, int startY) {
    for (int signX=-1; signX<=1; signX+=2) {
      int endXHorizontal = startX + signX*LENGTH_X;
      int endXDiagonal = startX + signX*LENGTH_DIAGONAL;
      for (int signY=-1; signY<=1; signY+=2) {
        int endYVertical = startY + signY*LENGTH_Y;
        int endYDiagonal = startY + signY*LENGTH_DIAGONAL;
        drawLine(startX, startY, endXDiagonal, endYVertical);
        drawLine(startX, startY, endXHorizontal, endYDiagonal);
      }
    }
  }

  abstract void drawLine(int startX, int startY, int endX, int endY);
}

class ComparisonLines extends TestPattern {
  void drawLine(int startX, int startY, int endX, int endY) {
    final int SMALL_SHIFT = 3;

    // shift left/right or up/down
    int xDir = -Integer.signum(endY - startY);
    int yDir = Integer.signum(endX - startX);

    int pStartX = rasterToProcessingX(startX + xDir*SMALL_SHIFT);
    int pStartY = rasterToProcessingY(startY + yDir*SMALL_SHIFT);

    int pEndX = rasterToProcessingX(endX + xDir*SMALL_SHIFT);
    int pEndY = rasterToProcessingY(endY + yDir*SMALL_SHIFT);

    buffer.line(pStartX, pStartY, pEndX, pEndY);
  }

  int rasterToProcessingX(int rx) {
    return width/2 + rx;
  }

  int rasterToProcessingY(int ry) {
    return height/2 - ry;
  }
}

class BresenhamLines extends TestPattern {
  void drawLine(int startX, int startY, int endX, int endY) {
    bresenhamLine(startX, startY, endX, endY);
  }
}
