


// test SDSTK struct
typedef struct {
	int iVal;
	char sVal[32];
} SDSTK_Test_Struct, *SDSTK_Test_Object;



void sdstk_error_proc(SDSTK_Object objSTK, int ErrorID)
{
	printf("!!! Error !!! error code : %d\n", ErrorID);
}



void test_sdstk()
{
	system("cls");
	MMU_Init();
	
	
	
	// subject 1 : create object
	printf("SDSTK test subject 1 : create object\n\n");
	SDSTK_Object objSTK = SDSTK_Create(sizeof(SDSTK_Test_Struct));
	if ( objSTK ) {
		objSTK->OnError = (void*)sdstk_error_proc;
		printf("SDSTK object : %p\t\t\t\tpass! √\n", objSTK);
		printf("\tItemLength : %d\t\t\t\t=> %d\n", objSTK->ItemLength, sizeof(MM256_Test_Struct));
		printf("\tCount : %d\t\t\t\t=> 0\n", objSTK->Count);
		printf("\tMMU.Count : %d\t\t\t\t=> 0\n", objSTK->MMU.Count);
		printf("\tMMU.AllocCount : %d\t\t\t=> 0\n", objSTK->MMU.AllocCount);
		if ( objSTK->MMU.Memory ) {
			printf("\tMMU.Memory : %p\t\t\tfail! ×\n", objSTK->MMU.Memory);
		} else {
			printf("\tMMU.Memory : %p\t\t\tpass! √\n", objSTK->MMU.Memory);
		}
	} else {
		printf("SDSTK object : %p\t\t\t\t\tfail! ×\n", objSTK);
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 2 : push
	printf("SDSTK test subject 2 : push\n\n");
	for ( int i = 0; i < 5; i++ ) {
		SDSTK_Test_Object pDat1 = SDSTK_Push(objSTK);
		pDat1->iVal = i;
		sprintf(pDat1->sVal, "Push Item idx : %d", i);
		printf("push stack ptr : %p (%s)\n", pDat1, pDat1->sVal);
	}
	
	printf("\nSDSTK state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objSTK->ItemLength, sizeof(MM256_Test_Struct));
	printf("\tCount : %d\t\t\t\t=> 5\n", objSTK->Count);
	printf("\tMMU.Count : %d\t\t\t\t=> 1\n", objSTK->MMU.Count);
	printf("\tMMU.AllocCount : %d\t\t\t=> 64\n", objSTK->MMU.AllocCount);
	if ( objSTK->MMU.Memory ) {
		printf("\tMMU.Memory : %p\t\t\tpass! √\n", objSTK->MMU.Memory);
	} else {
		printf("\tMMU.Memory : %p\t\t\tfail! ×\n", objSTK->MMU.Memory);
	}
	SDSTK_Test_Object pTop = SDSTK_Top(objSTK);
	printf("\tstack top iVal : %d\n", pTop->iVal);
	
	printf("\nSDSTK values : \n");
	printf("\tidx\tptr\t\t\tiVal\tsVal\t\t\texpected val\n");
	int t1val[5] = { 0, 1, 2, 3, 4 };
	for ( int i = 1; i <= objSTK->Count; i++ ) {
		SDSTK_Test_Object pDat1 = SDSTK_GetPos_Unsafe(objSTK, i);
		if ( pDat1->iVal == t1val[i - 1] ) {
			printf("\t%d\t%p\t%d\t%s\t%d\t\tsucc! √\n", i, pDat1, pDat1->iVal, pDat1->sVal, t1val[i - 1]);
		} else {
			printf("\t%d\t%p\t%d\t%s\t%d\t\tfail! ×\n", i, pDat1, pDat1->iVal, pDat1->sVal, t1val[i - 1]);
		}
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 3 : pop
	printf("SDSTK test subject 3 : pop\n\n");
	SDSTK_Test_Object pPop = SDSTK_Pop(objSTK);
	if ( pPop->iVal == 4 ) {
		printf("pop value : %d (%s)\t\t\tpass! √\n", pPop->iVal, pPop->sVal);
	} else {
		printf("pop value : %d (%s)\t\t\tfail! ×\n", pPop->iVal, pPop->sVal);
	}
	pPop = SDSTK_Pop(objSTK);
	if ( pPop->iVal == 3 ) {
		printf("pop value : %d (%s)\t\t\tpass! √\n", pPop->iVal, pPop->sVal);
	} else {
		printf("pop value : %d (%s)\t\t\tfail! ×\n", pPop->iVal, pPop->sVal);
	}
	
	printf("\nSDSTK state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objSTK->ItemLength, sizeof(MM256_Test_Struct));
	printf("\tCount : %d\t\t\t\t=> 3\n", objSTK->Count);
	printf("\tMMU.Count : %d\t\t\t\t=> 1\n", objSTK->MMU.Count);
	printf("\tMMU.AllocCount : %d\t\t\t=> 64\n", objSTK->MMU.AllocCount);
	if ( objSTK->MMU.Memory ) {
		printf("\tMMU.Memory : %p\t\t\tpass! √\n", objSTK->MMU.Memory);
	} else {
		printf("\tMMU.Memory : %p\t\t\tfail! ×\n", objSTK->MMU.Memory);
	}
	pTop = SDSTK_Top(objSTK);
	printf("\tstack top iVal : %d\n", pTop->iVal);
	
	printf("\nSDSTK values : \n");
	printf("\tidx\tptr\t\t\tiVal\tsVal\t\t\texpected val\n");
	for ( int i = 1; i <= objSTK->Count; i++ ) {
		SDSTK_Test_Object pDat1 = SDSTK_GetPos_Unsafe(objSTK, i);
		if ( pDat1->iVal == t1val[i - 1] ) {
			printf("\t%d\t%p\t%d\t%s\t%d\t\tsucc! √\n", i, pDat1, pDat1->iVal, pDat1->sVal, t1val[i - 1]);
		} else {
			printf("\t%d\t%p\t%d\t%s\t%d\t\tfail! ×\n", i, pDat1, pDat1->iVal, pDat1->sVal, t1val[i - 1]);
		}
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 4 : push data
	printf("SDSTK test subject 4 : push data\n\n");
	for ( int i = 0; i < 5; i++ ) {
		SDSTK_Test_Struct stuDat;
		stuDat.iVal = (i + 1) * 10;
		sprintf(stuDat.sVal, "Push Data idx : %d", stuDat.iVal);
		int idx = SDSTK_PushData(objSTK, &stuDat);
		printf("push stack idx : %d (%s)\n", idx, stuDat.sVal);
	}
	
	printf("\nSDSTK state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objSTK->ItemLength, sizeof(MM256_Test_Struct));
	printf("\tCount : %d\t\t\t\t=> 8\n", objSTK->Count);
	printf("\tMMU.Count : %d\t\t\t\t=> 1\n", objSTK->MMU.Count);
	printf("\tMMU.AllocCount : %d\t\t\t=> 64\n", objSTK->MMU.AllocCount);
	if ( objSTK->MMU.Memory ) {
		printf("\tMMU.Memory : %p\t\t\tpass! √\n", objSTK->MMU.Memory);
	} else {
		printf("\tMMU.Memory : %p\t\t\tfail! ×\n", objSTK->MMU.Memory);
	}
	pTop = SDSTK_Top(objSTK);
	printf("\tstack top iVal : %d\n", pTop->iVal);
	
	printf("\nSDSTK values : \n");
	printf("\tidx\tptr\t\t\tiVal\tsVal\t\t\texpected val\n");
	int t2val[8] = { 0, 1, 2, 10, 20, 30, 40, 50 };
	for ( int i = 1; i <= objSTK->Count; i++ ) {
		SDSTK_Test_Object pDat1 = SDSTK_GetPos_Unsafe(objSTK, i);
		if ( pDat1->iVal == t2val[i - 1] ) {
			printf("\t%d\t%p\t%d\t%s\t%d\t\tsucc! √\n", i, pDat1, pDat1->iVal, pDat1->sVal, t2val[i - 1]);
		} else {
			printf("\t%d\t%p\t%d\t%s\t%d\t\tfail! ×\n", i, pDat1, pDat1->iVal, pDat1->sVal, t2val[i - 1]);
		}
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 5 : overflow
	printf("SDSTK test subject 5 : overflow\n\n");
	pPop = SDSTK_Pop(objSTK);
	if ( pPop->iVal == 50 ) {
		printf("pop value : %d (%s)\t\t\tpass! √\n", pPop->iVal, pPop->sVal);
	} else {
		printf("pop value : %d (%s)\t\t\tfail! ×\n", pPop->iVal, pPop->sVal);
	}
	for ( int i = 0; i < 249; i++ ) {
		SDSTK_Test_Object pDat1 = SDSTK_Push(objSTK);
		pDat1->iVal = (i + 1) * 100;
		sprintf(pDat1->sVal, "New Item idx : %d", pDat1->iVal);
		printf("push stack ptr : %p (%s)\n", pDat1, pDat1->sVal);
	}
	// overflow item
	SDSTK_Test_Object pDat = SDSTK_Push(objSTK);
	if ( pDat ) {
		pDat->iVal = 123456;
		strcpy(pDat->sVal, "overflow item : 123456");
		printf("SDSTK_Push Succ (Should be succ)\t\t\tsucc! √\n");
	} else {
		printf("SDSTK_Push Fail (Should be succ)\t\t\tfail! ×\n");
	}
	
	printf("\nSDSTK state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objSTK->ItemLength, sizeof(MM256_Test_Struct));
	printf("\tCount : %d\t\t\t\t=> 257\n", objSTK->Count);
	printf("\tMMU.Count : %d\t\t\t\t=> 2\n", objSTK->MMU.Count);
	printf("\tMMU.AllocCount : %d\t\t\t=> 64\n", objSTK->MMU.AllocCount);
	if ( objSTK->MMU.Memory ) {
		printf("\tMMU.Memory : %p\t\t\tpass! √\n", objSTK->MMU.Memory);
	} else {
		printf("\tMMU.Memory : %p\t\t\tfail! ×\n", objSTK->MMU.Memory);
	}
	pTop = SDSTK_Top(objSTK);
	printf("\tstack top iVal : %d\n", pTop->iVal);
	
	printf("\nSDSTK values : \n");
	printf("\tidx\tptr\t\t\tiVal\tsVal\t\t\texpected val\n");
	for ( int i = 1; i <= objSTK->Count; i++ ) {
		SDSTK_Test_Object pDat1 = SDSTK_GetPos_Unsafe(objSTK, i);
		int iExpected = i > 7 ? ((i - 7) * 100) : t2val[i - 1];
		if ( i > 256 ) { iExpected = 123456; }
		if ( pDat1->iVal == iExpected ) {
			printf("\t%d\t%p\t%d\t%s\t%d\t\tsucc! √\n", i, pDat1, pDat1->iVal, pDat1->sVal, iExpected);
		} else {
			printf("\t%d\t%p\t%d\t%s\t%d\t\tfail! ×\n", i, pDat1, pDat1->iVal, pDat1->sVal, iExpected);
		}
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 6 : struct unit & destroy
	printf("SDSTK test subject 6 : struct unit & destroy\n\n");
	SDSTK_Unit(objSTK);
	printf("SDSTK object (%p) already unit!\n\n", objSTK);
	
	printf("\nSDSTK state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objSTK->ItemLength, sizeof(MM256_Test_Struct));
	printf("\tCount : %d\t\t\t\t=> 0\n", objSTK->Count);
	printf("\tMMU.Count : %d\t\t\t\t=> 0\n", objSTK->MMU.Count);
	printf("\tMMU.AllocCount : %d\t\t\t=> 0\n", objSTK->MMU.AllocCount);
	if ( objSTK->MMU.Memory ) {
		printf("\tMMU.Memory : %p\t\t\tfail! ×\n", objSTK->MMU.Memory);
	} else {
		printf("\tMMU.Memory : %p\t\t\tpass! √\n", objSTK->MMU.Memory);
	}
	
	SDSTK_Destroy(objSTK);
	printf("\nSDSTK object (%p) already destroyed!\n", objSTK);
	
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	MMU_Unit();
}


