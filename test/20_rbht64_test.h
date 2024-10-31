


// test RBHT64 struct
typedef struct {
	int Val;
} RBHT64_Test_Struct, *RBHT64_Test_Object;



// each table
int test_rbht64_eachproc(HT64_NodeBase* pKey, RBHT64_Test_Object pVal, void* pArg)
{
	printf("\t%s = %d (keylen:%d, Hash:%lld)\n", pKey->Key, pVal->Val, pKey->KeyLen, pKey->Hash);
	return 0;
}



void test_rbht64()
{
	system("cls");
	MMU_Init();
	
	
	
	// subject 1 : create object
	printf("RBHT64 test subject 1 : create object\n\n");
	RBHT64_Object objHT = RBHT64_Create(sizeof(RBT_Test_Struct));
	if ( objHT ) {
		//objHT->OnError = (void*)rbht64_error_proc;
		printf("RBHT64 object : %p\t\t\t\tpass! √\n", objHT);
		printf("\tCount : %d\t\t\t\t=> 0\n", objHT->RBT.Count);
		printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objHT->RBT.objMM.ItemLength, sizeof(RBTree_NodeBase) + sizeof(HT64_NodeBase) + sizeof(RBHT64_Test_Struct));
		printf("\tMM.MMU.Count : %d\t\t\t=> 0\n", objHT->RBT.objMM.MMU.Count);
		printf("\tMM.MMU.AllocCount : %d\t\t\t=> 0\n", objHT->RBT.objMM.MMU.AllocCount);
		if ( objHT->RBT.objMM.MMU.Memory ) {
			printf("\tMM.MMU.Memory : %p\t\t\tfail! ×\n", objHT->RBT.objMM.MMU.Memory);
		} else {
			printf("\tMM.MMU.Memory : %p\t\t\tpass! √\n", objHT->RBT.objMM.MMU.Memory);
		}
	} else {
		printf("RBHT64 object : %p\t\t\t\tfail! ×\n", objHT);
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 2 : set key & value
	printf("RBHT64 test subject 2 : set key & value\n\n");
	for ( int i = 0; i < 10; i++ ) {
		char sKey[32];
		sprintf(sKey, "key-idx-%d", i);
		RBHT64_Test_Object pVal = RBHT64_Set(objHT, sKey, strlen(sKey));
		if ( pVal ) {
			pVal->Val = i;
			printf("RBHT64 set key = value, %s = %d\t\t\t\tpass! √\n", sKey, i);
		} else {
			printf("RBHT64 set key = value, %s = %d\t\t\t\tfail! ×\n", sKey, i);
		}
	}
	
	printf("\nRBHT64 state : \n");
	printf("\tCount : %d\t\t\t\t=> 10\n", objHT->RBT.Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objHT->RBT.objMM.ItemLength, sizeof(RBTree_NodeBase) + sizeof(HT64_NodeBase) + sizeof(RBHT64_Test_Struct));
	printf("\tMM.MMU.Count : %d\t\t\t=> 1\n", objHT->RBT.objMM.MMU.Count);
	printf("\tMM.MMU.AllocCount : %d\t\t\t=> 64\n", objHT->RBT.objMM.MMU.AllocCount);
	if ( objHT->RBT.objMM.MMU.Memory ) {
		printf("\tMM.MMU.Memory : %p\t\t\tpass! √\n", objHT->RBT.objMM.MMU.Memory);
	} else {
		printf("\tMM.MMU.Memory : %p\t\t\tfail! ×\n", objHT->RBT.objMM.MMU.Memory);
	}
	
	printf("\nRBHT64 List : \n");
	RBHT64_Walk(objHT, (void*)test_rbht64_eachproc, NULL);
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 3 : remove key & value
	printf("RBHT64 test subject 3 : remove key & value\n\n");
	for ( int i = 0; i < 10; i++ ) {
		if ( (i & 1) == 0 ) {
			char sKey[32];
			sprintf(sKey, "key-idx-%d", i);
			int bRet = RBHT64_Remove(objHT, sKey, strlen(sKey));
			if ( bRet ) {
				printf("RBHT64 remove key (%s)\t\t\t\t\tpass! √\n", sKey);
			} else {
				printf("RBHT64 remove key (%s)\t\t\t\t\tfail! ×\n", sKey);
			}
		}
	}
	
	printf("\nRBHT64 state : \n");
	printf("\tCount : %d\t\t\t\t=> 5\n", objHT->RBT.Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objHT->RBT.objMM.ItemLength, sizeof(RBTree_NodeBase) + sizeof(HT64_NodeBase) + sizeof(RBHT64_Test_Struct));
	printf("\tMM.MMU.Count : %d\t\t\t=> 1\n", objHT->RBT.objMM.MMU.Count);
	printf("\tMM.MMU.AllocCount : %d\t\t\t=> 64\n", objHT->RBT.objMM.MMU.AllocCount);
	if ( objHT->RBT.objMM.MMU.Memory ) {
		printf("\tMM.MMU.Memory : %p\t\t\tpass! √\n", objHT->RBT.objMM.MMU.Memory);
	} else {
		printf("\tMM.MMU.Memory : %p\t\t\tfail! ×\n", objHT->RBT.objMM.MMU.Memory);
	}
	
	printf("\nRBHT64 List : \n");
	RBHT64_Walk(objHT, (void*)test_rbht64_eachproc, NULL);
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 4 : 1m k&v stress testing
	printf("RBHT64 test subject 4 : 1m k&v stress testing\n\n");
	clock_t tStart = clock();
	for ( int i = 0; i < 1000000; i++ ) {
		char sKey[32];
		sprintf(sKey, "key-idx-%d", i);
		RBHT64_Test_Object pVal = RBHT64_Set(objHT, sKey, strlen(sKey));
		if ( pVal ) {
			pVal->Val = i;
		} else {
			printf("RBHT64 set key = value, %s = %d\t\t\t\tfail! ×\n", sKey, i);
		}
	}
	clock_t tStop = clock();
	printf("RBHT64 add 1000000 items time interval is: %f seconds\n", (double)(tStop - tStart) / CLOCKS_PER_SEC);
	
	printf("\nRBHT64 state : \n");
	printf("\tCount : %d\t\t\t\t=> 1000000\n", objHT->RBT.Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objHT->RBT.objMM.ItemLength, sizeof(RBTree_NodeBase) + sizeof(HT64_NodeBase) + sizeof(RBHT64_Test_Struct));
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
	printf("RBHT64 test subject 5 : 10m select stress testing\n\n");
	tStart = clock();
	for ( int i = 0; i < 10000000; i++ ) {
		char sKey[32];
		sprintf(sKey, "key-idx-%d", i / 10);
		RBHT64_Test_Object pVal = RBHT64_Get(objHT, sKey, strlen(sKey));
		if ( pVal == NULL ) {
			printf("RBHT64 get key (%s) return NULL\t\t\t\t\tfail! ×\n", sKey);
		}
	}
	tStop = clock();
	printf("RBHT64 select 10000000 items time interval is: %f seconds\n", (double)(tStop - tStart) / CLOCKS_PER_SEC);
	
	printf("\nRBHT64 state : \n");
	printf("\tCount : %d\t\t\t\t=> 1000000\n", objHT->RBT.Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objHT->RBT.objMM.ItemLength, sizeof(RBTree_NodeBase) + sizeof(HT64_NodeBase) + sizeof(RBHT64_Test_Struct));
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
	printf("RBHT64 test subject 9 : struct unit & destroy\n\n");
	RBHT64_Unit(objHT);
	printf("RBHT64 object (%p) already unit!\n\n", objHT);
	
	printf("\nRBHT64 state : \n");
	printf("\tCount : %d\t\t\t\t=> 0\n", objHT->RBT.Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objHT->RBT.objMM.ItemLength, sizeof(RBTree_NodeBase) + sizeof(HT64_NodeBase) + sizeof(RBHT64_Test_Struct));
	printf("\tMM.MMU.Count : %d\t\t\t=> 0\n", objHT->RBT.objMM.MMU.Count);
	printf("\tMM.MMU.AllocCount : %d\t\t\t=> 0\n", objHT->RBT.objMM.MMU.AllocCount);
	if ( objHT->RBT.objMM.MMU.Memory ) {
		printf("\tMM.MMU.Memory : %p\t\t\tfail! ×\n", objHT->RBT.objMM.MMU.Memory);
	} else {
		printf("\tMM.MMU.Memory : %p\t\t\tpass! √\n", objHT->RBT.objMM.MMU.Memory);
	}
	
	RBHT64_Destroy(objHT);
	printf("\nRBHT64 object (%p) already destroyed!\n", objHT);
	
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	MMU_Unit();
}


