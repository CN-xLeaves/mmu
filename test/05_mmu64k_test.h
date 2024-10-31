


// test MMU64K struct
typedef struct {
	int iVal;
	char sVal[32];
} MMU64K_Test_Struct, *MMU64K_Test_Object;



void test_mmu64k()
{
	system("cls");
	MMU_Init();
	
	
	
	// subject 1 : create object
	printf("MMU64K test subject 1 : create object\n\n");
	MMU64K_Object objMMU = MMU64K_Create(sizeof(MMU64K_Test_Struct));
	if ( objMMU ) {
		objMMU->Flag = 0x10000;
		printf("MMU64K object : %p\t\t\t\tpass! √\n", objMMU);
		if ( objMMU->Memory ) {
			printf("\tMemory : %p\t\t\t\tpass! √\n", objMMU->Memory);
		} else {
			printf("\tMemory : %p\t\t\t\tfail! ×\n", objMMU->Memory);
		}
		printf("\tItemLength : %d\t\t\t\t=> %d\n", objMMU->ItemLength, sizeof(MMU64K_Test_Struct) + sizeof(MMU_Value));
		printf("\tCount : %d\t\t\t\t=> 0\n", objMMU->Count);
		printf("\tFreeCount : %d\t\t\t\t=> 0\n", objMMU->FreeCount);
		printf("\tFreeOffset : %d\t\t\t\t=> 0\n", objMMU->FreeOffset);
		printf("\tFlag : %x\t\t\t\t=> 10000\n", objMMU->Flag);
	} else {
		printf("MMU64K object : %p\t\t\t\t\tfail! ×\n", objMMU);
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 2 : alloc struct memory
	printf("MMU64K test subject 2 : alloc struct memory\n\n");
	MMU64K_Test_Struct* ptr[20];
	for ( int i = 0; i < 10; i++ ) {
		ptr[i] = MMU64K_Alloc(objMMU);
		if ( ptr[i] ) {
			ptr[i]->iVal = i * 10;
			sprintf(ptr[i]->sVal, "String Field idx = %d", i);
			printf("MMU64K_Alloc ptr : %p\t\t\t\tpass! √\n", ptr[i]);
		} else {
			printf("MMU64K_Alloc ptr : %p\t\t\t\tfail! ×\n", ptr[i]);
		}
	}
	
	printf("\nMMU64K state : \n");
	if ( objMMU->Memory ) {
		printf("\tMemory : %p\t\t\t\tpass! √\n", objMMU->Memory);
	} else {
		printf("\tMemory : %p\t\t\t\tfail! ×\n", objMMU->Memory);
	}
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objMMU->ItemLength, sizeof(MMU64K_Test_Struct) + sizeof(MMU_Value));
	printf("\tCount : %d\t\t\t\t=> 10\n", objMMU->Count);
	printf("\tFreeCount : %d\t\t\t\t=> 0\n", objMMU->FreeCount);
	printf("\tFreeOffset : %d\t\t\t\t=> 0\n", objMMU->FreeOffset);
	printf("\tFlag : %x\t\t\t\t=> 10000\n", objMMU->Flag);
	
	printf("\nMMU64K values : \n");
	printf("\tidx\tptr\t\t\tflag\tiVal\tsVal\t\t\texpected val\n");
	int t1val[10] = { 0, 10, 20, 30, 40, 50, 60, 70, 80, 90 };
	for ( int i = 0; i < 10; i++ ) {
		MMU_ValuePtr mvp = (void*)(ptr[i]) - 4;
		if ( ptr[i]->iVal == t1val[i] ) {
			printf("\t%d\t%p\t%x\t%d\t%s\t%d\t\tsucc! √\n", i, ptr[i], mvp->ItemFlag, ptr[i]->iVal, ptr[i]->sVal, t1val[i]);
		} else {
			printf("\t%d\t%p\t%x\t%d\t%s\t%d\t\tfail! ×\n", i, ptr[i], mvp->ItemFlag, ptr[i]->iVal, ptr[i]->sVal, t1val[i]);
		}
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 3 : free & alloc struct memory
	printf("MMU64K test subject 3 : free & alloc struct memory\n\n");
	MMU64K_Free(objMMU, ptr[3]);
	MMU64K_Free(objMMU, ptr[5]);
	MMU64K_Free(objMMU, ptr[7]);
	printf("free struct memory : %p\n", ptr[3]);
	printf("free struct memory : %p\n", ptr[5]);
	printf("free struct memory : %p\n", ptr[7]);
	for ( int i = 0; i < 10; i++ ) {
		ptr[i + 10] = MMU64K_Alloc(objMMU);
		if ( ptr[i + 10] ) {
			ptr[i + 10]->iVal = i + 100;
			sprintf(ptr[i + 10]->sVal, "String Field NewID %d", i);
			printf("MMU64K_Alloc ptr : %p\t\t\t\tpass! √\n", ptr[i]);
		} else {
			printf("MMU64K_Alloc ptr : %p\t\t\t\tfail! ×\n", ptr[i]);
		}
	}
	
	printf("\nMMU64K state : \n");
	if ( objMMU->Memory ) {
		printf("\tMemory : %p\t\t\t\tpass! √\n", objMMU->Memory);
	} else {
		printf("\tMemory : %p\t\t\t\tfail! ×\n", objMMU->Memory);
	}
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objMMU->ItemLength, sizeof(MMU64K_Test_Struct) + sizeof(MMU_Value));
	printf("\tCount : %d\t\t\t\t=> 17\n", objMMU->Count);
	printf("\tFreeCount : %d\t\t\t\t=> 0\n", objMMU->FreeCount);
	printf("\tFreeOffset : %d\t\t\t\t=> 3\n", objMMU->FreeOffset);
	printf("\tFlag : %x\t\t\t\t=> 10000\n", objMMU->Flag);
	
	printf("\nMMU64K values : \n");
	printf("\tidx\tptr\t\t\tflag\tiVal\tsVal\t\t\texpected val\n");
	int t2val[20] = { 0, 10, 20, 100, 40, 101, 60, 102, 80, 90, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109 };
	for ( int i = 0; i < 20; i++ ) {
		MMU_ValuePtr mvp = (void*)(ptr[i]) - 4;
		if ( ptr[i]->iVal == t2val[i] ) {
			printf("\t%d\t%p\t%x\t%d\t%s\t%d\t\tsucc! √\n", i, ptr[i], mvp->ItemFlag, ptr[i]->iVal, ptr[i]->sVal, t2val[i]);
		} else {
			printf("\t%d\t%p\t%x\t%d\t%s\t%d\t\tfail! ×\n", i, ptr[i], mvp->ItemFlag, ptr[i]->iVal, ptr[i]->sVal, t2val[i]);
		}
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 4 : free struct memory
	printf("MMU64K test subject 4 : free struct memory\n\n");
	MMU64K_Free(objMMU, ptr[14]);
	MMU64K_Free(objMMU, ptr[16]);
	MMU64K_Free(objMMU, ptr[18]);
	printf("free struct memory : %p\n", ptr[14]);
	printf("free struct memory : %p\n", ptr[16]);
	printf("free struct memory : %p\n", ptr[18]);
	
	printf("\nMMU64K state : \n");
	if ( objMMU->Memory ) {
		printf("\tMemory : %p\t\t\t\tpass! √\n", objMMU->Memory);
	} else {
		printf("\tMemory : %p\t\t\t\tfail! ×\n", objMMU->Memory);
	}
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objMMU->ItemLength, sizeof(MMU64K_Test_Struct) + sizeof(MMU_Value));
	printf("\tCount : %d\t\t\t\t=> 14\n", objMMU->Count);
	printf("\tFreeCount : %d\t\t\t\t=> 3\n", objMMU->FreeCount);
	printf("\tFreeOffset : %d\t\t\t\t=> 3\n", objMMU->FreeOffset);
	printf("\tFlag : %x\t\t\t\t=> 10000\n", objMMU->Flag);
	
	printf("\nMMU64K values : \n");
	printf("\tidx\tptr\t\t\tflag\tiVal\tsVal\t\t\texpected val\n");
	for ( int i = 0; i < 20; i++ ) {
		MMU_ValuePtr mvp = (void*)(ptr[i]) - 4;
		if ( ptr[i]->iVal == t2val[i] ) {
			printf("\t%d\t%p\t%x\t%d\t%s\t%d\t\tsucc! √\n", i, ptr[i], mvp->ItemFlag, ptr[i]->iVal, ptr[i]->sVal, t2val[i]);
		} else {
			printf("\t%d\t%p\t%x\t%d\t%s\t%d\t\tfail! ×\n", i, ptr[i], mvp->ItemFlag, ptr[i]->iVal, ptr[i]->sVal, t2val[i]);
		}
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 5 : alloc all (max 65536)
	printf("MMU64K test subject 5 : alloc all (max 65536)\n\n");
	for ( int i = 0; i < 65522; i++ ) {
		MMU64K_Test_Object mto = MMU64K_Alloc(objMMU);
		if ( mto ) {
			mto->iVal = -i;
			sprintf(mto->sVal, "test idx : %d", i);
		} else {
			printf("MMU64K_Alloc Fail (Should be succ) pos : %d\t\t\tfail! ×\n", i);
		}
	}
	void* failptr = MMU64K_Alloc(objMMU);
	if ( failptr ) {
		printf("MMU64K_Alloc Succ (Should be fail)\t\t\t\tfail! ×\n");
	}
	
	printf("\nMMU64K state : \n");
	if ( objMMU->Memory ) {
		printf("\tMemory : %p\t\t\t\tpass! √\n", objMMU->Memory);
	} else {
		printf("\tMemory : %p\t\t\t\tfail! ×\n", objMMU->Memory);
	}
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objMMU->ItemLength, sizeof(MMU64K_Test_Struct) + sizeof(MMU_Value));
	printf("\tCount : %d\t\t\t\t=> 65536\n", objMMU->Count);
	printf("\tFreeCount : %d\t\t\t\t=> 0\n", objMMU->FreeCount);
	printf("\tFreeOffset : %d\t\t\t\t=> 6\n", objMMU->FreeOffset);
	printf("\tFlag : %x\t\t\t\t=> 10000\n", objMMU->Flag);
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// not require testing
	MMU64K_Destroy(objMMU);
	
	
	
	MMU_Unit();
}


