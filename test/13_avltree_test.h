


// test AVLTree struct
typedef struct {
	int Val;
} AVLT_Test_Struct, *AVLT_Test_Object;



void avltree_error_proc(AVLTree_Object objTree, int ErrorID)
{
	printf("!!! Error !!! error code : %d\n", ErrorID);
}



// AVLTree compear function
int avltree_comp_proc(AVLT_Test_Object pNode, intptr_t iKey)
{
	return pNode->Val - iKey;
}



// print AVLTree
int iHeigth = 1;
void avltree_print(AVLTree_NodeBase* pRoot)
{
	for ( int i = 0; i < iHeigth; i++ ) {
		printf("    ");
	}
	if ( pRoot ) {
		AVLT_Test_Object pData = AVLTree_GetNodeData(pRoot);
		printf("Val : %d [Height:%d]\n", pData->Val, pRoot->height);
		iHeigth++;
		avltree_print(pRoot->left);
		avltree_print(pRoot->right);
		iHeigth--;
	} else {
		printf("null\n");
	}
}



void test_avltree()
{
	system("cls");
	MMU_Init();
	
	
	
	// subject 1 : create object
	printf("AVLTree test subject 1 : create object\n\n");
	AVLTree_Object objTree = AVLTree_Create(sizeof(AVLT_Test_Struct), (void*)avltree_comp_proc);
	if ( objTree ) {
		//objTree->OnError = (void*)avltree_error_proc;
		printf("AVLTree object : %p\t\t\t\tpass! √\n", objTree);
		printf("\tRootNode : %p\t\t=> 0\n", objTree->RootNode);
		printf("\tCount : %d\t\t\t\t=> 0\n", objTree->Count);
		printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objTree->objMM.ItemLength, sizeof(AVLT_Test_Struct) + sizeof(AVLTree_NodeBase));
		printf("\tMM.MMU.Count : %d\t\t\t=> 0\n", objTree->objMM.MMU.Count);
		printf("\tMM.MMU.AllocCount : %d\t\t\t=> 0\n", objTree->objMM.MMU.AllocCount);
		if ( objTree->objMM.MMU.Memory ) {
			printf("\tMM.MMU.Memory : %p\t\t\tfail! ×\n", objTree->objMM.MMU.Memory);
		} else {
			printf("\tMM.MMU.Memory : %p\t\t\tpass! √\n", objTree->objMM.MMU.Memory);
		}
	} else {
		printf("AVLTree object : %p\t\t\t\tfail! ×\n", objTree);
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 2 : insert node
	printf("AVLTree test subject 2 : insert node\n\n");
	int iValArr1[] = { 27, 52, 19, 13, 2, 30, 5, 49, 48, 55 };
	for ( int i = 0; i < 10; i++ ) {
		AVLT_Test_Object objNode = AVLTree_Insert(objTree, (void*)(intptr_t)iValArr1[i]);
		if ( objNode ) {
			objNode->Val = iValArr1[i];
			printf("AVLTree insert node , k&v = %d\t\t\t\t\tpass! √\n", iValArr1[i]);
		} else {
			printf("AVLTree insert node , k&v = %d\t\t\t\t\tfail! ×\n", iValArr1[i]);
		}
	}
	
	printf("\nAVLTree state : \n");
	printf("\tRootNode : %p\t\t=> not 0\n", objTree->RootNode);
	printf("\tCount : %d\t\t\t\t=> 10\n", objTree->Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objTree->objMM.ItemLength, sizeof(AVLT_Test_Struct) + sizeof(AVLTree_NodeBase));
	printf("\tMM.MMU.Count : %d\t\t\t=> 1\n", objTree->objMM.MMU.Count);
	printf("\tMM.MMU.AllocCount : %d\t\t\t=> 64\n", objTree->objMM.MMU.AllocCount);
	if ( objTree->objMM.MMU.Memory ) {
		printf("\tMM.MMU.Memory : %p\t\t\tpass! √\n", objTree->objMM.MMU.Memory);
	} else {
		printf("\tMM.MMU.Memory : %p\t\t\tfail! ×\n", objTree->objMM.MMU.Memory);
	}
	
	printf("\nAVLTree print : \n");
	avltree_print(objTree->RootNode);
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 3 : search node
	printf("AVLTree test subject 3 : search node\n\n");
	for ( int i = 0; i < 10; i++ ) {
		AVLT_Test_Object objNode = AVLTree_Search(objTree, (void*)(intptr_t)iValArr1[i]);
		if ( objNode ) {
			printf("AVLTree search node , k&v = %d (%d)\t\t\t\tpass! √\n", iValArr1[i], AVLTree_GetNodeBase(objNode)->height);
		} else {
			printf("AVLTree search node , k&v = %d\t\t\t\tfail! ×\n", iValArr1[i]);
		}
	}
	
	printf("\nAVLTree state : \n");
	printf("\tRootNode : %p\t\t=> not 0\n", objTree->RootNode);
	printf("\tCount : %d\t\t\t\t=> 10\n", objTree->Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objTree->objMM.ItemLength, sizeof(AVLT_Test_Struct) + sizeof(AVLTree_NodeBase));
	printf("\tMM.MMU.Count : %d\t\t\t=> 1\n", objTree->objMM.MMU.Count);
	printf("\tMM.MMU.AllocCount : %d\t\t\t=> 64\n", objTree->objMM.MMU.AllocCount);
	if ( objTree->objMM.MMU.Memory ) {
		printf("\tMM.MMU.Memory : %p\t\t\tpass! √\n", objTree->objMM.MMU.Memory);
	} else {
		printf("\tMM.MMU.Memory : %p\t\t\tfail! ×\n", objTree->objMM.MMU.Memory);
	}
	
	printf("\nAVLTree print : \n");
	avltree_print(objTree->RootNode);
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 4 : remove node
	printf("AVLTree test subject 4 : remove node\n\n");
	for ( int i = 0; i < 10; i++ ) {
		if ( i & 1 ) {
			int bRet = AVLTree_Remove(objTree, (void*)(intptr_t)iValArr1[i]);
			if ( bRet ) {
				printf("AVLTree remove node , k&v = %d\t\t\t\t\tpass! √\n", iValArr1[i]);
			} else {
				printf("AVLTree remove node , k&v = %d\t\t\t\t\tfail! ×\n", iValArr1[i]);
			}
		}
	}
	
	printf("\nAVLTree state : \n");
	printf("\tRootNode : %p\t\t=> not 0\n", objTree->RootNode);
	printf("\tCount : %d\t\t\t\t=> 5\n", objTree->Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objTree->objMM.ItemLength, sizeof(AVLT_Test_Struct) + sizeof(AVLTree_NodeBase));
	printf("\tMM.MMU.Count : %d\t\t\t=> 1\n", objTree->objMM.MMU.Count);
	printf("\tMM.MMU.AllocCount : %d\t\t\t=> 64\n", objTree->objMM.MMU.AllocCount);
	if ( objTree->objMM.MMU.Memory ) {
		printf("\tMM.MMU.Memory : %p\t\t\tpass! √\n", objTree->objMM.MMU.Memory);
	} else {
		printf("\tMM.MMU.Memory : %p\t\t\tfail! ×\n", objTree->objMM.MMU.Memory);
	}
	
	printf("\nAVLTree print : \n");
	avltree_print(objTree->RootNode);
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 5 : insert node
	printf("AVLTree test subject 5 : insert node\n\n");
	int iValArr2[] = { 199, 154, 191, 200, 115, 183, 112, 153, 126, 137 };
	for ( int i = 0; i < 10; i++ ) {
		AVLT_Test_Object objNode = AVLTree_Insert(objTree, (void*)(intptr_t)iValArr2[i]);
		if ( objNode ) {
			objNode->Val = iValArr2[i];
			printf("AVLTree insert node , k&v = %d\t\t\t\t\tpass! √\n", iValArr2[i]);
		} else {
			printf("AVLTree insert node , k&v = %d\t\t\t\t\tfail! ×\n", iValArr2[i]);
		}
	}
	
	printf("\nAVLTree state : \n");
	printf("\tRootNode : %p\t\t=> not 0\n", objTree->RootNode);
	printf("\tCount : %d\t\t\t\t=> 15\n", objTree->Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objTree->objMM.ItemLength, sizeof(AVLT_Test_Struct) + sizeof(AVLTree_NodeBase));
	printf("\tMM.MMU.Count : %d\t\t\t=> 1\n", objTree->objMM.MMU.Count);
	printf("\tMM.MMU.AllocCount : %d\t\t\t=> 64\n", objTree->objMM.MMU.AllocCount);
	if ( objTree->objMM.MMU.Memory ) {
		printf("\tMM.MMU.Memory : %p\t\t\tpass! √\n", objTree->objMM.MMU.Memory);
	} else {
		printf("\tMM.MMU.Memory : %p\t\t\tfail! ×\n", objTree->objMM.MMU.Memory);
	}
	
	printf("\nAVLTree print : \n");
	avltree_print(objTree->RootNode);
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 6 : insert node
	printf("AVLTree test subject 6 : insert node\n\n");
	int iValArr3[] = { 241, 270, 208, 293, 227, 215, 205, 262, 209, 251 };
	for ( int i = 0; i < 10; i++ ) {
		AVLT_Test_Object objNode = AVLTree_Insert(objTree, (void*)(intptr_t)iValArr3[i]);
		if ( objNode ) {
			objNode->Val = iValArr3[i];
			printf("AVLTree insert node , k&v = %d\t\t\t\t\tpass! √\n", iValArr3[i]);
		} else {
			printf("AVLTree insert node , k&v = %d\t\t\t\t\tfail! ×\n", iValArr3[i]);
		}
	}
	
	printf("\nAVLTree state : \n");
	printf("\tRootNode : %p\t\t=> not 0\n", objTree->RootNode);
	printf("\tCount : %d\t\t\t\t=> 25\n", objTree->Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objTree->objMM.ItemLength, sizeof(AVLT_Test_Struct) + sizeof(AVLTree_NodeBase));
	printf("\tMM.MMU.Count : %d\t\t\t=> 1\n", objTree->objMM.MMU.Count);
	printf("\tMM.MMU.AllocCount : %d\t\t\t=> 64\n", objTree->objMM.MMU.AllocCount);
	if ( objTree->objMM.MMU.Memory ) {
		printf("\tMM.MMU.Memory : %p\t\t\tpass! √\n", objTree->objMM.MMU.Memory);
	} else {
		printf("\tMM.MMU.Memory : %p\t\t\tfail! ×\n", objTree->objMM.MMU.Memory);
	}
	
	printf("\nAVLTree print : \n");
	avltree_print(objTree->RootNode);
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 7 : add node
	printf("AVLTree test subject 7 : add node\n\n");
	int iValArr4[] = { 380, 241, 392, 270, 391, 208, 321, 293, 325, 227 };
	for ( int i = 0; i < 10; i++ ) {
		int bNew;
		AVLT_Test_Object objNode = AVLTree_AddNode(objTree, (void*)(intptr_t)iValArr4[i], &bNew);
		if ( objNode ) {
			objNode->Val = iValArr4[i];
			if ( bNew ) {
				printf("AVLTree add node , k&v = %d (new)\t\t\t\tpass! √\n", iValArr4[i]);
			} else {
				printf("AVLTree add node , k&v = %d\t\t\t\t\tpass! √\n", iValArr4[i]);
			}
		} else {
			printf("AVLTree add node , k&v = %d\t\t\t\tfail! ×\n", iValArr4[i]);
		}
	}
	
	printf("\nAVLTree state : \n");
	printf("\tRootNode : %p\t\t=> not 0\n", objTree->RootNode);
	printf("\tCount : %d\t\t\t\t=> 30\n", objTree->Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objTree->objMM.ItemLength, sizeof(AVLT_Test_Struct) + sizeof(AVLTree_NodeBase));
	printf("\tMM.MMU.Count : %d\t\t\t=> 1\n", objTree->objMM.MMU.Count);
	printf("\tMM.MMU.AllocCount : %d\t\t\t=> 64\n", objTree->objMM.MMU.AllocCount);
	if ( objTree->objMM.MMU.Memory ) {
		printf("\tMM.MMU.Memory : %p\t\t\tpass! √\n", objTree->objMM.MMU.Memory);
	} else {
		printf("\tMM.MMU.Memory : %p\t\t\tfail! ×\n", objTree->objMM.MMU.Memory);
	}
	
	printf("\nAVLTree print : \n");
	avltree_print(objTree->RootNode);
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 8 : add node
	printf("AVLTree test subject 8 : add node\n\n");
	int iValArr5[] = { 380, 444, 453, 270, 391, 442, 486, 293, 497, 227, 552, 504, 516, 583, 565, 569, 597, 554, 543, 529, 662, 607, 642, 623, 689, 604, 666, 639, 677, 679 };
	for ( int i = 0; i < 30; i++ ) {
		int bNew;
		AVLT_Test_Object objNode = AVLTree_AddNode(objTree, (void*)(intptr_t)iValArr5[i], &bNew);
		if ( objNode ) {
			objNode->Val = iValArr5[i];
			if ( bNew ) {
				printf("AVLTree add node , k&v = %d (new)\t\t\t\tpass! √\n", iValArr5[i]);
			} else {
				printf("AVLTree add node , k&v = %d\t\t\t\t\tpass! √\n", iValArr5[i]);
			}
		} else {
			printf("AVLTree add node , k&v = %d\t\t\t\tfail! ×\n", iValArr5[i]);
		}
	}
	
	printf("\nAVLTree state : \n");
	printf("\tRootNode : %p\t\t=> not 0\n", objTree->RootNode);
	printf("\tCount : %d\t\t\t\t=> 55\n", objTree->Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objTree->objMM.ItemLength, sizeof(AVLT_Test_Struct) + sizeof(AVLTree_NodeBase));
	printf("\tMM.MMU.Count : %d\t\t\t=> 1\n", objTree->objMM.MMU.Count);
	printf("\tMM.MMU.AllocCount : %d\t\t\t=> 64\n", objTree->objMM.MMU.AllocCount);
	if ( objTree->objMM.MMU.Memory ) {
		printf("\tMM.MMU.Memory : %p\t\t\tpass! √\n", objTree->objMM.MMU.Memory);
	} else {
		printf("\tMM.MMU.Memory : %p\t\t\tfail! ×\n", objTree->objMM.MMU.Memory);
	}
	
	printf("\nAVLTree print : \n");
	avltree_print(objTree->RootNode);
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 9 : struct unit & destroy
	printf("AVLTree test subject 9 : struct unit & destroy\n\n");
	AVLTree_Unit(objTree);
	printf("AVLTree object (%p) already unit!\n\n", objTree);
	
	printf("\nAVLTree state : \n");
	printf("\tRootNode : %p\t\t=> 0\n", objTree->RootNode);
	printf("\tCount : %d\t\t\t\t=> 0\n", objTree->Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objTree->objMM.ItemLength, sizeof(AVLT_Test_Struct) + sizeof(AVLTree_NodeBase));
	printf("\tMM.MMU.Count : %d\t\t\t=> 0\n", objTree->objMM.MMU.Count);
	printf("\tMM.MMU.AllocCount : %d\t\t\t=> 0\n", objTree->objMM.MMU.AllocCount);
	if ( objTree->objMM.MMU.Memory ) {
		printf("\tMM.MMU.Memory : %p\t\t\tfail! ×\n", objTree->objMM.MMU.Memory);
	} else {
		printf("\tMM.MMU.Memory : %p\t\t\tpass! √\n", objTree->objMM.MMU.Memory);
	}
	
	AVLTree_Destroy(objTree);
	printf("\nAVLTree object (%p) already destroyed!\n", objTree);
	
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	MMU_Unit();
}


