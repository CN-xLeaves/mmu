


// test RBHT32 struct
typedef struct {
	int Val;
} RBHT32_Test_Struct, *RBHT32_Test_Object;



// each table
int test_rbht32_eachproc(HT32_NodeBase* pKey, RBHT32_Test_Object pVal, void* pArg)
{
	printf("\t%s = %d (keylen:%d, Hash:%d)\n", pKey->Key, pVal->Val, pKey->KeyLen, pKey->Hash);
	return 0;
}



void test_rbht32()
{
	system("cls");
	MMU_Init();
	
	
	
	// subject 1 : create object
	printf("RBHT32 test subject 1 : create object\n\n");
	RBHT32_Object objHT = RBHT32_Create(sizeof(RBT_Test_Struct));
	if ( objHT ) {
		//objHT->OnError = (void*)rbht32_error_proc;
		printf("RBHT32 object : %p\t\t\t\tpass! √\n", objHT);
		printf("\tCount : %d\t\t\t\t=> 0\n", objHT->RBT.Count);
		printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objHT->RBT.objMM.ItemLength, sizeof(RBTree_NodeBase) + sizeof(HT32_NodeBase) + sizeof(RBHT32_Test_Struct));
		printf("\tMM.MMU.Count : %d\t\t\t=> 0\n", objHT->RBT.objMM.MMU.Count);
		printf("\tMM.MMU.AllocCount : %d\t\t\t=> 0\n", objHT->RBT.objMM.MMU.AllocCount);
		if ( objHT->RBT.objMM.MMU.Memory ) {
			printf("\tMM.MMU.Memory : %p\t\t\tfail! ×\n", objHT->RBT.objMM.MMU.Memory);
		} else {
			printf("\tMM.MMU.Memory : %p\t\t\tpass! √\n", objHT->RBT.objMM.MMU.Memory);
		}
	} else {
		printf("RBHT32 object : %p\t\t\t\tfail! ×\n", objHT);
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 2 : set key & value
	printf("RBHT32 test subject 2 : set key & value\n\n");
	for ( int i = 0; i < 10; i++ ) {
		char sKey[32];
		sprintf(sKey, "key-idx-%d", i);
		RBHT32_Test_Object pVal = RBHT32_Set(objHT, sKey, strlen(sKey));
		if ( pVal ) {
			pVal->Val = i;
			printf("RBHT32 set key = value, %s = %d\t\t\t\tpass! √\n", sKey, i);
		} else {
			printf("RBHT32 set key = value, %s = %d\t\t\t\tfail! ×\n", sKey, i);
		}
	}
	
	printf("\nRBHT32 state : \n");
	printf("\tCount : %d\t\t\t\t=> 10\n", objHT->RBT.Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objHT->RBT.objMM.ItemLength, sizeof(RBTree_NodeBase) + sizeof(HT32_NodeBase) + sizeof(RBHT32_Test_Struct));
	printf("\tMM.MMU.Count : %d\t\t\t=> 1\n", objHT->RBT.objMM.MMU.Count);
	printf("\tMM.MMU.AllocCount : %d\t\t\t=> 64\n", objHT->RBT.objMM.MMU.AllocCount);
	if ( objHT->RBT.objMM.MMU.Memory ) {
		printf("\tMM.MMU.Memory : %p\t\t\tpass! √\n", objHT->RBT.objMM.MMU.Memory);
	} else {
		printf("\tMM.MMU.Memory : %p\t\t\tfail! ×\n", objHT->RBT.objMM.MMU.Memory);
	}
	
	printf("\nRBHT32 List : \n");
	RBHT32_Walk(objHT, (void*)test_rbht32_eachproc, NULL);
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 3 : remove key & value
	printf("RBHT32 test subject 3 : remove key & value\n\n");
	for ( int i = 0; i < 10; i++ ) {
		if ( (i & 1) == 0 ) {
			char sKey[32];
			sprintf(sKey, "key-idx-%d", i);
			int bRet = RBHT32_Remove(objHT, sKey, strlen(sKey));
			if ( bRet ) {
				printf("RBHT32 remove key (%s)\t\t\t\t\tpass! √\n", sKey);
			} else {
				printf("RBHT32 remove key (%s)\t\t\t\t\tfail! ×\n", sKey);
			}
		}
	}
	
	printf("\nRBHT32 state : \n");
	printf("\tCount : %d\t\t\t\t=> 5\n", objHT->RBT.Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objHT->RBT.objMM.ItemLength, sizeof(RBTree_NodeBase) + sizeof(HT32_NodeBase) + sizeof(RBHT32_Test_Struct));
	printf("\tMM.MMU.Count : %d\t\t\t=> 1\n", objHT->RBT.objMM.MMU.Count);
	printf("\tMM.MMU.AllocCount : %d\t\t\t=> 64\n", objHT->RBT.objMM.MMU.AllocCount);
	if ( objHT->RBT.objMM.MMU.Memory ) {
		printf("\tMM.MMU.Memory : %p\t\t\tpass! √\n", objHT->RBT.objMM.MMU.Memory);
	} else {
		printf("\tMM.MMU.Memory : %p\t\t\tfail! ×\n", objHT->RBT.objMM.MMU.Memory);
	}
	
	printf("\nRBHT32 List : \n");
	RBHT32_Walk(objHT, (void*)test_rbht32_eachproc, NULL);
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 4 : 1m k&v stress testing
	printf("RBHT32 test subject 4 : 1m k&v stress testing\n\n");
	clock_t tStart = clock();
	for ( int i = 0; i < 1000000; i++ ) {
		char sKey[32];
		sprintf(sKey, "key-idx-%d", i);
		RBHT32_Test_Object pVal = RBHT32_Set(objHT, sKey, strlen(sKey));
		if ( pVal ) {
			pVal->Val = i;
		} else {
			printf("RBHT32 set key = value, %s = %d\t\t\t\tfail! ×\n", sKey, i);
		}
	}
	clock_t tStop = clock();
	printf("RBHT32 add 1000000 items time interval is: %f seconds\n", (double)(tStop - tStart) / CLOCKS_PER_SEC);
	
	printf("\nRBHT32 state : \n");
	printf("\tCount : %d\t\t\t\t=> 1000000\n", objHT->RBT.Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objHT->RBT.objMM.ItemLength, sizeof(RBTree_NodeBase) + sizeof(HT32_NodeBase) + sizeof(RBHT32_Test_Struct));
	printf("\tMM.MMU.Count : %d\t\t\t=> 3907\n", objHT->RBT.objMM.MMU.Count);
	printf("\tMM.MMU.AllocCount : %d\t\t=> 3968\n", objHT->RBT.objMM.MMU.AllocCount);
	if ( objHT->RBT.objMM.MMU.Memory ) {
		printf("\tMM.MMU.Memory : %p\t\t\tpass! √\n", objHT->RBT.objMM.MMU.Memory);
	} else {
		printf("\tMM.MMU.Memory : %p\t\t\tfail! ×\n", objHT->RBT.objMM.MMU.Memory);
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 5 : 10m select stress testing
	printf("RBHT32 test subject 5 : 10m select stress testing\n\n");
	tStart = clock();
	for ( int i = 0; i < 10000000; i++ ) {
		char sKey[32];
		sprintf(sKey, "key-idx-%d", i / 10);
		RBHT32_Test_Object pVal = RBHT32_Get(objHT, sKey, strlen(sKey));
		if ( pVal == NULL ) {
			printf("RBHT32 get key (%s) return NULL\t\t\t\t\tfail! ×\n", sKey);
		}
	}
	tStop = clock();
	printf("RBHT32 select 10000000 items time interval is: %f seconds\n", (double)(tStop - tStart) / CLOCKS_PER_SEC);
	
	printf("\nRBHT32 state : \n");
	printf("\tCount : %d\t\t\t\t=> 1000000\n", objHT->RBT.Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objHT->RBT.objMM.ItemLength, sizeof(RBTree_NodeBase) + sizeof(HT32_NodeBase) + sizeof(RBHT32_Test_Struct));
	printf("\tMM.MMU.Count : %d\t\t\t=> 3907\n", objHT->RBT.objMM.MMU.Count);
	printf("\tMM.MMU.AllocCount : %d\t\t=> 3968\n", objHT->RBT.objMM.MMU.AllocCount);
	if ( objHT->RBT.objMM.MMU.Memory ) {
		printf("\tMM.MMU.Memory : %p\t\t\tpass! √\n", objHT->RBT.objMM.MMU.Memory);
	} else {
		printf("\tMM.MMU.Memory : %p\t\t\tfail! ×\n", objHT->RBT.objMM.MMU.Memory);
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 9 : struct unit & destroy
	printf("RBHT32 test subject 9 : struct unit & destroy\n\n");
	RBHT32_Unit(objHT);
	printf("RBHT32 object (%p) already unit!\n\n", objHT);
	
	printf("\nRBHT32 state : \n");
	printf("\tCount : %d\t\t\t\t=> 0\n", objHT->RBT.Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objHT->RBT.objMM.ItemLength, sizeof(RBTree_NodeBase) + sizeof(HT32_NodeBase) + sizeof(RBHT32_Test_Struct));
	printf("\tMM.MMU.Count : %d\t\t\t=> 0\n", objHT->RBT.objMM.MMU.Count);
	printf("\tMM.MMU.AllocCount : %d\t\t\t=> 0\n", objHT->RBT.objMM.MMU.AllocCount);
	if ( objHT->RBT.objMM.MMU.Memory ) {
		printf("\tMM.MMU.Memory : %p\t\t\tfail! ×\n", objHT->RBT.objMM.MMU.Memory);
	} else {
		printf("\tMM.MMU.Memory : %p\t\t\tpass! √\n", objHT->RBT.objMM.MMU.Memory);
	}
	
	RBHT32_Destroy(objHT);
	printf("\nRBHT32 object (%p) already destroyed!\n", objHT);
	
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	MMU_Unit();
}


