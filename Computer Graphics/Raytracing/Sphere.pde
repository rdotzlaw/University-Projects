class Sphere extends Shape {
 
  float radius;
  
  Sphere(PVector c, boolean p, float[] c2, float r){
   super(c, p, c2); 
   this.radius = r;
  }
  
  Sphere(PVector c, boolean p, float[] c2, float r, boolean m){
   super(c, p, c2); 
   this.radius = r;
   this.mirror = m;
  }
  
  // takes point on surface, calculates normal vector at that point
  PVector getNormal(PVector p){
   return p.copy().sub(this.center).normalize(); 
  }
  
  
  float intersectDist(PVector ray){
    return intersectDist(ray, EYE); 
  }
  
  
  float intersectDist(PVector ray, PVector from){
   // plug R(t) into sphere formula and solve for 't'
   PVector centNeg = from.copy().sub(this.center);
   float a = ray.dot(ray); // 1 if ray is normalized; == ray magnitude
   float b = ray.dot(centNeg) * 2.0; // 2(ray . centNeg)
   float c = centNeg.dot(centNeg) - (float)pow(this.radius, 2.0); // centNeg.centNeg - this.radius^2
   
   float desc = b*b - (4.0*a*c);
   if(desc < 0.0) return -1.0;
   float root = sqrt(desc);
   
   float t1 = (-b + root) / (2.0*a);
   float t2 = (-b - root) / (2.0*a);
   
   float res;
   
   // if both t's are negative, sphere is behind camera
   if(t1 < 0.0 && t2 < 0.0){
     res = -1.0;
   } else if(t1 > 0.0 ^ t2 > 0.0){ // if one is positive, and one is negative (or zero), we're inside the sphere (^ == XOR operator)
     // do we want to draw the inside of the sphere?
     res = -1.0; // no, we don't
     // res = max(t1, t2); // yes, we do
   } else if(t1 > 0.0 && t2 > 0.0){ // both positive, sphere is in front of camera
     res = min(t1, t2); // min of t's is the closest intersect -> front part of sphere
   } else if(t1 == 0.0 || t2 == 0.0) { // ray intersecting with 'this', no bueno
     // println("Incident ray cast from sphere");
     res = -10.0;
   } else {
     println("Problem with sphere intersectDist");
     res = -10.0;
   }
   
   return res;
  }
  
  // gets color at point on surface
  float[] getColor(PVector point, PVector incident){
    PVector p = point;
    PVector norm = this.getNormal(point);
    if(this.mirror) { // need to reflect off sphere
        
      if(mirror_debug && abs(norm.dot(LOOK_DIR)) <= 0.3){
         return PHONG_COLORS[0];
      }
      
      // create reflection ray
      PVector start = p.copy(); // no need to offset ray, we exclude 'this' from ray intersect calc
      Ray r = new Ray(start, getReflection(norm, incident.copy().normalize()), this); // exclude shape 'this'
      
      if(r.iShape == null) { // no intersect
        return BLACK;
      } else {
        
        // compute color at intersect point
        float[] c = r.iShape.getColor(r.iPoint, r.dir);
        return c;
      }
    }
    
    if(!this.phong){
     return this.col;
    } else { //<>//
     // non-reflective
     // do phong calculation at point
     float[][] colors = {this.col, this.col, PHONG_COLORS[S]}; // ambient color is object color
     return phong(p, norm, EYE, LIGHT, MATERIAL, colors, PHONG_SHININESS, this);
    }
  }
}
