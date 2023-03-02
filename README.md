# bullet-physics-performance-optimized
A Bullet Physics Engine clone for devices like Nintendo DS

Here is exmple of common physics setup using bullet

//should be called once on game start 
void setupPhysics()
{
	//btScalar gJitterVelocityDampingFactor = 1.f;

	btCollisionDispatcher* dispatcher = new btCollisionDispatcher();
	btVector3 worldMin(-1000,-1000,-1000);
	btVector3 worldMax(1000,1000,1000);
	btOverlappingPairCache* pairCache = new btAxisSweep3(worldMin,worldMax);
	btConstraintSolver* constraintSolver = new btSequentialImpulseConstraintSolver();
	m_dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,pairCache,constraintSolver);
	//m_dynamicsWorld = new btSimpleDynamicsWorld(dispatcher,pairCache,constraintSolver);

	m_dynamicsWorld->setGravity(btVector3(0,0,-10));
	m_dynamicsWorld->setDebugDrawer(&m_ebugDrawer);

}



void UpdatePhysics()
{
		m_dynamicsWorld->stepSimulation(1.f/15.f,0); 
}
