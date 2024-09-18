// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ''AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Copyright (c) 2008-2023 NVIDIA Corporation. All rights reserved.
// Copyright (c) 2004-2008 AGEIA Technologies, Inc. All rights reserved.
// Copyright (c) 2001-2004 NovodeX AG. All rights reserved.  

// ****************************************************************************
// This snippet illustrates simple use of physx
//
// It creates a number of box stacks on a plane, and if rendering, allows the
// user to create new stacks and fire a ball from the camera position
// ****************************************************************************

#include <ctype.h>
#include <iostream>
#include "PxPhysicsAPI.h"
#include "gpu/PxPhysicsGpu.h"
#include "../snippetcommon/SnippetPrint.h"
#include "../snippetcommon/SnippetPVD.h"
#include "../snippetutils/SnippetUtils.h"
#include "../../lib/Open3D.h"

using namespace physx;

static PxDefaultAllocator		gAllocator;
static PxDefaultErrorCallback	gErrorCallback;
static PxFoundation*			gFoundation = NULL;
static PxPhysics*				gPhysics	= NULL;
static PxDefaultCpuDispatcher*	gDispatcher = NULL;
static PxScene*					gScene		= NULL;
static PxMaterial*				gMaterial	= NULL;
static PxPvd*					gPvd        = NULL;
static PxCudaContextManager*	gCudaContextManager = NULL;

static PxReal stackZ = 10.0f;

char* objectPath1 = nullptr;
char* objectPath2 = nullptr;

// Function to load point cloud or mesh from .ply file **not used**
std::shared_ptr<open3d::geometry::Geometry> LoadPLY(const std::string& ply_path) {
	auto mesh = open3d::io::CreateMeshFromFile(ply_path);
	if (mesh) {
		return mesh;
	}
	else {
		auto point_cloud = open3d::io::CreatePointCloudFromFile(ply_path);
		if (point_cloud) {
			return point_cloud;
		}
		else {
			std::cerr << "Error: Failed to load geometry from file." << std::endl;
			return nullptr;
		}
	}
}

/* Load triangle mesh from a file (only confirmed .ply and .stl) using Open3D then transform 3D object from open3D triangle mesh to PhysX triangle mesh.
Then add 3D model to static actor.*/
static PxRigidStatic* createStaticRigidBodyFromFile(const std::string& filename, std::vector<float>& vertices, std::vector<unsigned int>& indices) 
{
	// Read PLY file using Open3D
	auto mesh = std::make_shared<open3d::geometry::TriangleMesh>();
	if (!open3d::io::ReadTriangleMesh(filename, *mesh)) {
		std::cerr << "Failed to load STL file: " << filename << std::endl;
		return false;
	}

	// Extract vertices
	for (const auto& vertex : mesh->vertices_) {
		vertices.push_back(static_cast<float>(vertex(0)));
		vertices.push_back(static_cast<float>(vertex(1)));
		vertices.push_back(static_cast<float>(vertex(2)));
	}

	// Extract indices
	for (const auto& triangle : mesh->triangles_) {
		indices.push_back(static_cast<unsigned int>(triangle(0)));
		indices.push_back(static_cast<unsigned int>(triangle(1)));
		indices.push_back(static_cast<unsigned int>(triangle(2)));
	}

	// Create PhysX triangle mesh
	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = static_cast<PxU32>(vertices.size() / 3);
	meshDesc.points.stride = sizeof(float) * 3;
	meshDesc.points.data = vertices.data();

	meshDesc.triangles.count = static_cast<PxU32>(indices.size() / 3);
	meshDesc.triangles.stride = sizeof(unsigned int) * 3;
	meshDesc.triangles.data = indices.data();

	meshDesc.flags = PxMeshFlags();

	PxCookingParams cookingParams(gPhysics->getTolerancesScale());
	PxDefaultMemoryOutputStream writeBuffer;
	if (!PxCookTriangleMesh(cookingParams, meshDesc, writeBuffer)) {
		std::cerr << "Error cooking mesh." << std::endl;
		return nullptr;
	}

	// Create triangle mesh from cooked data

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	PxTriangleMesh* gTriangleMesh = gPhysics->createTriangleMesh(readBuffer);

	PxTriangleMeshGeometry gMesh{};
	gMesh.triangleMesh = gTriangleMesh;

	// Create PhysX rigid body
	PxShape* meshShape = gPhysics->createShape(gMesh, *gMaterial);
	meshShape->setLocalPose(PxTransform(PxIdentity));
	PxRigidStatic* staticActor = PxCreateStatic(*gPhysics, PxTransform(PxVec3(10, 10, 10)), *meshShape);
	PxQuat rotation(PxPi / 4, PxVec3(0, 1, 0));

	return staticActor;

}

/* Load triangle mesh from a file (only confirmed .ply and .stl) using Open3D then transform 3D object from open3D triangle mesh to PhysX triangle mesh.
Then add 3D model to static actor.*/
static PxRigidDynamic* createDynamicRigidBodyFromFile(const std::string& filename, std::vector<float>& vertices, std::vector<unsigned int>& indices)
{
	// Read PLY file using Open3D
	auto mesh = std::make_shared<open3d::geometry::TriangleMesh>();
	if (!open3d::io::ReadTriangleMesh(filename, *mesh)) {
		std::cerr << "Failed to load STL file: " << filename << std::endl;
		return false;
	}

	// Extract vertices
	for (const auto& vertex : mesh->vertices_) {
		vertices.push_back(static_cast<float>(vertex(0)));
		vertices.push_back(static_cast<float>(vertex(1)));
		vertices.push_back(static_cast<float>(vertex(2)));
	}

	// Extract indices
	for (const auto& triangle : mesh->triangles_) {
		indices.push_back(static_cast<unsigned int>(triangle(0)));
		indices.push_back(static_cast<unsigned int>(triangle(1)));
		indices.push_back(static_cast<unsigned int>(triangle(2)));
	}

	PxSDFDesc sdfDesc;
	sdfDesc.spacing = 0.1f;
	sdfDesc.subgridSize = 32;
	sdfDesc.bitsPerSubgridPixel = PxSdfBitsPerSubgridPixel::e16_BIT_PER_PIXEL;
	sdfDesc.numThreadsForSdfConstruction = 8;

	bool enableGpuAcceleratedCooking = true;
	if (enableGpuAcceleratedCooking)
	{
		//If sdfBuilder is NULL, the sdf will get cooked on the CPU using the number of threads specified above
		sdfDesc.sdfBuilder = PxGetPhysicsGpu()->createSDFBuilder(gCudaContextManager);
	}

	// Create PhysX triangle mesh
	PxTriangleMeshDesc meshDesc;
	meshDesc.sdfDesc = &sdfDesc;
	meshDesc.points.count = static_cast<PxU32>(vertices.size() / 3);
	meshDesc.points.stride = sizeof(float) * 3;
	meshDesc.points.data = vertices.data();

	meshDesc.triangles.count = static_cast<PxU32>(indices.size() / 3);
	meshDesc.triangles.stride = sizeof(unsigned int) * 3;
	meshDesc.triangles.data = indices.data();

	meshDesc.flags = PxMeshFlags();

	PxCookingParams cookingParams(gPhysics->getTolerancesScale());
	PxDefaultMemoryOutputStream writeBuffer;
	if (!PxCookTriangleMesh(cookingParams, meshDesc, writeBuffer)) {
		std::cerr << "Error cooking mesh." << std::endl;
		return nullptr;
	}

	// Create triangle mesh from cooked data

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	PxTriangleMesh* gTriangleMesh = gPhysics->createTriangleMesh(readBuffer);

	PxTriangleMeshGeometry gMesh{};
	gMesh.triangleMesh = gTriangleMesh;
	// Create PhysX rigid body
	PxShape* meshShape = gPhysics->createShape(gMesh, *gMaterial);
	meshShape->setLocalPose(PxTransform(PxIdentity));
	PxRigidDynamic* dynamicActor = PxCreateKinematic(*gPhysics, PxTransform(PxVec3(10, 10, 10)), *meshShape, 100.0f);
	PxQuat rotation(PxPi / 4, PxVec3(1, 1, 0));
	dynamicActor->setAngularDamping(0.5f);
	dynamicActor->setLinearVelocity(PxVec3(0));

	return dynamicActor;

}

// Function to create a 3D triangle mesh model
static PxRigidDynamic* CreateTriangleMeshDynamic(const PxVec3* verts, const PxU32 numVerts,
	const PxU32* indices, const PxU32 numIndices) {
	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = numVerts;
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = verts;

	meshDesc.triangles.count = numIndices / 3;
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = indices;

	meshDesc.flags = PxMeshFlags();

	// Cook the triangle mesh
	PxCookingParams cookingParams(gPhysics->getTolerancesScale());
	PxDefaultMemoryOutputStream writeBuffer;
	if (!PxCookTriangleMesh(cookingParams, meshDesc, writeBuffer)) {
		std::cerr << "Error cooking mesh." << std::endl;
		return nullptr;
	}

	// Create triangle mesh from cooked data
	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	PxTriangleMesh* gtriangleMesh = gPhysics->createTriangleMesh(readBuffer);

	PxTriangleMeshGeometry mesh{};
	mesh.triangleMesh = gtriangleMesh;

	PxShape* meshShape = gPhysics->createShape(mesh, *gMaterial);
	// Setting the initial local position (setting in triangleRigitStatic definition)
	meshShape->setLocalPose(PxTransform(PxIdentity));
	PxRigidDynamic* triangleRigitDynamic = PxCreateKinematic(*gPhysics, PxTransform(PxVec3(20, 20, 20)), *meshShape, 10.0f);

	return triangleRigitDynamic;
}

// Function to create a 3D triangle mesh model
static PxRigidStatic* CreateTriangleMeshStatic(const PxVec3* verts, const PxU32 numVerts,
	const PxU32* indices, const PxU32 numIndices) {
	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = numVerts;
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = verts;

	meshDesc.triangles.count = numIndices / 3;
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = indices;

	meshDesc.flags = PxMeshFlags();

	// Cook the triangle mesh
	PxCookingParams cookingParams(gPhysics->getTolerancesScale());
	PxDefaultMemoryOutputStream writeBuffer;
	if (!PxCookTriangleMesh(cookingParams, meshDesc, writeBuffer)) {
		std::cerr << "Error cooking mesh." << std::endl;
		return nullptr;
	}

	// Create triangle mesh from cooked data
	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	PxTriangleMesh* gtriangleMesh = gPhysics->createTriangleMesh(readBuffer);

	PxTriangleMeshGeometry mesh{};
	mesh.triangleMesh = gtriangleMesh;

	PxShape* meshShape = gPhysics->createShape(mesh, *gMaterial);
	// Setting the initial local position (setting in triangleRigitStatic definition)
	meshShape->setLocalPose(PxTransform(PxIdentity));
	PxRigidStatic* triangleRigitStatic = PxCreateStatic(*gPhysics, PxTransform(PxVec3(20, 20, 20)), *meshShape);

	return triangleRigitStatic;
}

static PxRigidDynamic* createDynamic(const PxTransform& t, const PxGeometry& geometry, const PxVec3& velocity=PxVec3(0))
{
	PxRigidDynamic* dynamic = PxCreateDynamic(*gPhysics, t, geometry, *gMaterial, 10.0f);
	dynamic->setAngularDamping(0.5f);
	dynamic->setLinearVelocity(velocity);
	gScene->addActor(*dynamic);
	return dynamic;
}

static void createStack(const PxTransform& t, PxU32 size, PxReal halfExtent)
{
	PxShape* shape = gPhysics->createShape(PxBoxGeometry(halfExtent, halfExtent, halfExtent), *gMaterial);
	for(PxU32 i=0; i<size;i++)	
	{
		for(PxU32 j=0;j<size-i;j++)
		{
			PxTransform localTm(PxVec3(PxReal(j*2) - PxReal(size-i), PxReal(i*2+1), 0) * halfExtent);
			PxRigidDynamic* body = gPhysics->createRigidDynamic(t.transform(localTm));
			body->attachShape(*shape);
			PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
			gScene->addActor(*body);
		}
	}
	shape->release();
}

void initPhysics(bool interactive)
{
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport,PxPvdInstrumentationFlag::eALL);

	// initialize cuda
	PxCudaContextManagerDesc cudaContextManagerDesc;
	gCudaContextManager = PxCreateCudaContextManager(*gFoundation, cudaContextManagerDesc, PxGetProfilerCallback());
	if (gCudaContextManager && !gCudaContextManager->contextIsValid())
	{
		gCudaContextManager->release();
		gCudaContextManager = NULL;
		printf("Failed to initialize cuda context.\n");
	}

	PxTolerancesScale scale;
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(),true,gPvd);
	PxInitExtensions(*gPhysics, gPvd);

	PxCookingParams params(scale);
	params.meshWeldTolerance = 0.001f;
	params.meshPreprocessParams = PxMeshPreprocessingFlags(PxMeshPreprocessingFlag::eWELD_VERTICES);
	params.buildTriangleAdjacencies = false;
	params.buildGPUData = true;

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	// cuda 
	if (!sceneDesc.cudaContextManager)
		sceneDesc.cudaContextManager = gCudaContextManager;

	sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
	sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;

	PxU32 numCores = SnippetUtils::getNbPhysicalCores();
	gDispatcher = PxDefaultCpuDispatcherCreate(numCores == 0 ? 0 : numCores - 1);
	//cuda end

	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher	= gDispatcher;
	sceneDesc.filterShader	= PxDefaultSimulationFilterShader;
	sceneDesc.flags |= PxSceneFlag::eENABLE_ACTIVE_ACTORS;

	//cuda
	sceneDesc.sceneQueryUpdateMode = PxSceneQueryUpdateMode::eBUILD_ENABLED_COMMIT_DISABLED;
	sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;
	sceneDesc.gpuMaxNumPartitions = 8;

	sceneDesc.solverType = PxSolverType::eTGS;
	//cuda end
	gScene = gPhysics->createScene(sceneDesc);

	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if(pvdClient)
	{
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);

	PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0,1,0,0), *gMaterial);
	gScene->addActor(*groundPlane);

	 //Create 3D triangle mesh models (cube and rectangle box)
	 //Vertices for a cube with sides of 5 units
		PxVec3 cubeVerts[] = {
		PxVec3(-2.5f, -2.5f, -2.5f), // Front bottom left
		PxVec3(-2.5f, -2.5f,  2.5f), // Front bottom right
		PxVec3(-2.5f,  2.5f, -2.5f), // Front top left
		PxVec3(-2.5f,  2.5f,  2.5f), // Front top right
		PxVec3(2.5f, -2.5f, -2.5f), // Back bottom left
		PxVec3(2.5f, -2.5f,  2.5f), // Back bottom right
		PxVec3(2.5f,  2.5f, -2.5f), // Back top left
		PxVec3(2.5f,  2.5f,  2.5f)  // Back top right
	};

	PxU32 cubeIndices[] = {
		0, 1, 2,
		1, 3, 2,
		4, 6, 5,
		5, 6, 7,
		0, 2, 4,
		4, 2, 6,
		1, 5, 3,
		5, 7, 3,
		0, 4, 1,
		4, 5, 1,
		2, 3, 6,
		6, 3, 7
	};

	PxRigidDynamic* cube = CreateTriangleMeshDynamic(cubeVerts, 8, cubeIndices, 36);
	gScene->addActor(*cube);

	//PxRigidStatic* cube = CreateTriangleMeshStatic(cubeVerts, 8, cubeIndices, 36);
	//gScene->addActor(*cube);

	//for (PxU32 i = 0; i < 5; i++)
	//	createStack(PxTransform(PxVec3(0, 0, stackZ -= 10.0f)), 10, 2.0f);
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	//PxRigidStatic* rigidStaticBody1 = createStaticRigidBodyFromFile(objectPath1, vertices, indices);
	//gScene->addActor(*rigidStaticBody1);
	PxRigidDynamic* rigidDynamicBody1 = createDynamicRigidBodyFromFile(objectPath1, vertices, indices);
	gScene->addActor(*rigidDynamicBody1);
	if (!interactive)
		createDynamic(PxTransform(PxVec3(0, 40, 100)), PxSphereGeometry(5), PxVec3(0, -50, -100));
}

void stepPhysics(bool /*interactive*/)
{
	gScene->simulate(1.0f/60.0f);
	gScene->fetchResults(true);
}
	
void cleanupPhysics(bool /*interactive*/)
{
	PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);
	if(gPvd)
	{
		PxPvdTransport* transport = gPvd->getTransport();
		gPvd->release();	gPvd = NULL;
		PX_RELEASE(transport);
	}
	PX_RELEASE(gFoundation);
	
	printf("SnippetHelloWorld done.\n");
}

void keyPress(unsigned char key, const PxTransform& camera)
{
	switch(toupper(key))
	{
	// case 'B':	createStack(PxTransform(PxVec3(0,0,stackZ-=10.0f)), 10, 2.0f);						break;
	case ' ':	createDynamic(camera, PxSphereGeometry(3.0f), camera.rotate(PxVec3(0,0,-1))*200);	break;
	}
}

int snippetMain(int argc, char** argv)
{
	 // Check if file paths are provided as command-line arguments
		if (argc != 2) {
			std::cerr << "Usage: " << argv[0] << " <object_path_1>" << std::endl;
			return 0;
		}

	// Save file paths of 3D objects
		objectPath1 = argv[1];
		objectPath2 = argv[2];

#ifdef RENDER_SNIPPET
	extern void renderLoop();
	renderLoop();
#else
	static const PxU32 frameCount = 100;
	initPhysics(false);
	for(PxU32 i=0; i<frameCount; i++)
		stepPhysics(false);
	cleanupPhysics(false);
#endif
	return 0;
}
