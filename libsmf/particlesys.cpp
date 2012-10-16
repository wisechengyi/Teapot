#include "particlesys.h"



ParticleSystem::ParticleSystem()
{

}

ParticleSystem::~ParticleSystem()
{

}

void ParticleSystem::particleRead(char const * const filename)
{

	ObjRead(filename);
	
	v.resize(vertices.size());
	f.resize(vertices.size());
	m.resize(vertices.size());
	
	
//	initialize mass
	for (unsigned i=0;i<m.size();i++)
	{
		m[i]=1.0;
	}
	
	//initialize speed
	for (unsigned i=0;i<v.size(); i++)
	{
		v[i]={0.0,0.0,0.0};
	}
	
	//initialize force
	for (unsigned i=0;i<f.size() ;i++)
	{
		f[i]={0.0,0.0,0.0};
	}
	
	

}

void ParticleSystem::updateParticles()
{
	v.resize(vertices.size());
	f.resize(vertices.size());
	m.resize(vertices.size());
	
	
//	initialize mass
	for (unsigned i=0;i<m.size();i++)
	{
		m[i]=1.0;
	}
	
	//initialize speed
	for (unsigned i=0;i<v.size(); i++)
	{
		v[i]={5.0,double(i%10),0.0};
	}
//	
//	//initialize force
//	for (unsigned i=0;i<f.size() ;i++)
//	{
//		f[i]={0.0,-1.0,0.0};
//	}
}

void ParticleSystem::simulate(float DeltaT)
{

	//need to initialize force every iteration
	
	for (unsigned i=0;i<f.size() ;i++)
	{
		f[i]={0.0,-9.0,0.0};
	}
//	f[0]={-.5,-0.1,-0.1};
//	f[6]={ .5, 0.1,0.1};
	
	//for all spring related particle calculate the force
	
	double const K=10.0; //spring constant
	double const zeta=0.05; //damping ratio
	
	for (map< pair<int,int>, float >::iterator it=springConnection.begin();it!=springConnection.end();it++)
	{
		float rest = it->second;
		Vector3D particleDistance= vertices[ it->first.first]->co - vertices[it->first.second]->co;
		double l = norm(particleDistance);
		
		unitize(particleDistance);
		
		Vector3D force = K*( rest- l)*particleDistance ;
		
		f[it->first.first]=f[it->first.first]+force-v[it->first.first]*zeta;
		f[it->first.second]=f[it->first.second]-force-v[it->first.second]*zeta;
	
	
	}


//	#pragma omp parallel for
	for (unsigned i=0; i<v.size();i++)
	{
		v[i] = ((f[i]/m[i])*DeltaT)+v[i];
		if (vertices[i]->co[1]<=0)
		{
			v[i]=-v[i];
		}
	}
//	#pragma omp parallel for
	for (unsigned i=0; i<v.size(); i++)
	{
		Vector3D offset=DeltaT*v[i];
		vertices[i]->co=vertices[i]->co+offset;
	}

}	

