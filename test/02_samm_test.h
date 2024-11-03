


// test SAMM struct
typedef struct {
	int val;
	double num;
} SAMM_Test_Struct, *SAMM_Test_Object;



// SAMM sort proc
int SAMM_TestSortProc(SAMM_Test_Object v1, SAMM_Test_Object v2)
{
	return v1->val - v2->val;
}



void test_samm()
{
	system("cls");
	MMU_Init();
	
	
	
	// subject 1 : create object
	printf("SAMM test subject 1 : create object\n\n");
	SAMM_Object objSAMM = SAMM_Create(sizeof(SAMM_Test_Struct));
	if ( objSAMM ) {
		objSAMM->AllocStep = 16;
		printf("SAMM object : %p\t\t\t\t\tpass! √\n", objSAMM);
		printf("\tMemory : %p\t\t=> 0\n", objSAMM->Memory);
		printf("\tItemLength : %d\t\t\t\t=> %d\n", objSAMM->ItemLength, sizeof(SAMM_Test_Struct));
		printf("\tCount : %d\t\t\t\t=> 0\n", objSAMM->Count);
		printf("\tAllocCount : %d\t\t\t\t=> 0\n", objSAMM->AllocCount);
		printf("\tAllocStep : %d\t\t\t\t=> 16\n", objSAMM->AllocStep);
	} else {
		printf("SAMM object : %p\t\t\t\t\tfail! ×\n", objSAMM);
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 2 : struct init
	printf("SAMM test subject 2 : struct init (Please check print)\n\n");
	SAMM_Struct stuSAMM;
	SAMM_Init(&stuSAMM, sizeof(SAMM_Test_Struct));
	stuSAMM.AllocStep = 16;
	printf("\tMemory : %p\t\t=> 0\n", stuSAMM.Memory);
	printf("\tItemLength : %d\t\t\t\t=> %d\n", stuSAMM.ItemLength, sizeof(SAMM_Test_Struct));
	printf("\tCount : %d\t\t\t\t=> 0\n", stuSAMM.Count);
	printf("\tAllocCount : %d\t\t\t\t=> 0\n", stuSAMM.AllocCount);
	printf("\tAllocStep : %d\t\t\t\t=> 16\n", stuSAMM.AllocStep);
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 3 : malloc
	printf("SAMM test subject 3 : malloc\n\n");
	int bRet = SAMM_Malloc(&stuSAMM, 4);
	printf("SAMM_Malloc return value : %d\t\t\t=> -1\n", bRet);
	if ( stuSAMM.Memory ) {
		printf("SAMM memory ptr : %p\t\t\t\tpass! √\n", stuSAMM.Memory);
		printf("\tItemLength : %d\t\t\t\t=> %d\n", stuSAMM.ItemLength, sizeof(SAMM_Test_Struct));
		printf("\tCount : %d\t\t\t\t=> 0\n", stuSAMM.Count);
		printf("\tAllocCount : %d\t\t\t\t=> 4\n", stuSAMM.AllocCount);
		printf("\tAllocStep : %d\t\t\t\t=> 16\n", stuSAMM.AllocStep);
	} else {
		printf("SAMM memory ptr : %p\t\t\t\tfail! ×\n", stuSAMM.Memory);
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 4 : append item
	printf("SAMM test subject 4 : append item\n\n");
	for ( int i = 0; i < 10; i++ ) {
		unsigned int idx = SAMM_Append(&stuSAMM, 1);
		SAMM_Test_Object objStu = SAMM_GetPtr_Unsafe(&stuSAMM, idx);
		if ( objStu ) {
			objStu->val = idx * 1000 + idx;
			objStu->num = idx * 1000 + idx / 1000.0;
			printf("SAMM_Append idx : %d\t( %d, \t%f\t)\tpass! √\n", idx, objStu->val, objStu->num);
		} else {
			printf("SAMM_Append idx : %d\t\t\t\t\t\tfail! ×\n", idx);
		}
	}
	
	printf("\nSAMM state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", stuSAMM.ItemLength, sizeof(SAMM_Test_Struct));
	printf("\tCount : %d\t\t\t\t=> 10\n", stuSAMM.Count);
	printf("\tAllocCount : %d\t\t\t\t=> 20 ( 4 + 16 )\n", stuSAMM.AllocCount);
	printf("\tAllocStep : %d\t\t\t\t=> 16\n", stuSAMM.AllocStep);
	
	printf("\nSAMM values : \n");
	printf("\tidx\tptr\t\t\tval\t\tnum\t\texpected val\n");
	int t1val[10] = { 1001, 2002, 3003, 4004, 5005, 6006, 7007, 8008, 9009, 10010 };
	for ( int i = 1; i <= stuSAMM.Count; i++ ) {
		SAMM_Test_Object objStu = SAMM_GetPtr_Unsafe(&stuSAMM, i);
		if ( objStu->val == t1val[i - 1] ) {
			printf("\t%d\t%p\t%d\t\t%f\t%d\t\tsucc! √\n", i, objStu, objStu->val, objStu->num, t1val[i - 1]);
		} else {
			printf("\t%d\t%p\t%d\t\t%f\t%d\t\tfail! ×\n", i, objStu, objStu->val, objStu->num, t1val[i - 1]);
		}
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 5 : insert item
	printf("SAMM test subject 5 : insert item\n\n");
	for ( int i = 0; i < 10; i++ ) {
		unsigned int idx = SAMM_Insert(&stuSAMM, i * 2, 1);
		SAMM_Test_Object objStu = SAMM_GetPtr_Unsafe(&stuSAMM, idx);
		if ( objStu ) {
			idx = i + 1;
			objStu->val = idx * 10000 + idx;
			objStu->num = idx * 10000 + idx / 10000.0;
			printf("SAMM_Insert idx : %d\t( %d, \t%f\t)\tpass! √\n", idx, objStu->val, objStu->num);
		} else {
			printf("SAMM_Insert idx : %d\t\t\t\t\t\tfail! ×\n", idx);
		}
	}
	
	printf("\nSAMM state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", stuSAMM.ItemLength, sizeof(SAMM_Test_Struct));
	printf("\tCount : %d\t\t\t\t=> 20\n", stuSAMM.Count);
	printf("\tAllocCount : %d\t\t\t\t=> 20 ( 4 + 16 )\n", stuSAMM.AllocCount);
	printf("\tAllocStep : %d\t\t\t\t=> 16\n", stuSAMM.AllocStep);
	
	printf("\nSAMM values : \n");
	printf("\tidx\tptr\t\t\tval\t\tnum\t\texpected val\n");
	int t2val[20] = { 10001, 1001, 20002, 2002, 30003, 3003, 40004, 4004, 50005, 5005, 60006, 6006, 70007, 7007, 80008, 8008, 90009, 9009, 100010, 10010 };
	for ( int i = 1; i <= stuSAMM.Count; i++ ) {
		SAMM_Test_Object objStu = SAMM_GetPtr_Unsafe(&stuSAMM, i);
		if ( objStu->val == t2val[i - 1] ) {
			printf("\t%d\t%p\t%d\t\t%f\t%d\t\tsucc! √\n", i, objStu, objStu->val, objStu->num, t2val[i - 1]);
		} else {
			printf("\t%d\t%p\t%d\t\t%f\t%d\t\tfail! ×\n", i, objStu, objStu->val, objStu->num, t2val[i - 1]);
		}
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 6 : remove item
	printf("SAMM test subject 6 : remove item\n\n");
	if ( SAMM_Remove(&stuSAMM, 17, 1) ) {
		printf("SAMM_Remove idx : 17 (90009)\t\t\t\t\tpass! √\n");
	} else {
		printf("SAMM_Remove idx : 17 (90009)\t\t\t\t\tfail! ×\n");
	}
	if ( SAMM_Remove(&stuSAMM, 14, 1) ) {
		printf("SAMM_Remove idx : 14 (7007)\t\t\t\t\tpass! √\n");
	} else {
		printf("SAMM_Remove idx : 14 (7007)\t\t\t\t\tfail! ×\n");
	}
	if ( SAMM_Remove(&stuSAMM, 11, 1) ) {
		printf("SAMM_Remove idx : 11 (60006)\t\t\t\t\tpass! √\n");
	} else {
		printf("SAMM_Remove idx : 11 (60006)\t\t\t\t\tfail! ×\n");
	}
	if ( SAMM_Remove(&stuSAMM, 8, 1) ) {
		printf("SAMM_Remove idx : 8 (4004)\t\t\t\t\tpass! √\n");
	} else {
		printf("SAMM_Remove idx : 8 (4004)\t\t\t\t\tfail! ×\n");
	}
	if ( SAMM_Remove(&stuSAMM, 5, 1) ) {
		printf("SAMM_Remove idx : 5 (30003)\t\t\t\t\tpass! √\n");
	} else {
		printf("SAMM_Remove idx : 5 (30003)\t\t\t\t\tfail! ×\n");
	}
	for ( int i = 0; i < 10; i++ ) {
		unsigned int idx = SAMM_Append(&stuSAMM, 1);
		SAMM_Test_Object objStu = SAMM_GetPtr_Unsafe(&stuSAMM, idx);
		if ( objStu ) {
			idx = i + 1;
			objStu->val = idx * 100 + idx;
			objStu->num = idx * 100 + idx / 100.0;
			printf("SAMM_Append idx : %d\t(\t%d, \t%f\t)\tpass! √\n", idx, objStu->val, objStu->num);
		} else {
			printf("SAMM_Append idx : %d\t\t\t\t\t\tfail! ×\n", idx);
		}
	}
	
	printf("\nSAMM state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", stuSAMM.ItemLength, sizeof(SAMM_Test_Struct));
	printf("\tCount : %d\t\t\t\t=> 25\n", stuSAMM.Count);
	printf("\tAllocCount : %d\t\t\t\t=> 36 ( 4 + 16 + 16 )\n", stuSAMM.AllocCount);
	printf("\tAllocStep : %d\t\t\t\t=> 16\n", stuSAMM.AllocStep);
	
	printf("\nSAMM values : \n");
	printf("\tidx\tptr\t\t\tval\t\tnum\t\texpected val\n");
	int t3val[25] = { 10001, 1001, 20002, 2002, 3003, 40004, 50005, 5005, 6006, 70007, 80008, 8008, 9009, 100010, 10010, 101, 202, 303, 404, 505, 606, 707, 808, 909, 1010 };
	for ( int i = 1; i <= stuSAMM.Count; i++ ) {
		SAMM_Test_Object objStu = SAMM_GetPtr_Unsafe(&stuSAMM, i);
		if ( objStu->val == t3val[i - 1] ) {
			printf("\t%d\t%p\t%d\t\t%f\t%d\t\tsucc! √\n", i, objStu, objStu->val, objStu->num, t3val[i - 1]);
		} else {
			printf("\t%d\t%p\t%d\t\t%f\t%d\t\tfail! ×\n", i, objStu, objStu->val, objStu->num, t3val[i - 1]);
		}
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 7 : swap item
	printf("SAMM test subject 7 : swap item\n\n");
	if ( SAMM_Swap(&stuSAMM, 3, 13) ) {
		printf("SAMM_Swap 3 - 13 (20002 - 9009)\t\t\t\t\tpass! √\n");
	} else {
		printf("SAMM_Swap 3 - 13 (20002 - 9009)\t\t\t\t\tfail! ×\n");
	}
	if ( SAMM_Swap(&stuSAMM, 5, 15) ) {
		printf("SAMM_Swap 5 - 15 (3003 - 10010)\t\t\t\t\tpass! √\n");
	} else {
		printf("SAMM_Swap 5 - 15 (3003 - 10010)\t\t\t\t\tfail! ×\n");
	}
	if ( SAMM_Swap(&stuSAMM, 7, 17) ) {
		printf("SAMM_Swap 7 - 17 (50005 - 202)\t\t\t\t\tpass! √\n");
	} else {
		printf("SAMM_Swap 7 - 17 (50005 - 202)\t\t\t\t\tfail! ×\n");
	}
	
	printf("\nSAMM state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", stuSAMM.ItemLength, sizeof(SAMM_Test_Struct));
	printf("\tCount : %d\t\t\t\t=> 25\n", stuSAMM.Count);
	printf("\tAllocCount : %d\t\t\t\t=> 36 ( 4 + 16 + 16 )\n", stuSAMM.AllocCount);
	printf("\tAllocStep : %d\t\t\t\t=> 16\n", stuSAMM.AllocStep);
	
	printf("\nSAMM values : \n");
	printf("\tidx\tptr\t\t\tval\t\tnum\t\texpected val\n");
	int t4val[25] = { 10001, 1001, 9009, 2002, 10010, 40004, 202, 5005, 6006, 70007, 80008, 8008, 20002, 100010, 3003, 101, 50005, 303, 404, 505, 606, 707, 808, 909, 1010 };
	for ( int i = 1; i <= stuSAMM.Count; i++ ) {
		SAMM_Test_Object objStu = SAMM_GetPtr_Unsafe(&stuSAMM, i);
		if ( objStu->val == t4val[i - 1] ) {
			printf("\t%d\t%p\t%d\t\t%f\t%d\t\tsucc! √\n", i, objStu, objStu->val, objStu->num, t4val[i - 1]);
		} else {
			printf("\t%d\t%p\t%d\t\t%f\t%d\t\tfail! ×\n", i, objStu, objStu->val, objStu->num, t4val[i - 1]);
		}
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 8 : sort
	printf("SAMM test subject 8 : sort\n\n");
	SAMM_Sort(&stuSAMM, SAMM_TestSortProc);
	
	printf("SAMM state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", stuSAMM.ItemLength, sizeof(SAMM_Test_Struct));
	printf("\tCount : %d\t\t\t\t=> 25\n", stuSAMM.Count);
	printf("\tAllocCount : %d\t\t\t\t=> 36 ( 4 + 16 + 16 )\n", stuSAMM.AllocCount);
	printf("\tAllocStep : %d\t\t\t\t=> 16\n", stuSAMM.AllocStep);
	
	printf("\nSAMM values : \n");
	printf("\tidx\tptr\t\t\tval\t\tnum\t\texpected val\n");
	int t5val[25] = { 101, 202, 303, 404, 505, 606, 707, 808, 909, 1001, 1010, 2002, 3003, 5005, 6006, 8008, 9009, 10001, 10010, 20002, 40004, 50005, 70007, 80008, 100010 };
	for ( int i = 1; i <= stuSAMM.Count; i++ ) {
		SAMM_Test_Object objStu = SAMM_GetPtr_Unsafe(&stuSAMM, i);
		if ( objStu->val == t5val[i - 1] ) {
			printf("\t%d\t%p\t%d\t\t%f\t%d\t\tsucc! √\n", i, objStu, objStu->val, objStu->num, t5val[i - 1]);
		} else {
			printf("\t%d\t%p\t%d\t\t%f\t%d\t\tfail! ×\n", i, objStu, objStu->val, objStu->num, t5val[i - 1]);
		}
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject X : destroy & struct unit
	printf("SAMM test subject X : destroy & struct unit\n\n");
	SAMM_Destroy(objSAMM);
	printf("SAMM object (%p) already destroyed!\n\n", objSAMM);
	
	SAMM_Unit(&stuSAMM);
	printf("SAMM state (struct) : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", stuSAMM.ItemLength, sizeof(SAMM_Test_Struct));
	printf("\tCount : %d\t\t\t\t=> 0\n", stuSAMM.Count);
	printf("\tAllocCount : %d\t\t\t\t=> 0\n", stuSAMM.AllocCount);
	printf("\tAllocStep : %d\t\t\t\t=> 16\n", stuSAMM.AllocStep);
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	MMU_Unit();
}


