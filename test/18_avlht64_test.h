


// test AVLHT64 struct
typedef struct {
	int Val;
} AVLHT64_Test_Struct, *AVLHT64_Test_Object;



// each table
int test_avlht64_eachproc(HT64_NodeBase* pKey, AVLHT64_Test_Object pVal, void* pArg)
{
	printf("\t%s = %d (keylen:%d, Hash:%lld)\n", pKey->Key, pVal->Val, pKey->KeyLen, pKey->Hash);
	return 0;
}



void test_avlht64()
{
	system("cls");
	MMU_Init();
	
	
	
	// subject 1 : create object
	printf("AVLHT64 test subject 1 : create object\n\n");
	AVLHT64_Object objHT = AVLHT64_Create(sizeof(AVLT_Test_Struct));
	if ( objHT ) {
		//objHT->OnError = (void*)avlht64_error_proc;
		printf("AVLHT64 object : %p\t\t\t\tpass! √\n", objHT);
		printf("\tCount : %d\t\t\t\t=> 0\n", objHT->AVLT.Count);
		printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objHT->AVLT.objMM.ItemLength, sizeof(AVLTree_NodeBase) + sizeof(HT64_NodeBase) + sizeof(AVLHT64_Test_Struct));
		printf("\tMM.MMU.Count : %d\t\t\t=> 0\n", objHT->AVLT.objMM.MMU.Count);
		printf("\tMM.MMU.AllocCount : %d\t\t\t=> 0\n", objHT->AVLT.objMM.MMU.AllocCount);
		if ( objHT->AVLT.objMM.MMU.Memory ) {
			printf("\tMM.MMU.Memory : %p\t\t\tfail! ×\n", objHT->AVLT.objMM.MMU.Memory);
		} else {
			printf("\tMM.MMU.Memory : %p\t\t\tpass! √\n", objHT->AVLT.objMM.MMU.Memory);
		}
	} else {
		printf("AVLHT64 object : %p\t\t\t\tfail! ×\n", objHT);
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 2 : set key & value
	printf("AVLHT64 test subject 2 : set key & value\n\n");
	for ( int i = 0; i < 10; i++ ) {
		char sKey[32];
		sprintf(sKey, "key-idx-%d", i);
		AVLHT64_Test_Object pVal = AVLHT64_Set(objHT, sKey, strlen(sKey));
		if ( pVal ) {
			pVal->Val = i;
			printf("AVLHT64 set key = value, %s = %d\t\t\t\tpass! √\n", sKey, i);
		} else {
			printf("AVLHT64 set key = value, %s = %d\t\t\t\tfail! ×\n", sKey, i);
		}
	}
	
	printf("\nAVLHT64 state : \n");
	printf("\tCount : %d\t\t\t\t=> 10\n", objHT->AVLT.Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objHT->AVLT.objMM.ItemLength, sizeof(AVLTree_NodeBase) + sizeof(HT64_NodeBase) + sizeof(AVLHT64_Test_Struct));
	printf("\tMM.MMU.Count : %d\t\t\t=> 1\n", objHT->AVLT.objMM.MMU.Count);
	printf("\tMM.MMU.AllocCount : %d\t\t\t=> 64\n", objHT->AVLT.objMM.MMU.AllocCount);
	if ( objHT->AVLT.objMM.MMU.Memory ) {
		printf("\tMM.MMU.Memory : %p\t\t\tpass! √\n", objHT->AVLT.objMM.MMU.Memory);
	} else {
		printf("\tMM.MMU.Memory : %p\t\t\tfail! ×\n", objHT->AVLT.objMM.MMU.Memory);
	}
	
	printf("\nAVLHT64 List : \n");
	AVLHT64_Walk(objHT, (void*)test_avlht64_eachproc, NULL);
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 3 : remove key & value
	printf("AVLHT64 test subject 3 : remove key & value\n\n");
	for ( int i = 0; i < 10; i++ ) {
		if ( (i & 1) == 0 ) {
			char sKey[32];
			sprintf(sKey, "key-idx-%d", i);
			int bRet = AVLHT64_Remove(objHT, sKey, strlen(sKey));
			if ( bRet ) {
				printf("AVLHT64 remove key (%s)\t\t\t\t\tpass! √\n", sKey);
			} else {
				printf("AVLHT64 remove key (%s)\t\t\t\t\tfail! ×\n", sKey);
			}
		}
	}
	
	printf("\nAVLHT64 state : \n");
	printf("\tCount : %d\t\t\t\t=> 5\n", objHT->AVLT.Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objHT->AVLT.objMM.ItemLength, sizeof(AVLTree_NodeBase) + sizeof(HT64_NodeBase) + sizeof(AVLHT64_Test_Struct));
	printf("\tMM.MMU.Count : %d\t\t\t=> 1\n", objHT->AVLT.objMM.MMU.Count);
	printf("\tMM.MMU.AllocCount : %d\t\t\t=> 64\n", objHT->AVLT.objMM.MMU.AllocCount);
	if ( objHT->AVLT.objMM.MMU.Memory ) {
		printf("\tMM.MMU.Memory : %p\t\t\tpass! √\n", objHT->AVLT.objMM.MMU.Memory);
	} else {
		printf("\tMM.MMU.Memory : %p\t\t\tfail! ×\n", objHT->AVLT.objMM.MMU.Memory);
	}
	
	printf("\nAVLHT64 List : \n");
	AVLHT64_Walk(objHT, (void*)test_avlht64_eachproc, NULL);
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 4 : 1m k&v stress testing
	printf("AVLHT64 test subject 4 : 1m k&v stress testing\n\n");
	clock_t tStart = clock();
	for ( int i = 0; i < 1000000; i++ ) {
		char sKey[32];
		sprintf(sKey, "key-idx-%d", i);
		AVLHT64_Test_Object pVal = AVLHT64_Set(objHT, sKey, strlen(sKey));
		if ( pVal ) {
			pVal->Val = i;
		} else {
			printf("AVLHT64 set key = value, %s = %d\t\t\t\tfail! ×\n", sKey, i);
		}
	}
	clock_t tStop = clock();
	printf("AVLHT64 add 1000000 items time interval is: %f seconds\n", (double)(tStop - tStart) / CLOCKS_PER_SEC);
	
	printf("\nAVLHT64 state : \n");
	printf("\tCount : %d\t\t\t\t=> 1000000\n", objHT->AVLT.Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objHT->AVLT.objMM.ItemLength, sizeof(AVLTree_NodeBase) + sizeof(HT64_NodeBase) + sizeof(AVLHT64_Test_Struct));
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
	printf("AVLHT64 test subject 5 : 10m select stress testing\n\n");
	tStart = clock();
	for ( int i = 0; i < 10000000; i++ ) {
		char sKey[32];
		sprintf(sKey, "key-idx-%d", i / 10);
		AVLHT64_Test_Object pVal = AVLHT64_Get(objHT, sKey, strlen(sKey));
		if ( pVal == NULL ) {
			printf("AVLHT64 get key (%s) return NULL\t\t\t\t\tfail! ×\n", sKey);
		}
	}
	tStop = clock();
	printf("AVLHT64 select 10000000 items time interval is: %f seconds\n", (double)(tStop - tStart) / CLOCKS_PER_SEC);
	
	printf("\nAVLHT64 state : \n");
	printf("\tCount : %d\t\t\t\t=> 1000000\n", objHT->AVLT.Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objHT->AVLT.objMM.ItemLength, sizeof(AVLTree_NodeBase) + sizeof(HT64_NodeBase) + sizeof(AVLHT64_Test_Struct));
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
	printf("AVLHT64 test subject 9 : struct unit & destroy\n\n");
	AVLHT64_Unit(objHT);
	printf("AVLHT64 object (%p) already unit!\n\n", objHT);
	
	printf("\nAVLHT64 state : \n");
	printf("\tCount : %d\t\t\t\t=> 0\n", objHT->AVLT.Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objHT->AVLT.objMM.ItemLength, sizeof(AVLTree_NodeBase) + sizeof(HT64_NodeBase) + sizeof(AVLHT64_Test_Struct));
	printf("\tMM.MMU.Count : %d\t\t\t=> 0\n", objHT->AVLT.objMM.MMU.Count);
	printf("\tMM.MMU.AllocCount : %d\t\t\t=> 0\n", objHT->AVLT.objMM.MMU.AllocCount);
	if ( objHT->AVLT.objMM.MMU.Memory ) {
		printf("\tMM.MMU.Memory : %p\t\t\tfail! ×\n", objHT->AVLT.objMM.MMU.Memory);
	} else {
		printf("\tMM.MMU.Memory : %p\t\t\tpass! √\n", objHT->AVLT.objMM.MMU.Memory);
	}
	
	AVLHT64_Destroy(objHT);
	printf("\nAVLHT64 object (%p) already destroyed!\n", objHT);
	
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	MMU_Unit();
}


