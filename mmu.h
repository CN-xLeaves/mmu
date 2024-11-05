


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
	#ifdef MMU_USE_LLIST
		#define MMU_USE_MM256
	#endif
	#ifdef MMU_USE_AVLTREE
		#define MMU_USE_MM256
	#endif
	#ifdef MMU_USE_RBTREE
		#define MMU_USE_MM256
	#endif
	#ifdef MMU_USE_MP256
		#define MMU_USE_MMU256
		#define MMU_USE_BSMM
	#endif
	#ifdef MMU_USE_MP64K
		#define MMU_USE_MMU64K
		#define MMU_USE_BSMM
	#endif
	#ifdef MMU_USE_MM256
		#define MMU_USE_MMU256
		#define MMU_USE_BSMM
	#endif
	#ifdef MMU_USE_MM64K
		#define MMU_USE_MMU64K
		#define MMU_USE_BSMM
	#endif
	#ifdef MMU_USE_SDSTK
		#define MMU_USE_PAMM
	#endif
	#ifdef MMU_USE_PDSTK
		#define MMU_USE_PAMM
	#endif
	#ifdef MMU_USE_BSMM
		#define MMU_USE_PAMM
	#endif
	
	
	
	/*
		MMU : Memory Management Unit (内存管理单元)
			用于管理各种结构化与非结构化的内存，降低内存申请与释放次数，提升运行效率
	*/
	
	// 内存指针单向链表数据结构
	typedef struct MemPtr_LLNode {
		void* Ptr;
		struct MemPtr_LLNode* Next;
	} MemPtr_LLNode;
	
	// MMU256 和 MMU64K 申请内存固定的前置数据（用于识别内存是哪个管理器分配的）
	typedef struct {
		unsigned int ItemFlag;
	} MMU_Value, *MMU_ValuePtr;
	
	// 报错信息
	#define STK_ERROR_ALLOC				1
	#define MM_ERROR_CREATEMMU			2
	#define MM_ERROR_ADDMMU				3
	#define MM_ERROR_NULLMMU			4
	#define MM_ERROR_FINDFSB			5
	
	// 报错回调函数定义
	typedef void (*MMU_OnErrorProc)(void* objMM, int ErrorID);
	
	// MMU有效ID区间掩码
	#define MMU_FLAG_MASK				0x3FFFFFFF
	
	// 结构体使用状态标记
	#define MMU_FLAG_USE				0x80000000
	
	// GC回收标记位
	#define MMU_FLAG_GC					0x40000000
	
	// 非内存管理器管理的内存
	#define MMU_FLAG_EXT				0xBFFFFFFF
	
	// MM256 or MM64K GC标记
	#define MM_GC_Mark(p) ((MMU_ValuePtr)((void*)p - sizeof(MMU_Value)))->ItemFlag |= MMU_FLAG_GC
		
	// MP256 or MP64K 大内存结构体前置结构
	typedef struct {
		unsigned int Index;							// BigMM 的块索引
		unsigned int Flag;							// 符合 MM256 标准的 Flag
	} MP_MemHead;
	
	// MP256 or MP64K 大内存信息链表结构体（实际返回的内存地址相当于 Ptr + sizeof(MP_MemHead)）
	typedef struct MP_BigInfoLL {
		unsigned int Size;							// 申请内存的大小，可有可无（可开发辅助功能，如泄漏检测）
		void* Ptr;									// 指针地址，使用 mmu_malloc 返回的地址
		struct MP_BigInfoLL* Next;				// 下一个链表节点（用于释放链表）
	} MP_BigInfoLL;
	
	// 内存分配器定义
	#define mmu_realloc	realloc
	#define mmu_malloc	malloc
	#define mmu_calloc	calloc
	#define mmu_free	free
		
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
	
	// 初始化 MMU 库（预留未来使用的兼容性函数，目前为空函数）
	XXAPI int MMU_Init();
	
	// 卸载 MMU 库（预留未来使用的兼容性函数，目前为空函数）
	XXAPI void MMU_Unit();
	
	// 线程初始化 MMU 库（预留未来使用的兼容性函数，目前为空函数）
	XXAPI int MMU_Thread_Init();
	
	// 线程卸载 MMU 库（预留未来使用的兼容性函数，目前为空函数）
	XXAPI void MMU_Thread_Unit();
	
	
	
	
	
	/*
		Point Array Memory Management [指针数组内存管理器]
			成员编号规则（0为不存在的成员编号）：
				┌──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬────┐
				│01│02│03│04│05│06│07│08│09│10│11│12│ .. │
				└──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴────┘
	*/
	
	#ifdef MMU_USE_PAMM
		
		// 申请步长
		#ifndef PAMM_PREASSIGNSTEP
			#define PAMM_PREASSIGNSTEP	256
		#endif
		
		// 指针数组内存管理器数据结构
		typedef struct {
			void** Memory;							// 管理器内存指针
			unsigned int Count;						// 管理器中存在多少成员
			unsigned int AllocCount;				// 已经申请的结构数量
			unsigned int AllocStep;					// 预分配内存步长
		} PAMM_Struct, *PAMM_Object;
		
		// 创建指针内存管理器
		XXAPI PAMM_Object PAMM_Create();
		
		// 销毁指针内存管理器
		XXAPI void PAMM_Destroy(PAMM_Object pObject);
		
		// 初始化内存管理器（对自维护结构体指针使用，和 PAMM_Create 功能类似）
		XXAPI void PAMM_Init(PAMM_Object pObject);
		
		// 释放内存管理器（对自维护结构体指针使用，和 PAMM_Destroy 功能类似）
		XXAPI void PAMM_Unit(PAMM_Object pObject);
		
		// 分配内存
		XXAPI int PAMM_Malloc(PAMM_Object pObject, unsigned int iCount);
		
		// 中间插入成员(0为头部插入，pObject->Count为末尾插入)
		XXAPI unsigned int PAMM_Insert(PAMM_Object pObject, unsigned int iPos, void* pVal);
		
		// 末尾添加成员
		XXAPI unsigned int PAMM_Append(PAMM_Object pObject, void* pVal);
		
		// 添加成员，自动查找空隙（替换为 NULL 的值）
		XXAPI unsigned int PAMM_AddAlt(PAMM_Object pObject, void* pVal);
		
		// 交换成员
		XXAPI int PAMM_Swap(PAMM_Object pObject, unsigned int iPosA, unsigned int iPosB);
		
		// 删除成员
		XXAPI int PAMM_Remove(PAMM_Object pObject, unsigned int iPos, unsigned int iCount);
		
		// 获取成员指针
		XXAPI void** PAMM_GetPtr(PAMM_Object pObject, unsigned int iPos);
		XXAPI void* PAMM_GetVal(PAMM_Object pObject, unsigned int iPos);
		XXAPI void** PAMM_GetPtr_Unsafe(PAMM_Object pObject, unsigned int iPos);
		XXAPI void* PAMM_GetVal_Unsafe(PAMM_Object pObject, unsigned int iPos);
		static inline void** PAMM_GetPtr_Inline(PAMM_Object pObject, unsigned int iPos)
		{
			return &(pObject->Memory[iPos - 1]);
		}
		static inline void* PAMM_GetVal_Inline(PAMM_Object pObject, unsigned int iPos)
		{
			return pObject->Memory[iPos - 1];
		}
		
		// 设置成员指针
		XXAPI int PAMM_SetVal(PAMM_Object pObject, unsigned int iPos, void* pVal);
		XXAPI void PAMM_SetVal_Unsafe(PAMM_Object pObject, unsigned int iPos, void* pVal);
		static inline void PAMM_SetVal_Inline(PAMM_Object pObject, unsigned int iPos, void* pVal)
		{
			pObject->Memory[iPos - 1] = pVal;
		}
		
		// 成员排序
		XXAPI int PAMM_Sort(PAMM_Object pObject, void* procCompar);
		
	#endif
	
	
	
	
	
	/*
		Struct Array Memory Management [结构体数组内存管理器]
			成员编号规则（0为不存在的成员编号）：
				┌──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬────┐
				│01│02│03│04│05│06│07│08│09│10│11│12│ .. │
				└──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴────┘
	*/
	
	#ifdef MMU_USE_SAMM
		
		// 申请步长
		#ifndef SAMM_PREASSIGNSTEP
			#define SAMM_PREASSIGNSTEP	256
		#endif
		
		// 结构体数组内存管理器数据结构
		typedef struct {
			char* Memory;						// 管理器内存指针
			unsigned int ItemLength;			// 成员占用内存长度
			unsigned int Count;					// 管理器中存在多少成员
			unsigned int AllocCount;			// 已经申请的结构数量
			unsigned int AllocStep;				// 预分配内存步长
		} SAMM_Struct, *SAMM_Object;
		
		// 创建结构化内存管理器
		XXAPI SAMM_Object SAMM_Create(unsigned int iItemLength);
		
		// 销毁结构化内存管理器
		XXAPI void SAMM_Destroy(SAMM_Object pObject);
		
		// 初始化内存管理器（对自维护结构体指针使用，和 SAMM_Create 功能类似）
		XXAPI void SAMM_Init(SAMM_Object pObject, unsigned int iItemLength);
		
		// 释放内存管理器（对自维护结构体指针使用，和 SAMM_Destroy 功能类似）
		XXAPI void SAMM_Unit(SAMM_Object pObject);
		
		// 分配内存
		XXAPI int SAMM_Malloc(SAMM_Object pObject, unsigned int iCount);
		
		// 中间插入成员
		XXAPI unsigned int SAMM_Insert(SAMM_Object pObject, unsigned int iPos, unsigned int iCount);
		
		// 末尾添加成员
		XXAPI unsigned int SAMM_Append(SAMM_Object pObject, unsigned int iCount);
		
		// 交换成员
		XXAPI int SAMM_Swap(SAMM_Object pObject, unsigned int iPosA, unsigned int iPosB);
		
		// 删除成员
		XXAPI int SAMM_Remove(SAMM_Object pObject, unsigned int iPos, unsigned int iCount);
		
		// 获取成员指针
		XXAPI void* SAMM_GetPtr(SAMM_Object pObject, unsigned int iPos);
		XXAPI void* SAMM_GetPtr_Unsafe(SAMM_Object pObject, unsigned int iPos);
		static inline void* SAMM_GetPtr_Inline(SAMM_Object pObject, unsigned int iPos)
		{
			return &(pObject->Memory[(iPos - 1) * pObject->ItemLength]);
		}
		
		// 成员排序
		XXAPI int SAMM_Sort(SAMM_Object pObject, void* procCompar);
		
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
		Blocks Struct Memory Management [数据块结构内存管理器]
			成员编号规则（从0开始，非特殊需求不建议使用）：
				┌──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬────┐
				│00│01│02│03│04│05│06│07│08│09│10│11│12│ .. │
				└──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴────┘
	*/
	
	#ifdef MMU_USE_BSMM
		
		// 数据块结构内存管理器数据结构
		typedef struct {
			unsigned int ItemLength;			// 成员占用内存长度
			unsigned int Count;					// 管理器中存在多少成员
			PAMM_Struct PageMMU;				// 内存页管理器
			MemPtr_LLNode* LL_Free;				// 已释放的内存块链表
		} BSMM_Struct, *BSMM_Object;
		
		// 创建数据块结构内存管理器
		XXAPI BSMM_Object BSMM_Create(unsigned int iItemLength);
		
		// 销毁数据块结构内存管理器
		XXAPI void BSMM_Destroy(BSMM_Object objBSMM);
		
		// 初始化数据块结构内存管理器（对自维护结构体指针使用，和 BSMM_Create 功能类似）
		XXAPI void BSMM_Init(BSMM_Object objBSMM, unsigned int iItemLength);
		
		// 释放数据块结构内存管理器（对自维护结构体指针使用，和 BSMM_Destroy 功能类似）
		XXAPI void BSMM_Unit(BSMM_Object objBSMM);
		
		// 申请结构体内存
		XXAPI void* BSMM_Alloc(BSMM_Object objBSMM);
		
		// 释放结构体内存
		XXAPI void BSMM_Free(BSMM_Object objBSMM, void* Ptr);
		
		// 获取成员指针（非特殊需求不建议使用）
		static inline void* BSMM_GetPtr_Inline(BSMM_Object objBSMM, unsigned int iIdx)
		{
			unsigned int iBlock = iIdx >> 8;
			unsigned int iPos = iIdx & 0xFF;
			char* pBlock = PAMM_GetVal_Inline(&objBSMM->PageMMU, iBlock + 1);
			return &pBlock[iPos * objBSMM->ItemLength];
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
		
		// 进行一轮GC，将 标记 或 未标记 的内存全部回收
		void MMU256_GC(MMU256_Object objUnit, int bFreeMark);
		
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
		
		// 进行一轮GC，将 标记 或 未标记 的内存全部回收
		void MMU64K_GC(MMU64K_Object objMMU, int bFreeMark);
		
	#endif
	
	
	
	
	
	/*
		Memory Management 256
			内存管理器（固定成员大小的内存池，使用 MMU256 加速分配和释放）
	*/
	
	#ifdef MMU_USE_MM256
		
		// 内存管理器链表结构
		typedef struct MMU256_LLNode {
			unsigned int Flag;
			MMU256_Object objMMU;
			struct MMU256_LLNode* Prev;
			struct MMU256_LLNode* Next;
		} MMU256_LLNode;
		
		// 256步进内存管理器数据结构
		typedef struct {
			unsigned int ItemLength;					// 成员占用内存长度
			BSMM_Struct arrMMU;							// MMU 阵列
			MMU256_LLNode* LL_Idle;						// 空闲的 MMU 内存管理单元链表 (优先分配内存的单元)
			MMU256_LLNode* LL_Full;						// 满载的 MMU 内存管理单元链表 (不会从这些单元中分配内存)
			MMU256_LLNode* LL_Null;						// 全空的 MMU 内存管理单元 (备用单元，最多只留一个)
			MMU256_LLNode* LL_Free;						// 已释放的 MMU 内存管理单元链表 (申请新单元优先从这里找)
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
		#define MM256_GC_Mark	MM_GC_Mark
		
		// 进行一轮GC，将 标记 或 未标记 的内存全部回收
		XXAPI void MM256_GC(MM256_Object objMM, int bFreeMark);
		
	#endif
	
	
	
	
	
	/*
		Memory Management 64K
			内存管理器（固定成员大小的内存池，使用 MMU64K 加速分配和释放）
	*/
	
	#ifdef MMU_USE_MM64K
		
		// 内存管理器链表结构
		typedef struct MMU64K_LLNode {
			unsigned int Flag;
			MMU64K_Object objMMU;
			struct MMU64K_LLNode* Prev;
			struct MMU64K_LLNode* Next;
		} MMU64K_LLNode;
		
		// 64K步进内存管理器数据结构
		typedef struct {
			unsigned int ItemLength;					// 成员占用内存长度
			BSMM_Struct arrMMU;							// MMU 阵列
			MMU64K_LLNode* LL_Idle;						// 空闲的 MMU 内存管理单元链表 (优先分配内存的单元)
			MMU64K_LLNode* LL_Full;						// 满载的 MMU 内存管理单元链表 (不会从这些单元中分配内存)
			MMU64K_LLNode* LL_Null;						// 全空的 MMU 内存管理单元 (备用单元，最多只留一个)
			MMU64K_LLNode* LL_Free;						// 已释放的 MMU 内存管理单元链表 (申请新单元优先从这里找)
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
		#define MM64K_GC_Mark	MM_GC_Mark
		
		// 进行一轮GC，将 标记 或 未标记 的内存全部回收
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
			PAMM_Struct MMU;							// MMU 管理器
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
			PAMM_Struct MMU;							// MMU 管理器
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
		Memory Pool 256
			内存管理器（可变成员大小的内存池，使用 MM256 加速分配和释放）
	*/
	
	#ifdef MMU_USE_MP256
		
		// 单个长度区间的内存管理器结构
		typedef struct FSB256_Item {
			unsigned int MinLength;						// 支持最小的内存长度
			unsigned int MaxLength;						// 支持最大的内存长度
			MMU256_LLNode* LL_Idle;						// 空闲的 MMU 内存管理单元链表 (优先分配内存的单元)
			MMU256_LLNode* LL_Full;						// 满载的 MMU 内存管理单元链表 (不会从这些单元中分配内存)
			MMU256_LLNode* LL_Null;						// 全空的 MMU 内存管理单元 (备用单元，最多只留一个)
			MMU256_LLNode* LL_Free;						// 已释放的 MMU 内存管理单元链表 (申请新单元优先从这里找)
			struct FSB256_Item* left;						// 左子树
			struct FSB256_Item* right;						// 右子树
		} FSB256_Item;
		
		// 256步进内存池数据结构
		typedef struct {
			FSB256_Item* FSB_Memory;					// fixed-size-blocks 内存（MP256_Create参数为 1 或 2 时自动创建，否则需要手动创建，不为空会调用 mmu_free 释放）
			FSB256_Item* FSB_RootNode;					// fixed-size-blocks 二叉树（固定大小区块内存管理器阵列）
			BSMM_Struct arrMMU;							// MMU 阵列
			BSMM_Struct BigMM;							// 大内存数组
			MP_BigInfoLL* LL_BigFree;					// 大内存已释放的内存块链表
			MMU_OnErrorProc OnError;					// 错误处理回调函数
		} MP256_Struct, *MP256_Object;
		
		// 创建内存池
		XXAPI MP256_Object MP256_Create(int bCustom);
		
		// 销毁内存池
		XXAPI void MP256_Destroy(MP256_Object objMP);
		
		// 初始化内存池（对自维护结构体指针使用，和 MP256_Create 功能类似）
		XXAPI void MP256_Init(MP256_Object objMP, int bCustom);
		
		// 释放内存池（对自维护结构体指针使用，和 MP256_Destroy 功能类似）
		XXAPI void MP256_Unit(MP256_Object objMP);
		
		// 从内存池中申请一块内存
		XXAPI void* MP256_Alloc(MP256_Object objMP, unsigned int iSize);
		
		// 将内存池申请的内存释放掉
		XXAPI void MP256_Free(MP256_Object objMP, void* ptr);
		
		// 将一块内存标记为使用中
		#define MP256_GC_Mark	MM_GC_Mark
		
		// 进行一轮GC，将 标记 或 未标记 的内存全部回收
		XXAPI void MP256_GC(MP256_Object objMP, int bFreeMark);
		
	#endif
	
	
	
	
	
	/*
		Memory Pool 64K
			内存管理器（可变成员大小的内存池，使用 MM64K 加速分配和释放）
	*/
	
	#ifdef MMU_USE_MP64K
		
		// 单个长度区间的内存管理器结构
		typedef struct FSB64K_Item {
			unsigned int MinLength;						// 支持最小的内存长度
			unsigned int MaxLength;						// 支持最大的内存长度
			MMU64K_LLNode* LL_Idle;						// 空闲的 MMU 内存管理单元链表 (优先分配内存的单元)
			MMU64K_LLNode* LL_Full;						// 满载的 MMU 内存管理单元链表 (不会从这些单元中分配内存)
			MMU64K_LLNode* LL_Null;						// 全空的 MMU 内存管理单元 (备用单元，最多只留一个)
			MMU64K_LLNode* LL_Free;						// 已释放的 MMU 内存管理单元链表 (申请新单元优先从这里找)
			struct FSB64K_Item* left;						// 左子树
			struct FSB64K_Item* right;						// 右子树
		} FSB64K_Item;
		
		// 64K步进内存池数据结构
		typedef struct {
			FSB64K_Item* FSB_Memory;					// fixed-size-blocks 内存（MP64K_Create参数为 1 或 2 时自动创建，否则需要手动创建，不为空会调用 mmu_free 释放）
			FSB64K_Item* FSB_RootNode;					// fixed-size-blocks 二叉树（固定大小区块内存管理器阵列）
			BSMM_Struct arrMMU;							// MMU 阵列
			BSMM_Struct BigMM;							// 大内存数组
			MP_BigInfoLL* LL_BigFree;					// 大内存已释放的内存块链表
			MMU_OnErrorProc OnError;					// 错误处理回调函数
		} MP64K_Struct, *MP64K_Object;
		
		// 创建内存池
		XXAPI MP64K_Object MP64K_Create(int bCustom);
		
		// 销毁内存池
		XXAPI void MP64K_Destroy(MP64K_Object objMP);
		
		// 初始化内存池（对自维护结构体指针使用，和 MP64K_Create 功能类似）
		XXAPI void MP64K_Init(MP64K_Object objMP, int bCustom);
		
		// 释放内存池（对自维护结构体指针使用，和 MP64K_Destroy 功能类似）
		XXAPI void MP64K_Unit(MP64K_Object objMP);
		
		// 从内存池中申请一块内存
		XXAPI void* MP64K_Alloc(MP64K_Object objMP, unsigned int iSize);
		
		// 将内存池申请的内存释放掉
		XXAPI void MP64K_Free(MP64K_Object objMP, void* ptr);
		
		// 将一块内存标记为使用中
		#define MP64K_GC_Mark	MM_GC_Mark
		
		// 进行一轮GC，将 标记 或 未标记 的内存全部回收
		XXAPI void MP64K_GC(MP64K_Object objMP, int bFreeMark);
		
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


