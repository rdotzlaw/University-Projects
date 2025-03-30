class Circle extends Plane {
  
 float radius;
 Shape root;
 
 Circle(PVector c, boolean p, float[] c2, PVector n, float r){
    super(c,p,c2,n);
    this.radius = r;
 }
 
 Circle(PVector c, boolean p, float[] c2, PVector n, float r, Shape cyl){
    super(c,p,c2,n);
    this.radius = r;
    this.root = cyl; // a part of root object (cylinder)
 }
 
 float intersectDist(PVector ray, PVector from){
   float t = super.intersectDist(ray,from);
   if(t < 0.0) return t; // doesn't hit plane
   // ensure 't' results in a point w/ in the wide & long bounds of the circle
   
   // vector starting at 'from' going in direction 'ray', with magnitute 't'
   
   PVector intersection = ray.copy().mult(t).add(from); // POI
   float distToCent = intersection.sub(this.center).mag();
   
   if(distToCent <= this.radius) return t;
   else return -1.0;

 }
}
