


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
	
	
	
	
	
	// 初始化 MMU 库（预留未来使用的兼容性函数，目前为空函数）
	XXAPI int MMU_Init()
	{
		return -1;
	}

	// 卸载 MMU 库（预留未来使用的兼容性函数，目前为空函数）
	XXAPI void MMU_Unit()
	{
		
	}

	// 线程初始化 MMU 库（预留未来使用的兼容性函数，目前为空函数）
	XXAPI int MMU_Thread_Init()
	{
		return -1;
	}

	// 线程卸载 MMU 库（预留未来使用的兼容性函数，目前为空函数）
	XXAPI void MMU_Thread_Unit()
	{
		
	}





	/*
		Point Memory Management Unit [指针内存管理单元]
			成员编号规则（0为不存在的成员编号）：
				┌──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬────┐
				│01│02│03│04│05│06│07│08│09│10│11│12│ .. │
				└──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴────┘
	*/

	#ifdef MMU_USE_PAMM
		
		// 创建指针内存管理器
		XXAPI PAMM_Object PAMM_Create()
		{
			PAMM_Object ObjPtr = mmu_malloc(sizeof(PAMM_Struct));
			if ( ObjPtr ) {
				PAMM_Init(ObjPtr);
			}
			return ObjPtr;
		}
		
		// 销毁指针内存管理器
		XXAPI void PAMM_Destroy(PAMM_Object pObject)
		{
			if ( pObject ) {
				PAMM_Unit(pObject);
				mmu_free(pObject);
			}
		}
		
		// 初始化内存管理器（对自维护结构体指针使用，和 PAMM_Create 功能类似）
		XXAPI void PAMM_Init(PAMM_Object pObject)
		{
			pObject->Memory = NULL;
			pObject->Count = 0;
			pObject->AllocCount = 0;
			pObject->AllocStep = PAMM_PREASSIGNSTEP;
		}
		
		// 释放内存管理器（对自维护结构体指针使用，和 PAMM_Destroy 功能类似）
		XXAPI void PAMM_Unit(PAMM_Object pObject)
		{
			if ( pObject->Memory ) { mmu_free(pObject->Memory); pObject->Memory = NULL; }
			pObject->Count = 0;
			pObject->AllocCount = 0;
		}
		
		// 分配内存
		XXAPI int PAMM_Malloc(PAMM_Object pObject, unsigned int iCount)
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
				PAMM_Unit(pObject);
				return -1;
			} else {
				// 不变
				return -1;
			}
			return 0;
		}
		
		// 中间插入成员(0为头部插入，pObject->Count为末尾插入)
		XXAPI unsigned int PAMM_Insert(PAMM_Object pObject, unsigned int iPos, void* pVal)
		{
			// 分配内存
			if ( pObject->Count >= pObject->AllocCount ) {
				if ( PAMM_Malloc(pObject, pObject->Count + pObject->AllocStep) == 0 ) {
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
		XXAPI unsigned int PAMM_Append(PAMM_Object pObject, void* pVal)
		{
			return PAMM_Insert(pObject, pObject->Count, pVal);
		}
		
		// 添加成员，自动查找空隙（替换为 NULL 的值）
		XXAPI unsigned int PAMM_AddAlt(PAMM_Object pObject, void* pVal)
		{
			for ( int i = 0; i < pObject->Count; i++ ) {
				if ( pObject->Memory[i] == NULL ) {
					pObject->Memory[i] = pVal;
					return i + 1;
				}
			}
			return PAMM_Insert(pObject, pObject->Count, pVal);
		}
		
		// 交换成员
		XXAPI int PAMM_Swap(PAMM_Object pObject, unsigned int iPosA, unsigned int iPosB)
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
		XXAPI int PAMM_Remove(PAMM_Object pObject, unsigned int iPos, unsigned int iCount)
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
		XXAPI void** PAMM_GetPtr(PAMM_Object pObject, unsigned int iPos)
		{
			if ( iPos ) {
				iPos--;
				if ( iPos < pObject->Count ) {
					return &(pObject->Memory[iPos]);
				}
			}
			return 0;
		}
		XXAPI void* PAMM_GetVal(PAMM_Object pObject, unsigned int iPos)
		{
			if ( iPos ) {
				iPos--;
				if ( iPos < pObject->Count ) {
					return pObject->Memory[iPos];
				}
			}
			return 0;
		}
		XXAPI void** PAMM_GetPtr_Unsafe(PAMM_Object pObject, unsigned int iPos)
		{
			return &(pObject->Memory[iPos - 1]);
		}
		XXAPI void* PAMM_GetVal_Unsafe(PAMM_Object pObject, unsigned int iPos)
		{
			return pObject->Memory[iPos - 1];
		}
		
		// 设置成员指针
		XXAPI int PAMM_SetVal(PAMM_Object pObject, unsigned int iPos, void* pVal)
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
		XXAPI void PAMM_SetVal_Unsafe(PAMM_Object pObject, unsigned int iPos, void* pVal)
		{
			pObject->Memory[iPos - 1] = pVal;
		}
		
		// 成员排序
		XXAPI int PAMM_Sort(PAMM_Object pObject, void* procCompar)
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
		Struct Memory Management Unit [结构体内存管理单元]
			成员编号规则（0为不存在的成员编号）：
				┌──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬────┐
				│01│02│03│04│05│06│07│08│09│10│11│12│ .. │
				└──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴────┘
	*/

	#ifdef MMU_USE_SAMM
		
		// 创建结构化内存管理器
		XXAPI SAMM_Object SAMM_Create(unsigned int iItemLength)
		{
			SAMM_Object ObjPtr = mmu_malloc(sizeof(SAMM_Struct));
			if ( ObjPtr ) {
				SAMM_Init(ObjPtr, iItemLength);
			}
			return ObjPtr;
		}
		
		// 销毁结构化内存管理器
		XXAPI void SAMM_Destroy(SAMM_Object pObject)
		{
			if ( pObject ) {
				SAMM_Unit(pObject);
				mmu_free(pObject);
			}
		}
		
		// 初始化内存管理器（结构体使用，和 SAMM_Create 功能类似）
		XXAPI void SAMM_Init(SAMM_Object pObject, unsigned int iItemLength)
		{
			pObject->Memory = 0;
			pObject->ItemLength = iItemLength;
			pObject->Count = 0;
			pObject->AllocCount = 0;
			pObject->AllocStep = SAMM_PREASSIGNSTEP;
		}
		
		// 释放内存管理器（对自维护结构体指针使用，和 SAMM_Destroy 功能类似）
		XXAPI void SAMM_Unit(SAMM_Object pObject)
		{
			if ( pObject->Memory ) { mmu_free(pObject->Memory); pObject->Memory = NULL; }
			pObject->Count = 0;
			pObject->AllocCount = 0;
		}
		
		// 分配内存
		XXAPI int SAMM_Malloc(SAMM_Object pObject, unsigned int iCount)
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
				SAMM_Unit(pObject);
				return -1;
			} else {
				// 不变
				return -1;
			}
			return 0;
		}
		
		// 中间插入成员
		XXAPI unsigned int SAMM_Insert(SAMM_Object pObject, unsigned int iPos, unsigned int iCount)
		{
			// 不能添加 0 个成员
			if ( iCount == 0 ) { iCount = 1; }
			// 分配内存
			if ( (pObject->Count + iCount) > pObject->AllocCount ) {
				int AddCount = pObject->AllocStep;
				if ( iCount > AddCount ) {
					AddCount += iCount;
				}
				if ( SAMM_Malloc(pObject, pObject->Count + AddCount) == 0 ) {
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
		XXAPI unsigned int SAMM_Append(SAMM_Object pObject, unsigned int iCount)
		{
			return SAMM_Insert(pObject, pObject->Count, iCount);
		}
		
		// 交换成员
		XXAPI int SAMM_Swap(SAMM_Object pObject, unsigned int iPosA, unsigned int iPosB)
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
		XXAPI int SAMM_Remove(SAMM_Object pObject, unsigned int iPos, unsigned int iCount)
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
		XXAPI void* SAMM_GetPtr(SAMM_Object pObject, unsigned int iPos)
		{
			if ( iPos ) {
				iPos--;
				if ( iPos < pObject->Count ) {
					return &pObject->Memory[iPos * pObject->ItemLength];
				}
			}
			return 0;
		}
		XXAPI void* SAMM_GetPtr_Unsafe(SAMM_Object pObject, unsigned int iPos)
		{
			return &(pObject->Memory[(iPos - 1) * pObject->ItemLength]);
		}
		
		// 成员排序
		XXAPI int SAMM_Sort(SAMM_Object pObject, void* procCompar)
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
		Blocks Struct Memory Management [数据块结构内存管理器]
			成员编号规则（0为不存在的成员编号）：
				┌──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬──┬────┐
				│01│02│03│04│05│06│07│08│09│10│11│12│ .. │
				└──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴──┴────┘
	*/

	#ifdef MMU_USE_BSMM
		
		// 创建数据块结构内存管理器
		XXAPI BSMM_Object BSMM_Create(unsigned int iItemLength)
		{
			BSMM_Object objBSMM = mmu_malloc(sizeof(BSMM_Struct));
			if ( objBSMM ) {
				BSMM_Init(objBSMM, iItemLength);
			}
			return objBSMM;
		}
		
		// 销毁数据块结构内存管理器
		XXAPI void BSMM_Destroy(BSMM_Object objBSMM)
		{
			if ( objBSMM ) {
				BSMM_Unit(objBSMM);
				mmu_free(objBSMM);
			}
		}
		
		// 初始化数据块结构内存管理器（对自维护结构体指针使用，和 BSMM_Create 功能类似）
		XXAPI void BSMM_Init(BSMM_Object objBSMM, unsigned int iItemLength)
		{
			objBSMM->ItemLength = iItemLength;
			objBSMM->Count = 0;
			PAMM_Init(&objBSMM->PageMMU);
			objBSMM->LL_Free = NULL;
		}
		
		// 释放数据块结构内存管理器（对自维护结构体指针使用，和 BSMM_Destroy 功能类似）
		XXAPI void BSMM_Unit(BSMM_Object objBSMM)
		{
			objBSMM->Count = 0;
			PAMM_Unit(&objBSMM->PageMMU);
			// 循环释放空闲内存块链表
			MemPtr_LLNode* pNode = objBSMM->LL_Free;
			while ( pNode ) {
				MemPtr_LLNode* pNext = pNode->Next;
				mmu_free(pNode);
				pNode = pNext;
			}
			objBSMM->LL_Free = NULL;
		}
		
		// 申请结构体内存
		XXAPI void* BSMM_Alloc(BSMM_Object objBSMM)
		{
			if ( objBSMM->LL_Free ) {
				// 有空闲内存块先用空闲的
				void* Ptr = objBSMM->LL_Free->Ptr;
				MemPtr_LLNode* pNext = objBSMM->LL_Free->Next;
				mmu_free(objBSMM->LL_Free);
				objBSMM->LL_Free = pNext;
				return Ptr;
			} else {
				// 需要申请新的内存块
				if ( objBSMM->Count >= (objBSMM->PageMMU.Count * 256) ) {
					char* pBlock = mmu_malloc(objBSMM->ItemLength * 256);
					if ( pBlock == NULL ) {
						return NULL;
					}
					int iIdx = PAMM_Append(&objBSMM->PageMMU, pBlock);
					if ( iIdx == 0 ) {
						mmu_free(pBlock);
						return NULL;
					}
				}
				// 从内存块中分配值
				unsigned int iBlock = objBSMM->Count >> 8;
				unsigned int iPos = objBSMM->Count & 0xFF;
				char* pBlock = PAMM_GetVal_Inline(&objBSMM->PageMMU, iBlock + 1);
				objBSMM->Count++;
				return &pBlock[iPos * objBSMM->ItemLength];
			}
		}
		
		// 释放结构体内存
		XXAPI void BSMM_Free(BSMM_Object objBSMM, void* Ptr)
		{
			MemPtr_LLNode* pNode = mmu_malloc(sizeof(MemPtr_LLNode));
			pNode->Ptr = Ptr;
			pNode->Next = objBSMM->LL_Free;
			objBSMM->LL_Free = pNode;
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
			MMU256_Object objUnit = mmu_malloc(sizeof(MMU256_Struct) + (256 * iItemLength) + 3);
			if ( objUnit ) {
				// 处理内存对齐
				if ( (intptr_t)objUnit & 3 ) {
					char* pTemp = (char*)&objUnit[1];
					objUnit->Memory = &pTemp[(intptr_t)objUnit & 3];
				} else {
					objUnit->Memory = (char*)&objUnit[1];
				}
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
		
		// 进行一轮GC，将 标记 或 未标记 的内存全部回收
		void MMU256_GC(MMU256_Object objUnit, int bFreeMark)
		{
			if ( objUnit && (objUnit->Count > 0) ) {
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
			MMU64K_Object objUnit = mmu_malloc(sizeof(MMU64K_Struct) + (65536 * iItemLength) + 3);
			if ( objUnit ) {
				// 处理内存对齐
				if ( (intptr_t)objUnit & 3 ) {
					char* pTemp = (char*)&objUnit[1];
					objUnit->Memory = &pTemp[(intptr_t)objUnit & 3];
				} else {
					objUnit->Memory = (char*)&objUnit[1];
				}
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
		
		// 进行一轮GC，将 标记 或 未标记 的内存全部回收
		void MMU64K_GC(MMU64K_Object objMMU, int bFreeMark)
		{
			if ( objMMU && (objMMU->Count > 0) ) {
				if ( bFreeMark ) {
					// 被标记的内存将被回收
					for ( int idx = 0; idx < 65536; idx++ ) {
						MMU_ValuePtr v = (MMU_ValuePtr)&(objMMU->Memory[objMMU->ItemLength * idx]);
						if ( v->ItemFlag & MMU_FLAG_USE ) {
							if ( v->ItemFlag & MMU_FLAG_GC ) {
								MMU64K_FreeIdx_Inline(objMMU, idx);
								v->ItemFlag = 0;
							}
						}
					}
				} else {
					// 未被标记的内存将被回收
					for ( int idx = 0; idx < 65536; idx++ ) {
						MMU_ValuePtr v = (MMU_ValuePtr)&(objMMU->Memory[objMMU->ItemLength * idx]);
						if ( v->ItemFlag & MMU_FLAG_USE ) {
							if ( v->ItemFlag & MMU_FLAG_GC ) {
								v->ItemFlag &= ~MMU_FLAG_GC;
							} else {
								MMU64K_FreeIdx_Inline(objMMU, idx);
								v->ItemFlag = 0;
							}
						}
					}
				}
			}
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
			BSMM_Init(&objMM->arrMMU, sizeof(MMU256_LLNode));
			objMM->arrMMU.PageMMU.AllocStep = 64;
			objMM->LL_Idle = NULL;
			objMM->LL_Full = NULL;
			objMM->LL_Null = NULL;
			objMM->LL_Free = NULL;
			objMM->OnError = NULL;
		}
		
		// 释放内存管理器（对自维护结构体指针使用，和 MM256_Destroy 功能类似）
		XXAPI void MM256_Unit(MM256_Object objMM)
		{
			for ( int i = 0; i < objMM->arrMMU.Count; i++ ) {
				MMU256_LLNode* pNode = BSMM_GetPtr_Inline(&objMM->arrMMU, i);
				if ( pNode->objMMU ) {
					MMU256_Destroy(pNode->objMMU);
				}
			}
			BSMM_Unit(&objMM->arrMMU);
			objMM->LL_Idle = NULL;
			objMM->LL_Full = NULL;
			objMM->LL_Null = NULL;
			objMM->LL_Free = NULL;
		}
		
		// 从内存管理器中申请一块内存
		XXAPI void* MM256_Alloc(MM256_Object objMM)
		{
			MMU256_Object objMMU = NULL;
			if ( objMM->LL_Idle == NULL ) {
				// 如果没有空闲的内存管理单元，优先使用备用的全空单元，或创建一个新的单元
				if ( objMM->LL_Null ) {
					// 使用备用的全空内存管理单元
					objMMU = objMM->LL_Null->objMMU;
					objMM->LL_Idle = objMM->LL_Null;
					objMM->LL_Null = NULL;
				} else if ( objMM->LL_Free ) {
					// 创建新的内存管理单元，使用已释放的内存管理单元位置
					objMMU = MMU256_Create(objMM->ItemLength);
					if ( (objMMU == NULL) && objMM->OnError ) {
						objMM->OnError(objMM, MM_ERROR_CREATEMMU);
						return NULL;
					}
					// 恢复Flag，写入新申请的单元
					MMU256_LLNode* pNode = objMM->LL_Free;
					objMMU->Flag = pNode->Flag;
					pNode->objMMU = objMMU;
					// 从 LL_Free 中移除
					if ( pNode->Next ) {
						pNode->Next->Prev = NULL;
					}
					objMM->LL_Free = pNode->Next;
					// 添加到 LL_Idle
					pNode->Prev = NULL;
					pNode->Next = NULL;
					objMM->LL_Idle = pNode;
				} else {
					// 创建新的内存管理单元，创建失败就报错处理
					objMMU = MMU256_Create(objMM->ItemLength);
					if ( (objMMU == NULL) && objMM->OnError ) {
						objMM->OnError(objMM, MM_ERROR_CREATEMMU);
						return NULL;
					}
					// 将创建好的内存管理单元添加到单元阵列管理器，添加失败就报错处理
					MMU256_LLNode* pNode = BSMM_Alloc(&objMM->arrMMU);
					if ( pNode ) {
						pNode->objMMU = objMMU;
						pNode->Prev = NULL;
						pNode->Next = NULL;
						pNode->Flag = MMU_FLAG_USE | ((objMM->arrMMU.Count - 1) << 8);
						objMM->LL_Idle = pNode;
						// 标记内存管理器单元的 Flag
						objMMU->Flag = pNode->Flag;
					} else {
						MMU256_Destroy(objMMU);
						if ( objMM->OnError ) {
							objMM->OnError(objMM, MM_ERROR_ADDMMU);
							return NULL;
						}
					}
				}
			} else {
				// 有空闲的内存管理单元，优先使用空闲的
				objMMU = objMM->LL_Idle->objMMU;
				// 如果空闲的内存管理单元即将满了，将它转移到满载单元链表
				if ( objMMU->Count >= 255 ) {
					MMU256_LLNode* pNode = objMM->LL_Idle;
					// 从 LL_Idle 中移除
					if ( pNode->Next ) {
						pNode->Next->Prev = NULL;
					}
					objMM->LL_Idle = pNode->Next;
					// 添加到 LL_Full
					pNode->Prev = NULL;
					pNode->Next = objMM->LL_Full;
					if ( objMM->LL_Full ) {
						objMM->LL_Full->Prev = pNode;
					}
					objMM->LL_Full = pNode;
				}
			}
			// 从选定内存管理器单元中申请内存块
			return MMU256_Alloc_Inline(objMMU);
		}
		
		// 将内存管理器申请的内存释放掉
		static inline void MM256_LLNode_ClearCheck(MM256_Object objMM, MMU256_LLNode* pNode, int bLL_Full)
		{
			// 如果这个内存管理单元已经清空
			if ( pNode->objMMU->Count == 0 ) {
				if ( objMM->LL_Null ) {
					// 有备用单元时，直接释放掉这个单元
					MMU256_Destroy(pNode->objMMU);
					pNode->objMMU = NULL;
					// 从 LL_Idle 或 LL_Full 中移除
					if ( pNode->Prev ) {
						pNode->Prev->Next = pNode->Next;
					} else {
						if ( bLL_Full ) {
							objMM->LL_Full = pNode->Next;
						} else {
							objMM->LL_Idle = pNode->Next;
						}
					}
					if ( pNode->Next ) {
						pNode->Next->Prev = pNode->Prev;
					}
					// 添加到 LL_Free
					pNode->Prev = NULL;
					pNode->Next = objMM->LL_Free;
					if ( objMM->LL_Free ) {
						objMM->LL_Free->Prev = pNode;
					}
					objMM->LL_Free = pNode;
				} else {
					// 没有备用单元时，让这个单元备用，避免临界状态反复申请和释放内存管理单元，造成性能损失
					// 从 LL_Idle 或 LL_Full 中移除
					if ( pNode->Prev ) {
						pNode->Prev->Next = pNode->Next;
					} else {
						if ( bLL_Full ) {
							objMM->LL_Full = pNode->Next;
						} else {
							objMM->LL_Idle = pNode->Next;
						}
					}
					if ( pNode->Next ) {
						pNode->Next->Prev = pNode->Prev;
					}
					// 添加到 LL_Null
					objMM->LL_Null = pNode;
					pNode->Prev = NULL;
					pNode->Next = NULL;
				}
			}
		}
		static inline void MM256_LLNode_IdleCheck(MM256_Object objMM, MMU256_LLNode* pNode)
		{
			if ( pNode->objMMU->Count < 256 ) {
				// 从 LL_Full 中移除
				if ( pNode->Prev ) {
					pNode->Prev->Next = pNode->Next;
				} else {
					objMM->LL_Full = pNode->Next;
				}
				if ( pNode->Next ) {
					pNode->Next->Prev = pNode->Prev;
				}
				// 添加到 LL_Idle
				pNode->Prev = NULL;
				pNode->Next = objMM->LL_Idle;
				if ( objMM->LL_Idle ) {
					objMM->LL_Idle->Prev = pNode;
				}
				objMM->LL_Idle = pNode;
			}
		}
		XXAPI void MM256_Free(MM256_Object objMM, void* ptr)
		{
			MMU_ValuePtr v = ptr - sizeof(MMU_Value);
			if ( v->ItemFlag & MMU_FLAG_USE ) {
				int iMMU = (v->ItemFlag & MMU_FLAG_MASK) >> 8;
				unsigned char idx = v->ItemFlag & 0xFF;
				// 获取对应的内存管理器单元链表结构
				MMU256_LLNode* pNode = BSMM_GetPtr_Inline(&objMM->arrMMU, iMMU);
				if ( (pNode->objMMU == NULL) && objMM->OnError ) {
					objMM->OnError(objMM, MM_ERROR_NULLMMU);
					return;
				}
				// 调用对应 MMU 的释放函数
				MMU256_FreeIdx_Inline(pNode->objMMU, idx);
				v->ItemFlag = 0;
				// 如果是一个满载的内存管理器单元，将它放入空闲单元列表
				if ( pNode->objMMU->Count >= 255 ) {
					MM256_LLNode_IdleCheck(objMM, pNode);
				}
				// 如果这个内存管理单元已经清空，将他释放或变为备用单元
				MM256_LLNode_ClearCheck(objMM, pNode, 0);
			}
		}
		
		// 进行一轮GC，将未标记为使用中的内存全部回收
		XXAPI void MM256_GC(MM256_Object objMM, int bFreeMark)
		{
			// 遍历所有 空闲的 和 满载的 内存管理单元，进行标记回收
			MMU256_LLNode* pNode = objMM->LL_Idle;
			while ( pNode ) {
				MMU256_GC(pNode->objMMU, bFreeMark);
				pNode = pNode->Next;
			}
			pNode = objMM->LL_Full;
			while ( pNode ) {
				MMU256_GC(pNode->objMMU, bFreeMark);
				pNode = pNode->Next;
			}
			// 再次遍历所有 空闲的 和 满载的 内存管理单元，将他们归类到正确的分组
			pNode = objMM->LL_Idle;
			while ( pNode ) {
				MMU256_LLNode* pNext = pNode->Next;
				MM256_LLNode_ClearCheck(objMM, pNode, 0);
				pNode = pNext;
			}
			pNode = objMM->LL_Full;
			while ( pNode ) {
				MMU256_LLNode* pNext = pNode->Next;
				if ( pNode->objMMU->Count == 0 ) {
					MM256_LLNode_ClearCheck(objMM, pNode, -1);
				} else {
					MM256_LLNode_IdleCheck(objMM, pNode);
				}
				pNode = pNext;
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
			BSMM_Init(&objMM->arrMMU, sizeof(MMU64K_LLNode));
			objMM->arrMMU.PageMMU.AllocStep = 64;
			objMM->LL_Idle = NULL;
			objMM->LL_Full = NULL;
			objMM->LL_Null = NULL;
			objMM->LL_Free = NULL;
			objMM->OnError = NULL;
		}
		
		// 释放内存管理器（对自维护结构体指针使用，和 MM64K_Destroy 功能类似）
		XXAPI void MM64K_Unit(MM64K_Object objMM)
		{
			for ( int i = 0; i < objMM->arrMMU.Count; i++ ) {
				MMU64K_LLNode* pNode = BSMM_GetPtr_Inline(&objMM->arrMMU, i);
				if ( pNode->objMMU ) {
					MMU64K_Destroy(pNode->objMMU);
				}
			}
			BSMM_Unit(&objMM->arrMMU);
			objMM->LL_Idle = NULL;
			objMM->LL_Full = NULL;
			objMM->LL_Null = NULL;
			objMM->LL_Free = NULL;
		}
		
		// 从内存管理器中申请一块内存
		XXAPI void* MM64K_Alloc(MM64K_Object objMM)
		{
			MMU64K_Object objMMU = NULL;
			if ( objMM->LL_Idle == NULL ) {
				// 如果没有空闲的内存管理单元，优先使用备用的全空单元，或创建一个新的单元
				if ( objMM->LL_Null ) {
					// 使用备用的全空内存管理单元
					objMMU = objMM->LL_Null->objMMU;
					objMM->LL_Idle = objMM->LL_Null;
					objMM->LL_Null = NULL;
				} else if ( objMM->LL_Free ) {
					// 创建新的内存管理单元，使用已释放的内存管理单元位置
					objMMU = MMU64K_Create(objMM->ItemLength);
					if ( (objMMU == NULL) && objMM->OnError ) {
						objMM->OnError(objMM, MM_ERROR_CREATEMMU);
						return NULL;
					}
					// 恢复Flag，写入新申请的单元
					MMU64K_LLNode* pNode = objMM->LL_Free;
					objMMU->Flag = pNode->Flag;
					pNode->objMMU = objMMU;
					// 从 LL_Free 中移除
					if ( pNode->Next ) {
						pNode->Next->Prev = NULL;
					}
					objMM->LL_Free = pNode->Next;
					// 添加到 LL_Idle
					pNode->Prev = NULL;
					pNode->Next = NULL;
					objMM->LL_Idle = pNode;
				} else {
					// 创建新的内存管理单元，创建失败就报错处理
					objMMU = MMU64K_Create(objMM->ItemLength);
					if ( (objMMU == NULL) && objMM->OnError ) {
						objMM->OnError(objMM, MM_ERROR_CREATEMMU);
						return NULL;
					}
					// 将创建好的内存管理单元添加到单元阵列管理器，添加失败就报错处理
					MMU64K_LLNode* pNode = BSMM_Alloc(&objMM->arrMMU);
					if ( pNode ) {
						pNode->objMMU = objMMU;
						pNode->Prev = NULL;
						pNode->Next = NULL;
						pNode->Flag = MMU_FLAG_USE | ((objMM->arrMMU.Count - 1) << 16);
						objMM->LL_Idle = pNode;
						// 标记内存管理器单元的 Flag
						objMMU->Flag = pNode->Flag;
					} else {
						MMU64K_Destroy(objMMU);
						if ( objMM->OnError ) {
							objMM->OnError(objMM, MM_ERROR_ADDMMU);
							return NULL;
						}
					}
				}
			} else {
				// 有空闲的内存管理单元，优先使用空闲的
				objMMU = objMM->LL_Idle->objMMU;
				// 如果空闲的内存管理单元即将满了，将它转移到满载单元链表
				if ( objMMU->Count >= 65535 ) {
					MMU64K_LLNode* pNode = objMM->LL_Idle;
					// 从 LL_Idle 中移除
					if ( pNode->Next ) {
						pNode->Next->Prev = NULL;
					}
					objMM->LL_Idle = pNode->Next;
					// 添加到 LL_Full
					pNode->Prev = NULL;
					pNode->Next = objMM->LL_Full;
					if ( objMM->LL_Full ) {
						objMM->LL_Full->Prev = pNode;
					}
					objMM->LL_Full = pNode;
				}
			}
			// 从选定内存管理器单元中申请内存块
			return MMU64K_Alloc_Inline(objMMU);
		}
		
		// 将内存管理器申请的内存释放掉
		static inline void MM64K_LLNode_ClearCheck(MM64K_Object objMM, MMU64K_LLNode* pNode, int bLL_Full)
		{
			// 如果这个内存管理单元已经清空
			if ( pNode->objMMU->Count == 0 ) {
				if ( objMM->LL_Null ) {
					// 有备用单元时，直接释放掉这个单元
					MMU64K_Destroy(pNode->objMMU);
					pNode->objMMU = NULL;
					// 从 LL_Idle 或 LL_Full 中移除
					if ( pNode->Prev ) {
						pNode->Prev->Next = pNode->Next;
					} else {
						if ( bLL_Full ) {
							objMM->LL_Full = pNode->Next;
						} else {
							objMM->LL_Idle = pNode->Next;
						}
					}
					if ( pNode->Next ) {
						pNode->Next->Prev = pNode->Prev;
					}
					// 添加到 LL_Free
					pNode->Prev = NULL;
					pNode->Next = objMM->LL_Free;
					if ( objMM->LL_Free ) {
						objMM->LL_Free->Prev = pNode;
					}
					objMM->LL_Free = pNode;
				} else {
					// 没有备用单元时，让这个单元备用，避免临界状态反复申请和释放内存管理单元，造成性能损失
					// 从 LL_Idle 或 LL_Full 中移除
					if ( pNode->Prev ) {
						pNode->Prev->Next = pNode->Next;
					} else {
						if ( bLL_Full ) {
							objMM->LL_Full = pNode->Next;
						} else {
							objMM->LL_Idle = pNode->Next;
						}
					}
					if ( pNode->Next ) {
						pNode->Next->Prev = pNode->Prev;
					}
					// 添加到 LL_Null
					objMM->LL_Null = pNode;
					pNode->Prev = NULL;
					pNode->Next = NULL;
				}
			}
		}
		static inline void MM64K_LLNode_IdleCheck(MM64K_Object objMM, MMU64K_LLNode* pNode)
		{
			if ( pNode->objMMU->Count < 65536 ) {
				// 从 LL_Full 中移除
				if ( pNode->Prev ) {
					pNode->Prev->Next = pNode->Next;
				} else {
					objMM->LL_Full = pNode->Next;
				}
				if ( pNode->Next ) {
					pNode->Next->Prev = pNode->Prev;
				}
				// 添加到 LL_Idle
				pNode->Prev = NULL;
				pNode->Next = objMM->LL_Idle;
				if ( objMM->LL_Idle ) {
					objMM->LL_Idle->Prev = pNode;
				}
				objMM->LL_Idle = pNode;
			}
		}
		XXAPI void MM64K_Free(MM64K_Object objMM, void* ptr)
		{
			MMU_ValuePtr v = ptr - sizeof(MMU_Value);
			if ( v->ItemFlag & MMU_FLAG_USE ) {
				int iMMU = (v->ItemFlag & MMU_FLAG_MASK) >> 16;
				unsigned char idx = v->ItemFlag & 0xFFFF;
				// 获取对应的内存管理器单元链表结构
				MMU64K_LLNode* pNode = BSMM_GetPtr_Inline(&objMM->arrMMU, iMMU);
				if ( (pNode->objMMU == NULL) && objMM->OnError ) {
					objMM->OnError(objMM, MM_ERROR_NULLMMU);
					return;
				}
				// 调用对应 MMU 的释放函数
				MMU64K_FreeIdx_Inline(pNode->objMMU, idx);
				v->ItemFlag = 0;
				// 如果是一个满载的内存管理器单元，将它放入空闲单元列表
				if ( pNode->objMMU->Count >= 65535 ) {
					MM64K_LLNode_IdleCheck(objMM, pNode);
				}
				// 如果这个内存管理单元已经清空，将他释放或变为备用单元
				MM64K_LLNode_ClearCheck(objMM, pNode, 0);
			}
		}
		
		// 进行一轮GC，将未标记为使用中的内存全部回收
		XXAPI void MM64K_GC(MM64K_Object objMM, int bFreeMark)
		{
			// 遍历所有 空闲的 和 满载的 内存管理单元，进行标记回收
			MMU64K_LLNode* pNode = objMM->LL_Idle;
			while ( pNode ) {
				MMU64K_GC(pNode->objMMU, bFreeMark);
				pNode = pNode->Next;
			}
			pNode = objMM->LL_Full;
			while ( pNode ) {
				MMU64K_GC(pNode->objMMU, bFreeMark);
				pNode = pNode->Next;
			}
			// 再次遍历所有 空闲的 和 满载的 内存管理单元，将他们归类到正确的分组
			pNode = objMM->LL_Idle;
			while ( pNode ) {
				MMU64K_LLNode* pNext = pNode->Next;
				MM64K_LLNode_ClearCheck(objMM, pNode, 0);
				pNode = pNext;
			}
			pNode = objMM->LL_Full;
			while ( pNode ) {
				MMU64K_LLNode* pNext = pNode->Next;
				if ( pNode->objMMU->Count == 0 ) {
					MM64K_LLNode_ClearCheck(objMM, pNode, -1);
				} else {
					MM64K_LLNode_IdleCheck(objMM, pNode);
				}
				pNode = pNext;
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
			PAMM_Init(&objSTK->MMU);
			objSTK->MMU.AllocStep = 64;
			objSTK->OnError = NULL;
		}
		
		// 释放结构体动态栈（对自维护结构体指针使用，和 SDSTK_Create 功能类似）
		XXAPI void SDSTK_Unit(SDSTK_Object objSTK)
		{
			objSTK->Count = 0;
			PAMM_Unit(&objSTK->MMU);
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
						objSTK->OnError(objSTK, STK_ERROR_ALLOC);
					}
					return NULL;
				}
				unsigned int idx = PAMM_Append(&objSTK->MMU, pBlock);
				// !!! 错误处理 !!! 无法将内存添加到内存阵列
				if ( idx == 0 ) {
					if ( objSTK->OnError ) {
						objSTK->OnError(objSTK, MM_ERROR_ADDMMU);
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
			PAMM_Init(&objSTK->MMU);
			objSTK->MMU.AllocStep = 64;
			objSTK->OnError = NULL;
		}
		
		// 释放指针动态栈（对自维护结构体指针使用，和 PDSTK_Create 功能类似）
		XXAPI void PDSTK_Unit(PDSTK_Object objSTK)
		{
			objSTK->Count = 0;
			PAMM_Unit(&objSTK->MMU);
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
						objSTK->OnError(objSTK, STK_ERROR_ALLOC);
					}
					return 0;
				}
				unsigned int idx = PAMM_Append(&objSTK->MMU, pBlock);
				// !!! 错误处理 !!! 无法将内存添加到内存阵列
				if ( idx == 0 ) {
					if ( objSTK->OnError ) {
						objSTK->OnError(objSTK, MM_ERROR_ADDMMU);
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
		Memory Pool 256 
			内存管理器（可变成员大小的内存池，使用 MM256 加速分配和释放）
	*/

	#ifdef MMU_USE_MP256
		
		// 创建内存池
		XXAPI MP256_Object MP256_Create(int bCustom)
		{
			MP256_Object objMP = mmu_malloc(sizeof(MP256_Struct));
			if ( objMP ) {
				MP256_Init(objMP, bCustom);
			}
			return objMP;
		}
		
		// 销毁内存池
		XXAPI void MP256_Destroy(MP256_Object objMP)
		{
			if ( objMP ) {
				MP256_Unit(objMP);
				mmu_free(objMP);
			}
		}
		
		// 初始化内存池（对自维护结构体指针使用，和 MP256_Create 功能类似）
		void MP256_SetFSB(FSB256_Item* FSB, int idx, unsigned int iSizeMin, unsigned int iSizeMax, FSB256_Item* left, FSB256_Item* right)
		{
			FSB[idx].MinLength = iSizeMin;
			FSB[idx].MaxLength = iSizeMax;
			FSB[idx].LL_Idle = NULL;
			FSB[idx].LL_Full = NULL;
			FSB[idx].LL_Null = NULL;
			FSB[idx].LL_Free = NULL;
			FSB[idx].left = left;
			FSB[idx].right = right;
		}
		XXAPI void MP256_Init(MP256_Object objMP, int bCustom)
		{
			BSMM_Init(&objMP->arrMMU, sizeof(MMU256_LLNode));
			BSMM_Init(&objMP->BigMM, sizeof(MP_BigInfoLL));
			objMP->LL_BigFree = NULL;
			objMP->OnError = NULL;
			if ( bCustom == 1 ) {
				// 添加默认的区块区间 (4层树，针对小内存的方案)
				//
				// 二叉树视图 (根据建树顺序插入避免旋转产生额外开销)：
				//								○
				//								160
				//				○								○
				//				64								320
				//		○				○				○				○
				//		32				96				224				448
				//	○		○		○		○		○		○		○		○
				//	16		48		80		128		192		256		384		512
				//
				objMP->FSB_Memory = mmu_malloc(sizeof(FSB256_Item) * 15);
				MP256_SetFSB(objMP->FSB_Memory, 0,	1,		16, NULL, NULL);
				MP256_SetFSB(objMP->FSB_Memory, 1,	17,		32, &objMP->FSB_Memory[0], &objMP->FSB_Memory[2]);
				MP256_SetFSB(objMP->FSB_Memory, 2,	33,		48, NULL, NULL);
				MP256_SetFSB(objMP->FSB_Memory, 3,	49,		64, &objMP->FSB_Memory[1], &objMP->FSB_Memory[5]);
				MP256_SetFSB(objMP->FSB_Memory, 4,	65,		80, NULL, NULL);
				MP256_SetFSB(objMP->FSB_Memory, 5,	81,		96, &objMP->FSB_Memory[4], &objMP->FSB_Memory[6]);
				MP256_SetFSB(objMP->FSB_Memory, 6,	97,		128, NULL, NULL);
				MP256_SetFSB(objMP->FSB_Memory, 7,	129,	160, &objMP->FSB_Memory[3], &objMP->FSB_Memory[11]);
				MP256_SetFSB(objMP->FSB_Memory, 8,	161,	192, NULL, NULL);
				MP256_SetFSB(objMP->FSB_Memory, 9,	193,	224, &objMP->FSB_Memory[8], &objMP->FSB_Memory[10]);
				MP256_SetFSB(objMP->FSB_Memory, 10,	225,	256, NULL, NULL);
				MP256_SetFSB(objMP->FSB_Memory, 11,	257,	320, &objMP->FSB_Memory[9], &objMP->FSB_Memory[13]);
				MP256_SetFSB(objMP->FSB_Memory, 12,	321,	384, NULL, NULL);
				MP256_SetFSB(objMP->FSB_Memory, 13,	385,	448, &objMP->FSB_Memory[12], &objMP->FSB_Memory[14]);
				MP256_SetFSB(objMP->FSB_Memory, 14,	449,	512, NULL, NULL);
				objMP->FSB_RootNode = &objMP->FSB_Memory[7];
			} else if ( bCustom == 2 ) {
				// 添加默认的区块区间 (5层树，针对大内存的方案)
				//
				// 二叉树视图 (根据建树顺序插入避免旋转产生额外开销)：
				//																○
				//																640
				//								○																○
				//								160																2304
				//				○								○								○								○
				//				64								320								1280							3328
				//		○				○				○				○				○				○				○				○
				//		32				96				224				448				896				1792			2816			3840
				//	○		○		○		○		○		○		○		○		○		○		○		○		○		○		○		○
				//	16		48		80		128		192		256		384		512		768		1024	1536	2048	2560	3072	3584	4096
				//
				objMP->FSB_Memory = mmu_malloc(sizeof(FSB256_Item) * 31);
				MP256_SetFSB(objMP->FSB_Memory, 0,	1,		16, NULL, NULL);
				MP256_SetFSB(objMP->FSB_Memory, 1,	17,		32, &objMP->FSB_Memory[0], &objMP->FSB_Memory[2]);
				MP256_SetFSB(objMP->FSB_Memory, 2,	33,		48, NULL, NULL);
				MP256_SetFSB(objMP->FSB_Memory, 3,	49,		64, &objMP->FSB_Memory[1], &objMP->FSB_Memory[5]);
				MP256_SetFSB(objMP->FSB_Memory, 4,	65,		80, NULL, NULL);
				MP256_SetFSB(objMP->FSB_Memory, 5,	81,		96, &objMP->FSB_Memory[4], &objMP->FSB_Memory[6]);
				MP256_SetFSB(objMP->FSB_Memory, 6,	97,		128, NULL, NULL);
				MP256_SetFSB(objMP->FSB_Memory, 7,	129,	160, &objMP->FSB_Memory[3], &objMP->FSB_Memory[11]);
				MP256_SetFSB(objMP->FSB_Memory, 8,	161,	192, NULL, NULL);
				MP256_SetFSB(objMP->FSB_Memory, 9,	193,	224, &objMP->FSB_Memory[8], &objMP->FSB_Memory[10]);
				MP256_SetFSB(objMP->FSB_Memory, 10,	225,	256, NULL, NULL);
				MP256_SetFSB(objMP->FSB_Memory, 11,	257,	320, &objMP->FSB_Memory[9], &objMP->FSB_Memory[13]);
				MP256_SetFSB(objMP->FSB_Memory, 12,	321,	384, NULL, NULL);
				MP256_SetFSB(objMP->FSB_Memory, 13,	385,	448, &objMP->FSB_Memory[12], &objMP->FSB_Memory[14]);
				MP256_SetFSB(objMP->FSB_Memory, 14,	449,	512, NULL, NULL);
				MP256_SetFSB(objMP->FSB_Memory, 15,	513,	640, &objMP->FSB_Memory[7], &objMP->FSB_Memory[23]);
				MP256_SetFSB(objMP->FSB_Memory, 16,	641,	768, NULL, NULL);
				MP256_SetFSB(objMP->FSB_Memory, 17,	769,	896, &objMP->FSB_Memory[16], &objMP->FSB_Memory[18]);
				MP256_SetFSB(objMP->FSB_Memory, 18,	897,	1024, NULL, NULL);
				MP256_SetFSB(objMP->FSB_Memory, 19,	1025,	1280, &objMP->FSB_Memory[17], &objMP->FSB_Memory[21]);
				MP256_SetFSB(objMP->FSB_Memory, 20,	1281,	1536, NULL, NULL);
				MP256_SetFSB(objMP->FSB_Memory, 21,	1537,	1792, &objMP->FSB_Memory[20], &objMP->FSB_Memory[22]);
				MP256_SetFSB(objMP->FSB_Memory, 22,	1793,	2048, NULL, NULL);
				MP256_SetFSB(objMP->FSB_Memory, 23,	2049,	2304, &objMP->FSB_Memory[19], &objMP->FSB_Memory[27]);
				MP256_SetFSB(objMP->FSB_Memory, 24,	2305,	2560, NULL, NULL);
				MP256_SetFSB(objMP->FSB_Memory, 25,	2561,	2816, &objMP->FSB_Memory[24], &objMP->FSB_Memory[26]);
				MP256_SetFSB(objMP->FSB_Memory, 26,	2817,	3072, NULL, NULL);
				MP256_SetFSB(objMP->FSB_Memory, 27,	3073,	3328, &objMP->FSB_Memory[25], &objMP->FSB_Memory[29]);
				MP256_SetFSB(objMP->FSB_Memory, 28,	3329,	3584, NULL, NULL);
				MP256_SetFSB(objMP->FSB_Memory, 29,	3585,	3840, &objMP->FSB_Memory[28], &objMP->FSB_Memory[30]);
				MP256_SetFSB(objMP->FSB_Memory, 30,	3841,	4096, NULL, NULL);
				objMP->FSB_RootNode = &objMP->FSB_Memory[15];
			}
		}
		
		// 释放内存池（对自维护结构体指针使用，和 MP256_Destroy 功能类似）
		XXAPI void MP256_Unit(MP256_Object objMP)
		{
			// 循环释放所有 MMU
			for ( int i = 0; i < objMP->arrMMU.Count; i++ ) {
				MMU256_LLNode* pNode = BSMM_GetPtr_Inline(&objMP->arrMMU, i);
				if ( pNode->objMMU ) {
					MMU256_Destroy(pNode->objMMU);
				}
			}
			BSMM_Unit(&objMP->arrMMU);
			if ( objMP->FSB_Memory ) {
				mmu_free(objMP->FSB_Memory);
			}
			// 循环释放所有大内存块
			for ( int i = 0; i < objMP->BigMM.Count; i++ ) {
				MP_BigInfoLL* pInfo = BSMM_GetPtr_Inline(&objMP->BigMM, i);
				if ( pInfo->Ptr ) {
					mmu_free(pInfo->Ptr);
				}
			}
			BSMM_Unit(&objMP->BigMM);
			objMP->LL_BigFree = NULL;
		}
		
		// 从内存池中申请一块内存
		XXAPI void* MP256_Alloc(MP256_Object objMP, unsigned int iSize)
		{
			if ( iSize == 0 ) { return NULL; }
			// 查找符合条件的 FSB 信息
			FSB256_Item* objFSB = objMP->FSB_RootNode;
			while ( objFSB ) {
				if ( iSize < objFSB->MinLength ) {
					objFSB = objFSB->left;
				} else if ( iSize > objFSB->MaxLength ) {
					objFSB = objFSB->right;
				} else {
					break;
				}
			}
			if ( objFSB ) {
				// 选定了 FSB，根据 FSB 区块信息通过 MMU256 分配内存
				MMU256_Object objMMU = NULL;
				if ( objFSB->LL_Idle == NULL ) {
					// 如果没有空闲的内存管理单元，优先使用备用的全空单元，或创建一个新的单元
					if ( objFSB->LL_Null ) {
						// 使用备用的全空内存管理单元
						objMMU = objFSB->LL_Null->objMMU;
						objFSB->LL_Idle = objFSB->LL_Null;
						objFSB->LL_Null = NULL;
					} else if ( objFSB->LL_Free ) {
						// 创建新的内存管理单元，使用已释放的内存管理单元位置
						objMMU = MMU256_Create(objFSB->MaxLength);
						if ( (objMMU == NULL) && objMP->OnError ) {
							objMP->OnError(objMP, MM_ERROR_CREATEMMU);
							return NULL;
						}
						// 恢复Flag，写入新申请的单元
						MMU256_LLNode* pNode = objFSB->LL_Free;
						objMMU->Flag = pNode->Flag;
						pNode->objMMU = objMMU;
						// 从 LL_Free 中移除
						if ( pNode->Next ) {
							pNode->Next->Prev = NULL;
						}
						objFSB->LL_Free = pNode->Next;
						// 添加到 LL_Idle
						pNode->Prev = NULL;
						pNode->Next = NULL;
						objFSB->LL_Idle = pNode;
					} else {
						// 创建新的内存管理单元，创建失败就报错处理
						objMMU = MMU256_Create(objFSB->MaxLength);
						if ( (objMMU == NULL) && objMP->OnError ) {
							objMP->OnError(objMP, MM_ERROR_CREATEMMU);
							return NULL;
						}
						// 将创建好的内存管理单元添加到单元阵列管理器，添加失败就报错处理
						MMU256_LLNode* pNode = BSMM_Alloc(&objMP->arrMMU);
						if ( pNode ) {
							pNode->objMMU = objMMU;
							pNode->Prev = NULL;
							pNode->Next = NULL;
							pNode->Flag = MMU_FLAG_USE | ((objMP->arrMMU.Count - 1) << 8);
							objFSB->LL_Idle = pNode;
							// 标记内存管理器单元的 Flag
							objMMU->Flag = pNode->Flag;
						} else {
							MMU256_Destroy(objMMU);
							if ( objMP->OnError ) {
								objMP->OnError(objMP, MM_ERROR_ADDMMU);
								return NULL;
							}
						}
					}
				} else {
					// 有空闲的内存管理单元，优先使用空闲的
					objMMU = objFSB->LL_Idle->objMMU;
					// 如果空闲的内存管理单元即将满了，将它转移到满载单元链表
					if ( objMMU->Count >= 255 ) {
						MMU256_LLNode* pNode = objFSB->LL_Idle;
						// 从 LL_Idle 中移除
						if ( pNode->Next ) {
							pNode->Next->Prev = NULL;
						}
						objFSB->LL_Idle = pNode->Next;
						// 添加到 LL_Full
						pNode->Prev = NULL;
						pNode->Next = objFSB->LL_Full;
						if ( objFSB->LL_Full ) {
							objFSB->LL_Full->Prev = pNode;
						}
						objFSB->LL_Full = pNode;
					}
				}
				return MMU256_Alloc_Inline(objMMU);
			} else {
				// 无法选定 FSB，使用 malloc 申请内存
				MP_MemHead* pHead = mmu_malloc(sizeof(MP_MemHead) + iSize);
				if ( pHead ) {
					if ( objMP->LL_BigFree ) {
						// 优先复用已释放的 BigMM 元素
						MP_BigInfoLL* pInfo = objMP->LL_BigFree;
						objMP->LL_BigFree = pInfo->Next;
						pHead->Flag = MMU_FLAG_EXT;
						pInfo->Size = iSize;
						pInfo->Ptr = pHead;
						return &pHead[1];
					} else {
						// 没有已释放的 BigMM 元素，就申请一个新的
						MP_BigInfoLL* pInfo = BSMM_Alloc(&objMP->BigMM);
						if ( pInfo ) {
							pHead->Index = objMP->BigMM.Count;
							pHead->Flag = MMU_FLAG_EXT;
							pInfo->Size = iSize;
							pInfo->Ptr = pHead;
							return &pHead[1];
						}
					}
					mmu_free(pHead);
				}
				return NULL;
			}
		}
		
		// 将内存池申请的内存释放掉
		static inline void MP256_LLNode_ClearCheck(FSB256_Item* objFSB, MMU256_LLNode* pNode, int bLL_Full)
		{
			// 如果这个内存管理单元已经清空
			if ( pNode->objMMU->Count == 0 ) {
				if ( objFSB->LL_Null ) {
					// 有备用单元时，直接释放掉这个单元
					MMU256_Destroy(pNode->objMMU);
					pNode->objMMU = NULL;
					// 从 LL_Idle 或 LL_Full 中移除
					if ( pNode->Prev ) {
						pNode->Prev->Next = pNode->Next;
					} else {
						if ( bLL_Full ) {
							objFSB->LL_Full = pNode->Next;
						} else {
							objFSB->LL_Idle = pNode->Next;
						}
					}
					if ( pNode->Next ) {
						pNode->Next->Prev = pNode->Prev;
					}
					// 添加到 LL_Free
					pNode->Prev = NULL;
					pNode->Next = objFSB->LL_Free;
					if ( objFSB->LL_Free ) {
						objFSB->LL_Free->Prev = pNode;
					}
					objFSB->LL_Free = pNode;
				} else {
					// 没有备用单元时，让这个单元备用，避免临界状态反复申请和释放内存管理单元，造成性能损失
					// 从 LL_Idle 或 LL_Full 中移除
					if ( pNode->Prev ) {
						pNode->Prev->Next = pNode->Next;
					} else {
						if ( bLL_Full ) {
							objFSB->LL_Full = pNode->Next;
						} else {
							objFSB->LL_Idle = pNode->Next;
						}
					}
					if ( pNode->Next ) {
						pNode->Next->Prev = pNode->Prev;
					}
					// 添加到 LL_Null
					objFSB->LL_Null = pNode;
					pNode->Prev = NULL;
					pNode->Next = NULL;
				}
			}
		}
		static inline void MP256_LLNode_IdleCheck(FSB256_Item* objFSB, MMU256_LLNode* pNode)
		{
			if ( pNode->objMMU->Count < 256 ) {
				// 从 LL_Full 中移除
				if ( pNode->Prev ) {
					pNode->Prev->Next = pNode->Next;
				} else {
					objFSB->LL_Full = pNode->Next;
				}
				if ( pNode->Next ) {
					pNode->Next->Prev = pNode->Prev;
				}
				// 添加到 LL_Idle
				pNode->Prev = NULL;
				pNode->Next = objFSB->LL_Idle;
				if ( objFSB->LL_Idle ) {
					objFSB->LL_Idle->Prev = pNode;
				}
				objFSB->LL_Idle = pNode;
			}
		}
		XXAPI void MP256_Free(MP256_Object objMP, void* ptr)
		{
			MMU_ValuePtr v = ptr - sizeof(MMU_Value);
			if ( (v->ItemFlag & MMU_FLAG_MASK) == MMU_FLAG_MASK ) {
				// 大内存释放
				MP_MemHead* pHead = ptr - sizeof(MP_MemHead);
				MP_BigInfoLL* pInfo = BSMM_GetPtr_Inline(&objMP->BigMM, pHead->Index);
				mmu_free(pInfo->Ptr);
				pHead->Flag = 0;
				pInfo->Ptr = NULL;
				pInfo->Next = objMP->LL_BigFree;
				objMP->LL_BigFree = pInfo;
			} else {
				// FSB内存释放
				if ( v->ItemFlag & MMU_FLAG_USE ) {
					int iMMU = (v->ItemFlag & MMU_FLAG_MASK) >> 8;
					unsigned char idx = v->ItemFlag & 0xFF;
					// 获取对应的内存管理器单元链表结构
					MMU256_LLNode* pNode = BSMM_GetPtr_Inline(&objMP->arrMMU, iMMU);
					if ( (pNode->objMMU == NULL) && objMP->OnError ) {
						objMP->OnError(objMP, MM_ERROR_NULLMMU);
						return;
					}
					// 查找符合条件的 FSB 信息
					FSB256_Item* objFSB = objMP->FSB_RootNode;
					unsigned int iMaxSize = pNode->objMMU->ItemLength - sizeof(MMU_Value);
					while ( objFSB ) {
						if ( iMaxSize < objFSB->MinLength ) {
							objFSB = objFSB->left;
						} else if ( iMaxSize > objFSB->MaxLength ) {
							objFSB = objFSB->right;
						} else {
							break;
						}
					}
					if ( (objFSB == NULL) && objMP->OnError ) {
						objMP->OnError(objMP, MM_ERROR_FINDFSB);
						return;
					}
					// 调用对应 MMU 的释放函数
					MMU256_FreeIdx_Inline(pNode->objMMU, idx);
					v->ItemFlag = 0;
					// 如果是一个满载的内存管理器单元，将它放入空闲单元列表
					if ( pNode->objMMU->Count >= 255 ) {
						MP256_LLNode_IdleCheck(objFSB, pNode);
					}
					// 如果这个内存管理单元已经清空，将他释放或变为备用单元
					MP256_LLNode_ClearCheck(objFSB, pNode, 0);
				}
			}
		}
		
		// 进行一轮GC，将 标记 或 未标记 的内存全部回收
		void MP256_GC_RecuFSB(FSB256_Item* objFSB, int bFreeMark)
		{
			// 遍历所有 空闲的 和 满载的 内存管理单元，进行标记回收
			MMU256_LLNode* pNode = objFSB->LL_Idle;
			while ( pNode ) {
				MMU256_GC(pNode->objMMU, bFreeMark);
				pNode = pNode->Next;
			}
			pNode = objFSB->LL_Full;
			while ( pNode ) {
				MMU256_GC(pNode->objMMU, bFreeMark);
				pNode = pNode->Next;
			}
			// 再次遍历所有 空闲的 和 满载的 内存管理单元，将他们归类到正确的分组
			pNode = objFSB->LL_Idle;
			while ( pNode ) {
				MMU256_LLNode* pNext = pNode->Next;
				MP256_LLNode_ClearCheck(objFSB, pNode, 0);
				pNode = pNext;
			}
			pNode = objFSB->LL_Full;
			while ( pNode ) {
				MMU256_LLNode* pNext = pNode->Next;
				if ( pNode->objMMU->Count == 0 ) {
					MP256_LLNode_ClearCheck(objFSB, pNode, -1);
				} else {
					MP256_LLNode_IdleCheck(objFSB, pNode);
				}
				pNode = pNext;
			}
			// 递归调用左子树
			if ( objFSB->left ) {
				MP256_GC_RecuFSB(objFSB-> left, bFreeMark);
			}
			// 递归调用右子树
			if ( objFSB->right ) {
				MP256_GC_RecuFSB(objFSB-> right, bFreeMark);
			}
		}
		XXAPI void MP256_GC(MP256_Object objMP, int bFreeMark)
		{
			// 递归回收 FSB 标记的内存
			MP256_GC_RecuFSB(objMP->FSB_RootNode, bFreeMark);
			// 循环大内存列表进行回收
			if ( bFreeMark ) {
				// 被标记的内存将被回收
				for ( int i = 0; i < objMP->BigMM.Count; i++ ) {
					MP_BigInfoLL* pInfo = BSMM_GetPtr_Inline(&objMP->BigMM, i);
					MP_MemHead* pHead = pInfo->Ptr;
					if ( pHead->Flag & MMU_FLAG_USE ) {
						if ( pHead->Flag & MMU_FLAG_GC ) {
							mmu_free(pInfo->Ptr);
							pHead->Flag = 0;
							pInfo->Ptr = NULL;
							pInfo->Next = objMP->LL_BigFree;
							objMP->LL_BigFree = pInfo;
						}
					}
				}
			} else {
				// 未被标记的内存将被回收
				for ( int i = 0; i < objMP->BigMM.Count; i++ ) {
					MP_BigInfoLL* pInfo = BSMM_GetPtr_Inline(&objMP->BigMM, i);
					MP_MemHead* pHead = pInfo->Ptr;
					if ( pHead->Flag & MMU_FLAG_USE ) {
						if ( pHead->Flag & MMU_FLAG_GC ) {
							pHead->Flag &= ~MMU_FLAG_GC;
						} else {
							mmu_free(pInfo->Ptr);
							pHead->Flag = 0;
							pInfo->Ptr = NULL;
							pInfo->Next = objMP->LL_BigFree;
							objMP->LL_BigFree = pInfo;
						}
					}
				}
			}
		}
		
	#endif





	/*
		Memory Pool 64K 
			内存管理器（可变成员大小的内存池，使用 MM64K 加速分配和释放）
	*/

	#ifdef MMU_USE_MP64K
		
		// 创建内存池
		XXAPI MP64K_Object MP64K_Create(int bCustom)
		{
			MP64K_Object objMP = mmu_malloc(sizeof(MP64K_Struct));
			if ( objMP ) {
				MP64K_Init(objMP, bCustom);
			}
			return objMP;
		}
		
		// 销毁内存池
		XXAPI void MP64K_Destroy(MP64K_Object objMP)
		{
			if ( objMP ) {
				MP64K_Unit(objMP);
				mmu_free(objMP);
			}
		}
		
		// 初始化内存池（对自维护结构体指针使用，和 MP64K_Create 功能类似）
		void MP64K_SetFSB(FSB64K_Item* FSB, int idx, unsigned int iSizeMin, unsigned int iSizeMax, FSB64K_Item* left, FSB64K_Item* right)
		{
			FSB[idx].MinLength = iSizeMin;
			FSB[idx].MaxLength = iSizeMax;
			FSB[idx].LL_Idle = NULL;
			FSB[idx].LL_Full = NULL;
			FSB[idx].LL_Null = NULL;
			FSB[idx].LL_Free = NULL;
			FSB[idx].left = left;
			FSB[idx].right = right;
		}
		XXAPI void MP64K_Init(MP64K_Object objMP, int bCustom)
		{
			BSMM_Init(&objMP->arrMMU, sizeof(MMU64K_LLNode));
			BSMM_Init(&objMP->BigMM, sizeof(MP_BigInfoLL));
			objMP->LL_BigFree = NULL;
			objMP->OnError = NULL;
			if ( bCustom == 1 ) {
				// 添加默认的区块区间 (4层树，针对小内存的方案)
				//
				// 二叉树视图 (根据建树顺序插入避免旋转产生额外开销)：
				//								○
				//								160
				//				○								○
				//				64								320
				//		○				○				○				○
				//		32				96				224				448
				//	○		○		○		○		○		○		○		○
				//	16		48		80		128		192		256		384		512
				//
				objMP->FSB_Memory = mmu_malloc(sizeof(FSB64K_Item) * 15);
				MP64K_SetFSB(objMP->FSB_Memory, 0,	1,		16, NULL, NULL);
				MP64K_SetFSB(objMP->FSB_Memory, 1,	17,		32, &objMP->FSB_Memory[0], &objMP->FSB_Memory[2]);
				MP64K_SetFSB(objMP->FSB_Memory, 2,	33,		48, NULL, NULL);
				MP64K_SetFSB(objMP->FSB_Memory, 3,	49,		64, &objMP->FSB_Memory[1], &objMP->FSB_Memory[5]);
				MP64K_SetFSB(objMP->FSB_Memory, 4,	65,		80, NULL, NULL);
				MP64K_SetFSB(objMP->FSB_Memory, 5,	81,		96, &objMP->FSB_Memory[4], &objMP->FSB_Memory[6]);
				MP64K_SetFSB(objMP->FSB_Memory, 6,	97,		128, NULL, NULL);
				MP64K_SetFSB(objMP->FSB_Memory, 7,	129,	160, &objMP->FSB_Memory[3], &objMP->FSB_Memory[11]);
				MP64K_SetFSB(objMP->FSB_Memory, 8,	161,	192, NULL, NULL);
				MP64K_SetFSB(objMP->FSB_Memory, 9,	193,	224, &objMP->FSB_Memory[8], &objMP->FSB_Memory[10]);
				MP64K_SetFSB(objMP->FSB_Memory, 10,	225,	256, NULL, NULL);
				MP64K_SetFSB(objMP->FSB_Memory, 11,	257,	320, &objMP->FSB_Memory[9], &objMP->FSB_Memory[13]);
				MP64K_SetFSB(objMP->FSB_Memory, 12,	321,	384, NULL, NULL);
				MP64K_SetFSB(objMP->FSB_Memory, 13,	385,	448, &objMP->FSB_Memory[12], &objMP->FSB_Memory[14]);
				MP64K_SetFSB(objMP->FSB_Memory, 14,	449,	512, NULL, NULL);
				objMP->FSB_RootNode = &objMP->FSB_Memory[7];
			} else if ( bCustom == 2 ) {
				// 添加默认的区块区间 (5层树，针对大内存的方案)
				//
				// 二叉树视图 (根据建树顺序插入避免旋转产生额外开销)：
				//																○
				//																640
				//								○																○
				//								160																2304
				//				○								○								○								○
				//				64								320								1280							3328
				//		○				○				○				○				○				○				○				○
				//		32				96				224				448				896				1792			2816			3840
				//	○		○		○		○		○		○		○		○		○		○		○		○		○		○		○		○
				//	16		48		80		128		192		256		384		512		768		1024	1536	2048	2560	3072	3584	4096
				//
				objMP->FSB_Memory = mmu_malloc(sizeof(FSB64K_Item) * 31);
				MP64K_SetFSB(objMP->FSB_Memory, 0,	1,		16, NULL, NULL);
				MP64K_SetFSB(objMP->FSB_Memory, 1,	17,		32, &objMP->FSB_Memory[0], &objMP->FSB_Memory[2]);
				MP64K_SetFSB(objMP->FSB_Memory, 2,	33,		48, NULL, NULL);
				MP64K_SetFSB(objMP->FSB_Memory, 3,	49,		64, &objMP->FSB_Memory[1], &objMP->FSB_Memory[5]);
				MP64K_SetFSB(objMP->FSB_Memory, 4,	65,		80, NULL, NULL);
				MP64K_SetFSB(objMP->FSB_Memory, 5,	81,		96, &objMP->FSB_Memory[4], &objMP->FSB_Memory[6]);
				MP64K_SetFSB(objMP->FSB_Memory, 6,	97,		128, NULL, NULL);
				MP64K_SetFSB(objMP->FSB_Memory, 7,	129,	160, &objMP->FSB_Memory[3], &objMP->FSB_Memory[11]);
				MP64K_SetFSB(objMP->FSB_Memory, 8,	161,	192, NULL, NULL);
				MP64K_SetFSB(objMP->FSB_Memory, 9,	193,	224, &objMP->FSB_Memory[8], &objMP->FSB_Memory[10]);
				MP64K_SetFSB(objMP->FSB_Memory, 10,	225,	256, NULL, NULL);
				MP64K_SetFSB(objMP->FSB_Memory, 11,	257,	320, &objMP->FSB_Memory[9], &objMP->FSB_Memory[13]);
				MP64K_SetFSB(objMP->FSB_Memory, 12,	321,	384, NULL, NULL);
				MP64K_SetFSB(objMP->FSB_Memory, 13,	385,	448, &objMP->FSB_Memory[12], &objMP->FSB_Memory[14]);
				MP64K_SetFSB(objMP->FSB_Memory, 14,	449,	512, NULL, NULL);
				MP64K_SetFSB(objMP->FSB_Memory, 15,	513,	640, &objMP->FSB_Memory[7], &objMP->FSB_Memory[23]);
				MP64K_SetFSB(objMP->FSB_Memory, 16,	641,	768, NULL, NULL);
				MP64K_SetFSB(objMP->FSB_Memory, 17,	769,	896, &objMP->FSB_Memory[16], &objMP->FSB_Memory[18]);
				MP64K_SetFSB(objMP->FSB_Memory, 18,	897,	1024, NULL, NULL);
				MP64K_SetFSB(objMP->FSB_Memory, 19,	1025,	1280, &objMP->FSB_Memory[17], &objMP->FSB_Memory[21]);
				MP64K_SetFSB(objMP->FSB_Memory, 20,	1281,	1536, NULL, NULL);
				MP64K_SetFSB(objMP->FSB_Memory, 21,	1537,	1792, &objMP->FSB_Memory[20], &objMP->FSB_Memory[22]);
				MP64K_SetFSB(objMP->FSB_Memory, 22,	1793,	2048, NULL, NULL);
				MP64K_SetFSB(objMP->FSB_Memory, 23,	2049,	2304, &objMP->FSB_Memory[19], &objMP->FSB_Memory[27]);
				MP64K_SetFSB(objMP->FSB_Memory, 24,	2305,	2560, NULL, NULL);
				MP64K_SetFSB(objMP->FSB_Memory, 25,	2561,	2816, &objMP->FSB_Memory[24], &objMP->FSB_Memory[26]);
				MP64K_SetFSB(objMP->FSB_Memory, 26,	2817,	3072, NULL, NULL);
				MP64K_SetFSB(objMP->FSB_Memory, 27,	3073,	3328, &objMP->FSB_Memory[25], &objMP->FSB_Memory[29]);
				MP64K_SetFSB(objMP->FSB_Memory, 28,	3329,	3584, NULL, NULL);
				MP64K_SetFSB(objMP->FSB_Memory, 29,	3585,	3840, &objMP->FSB_Memory[28], &objMP->FSB_Memory[30]);
				MP64K_SetFSB(objMP->FSB_Memory, 30,	3841,	4096, NULL, NULL);
				objMP->FSB_RootNode = &objMP->FSB_Memory[15];
			}
		}
		
		// 释放内存池（对自维护结构体指针使用，和 MP64K_Destroy 功能类似）
		XXAPI void MP64K_Unit(MP64K_Object objMP)
		{
			// 循环释放所有 MMU
			for ( int i = 0; i < objMP->arrMMU.Count; i++ ) {
				MMU64K_LLNode* pNode = BSMM_GetPtr_Inline(&objMP->arrMMU, i);
				if ( pNode->objMMU ) {
					MMU64K_Destroy(pNode->objMMU);
				}
			}
			BSMM_Unit(&objMP->arrMMU);
			if ( objMP->FSB_Memory ) {
				mmu_free(objMP->FSB_Memory);
			}
			// 循环释放所有大内存块
			for ( int i = 0; i < objMP->BigMM.Count; i++ ) {
				MP_BigInfoLL* pInfo = BSMM_GetPtr_Inline(&objMP->BigMM, i);
				if ( pInfo->Ptr ) {
					mmu_free(pInfo->Ptr);
				}
			}
			BSMM_Unit(&objMP->BigMM);
			objMP->LL_BigFree = NULL;
		}
		
		// 从内存池中申请一块内存
		XXAPI void* MP64K_Alloc(MP64K_Object objMP, unsigned int iSize)
		{
			if ( iSize == 0 ) { return NULL; }
			// 查找符合条件的 FSB 信息
			FSB64K_Item* objFSB = objMP->FSB_RootNode;
			while ( objFSB ) {
				if ( iSize < objFSB->MinLength ) {
					objFSB = objFSB->left;
				} else if ( iSize > objFSB->MaxLength ) {
					objFSB = objFSB->right;
				} else {
					break;
				}
			}
			if ( objFSB ) {
				// 选定了 FSB，根据 FSB 区块信息通过 MMU64K 分配内存
				MMU64K_Object objMMU = NULL;
				if ( objFSB->LL_Idle == NULL ) {
					// 如果没有空闲的内存管理单元，优先使用备用的全空单元，或创建一个新的单元
					if ( objFSB->LL_Null ) {
						// 使用备用的全空内存管理单元
						objMMU = objFSB->LL_Null->objMMU;
						objFSB->LL_Idle = objFSB->LL_Null;
						objFSB->LL_Null = NULL;
					} else if ( objFSB->LL_Free ) {
						// 创建新的内存管理单元，使用已释放的内存管理单元位置
						objMMU = MMU64K_Create(objFSB->MaxLength);
						if ( (objMMU == NULL) && objMP->OnError ) {
							objMP->OnError(objMP, MM_ERROR_CREATEMMU);
							return NULL;
						}
						// 恢复Flag，写入新申请的单元
						MMU64K_LLNode* pNode = objFSB->LL_Free;
						objMMU->Flag = pNode->Flag;
						pNode->objMMU = objMMU;
						// 从 LL_Free 中移除
						if ( pNode->Next ) {
							pNode->Next->Prev = NULL;
						}
						objFSB->LL_Free = pNode->Next;
						// 添加到 LL_Idle
						pNode->Prev = NULL;
						pNode->Next = NULL;
						objFSB->LL_Idle = pNode;
					} else {
						// 创建新的内存管理单元，创建失败就报错处理
						objMMU = MMU64K_Create(objFSB->MaxLength);
						if ( (objMMU == NULL) && objMP->OnError ) {
							objMP->OnError(objMP, MM_ERROR_CREATEMMU);
							return NULL;
						}
						// 将创建好的内存管理单元添加到单元阵列管理器，添加失败就报错处理
						MMU64K_LLNode* pNode = BSMM_Alloc(&objMP->arrMMU);
						if ( pNode ) {
							pNode->objMMU = objMMU;
							pNode->Prev = NULL;
							pNode->Next = NULL;
							pNode->Flag = MMU_FLAG_USE | ((objMP->arrMMU.Count - 1) << 16);
							objFSB->LL_Idle = pNode;
							// 标记内存管理器单元的 Flag
							objMMU->Flag = pNode->Flag;
						} else {
							MMU64K_Destroy(objMMU);
							if ( objMP->OnError ) {
								objMP->OnError(objMP, MM_ERROR_ADDMMU);
								return NULL;
							}
						}
					}
				} else {
					// 有空闲的内存管理单元，优先使用空闲的
					objMMU = objFSB->LL_Idle->objMMU;
					// 如果空闲的内存管理单元即将满了，将它转移到满载单元链表
					if ( objMMU->Count >= 65535 ) {
						MMU64K_LLNode* pNode = objFSB->LL_Idle;
						// 从 LL_Idle 中移除
						if ( pNode->Next ) {
							pNode->Next->Prev = NULL;
						}
						objFSB->LL_Idle = pNode->Next;
						// 添加到 LL_Full
						pNode->Prev = NULL;
						pNode->Next = objFSB->LL_Full;
						if ( objFSB->LL_Full ) {
							objFSB->LL_Full->Prev = pNode;
						}
						objFSB->LL_Full = pNode;
					}
				}
				return MMU64K_Alloc_Inline(objMMU);
			} else {
				// 无法选定 FSB，使用 malloc 申请内存
				MP_MemHead* pHead = mmu_malloc(sizeof(MP_MemHead) + iSize);
				if ( pHead ) {
					if ( objMP->LL_BigFree ) {
						// 优先复用已释放的 BigMM 元素
						MP_BigInfoLL* pInfo = objMP->LL_BigFree;
						objMP->LL_BigFree = pInfo->Next;
						pHead->Flag = MMU_FLAG_EXT;
						pInfo->Size = iSize;
						pInfo->Ptr = pHead;
						return &pHead[1];
					} else {
						// 没有已释放的 BigMM 元素，就申请一个新的
						MP_BigInfoLL* pInfo = BSMM_Alloc(&objMP->BigMM);
						if ( pInfo ) {
							pHead->Index = objMP->BigMM.Count;
							pHead->Flag = MMU_FLAG_EXT;
							pInfo->Size = iSize;
							pInfo->Ptr = pHead;
							return &pHead[1];
						}
					}
					mmu_free(pHead);
				}
				return NULL;
			}
		}
		
		// 将内存池申请的内存释放掉
		static inline void MP64K_LLNode_ClearCheck(FSB64K_Item* objFSB, MMU64K_LLNode* pNode, int bLL_Full)
		{
			// 如果这个内存管理单元已经清空
			if ( pNode->objMMU->Count == 0 ) {
				if ( objFSB->LL_Null ) {
					// 有备用单元时，直接释放掉这个单元
					MMU64K_Destroy(pNode->objMMU);
					pNode->objMMU = NULL;
					// 从 LL_Idle 或 LL_Full 中移除
					if ( pNode->Prev ) {
						pNode->Prev->Next = pNode->Next;
					} else {
						if ( bLL_Full ) {
							objFSB->LL_Full = pNode->Next;
						} else {
							objFSB->LL_Idle = pNode->Next;
						}
					}
					if ( pNode->Next ) {
						pNode->Next->Prev = pNode->Prev;
					}
					// 添加到 LL_Free
					pNode->Prev = NULL;
					pNode->Next = objFSB->LL_Free;
					if ( objFSB->LL_Free ) {
						objFSB->LL_Free->Prev = pNode;
					}
					objFSB->LL_Free = pNode;
				} else {
					// 没有备用单元时，让这个单元备用，避免临界状态反复申请和释放内存管理单元，造成性能损失
					// 从 LL_Idle 或 LL_Full 中移除
					if ( pNode->Prev ) {
						pNode->Prev->Next = pNode->Next;
					} else {
						if ( bLL_Full ) {
							objFSB->LL_Full = pNode->Next;
						} else {
							objFSB->LL_Idle = pNode->Next;
						}
					}
					if ( pNode->Next ) {
						pNode->Next->Prev = pNode->Prev;
					}
					// 添加到 LL_Null
					objFSB->LL_Null = pNode;
					pNode->Prev = NULL;
					pNode->Next = NULL;
				}
			}
		}
		static inline void MP64K_LLNode_IdleCheck(FSB64K_Item* objFSB, MMU64K_LLNode* pNode)
		{
			if ( pNode->objMMU->Count < 65536 ) {
				// 从 LL_Full 中移除
				if ( pNode->Prev ) {
					pNode->Prev->Next = pNode->Next;
				} else {
					objFSB->LL_Full = pNode->Next;
				}
				if ( pNode->Next ) {
					pNode->Next->Prev = pNode->Prev;
				}
				// 添加到 LL_Idle
				pNode->Prev = NULL;
				pNode->Next = objFSB->LL_Idle;
				if ( objFSB->LL_Idle ) {
					objFSB->LL_Idle->Prev = pNode;
				}
				objFSB->LL_Idle = pNode;
			}
		}
		XXAPI void MP64K_Free(MP64K_Object objMP, void* ptr)
		{
			MMU_ValuePtr v = ptr - sizeof(MMU_Value);
			if ( (v->ItemFlag & MMU_FLAG_MASK) == MMU_FLAG_MASK ) {
				// 大内存释放
				MP_MemHead* pHead = ptr - sizeof(MP_MemHead);
				MP_BigInfoLL* pInfo = BSMM_GetPtr_Inline(&objMP->BigMM, pHead->Index);
				mmu_free(pInfo->Ptr);
				pHead->Flag = 0;
				pInfo->Ptr = NULL;
				pInfo->Next = objMP->LL_BigFree;
				objMP->LL_BigFree = pInfo;
			} else {
				// FSB内存释放
				if ( v->ItemFlag & MMU_FLAG_USE ) {
					int iMMU = (v->ItemFlag & MMU_FLAG_MASK) >> 16;
					unsigned char idx = v->ItemFlag & 0xFFFF;
					// 获取对应的内存管理器单元链表结构
					MMU64K_LLNode* pNode = BSMM_GetPtr_Inline(&objMP->arrMMU, iMMU);
					if ( (pNode->objMMU == NULL) && objMP->OnError ) {
						objMP->OnError(objMP, MM_ERROR_NULLMMU);
						return;
					}
					// 查找符合条件的 FSB 信息
					FSB64K_Item* objFSB = objMP->FSB_RootNode;
					unsigned int iMaxSize = pNode->objMMU->ItemLength - sizeof(MMU_Value);
					while ( objFSB ) {
						if ( iMaxSize < objFSB->MinLength ) {
							objFSB = objFSB->left;
						} else if ( iMaxSize > objFSB->MaxLength ) {
							objFSB = objFSB->right;
						} else {
							break;
						}
					}
					if ( (objFSB == NULL) && objMP->OnError ) {
						objMP->OnError(objMP, MM_ERROR_FINDFSB);
						return;
					}
					// 调用对应 MMU 的释放函数
					MMU64K_FreeIdx_Inline(pNode->objMMU, idx);
					v->ItemFlag = 0;
					// 如果是一个满载的内存管理器单元，将它放入空闲单元列表
					if ( pNode->objMMU->Count >= 65535 ) {
						MP64K_LLNode_IdleCheck(objFSB, pNode);
					}
					// 如果这个内存管理单元已经清空，将他释放或变为备用单元
					MP64K_LLNode_ClearCheck(objFSB, pNode, 0);
				}
			}
		}
		
		// 进行一轮GC，将 标记 或 未标记 的内存全部回收
		void MP64K_GC_RecuFSB(FSB64K_Item* objFSB, int bFreeMark)
		{
			// 遍历所有 空闲的 和 满载的 内存管理单元，进行标记回收
			MMU64K_LLNode* pNode = objFSB->LL_Idle;
			while ( pNode ) {
				MMU64K_GC(pNode->objMMU, bFreeMark);
				pNode = pNode->Next;
			}
			pNode = objFSB->LL_Full;
			while ( pNode ) {
				MMU64K_GC(pNode->objMMU, bFreeMark);
				pNode = pNode->Next;
			}
			// 再次遍历所有 空闲的 和 满载的 内存管理单元，将他们归类到正确的分组
			pNode = objFSB->LL_Idle;
			while ( pNode ) {
				MMU64K_LLNode* pNext = pNode->Next;
				MP64K_LLNode_ClearCheck(objFSB, pNode, 0);
				pNode = pNext;
			}
			pNode = objFSB->LL_Full;
			while ( pNode ) {
				MMU64K_LLNode* pNext = pNode->Next;
				if ( pNode->objMMU->Count == 0 ) {
					MP64K_LLNode_ClearCheck(objFSB, pNode, -1);
				} else {
					MP64K_LLNode_IdleCheck(objFSB, pNode);
				}
				pNode = pNext;
			}
			// 递归调用左子树
			if ( objFSB->left ) {
				MP64K_GC_RecuFSB(objFSB-> left, bFreeMark);
			}
			// 递归调用右子树
			if ( objFSB->right ) {
				MP64K_GC_RecuFSB(objFSB-> right, bFreeMark);
			}
		}
		XXAPI void MP64K_GC(MP64K_Object objMP, int bFreeMark)
		{
			// 递归回收 FSB 标记的内存
			MP64K_GC_RecuFSB(objMP->FSB_RootNode, bFreeMark);
			// 循环大内存列表进行回收
			if ( bFreeMark ) {
				// 被标记的内存将被回收
				for ( int i = 0; i < objMP->BigMM.Count; i++ ) {
					MP_BigInfoLL* pInfo = BSMM_GetPtr_Inline(&objMP->BigMM, i);
					MP_MemHead* pHead = pInfo->Ptr;
					if ( pHead->Flag & MMU_FLAG_USE ) {
						if ( pHead->Flag & MMU_FLAG_GC ) {
							mmu_free(pInfo->Ptr);
							pHead->Flag = 0;
							pInfo->Ptr = NULL;
							pInfo->Next = objMP->LL_BigFree;
							objMP->LL_BigFree = pInfo;
						}
					}
				}
			} else {
				// 未被标记的内存将被回收
				for ( int i = 0; i < objMP->BigMM.Count; i++ ) {
					MP_BigInfoLL* pInfo = BSMM_GetPtr_Inline(&objMP->BigMM, i);
					MP_MemHead* pHead = pInfo->Ptr;
					if ( pHead->Flag & MMU_FLAG_USE ) {
						if ( pHead->Flag & MMU_FLAG_GC ) {
							pHead->Flag &= ~MMU_FLAG_GC;
						} else {
							mmu_free(pInfo->Ptr);
							pHead->Flag = 0;
							pInfo->Ptr = NULL;
							pInfo->Next = objMP->LL_BigFree;
							objMP->LL_BigFree = pInfo;
						}
					}
				}
			}
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


