// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <iomanip> 
#include <iostream>
#include <vector>
#include <sstream>
#include "D:\Downloads\dirent-1.21\include\dirent.h"

#include <Common/Base/hkBase.h>
#include <Common/Base/Memory/System/Util/hkMemoryInitUtil.h>
#include <Common/Base/Memory/Allocator/Malloc/hkMallocAllocator.h>
#include <Common/Base/System/Io/IStream/hkIStream.h>
#include <Common/Base/Reflection/Registry/hkDynamicClassNameRegistry.h>
#include <Common/Base/Reflection/Registry/hkDefaultClassNameRegistry.h>
#include <Common/Compat/Deprecated/Packfile/Binary/hkBinaryPackfileReader.h>
#include <Common/Compat/Deprecated/Packfile/Xml/hkXmlPackfileReader.h>

// Scene
#include <Common/SceneData/Scene/hkxScene.h>
#include <Common/Serialize/Util/hkRootLevelContainer.h>
#include <Common/Serialize/Util/hkNativePackfileUtils.h>
#include <Common/Serialize/Util/hkLoader.h>
#include <Common/Serialize/Version/hkVersionPatchManager.h>

// Physics
#include <Physics/Dynamics/Entity/hkpRigidBody.h>
#include <Physics/Collide/Shape/Convex/Box/hkpBoxShape.h>
#include <Physics/Utilities/Dynamics/Inertia/hkpInertiaTensorComputer.h>

// Animation
#include <Animation/Animation/Rig/hkaSkeleton.h>
#include <Animation/Animation/hkaAnimationContainer.h>
#include <Animation/Animation/Mapper/hkaSkeletonMapper.h>
#include <Animation/Animation/Playback/Control/Default/hkaDefaultAnimationControl.h>
#include <Animation/Animation/Playback/hkaAnimatedSkeleton.h>
#include <Animation/Animation/Rig/hkaPose.h>
#include <Animation/Ragdoll/Controller/PoweredConstraint/hkaRagdollPoweredConstraintController.h>
#include <Animation/Ragdoll/Controller/RigidBody/hkaRagdollRigidBodyController.h>
#include <Animation/Ragdoll/Utils/hkaRagdollUtils.h>
#include <Animation/Animation/Rig/hkaSkeletonUtils.h>

// Serialize
#include <Common/Serialize/Util/hkSerializeUtil.h>






	// [id=keycode]
	#include <Common/Base/keycode.cxx>

	#undef HK_FEATURE_PRODUCT_AI
	//#undef HK_FEATURE_PRODUCT_ANIMATION
	#undef HK_FEATURE_PRODUCT_CLOTH
	#undef HK_FEATURE_PRODUCT_DESTRUCTION_2012
	#undef HK_FEATURE_PRODUCT_DESTRUCTION
	#undef HK_FEATURE_PRODUCT_BEHAVIOR
	#undef HK_FEATURE_PRODUCT_PHYSICS_2012
	#undef HK_FEATURE_PRODUCT_SIMULATION
	#undef HK_FEATURE_PRODUCT_PHYSICS

	// We are using serialization, so we need ReflectedClasses.
	// The objects are being saved and then loaded immediately so we know the version of the saved data is the same 
	// as the version the application is linked with. Because of this we don't need RegisterVersionPatches or SerializeDeprecatedPre700.
	// If the demo was reading content saved from a previous version of the Havok content tools (common in real world Applications) 
	// RegisterVersionPatches and perhaps SerializeDeprecatedPre700 are needed.

	//#define HK_EXCLUDE_FEATURE_SerializeDeprecatedPre700

	// We can also restrict the compatibility to files created with the current version only using HK_SERIALIZE_MIN_COMPATIBLE_VERSION.
	// If we wanted to have compatibility with at most version 650b1 we could have used something like:
	// #define HK_SERIALIZE_MIN_COMPATIBLE_VERSION 650b1.
	#define HK_SERIALIZE_MIN_COMPATIBLE_VERSION HK_HAVOK_VERSION_201010r1

	//#define HK_EXCLUDE_FEATURE_RegisterVersionPatches
	//#define HK_EXCLUDE_FEATURE_RegisterReflectedClasses
	//#define HK_EXCLUDE_FEATURE_MemoryTracker

	// We also need to exclude the other common libraries referenced in Source\Common\Serialize\Classlist\hkCommonClasses.h
	// You may be linking these libraries in your application, in which case you will not need these #defines.
	//#define HK_EXCLUDE_LIBRARY_hkcdCollide
	//#define HK_EXCLUDE_LIBRARY_hkcdInternal
	//#define HK_EXCLUDE_LIBRARY_hkSceneData
	//#define HK_EXCLUDE_LIBRARY_hkGeometryUtilities

	#define HK_EXCLUDE_LIBRARY_hkpVehicle // 400k
	#define HK_EXCLUDE_FEATURE_hkpHeightField // 10k
	#define HK_EXCLUDE_FEATURE_hkpContinuousSimulation // 25k
	#define HK_EXCLUDE_FEATURE_hkpMultiThreadedSimulation // 50k
	#define HK_EXCLUDE_FEATURE_hkpAabbTreeWorldManager // 15k
	#define HK_EXCLUDE_FEATURE_hkpKdTreeWorldManager // 40k
	#define HK_EXCLUDE_FEATURE_hkpAccurateInertiaTensorComputer // 400k

	#include <Common/Base/Config/hkProductFeatures.cxx>

	// Platform specific initialization
	#include <Common/Base/System/Init/PlatformInit.cxx>


// TODO: reference additional headers your program requires here

void HK_CALL errorReport(const char* msg, void* userContext);

hkResult hkSerializeLoad(hkStreamReader *reader
                                , hkVariant &root
                                , hkResource *&resource);

hkResource* hkSerializeUtilLoad( hkStreamReader* stream
                                , hkSerializeUtil::ErrorDetails* detailsOut/*=HK_NULL*/
                                , const hkClassNameRegistry* classReg/*=HK_NULL*/
                                , hkSerializeUtil::LoadOptions options/*=hkSerializeUtil::LOAD_DEFAULT*/ );

// This automatically registers the patches from the HK_PATCHES_FILE
// with the given hkVersionPatchmanager
/*void HK_CALL CustomRegisterPatches(hkVersionPatchManager& man)
{
#	define HK_PATCHES_FILE <Common/Compat/Patches/2010_2/hkbPatches_2010_2.cxx>
#	include <Common/Serialize/Version/hkVersionPatchManager.cxx>
#	undef HK_PATCHES_FILE
}*/

/*void HK_CALL CustomRegisterDefaultClasses()
{
#define HKCLASS_VERIFY(XXX, YYY) REGISTER_CLASS_W_SIG(##XXX, ##YYY)
#include "hkxverify.inl"
#undef HKCLASS_VERIFY
}

void HK_CALL ValidateClassSignatures()
{
#define HKCLASS_VERIFY(XXX, YYY) \
   extern const hkClass XXX##Class; \
   if (XXX##Class.getSignature(0) != YYY) { Log::Warn("Class Signature Mismatch: '" #XXX "' %08lx != %08lx", XXX##Class.getSignature(0), YYY); }\

#include "hkxverify.inl"
#undef HKCLASS_VERIFY
}
*/
typedef struct stringPaths_s {
	std::string original;
	std::string newer;
} stringPaths_t;

void get_all_files_by_extension(std::vector<stringPaths_t> * all_paths, const char *cCurrentPath, const char * find, bool recurse);

