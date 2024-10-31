


/*
	MIT License

	Copyright (c) 2024 xLeaves [xywhsoft] <xywhsoft@qq.com>

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/



#ifndef XXRTL_MemoryManagementUnit
	#define XXRTL_MemoryManagementUnit
	
	
	
	#include <stddef.h>
	#include <stdlib.h>
	#include <stdint.h>
	#include <string.h>
	#include <stdio.h>
	#include <errno.h>
	#include <stdatomic.h>
	
	
	
	#ifdef BUILD_DLL
		#define XXAPI	__declspec(dllexport)
	#else
		#define XXAPI
	#endif
	
	
	
	// 模块依赖处理
	#ifdef MMU_USE_SDSTK
		#define MMU_USE_PMMU
	#endif
	#ifdef MMU_USE_PDSTK
		#define MMU_USE_PMMU
	#endif
	#ifdef MMU_USE_LLIST
		#define MMU_USE_MM256
	#endif
	#ifdef MMU_USE_AVLTREE
		#define MMU_USE_MM256
	#endif
	#ifdef MMU_USE_RBTREE
		#define MMU_USE_MM256
	#endif
	#ifdef MMU_USE_AVLHT32
		#define MMU_USE_AVLTREE
		#define MMU_USE_HASH32
	#endif
	#ifdef MMU_USE_AVLHT64
		#define MMU_USE_AVLTREE
		#define MMU_USE_HASH64
	#endif
	#ifdef MMU_USE_RBHT32
		#define MMU_USE_RBTREE
		#define MMU_USE_HASH32
	#endif
	#ifdef MMU_USE_RBHT64
		#define MMU_USE_RBTREE
		#define MMU_USE_HASH64
	#endif
	#ifdef MMU_USE_MM256
		#define MMU_USE_MMU256
		#define MMU_USE_PMMU
	#endif
	#ifdef MMU_USE_MM64K
		#define MMU_USE_MMU64K
		#define MMU_USE_PMMU
	#endif
	
	// TCC 暂不支持编译 rpmalloc
	#ifdef __TINYC__
		#undef MMU_USE_RPMALLOC
	#endif
	
	
	
	/*
		MMU : Memory Management Unit (内存管理单元)
			用于管理各种结构化与非结构化的内存，降低内存申请与释放次数，提升运行效率
	*/
	
	// MMU256 和 MMU64K 申请内存固定的前置数据（用于识别内存是哪个管理器分配的）
	typedef struct {
		unsigned int ItemFlag;
	} MMU_Value, *MMU_ValuePtr;
	
	// 报错信息
	#define MMU_ERROR_ALLOC				1
	#define MMU_ERROR_CREATEMMU			2
	#define MMU_ERROR_ADDMMU			3
	#define MMU_ERROR_GETMMU			4
	
	// 报错回调函数定义
	typedef void (*MMU_OnErrorProc)(void* objMM, int ErrorID);
	
	// MMU有效ID区间掩码
	#define MMU_FLAG_MASK				0x3FFFFFFF
	
	// 结构体使用状态标记
	#define MMU_FLAG_USE				0x80000000
	
	// GC回收标记位
	#define MMU_FLAG_GC					0x40000000
	
	// MM256 or MM64K GC标记
	static inline void MM_GC_Mark_Inline(void* ptr)
	{
		MMU_ValuePtr v = ptr - sizeof(MMU_Value);
		v->ItemFlag |= MMU_FLAG_GC;
	}
	
	// 内存分配器定义
	#ifdef MMU_USE_RPMALLOC
		#define mmu_realloc	rprealloc
		#define mmu_malloc	rpmalloc
		#define mmu_calloc	rpcalloc
		#define mmu_free	rpfree
	#else
		#define mmu_realloc	realloc
		#define mmu_malloc	malloc
		#define mmu_calloc	calloc
		#define mmu_free	free
	#endif
		
	// 32位哈希表节点基础定义
	typedef struct {
		void* Key;
		unsigned int Hash;
		unsigned int KeyLen;
	} HT32_NodeBase;
		
	// 64位哈希表节点基础定义
	typedef struct {
		void* Key;
		unsigned long long Hash;
		unsigned int KeyLen;
	} HT64_NodeBase;
	
	// 哈希表遍历回调函数
	typedef int (*HT32_EachProc)(HT32_NodeBase* pKey, void* pVal, void* pArg);
	typedef int (*HT64_EachProc)(HT64_NodeBase* pKey, void* pVal, void* pArg);
	
	// 初始化 MMU 库
	XXAPI int MMU_Init();
	
	// 卸载 MMU 库
	XXAPI void MMU_Unit();
	
	// 线程初始化 MMU 库
	XXAPI int MMU_Thread_Init();
	
	// 线程卸载 MMU 库
	XXAPI void MMU_Thread_Unit();
	
	
	
	
	
	/*
		rpmalloc [Ver1.4.5, Update : 2024/10/30 from https://github.com/mjansson/rpmalloc]
			内存分配器
	*/
	
	#ifdef MMU_USE_RPMALLOC
		
		#define RPMALLOC_CACHE_LINE_SIZE 64
		#if defined(__clang__) || defined(__GNUC__)
		#define RPMALLOC_EXPORT __attribute__((visibility("default")))
		#define RPMALLOC_RESTRICT __restrict
		#define RPMALLOC_ALLOCATOR
		#define RPMALLOC_CACHE_ALIGNED __attribute__((aligned(RPMALLOC_CACHE_LINE_SIZE)))
		#if (defined(__clang_major__) && (__clang_major__ < 4)) || (!defined(__clang_major__) && defined(__GNUC__))
		#define RPMALLOC_ATTRIB_MALLOC
		#define RPMALLOC_ATTRIB_ALLOC_SIZE(size)
		#define RPMALLOC_ATTRIB_ALLOC_SIZE2(count, size)
		#else
		#define RPMALLOC_ATTRIB_MALLOC __attribute__((__malloc__))
		#define RPMALLOC_ATTRIB_ALLOC_SIZE(size) __attribute__((alloc_size(size)))
		#define RPMALLOC_ATTRIB_ALLOC_SIZE2(count, size) __attribute__((alloc_size(count, size)))
		#endif
		#define RPMALLOC_CDECL
		#elif defined(_MSC_VER)
		#define RPMALLOC_EXPORT
		#define RPMALLOC_RESTRICT __declspec(restrict)
		#define RPMALLOC_ALLOCATOR __declspec(allocator) __declspec(restrict)
		#define RPMALLOC_CACHE_ALIGNED __declspec(align(RPMALLOC_CACHE_LINE_SIZE))
		#define RPMALLOC_ATTRIB_MALLOC
		#define RPMALLOC_ATTRIB_ALLOC_SIZE(size)
		#define RPMALLOC_ATTRIB_ALLOC_SIZE2(count, size)
		#define RPMALLOC_CDECL __cdecl
		#else
		#define RPMALLOC_EXPORT
		#define RPMALLOC_ALLOCATOR
		#define RPMALLOC_ATTRIB_MALLOC
		#define RPMALLOC_ATTRIB_ALLOC_SIZE(size)
		#define RPMALLOC_ATTRIB_ALLOC_SIZE2(count, size)
		#define RPMALLOC_CDECL
		#endif

		#define RPMALLOC_MAX_ALIGNMENT (256 * 1024)

		//! Define RPMALLOC_FIRST_CLASS_HEAPS to enable heap based API (rpmalloc_heap_* functions).
		#ifndef RPMALLOC_FIRST_CLASS_HEAPS
		#define RPMALLOC_FIRST_CLASS_HEAPS 0
		#endif

		//! Flag to rpaligned_realloc to not preserve content in reallocation
		#define RPMALLOC_NO_PRESERVE 1
		//! Flag to rpaligned_realloc to fail and return null pointer if grow cannot be done in-place,
		//  in which case the original pointer is still valid (just like a call to realloc which failes to allocate
		//  a new block).
		#define RPMALLOC_GROW_OR_FAIL 2

		typedef struct rpmalloc_global_statistics_t {
			//! Current amount of virtual memory mapped, all of which might not have been committed (only if
			//! ENABLE_STATISTICS=1)
			size_t mapped;
			//! Peak amount of virtual memory mapped, all of which might not have been committed (only if ENABLE_STATISTICS=1)
			size_t mapped_peak;
			//! Current amount of memory in global caches for small and medium sizes (<32KiB)
			size_t cached;
			//! Current amount of memory allocated in huge allocations, i.e larger than LARGE_SIZE_LIMIT which is 2MiB by
			//! default (only if ENABLE_STATISTICS=1)
			size_t huge_alloc;
			//! Peak amount of memory allocated in huge allocations, i.e larger than LARGE_SIZE_LIMIT which is 2MiB by default
			//! (only if ENABLE_STATISTICS=1)
			size_t huge_alloc_peak;
			//! Total amount of memory mapped since initialization (only if ENABLE_STATISTICS=1)
			size_t mapped_total;
			//! Total amount of memory unmapped since initialization  (only if ENABLE_STATISTICS=1)
			size_t unmapped_total;
		} rpmalloc_global_statistics_t;

		typedef struct rpmalloc_thread_statistics_t {
			//! Current number of bytes available in thread size class caches for small and medium sizes (<32KiB)
			size_t sizecache;
			//! Current number of bytes available in thread span caches for small and medium sizes (<32KiB)
			size_t spancache;
			//! Total number of bytes transitioned from thread cache to global cache (only if ENABLE_STATISTICS=1)
			size_t thread_to_global;
			//! Total number of bytes transitioned from global cache to thread cache (only if ENABLE_STATISTICS=1)
			size_t global_to_thread;
			//! Per span count statistics (only if ENABLE_STATISTICS=1)
			struct {
				//! Currently used number of spans
				size_t current;
				//! High water mark of spans used
				size_t peak;
				//! Number of spans transitioned to global cache
				size_t to_global;
				//! Number of spans transitioned from global cache
				size_t from_global;
				//! Number of spans transitioned to thread cache
				size_t to_cache;
				//! Number of spans transitioned from thread cache
				size_t from_cache;
				//! Number of spans transitioned to reserved state
				size_t to_reserved;
				//! Number of spans transitioned from reserved state
				size_t from_reserved;
				//! Number of raw memory map calls (not hitting the reserve spans but resulting in actual OS mmap calls)
				size_t map_calls;
			} span_use[64];
			//! Per size class statistics (only if ENABLE_STATISTICS=1)
			struct {
				//! Current number of allocations
				size_t alloc_current;
				//! Peak number of allocations
				size_t alloc_peak;
				//! Total number of allocations
				size_t alloc_total;
				//! Total number of frees
				size_t free_total;
				//! Number of spans transitioned to cache
				size_t spans_to_cache;
				//! Number of spans transitioned from cache
				size_t spans_from_cache;
				//! Number of spans transitioned from reserved state
				size_t spans_from_reserved;
				//! Number of raw memory map calls (not hitting the reserve spans but resulting in actual OS mmap calls)
				size_t map_calls;
			} size_use[128];
		} rpmalloc_thread_statistics_t;

		typedef struct rpmalloc_interface_t {
			//! Map memory pages for the given number of bytes. The returned address MUST be aligned to the given alignment,
			//! which will always be either 0 or the span size. The function can store an alignment offset in the offset
			//! variable in case it performs alignment and the returned pointer is offset from the actual start of the memory
			//! region due to this alignment. This alignment offset will be passed to the memory unmap function. The mapped size
			//! can be stored in the mapped_size variable, which will also be passed to the memory unmap function as the release
			//! parameter once the entire mapped region is ready to be released. If you set a memory_map function, you must also
			//! set a memory_unmap function or else the default implementation will be used for both. This function must be
			//! thread safe, it can be called by multiple threads simultaneously.
			void* (*memory_map)(size_t size, size_t alignment, size_t* offset, size_t* mapped_size);
			//! Commit a range of memory pages
			void (*memory_commit)(void* address, size_t size);
			//! Decommit a range of memory pages
			void (*memory_decommit)(void* address, size_t size);
			//! Unmap the memory pages starting at address and spanning the given number of bytes. If you set a memory_unmap
			//! function, you must also set a memory_map function or else the default implementation will be used for both. This
			//! function must be thread safe, it can be called by multiple threads simultaneously.
			void (*memory_unmap)(void* address, size_t offset, size_t mapped_size);
			//! Called when a call to map memory pages fails (out of memory). If this callback is not set or returns zero the
			//! library will return a null pointer in the allocation call. If this callback returns non-zero the map call will
			//! be retried. The argument passed is the number of bytes that was requested in the map call. Only used if the
			//! default system memory map function is used (memory_map callback is not set).
			int (*map_fail_callback)(size_t size);
			//! Called when an assert fails, if asserts are enabled. Will use the standard assert() if this is not set.
			void (*error_callback)(const char* message);
		} rpmalloc_interface_t;

		typedef struct rpmalloc_config_t {
			//! Size of memory pages. The page size MUST be a power of two. All memory mapping
			//  requests to memory_map will be made with size set to a multiple of the page size.
			//  Set to 0 to use the OS default page size.
			size_t page_size;
			//! Enable use of large/huge pages. If this flag is set to non-zero and page size is
			//  zero, the allocator will try to enable huge pages and auto detect the configuration.
			//  If this is set to non-zero and page_size is also non-zero, the allocator will
			//  assume huge pages have been configured and enabled prior to initializing the
			//  allocator.
			//  For Windows, see https://docs.microsoft.com/en-us/windows/desktop/memory/large-page-support
			//  For Linux, see https://www.kernel.org/doc/Documentation/vm/hugetlbpage.txt
			int enable_huge_pages;
			//! Disable decommitting unused pages when allocator determines the memory pressure
			//  is low and there is enough active pages cached. If set to 1, keep all pages committed.
			int disable_decommit;
			//! Allocated pages names for systems supporting it to be able to distinguish among anonymous regions.
			const char* page_name;
			//! Allocated huge pages names for systems supporting it to be able to distinguish among anonymous regions.
			const char* huge_page_name;
			//! Unmap all memory on finalize if set to 1. Normally you can let the OS unmap all pages
			//  when process exits, but if using rpmalloc in a dynamic library you might want to unmap
			//  all pages when the dynamic library unloads to avoid process memory leaks and bloat.
			int unmap_on_finalize;
		#if defined(__linux__) || defined(__ANDROID__)
			///! Allows to disable the Transparent Huge Page feature on Linux on a process basis,
			///  rather than enabling/disabling system-wise (done via /sys/kernel/mm/transparent_hugepage/enabled).
			///  It can possibly improve performance and reduced allocation overhead in some contexts, albeit
			///  THP is usually enabled by default.
			int disable_thp;
		#endif
		} rpmalloc_config_t;

		//! Initialize allocator
		RPMALLOC_EXPORT int
		rpmalloc_initialize(rpmalloc_interface_t* memory_interface);

		//! Initialize allocator
		RPMALLOC_EXPORT int
		rpmalloc_initialize_config(rpmalloc_interface_t* memory_interface, rpmalloc_config_t* config);

		//! Get allocator configuration
		RPMALLOC_EXPORT const rpmalloc_config_t*
		rpmalloc_config(void);

		//! Finalize allocator
		RPMALLOC_EXPORT void
		rpmalloc_finalize(void);

		//! Initialize allocator for calling thread
		RPMALLOC_EXPORT void
		rpmalloc_thread_initialize(void);

		//! Finalize allocator for calling thread
		RPMALLOC_EXPORT void
		rpmalloc_thread_finalize(void);

		//! Perform deferred deallocations pending for the calling thread heap
		RPMALLOC_EXPORT void
		rpmalloc_thread_collect(void);

		//! Query if allocator is initialized for calling thread
		RPMALLOC_EXPORT int
		rpmalloc_is_thread_initialized(void);

		//! Get per-thread statistics
		RPMALLOC_EXPORT void
		rpmalloc_thread_statistics(rpmalloc_thread_statistics_t* stats);

		//! Get global statistics
		RPMALLOC_EXPORT void
		rpmalloc_global_statistics(rpmalloc_global_statistics_t* stats);

		//! Dump all statistics in human readable format to file (should be a FILE*)
		RPMALLOC_EXPORT void
		rpmalloc_dump_statistics(void* file);

		//! Allocate a memory block of at least the given size
		RPMALLOC_EXPORT RPMALLOC_ALLOCATOR void*
		rpmalloc(size_t size) RPMALLOC_ATTRIB_MALLOC RPMALLOC_ATTRIB_ALLOC_SIZE(1);

		//! Allocate a zero initialized memory block of at least the given size
		RPMALLOC_EXPORT RPMALLOC_ALLOCATOR void*
		rpzalloc(size_t size) RPMALLOC_ATTRIB_MALLOC RPMALLOC_ATTRIB_ALLOC_SIZE(1);

		//! Allocate a memory block of at least the given size and zero initialize it
		RPMALLOC_EXPORT RPMALLOC_ALLOCATOR void*
		rpcalloc(size_t num, size_t size) RPMALLOC_ATTRIB_MALLOC RPMALLOC_ATTRIB_ALLOC_SIZE2(1, 2);

		//! Reallocate the given block to at least the given size
		RPMALLOC_EXPORT RPMALLOC_ALLOCATOR void*
		rprealloc(void* ptr, size_t size) RPMALLOC_ATTRIB_MALLOC RPMALLOC_ATTRIB_ALLOC_SIZE(2);

		//! Reallocate the given block to at least the given size and alignment,
		//  with optional control flags (see RPMALLOC_NO_PRESERVE).
		//  Alignment must be a power of two and a multiple of sizeof(void*),
		//  and should ideally be less than memory page size. A caveat of rpmalloc
		//  internals is that this must also be strictly less than the span size (default 64KiB)
		RPMALLOC_EXPORT RPMALLOC_ALLOCATOR void*
		rpaligned_realloc(void* ptr, size_t alignment, size_t size, size_t oldsize, unsigned int flags) RPMALLOC_ATTRIB_MALLOC
			RPMALLOC_ATTRIB_ALLOC_SIZE(3);

		//! Allocate a memory block of at least the given size and alignment.
		//  Alignment must be a power of two and a multiple of sizeof(void*),
		//  and should ideally be less than memory page size. A caveat of rpmalloc
		//  internals is that this must also be strictly less than the span size (default 64KiB)
		RPMALLOC_EXPORT RPMALLOC_ALLOCATOR void*
		rpaligned_alloc(size_t alignment, size_t size) RPMALLOC_ATTRIB_MALLOC RPMALLOC_ATTRIB_ALLOC_SIZE(2);

		//! Allocate a memory block of at least the given size and alignment.
		//  Alignment must be a power of two and a multiple of sizeof(void*),
		//  and should ideally be less than memory page size. A caveat of rpmalloc
		//  internals is that this must also be strictly less than the span size (default 64KiB)
		RPMALLOC_EXPORT RPMALLOC_ALLOCATOR void*
		rpaligned_zalloc(size_t alignment, size_t size) RPMALLOC_ATTRIB_MALLOC RPMALLOC_ATTRIB_ALLOC_SIZE(2);

		//! Allocate a memory block of at least the given size and alignment, and zero initialize it.
		//  Alignment must be a power of two and a multiple of sizeof(void*),
		//  and should ideally be less than memory page size. A caveat of rpmalloc
		//  internals is that this must also be strictly less than the span size (default 64KiB)
		RPMALLOC_EXPORT RPMALLOC_ALLOCATOR void*
		rpaligned_calloc(size_t alignment, size_t num, size_t size) RPMALLOC_ATTRIB_MALLOC RPMALLOC_ATTRIB_ALLOC_SIZE2(2, 3);

		//! Allocate a memory block of at least the given size and alignment.
		//  Alignment must be a power of two and a multiple of sizeof(void*),
		//  and should ideally be less than memory page size. A caveat of rpmalloc
		//  internals is that this must also be strictly less than the span size (default 64KiB)
		RPMALLOC_EXPORT RPMALLOC_ALLOCATOR void*
		rpmemalign(size_t alignment, size_t size) RPMALLOC_ATTRIB_MALLOC RPMALLOC_ATTRIB_ALLOC_SIZE(2);

		//! Allocate a memory block of at least the given size and alignment.
		//  Alignment must be a power of two and a multiple of sizeof(void*),
		//  and should ideally be less than memory page size. A caveat of rpmalloc
		//  internals is that this must also be strictly less than the span size (default 64KiB)
		RPMALLOC_EXPORT int
		rpposix_memalign(void** memptr, size_t alignment, size_t size);

		//! Free the given memory block
		RPMALLOC_EXPORT void
		rpfree(void* ptr);

		//! Query the usable size of the given memory block (from given pointer to the end of block)
		RPMALLOC_EXPORT size_t
		rpmalloc_usable_size(void* ptr);

		//! Dummy empty function for forcing linker symbol inclusion
		RPMALLOC_EXPORT void
		rpmalloc_linker_reference(void);

		#if RPMALLOC_FIRST_CLASS_HEAPS

		//! Heap type
		typedef struct heap_t rpmalloc_heap_t;

		//! Acquire a new heap. Will reuse existing released heaps or allocate memory for a new heap
		//  if none available. Heap API is implemented with the strict assumption that only one single
		//  thread will call heap functions for a given heap at any given time, no functions are thread safe.
		RPMALLOC_EXPORT rpmalloc_heap_t*
		rpmalloc_heap_acquire(void);

		//! Release a heap (does NOT free the memory allocated by the heap, use rpmalloc_heap_free_all before destroying the
		//! heap).
		//  Releasing a heap will enable it to be reused by other threads. Safe to pass a null pointer.
		RPMALLOC_EXPORT void
		rpmalloc_heap_release(rpmalloc_heap_t* heap);

		//! Allocate a memory block of at least the given size using the given heap.
		RPMALLOC_EXPORT RPMALLOC_ALLOCATOR void*
		rpmalloc_heap_alloc(rpmalloc_heap_t* heap, size_t size) RPMALLOC_ATTRIB_MALLOC RPMALLOC_ATTRIB_ALLOC_SIZE(2);

		//! Allocate a memory block of at least the given size using the given heap. The returned
		//  block will have the requested alignment. Alignment must be a power of two and a multiple of sizeof(void*),
		//  and should ideally be less than memory page size. A caveat of rpmalloc
		//  internals is that this must also be strictly less than the span size (default 64KiB).
		RPMALLOC_EXPORT RPMALLOC_ALLOCATOR void*
		rpmalloc_heap_aligned_alloc(rpmalloc_heap_t* heap, size_t alignment, size_t size) RPMALLOC_ATTRIB_MALLOC
			RPMALLOC_ATTRIB_ALLOC_SIZE(3);

		//! Allocate a memory block of at least the given size using the given heap and zero initialize it.
		RPMALLOC_EXPORT RPMALLOC_ALLOCATOR void*
		rpmalloc_heap_calloc(rpmalloc_heap_t* heap, size_t num, size_t size) RPMALLOC_ATTRIB_MALLOC
			RPMALLOC_ATTRIB_ALLOC_SIZE2(2, 3);

		//! Allocate a memory block of at least the given size using the given heap and zero initialize it. The returned
		//  block will have the requested alignment. Alignment must either be zero, or a power of two and a multiple of
		//  sizeof(void*), and should ideally be less than memory page size. A caveat of rpmalloc internals is that this must
		//  also be strictly less than the span size (default 64KiB).
		RPMALLOC_EXPORT RPMALLOC_ALLOCATOR void*
		rpmalloc_heap_aligned_calloc(rpmalloc_heap_t* heap, size_t alignment, size_t num, size_t size) RPMALLOC_ATTRIB_MALLOC
			RPMALLOC_ATTRIB_ALLOC_SIZE2(2, 3);

		//! Reallocate the given block to at least the given size. The memory block MUST be allocated
		//  by the same heap given to this function.
		RPMALLOC_EXPORT RPMALLOC_ALLOCATOR void*
		rpmalloc_heap_realloc(rpmalloc_heap_t* heap, void* ptr, size_t size, unsigned int flags) RPMALLOC_ATTRIB_MALLOC
			RPMALLOC_ATTRIB_ALLOC_SIZE(3);

		//! Reallocate the given block to at least the given size. The memory block MUST be allocated
		//  by the same heap given to this function. The returned block will have the requested alignment.
		//  Alignment must be either zero, or a power of two and a multiple of sizeof(void*), and should ideally be
		//  less than memory page size. A caveat of rpmalloc internals is that this must also be strictly less than
		//  the span size (default 64KiB).
		RPMALLOC_EXPORT RPMALLOC_ALLOCATOR void*
		rpmalloc_heap_aligned_realloc(rpmalloc_heap_t* heap, void* ptr, size_t alignment, size_t size,
									  unsigned int flags) RPMALLOC_ATTRIB_MALLOC RPMALLOC_ATTRIB_ALLOC_SIZE(4);

		//! Free the given memory block from the given heap. The memory block MUST be allocated
		//  by the same heap given to this function.
		RPMALLOC_EXPORT void
		rpmalloc_heap_free(rpmalloc_heap_t* heap, void* ptr);

		//! Free all memory allocated by the heap
		RPMALLOC_EXPORT void
		rpmalloc_heap_free_all(rpmalloc_heap_t* heap);

		//! Set the given heap as the current heap for the calling thread. A heap MUST only be current heap
		//  for a single thread, a heap can never be shared between multiple threads. The previous
		//  current heap for the calling thread is released to be reused by other threads.
		RPMALLOC_EXPORT void
		rpmalloc_heap_thread_set_current(rpmalloc_heap_t* heap);

		//! Returns which heap the given pointer is allocated on
		RPMALLOC_EXPORT rpmalloc_heap_t*
		rpmalloc_get_heap_for_ptr(void* ptr);

		#endif
		
	#endif
	
	
	
	
	
	/*
		Struct Memory Management Unit [结构体内存管理单元]
			成员编号规则（0为不存在的成员编号）：
				┌──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬────┐
				│01│02│03│04│05│06│07│08│09│10│11│12│ .. │
				└──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴────┘
	*/
	
	#ifdef MMU_USE_SMMU
		
		// 结构体内存管理单元数据结构
		typedef struct {
			char* Memory;						// 管理器内存指针
			unsigned int ItemLength;			// 成员占用内存长度
			unsigned int Count;					// 管理器中存在多少成员
			unsigned int AllocCount;			// 已经申请的结构数量
			unsigned int AllocStep;				// 预分配内存步长
		} SMMU_Struct, *SMMU_Object;
		
		// 创建结构化内存管理器
		XXAPI SMMU_Object SMMU_Create(unsigned int iItemLength, unsigned int PreassignStep);
		
		// 销毁结构化内存管理器
		XXAPI void SMMU_Destroy(SMMU_Object pObject);
		
		// 初始化内存管理器（对自维护结构体指针使用，和 SMMU_Create 功能类似）
		XXAPI void SMMU_Init(SMMU_Object pObject, unsigned int iItemLength, unsigned int PreassignStep);
		
		// 释放内存管理器（对自维护结构体指针使用，和 SMMU_Destroy 功能类似）
		XXAPI void SMMU_Unit(SMMU_Object pObject);
		
		// 分配内存
		XXAPI int SMMU_Malloc(SMMU_Object pObject, unsigned int iCount);
		
		// 中间插入成员
		XXAPI unsigned int SMMU_Insert(SMMU_Object pObject, unsigned int iPos, unsigned int iCount);
		
		// 末尾添加成员
		XXAPI unsigned int SMMU_Append(SMMU_Object pObject, unsigned int iCount);
		
		// 交换成员
		XXAPI int SMMU_Swap(SMMU_Object pObject, unsigned int iPosA, unsigned int iPosB);
		
		// 删除成员
		XXAPI int SMMU_Remove(SMMU_Object pObject, unsigned int iPos, unsigned int iCount);
		
		// 获取成员指针
		XXAPI void* SMMU_GetPtr(SMMU_Object pObject, unsigned int iPos);
		XXAPI void* SMMU_GetPtr_Unsafe(SMMU_Object pObject, unsigned int iPos);
		static inline void* SMMU_GetPtr_Inline(SMMU_Object pObject, unsigned int iPos)
		{
			return &(pObject->Memory[(iPos - 1) * pObject->ItemLength]);
		}
		
		// 成员排序
		XXAPI int SMMU_Sort(SMMU_Object pObject, void* procCompar);
		
	#endif
	
	
	
	
	
	/*
		Point Memory Management Unit [指针内存管理单元]
			成员编号规则（0为不存在的成员编号）：
				┌──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬────┐
				│01│02│03│04│05│06│07│08│09│10│11│12│ .. │
				└──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴────┘
	*/
	
	#ifdef MMU_USE_PMMU
		
		// 申请步长
		#define PMMU_PREASSIGNSTEP	256
		
		// 指针内存管理单元数据结构
		typedef struct {
			void** Memory;							// 管理器内存指针
			unsigned int Count;						// 管理器中存在多少成员
			unsigned int AllocCount;				// 已经申请的结构数量
			unsigned int AllocStep;					// 预分配内存步长
		} PMMU_Struct, *PMMU_Object;
		
		// 创建指针内存管理器
		XXAPI PMMU_Object PMMU_Create();
		
		// 销毁指针内存管理器
		XXAPI void PMMU_Destroy(PMMU_Object pObject);
		
		// 初始化内存管理器（对自维护结构体指针使用，和 PMMU_Create 功能类似）
		XXAPI void PMMU_Init(PMMU_Object pObject);
		
		// 释放内存管理器（对自维护结构体指针使用，和 PMMU_Destroy 功能类似）
		XXAPI void PMMU_Unit(PMMU_Object pObject);
		
		// 分配内存
		XXAPI int PMMU_Malloc(PMMU_Object pObject, unsigned int iCount);
		
		// 中间插入成员(0为头部插入，pObject->Count为末尾插入)
		XXAPI unsigned int PMMU_Insert(PMMU_Object pObject, unsigned int iPos, void* pVal);
		
		// 末尾添加成员
		XXAPI unsigned int PMMU_Append(PMMU_Object pObject, void* pVal);
		
		// 添加成员，自动查找空隙（替换为 NULL 的值）
		XXAPI unsigned int PMMU_Add(PMMU_Object pObject, void* pVal);
		
		// 交换成员
		XXAPI int PMMU_Swap(PMMU_Object pObject, unsigned int iPosA, unsigned int iPosB);
		
		// 删除成员
		XXAPI int PMMU_Remove(PMMU_Object pObject, unsigned int iPos, unsigned int iCount);
		
		// 获取成员指针
		XXAPI void** PMMU_GetPtr(PMMU_Object pObject, unsigned int iPos);
		XXAPI void* PMMU_GetVal(PMMU_Object pObject, unsigned int iPos);
		XXAPI void** PMMU_GetPtr_Unsafe(PMMU_Object pObject, unsigned int iPos);
		XXAPI void* PMMU_GetVal_Unsafe(PMMU_Object pObject, unsigned int iPos);
		static inline void** PMMU_GetPtr_Inline(PMMU_Object pObject, unsigned int iPos)
		{
			return &(pObject->Memory[iPos - 1]);
		}
		static inline void* PMMU_GetVal_Inline(PMMU_Object pObject, unsigned int iPos)
		{
			return pObject->Memory[iPos - 1];
		}
		
		// 设置成员指针
		XXAPI int PMMU_SetVal(PMMU_Object pObject, unsigned int iPos, void* pVal);
		XXAPI void PMMU_SetVal_Unsafe(PMMU_Object pObject, unsigned int iPos, void* pVal);
		static inline void PMMU_SetVal_Inline(PMMU_Object pObject, unsigned int iPos, void* pVal)
		{
			pObject->Memory[iPos - 1] = pVal;
		}
		
		// 成员排序
		XXAPI int PMMU_Sort(PMMU_Object pObject, void* procCompar);
		
	#endif
	
	
	
	
	
	/*
		Memory Buffer Management Unit [内存缓冲区管理单元]
	*/
	
	#ifdef MMU_USE_MBMU
		
		// 内容类型
		#define MBMU_BINARY 0						// 二进制
		#define MBMU_ANSI 1							// ANSI 字符串
		#define MBMU_UNICODE 2						// UTF16 字符串
		#define MBMU_UTF8 1							// UTF8 字符串
		#define MBMU_UTF32 4						// UTF32 字符串
		
		// 默认增量长度
		#define MBMU_AllocStep 0x10000
		
		// 内存缓冲区管理单元数据结构
		typedef struct {
			char* Buffer;							// 内存缓冲区
			unsigned int Length;					// 内存长度
			unsigned int AllocLength;				// 已申请内存长度
			unsigned int AllocStep;					// 预分配内存步长
		} MBMU_Struct, *MBMU_Object;
		
		// 创建内存缓冲区管理器
		XXAPI MBMU_Object MBMU_Create(unsigned int iAllocLength, unsigned int iStep);
		
		// 销毁内存缓冲区管理器
		XXAPI void MBMU_Destroy(MBMU_Object pObject);
		
		// 初始化缓冲区管理器（对自维护结构体指针使用，和 MBMU_Create 功能类似）
		XXAPI void MBMU_Init(MBMU_Object pObject, unsigned int iAllocLength, unsigned int iStep);
		
		// 释放缓冲区管理器（对自维护结构体指针使用，和 MBMU_Destroy 功能类似）
		XXAPI void MBMU_Unit(MBMU_Object pObject);
		
		// 分配内存
		XXAPI int MBMU_Malloc(MBMU_Object pObject, unsigned int iCount);
		
		// 中间添加数据（可以复制或者开辟新的数据区，不会自动将新开辟的数据区填充 \0）
		XXAPI int MBMU_Insert(MBMU_Object pObject, unsigned int iPos, void* pData, unsigned int iSize, unsigned int bStrMode);
		
		// 末尾添加数据
		XXAPI int MBMU_Append(MBMU_Object pObject, void* pData, unsigned int iSize, unsigned int bStrMode);
		
	#endif
	
	
	
	
	
	/*
		Memory Management Unit 256
			固定 256 个数量的内存管理单元
			不提供结构体调用方式（MMU256 是基础内存管理单元，需要作为MM256阵列单元使用，结构体调用不具备实用性）
			首个内存数据必定为4字节对齐（如果管理的内存数据本身也是4字节对齐的，则所有内存单元都是4字节对齐的）
			提供内联的 Alloc 和 Free 函数以加快内存分配效率
	*/
	
	#ifdef MMU_USE_MMU256
		
		// 256步进数据管理单元数据结构
		typedef struct {
			char* Memory;								// 管理器内存指针（和本结构体同时申请，所以不需要释放）
			unsigned char FreeList[256];				// 已释放成员列表
			unsigned int ItemLength;					// 成员占用内存长度
			unsigned short Count;						// 成员数量
			unsigned char FreeCount;					// 已释放成员数量
			unsigned char FreeOffset;					// 首个已释放成员在列表中的偏移位置
			unsigned int Flag;							// 值的 Flag 前缀（由上级管理器下发，0-255 区间会被 idx 覆盖）
		} MMU256_Struct, *MMU256_Object;
		
		// 创建内存管理单元（iItemLength会自动增加4个字节用于确定内存位置和所属的管理器单元编号）
		XXAPI MMU256_Object MMU256_Create(unsigned int iItemLength);
		
		// 销毁内存管理单元
		#define MMU256_Destroy mmu_free
		
		// 从内存管理单元中申请一个元素（内联函数没有进行指针和边界检查）
		static inline void* MMU256_Alloc_Inline(MMU256_Object objUnit)
		{
			unsigned char idx = objUnit->Count;
			// 优先复用已释放的数据
			if ( objUnit->FreeCount > 0 ) {
				idx = objUnit->FreeList[objUnit->FreeOffset];
				objUnit->FreeOffset++;
				objUnit->FreeCount--;
			}
			objUnit->Count++;
			MMU_ValuePtr v = (MMU_ValuePtr)&(objUnit->Memory[objUnit->ItemLength * idx]);
			v->ItemFlag = objUnit->Flag | idx;
			return (void*)&v[1];
		}
		XXAPI void* MMU256_Alloc(MMU256_Object objUnit);
		
		// 释放内存管理单元中某个元素（FreeIdx不会清空 ItemFlag，建议由调用方负责清空）
		static inline void MMU256_FreeIdx_Inline(MMU256_Object objUnit, unsigned char idx)
		{
			objUnit->FreeList[(objUnit->FreeOffset + objUnit->FreeCount) & 0xFF] = idx;
			objUnit->Count--;
			if ( objUnit->Count ) {
				objUnit->FreeCount++;
			} else {
				objUnit->FreeCount = 0;
				objUnit->FreeOffset = 0;
			}
		}
		XXAPI void MMU256_FreeIdx(MMU256_Object objUnit, unsigned char idx);
		static inline void MMU256_Free_Inline(MMU256_Object objUnit, void* obj)
		{
			MMU_ValuePtr v = obj - 4;
			unsigned char idx = v->ItemFlag & 0xFF;
			v->ItemFlag = 0;
			MMU256_FreeIdx_Inline(objUnit, idx);
		}
		XXAPI void MMU256_Free(MMU256_Object objUnit, void* obj);
		
	#endif
	
	
	
	
	
	/*
		Memory Management Unit 64K
			固定 65536 个数量的内存管理单元
			一次申请占用内存过多，通常用于内存占用换效率的场合
			不提供结构体调用方式（MMU64K 是基础内存管理单元，需要作为MM64K阵列单元使用，结构体调用不具备实用性）
			提供内联的 Alloc 和 Free 函数以加快内存分配效率
	*/
	
	#ifdef MMU_USE_MMU64K
		
		// 64K步进数据管理单元数据结构
		typedef struct {
			char* Memory;								// 管理器内存指针（和本结构体同时申请，所以不需要释放）
			unsigned char FreeList[65536];				// 已释放成员列表
			unsigned int ItemLength;					// 成员占用内存长度
			unsigned int Count;							// 成员数量
			unsigned short FreeCount;					// 已释放成员数量
			unsigned short FreeOffset;					// 首个已释放成员在列表中的偏移位置
			unsigned int Flag;							// 值的 Flag 前缀（由上级管理器下发，0-255 区间会被 idx 覆盖）
			unsigned short ForEachStep;					// 遍历当前 idx
		} MMU64K_Struct, *MMU64K_Object;
		
		// 创建内存管理单元（iItemLength会自动增加4个字节用于确定内存位置和所属的管理器单元编号）
		XXAPI MMU64K_Object MMU64K_Create(unsigned int iItemLength);
		
		// 销毁内存管理单元
		#define MMU64K_Destroy mmu_free
		
		// 从内存管理单元中申请一个元素（内联函数没有进行指针和边界检查）
		static inline void* MMU64K_Alloc_Inline(MMU64K_Object objUnit)
		{
			unsigned short idx = objUnit->Count;
			// 优先复用已释放的数据
			if ( objUnit->FreeCount > 0 ) {
				idx = objUnit->FreeList[objUnit->FreeOffset];
				objUnit->FreeOffset++;
				objUnit->FreeCount--;
			}
			objUnit->Count++;
			MMU_ValuePtr v = (MMU_ValuePtr)&(objUnit->Memory[objUnit->ItemLength * idx]);
			v->ItemFlag = objUnit->Flag | idx;
			return (void*)&v[1];
		}
		XXAPI void* MMU64K_Alloc(MMU64K_Object objUnit);
		
		// 释放内存管理单元中某个元素
		static inline void MMU64K_FreeIdx_Inline(MMU64K_Object objUnit, unsigned char idx)
		{
			objUnit->FreeList[(objUnit->FreeOffset + objUnit->FreeCount) & 0xFFFF] = idx;
			objUnit->Count--;
			if ( objUnit->Count ) {
				objUnit->FreeCount++;
			} else {
				objUnit->FreeCount = 0;
				objUnit->FreeOffset = 0;
			}
		}
		XXAPI void MMU64K_FreeIdx(MMU64K_Object objUnit, unsigned short idx);
		static inline void MMU64K_Free_Inline(MMU64K_Object objUnit, void* obj)
		{
			MMU_ValuePtr v = obj - 4;
			unsigned char idx = v->ItemFlag & 0xFFFF;
			v->ItemFlag = 0;
			MMU64K_FreeIdx_Inline(objUnit, idx);
		}
		XXAPI void MMU64K_Free(MMU64K_Object objUnit, void* obj);
		
	#endif
	
	
	
	
	
	/*
		Memory Management 256
			内存管理器（固定大小的内存池，使用 MMU256 加速分配和释放）
	*/
	
	#ifdef MMU_USE_MM256
		
		// 256步进内存管理器数据结构
		typedef struct {
			unsigned int ItemLength;					// 成员占用内存长度
			PMMU_Struct MMU;							// MMU 管理器
			MMU_OnErrorProc OnError;					// 错误处理回调函数
		} MM256_Struct, *MM256_Object;
		
		// 创建内存管理器
		XXAPI MM256_Object MM256_Create(unsigned int iItemLength);
		
		// 销毁内存管理器
		XXAPI void MM256_Destroy(MM256_Object objMM);
		
		// 初始化内存管理器（对自维护结构体指针使用，和 MM256_Create 功能类似）
		XXAPI void MM256_Init(MM256_Object objMM, unsigned int iItemLength);
		
		// 释放内存管理器（对自维护结构体指针使用，和 MM256_Destroy 功能类似）
		XXAPI void MM256_Unit(MM256_Object objMM);
		
		// 从内存管理器中申请一块内存
		XXAPI void* MM256_Alloc(MM256_Object objMM);
		
		// 将内存管理器申请的内存释放掉
		XXAPI void MM256_Free(MM256_Object objMM, void* ptr);
		
		// 将一块内存标记为使用中
		XXAPI void MM256_GC_Mark(void* ptr);
		
		// 进行一轮GC，将未标记为使用中的内存全部回收
		XXAPI void MM256_GC(MM256_Object objMM, int bFreeMark);
		
	#endif
	
	
	
	
	
	/*
		Memory Management 64K
			内存管理器（固定大小的内存池，使用 MMU64K 加速分配和释放）
	*/
	
	#ifdef MMU_USE_MM64K
		
		// 64K步进内存管理器数据结构
		typedef struct {
			unsigned int ItemLength;					// 成员占用内存长度
			PMMU_Struct MMU;							// MMU 管理器
			MMU_OnErrorProc OnError;					// 错误处理回调函数
		} MM64K_Struct, *MM64K_Object;
		
		// 创建内存管理器
		XXAPI MM64K_Object MM64K_Create(unsigned int iItemLength);
		
		// 销毁内存管理器
		XXAPI void MM64K_Destroy(MM64K_Object objMM);
		
		// 初始化内存管理器（对自维护结构体指针使用，和 MM64K_Create 功能类似）
		XXAPI void MM64K_Init(MM64K_Object objMM, unsigned int iItemLength);
		
		// 释放内存管理器（对自维护结构体指针使用，和 MM64K_Destroy 功能类似）
		XXAPI void MM64K_Unit(MM64K_Object objMM);
		
		// 从内存管理器中申请一块内存
		XXAPI void* MM64K_Alloc(MM64K_Object objMM);
		
		// 将内存管理器申请的内存释放掉
		XXAPI void MM64K_Free(MM64K_Object objMM, void* ptr);
		
		// 将一块内存标记为使用中
		XXAPI void MM64K_GC_Mark(void* ptr);
		
		// 进行一轮GC，将未标记为使用中的内存全部回收
		XXAPI void MM64K_GC(MM64K_Object objMM, int bFreeMark);
		
	#endif
	
	
	
	
	
	/*
		Struct Static Stack [结构体静态栈，初始化时申请内存，栈最大深度固定]
			不提供结构体调用方式（管理内存长度超过了结构体本身的长度，必须通过 SSSTK_Create 创建）
			成员编号规则（0为不存在的成员编号）：
				┌──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬────┐
				│01│02│03│04│05│06│07│08│09│10│11│12│ .. │
				└──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴────┘
	*/
	
	#ifdef MMU_USE_SSSTK
		
		// 结构体静态栈数据结构
		typedef struct {
			char* Memory;						// 栈数据内存指针
			unsigned int ItemLength;			// 栈成员占用内存长度
			unsigned int MaxCount;				// 栈最大可以容纳多少成员（栈深度）
			unsigned int Count;					// 栈中存在多少成员（栈顶位置）
		} SSSTK_Struct, *SSSTK_Object;
		
		// 创建结构体静态栈
		XXAPI SSSTK_Object SSSTK_Create(unsigned int iMaxCount, unsigned int iItemLength);
		
		// 销毁结构体静态栈
		#define SSSTK_Destroy mmu_free
		
		// 压栈
		XXAPI void* SSSTK_Push(SSSTK_Object objSTK);
		XXAPI unsigned int SSSTK_PushData(SSSTK_Object objSTK, void* pData);
		
		// 出栈
		XXAPI void* SSSTK_Pop(SSSTK_Object objSTK);
		
		// 获取栈顶对象
		XXAPI void* SSSTK_Top(SSSTK_Object objSTK);
		
		// 获取任意位置对象
		XXAPI void* SSSTK_GetPos(SSSTK_Object objSTK, unsigned int iPos);
		XXAPI void* SSSTK_GetPos_Unsafe(SSSTK_Object objSTK, unsigned int iPos);
		
	#endif
	
	
	
	
	
	/*
		Point Static Stack [指针静态栈，初始化时申请内存，栈最大深度固定]
			不提供结构体调用方式（管理内存长度超过了结构体本身的长度，必须通过 SSSTK_Create 创建）
			成员编号规则（0为不存在的成员编号）：
				┌──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬────┐
				│01│02│03│04│05│06│07│08│09│10│11│12│ .. │
				└──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴────┘
	*/
	
	#ifdef MMU_USE_PSSTK
		
		// 指针静态栈数据结构
		typedef struct {
			void** Memory;						// 栈数据内存指针
			unsigned int MaxCount;				// 栈最大可以容纳多少成员（栈深度）
			unsigned int Count;					// 栈中存在多少成员（栈顶位置）
		} PSSTK_Struct, *PSSTK_Object;
		
		// 创建指针静态栈
		XXAPI PSSTK_Object PSSTK_Create(unsigned int iMaxCount);
		
		// 销毁指针静态栈
		#define PSSTK_Destroy mmu_free
		
		// 压栈
		XXAPI unsigned int PSSTK_Push(PSSTK_Object objSTK, void* ptr);
		
		// 出栈
		XXAPI void* PSSTK_Pop(PSSTK_Object objSTK);
		
		// 获取栈顶指针
		XXAPI void* PSSTK_Top(PSSTK_Object objSTK);
		
		// 获取任意位置指针
		XXAPI void* PSSTK_GetPos(PSSTK_Object objSTK, unsigned int iPos);
		XXAPI void* PSSTK_GetPos_Unsafe(PSSTK_Object objSTK, unsigned int iPos);
		
	#endif
	
	
	
	
	
	/*
		Struct Dynamic Stack [结构体动态栈，结构体内存256个递增，栈最大深度不固定]
			成员编号规则（0为不存在的成员编号）：
				┌──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬────┐
				│01│02│03│04│05│06│07│08│09│10│11│12│ .. │
				└──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴────┘
	*/
	
	#ifdef MMU_USE_SDSTK
		
		// 结构体动态栈数据结构
		typedef struct {
			unsigned int ItemLength;					// 栈成员占用内存长度
			unsigned int Count;							// 栈中存在多少成员（栈顶位置）
			PMMU_Struct MMU;							// MMU 管理器
			MMU_OnErrorProc OnError;					// 错误处理回调函数
		} SDSTK_Struct, *SDSTK_Object;
		
		// 创建结构体动态栈
		XXAPI SDSTK_Object SDSTK_Create(unsigned int iItemLength);
		
		// 销毁结构体动态栈
		XXAPI void SDSTK_Destroy(SDSTK_Object objSTK);
		
		// 初始化结构体动态栈（对自维护结构体指针使用，和 SDSTK_Create 功能类似）
		XXAPI void SDSTK_Init(SDSTK_Object objSTK, unsigned int iItemLength);
		
		// 释放结构体动态栈（对自维护结构体指针使用，和 SDSTK_Create 功能类似）
		XXAPI void SDSTK_Unit(SDSTK_Object objSTK);
		
		// 压栈
		XXAPI void* SDSTK_Push(SDSTK_Object objSTK);
		XXAPI unsigned int SDSTK_PushData(SDSTK_Object objSTK, void* pData);
		
		// 出栈
		XXAPI void* SDSTK_Pop(SDSTK_Object objSTK);
		
		// 获取栈顶对象
		XXAPI void* SDSTK_Top(SDSTK_Object objSTK);
		
		// 获取任意位置对象
		XXAPI void* SDSTK_GetPos(SDSTK_Object objSTK, unsigned int iPos);
		XXAPI void* SDSTK_GetPos_Unsafe(SDSTK_Object objSTK, unsigned int iPos);
		
	#endif
	
	
	
	
	
	/*
		Point Dynamic Stack [指针动态栈，结构体内存256个递增，栈最大深度不固定]
			成员编号规则（0为不存在的成员编号）：
				┌──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬────┐
				│01│02│03│04│05│06│07│08│09│10│11│12│ .. │
				└──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴────┘
	*/
	
	#ifdef MMU_USE_PDSTK
		
		// 指针动态栈数据结构
		typedef struct {
			unsigned int Count;							// 栈中存在多少成员（栈顶位置）
			PMMU_Struct MMU;							// MMU 管理器
			MMU_OnErrorProc OnError;					// 错误处理回调函数
		} PDSTK_Struct, *PDSTK_Object;
		
		// 创建指针动态栈
		XXAPI PDSTK_Object PDSTK_Create();
		
		// 销毁指针动态栈
		XXAPI void PDSTK_Destroy(PDSTK_Object objSTK);
		
		// 初始化指针动态栈（对自维护结构体指针使用，和 PDSTK_Create 功能类似）
		XXAPI void PDSTK_Init(PDSTK_Object objSTK);
		
		// 释放指针动态栈（对自维护结构体指针使用，和 PDSTK_Create 功能类似）
		XXAPI void PDSTK_Unit(PDSTK_Object objSTK);
		
		// 压栈
		XXAPI unsigned int PDSTK_Push(PDSTK_Object objSTK, void* ptr);
		
		// 出栈
		XXAPI void* PDSTK_Pop(PDSTK_Object objSTK);
		
		// 获取栈顶指针
		XXAPI void* PDSTK_Top(PDSTK_Object objSTK);
		
		// 获取任意位置指针
		XXAPI void* PDSTK_GetPos(PDSTK_Object objSTK, unsigned int iPos);
		XXAPI void* PDSTK_GetPos_Unsafe(PDSTK_Object objSTK, unsigned int iPos);
		
	#endif
	
	
	
	
	
	/*
		Linked List [双向链表，使用 MM256 管理内存]
	*/
	
	#ifdef MMU_USE_LLIST
		
		// 双向链表节点基础定义
		typedef struct LList_NodeBase {
			struct LList_NodeBase* Prev;
			struct LList_NodeBase* Next;
		} LList_NodeBase;
		
		// 链表对象数据结构
		typedef struct {
			LList_NodeBase* FirstNode;
			LList_NodeBase* LastNode;
			unsigned int Count;
			MM256_Struct objMM;
		} LList_Struct, *LList_Object;
		
		// 创建链表
		XXAPI LList_Object LList_Create(unsigned int iItemLength);
		
		// 销毁链表
		XXAPI void LList_Destroy(LList_Object objLL);
		
		// 初始化链表（对自维护结构体指针使用，和 LList_Create 功能类似）
		XXAPI void LList_Init(LList_Object objLL, unsigned int iItemLength);
		
		// 释放链表（对自维护结构体指针使用，和 LList_Destroy 功能类似）
		XXAPI void LList_Unit(LList_Object objLL);
		
		// 节点前插入 (objNode为空则插入到FirstNode之前)
		XXAPI LList_NodeBase* LList_InsertPrev(LList_Object objLL, LList_NodeBase* objNode);
		
		// 节点后插入 (objNode为空则插入到LastNode之后)
		XXAPI LList_NodeBase* LList_InsertNext(LList_Object objLL, LList_NodeBase* objNode);
		
		// 删除节点
		XXAPI LList_NodeBase* LList_Remove(LList_Object objLL, LList_NodeBase* objNode, int bRetPrev);
		
	#endif
	
	
	
	
	
	/*
		AVLTree
	*/
	
	#ifdef MMU_USE_AVLTREE
		
		// AVL树最大高度
		#define AVLTree_MAX_HEIGHT  42
		
		// AVL树节点基础定义
		typedef struct AVLTree_NodeBase {
			struct AVLTree_NodeBase* left;
			struct AVLTree_NodeBase* right;
			int height;
		} AVLTree_NodeBase;
		
		// 比较回调函数
		typedef int (*AVLTree_CompProc)(void* pNode, void* pKey);
		
		// 遍历回调函数
		typedef int (*AVLTree_EachProc)(void* pNode, void* pArg);
		
		// AVL树对象数据结构
		typedef struct {
			AVLTree_NodeBase* RootNode;
			AVLTree_CompProc CompProc;
			unsigned int Count;
			MM256_Struct objMM;
		} AVLTree_Struct, *AVLTree_Object;
		
		// 创建 AVLTree
		XXAPI AVLTree_Object AVLTree_Create(unsigned int iItemLength, AVLTree_CompProc procComp);
		
		// 销毁 AVLTree
		XXAPI void AVLTree_Destroy(AVLTree_Object objAVLT);
		
		// 初始化 AVLTree（对自维护结构体指针使用，和 AVLTree_Create 功能类似）
		XXAPI void AVLTree_Init(AVLTree_Object objAVLT, unsigned int iItemLength, AVLTree_CompProc procComp);
		
		// 释放 AVLTree（对自维护结构体指针使用，和 AVLTree_Destroy 功能类似）
		XXAPI void AVLTree_Unit(AVLTree_Object objAVLT);
		
		// 向 AVLTree 中插入节点，返回数据段指针（如果值已经存在，则会返回已存在的数据段指针）
		XXAPI void* AVLTree_AddNode(AVLTree_Object objAVLT, void* pKey, int* bNew);
		
		// 向 AVLTree 中插入节点（值必须不存在，如果值已经存在，则返回NULL）
		XXAPI void* AVLTree_Insert(AVLTree_Object objAVLT, void* pKey);
		
		// 从 AVLTree 中删除节点（成功返回 TRUE、失败返回 FALSE）
		XXAPI int AVLTree_Remove(AVLTree_Object objAVLT, void* pKey);
		
		// 从 AVLTree 中查找节点
		XXAPI void* AVLTree_Search(AVLTree_Object objAVLT, void* pKey);
		
		// 遍历 AVLTree 所有节点
		XXAPI void AVLTree_Walk(AVLTree_Object objAVLT, AVLTree_EachProc procEach, void* pArg);
		XXAPI void AVLTree_WalkEx(AVLTree_Object objAVLT, AVLTree_EachProc procPre, void* argPre, AVLTree_EachProc procIn, void* argIn, AVLTree_EachProc procPost, void* argPost);
		
		// 获取 AVLTree_NodeBase 结构体指针
		#define AVLTree_GetNodeBase(p) ((AVLTree_NodeBase*)((void*)p - sizeof(AVLTree_NodeBase)))
		
		// 获取 AVLTree_NodeBase 结构体指针对应的数据段
		#define AVLTree_GetNodeData(p) ((void*)(&p[1]))
		
		// 获取根节点数据段
		#define AVLTree_GetRootData(obj) AVLTree_GetNodeData(obj->RootNode)
		
	#endif
	
	
	
	
	
	/*
		RBTree
	*/
	
	#ifdef MMU_USE_RBTREE
		
		// 定义红黑树颜色
		#define MMU_RBT_RED		0
		#define MMU_RBT_BLACK	1
		
		// 红黑树节点基础定义
		typedef struct RBTree_NodeBase {
			struct RBTree_NodeBase* left;
			struct RBTree_NodeBase* right;
			intptr_t parent_color;
		} RBTree_NodeBase;
		
		// 功能宏定义
		#define rb_parent(r)   ((RBTree_NodeBase*)((r)->parent_color & ~3))
		#define rb_color(r)   ((r)->parent_color & 1)
		#define rb_is_red(r)   (!rb_color(r))
		#define rb_is_black(r) rb_color(r)
		#define rb_set_red(r)  (r)->parent_color &= ~1
		#define rb_set_black(r) (r)->parent_color |= 1
		static inline void rb_set_parent(RBTree_NodeBase* rb, RBTree_NodeBase* p)
		{
			rb->parent_color = (rb->parent_color & 3) | (intptr_t)p;
		}
		static inline void rb_set_color(RBTree_NodeBase* rb, int color)
		{
			rb->parent_color = (rb->parent_color & ~1) | color;
		}
		
		// 比较回调函数
		typedef int (*RBTree_CompProc)(void* pNode, void* pKey);
		
		// 遍历回调函数
		typedef int (*RBTree_EachProc)(void* pNode, void* pArg);
		
		// 红黑树对象数据结构
		typedef struct {
			RBTree_NodeBase* RootNode;
			RBTree_CompProc CompProc;
			unsigned int Count;
			MM256_Struct objMM;
		} RBTree_Struct, *RBTree_Object;
		
		// 创建 RBTree
		XXAPI RBTree_Object RBTree_Create(unsigned int iItemLength, RBTree_CompProc procComp);
		
		// 销毁 RBTree
		XXAPI void RBTree_Destroy(RBTree_Object objRBT);
		
		// 初始化 RBTree（对自维护结构体指针使用，和 RBTree_Create 功能类似）
		XXAPI void RBTree_Init(RBTree_Object objRBT, unsigned int iItemLength, RBTree_CompProc procComp);
		
		// 释放 RBTree（对自维护结构体指针使用，和 RBTree_Destroy 功能类似）
		XXAPI void RBTree_Unit(RBTree_Object objRBT);
		
		// 向 RBTree 中插入节点，返回数据段指针（如果值已经存在，则会返回已存在的数据段指针）
		XXAPI void* RBTree_AddNode(RBTree_Object objRBT, void* pKey, int* bNew);
		
		// 向 RBTree 中插入节点（值必须不存在，如果值已经存在，则返回NULL）
		XXAPI void* RBTree_Insert(RBTree_Object objRBT, void* pKey);
		
		// 从 RBTree 中删除节点（成功返回 TRUE、失败返回 FALSE）
		XXAPI int RBTree_Remove(RBTree_Object objRBT, void* pKey);
		
		// 从 RBTree 中查找节点
		XXAPI void* RBTree_Search(RBTree_Object objRBT, void* pKey);
		
		// 遍历 RBTree 所有节点
		XXAPI void RBTree_Walk(RBTree_Object objRBT, RBTree_EachProc procEach, void* pArg);
		XXAPI void RBTree_WalkEx(RBTree_Object objRBT, RBTree_EachProc procPre, void* argPre, RBTree_EachProc procIn, void* argIn, RBTree_EachProc procPost, void* argPost);
		
		// 获取 RBTree_NodeBase 结构体指针
		#define RBTree_GetNodeBase(p) ((RBTree_NodeBase*)((void*)p - sizeof(RBTree_NodeBase)))
		
		// 获取 RBTree_NodeBase 结构体指针对应的数据段
		#define RBTree_GetNodeData(p) ((void*)(&p[1]))
		
		// 获取根节点数据段
		#define RBTree_GetRootData(obj) RBTree_GetNodeData(obj->RootNode)
		
	#endif
	
	
	
	
	
	/*
		Hash32 - nmhash32x [Ver2.0, Update : 2024/10/18 from https://github.com/rurban/smhasher]
			使用协议注意事项：
				BSD 2-Clause 协议
				允许个人使用、商业使用
				复制、分发、修改，除了加上作者的版权信息，还必须保留免责声明，免除作者的责任
	*/
	
	#ifdef MMU_USE_HASH32
		
		// 默认 seed
		#define HASH32_SEED		0
		
		// 计算 32 位哈希值
		XXAPI unsigned int Hash32_WithSeed(void* key, size_t len, unsigned int seed);
		XXAPI unsigned int Hash32(void* key, size_t len);
		
		// 内联 32 位哈希计算
		#define Hash32Inline	NMHASH32X
		
	#endif
	
	
	
	
	
	/*
		Hash64 - rapidhash [Ver1.0, Update : 2024/10/18 from https://github.com/Nicoshev/rapidhash]
			使用协议注意事项：
				BSD 2-Clause 协议
				允许个人使用、商业使用
				复制、分发、修改，除了加上作者的版权信息，还必须保留免责声明，免除作者的责任
	*/
	
	#ifdef MMU_USE_HASH64
		
		// 默认 seed
		#define HASH64_SEED		(0xbdd89aa982704029ull)
		
		// 计算 64 位哈希值
		XXAPI unsigned long long Hash64_WithSeed(void* key, size_t len, unsigned long long seed);
		XXAPI unsigned long long Hash64(void* key, size_t len);
		
		// 内联 64 位哈希计算
		#define Hash64Inline	rapidhash_withSeed
		
	#endif
	
	
	
	
	
	/*
		AVLHT32 - AVLTree Hash Table (32bit Hash Value)
	*/
	
	#ifdef MMU_USE_AVLHT32
		
		// 32位 AVLTree 哈希表对象数据结构
		typedef struct {
			AVLTree_Struct AVLT;
		} AVLHT32_Struct, *AVLHT32_Object;
		
		// 创建哈希表
		XXAPI AVLHT32_Object AVLHT32_Create(unsigned int iItemLength);
		
		// 销毁哈希表
		XXAPI void AVLHT32_Destroy(AVLHT32_Object objHT);
		
		// 初始化哈希表（对自维护结构体指针使用，和 AVLHT32_Create 功能类似）
		XXAPI void AVLHT32_Init(AVLHT32_Object objHT, unsigned int iItemLength);
		
		// 释放哈希表（对自维护结构体指针使用，和 AVLHT32_Destroy 功能类似）
		XXAPI void AVLHT32_Unit(AVLHT32_Object objHT);
		
		// 设置值
		XXAPI void* AVLHT32_Set(AVLHT32_Object objHT, void* sKey, unsigned int iKeyLen);
		
		// 获取值
		XXAPI void* AVLHT32_Get(AVLHT32_Object objHT, void* sKey, unsigned int iKeyLen);
		
		// 删除值
		XXAPI int AVLHT32_Remove(AVLHT32_Object objHT, void* sKey, unsigned int iKeyLen);
		
		// 判断值是否存在
		XXAPI int AVLHT32_Exists(AVLHT32_Object objHT, void* sKey, unsigned int iKeyLen);
		
		// 获取表内元素数量
		XXAPI unsigned int AVLHT32_Count(AVLHT32_Object objHT);
		
		// 遍历表元素
		XXAPI void AVLHT32_Walk(AVLHT32_Object objHT, HT32_EachProc procEach, void* pArg);
		
	#endif
	
	
	
	
	
	/*
		AVLHT64 - AVLTree Hash Table (64bit Hash Value)
	*/
	
	#ifdef MMU_USE_AVLHT64
		
		// 64位 AVLTree 哈希表对象数据结构
		typedef struct {
			AVLTree_Struct AVLT;
		} AVLHT64_Struct, *AVLHT64_Object;
		
		// 创建哈希表
		XXAPI AVLHT64_Object AVLHT64_Create(unsigned int iItemLength);
		
		// 销毁哈希表
		XXAPI void AVLHT64_Destroy(AVLHT64_Object objHT);
		
		// 初始化哈希表（对自维护结构体指针使用，和 AVLHT64_Create 功能类似）
		XXAPI void AVLHT64_Init(AVLHT64_Object objHT, unsigned int iItemLength);
		
		// 释放哈希表（对自维护结构体指针使用，和 AVLHT64_Destroy 功能类似）
		XXAPI void AVLHT64_Unit(AVLHT64_Object objHT);
		
		// 设置值
		XXAPI void* AVLHT64_Set(AVLHT64_Object objHT, void* sKey, unsigned int iKeyLen);
		
		// 获取值
		XXAPI void* AVLHT64_Get(AVLHT64_Object objHT, void* sKey, unsigned int iKeyLen);
		
		// 删除值
		XXAPI int AVLHT64_Remove(AVLHT64_Object objHT, void* sKey, unsigned int iKeyLen);
		
		// 判断值是否存在
		XXAPI int AVLHT64_Exists(AVLHT64_Object objHT, void* sKey, unsigned int iKeyLen);
		
		// 获取表内元素数量
		XXAPI unsigned int AVLHT64_Count(AVLHT64_Object objHT);
		
		// 遍历表元素
		XXAPI void AVLHT64_Walk(AVLHT64_Object objHT, HT64_EachProc procEach, void* pArg);
		
	#endif
	
	
	
	
	
	/*
		RBHT32 - RBTree Hash Table (32bit Hash Value)
	*/
	
	#ifdef MMU_USE_RBHT32
		
		// 32位 RBTree 哈希表对象数据结构
		typedef struct {
			RBTree_Struct RBT;
		} RBHT32_Struct, *RBHT32_Object;
		
		// 创建哈希表
		XXAPI RBHT32_Object RBHT32_Create(unsigned int iItemLength);
		
		// 销毁哈希表
		XXAPI void RBHT32_Destroy(RBHT32_Object objHT);
		
		// 初始化哈希表（对自维护结构体指针使用，和 RBHT32_Create 功能类似）
		XXAPI void RBHT32_Init(RBHT32_Object objHT, unsigned int iItemLength);
		
		// 释放哈希表（对自维护结构体指针使用，和 RBHT32_Destroy 功能类似）
		XXAPI void RBHT32_Unit(RBHT32_Object objHT);
		
		// 设置值
		XXAPI void* RBHT32_Set(RBHT32_Object objHT, void* sKey, unsigned int iKeyLen);
		
		// 获取值
		XXAPI void* RBHT32_Get(RBHT32_Object objHT, void* sKey, unsigned int iKeyLen);
		
		// 删除值
		XXAPI int RBHT32_Remove(RBHT32_Object objHT, void* sKey, unsigned int iKeyLen);
		
		// 判断值是否存在
		XXAPI int RBHT32_Exists(RBHT32_Object objHT, void* sKey, unsigned int iKeyLen);
		
		// 获取表内元素数量
		XXAPI unsigned int RBHT32_Count(RBHT32_Object objHT);
		
		// 遍历表元素
		XXAPI void RBHT32_Walk(RBHT32_Object objHT, HT32_EachProc procEach, void* pArg);
		
	#endif
	
	
	
	
	
	/*
		RBHT64 - RBTree Hash Table (64bit Hash Value)
	*/
	
	#ifdef MMU_USE_RBHT64
		
		// 64位 RBTree 哈希表对象数据结构
		typedef struct {
			RBTree_Struct RBT;
		} RBHT64_Struct, *RBHT64_Object;
		
		// 创建哈希表
		XXAPI RBHT64_Object RBHT64_Create(unsigned int iItemLength);
		
		// 销毁哈希表
		XXAPI void RBHT64_Destroy(RBHT64_Object objHT);
		
		// 初始化哈希表（对自维护结构体指针使用，和 RBHT64_Create 功能类似）
		XXAPI void RBHT64_Init(RBHT64_Object objHT, unsigned int iItemLength);
		
		// 释放哈希表（对自维护结构体指针使用，和 RBHT64_Destroy 功能类似）
		XXAPI void RBHT64_Unit(RBHT64_Object objHT);
		
		// 设置值
		XXAPI void* RBHT64_Set(RBHT64_Object objHT, void* sKey, unsigned int iKeyLen);
		
		// 获取值
		XXAPI void* RBHT64_Get(RBHT64_Object objHT, void* sKey, unsigned int iKeyLen);
		
		// 删除值
		XXAPI int RBHT64_Remove(RBHT64_Object objHT, void* sKey, unsigned int iKeyLen);
		
		// 判断值是否存在
		XXAPI int RBHT64_Exists(RBHT64_Object objHT, void* sKey, unsigned int iKeyLen);
		
		// 获取表内元素数量
		XXAPI unsigned int RBHT64_Count(RBHT64_Object objHT);
		
		// 遍历表元素
		XXAPI void RBHT64_Walk(RBHT64_Object objHT, HT64_EachProc procEach, void* pArg);
		
	#endif
	
	
	
	
	
	/*
		CSQUE - Circular Sequence Queue 循环顺序队列
	*/
	
	#ifdef MMU_USE_CSQUE
		
	#endif
	
	
	
	
	
	/*
		EEQUE - Elastic Expansion Queue 弹性扩展队列 [使用 MM256 管理内存]
	*/
	
	#ifdef MMU_USE_EEQUE
		
	#endif
	
	
	
	
	
	/*
		PRQUE - Priority Queue 优先级队列 [使用红黑树实现]
	*/
	
	#ifdef MMU_USE_PRQUE
		
	#endif
	
	
	
	
	
	/*
		DGRAPH - Directed Graph 单向图结构
	*/
	
	#ifdef MMU_USE_DGRAPH
		
	#endif
	
	
	
	
	
	/*
		UGRAPH - Undirected Graph 双向图结构
	*/
	
	#ifdef MMU_USE_UGRAPH
		
	#endif
	
	
	
	
	
	/*
		DOM - Document Object Model 文档对象模型
	*/
	
	#ifdef MMU_USE_DOM
		
	#endif
	
	
	
	
	
	/* -> #ifndef XXRTL_MemoryManagementUnit */
#endif


