class DeathParticle extends Particle {
  
  int lifespan = 0; // how many frames spent alive
  PVector dir; // direction of travel
  float speed = projSpeed * 0.5; // half the standard projectile speed
  Particle parent;
  
  DeathParticle(PVector origin, float s, color c, PVector d, Particle p) {
    super(origin.copy(), s, c, true);
    this.dir = d.mult(this.speed);
    this.parent = p;
  }
  
  void step() {
    // reached end of life, death animation is over
    if(this.lifespan == this.MAX_DEATH_FRAMES) {
      this.parent.debris.remove(this);
      this.parent.kill();
    }
    
    // alpha for this reduces over time
    float alpha = 255 - (this.lifespan * ((float)255 / this.MAX_DEATH_FRAMES));
    this.col = color(red(this.col), green(this.col), blue(this.col), alpha);
    // move in direction
    this.pos.add(this.dir);
    
    lifespan++;
  }
  
  void drawParticle() {
    fill(this.col);
    float x = this.pos.x;
    float y = this.pos.y;
    float z = this.pos.z;
    
    
    push();
    // for rotation
    translate(x,y,z);
    
    
    float s = this.size;
    // random rotation based on randomly given direction
    PVector temp = this.dir.copy().normalize();
    rotateY(temp.y * (float)lifespan*PI / 32);
    rotateX(temp.x * (float)lifespan*PI / 32);
    rotateZ(temp.z * (float)lifespan*PI / 32);
    triangle(0,s, s,0, -s,0);
    pop();
    
  }
}
