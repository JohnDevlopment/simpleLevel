#ifndef _RESOURCE_MANAGER_HPP
#define _RESOURCE_MANAGER_HPP

// C++ headers
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <new>

// C standard library
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>

enum MemoryMode {
	MEMORYMODE_ALLOC,
	MEMORYMODE_DEALLOC,
	MEMORYMODE_GET,
	MEMORYMODE_SET
};

typedef int (* ResCat_Function)(void**, void*, const MemoryMode);

struct ResourceCategory {
	char*  Name;       // name of the category
	size_t Capacity;   // total number of elements in the array
	size_t NumObjects; // elements in the array that aren't NULL
	void** PObjects;   // array of pointers referencing objects on the heap
	void** NextEntry;  // address of a NULL pointer in the array
	
	const char* Error; // pointer to the error string
	
	ResCat_Function MemoryFunction;
	
	
	ResourceCategory() : Name(nullptr), NumObjects(0),
	 PObjects(nullptr), Capacity(0), MemoryFunction(nullptr),
	 NextEntry(nullptr), Error(nullptr) {}
	
	~ResourceCategory() {clear(); delete[] Name;}
	
	// move constructor
	ResourceCategory(ResourceCategory&& source) {move( std::move(source) );}
	
	// move assignment
	void operator=(ResourceCategory&& rhs) {move( std::move(rhs) );}
	
	// move function
	void move(ResourceCategory&& source);
	
	
	// functions that allocate or free memory for the array //
	
	// increase the size of the array
	void reallocate();
	
	// change the memory capacity of the array
	bool reserve(const size_t bytes);
	
	
	// functions that create, delete, and modify existing entries //
	
	// creates a new object inside a preexisting category
	long int create(void* data);
	
	// clear all objects
	void clear();
	
	// remove specified object
	void remove(const size_t index);
	
	
	// misc. helper functions
	
	// set the NextEntry pointer
	void next_pointer();
	
	// concat an error message
	void concat_error(const char* str);
	
	/* Looks for the specified address in the pointer array. The return value is
	   zero or higher upon success and -1 if the address is not in this array.
	   The return value is which element in the array the address is in.
	   If addr is null the function will return -1.
	*/
	long int find_address(const void* addr);
	
	// get the pointer from the array specified by index
	template<class T>
	T* get(size_t index) const {
		if (index < Capacity)
		  return reinterpret_cast<T*>(PObjects[index]);
	
	return nullptr;
	}
};

class ResourceManager {
	size_t NumCategories;
	size_t Capacity;
	
	ResourceCategory* CategoryNodes;
	
	void reallocate();
	
	void init_ErrorString();
	
	ResourceManager& operator=(const ResourceManager& rhs); // disallow copy
	
	public:
	
	// default constructor and destructor
	ResourceManager() : NumCategories(0), Capacity(0), CategoryNodes(nullptr) {init_ErrorString();}
	~ResourceManager() {clear();}
	
	// functions that allocate and free memory for the array //
	
	// request a change in the size of the array
	bool reserve(const size_t num);
	
	// clear every member of the array and free all memory
	void clear();
	
	
	// functions that add, remove, or query categories
	
	/*
	    Add new category to the array. Returns zero or above on success and -1 on failure. An exception
	    is thrown if 'new' fails.
	*/
	long int create(const char* name, ResCat_Function mem_func = nullptr);
	
	// deletes the n'th category; returns whether it was successful or not
	bool remove(const size_t index);
	
	// deletes the category matching ptr. byte has no purpose other than to distinguish the function
	bool remove(const ResourceCategory* ptr, char byte);
	
	// returns a pointer to the category of the specified name
	ResourceCategory* search(const char* name);
	
	// get the number of categories
	size_t count() const {return NumCategories;}
	
	// get the length of the array in memory
	size_t length() const {return Capacity;}
	
	
	// functions that add, remove, or query entries in a particular category //
	
	/*
	    Adds an entry to the category specified by 'category', along with a pointer to some
	    arbitrary data the allocator function might use. Returns zero or up if successful, else
	    -1 upon failure. The return value is the index to the array in that category which contains
	    the entry.
	*/
	long int new_object(const char* category, void* data = nullptr);
	
	// remove an object from a category
	bool remove_object(const char* categoryName, const size_t index);
	
	// remove an object from a category
	bool remove_objectp(const char* categoryName, const void* addr);
	
	// pass a category's list of defined entries to <func> with the optional pointer to data <data>
	bool forward_list(const char* categoryName, void (* func)(void*, void*), void* data = nullptr);
	
	
	// accessor functions //
	
	// get the n'th element of the array
	ResourceCategory& operator[](size_t index) {return CategoryNodes[index];}
	
	// get a pointer to the array
	ResourceCategory* getp() {return CategoryNodes;}
	
	// get a pointer to the start of the array
	ResourceCategory* beg() const {return CategoryNodes;}
	
	// get a pointer to the end of the array
	ResourceCategory* end() const {return CategoryNodes + Capacity;}
	
	// retrieve the error message
	static const char* get_error();
};

#endif /* _RESOURCE_MANAGER_HPP */
