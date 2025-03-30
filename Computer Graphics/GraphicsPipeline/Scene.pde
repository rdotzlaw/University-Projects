
final color RED = color(250, 30, 30);
final color BLUE = color(30, 30, 250);
final color GREEN = color(10, 215, 10);
final color YELLOW = color(250, 250, 30);
final color DARK_BROWN = color(92, 64, 51);
final color BROWN = color(150, 75, 0);
final color DARK_BLUE = color(10, 10, 50);
final color LIGHT_GREY = color(200, 200, 200);
final color WHITE = color(255, 255, 255);
final color PINK = color(255, 100, 230);

final color[] all_colors = {RED, BLUE, GREEN, YELLOW, DARK_BROWN, BROWN, DARK_BLUE, LIGHT_GREY, WHITE, PINK};

void drawScene() {
  noStroke();
  // draw sun
  myPush();
  myScale(0.1, 0.1);
  drawSun();
  myPop();
  // draw moon
  myPush();
  myScale(0.1, 0.1);
  drawMoon();
  myPop();
  
  // draw planet
  myPush();
  drawPlanet(0.7);
  myPop();
}

void drawSun() {
  myRotate(-PI / 4.0);
  myTranslate(0.0, -12);
  drawOctagon(YELLOW);
}

void drawMoon() {
  myRotate((-PI / 4.0) + PI);
  myTranslate(0.0, -12);
  drawOctagon(LIGHT_GREY);
}

void drawHouse() {
  beginShape();
  fill(BROWN);
  myVertex(-1, 1);
  myVertex(1, 1);
  myVertex(1, -1);
  myVertex(-1, -1);
  myVertex(-1, 1);
  endShape();
  beginShape();
  fill(DARK_BROWN);
  myVertex(0.0, 2.0);
  myVertex(1,1);
  myVertex(-1,1);
  myVertex(0.0, 2.0);
  endShape();
}

void drawPlanet(float size) {
  // atmosphere
  myPush();
  myScale(size + 0.3, size + 0.3);
  drawOctagon(DARK_BLUE);
  myPop();
  // main planet
  myPush();
  myScale(size,size);
  drawOctagon(BLUE);
  myPop();
  // land
  myPush();
  myScale(size,size);
  drawLand(size, size / 8.0);
  myPop();
  
  
}



void drawLand(float planet_size, float house_size) {
  myPush();
  myTranslate(0.0, -planet_size*0.01);
  myScale(planet_size*1.425, 1.0);
  arctic();
  
  myPush();
  myTranslate(0.0, -planet_size-(house_size*2));
  myScale(house_size, house_size);
  drawHouse();
  myPop();
  
  myRotate(PI);
  
  arctic();
  myTranslate(0.0, -planet_size-(house_size*2));
  myScale(house_size, house_size);
  drawHouse();
  myPop();
  
  myPush();
  temperate(planet_size, house_size);
  myPop();
}

void temperate(float ps, float hs) {
  for(int i = 0; i < 4; i++) {
    myPush();
    myTranslate(0.0, -ps/2);
    myPush();
    myScale(ps/3.0, ps/3.0);
    drawOctagon(GREEN);
    myPop();
    myScale(hs, hs);
    drawHouse();
    myPop();
    myRotate(PI/2.0);
  }
  
}

void arctic() { 
  beginShape();
  fill(WHITE);
  myPush();
  myVertex(0.0, 1);
  myRotate(PI/4.0);
  myVertex(0.0, 1);
  myRotate(-PI/2.0);
  myVertex(0.0, 1);
  myPop();
  endShape();
}

void drawSquare(color c) {
  beginShape();
  fill(c);
  for(int i = 0; i < 4; i++){
    myVertex(0.0, 1);
    myRotate(PI / 2.0);
  }
  
  endShape(); 
}

void drawOctagon(color c) {
  beginShape();
  fill(c);
  for(int i = 0; i < 8; i++){
    myVertex(0.0, 1.0);
    myRotate(PI / 4.0);
  }
  
  endShape();
}
