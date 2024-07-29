#pragma once
#include <hz/spinlock.hpp>
#include <hz/vector.hpp>
#include <hz/manually_init.hpp>
#include <hz/optional.hpp>
#include <hz/result.hpp>
#include <hz/string.hpp>
#include <hz/unordered_map.hpp>
#include "allocator.hpp"
#include "elf.hpp"

struct SharedObject;

struct ObjectSymbol {
	SharedObject* object;
	const Elf_Sym* sym;
};

enum class LoadError {
	Success,
	NotFound,
	InvalidElf,
	InvalidArch,
	NoMemory
};

enum class LookupPolicy {
	None,
	Symbolic,
	IgnoreLocal,
	LocalAndDeps
};

struct ObjectStorage {
	ObjectStorage();

	void add_object(SharedObject* object);

	hz::result<SharedObject*, LoadError> load_object(
		SharedObject* origin,
		hz::string<Allocator> path,
		int fd);

	hz::result<SharedObject*, LoadError> load_object_at_path(
		SharedObject* origin,
		hz::string<Allocator> path
		);

	hz::result<SharedObject*, LoadError> load_object_with_name(
		SharedObject* origin,
		hz::string_view name
		);

	LoadError load_dependencies(SharedObject* object, bool global);

	hz::optional<ObjectSymbol> lookup(SharedObject* local, const char* name, LookupPolicy policy);

	void init_objects();
	void init_tls(void* tcb);
	void destruct_objects();

	hz::vector<SharedObject*, Allocator> objects {Allocator {}};
	hz::vector<SharedObject*, Allocator> global_scope {Allocator {}};
	hz::unordered_map<hz::string<Allocator>, ObjectSymbol, Allocator> unique_map {Allocator {}};
	hz::vector<SharedObject*, Allocator> init_list {Allocator {}};
	hz::vector<SharedObject*, Allocator> destruct_list {Allocator {}};
	uintptr_t initial_tls_size {};
	uintptr_t total_initial_tls_size {};
	hz::spinlock<hz::vector<SharedObject*, Allocator>> runtime_tls_map {
		hz::vector<SharedObject*, Allocator> {Allocator {}}};
};

extern hz::spinlock<hz::manually_init<ObjectStorage>> OBJECT_STORAGE;
