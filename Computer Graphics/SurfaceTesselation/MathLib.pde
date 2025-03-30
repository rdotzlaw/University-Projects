/*
Define any mathematical operations that you need here.
 E.g., the 2D cross product as discussed in Unit 1.
 */
 
 
 // computes area of triangle from projected vertices
 // from processing forums
 float triArea(PVector[] vertices){
   PVector v0 = vertices[0];
   PVector v1 = vertices[1];
   PVector v2 = vertices[2];
   //abs(x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2.0;
   return abs(v0.x * (v1.y - v2.y) + v1.x * (v2.y - v0.y) + v2.x * (v0.y - v1.y)) / 2.0;
 }
 
 // computes area from adjacent vectors
 float triAreaVector(PVector e1, PVector e2){
  return 0.5 * (e1.cross(e2)).z; 
 }
 
 // calculates the barycentric coordinates of p w.r.t. triangle t
 // p is a 2D ponint, t is a 3d triangle
 // it can be assumed that p is within t in 2d projection and 3d space
 // p's 3d counterpart P = uV1 + vV2 + wV3
 // p's normal is N = uN1 + vN2 + wN3 (Ni is Vi's normal)
 // where (u,v,w) are the barycentric coordinates of p w.r.t. t
 // in this case, we're using projected stuff
 // A1 = (1/2)(e2 x p2) = (1/2)(e2 x (p - v2))
 // A2 = (1/2)(e3 x p3) = (1/2)(e3 x (p - v3))
 // A3 = (1/2)(e1 x p1) = (1/2)(e1 x (p - v1))
 // u = A1 / A, v = A2 / A, w = A3 / A
 // A = (1/2)det(e1 x e2)
 float[] barycentricCoords(PVector p, Triangle t){
   float A = (t.edges[1].copy().cross(t.edges[1])).mult(0.5).z; // just getting z-coord
   
   float A1 = triAreaVector(t.projEdges[1], p.sub(t.projVertices[1])); // e2 x (p - v2)
   float A2 = triAreaVector(t.projEdges[2], p.sub(t.projVertices[2])); // e3 x (p - v3)
   float A3 = triAreaVector(t.projEdges[0], p.sub(t.projVertices[0])); // e1 x (p - v1)
   
   float[] result = {A1/A, A2/A, A3/A};
   return result;
 }
 
