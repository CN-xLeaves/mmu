


// test BSMM struct
typedef struct {
	int iVal;
} BSMM_Test_Struct, *BSMM_Test_Object;



void test_bsmm()
{
	system("cls");
	MMU_Init();
	
	
	
	// subject 1 : create object
	printf("BSMM test subject 1 : create object\n\n");
	BSMM_Object objBSMM = BSMM_Create(sizeof(BSMM_Test_Struct));
	if ( objBSMM ) {
		printf("BSMM object : %p\t\t\t\t\tpass! √\n", objBSMM);
		printf("\tItemLength : %d\t\t\t\t=> %d\n", objBSMM->ItemLength, sizeof(BSMM_Test_Struct));
		printf("\tCount : %d\t\t\t\t=> 0\n", objBSMM->Count);
		printf("\tPageMMU.Count : %d\t\t\t=> 0\n", objBSMM->PageMMU.Count);
		printf("\tPageMMU.AllocCount : %d\t\t\t=> 0\n", objBSMM->PageMMU.AllocCount);
		printf("\tPageMMU.AllocStep : %d\t\t\t=> 256\n", objBSMM->PageMMU.AllocStep);
		if ( objBSMM->PageMMU.Memory ) {
			printf("\tarrMMU.Memory : %p\t\t\tfail! ×\n", objBSMM->PageMMU.Memory);
		} else {
			printf("\tarrMMU.Memory : %p\t\t\tpass! √\n", objBSMM->PageMMU.Memory);
		}
		
		printf("\nBSMM LList : \n");
		if ( objBSMM->LL_Free ) {
			printf("\tLL_Free : %p\t\t\t\tfail! ×\n", objBSMM->LL_Free);
		} else {
			printf("\tLL_Free : null\n");
		}
	} else {
		printf("BSMM object : %p\t\t\t\t\tfail! ×\n", objBSMM);
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 2 : alloc struct memory
	printf("BSMM test subject 2 : alloc struct memory\n\n");
	BSMM_Test_Object ptr[10];
	for ( int i = 0; i < 10; i++ ) {
		ptr[i] = BSMM_Alloc(objBSMM);
		if ( ptr[i] ) {
			ptr[i]->iVal = i * 10;
			printf("BSMM_Alloc ptr (%p) = %d\t\t\t\tpass! √\n", ptr[i], ptr[i]->iVal);
		} else {
			printf("BSMM_Alloc ptr (%p) pos = %d\t\t\t\tfail! ×\n", ptr[i], i);
		}
	}
	
	printf("\nBSMM state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objBSMM->ItemLength, sizeof(BSMM_Test_Struct));
	printf("\tCount : %d\t\t\t\t=> 10\n", objBSMM->Count);
	printf("\tPageMMU.Count : %d\t\t\t=> 1\n", objBSMM->PageMMU.Count);
	printf("\tPageMMU.AllocCount : %d\t\t=> 256\n", objBSMM->PageMMU.AllocCount);
	printf("\tPageMMU.AllocStep : %d\t\t\t=> 256\n", objBSMM->PageMMU.AllocStep);
	if ( objBSMM->PageMMU.Memory ) {
		printf("\tarrMMU.Memory : %p\t\t\tpass! √\n", objBSMM->PageMMU.Memory);
	} else {
		printf("\tarrMMU.Memory : %p\t\t\tfail! ×\n", objBSMM->PageMMU.Memory);
	}
	
	printf("\nBSMM LList : \n");
	if ( objBSMM->LL_Free ) {
		printf("\tLL_Free :\n");
		MemPtr_LLNode* pNode = objBSMM->LL_Free;
		while ( pNode ) {
			printf("\t\t%p\t%p\t%p\n", pNode, pNode->Next, pNode->Ptr);
			pNode = pNode->Next;
		}
	} else {
		printf("\tLL_Free : null\n");
	}
	
	printf("\nBSMM values : \n");
	printf("\tidx\tptr\t\t\tiVal\texpected val\n");
	int t1val[10] = { 0, 10, 20, 30, 40, 50, 60, 70, 80, 90 };
	for ( int i = 0; i < 10; i++ ) {
		if ( ptr[i]->iVal == t1val[i] ) {
			printf("\t%d\t%p\t%d\t%d\t\tsucc! √\n", i, ptr[i], ptr[i]->iVal, t1val[i]);
		} else {
			printf("\t%d\t%p\t%d\t%d\t\tfail! ×\n", i, ptr[i], ptr[i]->iVal, t1val[i]);
		}
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 3 : free struct memory
	printf("BSMM test subject 3 : free struct memory\n\n");
	BSMM_Free(objBSMM, ptr[3]);
	BSMM_Free(objBSMM, ptr[5]);
	BSMM_Free(objBSMM, ptr[7]);
	BSMM_Free(objBSMM, ptr[9]);
	printf("free struct memory : %p\n", ptr[3]);
	printf("free struct memory : %p\n", ptr[5]);
	printf("free struct memory : %p\n", ptr[7]);
	printf("free struct memory : %p\n", ptr[9]);
	
	printf("\nBSMM state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objBSMM->ItemLength, sizeof(BSMM_Test_Struct));
	printf("\tCount : %d\t\t\t\t=> 10\n", objBSMM->Count);
	printf("\tPageMMU.Count : %d\t\t\t=> 1\n", objBSMM->PageMMU.Count);
	printf("\tPageMMU.AllocCount : %d\t\t=> 256\n", objBSMM->PageMMU.AllocCount);
	printf("\tPageMMU.AllocStep : %d\t\t\t=> 256\n", objBSMM->PageMMU.AllocStep);
	if ( objBSMM->PageMMU.Memory ) {
		printf("\tarrMMU.Memory : %p\t\t\tpass! √\n", objBSMM->PageMMU.Memory);
	} else {
		printf("\tarrMMU.Memory : %p\t\t\tfail! ×\n", objBSMM->PageMMU.Memory);
	}
	
	printf("\nBSMM LList : \n");
	if ( objBSMM->LL_Free ) {
		printf("\tLL_Free :\n");
		MemPtr_LLNode* pNode = objBSMM->LL_Free;
		while ( pNode ) {
			BSMM_Test_Object pVal = pNode->Ptr;
			printf("\t\t%p\t%p\t%p\t%d\n", pNode, pNode->Next, pNode->Ptr, pVal->iVal);
			pNode = pNode->Next;
		}
	} else {
		printf("\tLL_Free : null\n");
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 3 : alloc struct memory
	printf("BSMM test subject 3 : alloc struct memory\n\n");
	BSMM_Test_Object pVal = BSMM_Alloc(objBSMM);
	if ( pVal ) {
		pVal->iVal = 1000;
		printf("BSMM_Alloc ptr (%p) = %d\t\t\tpass! √\n", pVal, pVal->iVal);
	} else {
		printf("BSMM_Alloc ptr (%p) pos = %d\t\t\tfail! ×\n", pVal, 1);
	}
	pVal = BSMM_Alloc(objBSMM);
	if ( pVal ) {
		pVal->iVal = 2000;
		printf("BSMM_Alloc ptr (%p) = %d\t\t\tpass! √\n", pVal, pVal->iVal);
	} else {
		printf("BSMM_Alloc ptr (%p) pos = %d\t\t\tfail! ×\n", pVal, 2);
	}
	
	printf("\nBSMM state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objBSMM->ItemLength, sizeof(BSMM_Test_Struct));
	printf("\tCount : %d\t\t\t\t=> 10\n", objBSMM->Count);
	printf("\tPageMMU.Count : %d\t\t\t=> 1\n", objBSMM->PageMMU.Count);
	printf("\tPageMMU.AllocCount : %d\t\t=> 256\n", objBSMM->PageMMU.AllocCount);
	printf("\tPageMMU.AllocStep : %d\t\t\t=> 256\n", objBSMM->PageMMU.AllocStep);
	if ( objBSMM->PageMMU.Memory ) {
		printf("\tarrMMU.Memory : %p\t\t\tpass! √\n", objBSMM->PageMMU.Memory);
	} else {
		printf("\tarrMMU.Memory : %p\t\t\tfail! ×\n", objBSMM->PageMMU.Memory);
	}
	
	printf("\nBSMM LList : \n");
	if ( objBSMM->LL_Free ) {
		printf("\tLL_Free :\n");
		MemPtr_LLNode* pNode = objBSMM->LL_Free;
		while ( pNode ) {
			BSMM_Test_Object pVal = pNode->Ptr;
			printf("\t\t%p\t%p\t%p\t%d\n", pNode, pNode->Next, pNode->Ptr, pVal->iVal);
			pNode = pNode->Next;
		}
	} else {
		printf("\tLL_Free : null\n");
	}
	
	printf("\nBSMM values : \n");
	printf("\tidx\tptr\t\t\tiVal\texpected val\n");
	int t2val[10] = { 0, 10, 20, 30, 40, 50, 60, 2000, 80, 1000 };
	for ( int i = 0; i < 10; i++ ) {
		if ( ptr[i]->iVal == t2val[i] ) {
			printf("\t%d\t%p\t%d\t%d\t\tsucc! √\n", i, ptr[i], ptr[i]->iVal, t2val[i]);
		} else {
			printf("\t%d\t%p\t%d\t%d\t\tfail! ×\n", i, ptr[i], ptr[i]->iVal, t2val[i]);
		}
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 4 : alloc struct memory x 512
	printf("BSMM test subject 4 : alloc struct memory x 512\n\n");
	for ( int i = 0; i < 512; i++ ) {
		pVal = BSMM_Alloc(objBSMM);
		if ( pVal == NULL ) {
			printf("BSMM_Alloc ptr (%p) pos = %d\t\t\tfail! ×\n", pVal, i);
		}
	}
	
	printf("\nBSMM state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objBSMM->ItemLength, sizeof(BSMM_Test_Struct));
	printf("\tCount : %d\t\t\t\t=> 520\n", objBSMM->Count);
	printf("\tPageMMU.Count : %d\t\t\t=> 3\n", objBSMM->PageMMU.Count);
	printf("\tPageMMU.AllocCount : %d\t\t=> 256\n", objBSMM->PageMMU.AllocCount);
	printf("\tPageMMU.AllocStep : %d\t\t\t=> 256\n", objBSMM->PageMMU.AllocStep);
	if ( objBSMM->PageMMU.Memory ) {
		printf("\tarrMMU.Memory : %p\t\t\tpass! √\n", objBSMM->PageMMU.Memory);
	} else {
		printf("\tarrMMU.Memory : %p\t\t\tfail! ×\n", objBSMM->PageMMU.Memory);
	}
	
	printf("\nBSMM LList : \n");
	if ( objBSMM->LL_Free ) {
		printf("\tLL_Free :\n");
		MemPtr_LLNode* pNode = objBSMM->LL_Free;
		while ( pNode ) {
			BSMM_Test_Object pVal = pNode->Ptr;
			printf("\t\t%p\t%p\t%p\t%d\n", pNode, pNode->Next, pNode->Ptr, pVal->iVal);
			pNode = pNode->Next;
		}
	} else {
		printf("\tLL_Free : null\n");
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 5 : free struct memory
	printf("BSMM test subject 5 : free struct memory\n\n");
	BSMM_Free(objBSMM, ptr[2]);
	BSMM_Free(objBSMM, ptr[4]);
	BSMM_Free(objBSMM, ptr[6]);
	BSMM_Free(objBSMM, ptr[8]);
	printf("free struct memory : %p\n", ptr[2]);
	printf("free struct memory : %p\n", ptr[4]);
	printf("free struct memory : %p\n", ptr[6]);
	printf("free struct memory : %p\n", ptr[8]);
	
	printf("\nBSMM state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objBSMM->ItemLength, sizeof(BSMM_Test_Struct));
	printf("\tCount : %d\t\t\t\t=> 520\n", objBSMM->Count);
	printf("\tPageMMU.Count : %d\t\t\t=> 3\n", objBSMM->PageMMU.Count);
	printf("\tPageMMU.AllocCount : %d\t\t=> 256\n", objBSMM->PageMMU.AllocCount);
	printf("\tPageMMU.AllocStep : %d\t\t\t=> 256\n", objBSMM->PageMMU.AllocStep);
	if ( objBSMM->PageMMU.Memory ) {
		printf("\tarrMMU.Memory : %p\t\t\tpass! √\n", objBSMM->PageMMU.Memory);
	} else {
		printf("\tarrMMU.Memory : %p\t\t\tfail! ×\n", objBSMM->PageMMU.Memory);
	}
	
	printf("\nBSMM LList : \n");
	if ( objBSMM->LL_Free ) {
		printf("\tLL_Free :\n");
		MemPtr_LLNode* pNode = objBSMM->LL_Free;
		while ( pNode ) {
			BSMM_Test_Object pVal = pNode->Ptr;
			printf("\t\t%p\t%p\t%p\t%d\n", pNode, pNode->Next, pNode->Ptr, pVal->iVal);
			pNode = pNode->Next;
		}
	} else {
		printf("\tLL_Free : null\n");
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 6 : alloc struct memory
	printf("BSMM test subject 6 : alloc struct memory\n\n");
	pVal = BSMM_Alloc(objBSMM);
	if ( pVal ) {
		pVal->iVal = 3000;
		printf("BSMM_Alloc ptr (%p) = %d\t\t\tpass! √\n", pVal, pVal->iVal);
	} else {
		printf("BSMM_Alloc ptr (%p) pos = %d\t\t\tfail! ×\n", pVal, 1);
	}
	pVal = BSMM_Alloc(objBSMM);
	if ( pVal ) {
		pVal->iVal = 4000;
		printf("BSMM_Alloc ptr (%p) = %d\t\t\tpass! √\n", pVal, pVal->iVal);
	} else {
		printf("BSMM_Alloc ptr (%p) pos = %d\t\t\tfail! ×\n", pVal, 2);
	}
	
	printf("\nBSMM state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objBSMM->ItemLength, sizeof(BSMM_Test_Struct));
	printf("\tCount : %d\t\t\t\t=> 520\n", objBSMM->Count);
	printf("\tPageMMU.Count : %d\t\t\t=> 3\n", objBSMM->PageMMU.Count);
	printf("\tPageMMU.AllocCount : %d\t\t=> 256\n", objBSMM->PageMMU.AllocCount);
	printf("\tPageMMU.AllocStep : %d\t\t\t=> 256\n", objBSMM->PageMMU.AllocStep);
	if ( objBSMM->PageMMU.Memory ) {
		printf("\tarrMMU.Memory : %p\t\t\tpass! √\n", objBSMM->PageMMU.Memory);
	} else {
		printf("\tarrMMU.Memory : %p\t\t\tfail! ×\n", objBSMM->PageMMU.Memory);
	}
	
	printf("\nBSMM LList : \n");
	if ( objBSMM->LL_Free ) {
		printf("\tLL_Free :\n");
		MemPtr_LLNode* pNode = objBSMM->LL_Free;
		while ( pNode ) {
			BSMM_Test_Object pVal = pNode->Ptr;
			printf("\t\t%p\t%p\t%p\t%d\n", pNode, pNode->Next, pNode->Ptr, pVal->iVal);
			pNode = pNode->Next;
		}
	} else {
		printf("\tLL_Free : null\n");
	}
	
	printf("\nBSMM values : \n");
	printf("\tidx\tptr\t\t\tiVal\texpected val\n");
	int t3val[10] = { 0, 10, 20, 30, 40, 50, 4000, 2000, 3000, 1000 };
	for ( int i = 0; i < 10; i++ ) {
		if ( ptr[i]->iVal == t3val[i] ) {
			printf("\t%d\t%p\t%d\t%d\t\tsucc! √\n", i, ptr[i], ptr[i]->iVal, t3val[i]);
		} else {
			printf("\t%d\t%p\t%d\t%d\t\tfail! ×\n", i, ptr[i], ptr[i]->iVal, t3val[i]);
		}
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject X : struct unit & destroy
	printf("BSMM test subject X : struct unit & destroy\n\n");
	BSMM_Unit(objBSMM);
	printf("BSMM object (%p) already unit!\n\n", objBSMM);
	
	printf("\nBSMM state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objBSMM->ItemLength, sizeof(BSMM_Test_Struct));
	printf("\tCount : %d\t\t\t\t=> 0\n", objBSMM->Count);
	printf("\tPageMMU.Count : %d\t\t\t=> 0\n", objBSMM->PageMMU.Count);
	printf("\tPageMMU.AllocCount : %d\t\t\t=> 0\n", objBSMM->PageMMU.AllocCount);
	printf("\tPageMMU.AllocStep : %d\t\t\t=> 256\n", objBSMM->PageMMU.AllocStep);
	if ( objBSMM->PageMMU.Memory ) {
		printf("\tarrMMU.Memory : %p\t\t\tfail! ×\n", objBSMM->PageMMU.Memory);
	} else {
		printf("\tarrMMU.Memory : %p\t\t\tpass! √\n", objBSMM->PageMMU.Memory);
	}
	
	printf("\nBSMM LList : \n");
	if ( objBSMM->LL_Free ) {
		printf("\tLL_Free : %p\t\t\t\tfail! ×\n", objBSMM->LL_Free);
	} else {
		printf("\tLL_Free : null\n");
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	MMU_Unit();
}


