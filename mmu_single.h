


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
	
	
	
	
	// 初始化 MMU 库
	XXAPI int MMU_Init()
	{
		#ifdef MMU_USE_RPMALLOC
			rpmalloc_initialize(0);
		#endif
		return -1;
	}

	// 卸载 MMU 库
	XXAPI void MMU_Unit()
	{
		#ifdef MMU_USE_RPMALLOC
			rpmalloc_finalize();
		#endif
	}

	// 线程初始化 MMU 库
	XXAPI int MMU_Thread_Init()
	{
		#ifdef MMU_USE_RPMALLOC
			rpmalloc_thread_initialize();
		#endif
		return -1;
	}

	// 线程卸载 MMU 库
	XXAPI void MMU_Thread_Unit()
	{
		#ifdef MMU_USE_RPMALLOC
			rpmalloc_thread_finalize();
		#endif
	}





	/*
		rpmalloc [内存分配器]
			可选的内存分配器
	*/

	#ifdef MMU_USE_RPMALLOC
		
		/* rpmalloc.c  -  Memory allocator  -  Public Domain  -  2016-2020 Mattias
		 * Jansson
		 *
		 * This library provides a cross-platform lock free thread caching malloc
		 * implementation in C11. The latest source code is always available at
		 *
		 * https://github.com/mjansson/rpmalloc
		 *
		 * This library is put in the public domain; you can redistribute it and/or
		 * modify it without any restrictions.
		 *
		 */
		
		#if defined(__clang__)
		#pragma clang diagnostic ignored "-Wunused-macros"
		#pragma clang diagnostic ignored "-Wunused-function"
		#if __has_warning("-Wreserved-identifier")
		#pragma clang diagnostic ignored "-Wreserved-identifier"
		#endif
		#if __has_warning("-Wstatic-in-inline")
		#pragma clang diagnostic ignored "-Wstatic-in-inline"
		#endif
		#if __has_warning("-Wunsafe-buffer-usage")
		#pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
		#endif
		#elif defined(__GNUC__)
		#pragma GCC diagnostic ignored "-Wunused-macros"
		#pragma GCC diagnostic ignored "-Wunused-function"
		#endif
		
		#if defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
		#define PLATFORM_WINDOWS 1
		#define PLATFORM_POSIX 0
		#else
		#define PLATFORM_WINDOWS 0
		#define PLATFORM_POSIX 1
		#endif
		
		#if defined(_MSC_VER)
		#define NOINLINE __declspec(noinline)
		#else
		#define NOINLINE __attribute__((noinline))
		#endif
		
		#if PLATFORM_WINDOWS
		#include <windows.h>
		#include <fibersapi.h>
		static DWORD fls_key;
		#endif
		#if PLATFORM_POSIX
		#include <sys/mman.h>
		#include <sched.h>
		#include <unistd.h>
		#include <pthread.h>
		static pthread_key_t pthread_key;
		#ifdef __FreeBSD__
		#include <sys/sysctl.h>
		#define MAP_HUGETLB MAP_ALIGNED_SUPER
		#ifndef PROT_MAX
		#define PROT_MAX(f) 0
		#endif
		#else
		#define PROT_MAX(f) 0
		#endif
		#ifdef __sun
		extern int
		madvise(caddr_t, size_t, int);
		#endif
		#ifndef MAP_UNINITIALIZED
		#define MAP_UNINITIALIZED 0
		#endif
		#endif
		
		#if defined(__linux__) || defined(__ANDROID__)
		#include <sys/prctl.h>
		#if !defined(PR_SET_VMA)
		#define PR_SET_VMA 0x53564d41
		#define PR_SET_VMA_ANON_NAME 0
		#endif
		#endif
		#if defined(__APPLE__)
		#include <TargetConditionals.h>
		#if !TARGET_OS_IPHONE && !TARGET_OS_SIMULATOR
		#include <mach/mach_vm.h>
		#include <mach/vm_statistics.h>
		#endif
		#include <pthread.h>
		#endif
		#if defined(__HAIKU__) || defined(__TINYC__)
		#include <pthread.h>
		#endif
		
		#include <limits.h>
		#if (INTPTR_MAX > INT32_MAX)
		#define ARCH_64BIT 1
		#define ARCH_32BIT 0
		#else
		#define ARCH_64BIT 0
		#define ARCH_32BIT 1
		#endif
		
		#if !defined(__has_builtin)
		#define __has_builtin(b) 0
		#endif
		
		#define pointer_offset(ptr, ofs) (void*)((char*)(ptr) + (ptrdiff_t)(ofs))
		#define pointer_diff(first, second) (ptrdiff_t)((const char*)(first) - (const char*)(second))
		
		////////////
		///
		/// Build time configurable limits
		///
		//////
		
		#ifndef ENABLE_VALIDATE_ARGS
		//! Enable validation of args to public entry points
		#define ENABLE_VALIDATE_ARGS 0
		#endif
		#ifndef ENABLE_ASSERTS
		//! Enable asserts
		#define ENABLE_ASSERTS 0
		#endif
		#ifndef ENABLE_UNMAP
		//! Enable unmapping memory pages
		#define ENABLE_UNMAP 1
		#endif
		#ifndef ENABLE_DECOMMIT
		//! Enable decommitting memory pages
		#define ENABLE_DECOMMIT 1
		#endif
		#ifndef ENABLE_DYNAMIC_LINK
		//! Enable building as dynamic library
		#define ENABLE_DYNAMIC_LINK 0
		#endif
		#ifndef ENABLE_OVERRIDE
		//! Enable standard library malloc/free/new/delete overrides
		#define ENABLE_OVERRIDE 1
		#endif
		#ifndef ENABLE_STATISTICS
		//! Enable statistics
		#define ENABLE_STATISTICS 0
		#endif
		
		////////////
		///
		/// Built in size configurations
		///
		//////
		
		#define PAGE_HEADER_SIZE 128
		#define SPAN_HEADER_SIZE PAGE_HEADER_SIZE
		
		#define SMALL_GRANULARITY 16
		
		#define SMALL_BLOCK_SIZE_LIMIT (4 * 1024)
		#define MEDIUM_BLOCK_SIZE_LIMIT (256 * 1024)
		#define LARGE_BLOCK_SIZE_LIMIT (8 * 1024 * 1024)
		
		#define SMALL_SIZE_CLASS_COUNT 73
		#define MEDIUM_SIZE_CLASS_COUNT 24
		#define LARGE_SIZE_CLASS_COUNT 20
		#define SIZE_CLASS_COUNT (SMALL_SIZE_CLASS_COUNT + MEDIUM_SIZE_CLASS_COUNT + LARGE_SIZE_CLASS_COUNT)
		
		#define SMALL_PAGE_SIZE_SHIFT 16
		#define SMALL_PAGE_SIZE (1 << SMALL_PAGE_SIZE_SHIFT)
		#define SMALL_PAGE_MASK (~((uintptr_t)SMALL_PAGE_SIZE - 1))
		#define MEDIUM_PAGE_SIZE_SHIFT 22
		#define MEDIUM_PAGE_SIZE (1 << MEDIUM_PAGE_SIZE_SHIFT)
		#define MEDIUM_PAGE_MASK (~((uintptr_t)MEDIUM_PAGE_SIZE - 1))
		#define LARGE_PAGE_SIZE_SHIFT 26
		#define LARGE_PAGE_SIZE (1 << LARGE_PAGE_SIZE_SHIFT)
		#define LARGE_PAGE_MASK (~((uintptr_t)LARGE_PAGE_SIZE - 1))
		
		#define SPAN_SIZE (256 * 1024 * 1024)
		#define SPAN_MASK (~((uintptr_t)(SPAN_SIZE - 1)))
		
		////////////
		///
		/// Utility macros
		///
		//////
		
		#if ENABLE_ASSERTS
		#undef NDEBUG
		#if defined(_MSC_VER) && !defined(_DEBUG)
		#define _DEBUG
		#endif
		#include <assert.h>
		#define RPMALLOC_TOSTRING_M(x) #x
		#define RPMALLOC_TOSTRING(x) RPMALLOC_TOSTRING_M(x)
		#define rpmalloc_assert(truth, message) \
			do {                                \
				if (!(truth)) {                 \
					assert((truth) && message); \
				}                               \
			} while (0)
		#else
		#define rpmalloc_assert(truth, message) \
			do {                                \
			} while (0)
		#endif
		
		#if __has_builtin(__builtin_assume)
		#define rpmalloc_assume(cond) __builtin_assume(cond)
		#elif defined(__GNUC__)
		#define rpmalloc_assume(cond)           \
			do {                                \
				if (!__builtin_expect(cond, 0)) \
					__builtin_unreachable();    \
			} while (0)
		#elif defined(_MSC_VER)
		#define rpmalloc_assume(cond) __assume(cond)
		#else
		#define rpmalloc_assume(cond) 0
		#endif
		
		////////////
		///
		/// Statistics
		///
		//////
		
		#if ENABLE_STATISTICS
		
		typedef struct rpmalloc_statistics_t {
			atomic_size_t page_mapped;
			atomic_size_t page_mapped_peak;
			atomic_size_t page_commit;
			atomic_size_t page_decommit;
			atomic_size_t page_active;
			atomic_size_t page_active_peak;
			atomic_size_t heap_count;
		} rpmalloc_statistics_t;
		
		static rpmalloc_statistics_t global_statistics;
		
		#else
		
		#endif
		
		////////////
		///
		/// Low level abstractions
		///
		//////
		
		static inline size_t
		rpmalloc_clz(uintptr_t x) {
		#if ARCH_64BIT
		#if defined(_MSC_VER) && !defined(__clang__)
			return (size_t)_lzcnt_u64(x);
		#else
			return (size_t)__builtin_clzll(x);
		#endif
		#else
		#if defined(_MSC_VER) && !defined(__clang__)
			return (size_t)_lzcnt_u32(x);
		#else
			return (size_t)__builtin_clzl(x);
		#endif
		#endif
		}
		
		static inline void
		wait_spin(void) {
		#if defined(_MSC_VER)
		#if defined(_M_ARM64)
			__yield();
		#else
			_mm_pause();
		#endif
		#elif defined(__x86_64__) || defined(__i386__)
			__asm__ volatile("pause" ::: "memory");
		#elif defined(__aarch64__) || (defined(__arm__) && __ARM_ARCH >= 7)
			__asm__ volatile("yield" ::: "memory");
		#elif defined(__powerpc__) || defined(__powerpc64__)
			// No idea if ever been compiled in such archs but ... as precaution
			__asm__ volatile("or 27,27,27");
		#elif defined(__sparc__)
			__asm__ volatile("rd %ccr, %g0 \n\trd %ccr, %g0 \n\trd %ccr, %g0");
		#else
			struct timespec ts = {0};
			nanosleep(&ts, 0);
		#endif
		}
		
		#if defined(__GNUC__) || defined(__clang__)
		
		#define EXPECTED(x) __builtin_expect((x), 1)
		#define UNEXPECTED(x) __builtin_expect((x), 0)
		
		#else
		
		#define EXPECTED(x) x
		#define UNEXPECTED(x) x
		
		#endif
		#if defined(__GNUC__) || defined(__clang__)
		
		#if __has_builtin(__builtin_memcpy_inline)
		#define memcpy_const(x, y, s) __builtin_memcpy_inline(x, y, s)
		#else
		#define memcpy_const(x, y, s)                                                                                   \
			do {                                                                                                        \
				_Static_assert(__builtin_choose_expr(__builtin_constant_p(s), 1, 0), "len must be a constant integer"); \
				memcpy(x, y, s);                                                                                        \
			} while (0)
		#endif

		#if __has_builtin(__builtin_memset_inline)
		#define memset_const(x, y, s) __builtin_memset_inline(x, y, s)
		#else
		#define memset_const(x, y, s)                                                                                   \
			do {                                                                                                        \
				_Static_assert(__builtin_choose_expr(__builtin_constant_p(s), 1, 0), "len must be a constant integer"); \
				memset(x, y, s);                                                                                        \
			} while (0)
		#endif
		#else
		#define memcpy_const(x, y, s) memcpy(x, y, s)
		#define memset_const(x, y, s) memset(x, y, s)
		#endif
		
		////////////
		///
		/// Data types
		///
		//////
		
		//! A memory heap, per thread
		typedef struct heap_t heap_t;
		//! Span of memory pages
		typedef struct span_t span_t;
		//! Memory page
		typedef struct page_t page_t;
		//! Memory block
		typedef struct block_t block_t;
		//! Size class for a memory block
		typedef struct size_class_t size_class_t;
		
		//! Memory page type
		typedef enum page_type_t {
			PAGE_SMALL,   // 64KiB
			PAGE_MEDIUM,  // 4MiB
			PAGE_LARGE,   // 64MiB
			PAGE_HUGE
		} page_type_t;
		
		//! Block size class
		struct size_class_t {
			//! Size of blocks in this class
			uint32_t block_size;
			//! Number of blocks in each chunk
			uint32_t block_count;
		};
		
		//! A memory block
		struct block_t {
			//! Next block in list
			block_t* next;
		};
		
		//! A page contains blocks of a given size
		struct page_t {
			//! Size class of blocks
			uint32_t size_class;
			//! Block size
			uint32_t block_size;
			//! Block count
			uint32_t block_count;
			//! Block initialized count
			uint32_t block_initialized;
			//! Block used count
			uint32_t block_used;
			//! Page type
			page_type_t page_type;
			//! Flag set if part of heap full list
			uint32_t is_full : 1;
			//! Flag set if part of heap free list
			uint32_t is_free : 1;
			//! Flag set if blocks are zero initialied
			uint32_t is_zero : 1;
			//! Flag set if memory pages have been decommitted
			uint32_t is_decommitted : 1;
			//! Flag set if containing aligned blocks
			uint32_t has_aligned_block : 1;
			//! Fast combination flag for either huge, fully allocated or has aligned blocks
			uint32_t generic_free : 1;
			//! Local free list count
			uint32_t local_free_count;
			//! Local free list
			block_t* local_free;
			//! Owning heap
			heap_t* heap;
			//! Next page in list
			page_t* next;
			//! Previous page in list
			page_t* prev;
			//! Multithreaded free list, block index is in low 32 bit, list count is high 32 bit
			atomic_ullong thread_free;
		};
		
		//! A span contains pages of a given type
		struct span_t {
			//! Page header
			page_t page;
			//! Owning heap
			heap_t* heap;
			//! Page address mask
			uintptr_t page_address_mask;
			//! Number of pages initialized
			uint32_t page_initialized;
			//! Number of pages in use
			uint32_t page_count;
			//! Number of bytes per page
			uint32_t page_size;
			//! Page type
			page_type_t page_type;
			//! Offset to start of mapped memory region
			uint32_t offset;
			//! Mapped size
			uint64_t mapped_size;
			//! Next span in list
			span_t* next;
		};
		
		// Control structure for a heap, either a thread heap or a first class heap if enabled
		struct heap_t {
			//! Owning thread ID
			uintptr_t owner_thread;
			//! Heap local free list for small size classes
			block_t* local_free[SIZE_CLASS_COUNT];
			//! Available non-full pages for each size class
			page_t* page_available[SIZE_CLASS_COUNT];
			//! Free pages for each page type
			page_t* page_free[3];
			//! Free but still committed page count for each page tyoe
			uint32_t page_free_commit_count[3];
			//! Multithreaded free list
			atomic_uintptr_t thread_free[3];
			//! Available partially initialized spans for each page type
			span_t* span_partial[3];
			//! Spans in full use for each page type
			span_t* span_used[4];
			//! Next heap in queue
			heap_t* next;
			//! Previous heap in queue
			heap_t* prev;
			//! Heap ID
			uint32_t id;
			//! Finalization state flag
			uint32_t finalize;
			//! Memory map region offset
			uint32_t offset;
			//! Memory map size
			size_t mapped_size;
		};
		
		_Static_assert(sizeof(page_t) <= PAGE_HEADER_SIZE, "Invalid page header size");
		_Static_assert(sizeof(span_t) <= SPAN_HEADER_SIZE, "Invalid span header size");
		_Static_assert(sizeof(heap_t) <= 4096, "Invalid heap size");
		
		////////////
		///
		/// Global data
		///
		//////
		
		//! Fallback heap
		static RPMALLOC_CACHE_ALIGNED heap_t global_heap_fallback;
		//! Default heap
		static heap_t* global_heap_default = &global_heap_fallback;
		//! Available heaps
		static heap_t* global_heap_queue;
		//! In use heaps
		static heap_t* global_heap_used;
		//! Lock for heap queue
		static atomic_uintptr_t global_heap_lock;
		//! Heap ID counter
		static atomic_uint global_heap_id = 1;
		//! Initialized flag
		static int global_rpmalloc_initialized;
		//! Memory interface
		static rpmalloc_interface_t* global_memory_interface;
		//! Default memory interface
		static rpmalloc_interface_t global_memory_interface_default;
		//! Current configuration
		static rpmalloc_config_t global_config = {0};
		//! Main thread ID
		static uintptr_t global_main_thread_id;
		
		//! Size classes
		#define SCLASS(n) \
			{ (n * SMALL_GRANULARITY), (SMALL_PAGE_SIZE - PAGE_HEADER_SIZE) / (n * SMALL_GRANULARITY) }
		#define MCLASS(n) \
			{ (n * SMALL_GRANULARITY), (MEDIUM_PAGE_SIZE - PAGE_HEADER_SIZE) / (n * SMALL_GRANULARITY) }
		#define LCLASS(n) \
			{ (n * SMALL_GRANULARITY), (LARGE_PAGE_SIZE - PAGE_HEADER_SIZE) / (n * SMALL_GRANULARITY) }
		static const size_class_t global_size_class[SIZE_CLASS_COUNT] = {
			SCLASS(1),      SCLASS(1),      SCLASS(2),      SCLASS(3),      SCLASS(4),      SCLASS(5),      SCLASS(6),
			SCLASS(7),      SCLASS(8),      SCLASS(9),      SCLASS(10),     SCLASS(11),     SCLASS(12),     SCLASS(13),
			SCLASS(14),     SCLASS(15),     SCLASS(16),     SCLASS(17),     SCLASS(18),     SCLASS(19),     SCLASS(20),
			SCLASS(21),     SCLASS(22),     SCLASS(23),     SCLASS(24),     SCLASS(25),     SCLASS(26),     SCLASS(27),
			SCLASS(28),     SCLASS(29),     SCLASS(30),     SCLASS(31),     SCLASS(32),     SCLASS(33),     SCLASS(34),
			SCLASS(35),     SCLASS(36),     SCLASS(37),     SCLASS(38),     SCLASS(39),     SCLASS(40),     SCLASS(41),
			SCLASS(42),     SCLASS(43),     SCLASS(44),     SCLASS(45),     SCLASS(46),     SCLASS(47),     SCLASS(48),
			SCLASS(49),     SCLASS(50),     SCLASS(51),     SCLASS(52),     SCLASS(53),     SCLASS(54),     SCLASS(55),
			SCLASS(56),     SCLASS(57),     SCLASS(58),     SCLASS(59),     SCLASS(60),     SCLASS(61),     SCLASS(62),
			SCLASS(63),     SCLASS(64),     SCLASS(80),     SCLASS(96),     SCLASS(112),    SCLASS(128),    SCLASS(160),
			SCLASS(192),    SCLASS(224),    SCLASS(256),    MCLASS(320),    MCLASS(384),    MCLASS(448),    MCLASS(512),
			MCLASS(640),    MCLASS(768),    MCLASS(896),    MCLASS(1024),   MCLASS(1280),   MCLASS(1536),   MCLASS(1792),
			MCLASS(2048),   MCLASS(2560),   MCLASS(3072),   MCLASS(3584),   MCLASS(4096),   MCLASS(5120),   MCLASS(6144),
			MCLASS(7168),   MCLASS(8192),   MCLASS(10240),  MCLASS(12288),  MCLASS(14336),  MCLASS(16384),  LCLASS(20480),
			LCLASS(24576),  LCLASS(28672),  LCLASS(32768),  LCLASS(40960),  LCLASS(49152),  LCLASS(57344),  LCLASS(65536),
			LCLASS(81920),  LCLASS(98304),  LCLASS(114688), LCLASS(131072), LCLASS(163840), LCLASS(196608), LCLASS(229376),
			LCLASS(262144), LCLASS(327680), LCLASS(393216), LCLASS(458752), LCLASS(524288)};
		
		//! Threshold number of pages for when free pages are decommitted
		static uint32_t global_page_free_overflow[4] = {16, 8, 2, 0};
		
		//! Number of pages to retain when free page threshold overflows
		static uint32_t global_page_free_retain[4] = {4, 2, 1, 0};
		
		//! OS huge page support
		static int os_huge_pages;
		//! OS memory map granularity
		static size_t os_map_granularity;
		//! OS memory page size
		static size_t os_page_size;
		
		////////////
		///
		/// Thread local heap and ID
		///
		//////
		
		//! Current thread heap
		#if defined(_MSC_VER) && !defined(__clang__)
		#define TLS_MODEL
		#define _Thread_local __declspec(thread)
		#else
		// #define TLS_MODEL __attribute__((tls_model("initial-exec")))
		#define TLS_MODEL
		#endif
		static _Thread_local heap_t* global_thread_heap TLS_MODEL = &global_heap_fallback;
		
		static heap_t*
		heap_allocate(int first_class);
		
		static void
		heap_page_free_decommit(heap_t* heap, uint32_t page_type, uint32_t page_retain_count);
		
		//! Fast thread ID
		static inline uintptr_t
		get_thread_id(void) {
		#if defined(_WIN32)
			return (uintptr_t)((void*)NtCurrentTeb());
		#else
			void* thp = __builtin_thread_pointer();
			return (uintptr_t)thp;
		#endif
			/*
			#elif (defined(__GNUC__) || defined(__clang__)) && !defined(__CYGWIN__)
				uintptr_t tid;
			#if defined(__i386__)
				__asm__("movl %%gs:0, %0" : "=r"(tid) : :);
			#elif defined(__x86_64__)
			#if defined(__MACH__)
				__asm__("movq %%gs:0, %0" : "=r"(tid) : :);
			#else
				__asm__("movq %%fs:0, %0" : "=r"(tid) : :);
			#endif
			#elif defined(__arm__)
				__asm__ volatile("mrc p15, 0, %0, c13, c0, 3" : "=r"(tid));
			#elif defined(__aarch64__)
			#if defined(__MACH__)
				// tpidr_el0 likely unused, always return 0 on iOS
				__asm__ volatile("mrs %0, tpidrro_el0" : "=r"(tid));
			#else
				__asm__ volatile("mrs %0, tpidr_el0" : "=r"(tid));
			#endif
			#else
			#error This platform needs implementation of get_thread_id()
			#endif
				return tid;
			#else
			#error This platform needs implementation of get_thread_id()
			#endif
			*/
		}
		
		//! Set the current thread heap
		static void
		set_thread_heap(heap_t* heap) {
			global_thread_heap = heap;
			if (heap && (heap->id != 0)) {
				rpmalloc_assert(heap->id != 0, "Default heap being used");
				heap->owner_thread = get_thread_id();
			}
		#if PLATFORM_WINDOWS
			FlsSetValue(fls_key, heap);
		#else
			pthread_setspecific(pthread_key, heap);
		#endif
		}
		
		static heap_t*
		get_thread_heap_allocate(void) {
			heap_t* heap = heap_allocate(0);
			set_thread_heap(heap);
			return heap;
		}
		
		//! Get the current thread heap
		static inline heap_t*
		get_thread_heap(void) {
			return global_thread_heap;
		}
		
		//! Get the size class from given size in bytes for tiny blocks (below 16 times the minimum granularity)
		static inline uint32_t
		get_size_class_tiny(size_t size) {
			return (((uint32_t)size + (SMALL_GRANULARITY - 1)) / SMALL_GRANULARITY);
		}
		
		//! Get the size class from given size in bytes
		static inline uint32_t
		get_size_class(size_t size) {
			uintptr_t minblock_count = (size + (SMALL_GRANULARITY - 1)) / SMALL_GRANULARITY;
			// For sizes up to 64 times the minimum granularity (i.e 1024 bytes) the size class is equal to number of such
			// blocks
			if (size <= (SMALL_GRANULARITY * 64)) {
				rpmalloc_assert(global_size_class[minblock_count].block_size >= size, "Size class misconfiguration");
				return (uint32_t)(minblock_count ? minblock_count : 1);
			}
			--minblock_count;
			// Calculate position of most significant bit, since minblock_count now guaranteed to be > 64 this position is
			// guaranteed to be >= 6
		#if ARCH_64BIT
			const uint32_t most_significant_bit = (uint32_t)(63 - (int)rpmalloc_clz(minblock_count));
		#else
			const uint32_t most_significant_bit = (uint32_t)(31 - (int)rpmalloc_clz(minblock_count));
		#endif
			// Class sizes are of the bit format [..]000xxx000[..] where we already have the position of the most significant
			// bit, now calculate the subclass from the remaining two bits
			const uint32_t subclass_bits = (minblock_count >> (most_significant_bit - 2)) & 0x03;
			const uint32_t class_idx = (uint32_t)((most_significant_bit << 2) + subclass_bits) + 41;
			rpmalloc_assert((class_idx >= SIZE_CLASS_COUNT) || (global_size_class[class_idx].block_size >= size),
							"Size class misconfiguration");
			rpmalloc_assert((class_idx >= SIZE_CLASS_COUNT) || (global_size_class[class_idx - 1].block_size < size),
							"Size class misconfiguration");
			return class_idx;
		}
		
		static inline page_type_t
		get_page_type(uint32_t size_class) {
			if (size_class < SMALL_SIZE_CLASS_COUNT)
				return PAGE_SMALL;
			else if (size_class < (SMALL_SIZE_CLASS_COUNT + MEDIUM_SIZE_CLASS_COUNT))
				return PAGE_MEDIUM;
			else if (size_class < SIZE_CLASS_COUNT)
				return PAGE_LARGE;
			return PAGE_HUGE;
		}
		
		static inline size_t
		get_page_aligned_size(size_t size) {
			size_t unalign = size % global_config.page_size;
			if (unalign)
				size += global_config.page_size - unalign;
			return size;
		}
		
		////////////
		///
		/// OS entry points
		///
		//////
		
		static void
		os_set_page_name(void* address, size_t size) {
		#if defined(__linux__) || defined(__ANDROID__)
			const char* name = os_huge_pages ? global_config.huge_page_name : global_config.page_name;
			if ((address == MAP_FAILED) || !name)
				return;
			// If the kernel does not support CONFIG_ANON_VMA_NAME or if the call fails
			// (e.g. invalid name) it is a no-op basically.
			(void)prctl(PR_SET_VMA, PR_SET_VMA_ANON_NAME, (uintptr_t)address, size, (uintptr_t)name);
		#else
			(void)sizeof(size);
			(void)sizeof(address);
		#endif
		}
		
		static void*
		os_mmap(size_t size, size_t alignment, size_t* offset, size_t* mapped_size) {
			size_t map_size = size + alignment;
		#if PLATFORM_WINDOWS
			// Ok to MEM_COMMIT - according to MSDN, "actual physical pages are not allocated unless/until the virtual addresses
			// are actually accessed". But if we enable decommit it's better to not immediately commit and instead commit per
			// page to avoid saturating the OS commit limit
		#if ENABLE_DECOMMIT
			DWORD do_commit = 0;
		#else
			DWORD do_commit = MEM_COMMIT;
		#endif
			void* ptr =
				VirtualAlloc(0, map_size, (os_huge_pages ? MEM_LARGE_PAGES : 0) | MEM_RESERVE | do_commit, PAGE_READWRITE);
		#else
			int flags = MAP_PRIVATE | MAP_ANONYMOUS | MAP_UNINITIALIZED;
		#if defined(__APPLE__) && !TARGET_OS_IPHONE && !TARGET_OS_SIMULATOR
			int fd = (int)VM_MAKE_TAG(240U);
			if (os_huge_pages)
				fd |= VM_FLAGS_SUPERPAGE_SIZE_2MB;
			void* ptr = mmap(0, map_size, PROT_READ | PROT_WRITE, flags, fd, 0);
		#elif defined(MAP_HUGETLB)
			void* ptr = mmap(0, map_size, PROT_READ | PROT_WRITE | PROT_MAX(PROT_READ | PROT_WRITE),
							 (os_huge_pages ? MAP_HUGETLB : 0) | flags, -1, 0);
		#if defined(MADV_HUGEPAGE)
			// In some configurations, huge pages allocations might fail thus
			// we fallback to normal allocations and promote the region as transparent huge page
			if ((ptr == MAP_FAILED || !ptr) && os_huge_pages) {
				ptr = mmap(0, map_size, PROT_READ | PROT_WRITE, flags, -1, 0);
				if (ptr && ptr != MAP_FAILED) {
					int prm = madvise(ptr, size, MADV_HUGEPAGE);
					(void)prm;
					rpmalloc_assert((prm == 0), "Failed to promote the page to transparent huge page");
				}
			}
		#endif
			os_set_page_name(ptr, map_size);
		#elif defined(MAP_ALIGNED)
			const size_t align = (sizeof(size_t) * 8) - (size_t)(__builtin_clzl(size - 1));
			void* ptr = mmap(0, map_size, PROT_READ | PROT_WRITE, (os_huge_pages ? MAP_ALIGNED(align) : 0) | flags, -1, 0);
		#elif defined(MAP_ALIGN)
			caddr_t base = (os_huge_pages ? (caddr_t)(4 << 20) : 0);
			void* ptr = mmap(base, map_size, PROT_READ | PROT_WRITE, (os_huge_pages ? MAP_ALIGN : 0) | flags, -1, 0);
		#else
			void* ptr = mmap(0, map_size, PROT_READ | PROT_WRITE, flags, -1, 0);
		#endif
			if (ptr == MAP_FAILED)
				ptr = 0;
		#endif
			if (!ptr) {
				if (global_memory_interface->map_fail_callback) {
					if (global_memory_interface->map_fail_callback(map_size))
						return os_mmap(size, alignment, offset, mapped_size);
				} else {
					rpmalloc_assert(ptr != 0, "Failed to map more virtual memory");
				}
				return 0;
			}
			if (alignment) {
				size_t padding = ((uintptr_t)ptr & (uintptr_t)(alignment - 1));
				if (padding)
					padding = alignment - padding;
				rpmalloc_assert(padding <= alignment, "Internal failure in padding");
				rpmalloc_assert(!(padding % 8), "Internal failure in padding");
				ptr = pointer_offset(ptr, padding);
				*offset = padding;
			}
			*mapped_size = map_size;
		#if ENABLE_STATISTICS
			size_t page_count = map_size / global_config.page_size;
			size_t page_mapped_current =
				atomic_fetch_add_explicit(&global_statistics.page_mapped, page_count, memory_order_relaxed) + page_count;
			size_t page_mapped_peak = atomic_load_explicit(&global_statistics.page_mapped_peak, memory_order_relaxed);
			while (page_mapped_current > page_mapped_peak) {
				if (atomic_compare_exchange_weak_explicit(&global_statistics.page_mapped_peak, &page_mapped_peak,
														  page_mapped_current, memory_order_relaxed, memory_order_relaxed))
					break;
			}
		#if ENABLE_DECOMMIT
			size_t page_active_current =
				atomic_fetch_add_explicit(&global_statistics.page_active, page_count, memory_order_relaxed) + page_count;
			size_t page_active_peak = atomic_load_explicit(&global_statistics.page_active_peak, memory_order_relaxed);
			while (page_active_current > page_active_peak) {
				if (atomic_compare_exchange_weak_explicit(&global_statistics.page_active_peak, &page_active_peak,
														  page_active_current, memory_order_relaxed, memory_order_relaxed))
					break;
			}
		#endif
		#endif
			return ptr;
		}
		
		static void
		os_mcommit(void* address, size_t size) {
		#if ENABLE_DECOMMIT
			if (global_config.disable_decommit)
				return;
		#if PLATFORM_WINDOWS
			if (!VirtualAlloc(address, size, MEM_COMMIT, PAGE_READWRITE)) {
				rpmalloc_assert(0, "Failed to commit virtual memory block");
			}
		#else
				/*
				if (mprotect(address, size, PROT_READ | PROT_WRITE)) {
					rpmalloc_assert(0, "Failed to commit virtual memory block");
				}
				*/
		#endif
		#if ENABLE_STATISTICS
			size_t page_count = size / global_config.page_size;
			atomic_fetch_add_explicit(&global_statistics.page_commit, page_count, memory_order_relaxed);
			size_t page_active_current =
				atomic_fetch_add_explicit(&global_statistics.page_active, page_count, memory_order_relaxed) + page_count;
			size_t page_active_peak = atomic_load_explicit(&global_statistics.page_active_peak, memory_order_relaxed);
			while (page_active_current > page_active_peak) {
				if (atomic_compare_exchange_weak_explicit(&global_statistics.page_active_peak, &page_active_peak,
														  page_active_current, memory_order_relaxed, memory_order_relaxed))
					break;
			}
		#endif
		#endif
			(void)sizeof(address);
			(void)sizeof(size);
		}
		
		static void
		os_mdecommit(void* address, size_t size) {
		#if ENABLE_DECOMMIT
			if (global_config.disable_decommit)
				return;
		#if PLATFORM_WINDOWS
			if (!VirtualFree(address, size, MEM_DECOMMIT)) {
				rpmalloc_assert(0, "Failed to decommit virtual memory block");
			}
		#else
				/*
				if (mprotect(address, size, PROT_NONE)) {
					rpmalloc_assert(0, "Failed to decommit virtual memory block");
				}
				*/
		#if defined(MADV_DONTNEED)
			if (madvise(address, size, MADV_DONTNEED)) {
		#elif defined(MADV_FREE_REUSABLE)
			int ret;
			while ((ret = madvise(address, size, MADV_FREE_REUSABLE)) == -1 && (errno == EAGAIN))
				errno = 0;
			if ((ret == -1) && (errno != 0)) {
		#elif defined(MADV_PAGEOUT)
			if (madvise(address, size, MADV_PAGEOUT)) {
		#elif defined(MADV_FREE)
			if (madvise(address, size, MADV_FREE)) {
		#else
			if (posix_madvise(address, size, POSIX_MADV_DONTNEED)) {
		#endif
				rpmalloc_assert(0, "Failed to decommit virtual memory block");
			}
		#endif
		#if ENABLE_STATISTICS
			size_t page_count = size / global_config.page_size;
			atomic_fetch_add_explicit(&global_statistics.page_decommit, page_count, memory_order_relaxed);
			size_t page_active_current =
				atomic_fetch_sub_explicit(&global_statistics.page_active, page_count, memory_order_relaxed);
			rpmalloc_assert(page_active_current >= page_count, "Decommit counter out of sync");
			(void)sizeof(page_active_current);
		#endif
		#else
			(void)sizeof(address);
			(void)sizeof(size);
		#endif
		}
		
		static void
		os_munmap(void* address, size_t offset, size_t mapped_size) {
			(void)sizeof(mapped_size);
			address = pointer_offset(address, -(int32_t)offset);
		#if ENABLE_UNMAP
		#if PLATFORM_WINDOWS
			if (!VirtualFree(address, 0, MEM_RELEASE)) {
				rpmalloc_assert(0, "Failed to unmap virtual memory block");
			}
		#else
			if (munmap(address, mapped_size))
				rpmalloc_assert(0, "Failed to unmap virtual memory block");
		#endif
		#if ENABLE_STATISTICS
			size_t page_count = mapped_size / global_config.page_size;
			atomic_fetch_sub_explicit(&global_statistics.page_mapped, page_count, memory_order_relaxed);
			atomic_fetch_sub_explicit(&global_statistics.page_active, page_count, memory_order_relaxed);
		#endif
		#endif
		}
		
		////////////
		///
		/// Page interface
		///
		//////
		
		static inline span_t*
		page_get_span(page_t* page) {
			return (span_t*)((uintptr_t)page & SPAN_MASK);
		}
		
		static inline size_t
		page_get_size(page_t* page) {
			if (page->page_type == PAGE_SMALL)
				return SMALL_PAGE_SIZE;
			else if (page->page_type == PAGE_MEDIUM)
				return MEDIUM_PAGE_SIZE;
			else if (page->page_type == PAGE_LARGE)
				return LARGE_PAGE_SIZE;
			else
				return page_get_span(page)->page_size;
		}
		
		static inline int
		page_is_thread_heap(page_t* page) {
		#if RPMALLOC_FIRST_CLASS_HEAPS
			return (!page->heap->owner_thread || (page->heap->owner_thread == get_thread_id()));
		#else
			return (page->heap->owner_thread == get_thread_id());
		#endif
		}
		
		static inline block_t*
		page_block_start(page_t* page) {
			return pointer_offset(page, PAGE_HEADER_SIZE);
		}
		
		static inline block_t*
		page_block(page_t* page, uint32_t block_index) {
			return pointer_offset(page, PAGE_HEADER_SIZE + (page->block_size * block_index));
		}
		
		static inline uint32_t
		page_block_index(page_t* page, block_t* block) {
			block_t* block_first = page_block_start(page);
			return (uint32_t)pointer_diff(block, block_first) / page->block_size;
		}
		
		static inline uint32_t
		page_block_from_thread_free_list(page_t* page, uint64_t token, block_t** block) {
			uint32_t block_index = (uint32_t)(token & 0xFFFFFFFFULL);
			uint32_t list_count = (uint32_t)((token >> 32ULL) & 0xFFFFFFFFULL);
			*block = list_count ? page_block(page, block_index) : 0;
			return list_count;
		}
		
		static inline uint64_t
		page_block_to_thread_free_list(page_t* page, uint32_t block_index, uint32_t list_count) {
			(void)sizeof(page);
			return ((uint64_t)list_count << 32ULL) | (uint64_t)block_index;
		}
		
		static inline block_t*
		page_block_realign(page_t* page, block_t* block) {
			void* blocks_start = page_block_start(page);
			uint32_t block_offset = (uint32_t)pointer_diff(block, blocks_start);
			return pointer_offset(block, -(int32_t)(block_offset % page->block_size));
		}
		
		static block_t*
		page_get_local_free_block(page_t* page) {
			block_t* block = page->local_free;
			page->local_free = block->next;
			--page->local_free_count;
			++page->block_used;
			return block;
		}
		
		static inline void
		page_decommit_memory_pages(page_t* page) {
			if (page->is_decommitted)
				return;
			void* extra_page = pointer_offset(page, global_config.page_size);
			size_t extra_page_size = page_get_size(page) - global_config.page_size;
			global_memory_interface->memory_decommit(extra_page, extra_page_size);
			page->is_decommitted = 1;
		}
		
		static inline void
		page_commit_memory_pages(page_t* page) {
			if (!page->is_decommitted)
				return;
			void* extra_page = pointer_offset(page, global_config.page_size);
			size_t extra_page_size = page_get_size(page) - global_config.page_size;
			global_memory_interface->memory_commit(extra_page, extra_page_size);
			page->is_decommitted = 0;
		#if ENABLE_DECOMMIT
		#if !defined(__APPLE__)
			// When page is recommitted, the blocks in the second memory page and forward
			// will be zeroed out by OS - take advantage in zalloc/calloc calls and make sure
			// blocks in first page is zeroed out
			void* first_page = pointer_offset(page, PAGE_HEADER_SIZE);
			memset(first_page, 0, global_config.page_size - PAGE_HEADER_SIZE);
			page->is_zero = 1;
		#endif
		#endif
		}
		
		static void
		page_available_to_free(page_t* page) {
			rpmalloc_assert(page->is_full == 0, "Page full flag internal failure");
			rpmalloc_assert(page->is_decommitted == 0, "Page decommitted flag internal failure");
			heap_t* heap = page->heap;
			if (heap->page_available[page->size_class] == page) {
				heap->page_available[page->size_class] = page->next;
			} else {
				page->prev->next = page->next;
				if (page->next)
					page->next->prev = page->prev;
			}
			page->is_free = 1;
			page->is_zero = 0;
			page->next = heap->page_free[page->page_type];
			heap->page_free[page->page_type] = page;
			if (++heap->page_free_commit_count[page->page_type] >= global_page_free_overflow[page->page_type])
				heap_page_free_decommit(heap, page->page_type, global_page_free_retain[page->page_type]);
		}
		
		static void
		page_full_to_available(page_t* page) {
			rpmalloc_assert(page->is_full == 1, "Page full flag internal failure");
			rpmalloc_assert(page->is_decommitted == 0, "Page decommitted flag internal failure");
			heap_t* heap = page->heap;
			page->next = heap->page_available[page->size_class];
			if (page->next)
				page->next->prev = page;
			heap->page_available[page->size_class] = page;
			page->is_full = 0;
			if (page->has_aligned_block == 0)
				page->generic_free = 0;
		}
		
		static void
		page_full_to_free_on_new_heap(page_t* page, heap_t* heap) {
			rpmalloc_assert(heap->id, "Page full to free on default heap");
			rpmalloc_assert(page->is_full == 1, "Page full flag internal failure");
			rpmalloc_assert(page->is_decommitted == 0, "Page decommitted flag internal failure");
			page->is_full = 0;
			page->is_free = 1;
			page->heap = heap;
			atomic_store_explicit(&page->thread_free, 0, memory_order_relaxed);
			page->next = heap->page_free[page->page_type];
			heap->page_free[page->page_type] = page;
			if (++heap->page_free_commit_count[page->page_type] >= global_page_free_overflow[page->page_type])
				heap_page_free_decommit(heap, page->page_type, global_page_free_retain[page->page_type]);
		}
		
		static void
		page_available_to_full(page_t* page) {
			heap_t* heap = page->heap;
			if (heap->page_available[page->size_class] == page) {
				heap->page_available[page->size_class] = page->next;
			} else {
				page->prev->next = page->next;
				if (page->next)
					page->next->prev = page->prev;
			}
			page->is_full = 1;
			page->is_zero = 0;
			page->generic_free = 1;
		}
		
		static inline void
		page_put_local_free_block(page_t* page, block_t* block) {
			block->next = page->local_free;
			page->local_free = block;
			++page->local_free_count;
			if (UNEXPECTED(--page->block_used == 0)) {
				page_available_to_free(page);
			} else if (UNEXPECTED(page->is_full != 0)) {
				page_full_to_available(page);
			}
		}
		
		static NOINLINE void
		page_adopt_thread_free_block_list(page_t* page) {
			if (page->local_free)
				return;
			unsigned long long thread_free = atomic_load_explicit(&page->thread_free, memory_order_relaxed);
			if (thread_free != 0) {
				// Other threads can only replace with another valid list head, this will never change to 0 in other threads
				while (!atomic_compare_exchange_weak_explicit(&page->thread_free, &thread_free, 0, memory_order_relaxed,
															  memory_order_relaxed))
					wait_spin();
				page->local_free_count = page_block_from_thread_free_list(page, thread_free, &page->local_free);
				rpmalloc_assert(page->local_free_count <= page->block_used, "Page thread free list count internal failure");
				page->block_used -= page->local_free_count;
			}
		}
		
		static NOINLINE void
		page_put_thread_free_block(page_t* page, block_t* block) {
			atomic_thread_fence(memory_order_acquire);
			if (page->is_full) {
				// Page is full, put the block in the heap thread free list instead, otherwise
				// the heap will not pick up the free blocks until a thread local free happens
				heap_t* heap = page->heap;
				uintptr_t prev_head = atomic_load_explicit(&heap->thread_free[page->page_type], memory_order_relaxed);
				block->next = (void*)prev_head;
				while (!atomic_compare_exchange_weak_explicit(&heap->thread_free[page->page_type], &prev_head, (uintptr_t)block,
															  memory_order_relaxed, memory_order_relaxed)) {
					block->next = (void*)prev_head;
					wait_spin();
				}
			} else {
				unsigned long long prev_thread_free = atomic_load_explicit(&page->thread_free, memory_order_relaxed);
				uint32_t block_index = page_block_index(page, block);
				rpmalloc_assert(page_block(page, block_index) == block, "Block pointer is not aligned to start of block");
				uint32_t list_size = page_block_from_thread_free_list(page, prev_thread_free, &block->next) + 1;
				uint64_t thread_free = page_block_to_thread_free_list(page, block_index, list_size);
				while (!atomic_compare_exchange_weak_explicit(&page->thread_free, &prev_thread_free, thread_free,
															  memory_order_relaxed, memory_order_relaxed)) {
					list_size = page_block_from_thread_free_list(page, prev_thread_free, &block->next) + 1;
					thread_free = page_block_to_thread_free_list(page, block_index, list_size);
					wait_spin();
				}
			}
		}
		
		static void
		page_push_local_free_to_heap(page_t* page) {
			// Push the page free list as the fast track list of free blocks for heap
			page->heap->local_free[page->size_class] = page->local_free;
			page->block_used += page->local_free_count;
			page->local_free = 0;
			page->local_free_count = 0;
		}
		
		static NOINLINE void*
		page_initialize_blocks(page_t* page) {
			rpmalloc_assert(page->block_initialized < page->block_count, "Block initialization internal failure");
			block_t* block = page_block(page, page->block_initialized);
			++page->block_initialized;
			++page->block_used;
			
			if ((page->page_type == PAGE_SMALL) && (page->block_size < (global_config.page_size >> 1))) {
				// Link up until next memory page in free list
				void* memory_page_start = (void*)((uintptr_t)block & ~(uintptr_t)(global_config.page_size - 1));
				void* memory_page_next = pointer_offset(memory_page_start, global_config.page_size);
				block_t* free_block = pointer_offset(block, page->block_size);
				block_t* first_block = free_block;
				block_t* last_block = free_block;
				uint32_t list_count = 0;
				uint32_t max_list_count = page->block_count - page->block_initialized;
				while (((void*)free_block < memory_page_next) && (list_count < max_list_count)) {
					last_block = free_block;
					free_block->next = pointer_offset(free_block, page->block_size);
					free_block = free_block->next;
					++list_count;
				}
				if (list_count) {
					last_block->next = 0;
					page->local_free = first_block;
					page->block_initialized += list_count;
					page->local_free_count = list_count;
				}
			}
			
			return block;
		}
		
		static inline RPMALLOC_ALLOCATOR void*
		page_allocate_block(page_t* page, unsigned int zero) {
			unsigned int is_zero = 0;
			block_t* block = (page->local_free != 0) ? page_get_local_free_block(page) : 0;
			if (UNEXPECTED(block == 0)) {
				if (atomic_load_explicit(&page->thread_free, memory_order_relaxed) != 0) {
					page_adopt_thread_free_block_list(page);
					block = (page->local_free != 0) ? page_get_local_free_block(page) : 0;
				}
				if (block == 0) {
					block = page_initialize_blocks(page);
					is_zero = page->is_zero;
				}
			}
			
			rpmalloc_assert(page->block_used <= page->block_count, "Page block use counter out of sync");
			if (page->local_free && !page->heap->local_free[page->size_class])
				page_push_local_free_to_heap(page);
			
			// The page might be full when free list has been pushed to heap local free list,
			// check if there is a thread free list to adopt
			if (page->block_used == page->block_count)
				page_adopt_thread_free_block_list(page);
			
			if (page->block_used == page->block_count) {
				// Page is now fully utilized
				rpmalloc_assert(!page->is_full, "Page block use counter out of sync with full flag");
				page_available_to_full(page);
			}
			
			if (zero) {
				if (!is_zero)
					memset(block, 0, page->block_size);
				else
					*(uintptr_t*)block = 0;
			}
			
			return block;
		}
		
		////////////
		///
		/// Span interface
		///
		//////
		
		static inline int
		span_is_thread_heap(span_t* span) {
		#if RPMALLOC_FIRST_CLASS_HEAPS
			return (!span->heap->owner_thread || (span->heap->owner_thread == get_thread_id()));
		#else
			return (span->heap->owner_thread == get_thread_id());
		#endif
		}
		
		static inline page_t*
		span_get_page_from_block(span_t* span, void* block) {
			return (page_t*)((uintptr_t)block & span->page_address_mask);
		}
		
		//! Find or allocate a page from the given span
		static inline page_t*
		span_allocate_page(span_t* span) {
			// Allocate path, initialize a new chunk of memory for a page in the given span
			rpmalloc_assert(span->page_initialized < span->page_count, "Page initialization internal failure");
			heap_t* heap = span->heap;
			page_t* page = pointer_offset(span, span->page_size * span->page_initialized);
			
		#if ENABLE_DECOMMIT
			// The first page is always committed on initial span map of memory
			if (span->page_initialized)
				global_memory_interface->memory_commit(page, span->page_size);
		#endif
			++span->page_initialized;
			
			page->page_type = span->page_type;
			page->is_zero = 1;
			page->heap = heap;
			rpmalloc_assert(page_is_thread_heap(page), "Page owner thread mismatch");
			
			if (span->page_initialized == span->page_count) {
				// Span fully utilized
				rpmalloc_assert(span == heap->span_partial[span->page_type], "Span partial tracking out of sync");
				heap->span_partial[span->page_type] = 0;
				
				span->next = heap->span_used[span->page_type];
				heap->span_used[span->page_type] = span;
			}
			
			return page;
		}
		
		static NOINLINE void
		span_deallocate_block(span_t* span, page_t* page, void* block) {
			if (UNEXPECTED(page->page_type == PAGE_HUGE)) {
				global_memory_interface->memory_unmap(span, span->offset, span->mapped_size);
				return;
			}
			
			if (page->has_aligned_block) {
				// Realign pointer to block start
				block = page_block_realign(page, block);
			}
			
			int is_thread_local = page_is_thread_heap(page);
			if (EXPECTED(is_thread_local != 0)) {
				page_put_local_free_block(page, block);
			} else {
				// Multithreaded deallocation, push to deferred deallocation list.
				page_put_thread_free_block(page, block);
			}
		}
		
		////////////
		///
		/// Block interface
		///
		//////
		
		static inline span_t*
		block_get_span(block_t* block) {
			return (span_t*)((uintptr_t)block & SPAN_MASK);
		}
		
		static inline void
		block_deallocate(block_t* block) {
			span_t* span = (span_t*)((uintptr_t)block & SPAN_MASK);
			page_t* page = span_get_page_from_block(span, block);
			const int is_thread_local = page_is_thread_heap(page);
			
			// Optimized path for thread local free with non-huge block in page
			// that has no aligned blocks
			if (EXPECTED(is_thread_local != 0)) {
				if (EXPECTED(page->generic_free == 0)) {
					// Page is not huge, not full and has no aligned block - fast path
					block->next = page->local_free;
					page->local_free = block;
					++page->local_free_count;
					if (UNEXPECTED(--page->block_used == 0))
						page_available_to_free(page);
				} else {
					span_deallocate_block(span, page, block);
				}
			} else {
				span_deallocate_block(span, page, block);
			}
		}
		
		static inline size_t
		block_usable_size(block_t* block) {
			span_t* span = (span_t*)((uintptr_t)block & SPAN_MASK);
			if (EXPECTED(span->page_type <= PAGE_LARGE)) {
				page_t* page = span_get_page_from_block(span, block);
				void* blocks_start = pointer_offset(page, PAGE_HEADER_SIZE);
				return page->block_size - ((size_t)pointer_diff(block, blocks_start) % page->block_size);
			} else {
				return ((size_t)span->page_size * (size_t)span->page_count) - (size_t)pointer_diff(block, span);
			}
		}
		
		////////////
		///
		/// Heap interface
		///
		//////
		
		static inline void
		heap_lock_acquire(void) {
			uintptr_t lock = 0;
			uintptr_t this_lock = get_thread_id();
			while (!atomic_compare_exchange_strong(&global_heap_lock, &lock, this_lock)) {
				lock = 0;
				wait_spin();
			}
		}
		
		static inline void
		heap_lock_release(void) {
			rpmalloc_assert((uintptr_t)atomic_load_explicit(&global_heap_lock, memory_order_relaxed) == get_thread_id(),
							"Bad heap lock");
			atomic_store_explicit(&global_heap_lock, 0, memory_order_release);
		}
		
		static inline heap_t*
		heap_initialize(void* block) {
			heap_t* heap = block;
			memset_const(heap, 0, sizeof(heap_t));
			heap->id = 1 + atomic_fetch_add_explicit(&global_heap_id, 1, memory_order_relaxed);
			return heap;
		}
		
		static heap_t*
		heap_allocate_new(void) {
			if (!global_config.page_size)
				rpmalloc_initialize(0);
			size_t heap_size = get_page_aligned_size(sizeof(heap_t));
			size_t offset = 0;
			size_t mapped_size = 0;
			block_t* block = global_memory_interface->memory_map(heap_size, 0, &offset, &mapped_size);
		#if ENABLE_DECOMMIT
			global_memory_interface->memory_commit(block, heap_size);
		#endif
			heap_t* heap = heap_initialize((void*)block);
			heap->offset = (uint32_t)offset;
			heap->mapped_size = mapped_size;
		#if ENABLE_STATISTICS
			atomic_fetch_add_explicit(&global_statistics.heap_count, 1, memory_order_relaxed);
		#endif
			return heap;
		}
		
		static void
		heap_unmap(heap_t* heap) {
			global_memory_interface->memory_unmap(heap, heap->offset, heap->mapped_size);
		}
		
		static heap_t*
		heap_allocate(int first_class) {
			heap_t* heap = 0;
			if (!first_class) {
				heap_lock_acquire();
				heap = global_heap_queue;
				global_heap_queue = heap ? heap->next : 0;
				heap_lock_release();
			}
			if (!heap)
				heap = heap_allocate_new();
			if (heap) {
				uintptr_t current_thread_id = get_thread_id();
				heap_lock_acquire();
				heap->next = global_heap_used;
				heap->prev = 0;
				if (global_heap_used)
					global_heap_used->prev = heap;
				global_heap_used = heap;
				heap_lock_release();
				heap->owner_thread = current_thread_id;
			}
			return heap;
		}
		
		static inline void
		heap_release(heap_t* heap) {
			heap_lock_acquire();
			if (heap->prev)
				heap->prev->next = heap->next;
			if (heap->next)
				heap->next->prev = heap->prev;
			if (global_heap_used == heap)
				global_heap_used = heap->next;
			heap->next = global_heap_queue;
			global_heap_queue = heap;
			heap_lock_release();
		}
		
		static void
		heap_page_free_decommit(heap_t* heap, uint32_t page_type, uint32_t page_retain_count) {
			page_t* page = heap->page_free[page_type];
			while (page && page_retain_count) {
				page = page->next;
				--page_retain_count;
			}
			while (page && (page->is_decommitted == 0)) {
				page_decommit_memory_pages(page);
				--heap->page_free_commit_count[page_type];
				page = page->next;
			}
		}
		
		static inline void
		heap_make_free_page_available(heap_t* heap, uint32_t size_class, page_t* page) {
			page->size_class = size_class;
			page->block_size = global_size_class[size_class].block_size;
			page->block_count = global_size_class[size_class].block_count;
			page->block_used = 0;
			page->block_initialized = 0;
			page->local_free = 0;
			page->local_free_count = 0;
			page->is_full = 0;
			page->is_free = 0;
			page->has_aligned_block = 0;
			page->generic_free = 0;
			page->heap = heap;
			page_t* head = heap->page_available[size_class];
			page->next = head;
			page->prev = 0;
			atomic_store_explicit(&page->thread_free, 0, memory_order_relaxed);
			if (head)
				head->prev = page;
			heap->page_available[size_class] = page;
			if (page->is_decommitted)
				page_commit_memory_pages(page);
		}
		
		//! Find or allocate a span for the given page type with the given size class
		static inline span_t*
		heap_get_span(heap_t* heap, page_type_t page_type) {
			// Fast path, available span for given page type
			if (EXPECTED(heap->span_partial[page_type] != 0))
				return heap->span_partial[page_type];
			
			// Fallback path, map more memory
			size_t offset = 0;
			size_t mapped_size = 0;
			span_t* span = global_memory_interface->memory_map(SPAN_SIZE, SPAN_SIZE, &offset, &mapped_size);
			if (EXPECTED(span != 0)) {
				uint32_t page_count = 0;
				uint32_t page_size = 0;
				uintptr_t page_address_mask = 0;
				if (page_type == PAGE_SMALL) {
					page_count = SPAN_SIZE / SMALL_PAGE_SIZE;
					page_size = SMALL_PAGE_SIZE;
					page_address_mask = SMALL_PAGE_MASK;
				} else if (page_type == PAGE_MEDIUM) {
					page_count = SPAN_SIZE / MEDIUM_PAGE_SIZE;
					page_size = MEDIUM_PAGE_SIZE;
					page_address_mask = MEDIUM_PAGE_MASK;
				} else {
					page_count = SPAN_SIZE / LARGE_PAGE_SIZE;
					page_size = LARGE_PAGE_SIZE;
					page_address_mask = LARGE_PAGE_MASK;
				}
		#if ENABLE_DECOMMIT
				global_memory_interface->memory_commit(span, page_size);
		#endif
				span->heap = heap;
				span->page_type = page_type;
				span->page_count = page_count;
				span->page_size = page_size;
				span->page_address_mask = page_address_mask;
				span->offset = (uint32_t)offset;
				span->mapped_size = mapped_size;
				
				heap->span_partial[page_type] = span;
			}
			
			return span;
		}
		
		static page_t*
		heap_get_page(heap_t* heap, uint32_t size_class);
		
		static void
		block_deallocate(block_t* block);
		
		static page_t*
		heap_get_page_generic(heap_t* heap, uint32_t size_class) {
			page_type_t page_type = get_page_type(size_class);
			
			// Check if there is a free page from multithreaded deallocations
			uintptr_t block_mt = atomic_load_explicit(&heap->thread_free[page_type], memory_order_relaxed);
			if (UNEXPECTED(block_mt != 0)) {
				while (!atomic_compare_exchange_weak_explicit(&heap->thread_free[page_type], &block_mt, 0, memory_order_relaxed,
															  memory_order_relaxed)) {
					wait_spin();
				}
				block_t* block = (void*)block_mt;
				while (block) {
					block_t* next_block = block->next;
					block_deallocate(block);
					block = next_block;
				}
				// Retry after processing deferred thread frees
				return heap_get_page(heap, size_class);
			}
			
			// Check if there is a free page
			page_t* page = heap->page_free[page_type];
			if (EXPECTED(page != 0)) {
				heap->page_free[page_type] = page->next;
				if (page->is_decommitted == 0) {
					rpmalloc_assert(heap->page_free_commit_count[page_type] > 0, "Free committed page count out of sync");
					--heap->page_free_commit_count[page_type];
				}
				heap_make_free_page_available(heap, size_class, page);
				return page;
			}
			rpmalloc_assert(heap->page_free_commit_count[page_type] == 0, "Free committed page count out of sync");
			
			if (heap->id == 0) {
				// Thread has not yet initialized, assign heap and try again
				rpmalloc_initialize(0);
				return heap_get_page(get_thread_heap(), size_class);
			}
			
			// Fallback path, find or allocate span for given size class
			// If thread was not initialized, the heap for the new span
			// will be different from the local heap variable in this scope
			// (which is the default heap) - so use span page heap instead
			span_t* span = heap_get_span(heap, page_type);
			if (EXPECTED(span != 0)) {
				page = span_allocate_page(span);
				heap_make_free_page_available(page->heap, size_class, page);
			}
			
			return page;
		}
		
		//! Find or allocate a page for the given size class
		static page_t*
		heap_get_page(heap_t* heap, uint32_t size_class) {
			// Fast path, available page for given size class
			page_t* page = heap->page_available[size_class];
			if (EXPECTED(page != 0))
				return page;
			return heap_get_page_generic(heap, size_class);
		}
		
		//! Pop a block from the heap local free list
		static inline RPMALLOC_ALLOCATOR void*
		heap_pop_local_free(heap_t* heap, uint32_t size_class) {
			block_t** free_list = heap->local_free + size_class;
			block_t* block = *free_list;
			if (EXPECTED(block != 0))
				*free_list = block->next;
			return block;
		}
		
		//! Generic allocation path from heap pages, spans or new mapping
		static NOINLINE RPMALLOC_ALLOCATOR void*
		heap_allocate_block_small_to_large(heap_t* heap, uint32_t size_class, unsigned int zero) {
			page_t* page = heap_get_page(heap, size_class);
			if (EXPECTED(page != 0))
				return page_allocate_block(page, zero);
			return 0;
		}
		
		//! Generic allocation path from heap pages, spans or new mapping
		static NOINLINE RPMALLOC_ALLOCATOR void*
		heap_allocate_block_huge(heap_t* heap, size_t size, unsigned int zero) {
			(void)sizeof(heap);
			size_t alloc_size = get_page_aligned_size(size + SPAN_HEADER_SIZE);
			size_t offset = 0;
			size_t mapped_size = 0;
			void* block = global_memory_interface->memory_map(alloc_size, SPAN_SIZE, &offset, &mapped_size);
			if (block) {
				span_t* span = block;
		#if ENABLE_DECOMMIT
				global_memory_interface->memory_commit(span, alloc_size);
		#endif
				span->heap = heap;
				span->page_type = PAGE_HUGE;
				span->page_size = (uint32_t)global_config.page_size;
				span->page_count = (uint32_t)(alloc_size / global_config.page_size);
				span->page_address_mask = LARGE_PAGE_MASK;
				span->offset = (uint32_t)offset;
				span->mapped_size = mapped_size;
				span->page.heap = heap;
				span->page.is_full = 1;
				span->page.generic_free = 1;
				span->page.page_type = PAGE_HUGE;
				// Keep track of span if first class heap
				if (!heap->owner_thread) {
					span->next = heap->span_used[PAGE_HUGE];
					heap->span_used[PAGE_HUGE] = span;
				}
				void* ptr = pointer_offset(block, SPAN_HEADER_SIZE);
				if (zero)
					memset(ptr, 0, size);
				return ptr;
			}
			return 0;
		}
		
		static RPMALLOC_ALLOCATOR NOINLINE void*
		heap_allocate_block_generic(heap_t* heap, size_t size, unsigned int zero) {
			uint32_t size_class = get_size_class(size);
			if (EXPECTED(size_class < SIZE_CLASS_COUNT)) {
				block_t* block = heap_pop_local_free(heap, size_class);
				if (EXPECTED(block != 0)) {
					// Fast track with small block available in heap level local free list
					if (zero)
						memset(block, 0, global_size_class[size_class].block_size);
					return block;
				}
				
				return heap_allocate_block_small_to_large(heap, size_class, zero);
			}
			
			return heap_allocate_block_huge(heap, size, zero);
		}
		
		//! Find or allocate a block of the given size
		static inline RPMALLOC_ALLOCATOR void*
		heap_allocate_block(heap_t* heap, size_t size, unsigned int zero) {
			if (size <= (SMALL_GRANULARITY * 64)) {
				uint32_t size_class = get_size_class_tiny(size);
				block_t* block = heap_pop_local_free(heap, size_class);
				if (EXPECTED(block != 0)) {
					// Fast track with small block available in heap level local free list
					if (zero)
						memset(block, 0, global_size_class[size_class].block_size);
					return block;
				}
			}
			return heap_allocate_block_generic(heap, size, zero);
		}
		
		static RPMALLOC_ALLOCATOR void*
		heap_allocate_block_aligned(heap_t* heap, size_t alignment, size_t size, unsigned int zero) {
			if (alignment <= SMALL_GRANULARITY)
				return heap_allocate_block(heap, size, zero);
			
		#if ENABLE_VALIDATE_ARGS
			if ((size + alignment) < size) {
				errno = EINVAL;
				return 0;
			}
			if (alignment & (alignment - 1)) {
				errno = EINVAL;
				return 0;
			}
		#endif
			if (alignment >= RPMALLOC_MAX_ALIGNMENT) {
				errno = EINVAL;
				return 0;
			}
			
			size_t align_mask = alignment - 1;
			block_t* block = heap_allocate_block(heap, size + alignment, zero);
			if ((uintptr_t)block & align_mask) {
				block = (void*)(((uintptr_t)block & ~(uintptr_t)align_mask) + alignment);
				// Mark as having aligned blocks
				span_t* span = block_get_span(block);
				page_t* page = span_get_page_from_block(span, block);
				page->has_aligned_block = 1;
				page->generic_free = 1;
			}
			return block;
		}
		
		static void*
		heap_reallocate_block(heap_t* heap, void* block, size_t size, size_t old_size, unsigned int flags) {
			if (block) {
				// Grab the span using guaranteed span alignment
				span_t* span = block_get_span(block);
				if (EXPECTED(span->page_type <= PAGE_LARGE)) {
					// Normal sized block
					page_t* page = span_get_page_from_block(span, block);
					void* blocks_start = pointer_offset(page, PAGE_HEADER_SIZE);
					uint32_t block_offset = (uint32_t)pointer_diff(block, blocks_start);
					uint32_t block_idx = block_offset / page->block_size;
					void* block_origin = pointer_offset(blocks_start, (size_t)block_idx * page->block_size);
					if (!old_size)
						old_size = (size_t)((ptrdiff_t)page->block_size - pointer_diff(block, block_origin));
					if ((size_t)page->block_size >= size) {
						// Still fits in block, never mind trying to save memory, but preserve data if alignment changed
						if ((block != block_origin) && !(flags & RPMALLOC_NO_PRESERVE))
							memmove(block_origin, block, old_size);
						return block_origin;
					}
				} else {
					// Huge block
					void* block_start = pointer_offset(span, SPAN_HEADER_SIZE);
					if (!old_size)
						old_size = ((size_t)span->page_size * (size_t)span->page_count) - SPAN_HEADER_SIZE;
					if ((size < old_size) && (size > LARGE_BLOCK_SIZE_LIMIT)) {
						// Still fits in block and still huge, never mind trying to save memory,
						// but preserve data if alignment changed
						if ((block_start != block) && !(flags & RPMALLOC_NO_PRESERVE))
							memmove(block_start, block, old_size);
						return block_start;
					}
				}
			} else {
				old_size = 0;
			}
			
			if (!!(flags & RPMALLOC_GROW_OR_FAIL))
				return 0;
			
			// Size is greater than block size or saves enough memory to resize, need to allocate a new block
			// and deallocate the old. Avoid hysteresis by overallocating if increase is small (below 37%)
			size_t lower_bound = old_size + (old_size >> 2) + (old_size >> 3);
			size_t new_size = (size > lower_bound) ? size : ((size > old_size) ? lower_bound : size);
			void* old_block = block;
			block = heap_allocate_block(heap, new_size, 0);
			if (block && old_block) {
				if (!(flags & RPMALLOC_NO_PRESERVE))
					memcpy(block, old_block, old_size < new_size ? old_size : new_size);
				block_deallocate(old_block);
			}
			
			return block;
		}
		
		static void*
		heap_reallocate_block_aligned(heap_t* heap, void* block, size_t alignment, size_t size, size_t old_size,
									  unsigned int flags) {
			if (alignment <= SMALL_GRANULARITY)
				return heap_reallocate_block(heap, block, size, old_size, flags);
			
			int no_alloc = !!(flags & RPMALLOC_GROW_OR_FAIL);
			size_t usable_size = (block ? block_usable_size(block) : 0);
			if ((usable_size >= size) && !((uintptr_t)block & (alignment - 1))) {
				if (no_alloc || (size >= (usable_size / 2)))
					return block;
			}
			// Aligned alloc marks span as having aligned blocks
			void* old_block = block;
			block = (!no_alloc ? heap_allocate_block_aligned(heap, alignment, size, 0) : 0);
			if (EXPECTED(block != 0)) {
				if (!(flags & RPMALLOC_NO_PRESERVE) && old_block) {
					if (!old_size)
						old_size = usable_size;
					memcpy(block, old_block, old_size < size ? old_size : size);
				}
				if (EXPECTED(old_block != 0))
					block_deallocate(old_block);
			}
			return block;
		}
		
		static void
		heap_free_all(heap_t* heap) {
			for (int itype = 0; itype < 3; ++itype) {
				span_t* span = heap->span_partial[itype];
				while (span) {
					span_t* span_next = span->next;
					global_memory_interface->memory_unmap(span, span->offset, span->mapped_size);
					span = span_next;
				}
				heap->span_partial[itype] = 0;
				heap->page_free[itype] = 0;
				heap->page_free_commit_count[itype] = 0;
				atomic_store_explicit(&heap->thread_free[itype], 0, memory_order_relaxed);
			}
			for (int itype = 0; itype < 4; ++itype) {
				span_t* span = heap->span_used[itype];
				while (span) {
					span_t* span_next = span->next;
					global_memory_interface->memory_unmap(span, span->offset, span->mapped_size);
					span = span_next;
				}
				heap->span_used[itype] = 0;
			}
			memset(heap->local_free, 0, sizeof(heap->local_free));
			memset(heap->page_available, 0, sizeof(heap->page_available));
			
		#if ENABLE_STATISTICS
			// TODO: Fix
		#endif
		}
		
		////////////
		///
		/// Extern interface
		///
		//////
		
		int
		rpmalloc_is_thread_initialized(void) {
			return (get_thread_heap() != global_heap_default) ? 1 : 0;
		}
		
		extern inline RPMALLOC_ALLOCATOR void*
		rpmalloc(size_t size) {
		#if ENABLE_VALIDATE_ARGS
			if (size >= MAX_ALLOC_SIZE) {
				errno = EINVAL;
				return 0;
			}
		#endif
			heap_t* heap = get_thread_heap();
			return heap_allocate_block(heap, size, 0);
		}
		
		extern inline RPMALLOC_ALLOCATOR void*
		rpzalloc(size_t size) {
		#if ENABLE_VALIDATE_ARGS
			if (size >= MAX_ALLOC_SIZE) {
				errno = EINVAL;
				return 0;
			}
		#endif
			heap_t* heap = get_thread_heap();
			return heap_allocate_block(heap, size, 1);
		}
		
		extern inline void
		rpfree(void* ptr) {
			if (UNEXPECTED(ptr == 0))
				return;
			block_deallocate(ptr);
		}
		
		extern inline RPMALLOC_ALLOCATOR void*
		rpcalloc(size_t num, size_t size) {
			size_t total;
		#if ENABLE_VALIDATE_ARGS
		#if PLATFORM_WINDOWS
			int err = SizeTMult(num, size, &total);
			if ((err != S_OK) || (total >= MAX_ALLOC_SIZE)) {
				errno = EINVAL;
				return 0;
			}
		#else
			int err = __builtin_umull_overflow(num, size, &total);
			if (err || (total >= MAX_ALLOC_SIZE)) {
				errno = EINVAL;
				return 0;
			}
		#endif
		#else
			total = num * size;
		#endif
			heap_t* heap = get_thread_heap();
			return heap_allocate_block(heap, total, 1);
		}
		
		extern inline RPMALLOC_ALLOCATOR void*
		rprealloc(void* ptr, size_t size) {
		#if ENABLE_VALIDATE_ARGS
			if (size >= MAX_ALLOC_SIZE) {
				errno = EINVAL;
				return ptr;
			}
		#endif
			heap_t* heap = get_thread_heap();
			return heap_reallocate_block(heap, ptr, size, 0, 0);
		}
		
		extern RPMALLOC_ALLOCATOR void*
		rpaligned_realloc(void* ptr, size_t alignment, size_t size, size_t oldsize, unsigned int flags) {
		#if ENABLE_VALIDATE_ARGS
			if ((size + alignment < size) || (alignment > _memory_page_size)) {
				errno = EINVAL;
				return 0;
			}
		#endif
			heap_t* heap = get_thread_heap();
			return heap_reallocate_block_aligned(heap, ptr, alignment, size, oldsize, flags);
		}
		
		extern RPMALLOC_ALLOCATOR void*
		rpaligned_alloc(size_t alignment, size_t size) {
			heap_t* heap = get_thread_heap();
			return heap_allocate_block_aligned(heap, alignment, size, 0);
		}
		
		extern RPMALLOC_ALLOCATOR void*
		rpaligned_zalloc(size_t alignment, size_t size) {
			heap_t* heap = get_thread_heap();
			return heap_allocate_block_aligned(heap, alignment, size, 1);
		}
		
		extern inline RPMALLOC_ALLOCATOR void*
		rpaligned_calloc(size_t alignment, size_t num, size_t size) {
			size_t total;
		#if ENABLE_VALIDATE_ARGS
		#if PLATFORM_WINDOWS
			int err = SizeTMult(num, size, &total);
			if ((err != S_OK) || (total >= MAX_ALLOC_SIZE)) {
				errno = EINVAL;
				return 0;
			}
		#else
			int err = __builtin_umull_overflow(num, size, &total);
			if (err || (total >= MAX_ALLOC_SIZE)) {
				errno = EINVAL;
				return 0;
			}
		#endif
		#else
			total = num * size;
		#endif
			heap_t* heap = get_thread_heap();
			return heap_allocate_block_aligned(heap, alignment, total, 1);
		}
		
		extern inline RPMALLOC_ALLOCATOR void*
		rpmemalign(size_t alignment, size_t size) {
			heap_t* heap = get_thread_heap();
			return heap_allocate_block_aligned(heap, alignment, size, 0);
		}
		
		extern inline int
		rpposix_memalign(void** memptr, size_t alignment, size_t size) {
			heap_t* heap = get_thread_heap();
			if (memptr)
				*memptr = heap_allocate_block_aligned(heap, alignment, size, 0);
			else
				return EINVAL;
			return *memptr ? 0 : ENOMEM;
		}
		
		extern inline size_t
		rpmalloc_usable_size(void* ptr) {
			return (ptr ? block_usable_size(ptr) : 0);
		}
		
		////////////
		///
		/// Initialization and finalization
		///
		//////
		
		static void
		rpmalloc_thread_destructor(void* value) {
			// If this is called on main thread assume it means rpmalloc_finalize
			// has not been called and shutdown is forced (through _exit) or unclean
			if (get_thread_id() == global_main_thread_id)
				return;
			if (value)
				rpmalloc_thread_finalize();
		}
		
		extern int
		rpmalloc_initialize_config(rpmalloc_interface_t* memory_interface, rpmalloc_config_t* config) {
			if (global_rpmalloc_initialized) {
				rpmalloc_thread_initialize();
				if (config)
					*config = global_config;
				return 0;
			}
			
			if (config)
				global_config = *config;
			
			int result = rpmalloc_initialize(memory_interface);
			
			if (config)
				*config = global_config;
			
			return result;
		}
		
		extern int
		rpmalloc_initialize(rpmalloc_interface_t* memory_interface) {
			if (global_rpmalloc_initialized) {
				rpmalloc_thread_initialize();
				return 0;
			}
			
			global_rpmalloc_initialized = 1;
			
			global_memory_interface = memory_interface ? memory_interface : &global_memory_interface_default;
			if (!global_memory_interface->memory_map || !global_memory_interface->memory_unmap) {
				global_memory_interface->memory_map = os_mmap;
				global_memory_interface->memory_commit = os_mcommit;
				global_memory_interface->memory_decommit = os_mdecommit;
				global_memory_interface->memory_unmap = os_munmap;
			}
			
		#if PLATFORM_WINDOWS
			SYSTEM_INFO system_info;
			memset(&system_info, 0, sizeof(system_info));
			GetSystemInfo(&system_info);
			os_map_granularity = system_info.dwAllocationGranularity;
		#else
			os_map_granularity = (size_t)sysconf(_SC_PAGESIZE);
		#endif
			
		#if PLATFORM_WINDOWS
			os_page_size = system_info.dwPageSize;
		#else
			os_page_size = os_map_granularity;
		#endif
			if (global_config.enable_huge_pages) {
		#if PLATFORM_WINDOWS
				HANDLE token = 0;
				size_t large_page_minimum = GetLargePageMinimum();
				if (large_page_minimum)
					OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token);
				if (token) {
					LUID luid;
					if (LookupPrivilegeValue(0, SE_LOCK_MEMORY_NAME, &luid)) {
						TOKEN_PRIVILEGES token_privileges;
						memset(&token_privileges, 0, sizeof(token_privileges));
						token_privileges.PrivilegeCount = 1;
						token_privileges.Privileges[0].Luid = luid;
						token_privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
						if (AdjustTokenPrivileges(token, FALSE, &token_privileges, 0, 0, 0)) {
							if (GetLastError() == ERROR_SUCCESS)
								os_huge_pages = 1;
						}
					}
					CloseHandle(token);
				}
				if (os_huge_pages) {
					if (large_page_minimum > os_page_size)
						os_page_size = large_page_minimum;
					if (large_page_minimum > os_map_granularity)
						os_map_granularity = large_page_minimum;
				}
		#elif defined(__linux__)
				size_t huge_page_size = 0;
				FILE* meminfo = fopen("/proc/meminfo", "r");
				if (meminfo) {
					char line[128];
					while (!huge_page_size && fgets(line, sizeof(line) - 1, meminfo)) {
						line[sizeof(line) - 1] = 0;
						if (strstr(line, "Hugepagesize:"))
							huge_page_size = (size_t)strtol(line + 13, 0, 10) * 1024;
					}
					fclose(meminfo);
				}
				if (huge_page_size) {
					os_huge_pages = 1;
					os_page_size = huge_page_size;
					os_map_granularity = huge_page_size;
				}
		#elif defined(__FreeBSD__)
				int rc;
				size_t sz = sizeof(rc);
				
				if (sysctlbyname("vm.pmap.pg_ps_enabled", &rc, &sz, NULL, 0) == 0 && rc == 1) {
					os_huge_pages = 1;
					os_page_size = 2 * 1024 * 1024;
					os_map_granularity = os_page_size;
				}
		#elif defined(__APPLE__) || defined(__NetBSD__)
				os_huge_pages = 1;
				os_page_size = 2 * 1024 * 1024;
				os_map_granularity = os_page_size;
		#endif
			} else {
				os_huge_pages = 0;
			}
			
			global_config.enable_huge_pages = os_huge_pages;
			
			if (!memory_interface || (global_config.page_size < os_page_size))
				global_config.page_size = os_page_size;
			
			if (global_config.enable_huge_pages || global_config.page_size > (256 * 1024))
				global_config.disable_decommit = 1;
			
		#if defined(__linux__) || defined(__ANDROID__)
			if (global_config.disable_thp)
				(void)prctl(PR_SET_THP_DISABLE, 1, 0, 0, 0);
		#endif
			
		#ifdef _WIN32
			fls_key = FlsAlloc(&rpmalloc_thread_destructor);
		#else
			pthread_key_create(&pthread_key, rpmalloc_thread_destructor);
		#endif
			
			global_main_thread_id = get_thread_id();
			
			rpmalloc_thread_initialize();
			
			return 0;
		}
		
		extern const rpmalloc_config_t*
		rpmalloc_config(void) {
			return &global_config;
		}
		
		extern void
		rpmalloc_finalize(void) {
			rpmalloc_thread_finalize();

			if (global_config.unmap_on_finalize) {
				heap_t* heap = global_heap_queue;
				global_heap_queue = 0;
				while (heap) {
					heap_t* heap_next = heap->next;
					heap_free_all(heap);
					heap_unmap(heap);
					heap = heap_next;
				}
				heap = global_heap_used;
				global_heap_used = 0;
				while (heap) {
					heap_t* heap_next = heap->next;
					heap_free_all(heap);
					heap_unmap(heap);
					heap = heap_next;
				}
		#if ENABLE_STATISTICS
				memset(&global_statistics, 0, sizeof(global_statistics));
		#endif
			}
			
		#ifdef _WIN32
			FlsFree(fls_key);
			fls_key = 0;
		#else
			pthread_key_delete(pthread_key);
			pthread_key = 0;
		#endif
			
			global_main_thread_id = 0;
			global_rpmalloc_initialized = 0;
		}
		
		extern void
		rpmalloc_thread_initialize(void) {
			if (get_thread_heap() == global_heap_default)
				get_thread_heap_allocate();
		}
		
		extern void
		rpmalloc_thread_finalize(void) {
			heap_t* heap = get_thread_heap();
			if (heap != global_heap_default) {
				heap_release(heap);
				set_thread_heap(global_heap_default);
			}
		}
		
		extern void
		rpmalloc_thread_collect(void) {
		}
		
		void
		rpmalloc_dump_statistics(void* file) {
		#if ENABLE_STATISTICS
			fprintf(file, "Mapped pages:        %llu\n",
					(unsigned long long)atomic_load_explicit(&global_statistics.page_mapped, memory_order_relaxed));
			fprintf(file, "Mapped pages (peak): %llu\n",
					(unsigned long long)atomic_load_explicit(&global_statistics.page_mapped_peak, memory_order_relaxed));
			fprintf(file, "Active pages:        %llu\n",
					(unsigned long long)atomic_load_explicit(&global_statistics.page_active, memory_order_relaxed));
			fprintf(file, "Active pages (peak): %llu\n",
					(unsigned long long)atomic_load_explicit(&global_statistics.page_active_peak, memory_order_relaxed));
			fprintf(file, "Pages committed:     %llu\n",
					(unsigned long long)atomic_load_explicit(&global_statistics.page_commit, memory_order_relaxed));
			fprintf(file, "Pages decommitted:   %llu\n",
					(unsigned long long)atomic_load_explicit(&global_statistics.page_decommit, memory_order_relaxed));
			fprintf(file, "Heaps created:       %llu\n",
					(unsigned long long)atomic_load_explicit(&global_statistics.heap_count, memory_order_relaxed));
		#else
			(void)sizeof(file);
		#endif
		}
		
		#if RPMALLOC_FIRST_CLASS_HEAPS
		
		rpmalloc_heap_t*
		rpmalloc_heap_acquire(void) {
			// Must be a pristine heap from newly mapped memory pages, or else memory blocks
			// could already be allocated from the heap which would (wrongly) be released when
			// heap is cleared with rpmalloc_heap_free_all(). Also heaps guaranteed to be
			// pristine from the dedicated orphan list can be used.
			heap_t* heap = heap_allocate(1);
			rpmalloc_assume(heap != 0);
			heap->owner_thread = 0;
			return heap;
		}
		
		void
		rpmalloc_heap_release(rpmalloc_heap_t* heap) {
			if (heap)
				heap_release(heap);
		}
		
		RPMALLOC_ALLOCATOR void*
		rpmalloc_heap_alloc(rpmalloc_heap_t* heap, size_t size) {
		#if ENABLE_VALIDATE_ARGS
			if (size >= MAX_ALLOC_SIZE) {
				errno = EINVAL;
				return 0;
			}
		#endif
			return heap_allocate_block(heap, size, 0);
		}
		
		RPMALLOC_ALLOCATOR void*
		rpmalloc_heap_aligned_alloc(rpmalloc_heap_t* heap, size_t alignment, size_t size) {
		#if ENABLE_VALIDATE_ARGS
			if (size >= MAX_ALLOC_SIZE) {
				errno = EINVAL;
				return 0;
			}
		#endif
			return heap_allocate_block_aligned(heap, alignment, size, 0);
		}
		
		RPMALLOC_ALLOCATOR void*
		rpmalloc_heap_calloc(rpmalloc_heap_t* heap, size_t num, size_t size) {
			size_t total;
		#if ENABLE_VALIDATE_ARGS
		#if PLATFORM_WINDOWS
			int err = SizeTMult(num, size, &total);
			if ((err != S_OK) || (total >= MAX_ALLOC_SIZE)) {
				errno = EINVAL;
				return 0;
			}
		#else
			int err = __builtin_umull_overflow(num, size, &total);
			if (err || (total >= MAX_ALLOC_SIZE)) {
				errno = EINVAL;
				return 0;
			}
		#endif
		#else
			total = num * size;
		#endif
			return heap_allocate_block(heap, total, 1);
		}
		
		extern inline RPMALLOC_ALLOCATOR void*
		rpmalloc_heap_aligned_calloc(rpmalloc_heap_t* heap, size_t alignment, size_t num, size_t size) {
			size_t total;
		#if ENABLE_VALIDATE_ARGS
		#if PLATFORM_WINDOWS
			int err = SizeTMult(num, size, &total);
			if ((err != S_OK) || (total >= MAX_ALLOC_SIZE)) {
				errno = EINVAL;
				return 0;
			}
		#else
			int err = __builtin_umull_overflow(num, size, &total);
			if (err || (total >= MAX_ALLOC_SIZE)) {
				errno = EINVAL;
				return 0;
			}
		#endif
		#else
			total = num * size;
		#endif
			return heap_allocate_block_aligned(heap, alignment, total, 1);
		}
		
		RPMALLOC_ALLOCATOR void*
		rpmalloc_heap_realloc(rpmalloc_heap_t* heap, void* ptr, size_t size, unsigned int flags) {
		#if ENABLE_VALIDATE_ARGS
			if (size >= MAX_ALLOC_SIZE) {
				errno = EINVAL;
				return ptr;
			}
		#endif
			return heap_reallocate_block(heap, ptr, size, 0, flags);
		}
		
		RPMALLOC_ALLOCATOR void*
		rpmalloc_heap_aligned_realloc(rpmalloc_heap_t* heap, void* ptr, size_t alignment, size_t size, unsigned int flags) {
		#if ENABLE_VALIDATE_ARGS
			if ((size + alignment < size) || (alignment > _memory_page_size)) {
				errno = EINVAL;
				return 0;
			}
		#endif
			return heap_reallocate_block_aligned(heap, ptr, alignment, size, 0, flags);
		}
		
		void
		rpmalloc_heap_free(rpmalloc_heap_t* heap, void* ptr) {
			(void)sizeof(heap);
			block_deallocate(ptr);
		}
		
		//! Free all memory allocated by the heap
		void
		rpmalloc_heap_free_all(rpmalloc_heap_t* heap) {
			heap_free_all(heap);
		}
		
		extern inline void
		rpmalloc_heap_thread_set_current(rpmalloc_heap_t* heap) {
			heap_t* prev_heap = get_thread_heap();
			if (prev_heap != heap) {
				set_thread_heap(heap);
				if (prev_heap)
					heap_release(prev_heap);
			}
		}
		
		rpmalloc_heap_t*
		rpmalloc_get_heap_for_ptr(void* ptr) {
			// Grab the span, and then the heap from the span
			span_t* span = (span_t*)((uintptr_t)ptr & SPAN_MASK);
			if (span)
				return span_get_page_from_block(span, ptr)->heap;
			return 0;
		}
		
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
		
		// 创建结构化内存管理器
		XXAPI SMMU_Object SMMU_Create(unsigned int iItemLength, unsigned int PreassignStep)
		{
			SMMU_Object ObjPtr = mmu_malloc(sizeof(SMMU_Struct));
			if ( ObjPtr ) {
				SMMU_Init(ObjPtr, iItemLength, PreassignStep);
			}
			return ObjPtr;
		}
		
		// 销毁结构化内存管理器
		XXAPI void SMMU_Destroy(SMMU_Object pObject)
		{
			if ( pObject ) {
				SMMU_Unit(pObject);
				mmu_free(pObject);
			}
		}
		
		// 初始化内存管理器（结构体使用，和 SMMU_Create 功能类似）
		XXAPI void SMMU_Init(SMMU_Object pObject, unsigned int iItemLength, unsigned int PreassignStep)
		{
			pObject->Memory = 0;
			pObject->ItemLength = iItemLength;
			pObject->Count = 0;
			pObject->AllocCount = 0;
			pObject->AllocStep = PreassignStep ? PreassignStep : 64;
		}
		
		// 释放内存管理器（对自维护结构体指针使用，和 SMMU_Destroy 功能类似）
		XXAPI void SMMU_Unit(SMMU_Object pObject)
		{
			if ( pObject->Memory ) { mmu_free(pObject->Memory); pObject->Memory = NULL; }
			pObject->Count = 0;
			pObject->AllocCount = 0;
		}
		
		// 分配内存
		XXAPI int SMMU_Malloc(SMMU_Object pObject, unsigned int iCount)
		{
			if ( iCount > pObject->AllocCount ) {
				// 增量
				void* pNew = mmu_realloc(pObject->Memory, iCount * pObject->ItemLength);
				if ( pNew ) {
					pObject->AllocCount = iCount;
					pObject->Memory = pNew;
					return -1;
				}
			} else if ( iCount < pObject->AllocCount ) {
				// 裁剪
				void* pNew = mmu_realloc(pObject->Memory, iCount * pObject->ItemLength);
				if ( pNew ) {
					pObject->AllocCount = iCount;
					pObject->Memory = pNew;
					if ( pObject->Count > iCount ) {
						// 需要裁剪数据
						pObject->Count = iCount;
					}
					return -1;
				}
			} else if ( iCount = 0 ) {
				// 清空
				SMMU_Unit(pObject);
				return -1;
			} else {
				// 不变
				return -1;
			}
			return 0;
		}
		
		// 中间插入成员
		XXAPI unsigned int SMMU_Insert(SMMU_Object pObject, unsigned int iPos, unsigned int iCount)
		{
			// 不能添加 0 个成员
			if ( iCount == 0 ) { iCount = 1; }
			// 分配内存
			if ( (pObject->Count + iCount) > pObject->AllocCount ) {
				if ( SMMU_Malloc(pObject, pObject->Count + iCount + pObject->AllocStep) == 0 ) {
					return 0;
				}
			}
			if ( iPos < pObject->Count ) {
				// 插入模式（需要移动内存）
				void* dst = pObject->Memory + ((iPos + iCount) * pObject->ItemLength);
				void* src = pObject->Memory + (iPos * pObject->ItemLength);
				memmove(dst, src, (pObject->Count - iPos) * pObject->ItemLength);
				pObject->Count += iCount;
				return iPos + 1;
			} else {
				// 追加模式
				pObject->Count += iCount;
				return pObject->Count - iCount + 1;
			}
		}
		
		// 末尾添加成员
		XXAPI unsigned int SMMU_Append(SMMU_Object pObject, unsigned int iCount)
		{
			return SMMU_Insert(pObject, pObject->Count, iCount);
		}
		
		// 交换成员
		XXAPI int SMMU_Swap(SMMU_Object pObject, unsigned int iPosA, unsigned int iPosB)
		{
			// 范围检查
			if ( iPosA == 0 ) { return 0; }
			if ( iPosA > pObject->Count ) { return 0; }
			if ( iPosB == 0 ) { return 0; }
			if ( iPosB > pObject->Count ) { return 0; }
			if ( iPosA == iPosB ) { return -1; }
			// 交换成员
			iPosA--;
			iPosB--;
			void* pStuA = mmu_malloc(pObject->ItemLength);
			memmove(pStuA, pObject->Memory + (iPosA * pObject->ItemLength), pObject->ItemLength);
			memmove(pObject->Memory + (iPosA * pObject->ItemLength), pObject->Memory + (iPosB * pObject->ItemLength), pObject->ItemLength);
			memmove(pObject->Memory + (iPosB * pObject->ItemLength), pStuA, pObject->ItemLength);
			mmu_free(pStuA);
			return -1;
		}
		
		// 删除成员
		XXAPI int SMMU_Remove(SMMU_Object pObject, unsigned int iPos, unsigned int iCount)
		{
			// 不能添加 0 个成员、不能删除不存在的成员（0号成员也不存在）
			if ( iCount == 0 ) { return 0; }
			if ( iPos == 0 ) { return 0; }
			if ( iPos > pObject->Count ) { return 0; }
			// 删除成员（数量不足时删除后面的所有成员）
			iPos--;
			if ( iPos + iCount < pObject->Count ) {
				// 中段删除
				void* dst = pObject->Memory + (iPos * pObject->ItemLength);
				void* src = pObject->Memory + ((iPos + iCount) * pObject->ItemLength);
				memmove(dst, src, (pObject->Count - (iPos + iCount)) * pObject->ItemLength);
				pObject->Count -= iCount;
			} else {
				// 末尾删除
				pObject->Count = iPos;
			}
			return -1;
		}
		
		// 获取成员指针
		XXAPI void* SMMU_GetPtr(SMMU_Object pObject, unsigned int iPos)
		{
			if ( iPos ) {
				iPos--;
				if ( iPos < pObject->Count ) {
					return &pObject->Memory[iPos * pObject->ItemLength];
				}
			}
			return 0;
		}
		XXAPI void* SMMU_GetPtr_Unsafe(SMMU_Object pObject, unsigned int iPos)
		{
			return &(pObject->Memory[(iPos - 1) * pObject->ItemLength]);
		}
		
		// 成员排序
		XXAPI int SMMU_Sort(SMMU_Object pObject, void* procCompar)
		{
			if ( pObject ) {
				qsort(pObject->Memory, pObject->Count, pObject->ItemLength, procCompar);
				return -1;
			} else {
				return 0;
			}
		}
		
	#endif





	/*
		Point Memory Management Unit [指针内存管理单元]
			成员编号规则（0为不存在的成员编号）：
				┌──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬────┐
				│01│02│03│04│05│06│07│08│09│10│11│12│ .. │
				└──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴────┘
	*/

	#ifdef MMU_USE_PMMU
		
		// 创建指针内存管理器
		XXAPI PMMU_Object PMMU_Create()
		{
			PMMU_Object ObjPtr = mmu_malloc(sizeof(PMMU_Struct));
			if ( ObjPtr ) {
				PMMU_Init(ObjPtr);
			}
			return ObjPtr;
		}
		
		// 销毁指针内存管理器
		XXAPI void PMMU_Destroy(PMMU_Object pObject)
		{
			if ( pObject ) {
				PMMU_Unit(pObject);
				mmu_free(pObject);
			}
		}
		
		// 初始化内存管理器（对自维护结构体指针使用，和 PMMU_Create 功能类似）
		XXAPI void PMMU_Init(PMMU_Object pObject)
		{
			pObject->Memory = NULL;
			pObject->Count = 0;
			pObject->AllocCount = 0;
			pObject->AllocStep = PMMU_PREASSIGNSTEP;
		}
		
		// 释放内存管理器（对自维护结构体指针使用，和 PMMU_Destroy 功能类似）
		XXAPI void PMMU_Unit(PMMU_Object pObject)
		{
			if ( pObject->Memory ) { mmu_free(pObject->Memory); pObject->Memory = NULL; }
			pObject->Count = 0;
			pObject->AllocCount = 0;
		}
		
		// 分配内存
		XXAPI int PMMU_Malloc(PMMU_Object pObject, unsigned int iCount)
		{
			if ( iCount > pObject->AllocCount ) {
				// 增量
				void** pNew = mmu_realloc(pObject->Memory, iCount * sizeof(void*));
				if ( pNew ) {
					pObject->AllocCount = iCount;
					pObject->Memory = pNew;
					return -1;
				}
			} else if ( iCount < pObject->AllocCount ) {
				// 裁剪
				void** pNew = mmu_realloc(pObject->Memory, iCount * sizeof(void*));
				if ( pNew ) {
					pObject->AllocCount = iCount;
					pObject->Memory = pNew;
					if ( pObject->Count > iCount ) {
						// 需要裁剪数据
						pObject->Count = iCount;
					}
					return -1;
				}
			} else if ( iCount = 0 ) {
				// 清空
				PMMU_Unit(pObject);
				return -1;
			} else {
				// 不变
				return -1;
			}
			return 0;
		}
		
		// 中间插入成员(0为头部插入，pObject->Count为末尾插入)
		XXAPI unsigned int PMMU_Insert(PMMU_Object pObject, unsigned int iPos, void* pVal)
		{
			// 分配内存
			if ( pObject->Count >= pObject->AllocCount ) {
				if ( PMMU_Malloc(pObject, pObject->Count + pObject->AllocStep) == 0 ) {
					return 0;
				}
			}
			if ( iPos < pObject->Count ) {
				// 插入模式（需要移动内存）
				void** src = &(pObject->Memory[iPos]);
				memmove(src + 1, src, (pObject->Count - iPos) * sizeof(void*));
				pObject->Memory[iPos] = pVal;
				pObject->Count++;
				return iPos + 1;
			} else {
				// 追加模式
				pObject->Memory[pObject->Count] = pVal;
				pObject->Count++;
				return pObject->Count;
			}
		}
		
		// 末尾添加成员
		XXAPI unsigned int PMMU_Append(PMMU_Object pObject, void* pVal)
		{
			return PMMU_Insert(pObject, pObject->Count, pVal);
		}
		
		// 添加成员，自动查找空隙（替换为 NULL 的值）
		XXAPI unsigned int PMMU_Add(PMMU_Object pObject, void* pVal)
		{
			for ( int i = 0; i < pObject->Count; i++ ) {
				if ( pObject->Memory[i] == NULL ) {
					pObject->Memory[i] = pVal;
					return i + 1;
				}
			}
			return PMMU_Insert(pObject, pObject->Count, pVal);
		}
		
		// 交换成员
		XXAPI int PMMU_Swap(PMMU_Object pObject, unsigned int iPosA, unsigned int iPosB)
		{
			// 范围检查
			if ( iPosA == 0 ) { return 0; }
			if ( iPosA > pObject->Count ) { return 0; }
			if ( iPosB == 0 ) { return 0; }
			if ( iPosB > pObject->Count ) { return 0; }
			if ( iPosA == iPosB ) { return -1; }
			// 交换成员
			iPosA--;
			iPosB--;
			void* pA = pObject->Memory[iPosB];
			pObject->Memory[iPosB] = pObject->Memory[iPosA];
			pObject->Memory[iPosA] = pA;
			return -1;
		}
		
		// 删除成员
		XXAPI int PMMU_Remove(PMMU_Object pObject, unsigned int iPos, unsigned int iCount)
		{
			// 不能添加 0 个成员、不能删除不存在的成员（0号成员也不存在）
			if ( iCount == 0 ) { return 0; }
			if ( iPos == 0 ) { return 0; }
			if ( iPos > pObject->Count ) { return 0; }
			// 删除成员（数量不足时删除后面的所有成员）
			iPos--;
			if ( iPos + iCount < pObject->Count ) {
				// 中段删除
				void** dst = &(pObject->Memory[iPos]);
				memmove(dst, dst + iCount, (pObject->Count - (iPos + iCount)) * sizeof(void*));
				pObject->Count -= iCount;
			} else {
				// 末尾删除
				pObject->Count = iPos;
			}
			return -1;
		}
		
		// 获取成员指针
		XXAPI void** PMMU_GetPtr(PMMU_Object pObject, unsigned int iPos)
		{
			if ( iPos ) {
				iPos--;
				if ( iPos < pObject->Count ) {
					return &(pObject->Memory[iPos]);
				}
			}
			return 0;
		}
		XXAPI void* PMMU_GetVal(PMMU_Object pObject, unsigned int iPos)
		{
			if ( iPos ) {
				iPos--;
				if ( iPos < pObject->Count ) {
					return pObject->Memory[iPos];
				}
			}
			return 0;
		}
		XXAPI void** PMMU_GetPtr_Unsafe(PMMU_Object pObject, unsigned int iPos)
		{
			return &(pObject->Memory[iPos - 1]);
		}
		XXAPI void* PMMU_GetVal_Unsafe(PMMU_Object pObject, unsigned int iPos)
		{
			return pObject->Memory[iPos - 1];
		}
		
		// 设置成员指针
		XXAPI int PMMU_SetVal(PMMU_Object pObject, unsigned int iPos, void* pVal)
		{
			if ( iPos ) {
				iPos--;
				if ( iPos < pObject->Count ) {
					pObject->Memory[iPos] = pVal;
					return -1;
				}
			}
			return 0;
		}
		XXAPI void PMMU_SetVal_Unsafe(PMMU_Object pObject, unsigned int iPos, void* pVal)
		{
			pObject->Memory[iPos - 1] = pVal;
		}
		
		// 成员排序
		XXAPI int PMMU_Sort(PMMU_Object pObject, void* procCompar)
		{
			if ( pObject ) {
				qsort(pObject->Memory, pObject->Count, sizeof(void*), procCompar);
				return -1;
			} else {
				return 0;
			}
		}
		
	#endif





	/*
		Memory Buffer Management Unit [内存缓冲区管理单元]
	*/

	#ifdef MMU_USE_MBMU
		
		// 创建内存缓冲区管理器
		XXAPI MBMU_Object MBMU_Create(unsigned int iAllocLength, unsigned int iStep)
		{
			MBMU_Object ObjPtr = mmu_malloc(sizeof(MBMU_Struct));
			if ( ObjPtr ) {
				MBMU_Init(ObjPtr, iAllocLength, iStep);
			}
			return ObjPtr;
		}
		
		// 销毁内存缓冲区管理器
		XXAPI void MBMU_Destroy(MBMU_Object pObject)
		{
			if ( pObject ) {
				MBMU_Unit(pObject);
				mmu_free(pObject);
			}
		}
		
		// 初始化缓冲区管理器（对自维护结构体指针使用，和 MBMU_Create 功能类似）
		XXAPI void MBMU_Init(MBMU_Object pObject, unsigned int iAllocLength, unsigned int iStep)
		{
			pObject->Buffer = NULL;
			pObject->Length = 0;
			pObject->AllocLength = 0;
			pObject->AllocStep = iStep ? iStep : MBMU_AllocStep;
			if ( iAllocLength ) {
				MBMU_Malloc(pObject, iAllocLength);
			}
		}
		
		// 释放缓冲区管理器（对自维护结构体指针使用，和 MBMU_Destroy 功能类似）
		XXAPI void MBMU_Unit(MBMU_Object pObject)
		{
			if ( pObject->Buffer ) { mmu_free(pObject->Buffer); pObject->Buffer = NULL; }
			pObject->Length = 0;
			pObject->AllocLength = 0;
		}
		
		// 分配内存
		XXAPI int MBMU_Malloc(MBMU_Object pObject, unsigned int iCount)
		{
			if ( iCount > pObject->AllocLength ) {
				// 增量
				void* pNew = mmu_realloc(pObject->Buffer, iCount);
				if ( pNew ) {
					pObject->AllocLength = iCount;
					pObject->Buffer = pNew;
					return -1;
				}
			} else if ( iCount < pObject->AllocLength ) {
				// 裁剪
				void* pNew = mmu_realloc(pObject->Buffer, iCount);
				if ( pNew ) {
					pObject->AllocLength = iCount;
					pObject->Buffer = pNew;
					if ( iCount <= pObject->Length ) {
						// 需要裁剪数据
						pObject->Length = iCount;
					}
					return -1;
				}
			} else if ( iCount = 0 ) {
				// 清空
				MBMU_Unit(pObject);
				return -1;
			} else {
				// 不变
				return -1;
			}
			return 0;
		}
		
		// 中间添加数据（可以复制或者开辟新的数据区，不会自动将新开辟的数据区填充 \0）
		XXAPI int MBMU_Insert(MBMU_Object pObject, unsigned int iPos, void* pData, unsigned int iSize, unsigned int bStrMode)
		{
			// 长度为 0 时自动计算数据长度
			if ( iSize == 0 ) {
				if ( bStrMode == MBMU_ANSI ) {
					iSize = strlen(pData);
				} else if ( bStrMode == MBMU_UNICODE ) {
					iSize = wcslen(pData) * sizeof(wchar_t);
				} else if ( bStrMode == MBMU_UTF32 ) {
					// utf32 编码，没有获取长度的函数，先留空
				}
			}
			// 分配内存
			if ( (iPos + iSize + bStrMode) > pObject->AllocLength ) {
				if ( MBMU_Malloc(pObject, iPos + iSize + bStrMode + pObject->AllocStep) == 0 ) {
					return 0;
				}
			}
			// 复制数据
			if ( iSize ) {
				memcpy(&pObject->Buffer[iPos], pData, iSize);
				pObject->Length = iPos + iSize;
			}
			// 字符串模式自动添加 \0
			if ( bStrMode ) {
				for ( int i = 0; i < bStrMode; i++ ) {
					pObject->Buffer[pObject->Length + i] = 0;
				}
			}
			return -1;
		}
		
		// 末尾添加数据
		XXAPI int MBMU_Append(MBMU_Object pObject, void* pData, unsigned int iSize, unsigned int bStrMode)
		{
			return MBMU_Insert(pObject, pObject->Length, pData, iSize, bStrMode);
		}
		
	#endif





	/*
		Memory Management Unit 256
			固定 256 个数量的内存管理单元
			不提供结构体调用方式（MMU256 是基础内存管理单元，需要作为MM256阵列单元使用，结构体调用不具备实用性）
			首个内存数据必定为4字节对齐（如果管理的内存数据本身也是4字节对齐的，则所有内存单元都是4字节对齐的）
			提供内联的 Alloc 和 Free 函数以加快内存分配效率
	*/

	#ifdef MMU_USE_MMU256
		
		// 创建内存管理单元（iItemLength会自动增加4个字节用于确定内存位置和所属的管理器单元编号）
		XXAPI MMU256_Object MMU256_Create(unsigned int iItemLength)
		{
			iItemLength += sizeof(MMU_Value);
			#ifdef MMU_USE_RPMALLOC
				MMU256_Object objUnit = mmu_malloc(sizeof(MMU256_Struct) + (256 * iItemLength));
			#else
				MMU256_Object objUnit = mmu_malloc(sizeof(MMU256_Struct) + (256 * iItemLength) + 3);
			#endif
			if ( objUnit ) {
				// 处理内存对齐
				#ifdef MMU_USE_RPMALLOC
					objUnit->Memory = (char*)&objUnit[1];
				#else
					if ( (intptr_t)objUnit & 3 ) {
						char* pTemp = (char*)&objUnit[1];
						objUnit->Memory = &pTemp[(intptr_t)objUnit & 3];
					} else {
						objUnit->Memory = (char*)&objUnit[1];
					}
				#endif
				objUnit->ItemLength = iItemLength;
				objUnit->Count = 0;
				objUnit->FreeCount = 0;
				objUnit->FreeOffset = 0;
				objUnit->Flag = 0;
				return objUnit;
			}
			return NULL;
		}
		
		// 从内存管理单元中申请一个元素
		XXAPI void* MMU256_Alloc(MMU256_Object objUnit)
		{
			if ( objUnit && (objUnit->Count < 256) ) {
				return MMU256_Alloc_Inline(objUnit);
			}
			return NULL;
		}
		
		// 释放内存管理单元中某个元素
		XXAPI void MMU256_FreeIdx(MMU256_Object objUnit, unsigned char idx)
		{
			MMU256_FreeIdx_Inline(objUnit, idx);
		}
		XXAPI void MMU256_Free(MMU256_Object objUnit, void* obj)
		{
			MMU256_Free_Inline(objUnit, obj);
		}
		
	#endif





	/*
		Memory Management Unit 64K
			固定 65536 个数量的内存管理单元
			一次申请占用内存过多，通常用于内存占用换效率的场合
			不提供结构体调用方式（MMU64K 是基础内存管理单元，需要作为MM64K阵列单元使用，结构体调用不具备实用性）
			提供内联的 Alloc 和 Free 函数以加快内存分配效率
	*/

	#ifdef MMU_USE_MMU64K
		
		// 创建内存管理单元（iItemLength会自动增加4个字节用于确定内存位置和所属的管理器单元编号）
		XXAPI MMU64K_Object MMU64K_Create(unsigned int iItemLength)
		{
			iItemLength += sizeof(MMU_Value);
			#ifdef MMU_USE_RPMALLOC
				MMU64K_Object objUnit = mmu_malloc(sizeof(MMU64K_Struct) + (65536 * iItemLength));
			#else
				MMU64K_Object objUnit = mmu_malloc(sizeof(MMU64K_Struct) + (65536 * iItemLength) + 3);
			#endif
			if ( objUnit ) {
				// 处理内存对齐
				#ifdef MMU_USE_RPMALLOC
					objUnit->Memory = (char*)&objUnit[1];
				#else
					if ( (intptr_t)objUnit & 3 ) {
						char* pTemp = (char*)&objUnit[1];
						objUnit->Memory = &pTemp[(intptr_t)objUnit & 3];
					} else {
						objUnit->Memory = (char*)&objUnit[1];
					}
				#endif
				objUnit->ItemLength = iItemLength;
				objUnit->Count = 0;
				objUnit->FreeCount = 0;
				objUnit->FreeOffset = 0;
				objUnit->Flag = 0;
				return objUnit;
			}
			return NULL;
		}
		
		// 从内存管理单元中申请一个元素
		XXAPI void* MMU64K_Alloc(MMU64K_Object objUnit)
		{
			if ( objUnit && (objUnit->Count < 65536) ) {
				return MMU64K_Alloc_Inline(objUnit);
			}
			return NULL;
		}
		
		// 释放内存管理单元中某个元素
		XXAPI void MMU64K_FreeIdx(MMU64K_Object objUnit, unsigned short idx)
		{
			MMU64K_FreeIdx_Inline(objUnit, idx);
		}
		XXAPI void MMU64K_Free(MMU64K_Object objUnit, void* ptr)
		{
			MMU64K_Free_Inline(objUnit, ptr);
		}
		
	#endif





	/*
		Memory Management
			内存管理器（固定大小的内存池，使用 MMU256 加速分配和释放，因此申请步长为256）
	*/

	#ifdef MMU_USE_MM256
		
		// 创建内存管理器
		XXAPI MM256_Object MM256_Create(unsigned int iItemLength)
		{
			MM256_Object mm = mmu_malloc(sizeof(MM256_Struct));
			if ( mm ) {
				MM256_Init(mm, iItemLength);
			}
			return mm;
		}
		
		// 销毁内存管理器
		XXAPI void MM256_Destroy(MM256_Object objMM)
		{
			if ( objMM ) {
				MM256_Unit(objMM);
				mmu_free(objMM);
			}
		}
		
		// 初始化内存管理器（对自维护结构体指针使用，和 MM256_Create 功能类似）
		XXAPI void MM256_Init(MM256_Object objMM, unsigned int iItemLength)
		{
			objMM->ItemLength = iItemLength;
			PMMU_Init(&objMM->MMU);
			objMM->MMU.AllocStep = 64;
			objMM->OnError = NULL;
		}
		
		// 释放内存管理器（对自维护结构体指针使用，和 MM256_Destroy 功能类似）
		XXAPI void MM256_Unit(MM256_Object objMM)
		{
			for ( int i = 0; i < objMM->MMU.Count; i++ ) {
				MMU256_Object objMMU = objMM->MMU.Memory[i];
				if ( objMMU ) {
					MMU256_Destroy(objMMU);
				}
			}
			PMMU_Unit(&objMM->MMU);
		}
		
		// 从内存管理器中申请一块内存
		XXAPI void* MM256_Alloc(MM256_Object objMM)
		{
			// 获取一个空闲的内存管理器单元
			MMU256_Object objMMU = NULL;
			for ( int i = 0; i < objMM->MMU.Count; i++ ) {
				MMU256_Object objUnit = objMM->MMU.Memory[i];
				if ( objUnit && (objUnit->Count < 256 ) ) {
					objMMU = objUnit;
					break;
				}
			}
			// 如果没有空闲的内存管理器单元，则创建一个新的
			if ( objMMU == NULL ) {
				objMMU = MMU256_Create(objMM->ItemLength);
				if ( objMMU == NULL ) {
					// 无法创建内存管理器单元，报错处理
					if ( objMM->OnError ) {
						objMM->OnError(objMM, MMU_ERROR_CREATEMMU);
					}
				}
				unsigned int idx = PMMU_Add(&objMM->MMU, objMMU);
				if ( idx == 0 ) {
					// 无法将内存管理器单元添加到内存管理器阵列，报错处理
					if ( objMM->OnError ) {
						objMM->OnError(objMM, MMU_ERROR_ADDMMU);
					}
				}
				// 标记内存管理器单元的 Flag
				objMMU->Flag = MMU_FLAG_USE | ((idx - 1) << 8);
			}
			// 从内存管理器单元中申请内存块
			return MMU256_Alloc_Inline(objMMU);
		}
		
		// 将内存管理器申请的内存释放掉
		XXAPI void MM256_Free(MM256_Object objMM, void* ptr)
		{
			MMU_ValuePtr v = ptr - sizeof(MMU_Value);
			int iMMU = (v->ItemFlag & MMU_FLAG_MASK) >> 8;
			unsigned char idx = v->ItemFlag & 0xFF;
			// 调用对应 MMU 的释放函数
			MMU256_Object objMMU = objMM->MMU.Memory[iMMU];
			if ( objMMU ) {
				MMU256_FreeIdx_Inline(objMMU, idx);
				v->ItemFlag = 0;
				// 如果内存管理单元的所有数据都已释放，则回收它
				if ( objMMU->Count == 0 ) {
					MMU256_Destroy(objMMU);
					objMM->MMU.Memory[iMMU] = NULL;
				}
			} else {
				// 读取到的 MMU 对象为 NULL，报错处理
				if ( objMM->OnError ) {
					objMM->OnError(objMM, MMU_ERROR_GETMMU);
				}
			}
		}
		
		// 将一块内存标记为使用中
		XXAPI void MM256_GC_Mark(void* ptr)
		{
			MM_GC_Mark_Inline(ptr);
		}
		
		// 进行一轮GC，将未标记为使用中的内存全部回收
		XXAPI void MM256_GC(MM256_Object objMM, int bFreeMark)
		{
			// 遍历所有内存管理单元，每一块已申请的内存
			for ( int i = 0; i < objMM->MMU.Count; i++ ) {
				MMU256_Object objUnit = objMM->MMU.Memory[i];
				if ( objUnit && (objUnit->Count > 0 ) ) {
					// 遍历所有已申请的内存
					if ( bFreeMark ) {
						// 被标记的内存将被回收
						for ( int idx = 0; idx < 256; idx++ ) {
							MMU_ValuePtr v = (MMU_ValuePtr)&(objUnit->Memory[objUnit->ItemLength * idx]);
							if ( v->ItemFlag & MMU_FLAG_USE ) {
								if ( v->ItemFlag & MMU_FLAG_GC ) {
									MMU256_FreeIdx_Inline(objUnit, idx);
									v->ItemFlag = 0;
								}
							}
						}
					} else {
						// 未被标记的内存将被回收
						for ( int idx = 0; idx < 256; idx++ ) {
							MMU_ValuePtr v = (MMU_ValuePtr)&(objUnit->Memory[objUnit->ItemLength * idx]);
							if ( v->ItemFlag & MMU_FLAG_USE ) {
								if ( v->ItemFlag & MMU_FLAG_GC ) {
									v->ItemFlag &= ~MMU_FLAG_GC;
								} else {
									MMU256_FreeIdx_Inline(objUnit, idx);
									v->ItemFlag = 0;
								}
							}
						}
					}
				}
			}
			// 遍历所有内存管理单元，如果对应的内存管理单元中已经已经没有在使用的内存了，则销毁它
			for ( int i = 0; i < objMM->MMU.Count; i++ ) {
				MMU256_Object objUnit = objMM->MMU.Memory[i];
				if ( objUnit && (objUnit->Count == 0 ) ) {
					MMU256_Destroy(objUnit);
					objMM->MMU.Memory[i] = NULL;
				}
			}
		}
		
	#endif





	/*
		Memory Management
			内存管理器（固定大小的内存池，使用 MMU64K 加速分配和释放，因此申请步长为 65536）
	*/

	#ifdef MMU_USE_MM64K
		
		// 创建内存管理器
		XXAPI MM64K_Object MM64K_Create(unsigned int iItemLength)
		{
			MM64K_Object mm = mmu_malloc(sizeof(MM64K_Struct));
			if ( mm ) {
				MM64K_Init(mm, iItemLength);
			}
			return mm;
		}
		
		// 销毁内存管理器
		XXAPI void MM64K_Destroy(MM64K_Object objMM)
		{
			if ( objMM ) {
				MM64K_Unit(objMM);
				mmu_free(objMM);
			}
		}
		
		// 初始化内存管理器（对自维护结构体指针使用，和 MM64K_Create 功能类似）
		XXAPI void MM64K_Init(MM64K_Object objMM, unsigned int iItemLength)
		{
			objMM->ItemLength = iItemLength;
			PMMU_Init(&objMM->MMU);
			objMM->MMU.AllocStep = 64;
			objMM->OnError = NULL;
		}
		
		// 释放内存管理器（对自维护结构体指针使用，和 MM64K_Destroy 功能类似）
		XXAPI void MM64K_Unit(MM64K_Object objMM)
		{
			for ( int i = 0; i < objMM->MMU.Count; i++ ) {
				MMU64K_Object objMMU = objMM->MMU.Memory[i];
				if ( objMMU ) {
					MMU64K_Destroy(objMMU);
				}
			}
			PMMU_Unit(&objMM->MMU);
		}
		
		// 从内存管理器中申请一块内存
		XXAPI void* MM64K_Alloc(MM64K_Object objMM)
		{
			// 获取一个空闲的内存管理器单元
			MMU64K_Object objMMU = NULL;
			for ( int i = 0; i < objMM->MMU.Count; i++ ) {
				MMU64K_Object objUnit = objMM->MMU.Memory[i];
				if ( objUnit && (objUnit->Count < 65536 ) ) {
					objMMU = objUnit;
					break;
				}
			}
			// 如果没有空闲的内存管理器单元，则创建一个新的
			if ( objMMU == NULL ) {
				objMMU = MMU64K_Create(objMM->ItemLength);
				if ( objMMU == NULL ) {
					// 无法创建内存管理器单元，报错处理
					if ( objMM->OnError ) {
						objMM->OnError(objMM, MMU_ERROR_CREATEMMU);
					}
				}
				unsigned int idx = PMMU_Add(&objMM->MMU, objMMU);
				if ( idx == 0 ) {
					// 无法将内存管理器单元添加到内存管理器阵列，报错处理
					if ( objMM->OnError ) {
						objMM->OnError(objMM, MMU_ERROR_ADDMMU);
					}
				}
				// 标记内存管理器单元的 Flag
				objMMU->Flag = MMU_FLAG_USE | ((idx - 1) << 16);
			}
			// 从内存管理器单元中申请内存块
			return MMU64K_Alloc_Inline(objMMU);
		}
		
		// 将内存管理器申请的内存释放掉
		XXAPI void MM64K_Free(MM64K_Object objMM, void* ptr)
		{
			MMU_ValuePtr v = ptr - sizeof(MMU_Value);
			int iMMU = (v->ItemFlag & MMU_FLAG_MASK) >> 16;
			unsigned char idx = v->ItemFlag & 0xFFFF;
			// 调用对应 MMU 的释放函数
			MMU64K_Object objMMU = objMM->MMU.Memory[iMMU];
			if ( objMMU ) {
				MMU64K_FreeIdx_Inline(objMMU, idx);
				v->ItemFlag = 0;
				// 如果内存管理单元的所有数据都已释放，则回收它
				if ( objMMU->Count == 0 ) {
					MMU64K_Destroy(objMMU);
					objMM->MMU.Memory[iMMU] = NULL;
				}
			} else {
				// 读取到的 MMU 对象为 NULL，报错处理
				if ( objMM->OnError ) {
					objMM->OnError(objMM, MMU_ERROR_GETMMU);
				}
			}
		}
		
		// 将一块内存标记为使用中
		XXAPI void MM64K_GC_Mark(void* ptr)
		{
			MM_GC_Mark_Inline(ptr);
		}
		
		// 进行一轮GC，将未标记为使用中的内存全部回收
		XXAPI void MM64K_GC(MM64K_Object objMM, int bFreeMark)
		{
			// 遍历所有内存管理单元，每一块已申请的内存
			for ( int i = 0; i < objMM->MMU.Count; i++ ) {
				MMU64K_Object objUnit = objMM->MMU.Memory[i];
				if ( objUnit && (objUnit->Count > 0 ) ) {
					// 遍历所有已申请的内存
					if ( bFreeMark ) {
						// 被标记的内存将被回收
						for ( int idx = 0; idx < 65536; idx++ ) {
							MMU_ValuePtr v = (MMU_ValuePtr)&(objUnit->Memory[objUnit->ItemLength * idx]);
							if ( v->ItemFlag & MMU_FLAG_USE ) {
								if ( v->ItemFlag & MMU_FLAG_GC ) {
									MMU64K_FreeIdx_Inline(objUnit, idx);
									v->ItemFlag = 0;
								}
							}
						}
					} else {
						// 未被标记的内存将被回收
						for ( int idx = 0; idx < 65536; idx++ ) {
							MMU_ValuePtr v = (MMU_ValuePtr)&(objUnit->Memory[objUnit->ItemLength * idx]);
							if ( v->ItemFlag & MMU_FLAG_USE ) {
								if ( v->ItemFlag & MMU_FLAG_GC ) {
									v->ItemFlag &= ~MMU_FLAG_GC;
								} else {
									MMU64K_FreeIdx_Inline(objUnit, idx);
									v->ItemFlag = 0;
								}
							}
						}
					}
				}
			}
			// 遍历所有内存管理单元，如果对应的内存管理单元中已经已经没有在使用的内存了，则销毁它
			for ( int i = 0; i < objMM->MMU.Count; i++ ) {
				MMU64K_Object objUnit = objMM->MMU.Memory[i];
				if ( objUnit && (objUnit->Count == 0 ) ) {
					MMU64K_Destroy(objUnit);
					objMM->MMU.Memory[i] = NULL;
				}
			}
		}
		
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
		
		// 创建结构体静态栈
		XXAPI SSSTK_Object SSSTK_Create(unsigned int iMaxCount, unsigned int iItemLength)
		{
			SSSTK_Object objSTK = mmu_malloc(sizeof(SSSTK_Struct) + (iMaxCount * iItemLength));
			if ( objSTK ) {
				objSTK->Memory = (void*)&objSTK[1];
				objSTK->ItemLength = iItemLength;
				objSTK->MaxCount = iMaxCount;
				objSTK->Count = 0;
			}
			return objSTK;
		}
		
		// 压栈
		XXAPI void* SSSTK_Push(SSSTK_Object objSTK)
		{
			if ( objSTK->Count < objSTK->MaxCount ) {
				objSTK->Count++;
				return &objSTK->Memory[(objSTK->Count - 1) * objSTK->ItemLength];
			}
			return NULL;
		}
		XXAPI unsigned int SSSTK_PushData(SSSTK_Object objSTK, void* pData)
		{
			if ( objSTK->Count < objSTK->MaxCount ) {
				memcpy(&objSTK->Memory[objSTK->Count * objSTK->ItemLength], pData, objSTK->ItemLength);
				objSTK->Count++;
				return objSTK->Count;
			}
			return 0;
		}
		
		// 出栈
		XXAPI void* SSSTK_Pop(SSSTK_Object objSTK)
		{
			if ( objSTK->Count == 0 ) {
				return NULL;
			}
			objSTK->Count--;
			return &objSTK->Memory[objSTK->Count * objSTK->ItemLength];
		}
		
		// 获取栈顶对象
		XXAPI void* SSSTK_Top(SSSTK_Object objSTK)
		{
			if ( objSTK->Count == 0 ) {
				return NULL;
			}
			return &objSTK->Memory[(objSTK->Count - 1) * objSTK->ItemLength];
		}
		
		// 获取任意位置对象
		XXAPI void* SSSTK_GetPos(SSSTK_Object objSTK, unsigned int iPos)
		{
			if ( iPos > 0 ) {
				iPos--;
				if ( iPos < objSTK->Count ) {
					return &objSTK->Memory[iPos * objSTK->ItemLength];
				}
			}
			return NULL;
		}
		XXAPI void* SSSTK_GetPos_Unsafe(SSSTK_Object objSTK, unsigned int iPos)
		{
			return &objSTK->Memory[(iPos - 1) * objSTK->ItemLength];
		}
		
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
		
		// 创建指针静态栈
		XXAPI PSSTK_Object PSSTK_Create(unsigned int iMaxCount)
		{
			PSSTK_Object objSTK = mmu_malloc(sizeof(PSSTK_Struct) + (iMaxCount * sizeof(void*)));
			if ( objSTK ) {
				objSTK->Memory = (void*)&objSTK[1];
				objSTK->MaxCount = iMaxCount;
				objSTK->Count = 0;
			}
			return objSTK;
		}
		
		// 压栈
		XXAPI unsigned int PSSTK_Push(PSSTK_Object objSTK, void* ptr)
		{
			if ( objSTK->Count < objSTK->MaxCount ) {
				objSTK->Memory[objSTK->Count] = ptr;
				objSTK->Count++;
				return objSTK->Count;
			}
			return 0;
		}
		
		// 出栈
		XXAPI void* PSSTK_Pop(PSSTK_Object objSTK)
		{
			if ( objSTK->Count == 0 ) {
				return NULL;
			}
			objSTK->Count--;
			return objSTK->Memory[objSTK->Count];
		}
		
		// 获取栈顶指针
		XXAPI void* PSSTK_Top(PSSTK_Object objSTK)
		{
			if ( objSTK->Count == 0 ) {
				return NULL;
			}
			return objSTK->Memory[objSTK->Count - 1];
		}
		
		// 获取任意位置指针
		XXAPI void* PSSTK_GetPos(PSSTK_Object objSTK, unsigned int iPos)
		{
			if ( iPos > 0 ) {
				iPos--;
				if ( iPos < objSTK->Count ) {
					return objSTK->Memory[iPos];
				}
			}
			return NULL;
		}
		XXAPI void* PSSTK_GetPos_Unsafe(PSSTK_Object objSTK, unsigned int iPos)
		{
			return objSTK->Memory[iPos - 1];
		}
		
	#endif





	/*
		Struct Dynamic Stack [结构体动态栈，结构体内存256个递增，栈最大深度不固定]
			成员编号规则（0为不存在的成员编号）：
				┌──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬────┐
				│01│02│03│04│05│06│07│08│09│10│11│12│ .. │
				└──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴────┘
	*/

	#ifdef MMU_USE_SDSTK
		
		// 创建结构体动态栈
		XXAPI SDSTK_Object SDSTK_Create(unsigned int iItemLength)
		{
			SDSTK_Object objSTK = mmu_malloc(sizeof(SDSTK_Struct));
			if ( objSTK ) {
				SDSTK_Init(objSTK, iItemLength);
			}
			return objSTK;
		}
		
		// 销毁结构体动态栈
		XXAPI void SDSTK_Destroy(SDSTK_Object objSTK)
		{
			if ( objSTK ) {
				SDSTK_Unit(objSTK);
				mmu_free(objSTK);
			}
		}
		
		// 初始化结构体动态栈（对自维护结构体指针使用，和 SDSTK_Create 功能类似）
		XXAPI void SDSTK_Init(SDSTK_Object objSTK, unsigned int iItemLength)
		{
			objSTK->ItemLength = iItemLength;
			objSTK->Count = 0;
			PMMU_Init(&objSTK->MMU);
			objSTK->MMU.AllocStep = 64;
			objSTK->OnError = NULL;
		}
		
		// 释放结构体动态栈（对自维护结构体指针使用，和 SDSTK_Create 功能类似）
		XXAPI void SDSTK_Unit(SDSTK_Object objSTK)
		{
			objSTK->Count = 0;
			PMMU_Unit(&objSTK->MMU);
		}
		
		// 压栈
		XXAPI void* SDSTK_Push(SDSTK_Object objSTK)
		{
			char* pBlock = NULL;
			unsigned int iBlock = objSTK->Count >> 8;
			if ( objSTK->MMU.Count > iBlock ) {
				// 直接获取现有的内存块
				pBlock = objSTK->MMU.Memory[iBlock];
			} else {
				// 需要创建新的内存块
				pBlock = mmu_malloc(objSTK->ItemLength * 256);
				// !!! 错误处理 !!! 内存申请失败
				if ( pBlock == NULL ) {
					if ( objSTK->OnError ) {
						objSTK->OnError(objSTK, MMU_ERROR_ALLOC);
					}
					return NULL;
				}
				unsigned int idx = PMMU_Append(&objSTK->MMU, pBlock);
				// !!! 错误处理 !!! 无法将内存添加到内存阵列
				if ( idx == 0 ) {
					if ( objSTK->OnError ) {
						objSTK->OnError(objSTK, MMU_ERROR_ADDMMU);
					}
					return NULL;
				}
			}
			// 数据压栈
			unsigned int iPos = objSTK->Count & 0xFF;
			objSTK->Count++;
			return &pBlock[iPos * objSTK->ItemLength];
		}
		XXAPI unsigned int SDSTK_PushData(SDSTK_Object objSTK, void* pData)
		{
			void* ptr = SDSTK_Push(objSTK);
			memcpy(ptr, pData, objSTK->ItemLength);
			return objSTK->Count;
		}
		
		// 出栈
		XXAPI void* SDSTK_Pop(SDSTK_Object objSTK)
		{
			void* pRet = SDSTK_Top(objSTK);
			objSTK->Count--;
			// 延迟释放内存块（最大内存长度超过已使用内存长度 256 + 32 个结构体，释放掉内存块）
			if ( (objSTK->MMU.Count << 8) > (objSTK->Count + 288) ) {
				objSTK->MMU.Count--;
				mmu_free(objSTK->MMU.Memory[objSTK->MMU.Count]);
			}
			return pRet;
		}
		
		// 获取栈顶对象
		XXAPI void* SDSTK_Top(SDSTK_Object objSTK)
		{
			return SDSTK_GetPos_Unsafe(objSTK, objSTK->Count);
		}
		
		// 获取任意位置对象
		XXAPI void* SDSTK_GetPos(SDSTK_Object objSTK, unsigned int iPos)
		{
			if ( iPos > 0 ) {
				iPos--;
				if ( iPos < objSTK->Count ) {
					char* pBlock = objSTK->MMU.Memory[iPos >> 8];
					return &pBlock[(iPos & 0xFF) * objSTK->ItemLength];
				}
			}
			return NULL;
		}
		XXAPI void* SDSTK_GetPos_Unsafe(SDSTK_Object objSTK, unsigned int iPos)
		{
			iPos--;
			char* pBlock = objSTK->MMU.Memory[iPos >> 8];
			return &pBlock[(iPos & 0xFF) * objSTK->ItemLength];
		}
		
	#endif





	/*
		Point Dynamic Stack [指针动态栈，结构体内存256个递增，栈最大深度不固定]
			成员编号规则（0为不存在的成员编号）：
				┌──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬────┐
				│01│02│03│04│05│06│07│08│09│10│11│12│ .. │
				└──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴────┘
	*/

	#ifdef MMU_USE_PDSTK
		
		// 创建指针动态栈
		XXAPI PDSTK_Object PDSTK_Create()
		{
			PDSTK_Object objSTK = mmu_malloc(sizeof(PDSTK_Struct));
			if ( objSTK ) {
				PDSTK_Init(objSTK);
			}
			return objSTK;
		}
		
		// 销毁指针动态栈
		XXAPI void PDSTK_Destroy(PDSTK_Object objSTK)
		{
			if ( objSTK ) {
				PDSTK_Unit(objSTK);
				mmu_free(objSTK);
			}
		}
		
		// 初始化指针动态栈（对自维护结构体指针使用，和 PDSTK_Create 功能类似）
		XXAPI void PDSTK_Init(PDSTK_Object objSTK)
		{
			objSTK->Count = 0;
			PMMU_Init(&objSTK->MMU);
			objSTK->MMU.AllocStep = 64;
			objSTK->OnError = NULL;
		}
		
		// 释放指针动态栈（对自维护结构体指针使用，和 PDSTK_Create 功能类似）
		XXAPI void PDSTK_Unit(PDSTK_Object objSTK)
		{
			objSTK->Count = 0;
			PMMU_Unit(&objSTK->MMU);
		}
		
		// 压栈
		XXAPI unsigned int PDSTK_Push(PDSTK_Object objSTK, void* ptr)
		{
			void** pBlock = NULL;
			unsigned int iBlock = objSTK->Count >> 8;
			if ( objSTK->MMU.Count > iBlock ) {
				// 直接获取现有的内存块
				pBlock = objSTK->MMU.Memory[iBlock];
			} else {
				// 需要创建新的内存块
				pBlock = mmu_malloc(sizeof(void*) * 256);
				// !!! 错误处理 !!! 内存申请失败
				if ( pBlock == NULL ) {
					if ( objSTK->OnError ) {
						objSTK->OnError(objSTK, MMU_ERROR_ALLOC);
					}
					return 0;
				}
				unsigned int idx = PMMU_Append(&objSTK->MMU, pBlock);
				// !!! 错误处理 !!! 无法将内存添加到内存阵列
				if ( idx == 0 ) {
					if ( objSTK->OnError ) {
						objSTK->OnError(objSTK, MMU_ERROR_ADDMMU);
					}
					return 0;
				}
			}
			// 数据压栈
			pBlock[objSTK->Count & 0xFF] = ptr;
			objSTK->Count++;
			return objSTK->Count;
		}
		
		// 出栈
		XXAPI void* PDSTK_Pop(PDSTK_Object objSTK)
		{
			void* pRet = PDSTK_Top(objSTK);
			objSTK->Count--;
			// 延迟释放内存块（最大内存长度超过已使用内存长度 256 + 32 个结构体，释放掉内存块）
			if ( (objSTK->MMU.Count << 8) > (objSTK->Count + 288) ) {
				objSTK->MMU.Count--;
				mmu_free(objSTK->MMU.Memory[objSTK->MMU.Count]);
			}
			return pRet;
		}
		
		// 获取栈顶指针
		XXAPI void* PDSTK_Top(PDSTK_Object objSTK)
		{
			return PDSTK_GetPos_Unsafe(objSTK, objSTK->Count);
		}
		
		// 获取任意位置指针
		XXAPI void* PDSTK_GetPos(PDSTK_Object objSTK, unsigned int iPos)
		{
			if ( iPos > 0 ) {
				iPos--;
				if ( iPos < objSTK->Count ) {
					void** pBlock = objSTK->MMU.Memory[iPos >> 8];
					return pBlock[iPos & 0xFF];
				}
			}
			return NULL;
		}
		XXAPI void* PDSTK_GetPos_Unsafe(PDSTK_Object objSTK, unsigned int iPos)
		{
			iPos--;
			void** pBlock = objSTK->MMU.Memory[iPos >> 8];
			return pBlock[iPos & 0xFF];
		}
		
	#endif





	/*
		Linked List [双向链表，使用 MM256 管理内存]
	*/

	#ifdef MMU_USE_LLIST
		
		// 创建链表
		XXAPI LList_Object LList_Create(unsigned int iItemLength)
		{
			LList_Object objLL = mmu_malloc(sizeof(LList_Struct));
			if ( objLL ) {
				LList_Init(objLL, iItemLength);
			}
			return objLL;
		}
		
		// 销毁链表
		XXAPI void LList_Destroy(LList_Object objLL)
		{
			if ( objLL ) {
				LList_Unit(objLL);
				mmu_free(objLL);
			}
		}
		
		// 初始化链表（对自维护结构体指针使用，和 LList_Create 功能类似）
		XXAPI void LList_Init(LList_Object objLL, unsigned int iItemLength)
		{
			objLL->FirstNode = NULL;
			objLL->LastNode = NULL;
			objLL->Count = 0;
			MM256_Init(&objLL->objMM, iItemLength);
		}
		
		// 释放链表（对自维护结构体指针使用，和 LList_Destroy 功能类似）
		XXAPI void LList_Unit(LList_Object objLL)
		{
			objLL->FirstNode = NULL;
			objLL->LastNode = NULL;
			objLL->Count = 0;
			MM256_Unit(&objLL->objMM);
		}
		
		// 节点前插入 (objNode为空则插入到FirstNode之前)
		XXAPI LList_NodeBase* LList_InsertPrev(LList_Object objLL, LList_NodeBase* objNode)
		{
			LList_NodeBase* objNewNode = MM256_Alloc(&objLL->objMM);
			if ( objNewNode ) {
				if ( objNode ) {
					// 有参考节点（插入到参考节点前面）
					if ( objNode->Prev ) {
						objNode->Prev->Next = objNewNode;
					}
					objNewNode->Prev = objNode->Prev;
					objNewNode->Next = objNode;
					objNode->Prev = objNewNode;
				} else {
					// 无参考节点（插入到链表最前）
					if ( objLL->FirstNode ) {
						objNewNode->Prev = NULL;
						objNewNode->Next = objLL->FirstNode;
						objLL->FirstNode->Prev = objNewNode;
						objLL->FirstNode = objNewNode;
					} else {
						// 空链表，创建默认节点
						objNewNode->Prev = NULL;
						objNewNode->Next = NULL;
						objLL->FirstNode = objNewNode;
						objLL->LastNode = objNewNode;
					}
				}
				objLL->Count++;
			}
			return objNewNode;
		}
		
		// 节点后插入 (objNode为空则插入到LastNode之后)
		XXAPI LList_NodeBase* LList_InsertNext(LList_Object objLL, LList_NodeBase* objNode)
		{
			LList_NodeBase* objNewNode = MM256_Alloc(&objLL->objMM);
			if ( objNewNode ) {
				if ( objNode ) {
					// 有参考节点（插入到参考节点后面）
					if ( objNode->Next ) {
						objNode->Next->Prev = objNewNode;
					}
					objNewNode->Prev = objNode;
					objNewNode->Next = objNode->Next;
					objNode->Next = objNewNode;
				} else {
					// 无参考节点（插入到链表末尾）
					if ( objLL->LastNode ) {
						objNewNode->Prev = objLL->LastNode;
						objNewNode->Next = NULL;
						objLL->LastNode->Next = objNewNode;
						objLL->LastNode = objNewNode;
					} else {
						// 空链表，创建默认节点
						objNewNode->Prev = NULL;
						objNewNode->Next = NULL;
						objLL->FirstNode = objNewNode;
						objLL->LastNode = objNewNode;
					}
				}
				objLL->Count++;
			}
			return objNewNode;
		}
		
		// 删除节点
		XXAPI LList_NodeBase* LList_Remove(LList_Object objLL, LList_NodeBase* objNode, int bRetPrev)
		{
			if ( objNode ) {
				LList_NodeBase* pNext = objNode->Next;
				LList_NodeBase* pPrev = objNode->Prev;
				if ( pPrev ) {
					pPrev->Next = pNext;
				} else {
					objLL->FirstNode = pNext;
				}
				if ( pNext ) {
					pNext->Prev = pPrev;
				} else {
					objLL->LastNode = pPrev;
				}
				MM256_Free(&objLL->objMM, objNode);
				objLL->Count--;
				if ( bRetPrev ) {
					return pPrev;
				} else {
					return pNext;
				}
			}
			return NULL;
		}
		
	#endif





	/*
		AVLTree
	*/

	#ifdef MMU_USE_AVLTREE
		
		// 平衡二叉树（内部函数）
		static inline void AVLTree_Rebalance(AVLTree_NodeBase*** ancestors, int count)
		{
			while ( count > 0 ) {
				AVLTree_NodeBase** ppNode = ancestors[--count];							// 指向当前子树根节点的指针地址
				AVLTree_NodeBase* pNode = *ppNode;										// 指向当前子树的根节点
				AVLTree_NodeBase* leftp = pNode->left;									// 指向左子树的根节点
				AVLTree_NodeBase* rightp = pNode->right;								// 指向右子树的根节点
				int lefth = (leftp != NULL) ? leftp->height : 0;						// 左子树的高度
				int righth = (rightp != NULL) ? rightp->height : 0;						// 右子树的高度
				// 找到当前根节点及其两个子树。通过构造，我们知道它们都符合AVL平衡规则
				if ( righth - lefth < -1 ) {
					/*
					 *         *
					 *       /   \
					 *    n+2      n
					 *
					 * 当前子树左侧过高，违反了平衡规则。根据左子树的配置，我们必须使用两种不同的再平衡方法之一。
					 * 请注意，left p不能为NULL，否则我们不会传递给它
					 */
					AVLTree_NodeBase* leftleftp = leftp->left;							// 指向左-左子树的根
					AVLTree_NodeBase* leftrightp = leftp->right;						// 指向左-右子树的根
					int leftrighth = (leftrightp != NULL) ? leftrightp->height : 0;		// 左右子树高度
					if ( (leftleftp != NULL) && (leftleftp->height >= leftrighth) ) {
						/*
						 *            <D>                     <B>
						 *             *                    n+2|n+3
						 *           /   \                   /   \
						 *        <B>     <E>    ---->    <A>     <D>
						 *        n+2      n              n+1   n+1|n+2
						 *       /   \                           /   \
						 *    <A>     <C>                     <C>     <E>
						 *    n+1    n|n+1                   n|n+1     n
						 */
						pNode->left = leftrightp;										// D.left = C
						pNode->height = leftrighth + 1;
						leftp->right = pNode;											// B.right = D
						leftp->height = leftrighth + 2;
						*ppNode = leftp;												// B 成为 root
					} else {
						/*
						 *           <F>
						 *            *
						 *          /   \                        <D>
						 *       <B>     <G>                     n+2
						 *       n+2      n                     /   \
						 *      /   \           ---->        <B>     <F>
						 *   <A>     <D>                     n+1     n+1
						 *    n      n+1                    /  \     /  \
						 *          /   \                <A>   <C> <E>   <G>
						 *       <C>     <E>              n  n|n-1 n|n-1  n
						 *      n|n-1   n|n-1
						 *
						 * 我们可以假设left-rightp不是NULL，因为我们希望leftp和rightp符合AVL平衡规则。
						 * 请注意，如果这个假设是错误的，算法将在这里崩溃。
						 */
						leftp->right = leftrightp->left;								// B.right = C
						leftp->height = leftrighth;
						pNode->left = leftrightp->right;								// F.left = E
						pNode->height = leftrighth;
						leftrightp->left = leftp;										// D.left = B
						leftrightp->right = pNode;										// D.right = F
						leftrightp->height = leftrighth + 1;
						*ppNode = leftrightp;											// D 成为 root
					}
				} else if ( righth - lefth > 1 ) {
					/*
					 *        *
					 *      /   \
					 *    n      n+2
					 * 
					 * 当前子树在右侧过高，违反了平衡规则。这与前一种情况完全对称。我们必须根据右子树的配置使用两种不同的再平衡方法之一。
					 * 请注意，rightp不能为NULL，否则我们不会传递给它
					 */
					 AVLTree_NodeBase* rightleftp = rightp->left;						// 指向右-左子树的根
					 AVLTree_NodeBase* rightrightp = rightp->right;						// 指向右-右子树的根
					 int rightlefth = (rightleftp != NULL) ? rightleftp->height : 0;	// 右左子树高度
					 if ( (rightrightp != NULL) && (rightrightp->height >= rightlefth) ) {
						/*        <B>                             <D>
						 *         *                            n+2|n+3
						 *       /   \                           /   \
						 *    <A>     <D>        ---->        <B>     <E>
						 *     n      n+2                   n+1|n+2   n+1
						 *           /   \                   /   \
						 *        <C>     <E>             <A>     <C>
						 *       n|n+1    n+1              n     n|n+1
						 */
						pNode->right = rightleftp;										// B.right = C
						pNode->height = rightlefth + 1;
						rightp->left = pNode;											// D.left = B
						rightp->height = rightlefth + 2;
						*ppNode = rightp;												// D 成为 root
					} else {
						/*        <B>
						 *         *
						 *       /   \                            <D>
						 *    <A>     <F>                         n+2
						 *     n      n+2                        /   \
						 *           /   \       ---->        <B>     <F>
						 *        <D>     <G>                 n+1     n+1
						 *        n+1      n                 /  \     /  \
						 *       /   \                    <A>   <C> <E>   <G>
						 *    <C>     <E>                  n  n|n-1 n|n-1  n
						 *   n|n-1   n|n-1
						 *
						 * 我们可以假设right-leftp不为NULL，因为我们期望left p和right p符合AVL平衡规则
						 * 请注意，如果这个假设是错误的，算法将在这里崩溃
						 */
						pNode->right = rightleftp->left;								// B.right = C
						pNode->height = rightlefth;
						rightp->left = rightleftp->right;								// F.left = E
						rightp->height = rightlefth;
						rightleftp->left = pNode;										// D.left = B
						rightleftp->right = rightp;										// D.right = F
						rightleftp->height = rightlefth + 1;
						*ppNode = rightleftp;											// D 成为 root
					}
				} else {
					/*
					 * 无需重新平衡，只需设置树的高度
					 * 如果当前子树的高度没有改变，我们可以在这里停下来
					 * 因为我们知道我们没有违反祖先的AVL平衡规则。
					 */
					int height = ((righth > lefth) ? righth : lefth) + 1;
					if ( pNode->height == height ) {
						break;
					}
					pNode->height = height;
				}
			}
		}
		
		// 创建 AVLTree
		XXAPI AVLTree_Object AVLTree_Create(unsigned int iItemLength, AVLTree_CompProc procComp)
		{
			AVLTree_Object objAVLT = mmu_malloc(sizeof(AVLTree_Struct));
			if ( objAVLT ) {
				AVLTree_Init(objAVLT, iItemLength, procComp);
			}
			return objAVLT;
		}
		
		// 销毁 AVLTree
		XXAPI void AVLTree_Destroy(AVLTree_Object objAVLT)
		{
			if ( objAVLT ) {
				AVLTree_Unit(objAVLT);
				mmu_free(objAVLT);
			}
		}
		
		// 初始化 AVLTree（对自维护结构体指针使用，和 AVLTree_Create 功能类似）
		XXAPI void AVLTree_Init(AVLTree_Object objAVLT, unsigned int iItemLength, AVLTree_CompProc procComp)
		{
			objAVLT->RootNode = NULL;
			objAVLT->CompProc = procComp;
			objAVLT->Count = 0;
			MM256_Init(&objAVLT->objMM, iItemLength + sizeof(AVLTree_NodeBase));
		}
		
		// 释放 AVLTree（对自维护结构体指针使用，和 AVLTree_Destroy 功能类似）
		XXAPI void AVLTree_Unit(AVLTree_Object objAVLT)
		{
			objAVLT->RootNode = NULL;
			objAVLT->Count = 0;
			MM256_Unit(&objAVLT->objMM);
		}
		
		// 向 AVLTree 中插入节点，返回数据段指针（如果值已经存在，则会返回已存在的数据段指针）
		XXAPI void* AVLTree_AddNode(AVLTree_Object objAVLT, void* pKey, int* bNew)
		{
			// 初始化数据
			AVLTree_NodeBase** ppNode = &objAVLT->RootNode;		// 到当前节点的指针
			AVLTree_NodeBase** ancestor[AVLTree_MAX_HEIGHT];	// 上层节点列表
			int ancestorCount = 0;								// 上层节点数量
			// 找到要添加新节点的叶子节点
			while ( ancestorCount < AVLTree_MAX_HEIGHT ) {
				AVLTree_NodeBase* pNode = *ppNode;				// 指向当前节点的指针
				if ( pNode == NULL ) { break; }					// 可以在这里插入一个叶子节点
				ancestor[ancestorCount++] = ppNode;
				int delta = objAVLT->CompProc(&pNode[1], pKey);
				if ( delta < 0 ) {
					ppNode = &(pNode->left);
				} else if ( delta > 0 ) {
					ppNode = &(pNode->right);
				} else {
					if ( bNew ) { *bNew = 0; }
					return &pNode[1];
				}
			}
			if ( ancestorCount == AVLTree_MAX_HEIGHT ) { return NULL; }
			// 初始化 pNewNode
			AVLTree_NodeBase* pNewNode = MM256_Alloc(&objAVLT->objMM);
			if ( pNewNode == NULL ) { return NULL; }
			pNewNode->left = NULL;
			pNewNode->right = NULL;
			pNewNode->height = 1;
			if ( bNew ) { *bNew = -1; }
			*ppNode = pNewNode;
			// 平衡二叉树
			AVLTree_Rebalance(ancestor, ancestorCount);
			// 返回节点指针
			objAVLT->Count++;
			return &pNewNode[1];
		}
		
		// 向 AVLTree 中插入节点（值必须不存在，如果值已经存在，则返回NULL）
		XXAPI void* AVLTree_Insert(AVLTree_Object objAVLT, void* pKey)
		{
			// 初始化数据
			AVLTree_NodeBase** ppNode = &objAVLT->RootNode;
			AVLTree_NodeBase** ancestor[AVLTree_MAX_HEIGHT];	// 上层节点列表
			int ancestorCount = 0;								// 上层节点数量
			// 找到要添加新节点的叶子节点
			while ( ancestorCount < AVLTree_MAX_HEIGHT ) {
				AVLTree_NodeBase* pNode = *ppNode;
				if ( pNode == NULL ) { break; }					// 可以在这里插入节点
				ancestor[ancestorCount++] = ppNode;
				int delta = objAVLT->CompProc(&pNode[1], pKey);
				if ( delta < 0 ) {
					ppNode = &(pNode->left);
				} else if ( delta > 0 ) {
					ppNode = &(pNode->right);
				} else {
					return NULL;
				}
			}
			if ( ancestorCount == AVLTree_MAX_HEIGHT ) { return NULL; }
			// 初始化 pNewNode
			AVLTree_NodeBase* pNewNode = MM256_Alloc(&objAVLT->objMM);
			if ( pNewNode == NULL ) { return NULL; }
			pNewNode->left = NULL;
			pNewNode->right = NULL;
			pNewNode->height = 1;
			*ppNode = pNewNode;
			// 平衡二叉树
			AVLTree_Rebalance(ancestor, ancestorCount);
			// 返回节点数量
			objAVLT->Count++;
			return &pNewNode[1];
		}
		
		// 从 AVLTree 中删除节点（成功返回 TRUE、失败返回 FALSE）
		XXAPI int AVLTree_Remove(AVLTree_Object objAVLT, void* pKey)
		{
			AVLTree_NodeBase** ppNode = &objAVLT->RootNode;		// 到 root 节点的指针
			AVLTree_NodeBase** ancestor[AVLTree_MAX_HEIGHT];	// 上层节点列表
			int ancestorCount = 0;								// 上层节点数量
			AVLTree_NodeBase* pNode = NULL;						// 到当前节点指针
			// 查找需要删除的节点
			while ( ancestorCount < AVLTree_MAX_HEIGHT ) {
				pNode = *ppNode;
				if ( pNode == NULL ) { return 0; }				// 不存在的节点，直接跳出
				ancestor[ancestorCount++] = ppNode;
				int delta = objAVLT->CompProc(&pNode[1], pKey);
				if ( delta < 0 ) {
					ppNode = &(pNode->left);
				} else if ( delta > 0 ) {
					ppNode = &(pNode->right);
				} else {
					break;										// 找到要删除的节点了
				}
			}
			// 没找到要删除的节点
			if ( ancestorCount == AVLTree_MAX_HEIGHT ) {
				return 0;
			}
			AVLTree_NodeBase* pDelete = pNode;
			if ( pNode->left == NULL ) {
				// 删除节点的的左子树上没有节点。
				// 要么在它的右子树上有子节点（根据平衡规则，只能有一个），它取代了要删除的节点，要么它没有子节点(被删除)
				*ppNode = pNode->right;
				// 我们知道pNode->right已经平衡，所以我们不必再次检查
				ancestorCount--;
			} else {
				// 我们将在树的顺序中找到刚好在delNode之前的节点，并将其提升到树中delNode的位置
				AVLTree_NodeBase** ppDelete = ppNode;			// 指向我们必须删除的节点
				int deleteAncestorCount = ancestorCount;		// 替换节点必须插入到祖先列表中的位置
				// 在树排序中搜索要删除节点之前的节点
				ppNode = &(pNode->left);
				while ( ancestorCount < AVLTree_MAX_HEIGHT ) {
					pNode = *ppNode;
					if ( pNode->right == NULL ) { break; }
					ancestor[ancestorCount++] = ppNode;
					ppNode = &(pNode->right);
				}
				if ( ancestorCount == AVLTree_MAX_HEIGHT ) { return 0; }
				// 此节点将被其（由于平衡规则，它是唯一的）ld替换，或者如果它根本没有子节点，则将被删除
				*ppNode = pNode->left;
				// 现在，此节点将替换树中要删除的节点
				pNode->left = pDelete->left;
				pNode->right = pDelete->right;
				pNode->height = pDelete->height;
				*ppDelete = pNode;
				// 我们用pNode替换了delNode。因此，指向delNode左子树的指针存储在delNode->left中
				// 现在存储在pNode->left，我们必须调整祖先列表以反映这一点。
				ancestor[deleteAncestorCount] = &(pNode->left);
			}
			// 平衡二叉树
			AVLTree_Rebalance(ancestor, ancestorCount);
			// 返回结果
			if ( pDelete ) {
				MM256_Free(&objAVLT->objMM, pDelete);
				objAVLT->Count--;
				return -1;
			} else {
				return 0;
			}
		}
		
		// 从 AVLTree 中查找节点（返回 AVLTree 节点对象）
		XXAPI void* AVLTree_Search(AVLTree_Object objAVLT, void* pKey)
		{
			AVLTree_NodeBase* pNode = objAVLT->RootNode;
			while ( pNode != NULL ) {
				int delta = objAVLT->CompProc(&pNode[1], pKey);
				if ( delta < 0 ) {
					pNode = pNode->left;
				} else if ( delta > 0 ) {
					pNode = pNode->right;
				} else {
					return &pNode[1];
				}
			}
			return NULL;
		}
		
		// 遍历 AVLTree 所有节点
		int AVLTree_WalkRecuProc(AVLTree_NodeBase* root, AVLTree_EachProc procEach, void* pArg)
		{
			if ( root ) {
				// 递归左子树
				if ( root->left != NULL ) {
					if ( AVLTree_WalkRecuProc(root->left, procEach, pArg) ) {
						return -1;
					}
				}
				// 调用回调函数
				if ( procEach ) {
					if ( procEach(AVLTree_GetNodeData(root), pArg) ) {
						return -1;
					}
				}
				// 递归右子树
				if ( root->right != NULL ) {
					if ( AVLTree_WalkRecuProc(root->right, procEach, pArg) ) {
						return -1;
					}
				}
			}
			return 0;
		}
		int AVLTree_WalkExRecuProc(AVLTree_NodeBase* root, AVLTree_EachProc procPre, void* argPre, AVLTree_EachProc procIn, void* argIn, AVLTree_EachProc procPost, void* argPost)
		{
			if ( root ) {
				// 调用回调函数(前置)
				if ( procPre ) {
					if ( procPre(AVLTree_GetNodeData(root), argPre) ) {
						return -1;
					}
				}
				// 递归左子树
				if ( root->left != NULL ) {
					if ( AVLTree_WalkExRecuProc(root->left, procPre, argPre, procIn, argIn, procPost, argPost) ) {
						return -1;
					}
				}
				// 调用回调函数
				if ( procIn ) {
					if ( procIn(AVLTree_GetNodeData(root), argIn) ) {
						return -1;
					}
				}
				// 递归右子树
				if ( root->right != NULL ) {
					if ( AVLTree_WalkExRecuProc(root->right, procPre, argPre, procIn, argIn, procPost, argPost) ) {
						return -1;
					}
				}
				// 调用回调函数(后置)
				if ( procPost ) {
					if ( procPost(AVLTree_GetNodeData(root), argPost) ) {
						return -1;
					}
				}
			}
			return 0;
		}
		XXAPI void AVLTree_Walk(AVLTree_Object objAVLT, AVLTree_EachProc procEach, void* pArg)
		{
			AVLTree_WalkRecuProc(objAVLT->RootNode, procEach, pArg);
		}
		XXAPI void AVLTree_WalkEx(AVLTree_Object objAVLT, AVLTree_EachProc procPre, void* argPre, AVLTree_EachProc procIn, void* argIn, AVLTree_EachProc procPost, void* argPost)
		{
			AVLTree_WalkExRecuProc(objAVLT->RootNode, procPre, argPre, procIn, argIn, procPost, argPost);
		}
		
	#endif





	/*
		RBTree
	*/

	#ifdef MMU_USE_RBTREE
		
		// 创建 RBTree
		XXAPI RBTree_Object RBTree_Create(unsigned int iItemLength, RBTree_CompProc procComp)
		{
			RBTree_Object objRBT = mmu_malloc(sizeof(RBTree_Struct));
			if ( objRBT ) {
				RBTree_Init(objRBT, iItemLength, procComp);
			}
			return objRBT;
		}
		
		// 销毁 RBTree
		XXAPI void RBTree_Destroy(RBTree_Object objRBT)
		{
			if ( objRBT ) {
				RBTree_Unit(objRBT);
				mmu_free(objRBT);
			}
		}
		
		// 初始化 RBTree（对自维护结构体指针使用，和 RBTree_Create 功能类似）
		XXAPI void RBTree_Init(RBTree_Object objRBT, unsigned int iItemLength, RBTree_CompProc procComp)
		{
			// 红黑树节点必须是 4 字节对齐的（避免因内存对齐导致颜色和父节点数据访问出错）
			if ( iItemLength & 3 ) {
				iItemLength = iItemLength + 4 - (iItemLength & 3);
			}
			objRBT->RootNode = NULL;
			objRBT->CompProc = procComp;
			objRBT->Count = 0;
			MM256_Init(&objRBT->objMM, iItemLength + sizeof(RBTree_NodeBase));
		}
		
		// 释放 RBTree（对自维护结构体指针使用，和 RBTree_Destroy 功能类似）
		XXAPI void RBTree_Unit(RBTree_Object objRBT)
		{
			objRBT->RootNode = NULL;
			objRBT->Count = 0;
			MM256_Unit(&objRBT->objMM);
		}
		
		// 平衡、旋转、染色（内部函数）
		static inline void RBTree_Rotate_Left(RBTree_Object objRBT, RBTree_NodeBase* node)
		{
			RBTree_NodeBase* right = node->right;
			RBTree_NodeBase* parent = rb_parent(node);
			if ( (node->right = right->left) ) {
				rb_set_parent(right->left, node);
			}
			right->left = node;
			rb_set_parent(right, parent);
			if ( parent ) {
				if ( node == parent->left ) {
					parent->left = right;
				} else {
					parent->right = right;
				}
			} else {
				objRBT->RootNode = right;
			}
			rb_set_parent(node, right);
		}
		static inline void RBTree_Rotate_Right(RBTree_Object objRBT, RBTree_NodeBase* node)
		{
			RBTree_NodeBase* left = node->left;
			RBTree_NodeBase* parent = rb_parent(node);
			if ( (node->left = left->right) ) {
				rb_set_parent(left->right, node);
			}
			left->right = node;
			rb_set_parent(left, parent);
			if ( parent ) {
				if ( node == parent->right ) {
					parent->right = left;
				} else {
					parent->left = left;
				}
			} else {
				objRBT->RootNode = left;
			}
			rb_set_parent(node, left);
		}
		static inline void RBTree_Insert_Color(RBTree_Object objRBT, RBTree_NodeBase* node)
		{
			RBTree_NodeBase* parent;
			RBTree_NodeBase* gparent;
			while ( (parent = rb_parent(node)) && rb_is_red(parent) ) {
				gparent = rb_parent(parent);
				if ( parent == gparent->left ) {
					register RBTree_NodeBase* uncle = gparent->right;
					if ( uncle && rb_is_red(uncle) ) {
						rb_set_black(uncle);
						rb_set_black(parent);
						rb_set_red(gparent);
						node = gparent;
						continue;
					}
					if ( parent->right == node ) {
						RBTree_Rotate_Left(objRBT, parent);
						register RBTree_NodeBase* tmp = parent;
						parent = node;
						node = tmp;
					}
					rb_set_black(parent);
					rb_set_red(gparent);
					RBTree_Rotate_Right(objRBT, gparent);
				} else {
					register RBTree_NodeBase* uncle = gparent->left;
					if ( uncle && rb_is_red(uncle) ) {
						rb_set_black(uncle);
						rb_set_black(parent);
						rb_set_red(gparent);
						node = gparent;
						continue;
					}
					if ( parent->left == node ) {
						RBTree_Rotate_Right(objRBT, parent);
						register RBTree_NodeBase* tmp = parent;
						parent = node;
						node = tmp;
					}
					rb_set_black(parent);
					rb_set_red(gparent);
					RBTree_Rotate_Left(objRBT, gparent);
				}
			}
			rb_set_black(objRBT->RootNode);
		}
		static inline void RBTree_Erase_Color(RBTree_Object objRBT, RBTree_NodeBase* node, RBTree_NodeBase* parent)
		{
			RBTree_NodeBase* other;
			while ( (!node || rb_is_black(node)) && (node != objRBT->RootNode) ) {
				if ( parent->left == node ) {
					other = parent->right;
					if ( rb_is_red(other) ) {
						rb_set_black(other);
						rb_set_red(parent);
						RBTree_Rotate_Left(objRBT, parent);
						other = parent->right;
					}
					if ( (!other->left || rb_is_black(other->left)) && (!other->right || rb_is_black(other->right)) ) {
						rb_set_red(other);
						node = parent;
						parent = rb_parent(node);
					} else {
						if ( !other->right || rb_is_black(other->right) ) {
							rb_set_black(other->left);
							rb_set_red(other);
							RBTree_Rotate_Right(objRBT, other);
							other = parent->right;
						}
						rb_set_color(other, rb_color(parent));
						rb_set_black(parent);
						rb_set_black(other->right);
						RBTree_Rotate_Left(objRBT, parent);
						node = objRBT->RootNode;
						break;
					}
				} else {
					other = parent->left;
					if ( rb_is_red(other) ) {
						rb_set_black(other);
						rb_set_red(parent);
						RBTree_Rotate_Right(objRBT, parent);
						other = parent->left;
					}
					if ( (!other->left || rb_is_black(other->left)) && (!other->right || rb_is_black(other->right)) ) {
						rb_set_red(other);
						node = parent;
						parent = rb_parent(node);
					} else {
						if ( !other->left || rb_is_black(other->left) ) {
							rb_set_black(other->right);
							rb_set_red(other);
							RBTree_Rotate_Left(objRBT, other);
							other = parent->left;
						}
						rb_set_color(other, rb_color(parent));
						rb_set_black(parent);
						rb_set_black(other->left);
						RBTree_Rotate_Right(objRBT, parent);
						node = objRBT->RootNode;
						break;
					}
				}
			}
			if ( node ) {
				rb_set_black(node);
			}
		}

		static inline void RBTree_Erase(RBTree_Object objRBT, RBTree_NodeBase* node)
		{
			RBTree_NodeBase* child;
			RBTree_NodeBase* parent;
			int color;

			if ( !node->left ) {
				child = node->right;
			} else if (!node->right) {
				child = node->left;
			} else {
				RBTree_NodeBase* old = node;
				RBTree_NodeBase* left;

				node = node->right;
				while ((left = node->left) != NULL)
					node = left;

				if ( rb_parent(old) ) {
					if ( rb_parent(old)->left == old ) {
						rb_parent(old)->left = node;
					} else {
						rb_parent(old)->right = node;
					}
				} else {
					objRBT->RootNode = node;
				}

				child = node->right;
				parent = rb_parent(node);
				color = rb_color(node);

				if ( parent == old ) {
					parent = node;
				} else {
					if ( child ) {
						rb_set_parent(child, parent);
					}
					parent->left = child;

					node->right = old->right;
					rb_set_parent(old->right, node);
				}

				node->parent_color = old->parent_color;
				node->left = old->left;
				rb_set_parent(old->left, node);

				goto color;
			}

			parent = rb_parent(node);
			color = rb_color(node);

			if ( child ) {
				rb_set_parent(child, parent);
			}
			if ( parent ) {
				if ( parent->left == node ) {
					parent->left = child;
				} else {
					parent->right = child;
				}
			} else {
				objRBT->RootNode = child;
			}

		 color:
			if ( color == MMU_RBT_BLACK ) {
				RBTree_Erase_Color(objRBT, child, parent);
			}
		}
		
		// 向 RBTree 中插入节点，返回数据段指针（如果值已经存在，则会返回已存在的数据段指针）
		XXAPI void* RBTree_AddNode(RBTree_Object objRBT, void* pKey, int* bNew)
		{
			// 初始化数据
			RBTree_NodeBase** ppNode = &objRBT->RootNode;
			RBTree_NodeBase* pParent = NULL;
			// 找到新节点要添加的位置
			while ( *ppNode ) {
				RBTree_NodeBase* pNode = *ppNode;
				pParent = *ppNode;
				int delta = objRBT->CompProc(&pNode[1], pKey);
				if ( delta < 0 ) {
					ppNode = &(pNode->left);
				} else if ( delta > 0 ) {
					ppNode = &(pNode->right);
				} else {
					if ( bNew ) { *bNew = 0; }
					return &pNode[1];
				}
			}
			// 添加新节点
			RBTree_NodeBase* pNewNode = MM256_Alloc(&objRBT->objMM);
			if ( pNewNode == NULL ) { return NULL; }
			pNewNode->left = NULL;
			pNewNode->right = NULL;
			pNewNode->parent_color = (intptr_t)pParent;
			if ( bNew ) { *bNew = -1; }
			*ppNode = pNewNode;
			// 平衡、染色二叉树
			RBTree_Insert_Color(objRBT, pNewNode);
			// 返回节点
			objRBT->Count++;
			return &pNewNode[1];
		}
		
		// 向 RBTree 中插入节点（值必须不存在，如果值已经存在，则返回NULL）
		XXAPI void* RBTree_Insert(RBTree_Object objRBT, void* pKey)
		{
			// 初始化数据
			RBTree_NodeBase** ppNode = &objRBT->RootNode;
			RBTree_NodeBase* pParent = NULL;
			// 找到新节点要添加的位置
			while ( *ppNode ) {
				RBTree_NodeBase* pNode = *ppNode;
				pParent = *ppNode;
				int delta = objRBT->CompProc(&pNode[1], pKey);
				if ( delta < 0 ) {
					ppNode = &(pNode->left);
				} else if ( delta > 0 ) {
					ppNode = &(pNode->right);
				} else {
					return NULL;
				}
			}
			// 添加新节点
			RBTree_NodeBase* pNewNode = MM256_Alloc(&objRBT->objMM);
			if ( pNewNode == NULL ) { return NULL; }
			pNewNode->left = NULL;
			pNewNode->right = NULL;
			pNewNode->parent_color = (intptr_t)pParent;
			*ppNode = pNewNode;
			// 平衡二叉树
			RBTree_Insert_Color(objRBT, pNewNode);
			// 返回节点
			objRBT->Count++;
			return &pNewNode[1];
		}
		
		// 从 RBTree 中删除节点
		XXAPI int RBTree_Remove(RBTree_Object objRBT, void* pKey)
		{
			RBTree_NodeBase* pNode = objRBT->RootNode;
			while ( pNode ) {
				int delta = objRBT->CompProc(&pNode[1], pKey);
				if ( delta < 0 ) {
					pNode = pNode->left;
				} else if ( delta > 0 ) {
					pNode = pNode->right;
				} else {
					break;
				}
			}
			if ( pNode ) {
				RBTree_Erase(objRBT, pNode);
				MM256_Free(&objRBT->objMM, pNode);
				objRBT->Count--;
				return -1;
			}
			return 0;
		}
		
		// 从 RBTree 中查找节点（返回 RBTree 节点对象）
		XXAPI void* RBTree_Search(RBTree_Object objRBT, void* pKey)
		{
			RBTree_NodeBase* pNode = objRBT->RootNode;
			while ( pNode ) {
				int delta = objRBT->CompProc(&pNode[1], pKey);
				if ( delta < 0 ) {
					pNode = pNode->left;
				} else if ( delta > 0 ) {
					pNode = pNode->right;
				} else {
					return &pNode[1];
				}
			}
			return NULL;
		}
		
		// 遍历 RBTree 所有节点
		int RBTree_WalkRecuProc(RBTree_NodeBase* root, RBTree_EachProc procEach, void* pArg)
		{
			if ( root ) {
				// 递归左子树
				if ( root->left != NULL ) {
					if ( RBTree_WalkRecuProc(root->left, procEach, pArg) ) {
						return -1;
					}
				}
				// 调用回调函数
				if ( procEach ) {
					if ( procEach(RBTree_GetNodeData(root), pArg) ) {
						return -1;
					}
				}
				// 递归右子树
				if ( root->right != NULL ) {
					if ( RBTree_WalkRecuProc(root->right, procEach, pArg) ) {
						return -1;
					}
				}
			}
			return 0;
		}
		int RBTree_WalkExRecuProc(RBTree_NodeBase* root, RBTree_EachProc procPre, void* argPre, RBTree_EachProc procIn, void* argIn, RBTree_EachProc procPost, void* argPost)
		{
			if ( root ) {
				// 调用回调函数(前置)
				if ( procPre ) {
					if ( procPre(RBTree_GetNodeData(root), argPre) ) {
						return -1;
					}
				}
				// 递归左子树
				if ( root->left != NULL ) {
					if ( RBTree_WalkExRecuProc(root->left, procPre, argPre, procIn, argIn, procPost, argPost) ) {
						return -1;
					}
				}
				// 调用回调函数
				if ( procIn ) {
					if ( procIn(RBTree_GetNodeData(root), argIn) ) {
						return -1;
					}
				}
				// 递归右子树
				if ( root->right != NULL ) {
					if ( RBTree_WalkExRecuProc(root->right, procPre, argPre, procIn, argIn, procPost, argPost) ) {
						return -1;
					}
				}
				// 调用回调函数(后置)
				if ( procPost ) {
					if ( procPost(RBTree_GetNodeData(root), argPost) ) {
						return -1;
					}
				}
			}
			return 0;
		}
		XXAPI void RBTree_Walk(RBTree_Object objRBT, RBTree_EachProc procEach, void* pArg)
		{
			RBTree_WalkRecuProc(objRBT->RootNode, procEach, pArg);
		}
		XXAPI void RBTree_WalkEx(RBTree_Object objRBT, RBTree_EachProc procPre, void* argPre, RBTree_EachProc procIn, void* argIn, RBTree_EachProc procPost, void* argPost)
		{
			RBTree_WalkExRecuProc(objRBT->RootNode, procPre, argPre, procIn, argIn, procPost, argPost);
		}
		
	#endif





	/*
		Hash32 - nmhash32x [Ver2.0, Update : 2024/10/18 from https://github.com/rurban/smhasher]
			修改：
				删除函数：NMHASH32_0to8（仅NMHASH32依赖，只保留NMHASH32X）
				删除函数：NMHASH32_9to255（仅NMHASH32依赖，只保留NMHASH32X）
				删除定义：NMHASH32_9to32（仅NMHASH32依赖，只保留NMHASH32X）
				删除定义：NMHASH32_33to255（仅NMHASH32依赖，只保留NMHASH32X）
				删除函数：NMHASH32_avalanche32（仅NMHASH32依赖，只保留NMHASH32X）
				删除函数：NMHASH32（只保留NMHASH32X）
				添加部分条件编译分支：判断 __TINYC__ 以区分是否使用 TCC 编译
				删除头文件引入：<stdint.h> 和 <string.h>（最上面已经引入）
			使用协议注意事项：
				BSD 2-Clause 协议
				允许个人使用、商业使用
				复制、分发、修改，除了加上作者的版权信息，还必须保留免责声明，免除作者的责任
	*/

	#ifdef MMU_USE_HASH32
		
		/*
			BSD 2-Clause License

			Copyright (c) 2021, James Z.M. Gao 
			All rights reserved.

			Redistribution and use in source and binary forms, with or without
			modification, are permitted provided that the following conditions are met:

			1. Redistributions of source code must retain the above copyright notice, this
			   list of conditions and the following disclaimer.

			2. Redistributions in binary form must reproduce the above copyright notice,
			   this list of conditions and the following disclaimer in the documentation
			   and/or other materials provided with the distribution.

			THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
			AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
			IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
			DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
			FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
			DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
			SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
			CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
			OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
			OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
		*/
		
		#define NMH_VERSION 2
		
		#ifdef _MSC_VER
		#  pragma warning(push, 3)
		#endif
		
		#if defined(__cplusplus) && __cplusplus < 201103L
		#  define __STDC_CONSTANT_MACROS 1
		#endif
		
		#if defined(__GNUC__)
		#  if defined(__AVX2__)
		#    include <immintrin.h>
		#  elif defined(__SSE2__)
		#    include <emmintrin.h>
		#  endif
		#elif defined(_MSC_VER)
		#  include <intrin.h>
		#endif
		
		#ifdef _MSC_VER
		#  pragma warning(pop)
		#endif
		
		#if (defined(__GNUC__) && (__GNUC__ >= 3))  \
		  || (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 800)) \
		  || defined(__clang__)
		#    define NMH_likely(x) __builtin_expect(x, 1)
		#else
		#    define NMH_likely(x) (x)
		#endif
		
		#if defined(__has_builtin)
		#  if __has_builtin(__builtin_rotateleft32)
		#    define NMH_rotl32 __builtin_rotateleft32 /* clang */
		#  endif
		#endif
		#if !defined(NMH_rotl32)
		#  if defined(_MSC_VER)
			 /* Note: although _rotl exists for minGW (GCC under windows), performance seems poor */
		#    define NMH_rotl32(x,r) _rotl(x,r)
		#  else
		#    define NMH_rotl32(x,r) (((x) << (r)) | ((x) >> (32 - (r))))
		#  endif
		#endif
		
		#ifdef __TINYC__
		#  define NMH_RESTRICT   restrict
		#  define NMH_VECTOR NMH_SCALAR
		#elif ((defined(sun) || defined(__sun)) && __cplusplus) /* Solaris includes __STDC_VERSION__ with C++. Tested with GCC 5.5 */
		#  define NMH_RESTRICT   /* disable */
		#elif defined (__STDC_VERSION__) && __STDC_VERSION__ >= 199901L   /* >= C99 */
		#  define NMH_RESTRICT   restrict
		#elif defined(__cplusplus) && (defined(__GNUC__) || defined(__clang__) || defined(__INTEL_COMPILER))
		#  define NMH_RESTRICT __restrict__
		#elif defined(__cplusplus) && defined(_MSC_VER)
		#  define NMH_RESTRICT __restrict
		#else
		#  define NMH_RESTRICT   /* disable */
		#endif
		
		/* endian macros */
		#ifndef NMHASH_LITTLE_ENDIAN
		#  if defined(_WIN32) || defined(__LITTLE_ENDIAN__) || defined(__x86_64__) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) || defined(__SDCC)
		#    define NMHASH_LITTLE_ENDIAN 1
		#  elif defined(__BIG_ENDIAN__) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
		#    define NMHASH_LITTLE_ENDIAN 0
		#  else
		#    warning could not determine endianness! Falling back to little endian.
		#    define NMHASH_LITTLE_ENDIAN 1
		#  endif
		#endif
		
		/* vector macros */
		#define NMH_SCALAR 0
		#define NMH_SSE2   1
		#define NMH_AVX2   2
		#define NMH_AVX512 3
		
		#ifndef NMH_VECTOR    /* can be defined on command line */
		#  if defined(__AVX512BW__)
		#    define NMH_VECTOR NMH_AVX512 /* _mm512_mullo_epi16 requires AVX512BW */
		#  elif defined(__AVX2__)
		#    define NMH_VECTOR NMH_AVX2  /* add '-mno-avx256-split-unaligned-load' and '-mn-oavx256-split-unaligned-store' for gcc */
		#  elif defined(__SSE2__) || defined(_M_AMD64) || defined(_M_X64) || (defined(_M_IX86_FP) && (_M_IX86_FP == 2))
		#    define NMH_VECTOR NMH_SSE2
		#  else
		#    define NMH_VECTOR NMH_SCALAR
		#  endif
		#endif
		
		/* align macros */
		#if defined (__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)   /* C11+ */
		#  include <stdalign.h>
		#  define NMH_ALIGN(n)      alignas(n)
		#elif defined(__GNUC__)
		#  define NMH_ALIGN(n)      __attribute__ ((aligned(n)))
		#elif defined(_MSC_VER)
		#  define NMH_ALIGN(n)      __declspec(align(n))
		#else
		#  define NMH_ALIGN(n)   /* disabled */
		#endif
		
		#if NMH_VECTOR > 0
		#  define NMH_ACC_ALIGN 64
		#elif defined(__BIGGEST_ALIGNMENT__)
		#  define NMH_ACC_ALIGN __BIGGEST_ALIGNMENT__
		#elif defined(__SDCC)
		#  define NMH_ACC_ALIGN 1
		#else
		#  define NMH_ACC_ALIGN 16
		#endif
		
		/* constants */
		
		/* primes from xxh */
		#define NMH_PRIME32_1  UINT32_C(0x9E3779B1)
		#define NMH_PRIME32_2  UINT32_C(0x85EBCA77)
		#define NMH_PRIME32_3  UINT32_C(0xC2B2AE3D)
		#define NMH_PRIME32_4  UINT32_C(0x27D4EB2F)
		
		/*! Pseudorandom secret taken directly from FARSH. */
		NMH_ALIGN(NMH_ACC_ALIGN) static const uint32_t NMH_ACC_INIT[32] = {
			UINT32_C(0xB8FE6C39), UINT32_C(0x23A44BBE), UINT32_C(0x7C01812C), UINT32_C(0xF721AD1C),
			UINT32_C(0xDED46DE9), UINT32_C(0x839097DB), UINT32_C(0x7240A4A4), UINT32_C(0xB7B3671F),
			UINT32_C(0xCB79E64E), UINT32_C(0xCCC0E578), UINT32_C(0x825AD07D), UINT32_C(0xCCFF7221),
			UINT32_C(0xB8084674), UINT32_C(0xF743248E), UINT32_C(0xE03590E6), UINT32_C(0x813A264C),

			UINT32_C(0x3C2852BB), UINT32_C(0x91C300CB), UINT32_C(0x88D0658B), UINT32_C(0x1B532EA3),
			UINT32_C(0x71644897), UINT32_C(0xA20DF94E), UINT32_C(0x3819EF46), UINT32_C(0xA9DEACD8),
			UINT32_C(0xA8FA763F), UINT32_C(0xE39C343F), UINT32_C(0xF9DCBBC7), UINT32_C(0xC70B4F1D),
			UINT32_C(0x8A51E04B), UINT32_C(0xCDB45931), UINT32_C(0xC89F7EC9), UINT32_C(0xD9787364),
		};
		
		#if defined(_MSC_VER) && _MSC_VER >= 1914
		#  pragma warning(push)
		#  pragma warning(disable: 5045)
		#endif
		#ifdef __SDCC
		#  define const
		#  pragma save
		#  pragma disable_warning 110
		#  pragma disable_warning 126
		#endif
		
		/* read functions */
		static inline
		uint32_t
		NMH_readLE32(const void *const p)
		{
			uint32_t v;
			memcpy(&v, p, 4);
		#	if (NMHASH_LITTLE_ENDIAN)
			return v;
		#	elif defined(__GNUC__) || defined(__INTEL_COMPILER) || defined(__clang__)
			return __builtin_bswap32(v);
		#	elif defined(_MSC_VER)
			return _byteswap_ulong(v);
		#	else
			return ((v >> 24) & 0xff) | ((v >> 8) & 0xff00) | ((v << 8) & 0xff0000) | ((v << 24) & 0xff000000);
		#	endif
		}
		
		static inline
		uint16_t
		NMH_readLE16(const void *const p)
		{
			uint16_t v;
			memcpy(&v, p, 2);
		#	if (NMHASH_LITTLE_ENDIAN)
			return v;
		#	else
			return (uint16_t)((v << 8) | (v >> 8));
		#	endif
		}
		
		#define __NMH_M1 UINT32_C(0xF0D9649B)
		#define __NMH_M2 UINT32_C(0x29A7935D)
		#define __NMH_M3 UINT32_C(0x55D35831)
		
		NMH_ALIGN(NMH_ACC_ALIGN) static const uint32_t __NMH_M1_V[32] = {
			__NMH_M1, __NMH_M1, __NMH_M1, __NMH_M1, __NMH_M1, __NMH_M1, __NMH_M1, __NMH_M1,
			__NMH_M1, __NMH_M1, __NMH_M1, __NMH_M1, __NMH_M1, __NMH_M1, __NMH_M1, __NMH_M1,
			__NMH_M1, __NMH_M1, __NMH_M1, __NMH_M1, __NMH_M1, __NMH_M1, __NMH_M1, __NMH_M1,
			__NMH_M1, __NMH_M1, __NMH_M1, __NMH_M1, __NMH_M1, __NMH_M1, __NMH_M1, __NMH_M1,
		};
		NMH_ALIGN(NMH_ACC_ALIGN) static const uint32_t __NMH_M2_V[32] = {
			__NMH_M2, __NMH_M2, __NMH_M2, __NMH_M2, __NMH_M2, __NMH_M2, __NMH_M2, __NMH_M2,
			__NMH_M2, __NMH_M2, __NMH_M2, __NMH_M2, __NMH_M2, __NMH_M2, __NMH_M2, __NMH_M2,
			__NMH_M2, __NMH_M2, __NMH_M2, __NMH_M2, __NMH_M2, __NMH_M2, __NMH_M2, __NMH_M2,
			__NMH_M2, __NMH_M2, __NMH_M2, __NMH_M2, __NMH_M2, __NMH_M2, __NMH_M2, __NMH_M2,
		};
		NMH_ALIGN(NMH_ACC_ALIGN) static const uint32_t __NMH_M3_V[32] = {
			__NMH_M3, __NMH_M3, __NMH_M3, __NMH_M3, __NMH_M3, __NMH_M3, __NMH_M3, __NMH_M3,
			__NMH_M3, __NMH_M3, __NMH_M3, __NMH_M3, __NMH_M3, __NMH_M3, __NMH_M3, __NMH_M3,
			__NMH_M3, __NMH_M3, __NMH_M3, __NMH_M3, __NMH_M3, __NMH_M3, __NMH_M3, __NMH_M3,
			__NMH_M3, __NMH_M3, __NMH_M3, __NMH_M3, __NMH_M3, __NMH_M3, __NMH_M3, __NMH_M3,
		};
		
		#undef __NMH_M1
		#undef __NMH_M2
		#undef __NMH_M3
		
		#if NMH_VECTOR == NMH_SCALAR
		#define NMHASH32_long_round NMHASH32_long_round_scalar
		static inline
		void
		NMHASH32_long_round_scalar(uint32_t *const NMH_RESTRICT accX, uint32_t *const NMH_RESTRICT accY, const uint8_t* const NMH_RESTRICT p)
		{
			/* breadth first calculation will hint some compiler to auto vectorize the code
			 * on gcc, the performance becomes 10x than the depth first, and about 80% of the manually vectorized code
			 */
			const size_t nbGroups = sizeof(NMH_ACC_INIT) / sizeof(*NMH_ACC_INIT);
			size_t i;
			
			for (i = 0; i < nbGroups; ++i) {
				accX[i] ^= NMH_readLE32(p + i * 4);
			}
			for (i = 0; i < nbGroups; ++i) {
				accY[i] ^= NMH_readLE32(p + i * 4 + sizeof(NMH_ACC_INIT));
			}
			for (i = 0; i < nbGroups; ++i) {
				accX[i] += accY[i];
			}
			for (i = 0; i < nbGroups; ++i) {
				accY[i] ^= accX[i] >> 1;
			}
			for (i = 0; i < nbGroups * 2; ++i) {
				((uint16_t*)accX)[i] *= ((uint16_t*)__NMH_M1_V)[i];
			}
			for (i = 0; i < nbGroups; ++i) {
				accX[i] ^= accX[i] << 5 ^ accX[i] >> 13;
			}
			for (i = 0; i < nbGroups * 2; ++i) {
				((uint16_t*)accX)[i] *= ((uint16_t*)__NMH_M2_V)[i];
			}
			for (i = 0; i < nbGroups; ++i) {
				accX[i] ^= accY[i];
			}
			for (i = 0; i < nbGroups; ++i) {
				accX[i] ^= accX[i] << 11 ^ accX[i] >> 9;
			}
			for (i = 0; i < nbGroups * 2; ++i) {
				((uint16_t*)accX)[i] *= ((uint16_t*)__NMH_M3_V)[i];
			}
			for (i = 0; i < nbGroups; ++i) {
				accX[i] ^= accX[i] >> 10 ^ accX[i] >> 20;
			}
		}
		#endif
		
		#if NMH_VECTOR == NMH_SSE2
		#  define _NMH_MM_(F) _mm_ ## F
		#  define _NMH_MMW_(F) _mm_ ## F ## 128
		#  define _NMH_MM_T __m128i
		#elif NMH_VECTOR == NMH_AVX2
		#  define _NMH_MM_(F) _mm256_ ## F
		#  define _NMH_MMW_(F) _mm256_ ## F ## 256
		#  define _NMH_MM_T __m256i
		#elif NMH_VECTOR == NMH_AVX512
		#  define _NMH_MM_(F) _mm512_ ## F
		#  define _NMH_MMW_(F) _mm512_ ## F ## 512
		#  define _NMH_MM_T __m512i
		#endif
		
		#if NMH_VECTOR == NMH_SSE2 || NMH_VECTOR == NMH_AVX2 || NMH_VECTOR == NMH_AVX512
		#  define NMHASH32_long_round NMHASH32_long_round_sse
		#  define NMH_VECTOR_NB_GROUP (sizeof(NMH_ACC_INIT) / sizeof(*NMH_ACC_INIT) / (sizeof(_NMH_MM_T) / sizeof(*NMH_ACC_INIT)))
		static inline
		void
		NMHASH32_long_round_sse(uint32_t* const NMH_RESTRICT accX, uint32_t* const NMH_RESTRICT accY, const uint8_t* const NMH_RESTRICT p)
		{
			const _NMH_MM_T *const NMH_RESTRICT m1    = (const _NMH_MM_T * NMH_RESTRICT)__NMH_M1_V;
			const _NMH_MM_T *const NMH_RESTRICT m2    = (const _NMH_MM_T * NMH_RESTRICT)__NMH_M2_V;
			const _NMH_MM_T *const NMH_RESTRICT m3    = (const _NMH_MM_T * NMH_RESTRICT)__NMH_M3_V;
				  _NMH_MM_T *const              xaccX = (      _NMH_MM_T *             )accX;
				  _NMH_MM_T *const              xaccY = (      _NMH_MM_T *             )accY;
				  _NMH_MM_T *const              xp    = (      _NMH_MM_T *             )p;
			size_t i;
			
			for (i = 0; i < NMH_VECTOR_NB_GROUP; ++i) {
				xaccX[i] = _NMH_MMW_(xor_si)(xaccX[i], _NMH_MMW_(loadu_si)(xp + i));
			}
			for (i = 0; i < NMH_VECTOR_NB_GROUP; ++i) {
				xaccY[i] = _NMH_MMW_(xor_si)(xaccY[i], _NMH_MMW_(loadu_si)(xp + i + NMH_VECTOR_NB_GROUP));
			}
			for (i = 0; i < NMH_VECTOR_NB_GROUP; ++i) {
				xaccX[i] = _NMH_MM_(add_epi32)(xaccX[i], xaccY[i]);
			}
			for (i = 0; i < NMH_VECTOR_NB_GROUP; ++i) {
				xaccY[i] = _NMH_MMW_(xor_si)(xaccY[i], _NMH_MM_(srli_epi32)(xaccX[i], 1));
			}
			for (i = 0; i < NMH_VECTOR_NB_GROUP; ++i) {
				xaccX[i] = _NMH_MM_(mullo_epi16)(xaccX[i], *m1);
			}
			for (i = 0; i < NMH_VECTOR_NB_GROUP; ++i) {
				xaccX[i] = _NMH_MMW_(xor_si)(_NMH_MMW_(xor_si)(xaccX[i], _NMH_MM_(slli_epi32)(xaccX[i], 5)), _NMH_MM_(srli_epi32)(xaccX[i], 13));
			}
			for (i = 0; i < NMH_VECTOR_NB_GROUP; ++i) {
				xaccX[i] = _NMH_MM_(mullo_epi16)(xaccX[i], *m2);
			}
			for (i = 0; i < NMH_VECTOR_NB_GROUP; ++i) {
				xaccX[i] = _NMH_MMW_(xor_si)(xaccX[i], xaccY[i]);
			}
			for (i = 0; i < NMH_VECTOR_NB_GROUP; ++i) {
				xaccX[i] = _NMH_MMW_(xor_si)(_NMH_MMW_(xor_si)(xaccX[i], _NMH_MM_(slli_epi32)(xaccX[i], 11)), _NMH_MM_(srli_epi32)(xaccX[i], 9));
			}
			for (i = 0; i < NMH_VECTOR_NB_GROUP; ++i) {
				xaccX[i] = _NMH_MM_(mullo_epi16)(xaccX[i], *m3);
			}
			for (i = 0; i < NMH_VECTOR_NB_GROUP; ++i) {
				xaccX[i] = _NMH_MMW_(xor_si)(_NMH_MMW_(xor_si)(xaccX[i], _NMH_MM_(srli_epi32)(xaccX[i], 10)), _NMH_MM_(srli_epi32)(xaccX[i], 20));
			}
		}
		#  undef _NMH_MM_
		#  undef _NMH_MMW_
		#  undef _NMH_MM_T
		#  undef NMH_VECTOR_NB_GROUP
		#endif
		
		static
		uint32_t
		NMHASH32_long(const uint8_t* const NMH_RESTRICT p, size_t const len, uint32_t const seed)
		{
			NMH_ALIGN(NMH_ACC_ALIGN) uint32_t accX[sizeof(NMH_ACC_INIT)/sizeof(*NMH_ACC_INIT)];
			NMH_ALIGN(NMH_ACC_ALIGN) uint32_t accY[sizeof(accX)/sizeof(*accX)];
			size_t const nbRounds = (len - 1) / (sizeof(accX) + sizeof(accY));
			size_t i;
			uint32_t sum = 0;
			
			/* init */
			for (i = 0; i < sizeof(accX)/sizeof(*accX); ++i) accX[i] = NMH_ACC_INIT[i];
			for (i = 0; i < sizeof(accY)/sizeof(*accY); ++i) accY[i] = seed;
			
			for (i = 0; i < nbRounds; ++i) {
				NMHASH32_long_round(accX, accY, p + i * (sizeof(accX) + sizeof(accY)));
			}
			NMHASH32_long_round(accX, accY, p + len - (sizeof(accX) + sizeof(accY)));
			
			/* merge acc */
			for (i = 0; i < sizeof(accX)/sizeof(*accX); ++i) accX[i] ^= NMH_ACC_INIT[i];
			for (i = 0; i < sizeof(accX)/sizeof(*accX); ++i) sum += accX[i];
			
		#	if SIZE_MAX > UINT32_C(-1)
			sum += (uint32_t)(len >> 32);
		#	endif
			return sum ^ (uint32_t)len;
		}
		
		static inline
		uint32_t
		NMHASH32X_0to4(uint32_t x, uint32_t const seed)
		{
			/* [bdab1ea9 18 a7896a1b 12 83796a2d 16] = 0.092922873297662509 */
			x ^= seed;
			x *= UINT32_C(0xBDAB1EA9);
			x += NMH_rotl32(seed, 31);
			x ^= x >> 18;
			x *= UINT32_C(0xA7896A1B);
			x ^= x >> 12;
			x *= UINT32_C(0x83796A2D);
			x ^= x >> 16;
			return x;
		}
		
		static inline
		uint32_t
		NMHASH32X_5to8(const uint8_t* const NMH_RESTRICT p, size_t const len, uint32_t const seed)
		{
			/* - 5 to 9 bytes
			 * - mixer: [11049a7d 23 bcccdc7b 12 065e9dad 12] = 0.16577596555667246 */
			
			uint32_t       x = NMH_readLE32(p) ^ NMH_PRIME32_3;
			uint32_t const y = NMH_readLE32(p + len - 4) ^ seed;
			x += y;
			x ^= x >> len;
			x *= UINT32_C(0x11049A7D);
			x ^= x >> 23;
			x *= UINT32_C(0xBCCCDC7B);
			x ^= NMH_rotl32(y, 3);
			x ^= x >> 12;
			x *= UINT32_C(0x065E9DAD);
			x ^= x >> 12;
			return x;
		}
		
		static inline
		uint32_t
		NMHASH32X_9to255(const uint8_t* const NMH_RESTRICT p, size_t const len, uint32_t const seed)
		{
			/* - at least 9 bytes
			 * - base mixer: [11049a7d 23 bcccdc7b 12 065e9dad 12] = 0.16577596555667246
			 * - tail mixer: [16 a52fb2cd 15 551e4d49 16] = 0.17162579707098322
			 */
			
			uint32_t x = NMH_PRIME32_3;
			uint32_t y = seed;
			uint32_t a = NMH_PRIME32_4;
			uint32_t b = seed;
			size_t i, r = (len - 1) / 16;
			
			for (i = 0; i < r; ++i) {
				x ^= NMH_readLE32(p + i * 16 + 0);
				y ^= NMH_readLE32(p + i * 16 + 4);
				x ^= y;
				x *= UINT32_C(0x11049A7D);
				x ^= x >> 23;
				x *= UINT32_C(0xBCCCDC7B);
				y  = NMH_rotl32(y, 4);
				x ^= y;
				x ^= x >> 12;
				x *= UINT32_C(0x065E9DAD);
				x ^= x >> 12;

				a ^= NMH_readLE32(p + i * 16 + 8);
				b ^= NMH_readLE32(p + i * 16 + 12);
				a ^= b;
				a *= UINT32_C(0x11049A7D);
				a ^= a >> 23;
				a *= UINT32_C(0xBCCCDC7B);
				b  = NMH_rotl32(b, 3);
				a ^= b;
				a ^= a >> 12;
				a *= UINT32_C(0x065E9DAD);
				a ^= a >> 12;
			}
			
			if (NMH_likely(((uint8_t)len-1) & 8)) {
				if (NMH_likely(((uint8_t)len-1) & 4)) {
					a ^= NMH_readLE32(p + r * 16 + 0);
					b ^= NMH_readLE32(p + r * 16 + 4);
					a ^= b;
					a *= UINT32_C(0x11049A7D);
					a ^= a >> 23;
					a *= UINT32_C(0xBCCCDC7B);
					a ^= NMH_rotl32(b, 4);
					a ^= a >> 12;
					a *= UINT32_C(0x065E9DAD);
				} else {
					a ^= NMH_readLE32(p + r * 16) + b;
					a ^= a >> 16;
					a *= UINT32_C(0xA52FB2CD);
					a ^= a >> 15;
					a *= UINT32_C(0x551E4D49);
				}
				
				x ^= NMH_readLE32(p + len - 8);
				y ^= NMH_readLE32(p + len - 4);
				x ^= y;
				x *= UINT32_C(0x11049A7D);
				x ^= x >> 23;
				x *= UINT32_C(0xBCCCDC7B);
				x ^= NMH_rotl32(y, 3);
				x ^= x >> 12;
				x *= UINT32_C(0x065E9DAD);
			} else {
				if (NMH_likely(((uint8_t)len-1) & 4)) {
					a ^= NMH_readLE32(p + r * 16) + b;
					a ^= a >> 16;
					a *= UINT32_C(0xA52FB2CD);
					a ^= a >> 15;
					a *= UINT32_C(0x551E4D49);
				}
				x ^= NMH_readLE32(p + len - 4) + y;
				x ^= x >> 16;
				x *= UINT32_C(0xA52FB2CD);
				x ^= x >> 15;
				x *= UINT32_C(0x551E4D49);
			}
			
			x ^= (uint32_t)len;
			x ^= NMH_rotl32(a, 27); /* rotate one lane to pass Diff test */
			x ^= x >> 14;
			x *= UINT32_C(0x141CC535);
			
			return x;
		}
		
		static inline
		uint32_t
		NMHASH32X_avalanche32(uint32_t x)
		{
			/* mixer with 2 mul from skeeto/hash-prospector:
			 * [15 d168aaad 15 af723597 15] = 0.15983776156606694
			 */
			x ^= x >> 15;
			x *= UINT32_C(0xD168AAAD);
			x ^= x >> 15;
			x *= UINT32_C(0xAF723597);
			x ^= x >> 15;
			return x;
		}
		
		/* use 32*32->32 multiplication for short hash */
		static inline
		uint32_t
		NMHASH32X(const void* const NMH_RESTRICT input, size_t const len, uint32_t seed)
		{
			const uint8_t *const p = (const uint8_t *)input;
			if (NMH_likely(len <= 8)) {
				if (NMH_likely(len > 4)) {
					return NMHASH32X_5to8(p, len, seed);
				} else {
					/* 0-4 bytes */
					union { uint32_t u32; uint16_t u16[2]; uint8_t u8[4]; } data;
					switch (len) {
						case 0: seed += NMH_PRIME32_2;
							data.u32 = 0;
							break;
						case 1: seed += NMH_PRIME32_2 + (UINT32_C(1) << 24) + (1 << 1);
							data.u32 = p[0];
							break;
						case 2: seed += NMH_PRIME32_2 + (UINT32_C(2) << 24) + (2 << 1);
							data.u32 = NMH_readLE16(p);
							break;
						case 3: seed += NMH_PRIME32_2 + (UINT32_C(3) << 24) + (3 << 1);
							data.u16[1] = p[2];
							data.u16[0] = NMH_readLE16(p);
							break;
						case 4: seed += NMH_PRIME32_1;
							data.u32 = NMH_readLE32(p);
							break;
						default: return 0;
					}
					return NMHASH32X_0to4(data.u32, seed);
				}
			}
			if (NMH_likely(len < 256)) {
				return NMHASH32X_9to255(p, len, seed);
			}
			return NMHASH32X_avalanche32(NMHASH32_long(p, len, seed));
		}
		
		#if defined(_MSC_VER) && _MSC_VER >= 1914
		#  pragma warning(pop)
		#endif
		#ifdef __SDCC
		#  pragma restore
		#  undef const
		#endif
		
		// 计算 32 位哈希值
		XXAPI unsigned int Hash32_WithSeed(void* key, size_t len, unsigned int seed)
		{
			return NMHASH32X(key, len, seed);
		}
		XXAPI unsigned int Hash32(void* key, size_t len)
		{
			return Hash32_WithSeed(key, len, HASH32_SEED);
		}
		
	#endif





	/*
		Hash64 - rapidhash [Ver1.0, Update : 2024/10/18 from https://github.com/Nicoshev/rapidhash]
			修改：
				删除头文件引入：<stdint.h> 和 <string.h>（最上面已经引入）
			使用协议注意事项：
				BSD 2-Clause 协议
				允许个人使用、商业使用
				复制、分发、修改，除了加上作者的版权信息，还必须保留免责声明，免除作者的责任
	*/

	#ifdef MMU_USE_HASH64
		
		/*
		 * rapidhash - Very fast, high quality, platform-independent hashing algorithm.
		 * Copyright (C) 2024 Nicolas De Carli
		 *
		 * Based on 'wyhash', by Wang Yi <godspeed_china@yeah.net>
		 *
		 * BSD 2-Clause License (https://www.opensource.org/licenses/bsd-license.php)
		 *
		 * Redistribution and use in source and binary forms, with or without
		 * modification, are permitted provided that the following conditions are
		 * met:
		 *
		 *    * Redistributions of source code must retain the above copyright
		 *      notice, this list of conditions and the following disclaimer.
		 *    * Redistributions in binary form must reproduce the above
		 *      copyright notice, this list of conditions and the following disclaimer
		 *      in the documentation and/or other materials provided with the
		 *      distribution.
		 *
		 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
		 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
		 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
		 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
		 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
		 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
		 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
		 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
		 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
		 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
		 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
		 *
		 * You can contact the author at:
		 *   - rapidhash source repository: https://github.com/Nicoshev/rapidhash
		 */
		
		/*
		 *  Includes.
		 */
		#if defined(_MSC_VER)
		  #include <intrin.h>
		  #if defined(_M_X64) && !defined(_M_ARM64EC)
			#pragma intrinsic(_umul128)
		  #endif
		#endif
		
		/*
		 *  C++ macros.
		 *
		 *  RAPIDHASH_INLINE can be overriden to be stronger than a hint, i.e. by adding __attribute__((always_inline)).
		 */
		#ifdef __cplusplus
		  #define RAPIDHASH_NOEXCEPT noexcept
		  #define RAPIDHASH_CONSTEXPR constexpr
		  #ifndef RAPIDHASH_INLINE
			#define RAPIDHASH_INLINE inline
		  #endif
		#else
		  #define RAPIDHASH_NOEXCEPT 
		  #define RAPIDHASH_CONSTEXPR const
		  #ifndef RAPIDHASH_INLINE
			#define RAPIDHASH_INLINE static inline
		  #endif
		#endif
		
		/*
		 *  Protection macro, alters behaviour of rapid_mum multiplication function.
		 *  
		 *  RAPIDHASH_FAST: Normal behavior, max speed.
		 *  RAPIDHASH_PROTECTED: Extra protection against entropy loss.
		 */
		#ifndef RAPIDHASH_PROTECTED
		  #define RAPIDHASH_FAST
		#elif defined(RAPIDHASH_FAST)
		  #error "cannot define RAPIDHASH_PROTECTED and RAPIDHASH_FAST simultaneously."
		#endif

		/*
		 *  Unrolling macros, changes code definition for main hash function.
		 *  
		 *  RAPIDHASH_COMPACT: Legacy variant, each loop process 48 bytes.
		 *  RAPIDHASH_UNROLLED: Unrolled variant, each loop process 96 bytes.
		 *
		 *  Most modern CPUs should benefit from having RAPIDHASH_UNROLLED.
		 *
		 *  These macros do not alter the output hash.
		 */
		#ifndef RAPIDHASH_COMPACT
		  #define RAPIDHASH_UNROLLED
		#elif defined(RAPIDHASH_UNROLLED)
		  #error "cannot define RAPIDHASH_COMPACT and RAPIDHASH_UNROLLED simultaneously."
		#endif

		/*
		 *  Likely and unlikely macros.
		 */
		#if defined(__GNUC__) || defined(__INTEL_COMPILER) || defined(__clang__)
		  #define _likely_(x)  __builtin_expect(x,1)
		  #define _unlikely_(x)  __builtin_expect(x,0)
		#else
		  #define _likely_(x) (x)
		  #define _unlikely_(x) (x)
		#endif

		/*
		 *  Endianness macros.
		 */
		#ifndef RAPIDHASH_LITTLE_ENDIAN
		  #if defined(_WIN32) || defined(__LITTLE_ENDIAN__) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
			#define RAPIDHASH_LITTLE_ENDIAN
		  #elif defined(__BIG_ENDIAN__) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
			#define RAPIDHASH_BIG_ENDIAN
		  #else
			#warning "could not determine endianness! Falling back to little endian."
			#define RAPIDHASH_LITTLE_ENDIAN
		  #endif
		#endif

		/*
		 *  Default seed.
		 */
		#define RAPID_SEED (0xbdd89aa982704029ull)

		/*
		 *  Default secret parameters.
		 */
		RAPIDHASH_CONSTEXPR uint64_t rapid_secret[3] = {0x2d358dccaa6c78a5ull, 0x8bb84b93962eacc9ull, 0x4b33a62ed433d4a3ull};

		/*
		 *  64*64 -> 128bit multiply function.
		 *  
		 *  @param A  Address of 64-bit number.
		 *  @param B  Address of 64-bit number.
		 *
		 *  Calculates 128-bit C = *A * *B.
		 *
		 *  When RAPIDHASH_FAST is defined:
		 *  Overwrites A contents with C's low 64 bits.
		 *  Overwrites B contents with C's high 64 bits.
		 *
		 *  When RAPIDHASH_PROTECTED is defined:
		 *  Xors and overwrites A contents with C's low 64 bits.
		 *  Xors and overwrites B contents with C's high 64 bits.
		 */
		RAPIDHASH_INLINE void rapid_mum(uint64_t *A, uint64_t *B) RAPIDHASH_NOEXCEPT {
		#if defined(__SIZEOF_INT128__)
		  __uint128_t r=*A; r*=*B; 
		  #ifdef RAPIDHASH_PROTECTED
		  *A^=(uint64_t)r; *B^=(uint64_t)(r>>64);
		  #else
		  *A=(uint64_t)r; *B=(uint64_t)(r>>64);
		  #endif
		#elif defined(_MSC_VER) && (defined(_WIN64) || defined(_M_HYBRID_CHPE_ARM64))
		  #if defined(_M_X64)
			#ifdef RAPIDHASH_PROTECTED
			uint64_t a, b;
			a=_umul128(*A,*B,&b);
			*A^=a;  *B^=b;
			#else
			*A=_umul128(*A,*B,B);
			#endif
		  #else
			#ifdef RAPIDHASH_PROTECTED
			uint64_t a, b;
			b = __umulh(*A, *B);
			a = *A * *B;
			*A^=a;  *B^=b;
			#else
			uint64_t c = __umulh(*A, *B);
			*A = *A * *B;
			*B = c;
			#endif
		  #endif
		#else
		  uint64_t ha=*A>>32, hb=*B>>32, la=(uint32_t)*A, lb=(uint32_t)*B, hi, lo;
		  uint64_t rh=ha*hb, rm0=ha*lb, rm1=hb*la, rl=la*lb, t=rl+(rm0<<32), c=t<rl;
		  lo=t+(rm1<<32); c+=lo<t; hi=rh+(rm0>>32)+(rm1>>32)+c;
		  #ifdef RAPIDHASH_PROTECTED
		  *A^=lo;  *B^=hi;
		  #else
		  *A=lo;  *B=hi;
		  #endif
		#endif
		}

		/*
		 *  Multiply and xor mix function.
		 *  
		 *  @param A  64-bit number.
		 *  @param B  64-bit number.
		 *
		 *  Calculates 128-bit C = A * B.
		 *  Returns 64-bit xor between high and low 64 bits of C.
		 */
		RAPIDHASH_INLINE uint64_t rapid_mix(uint64_t A, uint64_t B) RAPIDHASH_NOEXCEPT { rapid_mum(&A,&B); return A^B; }

		/*
		 *  Read functions.
		 */
		#ifdef RAPIDHASH_LITTLE_ENDIAN
		RAPIDHASH_INLINE uint64_t rapid_read64(const uint8_t *p) RAPIDHASH_NOEXCEPT { uint64_t v; memcpy(&v, p, sizeof(uint64_t)); return v;}
		RAPIDHASH_INLINE uint64_t rapid_read32(const uint8_t *p) RAPIDHASH_NOEXCEPT { uint32_t v; memcpy(&v, p, sizeof(uint32_t)); return v;}
		#elif defined(__GNUC__) || defined(__INTEL_COMPILER) || defined(__clang__)
		RAPIDHASH_INLINE uint64_t rapid_read64(const uint8_t *p) RAPIDHASH_NOEXCEPT { uint64_t v; memcpy(&v, p, sizeof(uint64_t)); return __builtin_bswap64(v);}
		RAPIDHASH_INLINE uint64_t rapid_read32(const uint8_t *p) RAPIDHASH_NOEXCEPT { uint32_t v; memcpy(&v, p, sizeof(uint32_t)); return __builtin_bswap32(v);}
		#elif defined(_MSC_VER)
		RAPIDHASH_INLINE uint64_t rapid_read64(const uint8_t *p) RAPIDHASH_NOEXCEPT { uint64_t v; memcpy(&v, p, sizeof(uint64_t)); return _byteswap_uint64(v);}
		RAPIDHASH_INLINE uint64_t rapid_read32(const uint8_t *p) RAPIDHASH_NOEXCEPT { uint32_t v; memcpy(&v, p, sizeof(uint32_t)); return _byteswap_ulong(v);}
		#else
		RAPIDHASH_INLINE uint64_t rapid_read64(const uint8_t *p) RAPIDHASH_NOEXCEPT {
		  uint64_t v; memcpy(&v, p, 8);
		  return (((v >> 56) & 0xff)| ((v >> 40) & 0xff00)| ((v >> 24) & 0xff0000)| ((v >>  8) & 0xff000000)| ((v <<  8) & 0xff00000000)| ((v << 24) & 0xff0000000000)| ((v << 40) & 0xff000000000000)| ((v << 56) & 0xff00000000000000));
		}
		RAPIDHASH_INLINE uint64_t rapid_read32(const uint8_t *p) RAPIDHASH_NOEXCEPT {
		  uint32_t v; memcpy(&v, p, 4);
		  return (((v >> 24) & 0xff)| ((v >>  8) & 0xff00)| ((v <<  8) & 0xff0000)| ((v << 24) & 0xff000000));
		}
		#endif

		/*
		 *  Reads and combines 3 bytes of input.
		 *
		 *  @param p  Buffer to read from.
		 *  @param k  Length of @p, in bytes.
		 *
		 *  Always reads and combines 3 bytes from memory.
		 *  Guarantees to read each buffer position at least once.
		 *  
		 *  Returns a 64-bit value containing all three bytes read. 
		 */
		RAPIDHASH_INLINE uint64_t rapid_readSmall(const uint8_t *p, size_t k) RAPIDHASH_NOEXCEPT { return (((uint64_t)p[0])<<56)|(((uint64_t)p[k>>1])<<32)|p[k-1];}

		/*
		 *  rapidhash main function.
		 *
		 *  @param key     Buffer to be hashed.
		 *  @param len     @key length, in bytes.
		 *  @param seed    64-bit seed used to alter the hash result predictably.
		 *  @param secret  Triplet of 64-bit secrets used to alter hash result predictably.
		 *
		 *  Returns a 64-bit hash.
		 */
		RAPIDHASH_INLINE uint64_t rapidhash_internal(const void *key, size_t len, uint64_t seed, const uint64_t* secret) RAPIDHASH_NOEXCEPT {
		  const uint8_t *p=(const uint8_t *)key; seed^=rapid_mix(seed^secret[0],secret[1])^len;  uint64_t  a,  b;
		  if(_likely_(len<=16)){
			if(_likely_(len>=4)){ 
			  const uint8_t * plast = p + len - 4;
			  a = (rapid_read32(p) << 32) | rapid_read32(plast);
			  const uint64_t delta = ((len&24)>>(len>>3));
			  b = ((rapid_read32(p + delta) << 32) | rapid_read32(plast - delta)); }
			else if(_likely_(len>0)){ a=rapid_readSmall(p,len); b=0;}
			else a=b=0;
		  }
		  else{
			size_t i=len; 
			if(_unlikely_(i>48)){
			  uint64_t see1=seed, see2=seed;
		#ifdef RAPIDHASH_UNROLLED
			  while(_likely_(i>=96)){
				seed=rapid_mix(rapid_read64(p)^secret[0],rapid_read64(p+8)^seed);
				see1=rapid_mix(rapid_read64(p+16)^secret[1],rapid_read64(p+24)^see1);
				see2=rapid_mix(rapid_read64(p+32)^secret[2],rapid_read64(p+40)^see2);
				seed=rapid_mix(rapid_read64(p+48)^secret[0],rapid_read64(p+56)^seed);
				see1=rapid_mix(rapid_read64(p+64)^secret[1],rapid_read64(p+72)^see1);
				see2=rapid_mix(rapid_read64(p+80)^secret[2],rapid_read64(p+88)^see2);
				p+=96; i-=96;
			  }
			  if(_unlikely_(i>=48)){
				seed=rapid_mix(rapid_read64(p)^secret[0],rapid_read64(p+8)^seed);
				see1=rapid_mix(rapid_read64(p+16)^secret[1],rapid_read64(p+24)^see1);
				see2=rapid_mix(rapid_read64(p+32)^secret[2],rapid_read64(p+40)^see2);
				p+=48; i-=48;
			  }
		#else
			  do {
				seed=rapid_mix(rapid_read64(p)^secret[0],rapid_read64(p+8)^seed);
				see1=rapid_mix(rapid_read64(p+16)^secret[1],rapid_read64(p+24)^see1);
				see2=rapid_mix(rapid_read64(p+32)^secret[2],rapid_read64(p+40)^see2);
				p+=48; i-=48;
			  } while (_likely_(i>=48));
		#endif
			  seed^=see1^see2;
			}
			if(i>16){
			  seed=rapid_mix(rapid_read64(p)^secret[2],rapid_read64(p+8)^seed^secret[1]);
			  if(i>32)
				seed=rapid_mix(rapid_read64(p+16)^secret[2],rapid_read64(p+24)^seed);
			}
			a=rapid_read64(p+i-16);  b=rapid_read64(p+i-8);
		  }
		  a^=secret[1]; b^=seed;  rapid_mum(&a,&b);
		  return  rapid_mix(a^secret[0]^len,b^secret[1]);
		}

		/*
		 *  rapidhash default seeded hash function.
		 *
		 *  @param key     Buffer to be hashed.
		 *  @param len     @key length, in bytes.
		 *  @param seed    64-bit seed used to alter the hash result predictably.
		 *
		 *  Calls rapidhash_internal using provided parameters and default secrets.
		 *
		 *  Returns a 64-bit hash.
		 */
		RAPIDHASH_INLINE uint64_t rapidhash_withSeed(const void *key, size_t len, uint64_t seed) RAPIDHASH_NOEXCEPT {
		  return rapidhash_internal(key, len, seed, rapid_secret);
		}

		/*
		 *  rapidhash default hash function.
		 *
		 *  @param key     Buffer to be hashed.
		 *  @param len     @key length, in bytes.
		 *
		 *  Calls rapidhash_withSeed using provided parameters and the default seed.
		 *
		 *  Returns a 64-bit hash.
		 */
		RAPIDHASH_INLINE uint64_t rapidhash(const void *key, size_t len) RAPIDHASH_NOEXCEPT {
		  return rapidhash_withSeed(key, len, RAPID_SEED);
		}
		
		// 计算 64 位哈希值
		XXAPI unsigned long long Hash64_WithSeed(void* key, size_t len, unsigned long long seed)
		{
			return rapidhash_internal(key, len, seed, rapid_secret);
		}
		XXAPI unsigned long long Hash64(void* key, size_t len)
		{
			return Hash64_WithSeed(key, len, HASH64_SEED);
		}
		
	#endif





	/*
		Hash Table 32 通用函数
	*/
	#if defined(MMU_USE_AVLHT32) || defined(MMU_USE_RBHT32)
		
		// 哈希表比较函数（内部函数）
		int HT32_CompProc(HT32_NodeBase* pNode, HT32_NodeBase* pObjKey)
		{
			if ( pNode->Hash == pObjKey->Hash ) {
				if ( pNode->KeyLen == pObjKey->KeyLen ) {
					return memcmp(pNode->Key, pObjKey->Key, pObjKey->KeyLen);
				} else {
					return pNode->KeyLen - pObjKey->KeyLen;
				}
			} else if ( pNode->Hash > pObjKey->Hash ) {
				return 1;
			} else {
				return -1;
			}
		}
		
		// 哈希值计算函数（内部函数）
		#define HT32_EvalHash(obj, k, l) obj.Key = k; obj.KeyLen = l; obj.Hash = Hash32(k, l)
		
	#endif





	/*
		Hash Table 64 通用函数
	*/
	#if defined(MMU_USE_AVLHT64) || defined(MMU_USE_RBHT64)
		
		// 哈希表比较函数（内部函数）
		int HT64_CompProc(HT64_NodeBase* pNode, HT64_NodeBase* pObjKey)
		{
			if ( pNode->Hash == pObjKey->Hash ) {
				if ( pNode->KeyLen == pObjKey->KeyLen ) {
					return memcmp(pNode->Key, pObjKey->Key, pObjKey->KeyLen);
				} else {
					return pNode->KeyLen - pObjKey->KeyLen;
				}
			} else if ( pNode->Hash > pObjKey->Hash ) {
				return 1;
			} else {
				return -1;
			}
		}
		
		// 哈希值计算函数（内部函数）
		#define HT64_EvalHash(obj, k, l) obj.Key = k; obj.KeyLen = l; obj.Hash = Hash64(k, l)
		
	#endif





	/*
		AVLHT32 - AVLTree Hash Table (32bit Hash Value)
	*/

	#ifdef MMU_USE_AVLHT32
		
		// 创建哈希表
		XXAPI AVLHT32_Object AVLHT32_Create(unsigned int iItemLength)
		{
			AVLHT32_Object objHT = mmu_malloc(sizeof(AVLHT32_Struct));
			if ( objHT ) {
				AVLHT32_Init(objHT, iItemLength);
			}
			return objHT;
		}
		
		// 销毁哈希表
		XXAPI void AVLHT32_Destroy(AVLHT32_Object objHT)
		{
			if ( objHT ) {
				AVLHT32_Unit(objHT);
				mmu_free(objHT);
			}
		}
		
		// 初始化哈希表（对自维护结构体指针使用，和 AVLHT32_Create 功能类似）
		XXAPI void AVLHT32_Init(AVLHT32_Object objHT, unsigned int iItemLength)
		{
			AVLTree_Init(&objHT->AVLT, iItemLength + sizeof(HT32_NodeBase), (void*)HT32_CompProc);
		}
		
		// 释放哈希表（对自维护结构体指针使用，和 AVLHT32_Destroy 功能类似）
		void AVLHT32_FreeKeysRecuProc(AVLTree_NodeBase* root)
		{
			if ( root ) {
				// 递归左子树
				if ( root->left != NULL ) {
					AVLHT32_FreeKeysRecuProc(root->left);
				}
				// 释放 Key
				HT32_NodeBase* pNode = AVLTree_GetNodeData(root);
				mmu_free(pNode->Key);
				// 递归右子树
				if ( root->right != NULL ) {
					AVLHT32_FreeKeysRecuProc(root->right);
				}
			}
		}
		XXAPI void AVLHT32_Unit(AVLHT32_Object objHT)
		{
			AVLHT32_FreeKeysRecuProc(objHT->AVLT.RootNode);
			AVLTree_Unit(&objHT->AVLT);
		}
		
		// 设置值
		XXAPI void* AVLHT32_Set(AVLHT32_Object objHT, void* sKey, unsigned int iKeyLen)
		{
			HT32_NodeBase objKey;
			HT32_EvalHash(objKey, sKey, iKeyLen);
			HT32_NodeBase* pNode = AVLTree_AddNode(&objHT->AVLT, &objKey, NULL);
			if ( pNode ) {
				pNode->Key = mmu_malloc(iKeyLen + 1);
				pNode->KeyLen = iKeyLen;
				pNode->Hash = objKey.Hash;
				memcpy(pNode->Key, sKey, iKeyLen);
				((char*)pNode->Key)[iKeyLen] = 0;
				return &pNode[1];
			}
			return NULL;
		}
		
		// 获取值
		XXAPI void* AVLHT32_Get(AVLHT32_Object objHT, void* sKey, unsigned int iKeyLen)
		{
			HT32_NodeBase objKey;
			HT32_EvalHash(objKey, sKey, iKeyLen);
			HT32_NodeBase* pNode = AVLTree_Search(&objHT->AVLT, &objKey);
			if ( pNode ) {
				return &pNode[1];
			}
			return NULL;
		}
		
		// 删除值
		XXAPI int AVLHT32_Remove(AVLHT32_Object objHT, void* sKey, unsigned int iKeyLen)
		{
			HT32_NodeBase objKey;
			HT32_EvalHash(objKey, sKey, iKeyLen);
			return AVLTree_Remove(&objHT->AVLT, &objKey);
		}
		
		// 判断值是否存在
		XXAPI int AVLHT32_Exists(AVLHT32_Object objHT, void* sKey, unsigned int iKeyLen)
		{
			HT32_NodeBase objKey;
			HT32_EvalHash(objKey, sKey, iKeyLen);
			HT32_NodeBase* pNode = AVLTree_Search(&objHT->AVLT, &objKey);
			if ( pNode ) {
				return -1;
			}
			return 0;
		}
		
		// 获取表内元素数量
		XXAPI unsigned int AVLHT32_Count(AVLHT32_Object objHT)
		{
			return objHT->AVLT.Count;
		}
		
		// 遍历表元素
		int AVLHT32_WalkRecuProc(AVLTree_NodeBase* root, HT32_EachProc procEach, void* pArg)
		{
			if ( root ) {
				// 递归左子树
				if ( root->left != NULL ) {
					if ( AVLHT32_WalkRecuProc(root->left, procEach, pArg) ) {
						return -1;
					}
				}
				// 调用回调函数
				if ( procEach ) {
					if ( procEach(AVLTree_GetNodeData(root), ((void*)root) + sizeof(AVLTree_NodeBase) + sizeof(HT32_NodeBase), pArg) ) {
						return -1;
					}
				}
				// 递归右子树
				if ( root->right != NULL ) {
					if ( AVLHT32_WalkRecuProc(root->right, procEach, pArg) ) {
						return -1;
					}
				}
			}
			return 0;
		}
		XXAPI void AVLHT32_Walk(AVLHT32_Object objHT, HT32_EachProc procEach, void* pArg)
		{
			AVLHT32_WalkRecuProc(objHT->AVLT.RootNode, procEach, pArg);
		}
		
	#endif





	/*
		AVLHT64 - AVLTree Hash Table (64bit Hash Value)
	*/

	#ifdef MMU_USE_AVLHT64
		
		// 创建哈希表
		XXAPI AVLHT64_Object AVLHT64_Create(unsigned int iItemLength)
		{
			AVLHT64_Object objHT = mmu_malloc(sizeof(AVLHT64_Struct));
			if ( objHT ) {
				AVLHT64_Init(objHT, iItemLength);
			}
			return objHT;
		}
		
		// 销毁哈希表
		XXAPI void AVLHT64_Destroy(AVLHT64_Object objHT)
		{
			if ( objHT ) {
				AVLHT64_Unit(objHT);
				mmu_free(objHT);
			}
		}
		
		// 初始化哈希表（对自维护结构体指针使用，和 AVLHT64_Create 功能类似）
		XXAPI void AVLHT64_Init(AVLHT64_Object objHT, unsigned int iItemLength)
		{
			AVLTree_Init(&objHT->AVLT, iItemLength + sizeof(HT64_NodeBase), (void*)HT64_CompProc);
		}
		
		// 释放哈希表（对自维护结构体指针使用，和 AVLHT64_Destroy 功能类似）
		void AVLHT64_FreeKeysRecuProc(AVLTree_NodeBase* root)
		{
			if ( root ) {
				// 递归左子树
				if ( root->left != NULL ) {
					AVLHT64_FreeKeysRecuProc(root->left);
				}
				// 释放 Key
				HT64_NodeBase* pNode = AVLTree_GetNodeData(root);
				mmu_free(pNode->Key);
				// 递归右子树
				if ( root->right != NULL ) {
					AVLHT64_FreeKeysRecuProc(root->right);
				}
			}
		}
		XXAPI void AVLHT64_Unit(AVLHT64_Object objHT)
		{
			AVLHT64_FreeKeysRecuProc(objHT->AVLT.RootNode);
			AVLTree_Unit(&objHT->AVLT);
		}
		
		// 设置值
		XXAPI void* AVLHT64_Set(AVLHT64_Object objHT, void* sKey, unsigned int iKeyLen)
		{
			HT64_NodeBase objKey;
			HT64_EvalHash(objKey, sKey, iKeyLen);
			HT64_NodeBase* pNode = AVLTree_AddNode(&objHT->AVLT, &objKey, NULL);
			if ( pNode ) {
				pNode->Key = mmu_malloc(iKeyLen + 1);
				pNode->KeyLen = iKeyLen;
				pNode->Hash = objKey.Hash;
				memcpy(pNode->Key, sKey, iKeyLen);
				((char*)pNode->Key)[iKeyLen] = 0;
				return &pNode[1];
			}
			return NULL;
		}
		
		// 获取值
		XXAPI void* AVLHT64_Get(AVLHT64_Object objHT, void* sKey, unsigned int iKeyLen)
		{
			HT64_NodeBase objKey;
			HT64_EvalHash(objKey, sKey, iKeyLen);
			HT64_NodeBase* pNode = AVLTree_Search(&objHT->AVLT, &objKey);
			if ( pNode ) {
				return &pNode[1];
			}
			return NULL;
		}
		
		// 删除值
		XXAPI int AVLHT64_Remove(AVLHT64_Object objHT, void* sKey, unsigned int iKeyLen)
		{
			HT64_NodeBase objKey;
			HT64_EvalHash(objKey, sKey, iKeyLen);
			return AVLTree_Remove(&objHT->AVLT, &objKey);
		}
		
		// 判断值是否存在
		XXAPI int AVLHT64_Exists(AVLHT64_Object objHT, void* sKey, unsigned int iKeyLen)
		{
			HT64_NodeBase objKey;
			HT64_EvalHash(objKey, sKey, iKeyLen);
			HT64_NodeBase* pNode = AVLTree_Search(&objHT->AVLT, &objKey);
			if ( pNode ) {
				return -1;
			}
			return 0;
		}
		
		// 获取表内元素数量
		XXAPI unsigned int AVLHT64_Count(AVLHT64_Object objHT)
		{
			return objHT->AVLT.Count;
		}
		
		// 遍历表元素
		int AVLHT64_WalkRecuProc(AVLTree_NodeBase* root, HT64_EachProc procEach, void* pArg)
		{
			if ( root ) {
				// 递归左子树
				if ( root->left != NULL ) {
					if ( AVLHT64_WalkRecuProc(root->left, procEach, pArg) ) {
						return -1;
					}
				}
				// 调用回调函数
				if ( procEach ) {
					if ( procEach(AVLTree_GetNodeData(root), ((void*)root) + sizeof(AVLTree_NodeBase) + sizeof(HT64_NodeBase), pArg) ) {
						return -1;
					}
				}
				// 递归右子树
				if ( root->right != NULL ) {
					if ( AVLHT64_WalkRecuProc(root->right, procEach, pArg) ) {
						return -1;
					}
				}
			}
			return 0;
		}
		XXAPI void AVLHT64_Walk(AVLHT64_Object objHT, HT64_EachProc procEach, void* pArg)
		{
			AVLHT64_WalkRecuProc(objHT->AVLT.RootNode, procEach, pArg);
		}
		
	#endif





	/*
		RBHT32 - RBTree Hash Table (32bit Hash Value)
	*/

	#ifdef MMU_USE_RBHT32
		
		// 创建哈希表
		XXAPI RBHT32_Object RBHT32_Create(unsigned int iItemLength)
		{
			RBHT32_Object objHT = mmu_malloc(sizeof(RBHT32_Struct));
			if ( objHT ) {
				RBHT32_Init(objHT, iItemLength);
			}
			return objHT;
		}
		
		// 销毁哈希表
		XXAPI void RBHT32_Destroy(RBHT32_Object objHT)
		{
			if ( objHT ) {
				RBHT32_Unit(objHT);
				mmu_free(objHT);
			}
		}
		
		// 初始化哈希表（对自维护结构体指针使用，和 RBHT32_Create 功能类似）
		XXAPI void RBHT32_Init(RBHT32_Object objHT, unsigned int iItemLength)
		{
			RBTree_Init(&objHT->RBT, iItemLength + sizeof(HT32_NodeBase), (void*)HT32_CompProc);
		}
		
		// 释放哈希表（对自维护结构体指针使用，和 RBHT32_Destroy 功能类似）
		void RBHT32_FreeKeysRecuProc(RBTree_NodeBase* root)
		{
			if ( root ) {
				// 递归左子树
				if ( root->left != NULL ) {
					RBHT32_FreeKeysRecuProc(root->left);
				}
				// 释放 Key
				HT32_NodeBase* pNode = RBTree_GetNodeData(root);
				mmu_free(pNode->Key);
				// 递归右子树
				if ( root->right != NULL ) {
					RBHT32_FreeKeysRecuProc(root->right);
				}
			}
		}
		XXAPI void RBHT32_Unit(RBHT32_Object objHT)
		{
			RBHT32_FreeKeysRecuProc(objHT->RBT.RootNode);
			RBTree_Unit(&objHT->RBT);
		}
		
		// 设置值
		XXAPI void* RBHT32_Set(RBHT32_Object objHT, void* sKey, unsigned int iKeyLen)
		{
			HT32_NodeBase objKey;
			HT32_EvalHash(objKey, sKey, iKeyLen);
			HT32_NodeBase* pNode = RBTree_AddNode(&objHT->RBT, &objKey, NULL);
			if ( pNode ) {
				pNode->Key = mmu_malloc(iKeyLen + 1);
				pNode->KeyLen = iKeyLen;
				pNode->Hash = objKey.Hash;
				memcpy(pNode->Key, sKey, iKeyLen);
				((char*)pNode->Key)[iKeyLen] = 0;
				return &pNode[1];
			}
			return NULL;
		}
		
		// 获取值
		XXAPI void* RBHT32_Get(RBHT32_Object objHT, void* sKey, unsigned int iKeyLen)
		{
			HT32_NodeBase objKey;
			HT32_EvalHash(objKey, sKey, iKeyLen);
			HT32_NodeBase* pNode = RBTree_Search(&objHT->RBT, &objKey);
			if ( pNode ) {
				return &pNode[1];
			}
			return NULL;
		}
		
		// 删除值
		XXAPI int RBHT32_Remove(RBHT32_Object objHT, void* sKey, unsigned int iKeyLen)
		{
			HT32_NodeBase objKey;
			HT32_EvalHash(objKey, sKey, iKeyLen);
			return RBTree_Remove(&objHT->RBT, &objKey);
		}
		
		// 判断值是否存在
		XXAPI int RBHT32_Exists(RBHT32_Object objHT, void* sKey, unsigned int iKeyLen)
		{
			HT32_NodeBase objKey;
			HT32_EvalHash(objKey, sKey, iKeyLen);
			HT32_NodeBase* pNode = RBTree_Search(&objHT->RBT, &objKey);
			if ( pNode ) {
				return -1;
			}
			return 0;
		}
		
		// 获取表内元素数量
		XXAPI unsigned int RBHT32_Count(RBHT32_Object objHT)
		{
			return objHT->RBT.Count;
		}
		
		// 遍历表元素
		int RBHT32_WalkRecuProc(RBTree_NodeBase* root, HT32_EachProc procEach, void* pArg)
		{
			if ( root ) {
				// 递归左子树
				if ( root->left != NULL ) {
					if ( RBHT32_WalkRecuProc(root->left, procEach, pArg) ) {
						return -1;
					}
				}
				// 调用回调函数
				if ( procEach ) {
					if ( procEach(RBTree_GetNodeData(root), ((void*)root) + sizeof(RBTree_NodeBase) + sizeof(HT32_NodeBase), pArg) ) {
						return -1;
					}
				}
				// 递归右子树
				if ( root->right != NULL ) {
					if ( RBHT32_WalkRecuProc(root->right, procEach, pArg) ) {
						return -1;
					}
				}
			}
			return 0;
		}
		XXAPI void RBHT32_Walk(RBHT32_Object objHT, HT32_EachProc procEach, void* pArg)
		{
			RBHT32_WalkRecuProc(objHT->RBT.RootNode, procEach, pArg);
		}
		
	#endif





	/*
		RBHT64 - RBTree Hash Table (64bit Hash Value)
	*/

	#ifdef MMU_USE_RBHT64
		
		// 创建哈希表
		XXAPI RBHT64_Object RBHT64_Create(unsigned int iItemLength)
		{
			RBHT64_Object objHT = mmu_malloc(sizeof(RBHT64_Struct));
			if ( objHT ) {
				RBHT64_Init(objHT, iItemLength);
			}
			return objHT;
		}
		
		// 销毁哈希表
		XXAPI void RBHT64_Destroy(RBHT64_Object objHT)
		{
			if ( objHT ) {
				RBHT64_Unit(objHT);
				mmu_free(objHT);
			}
		}
		
		// 初始化哈希表（对自维护结构体指针使用，和 RBHT64_Create 功能类似）
		XXAPI void RBHT64_Init(RBHT64_Object objHT, unsigned int iItemLength)
		{
			RBTree_Init(&objHT->RBT, iItemLength + sizeof(HT64_NodeBase), (void*)HT64_CompProc);
		}
		
		// 释放哈希表（对自维护结构体指针使用，和 RBHT64_Destroy 功能类似）
		void RBHT64_FreeKeysRecuProc(RBTree_NodeBase* root)
		{
			if ( root ) {
				// 递归左子树
				if ( root->left != NULL ) {
					RBHT64_FreeKeysRecuProc(root->left);
				}
				// 释放 Key
				HT64_NodeBase* pNode = RBTree_GetNodeData(root);
				mmu_free(pNode->Key);
				// 递归右子树
				if ( root->right != NULL ) {
					RBHT64_FreeKeysRecuProc(root->right);
				}
			}
		}
		XXAPI void RBHT64_Unit(RBHT64_Object objHT)
		{
			RBHT64_FreeKeysRecuProc(objHT->RBT.RootNode);
			RBTree_Unit(&objHT->RBT);
		}
		
		// 设置值
		XXAPI void* RBHT64_Set(RBHT64_Object objHT, void* sKey, unsigned int iKeyLen)
		{
			HT64_NodeBase objKey;
			HT64_EvalHash(objKey, sKey, iKeyLen);
			HT64_NodeBase* pNode = RBTree_AddNode(&objHT->RBT, &objKey, NULL);
			if ( pNode ) {
				pNode->Key = mmu_malloc(iKeyLen + 1);
				pNode->KeyLen = iKeyLen;
				pNode->Hash = objKey.Hash;
				memcpy(pNode->Key, sKey, iKeyLen);
				((char*)pNode->Key)[iKeyLen] = 0;
				return &pNode[1];
			}
			return NULL;
		}
		
		// 获取值
		XXAPI void* RBHT64_Get(RBHT64_Object objHT, void* sKey, unsigned int iKeyLen)
		{
			HT64_NodeBase objKey;
			HT64_EvalHash(objKey, sKey, iKeyLen);
			HT64_NodeBase* pNode = RBTree_Search(&objHT->RBT, &objKey);
			if ( pNode ) {
				return &pNode[1];
			}
			return NULL;
		}
		
		// 删除值
		XXAPI int RBHT64_Remove(RBHT64_Object objHT, void* sKey, unsigned int iKeyLen)
		{
			HT64_NodeBase objKey;
			HT64_EvalHash(objKey, sKey, iKeyLen);
			return RBTree_Remove(&objHT->RBT, &objKey);
		}
		
		// 判断值是否存在
		XXAPI int RBHT64_Exists(RBHT64_Object objHT, void* sKey, unsigned int iKeyLen)
		{
			HT64_NodeBase objKey;
			HT64_EvalHash(objKey, sKey, iKeyLen);
			HT64_NodeBase* pNode = RBTree_Search(&objHT->RBT, &objKey);
			if ( pNode ) {
				return -1;
			}
			return 0;
		}
		
		// 获取表内元素数量
		XXAPI unsigned int RBHT64_Count(RBHT64_Object objHT)
		{
			return objHT->RBT.Count;
		}
		
		// 遍历表元素
		int RBHT64_WalkRecuProc(RBTree_NodeBase* root, HT64_EachProc procEach, void* pArg)
		{
			if ( root ) {
				// 递归左子树
				if ( root->left != NULL ) {
					if ( RBHT64_WalkRecuProc(root->left, procEach, pArg) ) {
						return -1;
					}
				}
				// 调用回调函数
				if ( procEach ) {
					if ( procEach(RBTree_GetNodeData(root), ((void*)root) + sizeof(RBTree_NodeBase) + sizeof(HT64_NodeBase), pArg) ) {
						return -1;
					}
				}
				// 递归右子树
				if ( root->right != NULL ) {
					if ( RBHT64_WalkRecuProc(root->right, procEach, pArg) ) {
						return -1;
					}
				}
			}
			return 0;
		}
		XXAPI void RBHT64_Walk(RBHT64_Object objHT, HT64_EachProc procEach, void* pArg)
		{
			RBHT64_WalkRecuProc(objHT->RBT.RootNode, procEach, pArg);
		}
		
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


