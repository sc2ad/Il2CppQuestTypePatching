#define IL2CPP_FUNC_VISIBILITY public
#include "register.hpp"
#include "logging.hpp"
// Ensure we can see the il2cpp-functions "private" held values
#include "beatsaber-hook/shared/utils/il2cpp-functions.hpp"
#include "beatsaber-hook/shared/utils/hooking.hpp"
#include "beatsaber-hook/shared/utils/capstone-utils.hpp"

#ifdef CT_USE_GCDESCRIPTOR_DEBUG
#include "capstone-helpers.hpp"
#endif

template<class... TArgs>
struct Hook_FromIl2CppTypeMain {
	constexpr static const char* name() { return "FromIl2CppType"; }
	constexpr static void* addr() { return nullptr; }
	using funcType = Il2CppClass * (*)(TArgs...);
	static funcType* trampoline() { return &FromIl2CppType; }
	static inline Il2CppClass* (*FromIl2CppType)(TArgs...) = nullptr;
	static funcType hook() { return hook_FromIl2CppType; }
	static Il2CppClass* hook_FromIl2CppType(TArgs... args) {
		static auto logger = ::custom_types::_logger().WithContext("FromIl2CppType");
		auto typ = std::get<0>(std::make_tuple(args...));
		// _logger().debug("FromIl2CppType: %p", typ);
		if (typ == nullptr) {
			// Extra error checking to avoid unknown null derefs.
			logger.warning("FromIl2CppType was given a null Il2CppType*! Returning a null!");
			return nullptr;
		}
		bool shouldBeOurs = false;
		// klassIndex is only meaningful for these types
		if ((typ->type == IL2CPP_TYPE_CLASS || typ->type == IL2CPP_TYPE_VALUETYPE) && typ->data.klassIndex < 0) {
			shouldBeOurs = true;
			// If the type matches our type
			size_t idx = kTypeDefinitionIndexInvalid - typ->data.klassIndex;
#ifndef NO_VERBOSE_LOGS
			logger.debug("Custom idx: %u for type: %p", idx, typ);
#endif
			if (idx < ::custom_types::Register::classes.size() && idx >= 0) {
#ifndef NO_VERBOSE_LOGS
				logger.debug("Returning custom class with idx %i!", idx);
#endif
				auto* k = ::custom_types::Register::classes[idx];
				if (k == nullptr) {
					logger.warning("Class at idx: %zu is null!", idx);
				}
				return k;
			}
		}
		// Otherwise, return orig
		auto klass = FromIl2CppType(args...);
		if (shouldBeOurs) {
			logger.debug("Called with klassIndex %i which is not our custom type?!", typ->data.klassIndex);
			il2cpp_utils::LogClass(logger, klass, false);
		}
		return klass;
	}
};

MAKE_HOOK(Class_Init, nullptr, bool, Il2CppClass* klass) {
	static auto logger = ::custom_types::_logger().WithContext("Class::Init");
	// If we are attempting to call Class::Init() on an Il2CppClass* that is a custom Il2CppClass*, we need to ignore.
	if (!klass) {
		// We will provide some useful debug info here
		logger.warning("Called with a null Il2CppClass*! (Specifically: %p)", klass);
		SAFE_ABORT();
	}
	auto typ = klass->this_arg;
	if ((typ.type == IL2CPP_TYPE_CLASS || typ.type == IL2CPP_TYPE_VALUETYPE) && typ.data.klassIndex < 0) {
		// This is a custom class. Skip it.
		auto idx = kTypeDefinitionIndexInvalid - typ.data.klassIndex;
#ifndef NO_VERBOSE_LOGS
		logger.debug("custom idx: %u", idx);
#endif
		return true;
	}
	else {
		return Class_Init(klass);
	}
}

MAKE_HOOK(MetadataCache_GetTypeInfoFromTypeDefinitionIndex, nullptr, Il2CppClass*, TypeDefinitionIndex index) {
	if (index < 0) {
		static auto logger = ::custom_types::_logger().WithContext("MetadataCache::GetTypeInfoFromTypeDefinitionIndex");
		// index is either invalid or one of ours
		size_t idx = kTypeDefinitionIndexInvalid - index;
		logger.debug("custom idx: %zu", idx);
		if (idx < ::custom_types::Register::classes.size() && idx >= 0) {
			logger.debug("Returning custom class with idx %zu!", idx);
			auto* k = ::custom_types::Register::classes[idx];
			return k;
		}
	}
	// Otherwise, return orig
	return MetadataCache_GetTypeInfoFromTypeDefinitionIndex(index);
}

MAKE_HOOK(GetScriptingClass, nullptr, Il2CppClass*, void* thisptr, char* assembly, char* namespaze, char* name) {
    static auto logger = ::custom_types::_logger().WithContext("GetScriptingClass");
    auto ret = GetScriptingClass(thisptr, assembly, namespaze, name);
    if(!ret) {
        for(auto clazz : ::custom_types::Register::classes) {
            if(strcmp(clazz->namespaze, namespaze) == 0 &&  strcmp(clazz->name, name) == 0) {
                logger.debug("Found class: %s, %s", namespaze, name);
                return clazz;
            }
        }
    }
    return ret;
}

#ifdef CT_USE_GCDESCRIPTOR_DEBUG
#ifndef UNITY_2019
#warning "Hey, this probably only works for 2019 unity, so be careful."
#endif

#ifdef LOCAL_TEST
#warning "Hey, you shouldn't use LOCAL_TEST while building with CT_USE_GCDESCRIPTOR_DEBUG!"
#endif

#include "libil2cpp/il2cpp/libil2cpp/utils/dynamic_array.h"

const char* namespaze(Il2CppClass* const klass) {
	return klass->namespaze ? klass->namespaze : "<NULL_NAMESPAZE>";
}
const char* namek(Il2CppClass* const klass) {
	return klass->name ? klass->name : "<NULL_NAME>";
}

#define WORDSIZE ((int)sizeof(size_t)*8)
#define GET_CLASS(obj) \
	((Il2CppClass*)(((size_t)(obj)->klass) & ~(size_t)1))
#define IS_MARKED(obj) \
	(((size_t)(obj)->klass) & (size_t)1)

std::size_t generic_obj_traverse_count = 0;
std::size_t obj_traverse_count = 0;

MAKE_HOOK(LivenessState_TraverseGenericObject, nullptr, void, Il2CppObject* obj, void* state) {
	// We are calling this with an object and a state.
	// The state is the LivenessState instance
	// There is a process_array that we want to look at
	// Likewise we also want to look at our traverse_depth
	// Not to mention the actual object and state ptrs

	// auto klass = GET_CLASS(obj);
	// custom_types::_logger().debug("%zu: LivenessState::TraverseGenericObject(%p, %p), with klass: %p (%s::%s)", generic_obj_traverse_count++, obj, state, obj->klass, namespaze(klass), namek(klass));
	// process_array is at 0x18 (custom_growable_array*)
	// traverse_depth is at 0x48 (int)
	// auto arrPtr = *reinterpret_cast<il2cpp::utils::dynamic_array<Il2CppObject*>**>(reinterpret_cast<uint8_t*>(state) + 0x18);
	// custom_types::_logger().debug("traverse_depth: %d", *reinterpret_cast<int*>(reinterpret_cast<uint8_t*>(state) + 0x48));
	// custom_types::_logger().debug("arrPtr: %p, inner: %p, size: %zu", arrPtr, arrPtr->data(), arrPtr->size());
	// custom_types::_logger().flush();
	// for (size_t i = 0; i < arrPtr->size(); i++) {
	// 	auto inst = arrPtr->data()[i];
	// 	custom_types::_logger().debug("arr val: %zu ptr: %p, class: %p (%s::%s)", i, inst, inst->klass, namespaze(GET_CLASS(inst)), namek(GET_CLASS(inst)));
	// }
	// if (arrPtr->size() > 0) {
	// 	auto inst = arrPtr->data()[arrPtr->size() - 1];
	// 	custom_types::_logger().debug("arr val: %zu, ptr: %p, class: %p (%s::%s)", arrPtr->size() - 1, inst, inst->klass, namespaze(GET_CLASS(inst)), namek(GET_CLASS(inst)));
	// 	custom_types::_logger().flush();
	// }
	generic_obj_traverse_count++;
	LivenessState_TraverseGenericObject(obj, state);
	// custom_types::_logger().debug("Complete LivenessState::TraverseGenericObject");
}

MAKE_HOOK(LivenessState_TraverseObjectInternal, nullptr, bool, Il2CppObject* obj, bool isStruct, Il2CppClass* klass, void* state) {
	// Here we are going to log... AGAIN
	// but this time only a few things
	// custom_types::_logger().debug("LivenessState::TraverseObjectInternal(%p, %s, %p, %p)", obj, isStruct ? "true" : "false", klass, state);
	// custom_types::_logger().flush();
	// custom_types::_logger().debug("class: (%s::%s)", namespaze(klass), namek(klass));
	// custom_types::_logger().flush();
	obj_traverse_count++;
	auto ret = LivenessState_TraverseObjectInternal(obj, isStruct, klass, state);
	// custom_types::_logger().debug("Complete LivenessState::TraverseObjectInternal");
	return ret;
}

// MAKE_HOOK(Liveness_FromStatics, nullptr, void, void* state) {
// 	// filter class is 0x10
// 	auto filter = *reinterpret_cast<Il2CppClass**>(reinterpret_cast<uint8_t*>(state) + 0x10);
// 	custom_types::_logger().debug("Liveness::FromStatics(%p)", state);
// 	custom_types::_logger().debug("filter class: %p", filter);
// 	custom_types::_logger().flush();
// 	custom_types::_logger().debug("filter class: %s::%s", namespaze(filter), namek(filter));
// 	custom_types::_logger().flush();
// 	// TODO: Log class statics info
// 	Liveness_FromStatics(state);
// 	custom_types::_logger().debug("Complete Liveness::FromStatics");
// }

static inline bool HasParentUnsafe(const Il2CppClass* klass, const Il2CppClass* parent) { return klass->typeHierarchyDepth >= parent->typeHierarchyDepth && klass->typeHierarchy[parent->typeHierarchyDepth - 1] == parent; }

MAKE_HOOK(LivenessState_TraverseGCDescriptor, nullptr, void, Il2CppObject* obj, void* state) {
	// Note: gc_desc is a bitfield that holds which fields of the type has references, or, if it is too large, a pointer
	// to a table which holds a larger bitfield.
	int i = 0;
	size_t mask = (size_t)(GET_CLASS(obj)->gc_desc);

	IL2CPP_ASSERT(mask & (size_t)1);

	for (i = 0; i < WORDSIZE - 2; i++)
	{
		size_t offset = ((size_t)1 << (WORDSIZE - 1 - i));
		if (mask & offset)
		{
			Il2CppObject** valptr = (Il2CppObject**)(((char*)obj) + i * sizeof(void*));
			Il2CppObject* val = *valptr;
			if (!val || IS_MARKED(val)) {
				// Null instances are permitted
				continue;
			}
			// We aren't marked at this point, val is a new pointer in our instance
			// PreviewDifficultyBeatmapSet
			// mask: 0b0011000...1
			// f1: BeatmapCharacteristicSO (skipped explicitly in if above)
			// f2: BeatmapDifficulty[] (crashes below)
			// val is: pointer to BeatmapDifficulty[] (at least, it SHOULD be)
			// specifically, val is a pointer to: 39D0 before obj
			// Offset of filter class
			auto filterClass = *reinterpret_cast<Il2CppClass**>(reinterpret_cast<uintptr_t>(state) + 0x10);
			if (!val->klass ||
					(GET_CLASS(val)->has_references == 0 && GET_CLASS(val)->klass != GET_CLASS(val) && GET_CLASS(val)->name == nullptr) ||
					// If our filter class is not null, and
					// our filter class' type hierarchy depth is <= ours and
					// our type hierarchy pointer is garbage
					(filterClass &&
					filterClass->typeHierarchyDepth <= GET_CLASS(val)->typeHierarchyDepth &&
					(reinterpret_cast<uintptr_t>(GET_CLASS(val)->typeHierarchy) <= 0x1000 || reinterpret_cast<uintptr_t>(GET_CLASS(val)->typeHierarchy) > 0xe000000000))
					) {
				// We have a VERY BIG PROBLEM!
				// This will cause a (hard to diagnose) crash!
				// So, we will dump as much info as we can.
				custom_types::_logger().critical("WARNING! THIS WILL CRASH, DUMPING SEMANTIC INFORMATION...");
				custom_types::_logger().critical("LivenessState::TraverseGCDescriptor(%p, %p) class: %p, gc_desc: %p, %s::%s", obj, state, GET_CLASS(obj), GET_CLASS(obj)->gc_desc, namespaze(GET_CLASS(obj)), namek(GET_CLASS(obj)));
				// malloc_info()
				// TODO: Yeah
				auto path = string_format(LOG_PATH, "com.beatgames.beatsaber") + "CustomTypesMallocInfoOnExit.xml";
				auto f = fopen(path.c_str(), "w");
				if (!malloc_info(0, f)) {
					custom_types::_logger().critical("Failed to write to: %s!", path.c_str());
				} else {
					custom_types::_logger().debug("Wrote malloc info to: %s", path.c_str());
				}
				fclose(f);
				custom_types::_logger().critical("LivenessState::TraverseGCDescriptor(%p, %p), with val: %p (klass: %p), idx: %u", obj, state, val, val->klass, i);
				if (GET_CLASS(val)) {
					custom_types::_logger().critical("has_references: %u", GET_CLASS(val)->has_references);
				}
				custom_types::_logger().critical("Logging filterClass: %p", filterClass);
				custom_types::logAll(filterClass);
				custom_types::_logger().critical("Logging all registered custom types...");
				for (auto k : custom_types::Register::classes) {
					custom_types::_logger().critical("KLASS PTR: %p", k);
					custom_types::logAll(k);
				}

				custom_types::_logger().debug("gc descriptor test field: obj: %p, field: %p, value: %p, class: %p", obj, valptr, val, val ? val->klass : nullptr);
				custom_types::_logger().critical("obj_traverse_count: %zu", obj_traverse_count);
				custom_types::_logger().critical("generic_obj_traverse_count: %zu", generic_obj_traverse_count);

				custom_types::_logger().critical("Talk to Sc2ad to try and understand what the hell is going on here and why.");
				custom_types::_logger().critical("Also, please be very kind and send him this whole log file! It would be much appreciated.");
				custom_types::_logger().critical("With that said, the log in this file may have been truncated, so consider grabbing the file log for custom types instead.");
				custom_types::_logger().critical("custom types will now try to log as much information it can about the offending instance's class before crashing...");
				custom_types::_logger().critical("KLASS PTR: %p", obj->klass);
				custom_types::_logger().flush();
				if (GET_CLASS(obj)) {
					custom_types::logAll(GET_CLASS(obj));
				}
				custom_types::_logger().critical("KLASS PTR: %p", val->klass);
				custom_types::_logger().critical("Attempting HasParentUnsafe(%p, %p)...", GET_CLASS(val), filterClass);
				custom_types::_logger().flush();
				auto ret = HasParentUnsafe(GET_CLASS(val), filterClass);
				custom_types::_logger().critical("HasParentUnsafe return: %s", ret ? "true" : "false");
				custom_types::_logger().flush();
				if (GET_CLASS(val)) {
					custom_types::logAll(GET_CLASS(val));
				}
				custom_types::_logger().flush();
				// Things I have learned, just dumping here:
				// static fields and classes that have a nonzero quantity of static fields
				// need to be added to:
				// Class::GetStaticFieldData()
				// as for Liveness::FromRoot, it MIGHT NOT be the base,
				// because the recursion layer is not bt-able due to b's instead of bl's
				// So, what COULD happen is that the root liveness calc DOES NOT have a gc descriptor, and it is only a type
				// later on that does.
				// Perhaps we should hook the TraverseGenericObject function instead and see what we can learn as we walk the root?
			}
		}
	}
	// Call orig
	LivenessState_TraverseGCDescriptor(obj, state);
	#undef WORDSIZE
	#undef GET_CLASS
	#undef IS_MARKED
}

std::optional<uint32_t*> readsafeb(uint32_t const* const addr) {
	cs_insn* insns;
    // Read from addr, 1 instruction, with pc at addr, into insns.
    // TODO: consider using cs_disasm_iter
    auto count = cs_disasm(cs::getHandle(), reinterpret_cast<const uint8_t*>(addr), sizeof(uint32_t), reinterpret_cast<uint64_t>(addr), 1, &insns);
    RET_DEFAULT_UNLESS(custom_types::_logger(), count == 1);
    auto inst = insns[0];
    // Thunks have a single b
    RET_DEFAULT_UNLESS(custom_types::_logger(), inst.id == ARM64_INS_B);
    auto platinsn = inst.detail->arm64;
    RET_DEFAULT_UNLESS(custom_types::_logger(), platinsn.op_count == 1);
    auto op = platinsn.operands[0];
    RET_DEFAULT_UNLESS(custom_types::_logger(), op.type == ARM64_OP_IMM);
    // Our b dest is addr + (imm << 2), except capstone does this for us.
    auto dst = reinterpret_cast<uint32_t*>(op.imm);
    cs_free(insns, 1);
    return dst;
}
#endif

// NOTE THAT THIS HOOK DOES NOT PERMIT TYPES OF IDENTICAL NAMESPACE AND NAME BUT IN DIFFERENT IMAGES!
// This could be worked around if we also have image be a part of the key, but as it stands, that is not necessary.
// MAKE_HOOK(Class_FromName, nullptr, Il2CppClass*, Il2CppImage* image, const char* namespaze, const char* name) {
//     #ifndef NO_VERBOSE_LOGS
//     static auto logger = ::custom_types::_logger().WithContext("Class::FromName");
//     #endif
//     auto pair = std::make_pair(std::string(namespaze), std::string(name));
//     auto itr = custom_types::Register::classMapping.find(pair);
//     if (itr != custom_types::Register::classMapping.end()) {
//         #ifndef NO_VERBOSE_LOGS
//         logger.debug("Returning custom class from: %s::%s lookup: %p", namespaze, name, itr->second);
//         #endif
//         return itr->second;
//     }
//     return Class_FromName(image, namespaze, name);
// }

namespace custom_types {
	std::unordered_map<std::string, Il2CppAssembly*> Register::assembs;
	std::unordered_map<std::string, Il2CppImage*> Register::images;
	std::unordered_map<std::pair<std::string, std::string>, Il2CppClass*> Register::classMapping;
	std::shared_mutex Register::assemblyMtx;
	std::shared_mutex Register::imageMtx;
	std::mutex Register::classMappingMtx;
	std::mutex Register::registrationMtx;
	std::mutex installationMtx;
	bool Register::installed = false;
	std::vector<Il2CppClass*> Register::classes;
	std::vector<TypeRegistration*> Register::toRegister;
	std::vector<TypeRegistration*> Register::registeredTypes;

	Il2CppAssembly* Register::createAssembly(std::string_view name, Il2CppImage* img) {
		// Name is NOT copied, so should be a constant string
		// Check to see if an assembly with the given name already exists.
		// If it does, use that instead.
		std::string strName(name);
		{
			std::shared_lock lock(assemblyMtx);
			auto itr = assembs.find(strName);
			if (itr != assembs.end()) {
				return itr->second;
			}
		}
		auto assemb = new Il2CppAssembly();
		assemb->image = img;
		img->assembly = assemb;
		assemb->aname.name = name.data();
		{
			std::unique_lock lock(assemblyMtx);
			// Add our new assembly to the collection of all known assemblies
			il2cpp_functions::Assembly_GetAllAssemblies()->push_back(assemb);
			_logger().debug("Added new assembly image: %p", assemb->image);
			assembs.insert({ strName, assemb });
		}
		_logger().debug("Created new assembly: %s, %p", name.data(), assemb);
		return assemb;
	}

	const Il2CppImage* Register::createImage(std::string_view name) {
		// Name is NOT copied, so should be a constant string
		// Check to see if an image with the given name already exists.
		// If it does, use that instead.
		std::string strName(name);
		{
			std::shared_lock lock(imageMtx);
			auto itr = images.find(strName);
			if (itr != images.end()) {
				return itr->second;
			}
		}
		auto img = new Il2CppImage();
		std::unique_lock lock(imageMtx);
		auto res = images.insert({ strName, img });
		lock.unlock();
		img->name = res.first->first.c_str();
		auto strToCopy = strName.substr(0, strName.find_last_of('.'));
		auto* allocNameNoExt = new char[strToCopy.size() + 1];
		memcpy(allocNameNoExt, strToCopy.c_str(), strToCopy.size());
		allocNameNoExt[strToCopy.size()] = '\0';
		img->nameNoExt = allocNameNoExt;
		img->dynamic = true;
		img->assembly = createAssembly(allocNameNoExt, img);
		img->nameToClassHashTable = new Il2CppNameToTypeDefinitionIndexHashTable();
		// Types are pushed here on class creation
		// TODO: Avoid copying eventually
		img->exportedTypeStart = 0;
		img->exportedTypeCount = 0;
		// Custom attribute start and count is used somewhere within unity
		// (which makes a call to: il2cpp_custom_attrs_from_class/il2cpp_custom_attrs_from_method)
		// These are required to not be undefined (though perhaps a -1 and a 0 would work just as well here?)
		// RGCTXes are also from codeGenModule, so that must also be defined.
		img->customAttributeStart = 0;
		img->customAttributeCount = 0;
		img->entryPointIndex = 0;
		// TODO: Populate this in a more reasonable way
		// auto* codegen = new Il2CppCodeGenModule{Il2CppCodeGenModule{
		//     .moduleName = name.data(),
		//     .methodPointerCount = 0,
		//     .reversePInvokeWrapperCount = 0,
		//     .rgctxRangesCount = 0,
		//     .rgctxsCount = 0
		// }};
		// img->codeGenModule = codegen;
		// TOOD: We shall leave the others undefined for now.
		_logger().debug("Created new image: %s, %p", name.data(), img);
		return img;
	}

	void Register::EnsureHooks() {
		std::lock_guard lock(installationMtx);
		if (!installed) {
			il2cpp_functions::Init();
			static auto logger = _logger().WithContext("EnsureHooks");
			logger.debug("Installing FromIl2CppType hook...");
			if constexpr (sizeof(Il2CppCodeGenModule) < 104) {
				Hooking::InstallHookDirect<Hook_FromIl2CppTypeMain<Il2CppType*>>(logger, (void*)il2cpp_functions::il2cpp_Class_FromIl2CppType);
			}
			else {
				Hooking::InstallHookDirect<Hook_FromIl2CppTypeMain<Il2CppType*, bool>>(logger, (void*)il2cpp_functions::il2cpp_Class_FromIl2CppType);
			}
			INSTALL_HOOK_DIRECT(logger, MetadataCache_GetTypeInfoFromTypeDefinitionIndex, (void*)il2cpp_functions::il2cpp_MetadataCache_GetTypeInfoFromTypeDefinitionIndex);
			INSTALL_HOOK_DIRECT(logger, Class_Init, (void*)il2cpp_functions::il2cpp_Class_Init);
			uintptr_t GetScriptingClassAddr = findPattern(baseAddr("libunity.so"), "ff c3 01 d1 f9 63 03 a9 f7 5b 04 a9 f5 53 05 a9 f3 7b 06 a9 57 d0 3b d5 e8 16 40 f9 f6 03 01 aa");
			INSTALL_HOOK_DIRECT(logger, GetScriptingClass, reinterpret_cast<void*>(GetScriptingClassAddr));
			// {
			//     // We need to do a tiny bit of xref tracing to find the bottom level Class::FromName call
			//     // Trace is: il2cpp_class_from_name --> b --> b --> result
			//     INSTALL_HOOK_DIRECT(logger, Class_FromName, (void*)cs::findNthB<1>(reinterpret_cast<const uint32_t*>(il2cpp_functions::il2cpp_class_from_name)));
			// }
			#ifdef CT_USE_GCDESCRIPTOR_DEBUG
			#define BREAK(var, ...) do {if (!var) {logger.warning(__VA_ARGS__); goto exit;}} while (0)
			{
				// We need to xref trace to get to LivenessState stuff
				// il2cpp_unity_liveness_calculation_from_root
				// only a b --> Liveness::FromRoot
				// 2nd bl --> LivenessState::TraverseObjects
				// 1st bl --> LivenessState::TraverseGenericObject
				// 2nd b --> LivenessState::TraverseGCDescriptor

				// If we fail anywhere in this chain, simply log it and move on.
				// We shouldn't care, this is just a debug hook after all.
				auto opt = readsafeb((uint32_t*)il2cpp_functions::il2cpp_unity_liveness_calculation_from_root);
				BREAK(opt, "Failed to find b in il2cpp_unity_liveness_calculation_from_root!");
				opt = cs::findNthBlSafe<2>(*opt);
				BREAK(opt, "Failed to find 2nd bl in Liveness::FromRoot!");
				opt = cs::findNthBlSafe<1>(*opt);
				BREAK(opt, "Failed to find 1st bl in Liveness::TraverseObject!");
				auto traverseGeneric = *opt;
				auto traverseInternal = cs::findNthBSafe<3>(*opt);
				opt = cs::findNthBSafe<2>(*opt);
				BREAK(opt, "Failed to find 2nd b in LivenessState::TraverseGenericObject!");
				BREAK(traverseInternal, "Failed to find 3rd b in LivenessState::TraverseGenericObject!");
				// We found all of the chain, lets install our debug hook!
				INSTALL_HOOK_DIRECT(logger, LivenessState_TraverseGenericObject, traverseGeneric);
				INSTALL_HOOK_DIRECT(logger, LivenessState_TraverseGCDescriptor, *opt);
				INSTALL_HOOK_DIRECT(logger, LivenessState_TraverseObjectInternal, *traverseInternal);
				// opt = readsafeb((uint32_t*)il2cpp_functions::il2cpp_unity_liveness_calculation_from_statics);
				// BREAK(opt, "Failed to find b in il2cpp_unity_liveness_calculation_from_statics!");
				// INSTALL_HOOK_DIRECT(logger, Liveness_FromStatics, *opt);
			}
			#undef BREAK
			exit:
			#endif
			installed = true;
		}
	}
}