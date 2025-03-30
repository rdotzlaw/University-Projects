class Enemy extends Particle {
  
  int shotGap = 0;
  PVector dir_to_player = new PVector(0,1,0);
  boolean right = false;
  int spawn_frame = 0;
  int MAX_SPAWN_FRAME = 30;
  float t = 0;
  
  Enemy(PVector p, float s, color c) {
    super(p, s, c, false);
    PVector temp = randDir();
    this.dir = new PVector(temp.x, temp.y, 0); // remove z direction for random movement
    this.dir.normalize();
    if(player != null && !player.marked) // keep track of the direction to the player
      this.dir_to_player = player.pos.copy().sub(this.pos).normalize();
  }
  
  boolean hitReg(Particle p) { // no hitReg if still spawning in
    if(this.spawn_frame == this.MAX_SPAWN_FRAME) {
      return super.hitReg(p); 
    }
    return false;
  }
  
  
  void drawParticle() {
    if(!this.marked && this.spawn_frame != this.MAX_SPAWN_FRAME) { // draw 'jump in' animation
      float alpha = 255 - lerp(0, 255, sin((PI/2.0)*t)); // ease out
      fill(color(255,255,255, alpha));
      circle(this.pos.x, this.pos.y, lerp(0, this.size*4, t)); // linear lerp
    }
    
    if(player != null && !player.marked && ENEMY_FIRE_RATE - shotGap <= ENEMY_CHARGE_TIME && !this.marked) { // charge shot
      PVector proj_pos = this.pos.copy().add(this.dir_to_player.copy().mult(this.size));
      stroke(color(255,255,255));
      fill(this.col);
      float tp = (float)(shotGap - ENEMY_FIRE_RATE + ENEMY_CHARGE_TIME + 1) / ENEMY_CHARGE_TIME;
      circle(proj_pos.x, proj_pos.y, lerp(0, projSize, 0.5 * (1 - cos(PI*tp)))); // ease in/out size
      noStroke();
    }
    
    if(!this.marked && doTextures) {
      // draw textures
      // rotate texture to face player at all times
      push();
      translate(this.pos.x, this.pos.y);
      float angle = adjAngle(this.dir_to_player.copy(), new PVector(0,-1,0));
      if(!this.right)
        rotateZ(-angle);
      else
        rotateZ(angle);
      translate(-this.pos.x, -this.pos.y);
      // now draw texture
      float x = this.pos.x;
      float y = this.pos.y;
      // ease in horizontal size according to spawn animation
      float s = t == 1.0 ? this.size : lerp(0, this.size, 1-cos((PI/2.0)*t)); // skip lerp if t==1
      noStroke();
      PImage img = loadImage("images/Ship_3.png");
      beginShape();
      texture(img);
      vertex(x-s, y-this.size, 0, 0);
      vertex(x+s, y-this.size, 1, 0);
      vertex(x+s, y+this.size, 1, 1);
      vertex(x-s, y+this.size, 0, 1);
      endShape(CLOSE);
      pop();
    } else if(!this.marked && !doTextures) {
      super.drawParticle(); 
    } else if(this.marked) {
      // draw death animation, same as Player death
      // explosion
      fill(color(250, 10, 10, this.MAX_DEATH_FRAMES - this.death_frames));
      circle(this.pos.x, this.pos.y, (float)this.death_frames * 0.5);
      fill(color(255, 255, 0, this.MAX_DEATH_FRAMES - this.death_frames));
      circle(this.pos.x, this.pos.y, (float)this.death_frames * 0.2);
      // debris
      for(int i = 0; i < this.debris.size(); i++) {
        this.debris.get(i).drawParticle();
      }
      
    }
  }
  
  void step() {
    if(!this.marked) { // enemy not dying
      if(this.spawn_frame < this.MAX_SPAWN_FRAME)
        this.spawn_frame++;
      // ease in when spawning
      t = (float)this.spawn_frame / (float)this.MAX_SPAWN_FRAME;
      float s = lerp(0, enemy_speed, 1-cos((PI/2.0)*t)); // ease in speed
      // move in direction
      this.pos.add(dir.copy().mult(s));
      // bounce off boundaries
      // no loss of speed when bouncing
      // Not using constrain() b/c want to have player bounce off walls
      if(this.pos.x >= WIDTH - this.size) { // right
        this.pos.x = WIDTH - this.size;
        this.dir.x = this.dir.x * -1;
      } else if(this.pos.x <= this.size) { // left
        this.pos.x = this.size;
        this.dir.x = this.dir.x * -1;
      }
      if(this.pos.y >= HEIGHT - this.size) { // down
        this.pos.y = HEIGHT - this.size;
        this.dir.y = this.dir.y * -1;
      } else if(this.pos.y <= this.size) { // up
        this.pos.y = this.size;
        this.dir.y = this.dir.y * -1;
      }
      if(player != null && !player.marked) { // this is what determines the rotation
        // don't do it if player is dead/dying
        this.dir_to_player = player.pos.copy().sub(this.pos).normalize(); // update direction to player
        // are we right of player rn?
        // -> angle between to vectors gives acute angle
        this.right = player.pos.x > this.pos.x;  // if right, we're rotating opposite angle
      }
      // shoot player
      if(shotGap < ENEMY_FIRE_RATE){
        shotGap++; 
      }
      
      shoot();
    } else {
      // step debris
      for(int i = 0; i < this.debris.size(); i++) {
        this.debris.get(i).step();
      }
      this.death_frames++;
    }
  }
  
  void shoot() {
    if(player == null || player.marked)
      return;
    if(shotGap == ENEMY_FIRE_RATE && !this.marked) {
       // create projectile on side closest to player
       PVector proj_pos = this.pos.copy().add(this.dir_to_player.copy().mult(this.size));
       projectiles.add(new Projectile(proj_pos, projSize, this.col, false));
       shotGap = 0;
    } 
  }
  
  void mark() {
    this.marked = true; 
    enemies.remove(this);
    dying.add(this);
    this.debris = genDeathAnimation(this.pos, this.size*2, this.col, this, this.NUM_DEBRIS);
  }
  
  void kill() {
    dying.remove(this); 
  }
}
