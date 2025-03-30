class Cylinder extends Shape {
   // only 'upright' cylinders
   float radius;
   boolean infinite;
   float hi;
   Cylinder(PVector c, boolean p, float[] c2, float r, boolean i){
     super(c,p,c2);
     this.radius = r;
     this.infinite = i;
   }
   
   
   float intersectDist(PVector ray){
    return intersectDist(ray, EYE); 
   }
  
  // normal vector is just vector from center -> point w/ the same y-values
  PVector getNormal(PVector point){
    PVector c = new PVector(this.center.x, 0, this.center.z);
    PVector p = new PVector(point.x, 0, point.z);
    return p.sub(c).normalize();
  }
  
  float intersectDist(PVector ray, PVector from){
    // check body intersect
    // x^2 + z^2 = radius is cylinder equation for cylinder about y-axis
    
    if(this.infinite){
     // any point in 3d space that can create a vector of magnitude
     // this.radius between itself and the point (this.center.x, y, this.center.z) for arbitrary 'y'
     // is a point on the cylinder
     // so if the ray: R(t) = from + t*ray can have a 't' that creates such a point
     // then the ray will intersect the cylinder
     // so solve ((from.x + t*ray.x) - this.center.x)^2 + ((from.z + t*ray.z) - this.center.z)^2 = r^2 for t
     float x = from.x - this.center.x;
     float z = from.z - this.center.z;
     float a = pow(ray.x, 2) + pow(ray.z, 2);
     float b = 2.0 * ((ray.x * x) + (ray.z * z));
     float c = pow(x, 2) + pow(z, 2) - pow(this.radius, 2);
      
     // apply quadratic formula
     float desc = b*b - (4.0 * a * c);
     if(desc < 0.0) return -1.0; // imaginary root
     float root = sqrt(desc);
      
     float t1 = (-b + root) / (2.0 * a);
     float t2 = (-b - root) / (2.0 * a);
       //<>//
      
     // check t's
     float res;
     // if both t's are negative, cylinder is behind camera
     if(t1 < 0.0 && t2 < 0.0){
       res = -1.0;
     } else if(t1 > 0.0 ^ t2 > 0.0){ // if one is positive, and one is negative (or zero), we're inside the cylinder (^ == XOR operator)
       // do we want to draw the inside of the cylinder?
       res = -1.0; // no, we don't
       // res = max(t1, t2); // yes, we do
     } else if(t1 > 0.0 && t2 > 0.0){ // both positive, cylinder is in front of camera
       res = min(t1, t2); // min of t's is the closest intersect -> front part of sphere
     } else if(t1 == 0.0 || t2 == 0.0) { // ray intersecting with 'this', no bueno
       res = -10.0;
     } else {
       println("Problem with sphere intersectDist");
       res = -10.0;
     }
     
     return res; 
    }
    
    // if not infinite, check endcaps intersect
    if(!this.infinite){
      // dead code, didn't do any non-infinite cylinders
    }
    
    return -1.0f;
  }
  
  float[] getColor(PVector point, PVector incident){
   if(!this.phong) return this.col;
   // do phong stuff
   PVector p = point.copy();
   PVector norm = this.getNormal(point);
   float[][] colors = {this.col, this.col, PHONG_COLORS[S]}; // ambient color is object color
   return phong(p, norm, EYE, LIGHT, MATERIAL, colors, PHONG_SHININESS, this);
  }
   
}
