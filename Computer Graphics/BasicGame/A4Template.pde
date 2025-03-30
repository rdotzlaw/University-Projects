
final int HEIGHT = 800;
final int WIDTH = 800;

int score = 0;
final int frames_till_score = 300; // roughly every 5 sec

// global entity variables
boolean game_over = false;
Player player;
int global_frame_count = 1;
final PVector playerOrigin = new PVector(WIDTH/2,5*HEIGHT/6, -10);
final float playerAccel = 0.1;
final float bounceFactor = 0.30;
final float projSize = 8.0;
final float projSpeed = 7.5; // 7.5
// stores all projectiles from both teams
ArrayList<Projectile> projectiles;
// stores enemy objects
ArrayList<Enemy> enemies;
int DIFF_SCALE = 0; // how many additional enemies spawn
final int DIFF_TIMER = 1024;
final float enemy_size = 24;
final float enemy_y = enemy_size * 2;
final float enemy_speed = 2.0;

// collection of elements going through their death animations
ArrayList<Particle> dying = new ArrayList<Particle>();

// how many frames need to pass before next projectile is created
final int PLAYER_FIRE_RATE = 20;
final int ENEMY_FIRE_RATE = 256;
final int ENEMY_CHARGE_TIME = 60;

// star positions
final int STAR_COUNT = 75;
int[][] star_pos = new int[STAR_COUNT][2];

// return random integer in [min,max] range
int rand(int min, int max){
  return min + (int)(Math.random() * (max - min + 1));
}

PVector randDir() {
  // decide random 3D-dir
  // generate random polar coords
  float theta = (((float)rand(0,1000)) / 1000.0) * PI; // 'elevation' [0,PI]
  float gamma = (((float)rand(0,2000)) / 1000.0) * PI; // 'rotation' [0, 2PI]
  float x = sin(theta)*cos(gamma);
  float y = sin(theta)*sin(gamma);
  float z = cos(theta);
  
  PVector dir = new PVector(x,y,z);
  dir.normalize(); // ensure normal for ease of use
  return dir;
}

// generate a collection of death particles for the Particle 'p'
ArrayList<DeathParticle> genDeathAnimation(PVector origin, float size, color c, Particle p, int count) {
  ArrayList<DeathParticle> ret = new ArrayList<DeathParticle>();
  
  for(int i = 0; i < count; i++) {
    PVector dir = randDir();
    ret.add(genDeathParticle(origin, size/6, c, dir, p));
  }
  
  return ret;
}

// generates a single death particle from given specifications
DeathParticle genDeathParticle(PVector origin, float size, color c, PVector dir, Particle p) {
  return new DeathParticle(origin, size, c, dir, p);
}

// randomly assigns coords to stars
void genStars() {
  for(int star_num = 0; star_num < STAR_COUNT; star_num++) {
    int x = rand(0, WIDTH);
    int y = rand(0, HEIGHT);
    star_pos[star_num][0] = x;
    star_pos[star_num][1] = y;
  }
}

// 2D adj angle for rotation towards direction 'a'
// returns values in range: [0,PI]
float adjAngle(PVector a, PVector b){
  PVector temp_a = new PVector(a.x, a.y);
  temp_a.normalize();
  PVector temp_b = new PVector(b.x, b.y);
  temp_b.normalize();
  return acos(temp_a.dot(temp_b));
}

void setup() {
  println("Ryan Dotzlaw");
  println("Spaceships from OpenGameArt.org, by Irmandito");
  println("Blackhole from deep-fold.itch.io planet generator");
  size(800, 800, P3D);
  //surface.setAlwaysOnTop(true);
  surface.setLocation(100, 100); // without this processing will start the sketch minimized, and not allow it to be un-minimized
  // idk why this happens, it wasn't a problem for 98% of the time spent working on the project
  textureMode(NORMAL);
  textureWrap(REPEAT);
  noFill();
  // create camera
  float fov = PI/3.0;
  float cameraZ = (HEIGHT/2.0) / tan(fov/2.0);
  camera(
    WIDTH/2, HEIGHT/2, cameraZ, // camera pos
    WIDTH/2, HEIGHT/2, cameraZ - 1, // camera look at
    0, 1, 0 // camera up
  );
  // create perspective
  perspective(fov, float(width)/float(height), 
              cameraZ/10.0, cameraZ*10.0);
  

  // do any additional setup here
  genStars();
  player = new Player(32, color(250,10,10));
  projectiles = new ArrayList<Projectile>();
  enemies = new ArrayList<Enemy>();
  // create initial enemy
  Enemy e = new Enemy(new PVector(rand((int)enemy_size, WIDTH - (int)enemy_size),enemy_y,-9), enemy_size, color(10,10,250));
  enemies.add(e);
}

// performed when an enemy dies to a projectile
// # enemies scales with time
void spawnEnemies() {
  for(int i = 0; i <= DIFF_SCALE; i++) {
    // println("spawing enemy");
    Enemy e = new Enemy(new PVector(rand((int)enemy_size, WIDTH - (int)enemy_size),enemy_y,-9), enemy_size, color(10,10,250));
    enemies.add(e);
  }
}


// perform hitreg checks for all
// mutual destruction for all impacts
void hitChecks() {
  // player hitreg
  if(player != null) {
    // -> hit enemy = die
    for(int i = 0; i < enemies.size(); i++) {
      Enemy e = enemies.get(i);
      if(player.hitReg(e)) {
        // game over
        e.mark();
        player.mark();
        return;
      }
    }
    // -> hit enemy projectile = die
    for(int i = 0; i < projectiles.size(); i++) {
      Projectile p = projectiles.get(i);
      if(player.team != p.team & player.hitReg(p)) {
        // game over
        p.mark();
        player.mark();
        return;
      }
    }  
  }
  
  // enemy hitreg
  // -> hit player = die -> taken care of in player check
  // -> hit player projectile = die
  for(int i = 0; i < enemies.size(); i++) {
    for(int j = 0; j < projectiles.size(); j++) {
      Enemy e = enemies.get(i);
      Projectile p = projectiles.get(j);
      if(e.team != p.team & e.hitReg(p)) {
        // decrement i & j
        i--;
        j--;
        if(j < 0) j = 0;
        if(i < 0) i = 0;
        // delete particles
        e.mark();
        p.mark();
        // spawn new enemies
        spawnEnemies();
        // increase score
        score += 10;
        // keep checking
      }
    }
  }
  
  // projectile hitreg
  // -> hit enemy projectile = die
  for(int i = 0; i < projectiles.size(); i++) {
    for(int j = 1; j < projectiles.size(); j++) {
      Projectile p1 = projectiles.get(i);
      Projectile p2 = projectiles.get(j);
      if(p1.team != p2.team & p1.hitReg(p2)) { // collision between opposite teams
        // decrement counters
        i--;
        j--;
        if(j < 0) j = 0;
        if(i < 0) i = 0;
        // kill particles
        p1.mark();
        p2.mark();
        // increase score
        score += 2;
      } // no collision, keep checking
    }
  }
}

// performs a single simulation step
void simStep() {
  if(player != null) // player not dead
    player.step();
  
  // step for dying objects (also steps their debris)
  for(int i = 0; i < dying.size(); i++) { 
    dying.get(i).step(); 
  }
  
  for(int i = 0; i < projectiles.size(); i++){
    projectiles.get(i).step();
  }
  
  for(int i = 0; i < enemies.size(); i++){
    enemies.get(i).step();
  }
  
  if(playerShooting && player != null) { 
    // player counts frames between shots every step, up to PLAYER_FIRE_RATE
    player.shoot();
  }
  
  // adjust difficulty
  if(global_frame_count % (DIFF_TIMER * (DIFF_SCALE+1)) == 0) {
    DIFF_SCALE++;
    println("Difficulty UP!");
  }
  
  hitChecks();
  
  // increase score
  if(global_frame_count % frames_till_score == 0)
    score++;
  
  global_frame_count++;
} // draw() calls itself every frame, no need to call it here

void drawStars() {
  push();
  translate(0,0,-120);
  for(int i = 0; i < STAR_COUNT; i++) {
    fill(color(255));
    circle(star_pos[i][0], star_pos[i][1], 1);
  }
  pop();
}

void drawBlackhole() {
  PImage img = loadImage("images/blackHole.png");
  noStroke();
  push();
  translate(0,0,-100);
  beginShape();
  texture(img);
  vertex(0, HEIGHT/3 + 100, 0, 0);
  vertex(WIDTH, HEIGHT/3 + 100, 1, 0);
  vertex(WIDTH, HEIGHT/3 + 800, 1, 1);
  vertex(0, HEIGHT/3 + 800, 0, 1);
  endShape(CLOSE);
  pop();
}

void draw() {
  background(0);
  // draw stuff
  // draw background
  drawStars();
  drawBlackhole();
  // draw dying
  for(int i = 0; i < dying.size(); i++) {
    dying.get(i).drawParticle(); 
  }
  
  // draw enemies
  for(int i = 0; i < enemies.size(); i++) {
    enemies.get(i).drawParticle();
  }
  // draw projectiles
  for(int i = 0; i < projectiles.size(); i++){
    projectiles.get(i).drawParticle();
  }
  
  
  //draw player, unless they're dying
  if(player != null) player.drawParticle();
  
  // draw score counter
  fill(color(255));
  textSize(32);
  textAlign(LEFT);
  String t = "Score: " + score;
  text(t, 36,36);
  // step simulation
  if(!game_over) {
    simStep();
  } else { // game is done
    fill(color(255));
    textSize(72);
    textAlign(CENTER);
    text("GAME OVER", WIDTH/2,HEIGHT/2);
  }
}
