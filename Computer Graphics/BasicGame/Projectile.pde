class Projectile extends Particle {
  boolean right;
  int lifetime = 0;
  int TAIL_MAX = 10; // how many sub particles max
  final float SUBP_SIZE = projSize / 2;
  PVector dir = new PVector(0.0, -1, 0.0);
  PVector standard = new PVector(0,-1,0);
  Projectile(PVector p, float s, color c, boolean t) {
    super(p, s, c, t);
    if(!this.team) { // if enemy team, need to travel in direction of player
      // at the point when the shot was taken
      PVector temp_p_pos = new PVector(player.pos.x, player.pos.y);
      PVector temp_pos = new PVector(this.pos.x, this.pos.y);
      this.right = temp_p_pos.x > temp_pos.x; // is the player right of the projectile
      this.dir.set(temp_p_pos.sub(temp_pos).normalize()); // this.pos -> player.pos (2D!)
    } // otherwise, we're just going straight up
  }
  
  void step() {
    float speed = this.team ? projSpeed : projSpeed / 2.0; // enemies projectiles move slower
    this.pos.add(this.dir.copy().mult(speed));
    // if out of bounds, delete self
    if(this.pos.y >= HEIGHT + (this.size + TAIL_MAX*SUBP_SIZE) |
       this.pos.y <= -(this.size + TAIL_MAX*SUBP_SIZE) |
       this.pos.x >= WIDTH + (this.size + TAIL_MAX*SUBP_SIZE) |
       this.pos.x <= -(this.size + TAIL_MAX*SUBP_SIZE)) {
      projectiles.remove(this);
    }
    lifetime++;
  }
  
  void drawParticle() {
    if(this.marked) {
      TAIL_MAX--;
      this.death_frames++;
    }
    int tail_len = min(lifetime, TAIL_MAX);
    // behind sub-particles
    // instead of creating extra particle objects for trail, just draw x-steps behind
    noStroke();
    for(int i = 0; i < tail_len; i++) { // start @ main particle
       push();
       // construct behind sub-particles
       fill(color(red(this.col), green(this.col), blue(this.col), 255 - ((float)(i)* (255.0/tail_len)))); // decreasing alpha thru loop
       //circle(this.pos.x + (SUBP_SIZE*cos((i+lifetime) * (PI/(2*tail_len)))), this.pos.y+(this.size+(i*SUBP_SIZE)), SUBP_SIZE);
       translate(this.pos.x, this.pos.y);
       float angle = adjAngle(this.dir.copy(), this.standard.copy());
       if(!this.right)
         rotateZ(-angle);
       else
         rotateZ(angle);
       circle((SUBP_SIZE*cos((i+lifetime) * (PI/(2*tail_len)))), (this.size+(i*SUBP_SIZE)), SUBP_SIZE);
       pop();
    }
    
    
    if(!this.marked) {
      // main particle
      stroke(color(255,255,255));
      fill(this.col);
      circle(this.pos.x, this.pos.y, this.size);
      
      // sub particles
      noStroke();
      for(int i = 0; i < tail_len; i++) { // start @ main particle
         // construct in-front sub-particles
         push();
         translate(this.pos.x, this.pos.y);
         fill(color(red(this.col), green(this.col), blue(this.col), 255 - ((float)(i)* (255.0/tail_len)))); // decreasing alpha thru loop
         // rotate so tail is away from direction
         float angle = adjAngle(this.dir.copy(), this.standard.copy());
         if(!this.right)
           rotateZ(-angle);
         else
           rotateZ(angle);
         circle(-(SUBP_SIZE*cos((i+lifetime) * (PI/(2*tail_len)))),(this.size+(i*SUBP_SIZE)), SUBP_SIZE);
         pop();
      }
    } else if (this.death_frames == this.MAX_DEATH_FRAMES) {
      // die
      this.kill();
    }
  }
  
  void mark() {
    this.marked = true; 
    projectiles.remove(this);
    dying.add(this);
  }
  
  void kill() {
    dying.remove(this);
  }
}
