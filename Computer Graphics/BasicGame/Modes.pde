// player character
final char KEY_LEFT = 'a';
final char KEY_RIGHT = 'd';
final char KEY_UP = 'w';
final char KEY_DOWN = 's';
final char KEY_SHOOT = ' ';

// turn textures or collisions on/off - useful for testing and debugging
final char KEY_TEXTURE = 't';
final char KEY_COLLISION = 'c';

// debug toggle flags
boolean doTextures = true;
boolean doCollision = true;

// shooting flag
boolean playerShooting = false;

// movement flags
boolean playerLeft = false;
boolean playerRight = false;
boolean playerUp = false;
boolean playerDown = false;

void keyPressed() {
  if(key == KEY_TEXTURE) {
    doTextures = !doTextures;
    println("Textures Enabled: " + doTextures); 
  } else if(key == KEY_COLLISION) {
    doCollision = !doCollision;
    println("Collision Enabled: " + doCollision); 
  } else if(key == KEY_SHOOT) {
    playerShooting = true; // set to false on release
  } else if(key == KEY_LEFT) {
    playerLeft = true;
  } else if(key == KEY_RIGHT) {
    playerRight = true;
  } else if(key == KEY_UP) {
    playerUp = true;
  } else if(key == KEY_DOWN) {
    playerDown = true;
  }
}

void keyReleased() {
  if(key == KEY_SHOOT) {
    playerShooting = false; // set to false on release
  } else if(key == KEY_LEFT) {
    playerLeft = false;
  } else if(key == KEY_RIGHT) {
    playerRight = false;
  } else if(key == KEY_UP) {
    playerUp = false;
  } else if(key == KEY_DOWN) {
    playerDown = false;
  }
}
