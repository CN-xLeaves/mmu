


// test MM64K struct
typedef struct {
	int iVal;
	char sVal[32];
} MM64K_Test_Struct, *MM64K_Test_Object;



void mm64k_error_proc(MM64K_Object objMM, int ErrorID)
{
	printf("!!! Error !!! error code : %d\n", ErrorID);
}



void test_mm64k()
{
	system("cls");
	MMU_Init();
	
	
	
	// subject 1 : create object
	printf("MM64K test subject 1 : create object\n\n");
	MM64K_Object objMM = MM64K_Create(sizeof(MM64K_Test_Struct));
	if ( objMM ) {
		objMM->OnError = (void*)mm64k_error_proc;
		printf("MM64K object : %p\t\t\t\tpass! √\n", objMM);
		printf("\tItemLength : %d\t\t\t\t=> %d\n", objMM->ItemLength, sizeof(MM64K_Test_Struct));
		printf("\tMMU.Count : %d\t\t\t\t=> 0\n", objMM->MMU.Count);
		printf("\tMMU.AllocCount : %d\t\t\t=> 0\n", objMM->MMU.AllocCount);
		if ( objMM->MMU.Memory ) {
			printf("\tMMU.Memory : %p\t\t\tfail! ×\n", objMM->MMU.Memory);
		} else {
			printf("\tMMU.Memory : %p\t\t\tpass! √\n", objMM->MMU.Memory);
		}
	} else {
		printf("MM64K object : %p\t\t\t\t\tfail! ×\n", objMM);
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 2 : alloc struct memory
	printf("MM64K test subject 2 : alloc struct memory\n\n");
	MM64K_Test_Struct* ptr[20];
	for ( int i = 0; i < 10; i++ ) {
		ptr[i] = MM64K_Alloc(objMM);
		if ( ptr[i] ) {
			ptr[i]->iVal = i * 10;
			sprintf(ptr[i]->sVal, "String Field idx = %d", i);
			printf("MM64K_Alloc ptr : %p\t\t\t\tpass! √\n", ptr[i]);
		} else {
			printf("MM64K_Alloc ptr : %p\t\t\t\tfail! ×\n", ptr[i]);
		}
	}
	
	printf("\nMM64K state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objMM->ItemLength, sizeof(MM64K_Test_Struct));
	printf("\tMMU.Count : %d\t\t\t\t=> 1\n", objMM->MMU.Count);
	printf("\tMMU.AllocCount : %d\t\t\t=> 64\n", objMM->MMU.AllocCount);
	if ( objMM->MMU.Memory ) {
		printf("\tMemory : %p\t\t\t\tpass! √\n", objMM->MMU.Memory);
	} else {
		printf("\tMemory : %p\t\t\t\tfail! ×\n", objMM->MMU.Memory);
	}
	
	printf("\nMM64K values : \n");
	printf("\tidx\tptr\t\t\tflag\t\tiVal\tsVal\t\t\texpected val\n");
	int t1val[10] = { 0, 10, 20, 30, 40, 50, 60, 70, 80, 90 };
	for ( int i = 0; i < 10; i++ ) {
		MMU_ValuePtr mvp = (void*)(ptr[i]) - 4;
		if ( ptr[i]->iVal == t1val[i] ) {
			printf("\t%d\t%p\t% 8x\t%d\t%s\t%d\t\tsucc! √\n", i, ptr[i], mvp->ItemFlag, ptr[i]->iVal, ptr[i]->sVal, t1val[i]);
		} else {
			printf("\t%d\t%p\t% 8x\t%d\t%s\t%d\t\tfail! ×\n", i, ptr[i], mvp->ItemFlag, ptr[i]->iVal, ptr[i]->sVal, t1val[i]);
		}
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 3 : free & alloc struct memory
	printf("MM64K test subject 3 : free & alloc struct memory\n\n");
	MM64K_Free(objMM, ptr[3]);
	MM64K_Free(objMM, ptr[5]);
	MM64K_Free(objMM, ptr[7]);
	printf("free struct memory : %p\n", ptr[3]);
	printf("free struct memory : %p\n", ptr[5]);
	printf("free struct memory : %p\n", ptr[7]);
	for ( int i = 0; i < 10; i++ ) {
		ptr[i + 10] = MM64K_Alloc(objMM);
		if ( ptr[i + 10] ) {
			ptr[i + 10]->iVal = i + 100;
			sprintf(ptr[i + 10]->sVal, "String Field NewID %d", i);
			printf("MM64K_Alloc ptr : %p\t\t\t\tpass! √\n", ptr[i]);
		} else {
			printf("MM64K_Alloc ptr : %p\t\t\t\tfail! ×\n", ptr[i]);
		}
	}
	
	printf("\nMM64K state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objMM->ItemLength, sizeof(MM64K_Test_Struct));
	printf("\tMMU.Count : %d\t\t\t\t=> 1\n", objMM->MMU.Count);
	printf("\tMMU.AllocCount : %d\t\t\t=> 64\n", objMM->MMU.AllocCount);
	if ( objMM->MMU.Memory ) {
		printf("\tMemory : %p\t\t\t\tpass! √\n", objMM->MMU.Memory);
	} else {
		printf("\tMemory : %p\t\t\t\tfail! ×\n", objMM->MMU.Memory);
	}
	
	printf("\nMM64K values : \n");
	printf("\tidx\tptr\t\t\tflag\t\tiVal\tsVal\t\t\texpected val\n");
	int t2val[20] = { 0, 10, 20, 100, 40, 101, 60, 102, 80, 90, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109 };
	for ( int i = 0; i < 20; i++ ) {
		MMU_ValuePtr mvp = (void*)(ptr[i]) - 4;
		if ( ptr[i]->iVal == t2val[i] ) {
			printf("\t%d\t%p\t% 8x\t%d\t%s\t%d\t\tsucc! √\n", i, ptr[i], mvp->ItemFlag, ptr[i]->iVal, ptr[i]->sVal, t2val[i]);
		} else {
			printf("\t%d\t%p\t% 8x\t%d\t%s\t%d\t\tfail! ×\n", i, ptr[i], mvp->ItemFlag, ptr[i]->iVal, ptr[i]->sVal, t2val[i]);
		}
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 4 : free struct memory
	printf("MM64K test subject 4 : free struct memory\n\n");
	MM64K_Free(objMM, ptr[14]);
	MM64K_Free(objMM, ptr[16]);
	MM64K_Free(objMM, ptr[18]);
	printf("free struct memory : %p\n", ptr[14]);
	printf("free struct memory : %p\n", ptr[16]);
	printf("free struct memory : %p\n", ptr[18]);
	
	printf("\nMM64K state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objMM->ItemLength, sizeof(MM64K_Test_Struct));
	printf("\tMMU.Count : %d\t\t\t\t=> 1\n", objMM->MMU.Count);
	printf("\tMMU.AllocCount : %d\t\t\t=> 64\n", objMM->MMU.AllocCount);
	if ( objMM->MMU.Memory ) {
		printf("\tMemory : %p\t\t\t\tpass! √\n", objMM->MMU.Memory);
	} else {
		printf("\tMemory : %p\t\t\t\tfail! ×\n", objMM->MMU.Memory);
	}
	
	printf("\nMM64K values : \n");
	printf("\tidx\tptr\t\t\tflag\t\tiVal\tsVal\t\t\texpected val\n");
	for ( int i = 0; i < 20; i++ ) {
		MMU_ValuePtr mvp = (void*)(ptr[i]) - 4;
		if ( ptr[i]->iVal == t2val[i] ) {
			printf("\t%d\t%p\t% 8x\t%d\t%s\t%d\t\tsucc! √\n", i, ptr[i], mvp->ItemFlag, ptr[i]->iVal, ptr[i]->sVal, t2val[i]);
		} else {
			printf("\t%d\t%p\t% 8x\t%d\t%s\t%d\t\tfail! ×\n", i, ptr[i], mvp->ItemFlag, ptr[i]->iVal, ptr[i]->sVal, t2val[i]);
		}
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 5 : alloc all (max 65536)
	printf("MM64K test subject 5 : alloc all (max 65536)\n\n");
	for ( int i = 0; i < 131058; i++ ) {
		MM64K_Test_Object mto = MM64K_Alloc(objMM);
		if ( mto ) {
			mto->iVal = -i;
			sprintf(mto->sVal, "test alloc idx : %d", i);
		} else {
			printf("MM64K_Alloc Fail (Should be succ) pos : %d\t\t\tfail! ×\n", i);
		}
	}
	void* failptr = MM64K_Alloc(objMM);
	if ( failptr == NULL ) {
		printf("MM64K_Alloc Fail (Should be succ)\t\t\t\tfail! ×\n");
	}
	
	printf("\nMM64K state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objMM->ItemLength, sizeof(MM64K_Test_Struct));
	printf("\tMMU.Count : %d\t\t\t\t=> 3\n", objMM->MMU.Count);
	printf("\tMMU.AllocCount : %d\t\t\t=> 64\n", objMM->MMU.AllocCount);
	if ( objMM->MMU.Memory ) {
		printf("\tMemory : %p\t\t\t\tpass! √\n", objMM->MMU.Memory);
	} else {
		printf("\tMemory : %p\t\t\t\tfail! ×\n", objMM->MMU.Memory);
	}
	
	printf("\nMMU64K list : \n");
	for ( int i = 0; i < objMM->MMU.Count; i++ ) {
		printf("\t%p\n", objMM->MMU.Memory[i]);
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 6 : free (max 65536)
	printf("MM64K test subject 6 : free (max 65536)\n\n");
	MM64K_Free(objMM, failptr);
	
	printf("\nMM64K state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objMM->ItemLength, sizeof(MM64K_Test_Struct));
	printf("\tMMU.Count : %d\t\t\t\t=> 3\n", objMM->MMU.Count);
	printf("\tMMU.AllocCount : %d\t\t\t=> 64\n", objMM->MMU.AllocCount);
	if ( objMM->MMU.Memory ) {
		printf("\tMemory : %p\t\t\t\tpass! √\n", objMM->MMU.Memory);
	} else {
		printf("\tMemory : %p\t\t\t\tfail! ×\n", objMM->MMU.Memory);
	}
	
	printf("\nMMU64K list : \n");
	for ( int i = 0; i < objMM->MMU.Count; i++ ) {
		printf("\t%p\n", objMM->MMU.Memory[i]);
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 7 : garbage collection (marked free)
	printf("MM64K test subject 7 : garbage collection (marked free)\n\n");
	MM64K_GC_Mark(ptr[2]);
	MM64K_GC_Mark(ptr[4]);
	MM64K_GC_Mark(ptr[6]);
	MM64K_GC(objMM, TRUE);
	printf("garbage collection mark : ptr[2]、ptr[4]、ptr[6]\n");
	
	printf("\nMM64K state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objMM->ItemLength, sizeof(MM64K_Test_Struct));
	printf("\tMMU.Count : %d\t\t\t\t=> 3\n", objMM->MMU.Count);
	printf("\tMMU.AllocCount : %d\t\t\t=> 64\n", objMM->MMU.AllocCount);
	if ( objMM->MMU.Memory ) {
		printf("\tMemory : %p\t\t\t\tpass! √\n", objMM->MMU.Memory);
	} else {
		printf("\tMemory : %p\t\t\t\tfail! ×\n", objMM->MMU.Memory);
	}
	
	printf("\nMM64K values : \n");
	printf("\tidx\tptr\t\t\tflag\t\tiVal\tsVal\n");
	for ( int i = 0; i < 20; i++ ) {
		MMU_ValuePtr mvp = (void*)(ptr[i]) - 4;
		printf("\t%d\t%p\t% 8x\t%d\t%s\n", i, ptr[i], mvp->ItemFlag, ptr[i]->iVal, ptr[i]->sVal);
	}
	
	printf("\nMMU64K list : \n");
	for ( int i = 0; i < objMM->MMU.Count; i++ ) {
		printf("\t%p\n", objMM->MMU.Memory[i]);
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 8 : garbage collection (not marked free)
	printf("MM64K test subject 8 : garbage collection (not marked free)\n\n");
	MM64K_GC_Mark(ptr[13]);
	MM64K_GC_Mark(ptr[15]);
	MM64K_GC_Mark(ptr[17]);
	MM64K_GC(objMM, FALSE);
	printf("garbage collection mark : ptr[13]、ptr[15]、ptr[17]\n");
	
	printf("\nMM64K state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objMM->ItemLength, sizeof(MM64K_Test_Struct));
	printf("\tMMU.Count : %d\t\t\t\t=> 3\n", objMM->MMU.Count);
	printf("\tMMU.AllocCount : %d\t\t\t=> 64\n", objMM->MMU.AllocCount);
	if ( objMM->MMU.Memory ) {
		printf("\tMemory : %p\t\t\t\tpass! √\n", objMM->MMU.Memory);
	} else {
		printf("\tMemory : %p\t\t\t\tfail! ×\n", objMM->MMU.Memory);
	}
	
	printf("\nMM64K values : \n");
	printf("\tidx\tptr\t\t\tflag\t\tiVal\tsVal\n");
	for ( int i = 0; i < 20; i++ ) {
		MMU_ValuePtr mvp = (void*)(ptr[i]) - 4;
		printf("\t%d\t%p\t% 8x\t%d\t%s\n", i, ptr[i], mvp->ItemFlag, ptr[i]->iVal, ptr[i]->sVal);
	}
	
	printf("\nMMU64K list : \n");
	for ( int i = 0; i < objMM->MMU.Count; i++ ) {
		printf("\t%p\n", objMM->MMU.Memory[i]);
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 9 : struct unit & destroy
	printf("MM64K test subject 9 : struct unit & destroy\n\n");
	MM64K_Unit(objMM);
	printf("MM64K object (%p) already unit!\n\n", objMM);
	
	printf("\nMM64K state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objMM->ItemLength, sizeof(MM64K_Test_Struct));
	printf("\tMMU.Count : %d\t\t\t\t=> 0\n", objMM->MMU.Count);
	printf("\tMMU.AllocCount : %d\t\t\t=> 0\n", objMM->MMU.AllocCount);
	if ( objMM->MMU.Memory ) {
		printf("\tMemory : %p\t\t\t\tfail! ×\n", objMM->MMU.Memory);
	} else {
		printf("\tMemory : %p\t\t\t\tpass! √\n", objMM->MMU.Memory);
	}
	
	MM64K_Destroy(objMM);
	printf("\nMM64K object (%p) already destroyed!\n", objMM);
	
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	MMU_Unit();
}


