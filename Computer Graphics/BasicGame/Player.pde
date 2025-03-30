class Player extends Particle {
  int shotGap = PLAYER_FIRE_RATE; // so pkayer can fire immediately
  final float stopAura = 10.0; // how close to origin needed to stop

  int t_frame = 0; // frame of thruster animation
  float theta = 0.0; // for ship roll
  final float DELTA_THETA = PI/24.0;
  final float MAX_THETA = PI/4.0;
  
  Player(float s, color c) {
    super(playerOrigin.copy(), s, c, true);
    dir = new PVector();
  }
  
  void step() {
    if(!this.marked) { // player not dying
      if(shotGap < PLAYER_FIRE_RATE) { // prevents int overflow if no shot is taken in MAX_INT frames
        shotGap++;
      }
      
      int xDir = 0;
      int yDir = 0;
      // handle movement
      if(playerLeft) {
        xDir = -1;
        theta -= DELTA_THETA;
      } else if(playerRight) {
        xDir = 1;
        theta += DELTA_THETA;
      } else if(!playerLeft & !playerRight & theta != 0.0) { 
        if(theta > 0) { // slowly reduce ship roll
          theta -= DELTA_THETA;
        } else if (theta < 0) {
          theta += DELTA_THETA;
        }
      }
      if(abs(theta) > MAX_THETA) theta = MAX_THETA*xDir; // limit roll to MAX_THETA in xDir direction
      
      if(playerUp) {
        yDir = -1;
        if(t_frame < 4) // 4 is max frames in thrust animation
          t_frame++;
      } else if(playerDown) {
        yDir = 1;
      } else if(!playerUp & t_frame > 0) t_frame--; // reduce thrust animation frames
      
      if(xDir == 0 & yDir == 0){ // move towards origin
        PVector temp = playerOrigin.copy().sub(this.pos); // playerPos -> playerOrigin
        if(temp.mag() >= stopAura) { // full speed ahead
          float t = 1.0/temp.mag(); // get stronger the closer you are
          temp.normalize().mult(t*3);
          this.dir.add(temp); 
        } else { // we're close to origin, snap to it
          // this fixes problems that occur when travelling slowly really close to origin
          // -> player would just jitter about playerOrigin
          this.pos = playerOrigin.copy();
          this.dir = new PVector();
        }
        
      } else { // move in input direction
        PVector temp = new PVector(xDir, yDir, 0);
        temp.normalize().mult(playerAccel);
        this.dir.add(temp); // get faster over time
        // speed cap is uneccessary b/c heavy speed penalty for bouncing off boundary
      }
      // apply movement to position
      this.pos.add(this.dir);
      // lock boundary
      // Not using constrain() b/c want to have player bounce off walls
      if(this.pos.x >= WIDTH - this.size) { // right
        this.pos.x = WIDTH - this.size;
        this.dir.x = this.dir.x * bounceFactor * -1;
      } else if(this.pos.x <= this.size) { // left
        this.pos.x = this.size;
        this.dir.x = this.dir.x * bounceFactor * -1;
      }
      if(this.pos.y >= HEIGHT - this.size) { // down
        this.pos.y = HEIGHT - this.size;
        this.dir.y = this.dir.y * bounceFactor * -1;
      } else if(this.pos.y <= this.size) { // up
        this.pos.y = this.size;
        this.dir.y = this.dir.y * bounceFactor * -1;
      }
      
    } else { // marked for death
      for(int i = 0; i < this.debris.size(); i++) { // step debris
        this.debris.get(i).step(); 
      }
      this.death_frames++; // for explosion blast animation
    }
  }
  
  void shoot(){
    if(shotGap == PLAYER_FIRE_RATE & !this.marked){ // reloaded and not dying
      // create projectile
      projectiles.add(new Projectile(new PVector(this.pos.x, this.pos.y - this.size), projSize, this.col, true));
      shotGap = 0;
    }
    // else do nothing
  }
  
  void drawParticle() {
    if(doTextures && !this.marked) { // want to texture, and not dying
      if(t_frame > 0 && !game_over) { // doing thruster frame animation
        String src = "images/thrust_" + t_frame + ".png";
        PImage i = loadImage(src);
        noStroke();
        beginShape();
        texture(i);
        vertex(this.pos.x-this.size/2, this.pos.y+this.size, 0,0);
        vertex(this.pos.x+this.size/2, this.pos.y+this.size, 1,0);
        vertex(this.pos.x, this.pos.y + (2*this.size), 0.5,1);
        endShape(CLOSE);
      }
      push();
      
      translate(this.pos.x, this.pos.y);
      
      PImage img = loadImage("images/Ship_2.png");
      noStroke();
      
      // roll rotation
      rotateY(theta);
      translate(-this.pos.x, -this.pos.y);
      
      beginShape();
      texture(img);
      vertex(this.pos.x - this.size, this.pos.y - this.size, 0, 0);
      vertex(this.pos.x - this.size, this.pos.y + this.size, 0, 1);
      vertex(this.pos.x + this.size, this.pos.y + this.size, 1, 1);
      vertex(this.pos.x + this.size, this.pos.y - this.size, 1, 0);
      endShape(CLOSE);
      
      pop();

    } else if(!this.marked){ // no texturing
      super.drawParticle(); 
    } else if(this.marked) { // we're dying
      // explosion blast
      fill(color(250, 10, 10, this.MAX_DEATH_FRAMES - this.death_frames));
      circle(this.pos.x, this.pos.y, (float)this.death_frames * 0.5);
      fill(color(255, 255, 0, this.MAX_DEATH_FRAMES - this.death_frames));
      circle(this.pos.x, this.pos.y, (float)this.death_frames * 0.2);
      // draw debris
      for(int i = 0; i < this.debris.size(); i++) {
        this.debris.get(i).drawParticle();
      }
    }
  }
  
  void mark() { // we hit something
    this.marked = true;
    player = null;
    this.debris = genDeathAnimation(this.pos, this.size, this.col, this, this.NUM_DEBRIS);
    dying.add(this);
  }
  
  void kill() { // game over, death animation over too
    game_over = true; 
  }
}
