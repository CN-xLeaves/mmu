


// test RBTree struct
typedef struct {
	int Val;
} RBT_Test_Struct, *RBT_Test_Object;



// RBTree compear function
int rbtree_comp_proc(RBT_Test_Object pNode, intptr_t iKey)
{
	return pNode->Val - iKey;
}



// print RBTree
int iHeigthRB = 1;
void rbtree_print(RBTree_NodeBase* pRoot)
{
	for ( int i = 0; i < iHeigthRB; i++ ) {
		printf("    ");
	}
	if ( pRoot ) {
		RBT_Test_Object pData = RBTree_GetNodeData(pRoot);
		printf("Val : %d (ptr:%p) (parent:%p) [color:%s]\n", pData->Val, pRoot, rb_parent(pRoot),  rb_color(pRoot) ? "Black" : "Red");
		iHeigthRB++;
		rbtree_print(pRoot->left);
		rbtree_print(pRoot->right);
		iHeigthRB--;
	} else {
		printf("null\n");
	}
}



void test_rbtree()
{
	system("cls");
	MMU_Init();
	
	
	
	// subject 1 : create object
	printf("RBTree test subject 1 : create object\n\n");
	RBTree_Object objTree = RBTree_Create(sizeof(RBT_Test_Struct), (void*)rbtree_comp_proc);
	if ( objTree ) {
		//objTree->OnError = (void*)rbtree_error_proc;
		printf("RBTree object : %p\t\t\t\tpass! √\n", objTree);
		printf("\tRootNode : %p\t\t=> 0\n", objTree->RootNode);
		printf("\tCount : %d\t\t\t\t=> 0\n", objTree->Count);
		printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objTree->objMM.ItemLength, sizeof(RBT_Test_Struct) + sizeof(RBTree_NodeBase));
		printf("\tMM.arrMMU.ItemLength : %d\t\t=> %d\n", objTree->objMM.arrMMU.ItemLength, sizeof(MMU256_LLNode));
		printf("\tMM.arrMMU.Count : %d\t\t\t=> 0\n", objTree->objMM.arrMMU.Count);
		printf("\tMM.arrMMU.PageMMU.Count : %d\t\t=> 0\n", objTree->objMM.arrMMU.PageMMU.Count);
		printf("\tMM.arrMMU.PageMMU.AllocCount : %d\t=> 0\n", objTree->objMM.arrMMU.PageMMU.AllocCount);
		printf("\tMM.arrMMU.PageMMU.AllocStep : %d\t=> 64\n", objTree->objMM.arrMMU.PageMMU.AllocStep);
		if ( objTree->objMM.arrMMU.PageMMU.Memory ) {
			printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tfail! ×\n", objTree->objMM.arrMMU.PageMMU.Memory);
		} else {
			printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tpass! √\n", objTree->objMM.arrMMU.PageMMU.Memory);
		}
	} else {
		printf("RBTree object : %p\t\t\t\tfail! ×\n", objTree);
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 2 : insert node
	printf("RBTree test subject 2 : insert node\n\n");
	int iValArr1[] = { 27, 52, 19, 13, 2, 30, 5, 49, 48, 55 };
	for ( int i = 0; i < 10; i++ ) {
		RBT_Test_Object objNode = RBTree_Insert(objTree, (void*)(intptr_t)iValArr1[i]);
		if ( objNode ) {
			objNode->Val = iValArr1[i];
			printf("RBTree insert node , k&v = %d\t\t\t\t\tpass! √\n", iValArr1[i]);
		} else {
			printf("RBTree insert node , k&v = %d\t\t\t\t\tfail! ×\n", iValArr1[i]);
		}
	}
	
	printf("\nRBTree state : \n");
	printf("\tRootNode : %p\t\t=> not 0\n", objTree->RootNode);
	printf("\tCount : %d\t\t\t\t=> 10\n", objTree->Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objTree->objMM.ItemLength, sizeof(RBT_Test_Struct) + sizeof(RBTree_NodeBase));
	printf("\tMM.arrMMU.ItemLength : %d\t\t=> %d\n", objTree->objMM.arrMMU.ItemLength, sizeof(MMU256_LLNode));
	printf("\tMM.arrMMU.Count : %d\t\t\t=> 1\n", objTree->objMM.arrMMU.Count);
	printf("\tMM.arrMMU.PageMMU.Count : %d\t\t=> 1\n", objTree->objMM.arrMMU.PageMMU.Count);
	printf("\tMM.arrMMU.PageMMU.AllocCount : %d\t=> 64\n", objTree->objMM.arrMMU.PageMMU.AllocCount);
	printf("\tMM.arrMMU.PageMMU.AllocStep : %d\t=> 64\n", objTree->objMM.arrMMU.PageMMU.AllocStep);
	if ( objTree->objMM.arrMMU.PageMMU.Memory ) {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tpass! √\n", objTree->objMM.arrMMU.PageMMU.Memory);
	} else {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tfail! ×\n", objTree->objMM.arrMMU.PageMMU.Memory);
	}
	
	printf("\nRBTree print : \n");
	rbtree_print(objTree->RootNode);
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 3 : search node
	printf("RBTree test subject 3 : search node\n\n");
	for ( int i = 0; i < 10; i++ ) {
		RBT_Test_Object objNode = RBTree_Search(objTree, (void*)(intptr_t)iValArr1[i]);
		if ( objNode ) {
			printf("RBTree search node , k&v = %d\t\t\t\tpass! √\n", iValArr1[i]);
		} else {
			printf("RBTree search node , k&v = %d\t\t\t\tfail! ×\n", iValArr1[i]);
		}
	}
	
	printf("\nRBTree state : \n");
	printf("\tRootNode : %p\t\t=> not 0\n", objTree->RootNode);
	printf("\tCount : %d\t\t\t\t=> 10\n", objTree->Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objTree->objMM.ItemLength, sizeof(RBT_Test_Struct) + sizeof(RBTree_NodeBase));
	printf("\tMM.arrMMU.ItemLength : %d\t\t=> %d\n", objTree->objMM.arrMMU.ItemLength, sizeof(MMU256_LLNode));
	printf("\tMM.arrMMU.Count : %d\t\t\t=> 1\n", objTree->objMM.arrMMU.Count);
	printf("\tMM.arrMMU.PageMMU.Count : %d\t\t=> 1\n", objTree->objMM.arrMMU.PageMMU.Count);
	printf("\tMM.arrMMU.PageMMU.AllocCount : %d\t=> 64\n", objTree->objMM.arrMMU.PageMMU.AllocCount);
	printf("\tMM.arrMMU.PageMMU.AllocStep : %d\t=> 64\n", objTree->objMM.arrMMU.PageMMU.AllocStep);
	if ( objTree->objMM.arrMMU.PageMMU.Memory ) {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tpass! √\n", objTree->objMM.arrMMU.PageMMU.Memory);
	} else {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tfail! ×\n", objTree->objMM.arrMMU.PageMMU.Memory);
	}
	
	printf("\nRBTree print : \n");
	rbtree_print(objTree->RootNode);
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 4 : remove node
	printf("RBTree test subject 4 : remove node\n\n");
	for ( int i = 0; i < 10; i++ ) {
		if ( i & 1 ) {
			int bRet = RBTree_Remove(objTree, (void*)(intptr_t)iValArr1[i]);
			if ( bRet ) {
				printf("RBTree remove node , k&v = %d\t\t\t\t\tpass! √\n", iValArr1[i]);
			} else {
				printf("RBTree remove node , k&v = %d\t\t\t\t\tfail! ×\n", iValArr1[i]);
			}
		}
	}
	
	printf("\nRBTree state : \n");
	printf("\tRootNode : %p\t\t=> not 0\n", objTree->RootNode);
	printf("\tCount : %d\t\t\t\t=> 5\n", objTree->Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objTree->objMM.ItemLength, sizeof(RBT_Test_Struct) + sizeof(RBTree_NodeBase));
	printf("\tMM.arrMMU.ItemLength : %d\t\t=> %d\n", objTree->objMM.arrMMU.ItemLength, sizeof(MMU256_LLNode));
	printf("\tMM.arrMMU.Count : %d\t\t\t=> 1\n", objTree->objMM.arrMMU.Count);
	printf("\tMM.arrMMU.PageMMU.Count : %d\t\t=> 1\n", objTree->objMM.arrMMU.PageMMU.Count);
	printf("\tMM.arrMMU.PageMMU.AllocCount : %d\t=> 64\n", objTree->objMM.arrMMU.PageMMU.AllocCount);
	printf("\tMM.arrMMU.PageMMU.AllocStep : %d\t=> 64\n", objTree->objMM.arrMMU.PageMMU.AllocStep);
	if ( objTree->objMM.arrMMU.PageMMU.Memory ) {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tpass! √\n", objTree->objMM.arrMMU.PageMMU.Memory);
	} else {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tfail! ×\n", objTree->objMM.arrMMU.PageMMU.Memory);
	}
	
	printf("\nRBTree print : \n");
	rbtree_print(objTree->RootNode);
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 5 : insert node
	printf("RBTree test subject 5 : insert node\n\n");
	int iValArr2[] = { 199, 154, 191, 200, 115, 183, 112, 153, 126, 137 };
	for ( int i = 0; i < 10; i++ ) {
		RBT_Test_Object objNode = RBTree_Insert(objTree, (void*)(intptr_t)iValArr2[i]);
		if ( objNode ) {
			objNode->Val = iValArr2[i];
			printf("RBTree insert node , k&v = %d\t\t\t\t\tpass! √\n", iValArr2[i]);
		} else {
			printf("RBTree insert node , k&v = %d\t\t\t\t\tfail! ×\n", iValArr2[i]);
		}
	}
	
	printf("\nRBTree state : \n");
	printf("\tRootNode : %p\t\t=> not 0\n", objTree->RootNode);
	printf("\tCount : %d\t\t\t\t=> 15\n", objTree->Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objTree->objMM.ItemLength, sizeof(RBT_Test_Struct) + sizeof(RBTree_NodeBase));
	printf("\tMM.arrMMU.ItemLength : %d\t\t=> %d\n", objTree->objMM.arrMMU.ItemLength, sizeof(MMU256_LLNode));
	printf("\tMM.arrMMU.Count : %d\t\t\t=> 1\n", objTree->objMM.arrMMU.Count);
	printf("\tMM.arrMMU.PageMMU.Count : %d\t\t=> 1\n", objTree->objMM.arrMMU.PageMMU.Count);
	printf("\tMM.arrMMU.PageMMU.AllocCount : %d\t=> 64\n", objTree->objMM.arrMMU.PageMMU.AllocCount);
	printf("\tMM.arrMMU.PageMMU.AllocStep : %d\t=> 64\n", objTree->objMM.arrMMU.PageMMU.AllocStep);
	if ( objTree->objMM.arrMMU.PageMMU.Memory ) {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tpass! √\n", objTree->objMM.arrMMU.PageMMU.Memory);
	} else {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tfail! ×\n", objTree->objMM.arrMMU.PageMMU.Memory);
	}
	
	printf("\nRBTree print : \n");
	rbtree_print(objTree->RootNode);
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 6 : insert node
	printf("RBTree test subject 6 : insert node\n\n");
	int iValArr3[] = { 241, 270, 208, 293, 227, 215, 205, 262, 209, 251 };
	for ( int i = 0; i < 10; i++ ) {
		RBT_Test_Object objNode = RBTree_Insert(objTree, (void*)(intptr_t)iValArr3[i]);
		if ( objNode ) {
			objNode->Val = iValArr3[i];
			printf("RBTree insert node , k&v = %d\t\t\t\t\tpass! √\n", iValArr3[i]);
		} else {
			printf("RBTree insert node , k&v = %d\t\t\t\t\tfail! ×\n", iValArr3[i]);
		}
	}
	
	printf("\nRBTree state : \n");
	printf("\tRootNode : %p\t\t=> not 0\n", objTree->RootNode);
	printf("\tCount : %d\t\t\t\t=> 25\n", objTree->Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objTree->objMM.ItemLength, sizeof(RBT_Test_Struct) + sizeof(RBTree_NodeBase));
	printf("\tMM.arrMMU.ItemLength : %d\t\t=> %d\n", objTree->objMM.arrMMU.ItemLength, sizeof(MMU256_LLNode));
	printf("\tMM.arrMMU.Count : %d\t\t\t=> 1\n", objTree->objMM.arrMMU.Count);
	printf("\tMM.arrMMU.PageMMU.Count : %d\t\t=> 1\n", objTree->objMM.arrMMU.PageMMU.Count);
	printf("\tMM.arrMMU.PageMMU.AllocCount : %d\t=> 64\n", objTree->objMM.arrMMU.PageMMU.AllocCount);
	printf("\tMM.arrMMU.PageMMU.AllocStep : %d\t=> 64\n", objTree->objMM.arrMMU.PageMMU.AllocStep);
	if ( objTree->objMM.arrMMU.PageMMU.Memory ) {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tpass! √\n", objTree->objMM.arrMMU.PageMMU.Memory);
	} else {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tfail! ×\n", objTree->objMM.arrMMU.PageMMU.Memory);
	}
	
	printf("\nRBTree print : \n");
	rbtree_print(objTree->RootNode);
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 7 : add node
	printf("RBTree test subject 7 : add node\n\n");
	int iValArr4[] = { 380, 241, 392, 270, 391, 208, 321, 293, 325, 227 };
	for ( int i = 0; i < 10; i++ ) {
		int bNew;
		RBT_Test_Object objNode = RBTree_AddNode(objTree, (void*)(intptr_t)iValArr4[i], &bNew);
		if ( objNode ) {
			objNode->Val = iValArr4[i];
			if ( bNew ) {
				printf("RBTree add node , k&v = %d (new)\t\t\t\tpass! √\n", iValArr4[i]);
			} else {
				printf("RBTree add node , k&v = %d\t\t\t\t\tpass! √\n", iValArr4[i]);
			}
		} else {
			printf("RBTree add node , k&v = %d\t\t\t\tfail! ×\n", iValArr4[i]);
		}
	}
	
	printf("\nRBTree state : \n");
	printf("\tRootNode : %p\t\t=> not 0\n", objTree->RootNode);
	printf("\tCount : %d\t\t\t\t=> 30\n", objTree->Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objTree->objMM.ItemLength, sizeof(RBT_Test_Struct) + sizeof(RBTree_NodeBase));
	printf("\tMM.arrMMU.ItemLength : %d\t\t=> %d\n", objTree->objMM.arrMMU.ItemLength, sizeof(MMU256_LLNode));
	printf("\tMM.arrMMU.Count : %d\t\t\t=> 1\n", objTree->objMM.arrMMU.Count);
	printf("\tMM.arrMMU.PageMMU.Count : %d\t\t=> 1\n", objTree->objMM.arrMMU.PageMMU.Count);
	printf("\tMM.arrMMU.PageMMU.AllocCount : %d\t=> 64\n", objTree->objMM.arrMMU.PageMMU.AllocCount);
	printf("\tMM.arrMMU.PageMMU.AllocStep : %d\t=> 64\n", objTree->objMM.arrMMU.PageMMU.AllocStep);
	if ( objTree->objMM.arrMMU.PageMMU.Memory ) {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tpass! √\n", objTree->objMM.arrMMU.PageMMU.Memory);
	} else {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tfail! ×\n", objTree->objMM.arrMMU.PageMMU.Memory);
	}
	
	printf("\nRBTree print : \n");
	rbtree_print(objTree->RootNode);
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 8 : set node
	printf("RBTree test subject 8 : set node\n\n");
	int iValArr5[] = { 380, 444, 453, 270, 391, 442, 486, 293, 497, 227, 552, 504, 516, 583, 565, 569, 597, 554, 543, 529, 662, 607, 642, 623, 689, 604, 666, 639, 677, 679 };
	for ( int i = 0; i < 30; i++ ) {
		int bNew;
		RBT_Test_Object objNode = RBTree_AddNode(objTree, (void*)(intptr_t)iValArr5[i], &bNew);
		if ( objNode ) {
			objNode->Val = iValArr5[i];
			if ( bNew ) {
				printf("RBTree add node , k&v = %d (new)\t\t\t\tpass! √\n", iValArr5[i]);
			} else {
				printf("RBTree add node , k&v = %d\t\t\t\t\tpass! √\n", iValArr5[i]);
			}
		} else {
			printf("RBTree add node , k&v = %d\t\t\t\tfail! ×\n", iValArr5[i]);
		}
	}
	
	printf("\nRBTree state : \n");
	printf("\tRootNode : %p\t\t=> not 0\n", objTree->RootNode);
	printf("\tCount : %d\t\t\t\t=> 55\n", objTree->Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objTree->objMM.ItemLength, sizeof(RBT_Test_Struct) + sizeof(RBTree_NodeBase));
	printf("\tMM.arrMMU.ItemLength : %d\t\t=> %d\n", objTree->objMM.arrMMU.ItemLength, sizeof(MMU256_LLNode));
	printf("\tMM.arrMMU.Count : %d\t\t\t=> 1\n", objTree->objMM.arrMMU.Count);
	printf("\tMM.arrMMU.PageMMU.Count : %d\t\t=> 1\n", objTree->objMM.arrMMU.PageMMU.Count);
	printf("\tMM.arrMMU.PageMMU.AllocCount : %d\t=> 64\n", objTree->objMM.arrMMU.PageMMU.AllocCount);
	printf("\tMM.arrMMU.PageMMU.AllocStep : %d\t=> 64\n", objTree->objMM.arrMMU.PageMMU.AllocStep);
	if ( objTree->objMM.arrMMU.PageMMU.Memory ) {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tpass! √\n", objTree->objMM.arrMMU.PageMMU.Memory);
	} else {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tfail! ×\n", objTree->objMM.arrMMU.PageMMU.Memory);
	}
	
	printf("\nRBTree print : \n");
	rbtree_print(objTree->RootNode);
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject X : struct unit & destroy
	printf("RBTree test subject X : struct unit & destroy\n\n");
	RBTree_Unit(objTree);
	printf("RBTree object (%p) already unit!\n\n", objTree);
	
	printf("\nRBTree state : \n");
	printf("\tRootNode : %p\t\t=> 0\n", objTree->RootNode);
	printf("\tCount : %d\t\t\t\t=> 0\n", objTree->Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objTree->objMM.ItemLength, sizeof(RBT_Test_Struct) + sizeof(RBTree_NodeBase));
		printf("\tMM.arrMMU.ItemLength : %d\t\t=> %d\n", objTree->objMM.arrMMU.ItemLength, sizeof(MMU256_LLNode));
		printf("\tMM.arrMMU.Count : %d\t\t\t=> 0\n", objTree->objMM.arrMMU.Count);
		printf("\tMM.arrMMU.PageMMU.Count : %d\t\t=> 0\n", objTree->objMM.arrMMU.PageMMU.Count);
		printf("\tMM.arrMMU.PageMMU.AllocCount : %d\t=> 0\n", objTree->objMM.arrMMU.PageMMU.AllocCount);
		printf("\tMM.arrMMU.PageMMU.AllocStep : %d\t=> 64\n", objTree->objMM.arrMMU.PageMMU.AllocStep);
		if ( objTree->objMM.arrMMU.PageMMU.Memory ) {
			printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tfail! ×\n", objTree->objMM.arrMMU.PageMMU.Memory);
		} else {
			printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tpass! √\n", objTree->objMM.arrMMU.PageMMU.Memory);
		}
	
	RBTree_Destroy(objTree);
	printf("\nRBTree object (%p) already destroyed!\n", objTree);
	
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	MMU_Unit();
}


