class Particle {
  // which team the particle belongs to
  boolean team; // false = enemy team, true = player team
  PVector pos;
  PVector dir;
  float size;
  color col;
  
  boolean marked = false; // when true, play death animation
  int death_frames = 0;
  final int MAX_DEATH_FRAMES = 256;
  ArrayList<DeathParticle> debris;
  final int NUM_DEBRIS = 20;
  // use bounding circle for hitreg
  
  Particle(PVector p, float s, color c, boolean t) {
    this.pos = p;
    this.size = s;
    this.col = c;
    this.team = t;
  }
  
  // boundign circle w/ radius = 2*size?
  boolean hitReg(Particle p) {
    if(doCollision) {
      // distance
      PVector my_pos = new PVector(this.pos.x, this.pos.y);
      PVector you_pos = new PVector(p.pos.x, p.pos.y);
      float dist = abs(my_pos.sub(you_pos).mag());
      
      // players and enemies are made of boxes that go [-size,size]
      // so use double size for hitreg
      if(this.size >= p.size) {
        return dist <= this.size*2; 
      } else {
        return dist <= p.size*2; 
      }
    } else { // no collision enabled
      return false; 
    }  
  }
  
  void drawParticle() {
     fill(this.col);
     circle(this.pos.x, this.pos.y, this.size*2); // 2*size to match hitbox
  }
  
  void step() {}
  
  void kill() {}
}
