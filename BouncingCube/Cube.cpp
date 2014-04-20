#include "StdAfx.h"
#include "Cube.h"

CCube::CCube(double size)
{
	// Vertices of a unit cube centered at origin, sides aligned with axes
	vertex_positions[0] = point4( -size, -size,  size, 1.0 );
	vertex_positions[1] = point4( -size,  size,  size, 1.0 );
	vertex_positions[2] = point4(  size,  size,  size, 1.0 );
	vertex_positions[3] = point4(  size, -size,  size, 1.0 );
	vertex_positions[4] = point4( -size, -size, -size, 1.0 );
	vertex_positions[5] = point4( -size,  size, -size, 1.0 );
	vertex_positions[6] = point4(  size,  size, -size, 1.0 );
	vertex_positions[7] = point4(  size, -size, -size, 1.0 );


	int Index = 0;

	quad( 1, 0, 3, 2 , vertex_positions, Index);
	quad( 2, 3, 7, 6 ,  vertex_positions, Index);
	quad( 3, 0, 4, 7 ,  vertex_positions, Index);
	quad( 6, 5, 1, 2 ,  vertex_positions, Index);
	quad( 4, 5, 6, 7 ,  vertex_positions, Index);
	quad( 5, 4, 0, 1 ,  vertex_positions, Index);

	//Initialize the motion
	v = vec3(15.0, 0., 0.);
	w = vec3(1., 1., 1.);
}

void CCube::quad(int a, int b, int c, int d, point4 * vertex_positions, int& Index)
{
	vec3 u = vec3(vertex_positions[b]-vertex_positions[a]);
	vec3 v = vec3(vertex_positions[c]-vertex_positions[b]);

	vec3 normal = normalize(cross(u,v));

	normals[Index] = normal; points[Index] = vertex_positions[a]; 
	tex_coords[Index] = vec2(0.f, 0.f);
	Index++;
	normals[Index] = normal; points[Index] = vertex_positions[b]; 
	tex_coords[Index] = vec2(0.f, 1.f);
	Index++;
	normals[Index] = normal; points[Index] = vertex_positions[c]; 
	tex_coords[Index] = vec2(1.f, 1.f);
	Index++;
	normals[Index] = normal; points[Index] = vertex_positions[a]; 
	tex_coords[Index] = vec2(0.f, 0.f);
	Index++;
	normals[Index] = normal; points[Index] = vertex_positions[c]; 
	tex_coords[Index] = vec2(1.f, 1.f);
	Index++;
	normals[Index] = normal; points[Index] = vertex_positions[d]; 
	tex_coords[Index] = vec2(1.f, 0.f);
	Index++;
}


CCube::~CCube(void)
{
}

void CCube::InitGL(GLuint program)
{
	// Create and initialize a buffer object
	glGenVertexArrays( 1, &vao);
	glBindVertexArray( vao);
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) +  sizeof(tex_coords) +        sizeof(normals), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0,                sizeof(points), value_ptr(points[0]));
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(normals), value_ptr(normals[0]));
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(normals), sizeof(tex_coords), value_ptr(tex_coords[0]));

	//set up vertex arrays (after shaders are loaded)
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));
	GLuint vTex = glGetAttribLocation(program, "vTexCoord");
	glEnableVertexAttribArray(vTex);
	glVertexAttribPointer(vTex, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)+sizeof(normals)));

}

void CCube::RenderGL(GLuint program)
{

	glBindVertexArray(vao);

	glUniformMatrix4fv(glGetUniformLocation(program,"mT"), 1, GL_FALSE, value_ptr(m_translation));
	glUniformMatrix4fv(glGetUniformLocation(program,"mR"), 1, GL_FALSE, value_ptr(m_rotation));

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0,  NumVertices); 
}

void CCube::Update(double dt)
{
	//update velocity and angular velocity
	v = 0.99*v+ dt* down_vec;
	w = 0.99*w;

	//update translation of center of mass c, and rotation (represented by a quaternion q)
	c = c + vec3(v.x*dt, v.y*dt, v.z*dt);
	q = q + 0.5*dt*w*q;
	float s = 1.f/sqrt(q[0]*q[0]+q[1]*q[1]+q[2]*q[2]+q[3]*q[3]);
	q = s*q;
	m_rotation =mat4_cast(q);
	m_translation = glm::translate(mat4(1.f),c);

	//Handling collision
	vec3 n;
	for (int i = 0; i < 8; i++) {
		vec4 contact = m_rotation*vertex_positions[i];
		vec4 p = m_translation * contact;
		bool bCol = false;
		vec3 constraint_f=vec3(0.);

		//Detect collision with which wall
		if (p.x < -10.f) {
			bCol = true;
			n = vec3(1.f, 0.f, 0.f);
			constraint_f.x += -10-p.x;
		} else if (p.x > 10.f) {
			bCol = true;
			n = vec3(-1.f, 0.f, 0.f);
			constraint_f.x += 10-p.x;
		} else if (p.y < -10.f) {
			bCol = true;
			n = vec3(0.f, 1.f, 0.f);
			constraint_f.y += -10-p.y;
		} else if (p.y > 10.f) {
			bCol = true;
			n = vec3(0.f, -1.f, 0.f);
			constraint_f.y += 10-p.y;
		} else if (p.z < -10.f) {
			bCol = true;
			n = vec3(0.f, 0.f, 1.f);
			constraint_f.z += -10-p.z;
		} else if (p.z > 10.f) {
			bCol = true;
			n = vec3(0.f, 0.f, -1.f);
			constraint_f.z += 10-p.z;
		}

		if (bCol) {
			//Handle the collision: compute the impulse j, and update v and w

			vec3 u_rel = v + glm::cross(w, vec3(contact));
			vec3 rxn = glm::cross(vec3(contact), n);
			//For a cube, the impulse j=(1+e)* dot(u_rel,n) / (1+ 6 |r cross n|^2)
			//double j = -1.9f * glm::dot(u_rel,n);
			double j = -1.9f * glm::dot(u_rel,n) / (1.+6.*glm::dot(rxn,rxn));
			if (j > 0) {
				v = v + j*n;
				w = w + 6.*j*glm::cross(vec3(contact), n);
			}

			//Constraint force to prevent drifting down the floor
			constraint_f = dt * 100. * constraint_f;
			v = v + constraint_f;
			w = w + 6.*glm::cross(vec3(contact), constraint_f);
		}
	}
}

//Utility functions
const vec3 operator* (double dt, vec3 v)
{
	return vec3(v.x*dt, v.y*dt, v.z*dt);
}

const quat operator+ (quat q1, quat q2)
{
	return quat (q1[3]+q2[3], q1[0]+q2[0],q1[1]+q2[1],q1[2]+q2[2]);
}

const quat operator* (vec3 v, quat q)
{
	return quat (-v[0]*q[0]-v[1]*q[1]-v[2]*q[2], q[3]*v[0] + v[1]*q[2]-v[2]*q[1] , q[3]*v[1] - v[0]*q[2] + v[2] * q[0], q[3]*v[2] + v[0]*q[1] - v[1]*q[0]);
}

const quat operator* (float s, quat q)
{
	return quat ( q[3]*s, q[0]*s,q[1]*s,q[2]*s);
}

void CCube::SetDown(vec3 down)
{
	down_vec = down * 9.8f;
}
