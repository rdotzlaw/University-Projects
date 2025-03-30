class Ray {
  
  PVector source;
  PVector dir;
  Shape sShape;
  
  PVector iPoint; // point of intersection with...
  Shape iShape;
  float iDist;
  
  
  Ray(PVector s, PVector direction){
    this.source = s;
    this.dir = direction;
    this.sShape = null;
    cast();
  }
  
  Ray(PVector s, PVector direction, Shape toExclude){
    this.source = s.copy();
    this.dir = direction.copy();
    this.sShape = toExclude;
    cast();
  }
  
  // casts ray out from 'source' in 'dir' direction
  void cast(){
    
    int closest = -1;
    float closeDist = Float.MAX_VALUE;
    for(int s = 0; s < shapes.length; s++) {
      
      if(shapes[s] != this.sShape){ // exclude source shape for reflections
        float dist = shapes[s].intersectDist(this.dir, this.source);
        
        if(dist > 0.0 && closeDist > dist){ // there is an intersect and new dist is less than prev closest dist
          closeDist = dist;
          closest = s;
        }
      }
    }
    if(closest > -1){
      this.iShape = shapes[closest];
      this.iPoint = calcPOI(closeDist);
      this.iDist = closeDist;
    } else { // no intersect
      this.iShape = null;
      this.iPoint = null;
      this.iDist = -1.0;
    }
    
    
  }
  
  // calculates the point of intersection
  PVector calcPOI(float t){
    PVector res = this.source.copy();
    res.add(this.dir.copy().mult(t));
    return res;
  }
  
}
