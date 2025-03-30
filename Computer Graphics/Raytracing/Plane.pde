class Plane extends Shape {
  
  PVector normal; // defines normal to plane
  
  Plane(PVector c, boolean p, float[] c2, PVector n){
    super(c,p,c2);
    this.normal = n.copy().normalize();
  }
  
  // gets color at point on surface
  float[] getColor(PVector point, PVector incident){
    if(!this.phong){
     return this.col;
    } else {
     // do phong calculation at point
     float[][] colors = {this.col, this.col, PHONG_COLORS[S]}; // ambient color is object color
     return phong(point, this.getNormal(point), EYE, LIGHT, MATERIAL, colors, PHONG_SHININESS, this);
    }
  }
  
  PVector getNormal(PVector point){
    return this.normal.copy();
  }
  
  float intersectDist(PVector ray){
   // ray scaled by 't'
   return this.normal.dot(this.center) / this.normal.dot(ray);
  }
  
  
  float intersectDist(PVector ray, PVector from){
    return (this.normal.dot(this.center) - this.normal.dot(from)) / this.normal.dot(ray);
  }
}
