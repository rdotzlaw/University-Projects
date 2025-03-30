abstract class Shape {
  
  PVector center;
  boolean phong;
  boolean mirror;
  float[] col; // color
  int id;
  
  Shape(PVector c, boolean p, float[] c2){
    this.center = c;
    this.phong = p;
    this.col = c2;
    this.mirror = false;
    this.id = shape_count;
    shape_count++;
  }
  
  abstract PVector getNormal(PVector p);
  
  // returns -1.0 if no intersect
  // otherwise, returns distance between EYE and intersect
  abstract float intersectDist(PVector ray); // from = EYE
  // returns distance between 'from' and intersect
  abstract float intersectDist(PVector ray, PVector from);
  
  abstract float[] getColor(PVector point, PVector incident);
  
}
