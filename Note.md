# Phsyics Simulation note  

## Overview of Project  

- Research physics simulation and utilize it for dental treatment.  
- e.g.) Integrate with Ortho simuration - simulate effects onto joint from occlusion, Pressures on gum from teeth move by bracket or aligner, treatment simulation with fruid simulatoin for hygienist (Syringe, Vacuum)  

## Milestone (updated 1/11/2024)  

1. Download the sample program and run to understand the principle and method  
	1. Solid (Rigid body)
		1. Equation of motion basically  
	2. Softbody (implemented on NVDIA GPU) >> no samples
		1. Finite Element Method (有限要素法) >> heavy, accurate
		2. PBD - Position Based Dynamics >> Faster, poor accuracy >> Fluid, Blast, Cloth
2. Read textbook, paper regarding phisics simulation  
3. Breakdown the source code of the sample program  
4. Import 3D data of dental arch CBCT, IOS, FS >> merge them??

## Environment  

- Root Dir: E:\Physics_Simulation
- Use PhysX SDK by NVDIA
- GPU: NVDIA GeForce RTX 4070
- Setup refference page <https://qiita.com/hama1080/items/399e60f4c9c1bfd0640c> <- this article is for an old vertion of PhysX. 


## Note  

### 12/28/2023  

* setup the environment - installed CMake, PhysX (Python, VS2019, git already installed)
- build environment toolset - cofigured for VC16 by generate_project.cmd
- Had to install Microsoft DirectX SDK (June 2010) as some lib/dll are missing e.g.) d3dx9.h -> copy .h files to C:\Program Files (x86)\Windows Kits\10\Include\10.0.19041.0\shared for PhysX SDK build
- Had to change the additional library directories in KaplaDemo\Linker\General property (changed directory path from ./../../../../physx/bin/win.x86_64.VC140.mt/$(ConfigurationName) to ./../../../../physx/bin/win.x86_64.VC142.mt/$(ConfigurationName)) as SDK was builed by VC142.
- Had to change the reference library path in KaplaDemo\Build Events\Post-Build Event property (changed directory path from ./../../../../physx/bin/win.x86_64.VC140.mt/$(ConfigurationName) to ./../../../../physx/bin/win.x86_64.VC142.mt/$(ConfigurationName)) as SDK was builed by VC142.
- Demo worked anyways (E:\Physics_Simulation\git\PhysX\kaplademo\bin\VC14WIN64\RELEASE\KaplaDemo.exe)

### 1/11/2024  

- Updated milestone  
- Mainly research the methods of caliculation for simutation >> updated on milestone  
- Focused on Fluid simulation >> utilize with FS to see how skin moves interactively along with joint motions  
- What to do the next:s
	- find a book to learn analysis and caliculation method for pysyics simulation  
	- find and try a sample of fluid simulation (ideally both soft body and liquid)  
	- Breakdown the source code of KaplaDeamo  
		
### 1/18/2024  
- Breakdown the source code  
	- Snippet programs (PhysX\physx\snippets)  

### 1/25/2024  
- Breakdown the source code  
	- Snippet programs (PhysX\physx\snippets)
		
	- KaplaDemo
	- Physx  

### 2/1/2024
- Breakdown the source code  
	- Snippet programs (PhysX\physx\snippets)
		
	- KaplaDemo  
	- Physx  
	
### 2/8/2024
API, SDK, libraries for physics simulation (physics engine):
a. ODE (Open Dynamics Engine) - MS
b. Bullet Physics Engine
c. Havok Physics - affiliated by MS
d. PhysX - Nvidia

Noticed the PhysX SDK installed was old version (4.0).. Cloned repository of the latest(5.3.1) from github.

### 2/15/2024

Regular presentation report day.  

- introducing the theme and overview of the project  
- target and schedule  
- achievement and To-do -> create a field of simulation and import .stl or .ply first.

### 2/22/2024

Learning PhysX SDK by documentation - https://nvidia-omniverse.github.io/PhysX/physx/5.3.1/index.html

1. Startup and shutdown -> create an object of PxFoundation - release when closing.

### 2/29/2024

- PxMaterial class: creates a new rigid body material

### 5/13/2024

- Created a sample program: PhysX_test_nonMFC
- seems not to connect with PVD - need to breakdown line 62,

### 6/19/2024

- Using SnippetHelloWorld project for the test.
- need to separate the process for PLY and STL. (Using Open3D to load for both)
	- PLY
		- doesn't matter if the data is point cloud or triangle mesh.
		- Use open3d::io::CreatePointCloudFromFile(string filePath) [std::shared_ptr\<open3d::geometry::PointCloud>]
		- convert Open3D triangle mesh to PhysX triangle mesh object -> open3d::io::ReadTriangleMesh(filename, *mesh) -> extract vertices and indices -> import them to PhysX msshDesc.
		
	- STL
		- load triangle mesh then convert to PhysX triangle mesh. -> samke as PLY.

### 6/25/2024

- Loaded PLY needed initial rotaion as the orientation was not as expected.  
- Need to configure the scene and camera as the loaded object is too big for snippet camera field.
- Time to segmentate the project..

### 6/27/2024

useful sample snippets
- SnippetPathTracing_64
- SnippetStandaloneQuerySystem_64
- Softbody snippet has CUDA activated coding

### 7/4/2024

- cuda activated successfully!
- but RigidDynamic can not be added. App won't launch if RigidDynamic is included..

### 8/8/2024


