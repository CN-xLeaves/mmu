## 1. Project Introduction

#### 1.1 Introduction

MMU is the abbreviation of Memory Management Unit (and CPU memory management unit is not the same purpose), initially used in my personal project, convenient, efficient and easy to use management of all kinds of structural memory, I personally prefer to use pure C development, each project organizes a variety of data, we need all kinds of memory managers and data structures, to help me solve the problem of data management, over time, I have accumulated some basic code. I personally prefer to develop in pure C. When organizing various data in each project, I need all kinds of memory managers and data structures to help me solve the data management problems.

Later, I had to develop my own programming language, xLang, and the need for data management became even more urgent. After referring to the memory management part of the code of several scripting languages, I never felt satisfied enough, so I decided to integrate several modules developed in the past, together with the possible new needs in the future, into a fully functional memory manager, because at the beginning, this library had only a few functions, and therefore it was called Memory Management Unit. The library was called Memory Management Unit because it had very little functionality at the beginning, but the name was never changed as the functionality was slowly integrated into the library.

#### 1.2 Scope of Functionality

Currently, the MMU library provides 21 functions for managing various forms of data, mainly fixed-length structures, data pointers, and so on.

I think the basis for organizing a large amount of data lies in the management of various forms and tree structures. Forms are embodied in the C language, i.e., structured arrays, but C language support for arrays is very basic, and operations such as adding, deleting, changing, and checking need to be implemented on their own, which is more troublesome and does not have a high performance. MMU was created to assist in the management of various structured data and data pointers (structured structures, data managers refer to each other and need to use the pointer manager). (structure, data manager references each other, need to use the pointer manager).

In the future, we will continue to provide several or even dozens of data structure managers to meet the challenges of various projects.

Some of the functions are overlapping, such as hash tables provide 32-bit, 64-bit and AVLTree and RBTree based on a combination of implementation, a total of four, so as to facilitate end-users according to their own business needs trade-offs, choose the existing modules directly.

#### 1.3 Design Principles

MMU library is a performance-first, within my ability to optimize the speed of all types of data structures as much as possible, I always believe that the base library to leave the maximum performance redundancy, high-rise buildings can be built solidly, not because of some bottlenecks someday and collapsed, the third-party functionality used in the code (rpmalloc, nmhash32x, rapidhash is also after a full benchmarking, the best choice). (rpmalloc, nmhash32x, rapidhash are also fully benchmarked and selected to ensure strong enough performance).

MMU is designed with a highly efficient memory pool, which can be used to request and release memory within the structure very quickly. Almost all managers come with a buffering mechanism, which avoids the performance loss caused by repeated memory requests, and improves the operation efficiency by sacrificing a certain amount of memory space (however, MMU is still quite memory-efficient).

MMU integrates rpmalloc to manage dynamically sized memory, which can greatly improve the efficiency of malloc operation.

MMU library is very easy to integrate, influenced by Sean Barrett, I also used to use the header file based on the way to share code modules for development, I personally believe that the library used for distribution should be clearly organized, integration only need a header file, at most need a .c code file and a header file can be completed, easy to compile without making tools, do not need to consider the compiler environment, do not need to solve the problem during the compile process, and do not need to make the compiler environment. It is a better open source standard if you don't need to solve all kinds of strange errors during the compilation process, which brings too much burden to the users.

MMU library is tailorable , according to their own needs , through the macro definition to open the corresponding function , rather than because of the use of a basic function , forced to compile a large pile of garbage code into their own programs , so that the compiled program somehow the volume of the expansion .

#### 1.4 Project Integration

There are two ways to integrate mmu modules in your project:

##### Integration Program 1:

Add mmu.c to the compilation list.
Put mmu_config.h and mmu.h in the same directory as mmu.c.
Modify the mmu_config.h file and cut your own reserved functions according to your needs.
Reference mmu.h in your own code.
Use the mmu library and compile your program to check if it works.

##### Integration 2:

Add mmu_single.h to your own code directory.
Reference mmu_single.h in your own code.
Before referencing mmu_single.h, define the cut switches for the various functions according to the macro definitions in mmu_config.h.
Use the mmu library and compile your own program to check if it works.

#### 1.5 Known Bugs

Currently MMU is not thread-safe, you need to lock yourself to use various data managers in a multi-threaded environment, you may consider adding this part of the function in the future.

## 2. Feature Tailoring Customization

	MMU_USE_RPMALLOC
		Once defined, calls to mmu_realloc, mmu_malloc, mmu_calloc, and mmu_free will use the rpmalloc library function instead.

	MMU_USE_SMMU
		Defined to use the SMMU module - Struct Memory Management Unit

	MMU_USE_PMMU
		PMMU Module Usable by Definition - Point Memory Management Unit

	MMU_USE_MBMU
		MBMU Module Usable by Definition - Memory Buffer Management Unit

	MMU_USE_MMU256
		MMU256 Module Usable by Definition - Memory Management Unit 256 [Fixed number of 256 memory management units]

	MMU_USE_MMU64K
		MMU64K Module Usable by Definition - Memory Management Unit 64K [Fixed number of 65536 memory management units].

	MMU_USE_MM256
		MM256 Module Available After Definition - Memory Management 256 [Memory Manager (fixed size memory pool, accelerated allocation and release using MMU256)]]

	MMU_USE_MM64K
		MM64K Module Available After Definition - Memory Management 64K [Memory Manager (fixed size memory pool, uses MMU64K to speed up allocation and release)]]

	MMU_USE_SSSTK
		SSSTK Module Available After Definition - Struct Static Stack [Struct Static Stack, memory requested at initialization, maximum stack depth fixed]

	MMU_USE_PSSTK
		PSSTK Module Available After Definition - Point Static Stack [Pointer Static Stack, memory is requested at initialization, maximum stack depth is fixed].

	MMU_USE_SDSTK
		SDSTK Module Usable After Definition - Struct Dynamic Stack [Struct Dynamic Stack, 256 increments inside structure, max stack depth is not fixed].

	MMU_USE_PDSTK
		PDSTK Module Available after Definition - Point Dynamic Stack [Pointer Dynamic Stack, 256 increments inside structure, variable maximum stack depth].

	MMU_USE_LLIST
		MMU_USE_LLIST Module - Linked List Bidirectional Chained List [uses MM256 to manage memory].

	MMU_USE_AVLTREE
		AVLTREE Module Available after Definition - AVLTree AVL Tree [uses MM256 to manage memory].

	MMU_USE_RBTREE
		RBTREE Module Available by Definition - Red Black Tree [Manages memory using MM256].

	MMU_USE_HASH32
		HASH32 Module Available by Definition - nmhash32x Ver 2.0 Hash Algorithm

	MMU_USE_HASH64
		HASH64 module available after definition - rapidhash Ver 1.0 hash algorithm

	MMU_USE_AVLHT32
		AVLHT32 Module Available by Definition - Hash table implementation based on AVLTree and 32-bit length hashes.

	MMU_USE_AVLHT64
		AVLHT64 Module Available by Definition - Hash table based on AVLTree and 64-bit length hash implementation

	MMU_USE_RBHT32
		Defined to use the RBHT32 module - a hash table based on RBTree and 32-bit length hash implementations.

	MMU_USE_RBHT64
		RBHT64 Module Available by Definition - Hash table based on RBTree and 64-bit length hash implementation

	MMU_USE_CSQUE (not implemented)
		CSQUE module available by definition - Circular Sequence Queue

	MMU_USE_EEQUE (not implemented)
		EEQUE Module Available After Definition - Elastic Expansion Queue Elastic Expansion Queue [uses MM256 to manage memory]

	MMU_USE_PRQUE (not implemented)
		PRQUE Module Available After Definition - Priority Queue Priority Queue [implements using red-black tree]

	MMU_USE_DGRAPH (not implemented)
		DGRAPH module can be used after definition - Directed Graph Unidirectional Graph Structure

	MMU_USE_UGRAPH (not implemented)
		UGRAPH Module - Undirected Graph Bidirectional Graph Structure Available After Definition

	MMU_USE_DOM (not implemented)
		Available when defined DOM Module - Document Object Model Document Object Model

## 3. rpmalloc

rpmalloc is a general-purpose memory management module, when used alone, it can be interpreted as a variable-length memory pool. rpmalloc can also directly replace the malloc, calloc, realloc and free functions in the program, so as to solve the problems of program memory fragmentation and low performance of memory request and release.

The rpmalloc function can be enabled by defining MMU_USE_RPMALLOC, the memory request and release of MMU module will be done by rpmalloc.

The rpmalloc module, when enabled, adds about 50KB of file size to the compilation (mingw gcc 14.2), which can increase the performance of memory request and release by a few times or more, but in some cases it may cause performance regression, so please test it out and decide whether to use it.

rpmalloc is written by Mattias Jansson and released under a public domain license.

rpmalloc github address: https://github.com/mjansson/rpmalloc

## 4. Struct Memory Management Unit

SMMU is a struct memory manager, which is equivalent to struct arrays, but provides functions such as insertion, swapping, deletion, sorting, etc. It is based on self-growing buffers, which improves the operation efficiency and eliminates the need to repeatedly request memory for operations.

SMMU can be a good alternative to structured arrays, but the inherent defects of arrays can not be avoided, such as insertion and deletion efficiency is very low, and the query performance is insufficient.

SMMUs can be used to manage content that needs to be traversed efficiently, such as render lists in games, UI render lists, etc. The access to an array element is only equivalent to a single pointer offset, which is very high performance.

## 5. Point Memory Management Unit

PMMU module is similar to SMMU module in that it replaces the function of arrays, but PMMU is mainly used to manage pointers, while SMMU manages structures. SMMU can replace the function of PMMU, but in the case of only needing to manage the array of pointers, you need to define a structure first, and you have to get the pointer of the structure for each operation, and then get the members of pointers inside the structure, which is a very troublesome process. This process is very troublesome.

On the other hand, using PMMU is very convenient, you can directly read or write the pointer value at the specified location without passing through a layer of structure pointer, so PMMU is very convenient in the case of managing an array of pointer references.

For example, if you create a collection of structure references, the structure data pool can be managed using MM256 or SMMU, while the pointer references can be managed using PMMU. In fact, many of the MMU library submodules, including MM256, Dynamic Stacks (SDSTK, PDSTK), Chained Lists, and many other structured paging data, are managed using the PMMU module.

## 6. Memory Buffer Management Unit

MBMU Memory Buffer Management Unit is equivalent to StringBuffer or StringBuilder, which is designed for efficient string splicing. Based on the self-growing buffer method, MBMU can avoid the performance degradation of string splicing caused by repeated memory requests and releases.

MBMU can be used for string processing in various encodings (ANSI, GBK, UTF-8, UTF-16, UTF-32), and in string mode, it will automatically add \0 (ANSI, GBK, UTF-8) or \0\0 (UTF-16) or \0\0\0 (UTF-32) at the end, or process binary data directly (internally, it is based on length management). data is managed internally based on length, not based on \0).

## 7. Memory Management Unit 256

MMU256 is a base module that only provides the ability to manage up to 256 structures (can request or free memory for 256 structure units).

In general, you should not consider using it in your program. It exists primarily as an array unit, and in the MM256 module, you can create very large numbers of MMU256s to implement a structured memory pool that supports dynamic growth and shrinkage.

The MMU256 allows the memory to be requested and released in a fixed instruction cycle, which greatly improves the speed of memory request and release.

## 8. Memory Management Unit 64K

MMU64K is similar to MMU256, the only difference is that one MMU64K unit can manage up to 65536 structures.

Assuming that the size of a structure is 100 bytes, the memory required by MMU256 to manage this structure is 256 * 100 + about 280 bytes; while the memory required by MMU64K to manage this structure is 65536 * 100 + about 65560 bytes.

When there are a large number of structures to be buffered and managed, MMU64K performs better but takes more memory.

So you can choose a different manager type according to your needs (which is more important, speed or memory footprint) (many MMU modules choose MM256 for memory management to minimize memory waste).

## 9. Memory Management 256

MM256 is a memory pool manager based on MMU256 arrays. The principle of MM256 is to manage many MMU256s through PMMUs, when requesting memory in a structure, it will traverse and select a free MMU256 unit to apply for a block of memory, and when releasing memory in a structure, it will fetch the corresponding MMU256 unit through the Memory Prefix header, and notify it to reclaim the memory.

When the applied MMU256 management unit is full, it will automatically apply for a new MMU256 to expand its capacity. Theoretically, MM256 can manage more than 1 billion structural memory.

MM256 can greatly improve the efficiency of applying and releasing structured memory, and is suitable for the scenarios that need to manage a large amount of structured data, for example, the hash table, chain table, binary tree and other modules of MMU are based on MM256 to manage structured memory.

MM256 relies on MMU256 arrays to manage memory request and release, so when the number of MMU256 arrays in the manager is too large, the efficiency will be reduced (caused by cyclically searching for free MMU256 units), MM64K can be used to improve the situation, but most of the time it is not needed, unless the amount of data is counted by 100 thousand.

## 10. Memory Management 64K

MM64K is similar to MM256, except that it uses an array of MMU64Ks as the memory management unit, which can be used to improve the speed of operations with a very large number of elements at the cost of more memory waste.

Both MM64K and MM256 can manage up to 1 billion struct memory, and there is no difference between them in this respect.

## 11. Struct Static Stack

SSSTK is a struct static stack. The maximum size of the stack is specified during initialization, and enough memory to hold all the members is requested at once.

Static stacks are very efficient (equivalent to arrays), but they cannot be expanded at any time as needed.

Please note that the stack here is built on the memory heap, the performance is not comparable to the call stack, mainly used for the stack as an abstract data structure to use, SSSTK can manage fixed-length structures, so it is suitable for the need to hierarchical processing of data occasions, such as compiler statements to manage the depth of the structure.

## 12. Point Static Stack

PSSTK is a pointer static stack, similar to SSSTK, the difference is that it manages pointers instead of structures, see the difference between SMMU and PMMU.

## 13. Struct Dynamic Stack

SDSTK is a struct dynamic stack, it uses MM256 to manage the memory, it doesn't need to specify the stack capacity at initialization, but expand and reduce the capacity at any time according to the need, so we don't need to worry about the stack memory management, and we don't need to worry about bursting the stack (of course, the consequence of infinite recursion may be the bursting of the memory too).

Like SSSTK, this is an abstract data structure built on heap memory to manage the use of fixed-length structures.

## 14. Point Dynamic Stack

PDSTK is a pointer dynamic stack, similar to SDSTK, except that it manages pointers instead of structures.

## 15. Linked List

LLIST is a bi-directional linked list. Bi-directional linked lists have excellent insertion and deletion performance, but traversal and querying performance is very low. Like other modules, the LLIST module of MMU, in addition to being used as a linked list, plays a more critical role in managing the memory of the data structures within the linked list, which uses MM256 to manage the memory.

## 16. AVLTree

AVLTree is a balanced binary tree, binary tree in the insertion, deletion, query, traversal and other performance has achieved a very good balance, and the balanced binary tree is the most good at the data query, the theory is that it can be within 10 cycles, from the 4 billion data to find the required data.

The main difference between Balanced Binary Tree and Red-Black Tree is that Red-Black Tree emphasizes more on insertion and deletion performance, while Balanced Binary Tree emphasizes more on lookup performance.

AVLTree uses MM256 to manage memory.

## 17. Red Black Tree

RBTree is a Red-Black Tree, similar to AVLTree, Red-Black Tree has better data insertion and deletion performance and worse data querying performance, the key to choosing the trade-off lies in whether the data is read more (with AVLTree) or written more (with RBTree).

RBTree uses MM256 to manage memory.

## 18. nmhash32x

nmhash32x is a 32-bit hash algorithm with excellent performance, both in terms of hash speed and collision characteristics. MMU chooses to use this algorithm as the implementation of the Hash Table algorithm, and you can also use this algorithm alone for hash value calculation.

nmhash32x is open source based on the BSD2 protocol, and the source code comes with an open source license.

## 19. rapidhash

rapidhash is a 64-bit hash algorithm with excellent performance, both in terms of hash speed and collision characteristics are very good, MMU chose to use this algorithm as the implementation of the Hash Table algorithm, you can also use this algorithm alone for the calculation of hash values. rapidhash is based on the BSD2 protocol and is available as an open source license in the source code.

You can also use this algorithm alone for hash calculation. rapidhash is open source based on the BSD2 protocol, and the source code comes with an open source license.

## 20. AVLTree Hash Table 32

AVLHT32 based on 32-bit hash algorithm + AVLTree hash table implementation , binary tree with very strong query performance , excellent insertion, deletion performance , is the ultimate choice for making hash tables , with 32-bit hash algorithm , saving unit data memory.

Usually four hash tables according to the use of the need to choose one can be.

## 21. AVLTree Hash Table 64

AVLHT64 is based on 64-bit hash algorithm AVLTree hash table , binary tree has a very strong query performance , excellent insertion and deletion performance , is the ultimate choice for the production of hash tables , with 64-bit hash algorithm , effectively reducing the possibility of data collision .

Usually four hash tables according to the use of the need to choose one can be.

## 22. RBTree Hash Table 32

RBHT32 is based on 32-bit hash algorithm + RBTree hash table, binary tree has a very strong query performance, excellent insertion, deletion performance, is the ultimate choice for making hash tables, with 32-bit hash algorithm, saving unit data memory.

Usually four hash tables according to the use of the need to choose one can be.

## 23. RBTree Hash Table 64

RBHT64 is based on 64-bit hash algorithm + RBTree hash table, binary tree with very strong query performance, excellent insertion, deletion performance, is the ultimate choice for the production of hash tables, with 64-bit hash algorithm, effectively reducing the possibility of data collision.

Usually four hash tables according to the use of the need to choose one can be.

## 24. to be realized features (follow-up plan)

#### Improve the documentation

Although the MMU module is not complicated, and the functions are clearly defined in the header file, and the complete test code is provided, but due to the lack of documentation, it still needs a lot of time to read the code to learn how to use it.

#### Circular Sequence Queue

#### Elastic Expansion Queue Elastic Expansion Queue [uses MM256 to manage memory].

#### Priority Queue Priority Queue [implemented using red-black trees].

#### Directed Graph Unidirected Graph Structure

#### Undirected Graph Bidirectional Graph Structure

#### Document Object Model

......

For more features, please contact the author for development or integration, and feel free to submit a merge request!

Author's email: xywhsoft@qq.com