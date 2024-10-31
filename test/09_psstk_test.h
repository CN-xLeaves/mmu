


void test_psstk()
{
	system("cls");
	MMU_Init();
	
	
	
	// subject 1 : create object
	printf("PSSTK test subject 1 : create object\n\n");
	PSSTK_Object objSTK = PSSTK_Create(24);
	if ( objSTK ) {
		printf("PSSTK object : %p\t\t\t\tpass! √\n", objSTK);
		printf("\tCount : %d\t\t\t\t=> 0\n", objSTK->Count);
		printf("\tMaxCount : %d\t\t\t\t=> 24\n", objSTK->MaxCount);
		if ( objSTK->Memory ) {
			printf("\tMemory : %p\t\t\tpass! √\n", objSTK->Memory);
		} else {
			printf("\tMemory : %p\t\t\tfail! ×\n", objSTK->Memory);
		}
	} else {
		printf("PSSTK object : %p\t\t\t\t\tfail! ×\n", objSTK);
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 2 : push
	printf("PSSTK test subject 2 : push\n\n");
	for ( int i = 0; i < 5; i++ ) {
		int idx = PSSTK_Push(objSTK, (void*)(intptr_t)i);
		printf("push stack idx : %d (%d)\n", idx, i);
	}
	
	printf("\nPSSTK state : \n");
	printf("\tCount : %d\t\t\t\t=> 5\n", objSTK->Count);
	printf("\tMaxCount : %d\t\t\t\t=> 24\n", objSTK->MaxCount);
	if ( objSTK->Memory ) {
		printf("\tMemory : %p\t\t\tpass! √\n", objSTK->Memory);
	} else {
		printf("\tMemory : %p\t\t\tfail! ×\n", objSTK->Memory);
	}
	printf("\tstack top val : %d\n", PSSTK_Top(objSTK));
	
	printf("\nPSSTK values : \n");
	printf("\tidx\tptr\texpected val\n");
	intptr_t t1val[5] = { 0, 1, 2, 3, 4 };
	for ( int i = 1; i <= objSTK->Count; i++ ) {
		intptr_t iPtr = (intptr_t)PSSTK_GetPos_Unsafe(objSTK, i);
		if ( iPtr == t1val[i - 1] ) {
			printf("\t%d\t%d\t%d\t\t\t\tsucc! √\n", i, iPtr, t1val[i - 1]);
		} else {
			printf("\t%d\t%d\t%d\t\t\t\tfail! ×\n", i, iPtr, t1val[i - 1]);
		}
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 3 : pop
	printf("PSSTK test subject 3 : pop\n\n");
	intptr_t iPop = (intptr_t)PSSTK_Pop(objSTK);
	if ( iPop == 4 ) {
		printf("pop value : %d\t\t\t\t\t\tpass! √\n", iPop);
	} else {
		printf("pop value : %d\t\t\t\t\t\tfail! ×\n", iPop);
	}
	iPop = (intptr_t)PSSTK_Pop(objSTK);
	if ( iPop == 3 ) {
		printf("pop value : %d\t\t\t\t\t\tpass! √\n", iPop);
	} else {
		printf("pop value : %d\t\t\t\t\t\tfail! ×\n", iPop);
	}
	
	printf("\nPSSTK state : \n");
	printf("\tCount : %d\t\t\t\t=> 3\n", objSTK->Count);
	printf("\tMaxCount : %d\t\t\t\t=> 24\n", objSTK->MaxCount);
	if ( objSTK->Memory ) {
		printf("\tMemory : %p\t\t\tpass! √\n", objSTK->Memory);
	} else {
		printf("\tMemory : %p\t\t\tfail! ×\n", objSTK->Memory);
	}
	printf("\tstack top val : %d\n", PSSTK_Top(objSTK));
	
	printf("\nPSSTK values : \n");
	printf("\tidx\tptr\texpected val\n");
	for ( int i = 1; i <= objSTK->Count; i++ ) {
		intptr_t iPtr = (intptr_t)PSSTK_GetPos_Unsafe(objSTK, i);
		if ( iPtr == t1val[i - 1] ) {
			printf("\t%d\t%d\t%d\t\t\t\tsucc! √\n", i, iPtr, t1val[i - 1]);
		} else {
			printf("\t%d\t%d\t%d\t\t\t\tfail! ×\n", i, iPtr, t1val[i - 1]);
		}
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 4 : push
	printf("PSSTK test subject 4 : push\n\n");
	for ( int i = 0; i < 5; i++ ) {
		int idx = PSSTK_Push(objSTK, (void*)(intptr_t)((i + 1) * 10));
		printf("push stack idx : %d (%d)\n", idx, (i + 1) * 10);
	}
	
	printf("\nPSSTK state : \n");
	printf("\tCount : %d\t\t\t\t=> 8\n", objSTK->Count);
	printf("\tMaxCount : %d\t\t\t\t=> 24\n", objSTK->MaxCount);
	if ( objSTK->Memory ) {
		printf("\tMemory : %p\t\t\tpass! √\n", objSTK->Memory);
	} else {
		printf("\tMemory : %p\t\t\tfail! ×\n", objSTK->Memory);
	}
	printf("\tstack top val : %d\n", PSSTK_Top(objSTK));
	
	printf("\nPSSTK values : \n");
	printf("\tidx\tptr\texpected val\n");
	int t2val[8] = { 0, 1, 2, 10, 20, 30, 40, 50 };
	for ( int i = 1; i <= objSTK->Count; i++ ) {
		intptr_t iPtr = (intptr_t)PSSTK_GetPos_Unsafe(objSTK, i);
		if ( iPtr == t2val[i - 1] ) {
			printf("\t%d\t%d\t%d\t\t\t\tsucc! √\n", i, iPtr, t2val[i - 1]);
		} else {
			printf("\t%d\t%d\t%d\t\t\t\tfail! ×\n", i, iPtr, t2val[i - 1]);
		}
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 5 : overflow
	printf("PSSTK test subject 5 : overflow\n\n");
	iPop = (intptr_t)PSSTK_Pop(objSTK);
	if ( iPop == 50 ) {
		printf("pop value : %d\t\t\t\t\t\tpass! √\n", iPop);
	} else {
		printf("pop value : %d\t\t\t\t\t\tfail! ×\n", iPop);
	}
	for ( int i = 0; i < 17; i++ ) {
		int idx = PSSTK_Push(objSTK, (void*)(intptr_t)((i + 1) * 100));
		printf("push stack idx : %d (%d)\n", idx, (i + 1) * 100);
	}
	// overflow item
	int idx = PSSTK_Push(objSTK, NULL);
	if ( idx ) {
		printf("PSSTK_Push Succ (Should be fail)\t\t\tfail! ×\n");
	} else {
		printf("PSSTK_Push Fail (Should be fail)\t\t\tsucc! √\n");
	}
	
	printf("\nPSSTK state : \n");
	printf("\tCount : %d\t\t\t\t=> 24\n", objSTK->Count);
	printf("\tMaxCount : %d\t\t\t\t=> 24\n", objSTK->MaxCount);
	if ( objSTK->Memory ) {
		printf("\tMemory : %p\t\t\tpass! √\n", objSTK->Memory);
	} else {
		printf("\tMemory : %p\t\t\tfail! ×\n", objSTK->Memory);
	}
	printf("\tstack top val : %d\n", PSSTK_Top(objSTK));
	
	printf("\nPSSTK values : \n");
	printf("\tidx\tptr\texpected val\n");
	int t3val[24] = { 0, 1, 2, 10, 20, 30, 40, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1600, 1700 };
	for ( int i = 1; i <= objSTK->Count; i++ ) {
		intptr_t iPtr = (intptr_t)PSSTK_GetPos_Unsafe(objSTK, i);
		if ( iPtr == t3val[i - 1] ) {
			printf("\t%d\t%d\t%d\t\t\t\tsucc! √\n", i, iPtr, t3val[i - 1]);
		} else {
			printf("\t%d\t%d\t%d\t\t\t\tfail! ×\n", i, iPtr, t3val[i - 1]);
		}
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// not require testing
	PSSTK_Destroy(objSTK);
	
	
	
	MMU_Unit();
}


