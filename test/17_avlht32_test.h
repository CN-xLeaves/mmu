


// test AVLHT32 struct
typedef struct {
	int Val;
} AVLHT32_Test_Struct, *AVLHT32_Test_Object;



// each table
int test_avlht32_eachproc(HT32_NodeBase* pKey, AVLHT32_Test_Object pVal, void* pArg)
{
	printf("\t%s = %d (keylen:%d, Hash:%d)\n", pKey->Key, pVal->Val, pKey->KeyLen, pKey->Hash);
	return 0;
}



void test_avlht32()
{
	system("cls");
	MMU_Init();
	
	
	
	// subject 1 : create object
	printf("AVLHT32 test subject 1 : create object\n\n");
	AVLHT32_Object objHT = AVLHT32_Create(sizeof(AVLT_Test_Struct));
	if ( objHT ) {
		//objHT->OnError = (void*)avlht32_error_proc;
		printf("AVLHT32 object : %p\t\t\t\tpass! √\n", objHT);
		printf("\tCount : %d\t\t\t\t=> 0\n", objHT->AVLT.Count);
		printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objHT->AVLT.objMM.ItemLength, sizeof(AVLTree_NodeBase) + sizeof(HT32_NodeBase) + sizeof(AVLHT32_Test_Struct));
		printf("\tMM.MMU.Count : %d\t\t\t=> 0\n", objHT->AVLT.objMM.MMU.Count);
		printf("\tMM.MMU.AllocCount : %d\t\t\t=> 0\n", objHT->AVLT.objMM.MMU.AllocCount);
		if ( objHT->AVLT.objMM.MMU.Memory ) {
			printf("\tMM.MMU.Memory : %p\t\t\tfail! ×\n", objHT->AVLT.objMM.MMU.Memory);
		} else {
			printf("\tMM.MMU.Memory : %p\t\t\tpass! √\n", objHT->AVLT.objMM.MMU.Memory);
		}
	} else {
		printf("AVLHT32 object : %p\t\t\t\tfail! ×\n", objHT);
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 2 : set key & value
	printf("AVLHT32 test subject 2 : set key & value\n\n");
	for ( int i = 0; i < 10; i++ ) {
		char sKey[32];
		sprintf(sKey, "key-idx-%d", i);
		AVLHT32_Test_Object pVal = AVLHT32_Set(objHT, sKey, strlen(sKey));
		if ( pVal ) {
			pVal->Val = i;
			printf("AVLHT32 set key = value, %s = %d\t\t\t\tpass! √\n", sKey, i);
		} else {
			printf("AVLHT32 set key = value, %s = %d\t\t\t\tfail! ×\n", sKey, i);
		}
	}
	
	printf("\nAVLHT32 state : \n");
	printf("\tCount : %d\t\t\t\t=> 10\n", objHT->AVLT.Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objHT->AVLT.objMM.ItemLength, sizeof(AVLTree_NodeBase) + sizeof(HT32_NodeBase) + sizeof(AVLHT32_Test_Struct));
	printf("\tMM.MMU.Count : %d\t\t\t=> 1\n", objHT->AVLT.objMM.MMU.Count);
	printf("\tMM.MMU.AllocCount : %d\t\t\t=> 64\n", objHT->AVLT.objMM.MMU.AllocCount);
	if ( objHT->AVLT.objMM.MMU.Memory ) {
		printf("\tMM.MMU.Memory : %p\t\t\tpass! √\n", objHT->AVLT.objMM.MMU.Memory);
	} else {
		printf("\tMM.MMU.Memory : %p\t\t\tfail! ×\n", objHT->AVLT.objMM.MMU.Memory);
	}
	
	printf("\nAVLHT32 List : \n");
	AVLHT32_Walk(objHT, (void*)test_avlht32_eachproc, NULL);
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 3 : remove key & value
	printf("AVLHT32 test subject 3 : remove key & value\n\n");
	for ( int i = 0; i < 10; i++ ) {
		if ( (i & 1) == 0 ) {
			char sKey[32];
			sprintf(sKey, "key-idx-%d", i);
			int bRet = AVLHT32_Remove(objHT, sKey, strlen(sKey));
			if ( bRet ) {
				printf("AVLHT32 remove key (%s)\t\t\t\t\tpass! √\n", sKey);
			} else {
				printf("AVLHT32 remove key (%s)\t\t\t\t\tfail! ×\n", sKey);
			}
		}
	}
	
	printf("\nAVLHT32 state : \n");
	printf("\tCount : %d\t\t\t\t=> 5\n", objHT->AVLT.Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objHT->AVLT.objMM.ItemLength, sizeof(AVLTree_NodeBase) + sizeof(HT32_NodeBase) + sizeof(AVLHT32_Test_Struct));
	printf("\tMM.MMU.Count : %d\t\t\t=> 1\n", objHT->AVLT.objMM.MMU.Count);
	printf("\tMM.MMU.AllocCount : %d\t\t\t=> 64\n", objHT->AVLT.objMM.MMU.AllocCount);
	if ( objHT->AVLT.objMM.MMU.Memory ) {
		printf("\tMM.MMU.Memory : %p\t\t\tpass! √\n", objHT->AVLT.objMM.MMU.Memory);
	} else {
		printf("\tMM.MMU.Memory : %p\t\t\tfail! ×\n", objHT->AVLT.objMM.MMU.Memory);
	}
	
	printf("\nAVLHT32 List : \n");
	AVLHT32_Walk(objHT, (void*)test_avlht32_eachproc, NULL);
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 4 : 1m k&v stress testing
	printf("AVLHT32 test subject 4 : 1m k&v stress testing\n\n");
	clock_t tStart = clock();
	for ( int i = 0; i < 1000000; i++ ) {
		char sKey[32];
		sprintf(sKey, "key-idx-%d", i);
		AVLHT32_Test_Object pVal = AVLHT32_Set(objHT, sKey, strlen(sKey));
		if ( pVal ) {
			pVal->Val = i;
		} else {
			printf("AVLHT32 set key = value, %s = %d\t\t\t\tfail! ×\n", sKey, i);
		}
	}
	clock_t tStop = clock();
	printf("AVLHT32 add 1000000 items time interval is: %f seconds\n", (double)(tStop - tStart) / CLOCKS_PER_SEC);
	
	printf("\nAVLHT32 state : \n");
	printf("\tCount : %d\t\t\t\t=> 1000000\n", objHT->AVLT.Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objHT->AVLT.objMM.ItemLength, sizeof(AVLTree_NodeBase) + sizeof(HT32_NodeBase) + sizeof(AVLHT32_Test_Struct));
	printf("\tMM.MMU.Count : %d\t\t\t=> 3907\n", objHT->AVLT.objMM.MMU.Count);
	printf("\tMM.MMU.AllocCount : %d\t\t=> 3968\n", objHT->AVLT.objMM.MMU.AllocCount);
	if ( objHT->AVLT.objMM.MMU.Memory ) {
		printf("\tMM.MMU.Memory : %p\t\t\tpass! √\n", objHT->AVLT.objMM.MMU.Memory);
	} else {
		printf("\tMM.MMU.Memory : %p\t\t\tfail! ×\n", objHT->AVLT.objMM.MMU.Memory);
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 5 : 10m select stress testing
	printf("AVLHT32 test subject 5 : 10m select stress testing\n\n");
	tStart = clock();
	for ( int i = 0; i < 10000000; i++ ) {
		char sKey[32];
		sprintf(sKey, "key-idx-%d", i / 10);
		AVLHT32_Test_Object pVal = AVLHT32_Get(objHT, sKey, strlen(sKey));
		if ( pVal == NULL ) {
			printf("AVLHT32 get key (%s) return NULL\t\t\t\t\tfail! ×\n", sKey);
		}
	}
	tStop = clock();
	printf("AVLHT32 select 10000000 items time interval is: %f seconds\n", (double)(tStop - tStart) / CLOCKS_PER_SEC);
	
	printf("\nAVLHT32 state : \n");
	printf("\tCount : %d\t\t\t\t=> 1000000\n", objHT->AVLT.Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objHT->AVLT.objMM.ItemLength, sizeof(AVLTree_NodeBase) + sizeof(HT32_NodeBase) + sizeof(AVLHT32_Test_Struct));
	printf("\tMM.MMU.Count : %d\t\t\t=> 3907\n", objHT->AVLT.objMM.MMU.Count);
	printf("\tMM.MMU.AllocCount : %d\t\t=> 3968\n", objHT->AVLT.objMM.MMU.AllocCount);
	if ( objHT->AVLT.objMM.MMU.Memory ) {
		printf("\tMM.MMU.Memory : %p\t\t\tpass! √\n", objHT->AVLT.objMM.MMU.Memory);
	} else {
		printf("\tMM.MMU.Memory : %p\t\t\tfail! ×\n", objHT->AVLT.objMM.MMU.Memory);
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 9 : struct unit & destroy
	printf("AVLHT32 test subject 9 : struct unit & destroy\n\n");
	AVLHT32_Unit(objHT);
	printf("AVLHT32 object (%p) already unit!\n\n", objHT);
	
	printf("\nAVLHT32 state : \n");
	printf("\tCount : %d\t\t\t\t=> 0\n", objHT->AVLT.Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objHT->AVLT.objMM.ItemLength, sizeof(AVLTree_NodeBase) + sizeof(HT32_NodeBase) + sizeof(AVLHT32_Test_Struct));
	printf("\tMM.MMU.Count : %d\t\t\t=> 0\n", objHT->AVLT.objMM.MMU.Count);
	printf("\tMM.MMU.AllocCount : %d\t\t\t=> 0\n", objHT->AVLT.objMM.MMU.AllocCount);
	if ( objHT->AVLT.objMM.MMU.Memory ) {
		printf("\tMM.MMU.Memory : %p\t\t\tfail! ×\n", objHT->AVLT.objMM.MMU.Memory);
	} else {
		printf("\tMM.MMU.Memory : %p\t\t\tpass! √\n", objHT->AVLT.objMM.MMU.Memory);
	}
	
	AVLHT32_Destroy(objHT);
	printf("\nAVLHT32 object (%p) already destroyed!\n", objHT);
	
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	MMU_Unit();
}


