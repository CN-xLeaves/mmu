


// test SMMU struct
typedef struct {
	int val;
	double num;
} SMMU_Test_Struct, *SMMU_Test_Object;



// SMMU sort proc
int SMMU_TestSortProc(SMMU_Test_Object v1, SMMU_Test_Object v2)
{
	return v1->val - v2->val;
}



void test_smmu()
{
	system("cls");
	MMU_Init();
	
	
	
	// subject 1 : create object
	printf("SMMU test subject 1 : create object\n\n");
	SMMU_Object objSMMU = SMMU_Create(sizeof(SMMU_Test_Struct), 16);
	if ( objSMMU ) {
		printf("SMMU object : %p\t\t\t\t\tpass! √\n", objSMMU);
		printf("\tMemory : %p\t\t=> 0\n", objSMMU->Memory);
		printf("\tItemLength : %d\t\t\t\t=> %d\n", objSMMU->ItemLength, sizeof(SMMU_Test_Struct));
		printf("\tCount : %d\t\t\t\t=> 0\n", objSMMU->Count);
		printf("\tAllocCount : %d\t\t\t\t=> 0\n", objSMMU->AllocCount);
		printf("\tAllocStep : %d\t\t\t\t=> 16\n", objSMMU->AllocStep);
	} else {
		printf("SMMU object : %p\t\t\t\t\tfail! ×\n", objSMMU);
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 2 : struct init
	printf("SMMU test subject 2 : struct init (Please check print)\n\n");
	SMMU_Struct stuSMMU;
	SMMU_Init(&stuSMMU, sizeof(SMMU_Test_Struct), 16);
	printf("\tMemory : %p\t\t=> 0\n", stuSMMU.Memory);
	printf("\tItemLength : %d\t\t\t\t=> %d\n", stuSMMU.ItemLength, sizeof(SMMU_Test_Struct));
	printf("\tCount : %d\t\t\t\t=> 0\n", stuSMMU.Count);
	printf("\tAllocCount : %d\t\t\t\t=> 0\n", stuSMMU.AllocCount);
	printf("\tAllocStep : %d\t\t\t\t=> 16\n", stuSMMU.AllocStep);
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 3 : malloc
	printf("SMMU test subject 3 : malloc\n\n");
	int bRet = SMMU_Malloc(&stuSMMU, 4);
	printf("SMMU_Malloc return value : %d\t\t\t=> -1\n", bRet);
	if ( stuSMMU.Memory ) {
		printf("SMMU memory ptr : %p\t\t\t\tpass! √\n", stuSMMU.Memory);
		printf("\tItemLength : %d\t\t\t\t=> %d\n", stuSMMU.ItemLength, sizeof(SMMU_Test_Struct));
		printf("\tCount : %d\t\t\t\t=> 0\n", stuSMMU.Count);
		printf("\tAllocCount : %d\t\t\t\t=> 4\n", stuSMMU.AllocCount);
		printf("\tAllocStep : %d\t\t\t\t=> 16\n", stuSMMU.AllocStep);
	} else {
		printf("SMMU memory ptr : %p\t\t\t\tfail! ×\n", stuSMMU.Memory);
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 4 : append item
	printf("SMMU test subject 4 : append item\n\n");
	for ( int i = 0; i < 10; i++ ) {
		unsigned int idx = SMMU_Append(&stuSMMU, 1);
		SMMU_Test_Object objStu = SMMU_GetPtr_Unsafe(&stuSMMU, idx);
		if ( objStu ) {
			objStu->val = idx * 1000 + idx;
			objStu->num = idx * 1000 + idx / 1000.0;
			printf("SMMU_Append idx : %d\t( %d, \t%f\t)\tpass! √\n", idx, objStu->val, objStu->num);
		} else {
			printf("SMMU_Append idx : %d\t\t\t\t\t\tfail! ×\n", idx);
		}
	}
	
	printf("\nSMMU state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", stuSMMU.ItemLength, sizeof(SMMU_Test_Struct));
	printf("\tCount : %d\t\t\t\t=> 10\n", stuSMMU.Count);
	printf("\tAllocCount : %d\t\t\t\t=> 21 ( 4 + 16 + 1 )\n", stuSMMU.AllocCount);
	printf("\tAllocStep : %d\t\t\t\t=> 16\n", stuSMMU.AllocStep);
	
	printf("\nSMMU values : \n");
	printf("\tidx\tptr\t\t\tval\t\tnum\t\texpected val\n");
	int t1val[10] = { 1001, 2002, 3003, 4004, 5005, 6006, 7007, 8008, 9009, 10010 };
	for ( int i = 1; i <= stuSMMU.Count; i++ ) {
		SMMU_Test_Object objStu = SMMU_GetPtr_Unsafe(&stuSMMU, i);
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
	printf("SMMU test subject 5 : insert item\n\n");
	for ( int i = 0; i < 10; i++ ) {
		unsigned int idx = SMMU_Insert(&stuSMMU, i * 2, 1);
		SMMU_Test_Object objStu = SMMU_GetPtr_Unsafe(&stuSMMU, idx);
		if ( objStu ) {
			idx = i + 1;
			objStu->val = idx * 10000 + idx;
			objStu->num = idx * 10000 + idx / 10000.0;
			printf("SMMU_Insert idx : %d\t( %d, \t%f\t)\tpass! √\n", idx, objStu->val, objStu->num);
		} else {
			printf("SMMU_Insert idx : %d\t\t\t\t\t\tfail! ×\n", idx);
		}
	}
	
	printf("\nSMMU state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", stuSMMU.ItemLength, sizeof(SMMU_Test_Struct));
	printf("\tCount : %d\t\t\t\t=> 20\n", stuSMMU.Count);
	printf("\tAllocCount : %d\t\t\t\t=> 21 ( 4 + 16 + 1 )\n", stuSMMU.AllocCount);
	printf("\tAllocStep : %d\t\t\t\t=> 16\n", stuSMMU.AllocStep);
	
	printf("\nSMMU values : \n");
	printf("\tidx\tptr\t\t\tval\t\tnum\t\texpected val\n");
	int t2val[20] = { 10001, 1001, 20002, 2002, 30003, 3003, 40004, 4004, 50005, 5005, 60006, 6006, 70007, 7007, 80008, 8008, 90009, 9009, 100010, 10010 };
	for ( int i = 1; i <= stuSMMU.Count; i++ ) {
		SMMU_Test_Object objStu = SMMU_GetPtr_Unsafe(&stuSMMU, i);
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
	printf("SMMU test subject 6 : remove item\n\n");
	if ( SMMU_Remove(&stuSMMU, 17, 1) ) {
		printf("SMMU_Remove idx : 17 (90009)\t\t\t\t\tpass! √\n");
	} else {
		printf("SMMU_Remove idx : 17 (90009)\t\t\t\t\tfail! ×\n");
	}
	if ( SMMU_Remove(&stuSMMU, 14, 1) ) {
		printf("SMMU_Remove idx : 14 (7007)\t\t\t\t\tpass! √\n");
	} else {
		printf("SMMU_Remove idx : 14 (7007)\t\t\t\t\tfail! ×\n");
	}
	if ( SMMU_Remove(&stuSMMU, 11, 1) ) {
		printf("SMMU_Remove idx : 11 (60006)\t\t\t\t\tpass! √\n");
	} else {
		printf("SMMU_Remove idx : 11 (60006)\t\t\t\t\tfail! ×\n");
	}
	if ( SMMU_Remove(&stuSMMU, 8, 1) ) {
		printf("SMMU_Remove idx : 8 (4004)\t\t\t\t\tpass! √\n");
	} else {
		printf("SMMU_Remove idx : 8 (4004)\t\t\t\t\tfail! ×\n");
	}
	if ( SMMU_Remove(&stuSMMU, 5, 1) ) {
		printf("SMMU_Remove idx : 5 (30003)\t\t\t\t\tpass! √\n");
	} else {
		printf("SMMU_Remove idx : 5 (30003)\t\t\t\t\tfail! ×\n");
	}
	for ( int i = 0; i < 10; i++ ) {
		unsigned int idx = SMMU_Append(&stuSMMU, 1);
		SMMU_Test_Object objStu = SMMU_GetPtr_Unsafe(&stuSMMU, idx);
		if ( objStu ) {
			idx = i + 1;
			objStu->val = idx * 100 + idx;
			objStu->num = idx * 100 + idx / 100.0;
			printf("SMMU_Append idx : %d\t(\t%d, \t%f\t)\tpass! √\n", idx, objStu->val, objStu->num);
		} else {
			printf("SMMU_Append idx : %d\t\t\t\t\t\tfail! ×\n", idx);
		}
	}
	
	printf("\nSMMU state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", stuSMMU.ItemLength, sizeof(SMMU_Test_Struct));
	printf("\tCount : %d\t\t\t\t=> 25\n", stuSMMU.Count);
	printf("\tAllocCount : %d\t\t\t\t=> 38 ( 4 + 16 + 1 + 16 + 1 )\n", stuSMMU.AllocCount);
	printf("\tAllocStep : %d\t\t\t\t=> 16\n", stuSMMU.AllocStep);
	
	printf("\nSMMU values : \n");
	printf("\tidx\tptr\t\t\tval\t\tnum\t\texpected val\n");
	int t3val[25] = { 10001, 1001, 20002, 2002, 3003, 40004, 50005, 5005, 6006, 70007, 80008, 8008, 9009, 100010, 10010, 101, 202, 303, 404, 505, 606, 707, 808, 909, 1010 };
	for ( int i = 1; i <= stuSMMU.Count; i++ ) {
		SMMU_Test_Object objStu = SMMU_GetPtr_Unsafe(&stuSMMU, i);
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
	printf("SMMU test subject 7 : swap item\n\n");
	if ( SMMU_Swap(&stuSMMU, 3, 13) ) {
		printf("SMMU_Swap 3 - 13 (20002 - 9009)\t\t\t\t\tpass! √\n");
	} else {
		printf("SMMU_Swap 3 - 13 (20002 - 9009)\t\t\t\t\tfail! ×\n");
	}
	if ( SMMU_Swap(&stuSMMU, 5, 15) ) {
		printf("SMMU_Swap 5 - 15 (3003 - 10010)\t\t\t\t\tpass! √\n");
	} else {
		printf("SMMU_Swap 5 - 15 (3003 - 10010)\t\t\t\t\tfail! ×\n");
	}
	if ( SMMU_Swap(&stuSMMU, 7, 17) ) {
		printf("SMMU_Swap 7 - 17 (50005 - 202)\t\t\t\t\tpass! √\n");
	} else {
		printf("SMMU_Swap 7 - 17 (50005 - 202)\t\t\t\t\tfail! ×\n");
	}
	
	printf("\nSMMU state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", stuSMMU.ItemLength, sizeof(SMMU_Test_Struct));
	printf("\tCount : %d\t\t\t\t=> 25\n", stuSMMU.Count);
	printf("\tAllocCount : %d\t\t\t\t=> 38 ( 4 + 16 + 1 + 16 + 1 )\n", stuSMMU.AllocCount);
	printf("\tAllocStep : %d\t\t\t\t=> 16\n", stuSMMU.AllocStep);
	
	printf("\nSMMU values : \n");
	printf("\tidx\tptr\t\t\tval\t\tnum\t\texpected val\n");
	int t4val[25] = { 10001, 1001, 9009, 2002, 10010, 40004, 202, 5005, 6006, 70007, 80008, 8008, 20002, 100010, 3003, 101, 50005, 303, 404, 505, 606, 707, 808, 909, 1010 };
	for ( int i = 1; i <= stuSMMU.Count; i++ ) {
		SMMU_Test_Object objStu = SMMU_GetPtr_Unsafe(&stuSMMU, i);
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
	printf("SMMU test subject 8 : sort\n\n");
	SMMU_Sort(&stuSMMU, SMMU_TestSortProc);
	
	printf("SMMU state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", stuSMMU.ItemLength, sizeof(SMMU_Test_Struct));
	printf("\tCount : %d\t\t\t\t=> 25\n", stuSMMU.Count);
	printf("\tAllocCount : %d\t\t\t\t=> 38 ( 4 + 16 + 1 + 16 + 1 )\n", stuSMMU.AllocCount);
	printf("\tAllocStep : %d\t\t\t\t=> 16\n", stuSMMU.AllocStep);
	
	printf("\nSMMU values : \n");
	printf("\tidx\tptr\t\t\tval\t\tnum\t\texpected val\n");
	int t5val[25] = { 101, 202, 303, 404, 505, 606, 707, 808, 909, 1001, 1010, 2002, 3003, 5005, 6006, 8008, 9009, 10001, 10010, 20002, 40004, 50005, 70007, 80008, 100010 };
	for ( int i = 1; i <= stuSMMU.Count; i++ ) {
		SMMU_Test_Object objStu = SMMU_GetPtr_Unsafe(&stuSMMU, i);
		if ( objStu->val == t5val[i - 1] ) {
			printf("\t%d\t%p\t%d\t\t%f\t%d\t\tsucc! √\n", i, objStu, objStu->val, objStu->num, t5val[i - 1]);
		} else {
			printf("\t%d\t%p\t%d\t\t%f\t%d\t\tfail! ×\n", i, objStu, objStu->val, objStu->num, t5val[i - 1]);
		}
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 9 : destroy & struct unit
	printf("SMMU test subject 9 : destroy & struct unit\n\n");
	SMMU_Destroy(objSMMU);
	printf("SMMU object (%p) already destroyed!\n\n", objSMMU);
	
	SMMU_Unit(&stuSMMU);
	printf("SMMU state (struct) : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", stuSMMU.ItemLength, sizeof(SMMU_Test_Struct));
	printf("\tCount : %d\t\t\t\t=> 0\n", stuSMMU.Count);
	printf("\tAllocCount : %d\t\t\t\t=> 0\n", stuSMMU.AllocCount);
	printf("\tAllocStep : %d\t\t\t\t=> 16\n", stuSMMU.AllocStep);
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	MMU_Unit();
}


