


// test MM256 struct
typedef struct {
	int iVal;
	char sVal[32];
} MM256_Test_Struct, *MM256_Test_Object;



void mm256_error_proc(MM256_Object objMM, int ErrorID)
{
	printf("!!! Error !!! error code : %d\n", ErrorID);
}



void test_mm256()
{
	system("cls");
	MMU_Init();
	
	
	
	int iViewCount;
	
	
	
	// subject 1 : create object
	printf("MM256 test subject 1 : create object\n\n");
	MM256_Object objMM = MM256_Create(sizeof(MM256_Test_Struct));
	if ( objMM ) {
		objMM->OnError = (void*)mm256_error_proc;
		printf("MM256 object : %p\t\t\t\t\tpass! √\n", objMM);
		printf("\tItemLength : %d\t\t\t\t=> %d\n", objMM->ItemLength, sizeof(MM256_Test_Struct));
		printf("\tarrMMU.ItemLength : %d\t\t\t=> %d\n", objMM->arrMMU.ItemLength, sizeof(MMU256_LLNode));
		printf("\tarrMMU.Count : %d\t\t\t=> 0\n", objMM->arrMMU.Count);
		printf("\tarrMMU.PageMMU.Count : %d\t\t=> 0\n", objMM->arrMMU.PageMMU.Count);
		printf("\tarrMMU.PageMMU.AllocCount : %d\t\t=> 0\n", objMM->arrMMU.PageMMU.AllocCount);
		printf("\tarrMMU.PageMMU.AllocStep : %d\t\t=> 64\n", objMM->arrMMU.PageMMU.AllocStep);
		if ( objMM->arrMMU.PageMMU.Memory ) {
			printf("\tarrMMU.PageMMU.Memory : %p\t\tfail! ×\n", objMM->arrMMU.PageMMU.Memory);
		} else {
			printf("\tarrMMU.PageMMU.Memory : %p\t\tpass! √\n", objMM->arrMMU.PageMMU.Memory);
		}
		
		printf("\nMM256 LList : \n");
		if ( objMM->LL_Idle ) {
			printf("\tLL_Idle : %p\t\t\t\tfail! ×\n", objMM->LL_Idle);
		} else {
			printf("\tLL_Idle : null\n");
		}
		if ( objMM->LL_Full ) {
			printf("\tLL_Full : %p\t\t\t\tfail! ×\n", objMM->LL_Full);
		} else {
			printf("\tLL_Full : null\n");
		}
		if ( objMM->LL_Free ) {
			printf("\tLL_Free : %p\t\t\t\tfail! ×\n", objMM->LL_Free);
		} else {
			printf("\tLL_Free : null\n");
		}
		if ( objMM->LL_Null ) {
			printf("\tLL_Null : %p\t\t\t\tfail! ×\n", objMM->LL_Null);
		} else {
			printf("\tLL_Null : null\n");
		}
	} else {
		printf("MM256 object : %p\t\t\t\t\tfail! ×\n", objMM);
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 2 : alloc struct memory
	printf("MM256 test subject 2 : alloc struct memory\n\n");
	MM256_Test_Struct* ptr[20];
	for ( int i = 0; i < 10; i++ ) {
		ptr[i] = MM256_Alloc(objMM);
		if ( ptr[i] ) {
			ptr[i]->iVal = i * 10;
			sprintf(ptr[i]->sVal, "String Field idx = %d", i);
			printf("MM256_Alloc ptr : %p\t\t\t\tpass! √\n", ptr[i]);
		} else {
			printf("MM256_Alloc ptr : %p\t\t\t\tfail! ×\n", ptr[i]);
		}
	}
	
	printf("\nMM256 state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objMM->ItemLength, sizeof(MM256_Test_Struct));
	printf("\tarrMMU.ItemLength : %d\t\t\t=> %d\n", objMM->arrMMU.ItemLength, sizeof(MMU256_LLNode));
	printf("\tarrMMU.Count : %d\t\t\t=> 1\n", objMM->arrMMU.Count);
	printf("\tarrMMU.PageMMU.Count : %d\t\t=> 1\n", objMM->arrMMU.PageMMU.Count);
	printf("\tarrMMU.PageMMU.AllocCount : %d\t\t=> 64\n", objMM->arrMMU.PageMMU.AllocCount);
	printf("\tarrMMU.PageMMU.AllocStep : %d\t\t=> 64\n", objMM->arrMMU.PageMMU.AllocStep);
	if ( objMM->arrMMU.PageMMU.Memory ) {
		printf("\tarrMMU.PageMMU.Memory : %p\t\tpass! √\n", objMM->arrMMU.PageMMU.Memory);
	} else {
		printf("\tarrMMU.PageMMU.Memory : %p\t\tfail! ×\n", objMM->arrMMU.PageMMU.Memory);
	}
	
	printf("\nMM256 LList (pLLI, pMMU, Flag, Count, FreeCount, FreeOffset) : \n");
	if ( objMM->LL_Idle ) {
		printf("\tLL_Idle :\n");
		MMU256_LLNode* pNode = objMM->LL_Idle;
		while ( pNode ) {
			printf("\t\t%p\t%p\t%x\t%d\t%d\t%d\n", pNode, pNode->objMMU, pNode->objMMU->Flag, pNode->objMMU->Count, pNode->objMMU->FreeCount, pNode->objMMU->FreeOffset);
			pNode = pNode->Next;
		}
	} else {
		printf("\tLL_Idle : null\n");
	}
	if ( objMM->LL_Full ) {
		printf("\tLL_Full :\n");
		MMU256_LLNode* pNode = objMM->LL_Full;
		while ( pNode ) {
			printf("\t\t%p\t%p\t%x\t%d\t%d\t%d\n", pNode, pNode->objMMU, pNode->objMMU->Flag, pNode->objMMU->Count, pNode->objMMU->FreeCount, pNode->objMMU->FreeOffset);
			pNode = pNode->Next;
		}
	} else {
		printf("\tLL_Full : null\n");
	}
	if ( objMM->LL_Free ) {
		printf("\tLL_Free :\n");
		MMU256_LLNode* pNode = objMM->LL_Free;
		while ( pNode ) {
			printf("\t\t%p\t%p\t%x\n", pNode, pNode->objMMU, pNode->Flag);
			pNode = pNode->Next;
		}
	} else {
		printf("\tLL_Free : null\n");
	}
	if ( objMM->LL_Null ) {
		printf("\tLL_Null :\n");
		MMU256_LLNode* pNode = objMM->LL_Null;
		printf("\t\t%p\t%p\t%x\t%d\t%d\t%d\n", pNode, pNode->objMMU, pNode->objMMU->Flag, pNode->objMMU->Count, pNode->objMMU->FreeCount, pNode->objMMU->FreeOffset);
	} else {
		printf("\tLL_Null : null\n");
	}
	
	printf("\nMM256 values : \n");
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
	printf("MM256 test subject 3 : free & alloc struct memory\n\n");
	MM256_Free(objMM, ptr[3]);
	MM256_Free(objMM, ptr[5]);
	MM256_Free(objMM, ptr[7]);
	printf("free struct memory : %p\n", ptr[3]);
	printf("free struct memory : %p\n", ptr[5]);
	printf("free struct memory : %p\n", ptr[7]);
	for ( int i = 0; i < 10; i++ ) {
		ptr[i + 10] = MM256_Alloc(objMM);
		if ( ptr[i + 10] ) {
			ptr[i + 10]->iVal = i + 100;
			sprintf(ptr[i + 10]->sVal, "String Field NewID %d", i);
			printf("MM256_Alloc ptr : %p\t\t\t\tpass! √\n", ptr[i]);
		} else {
			printf("MM256_Alloc ptr : %p\t\t\t\tfail! ×\n", ptr[i]);
		}
	}
	
	printf("\nMM256 state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objMM->ItemLength, sizeof(MM256_Test_Struct));
	printf("\tarrMMU.ItemLength : %d\t\t\t=> %d\n", objMM->arrMMU.ItemLength, sizeof(MMU256_LLNode));
	printf("\tarrMMU.Count : %d\t\t\t=> 1\n", objMM->arrMMU.Count);
	printf("\tarrMMU.PageMMU.Count : %d\t\t=> 1\n", objMM->arrMMU.PageMMU.Count);
	printf("\tarrMMU.PageMMU.AllocCount : %d\t\t=> 64\n", objMM->arrMMU.PageMMU.AllocCount);
	printf("\tarrMMU.PageMMU.AllocStep : %d\t\t=> 64\n", objMM->arrMMU.PageMMU.AllocStep);
	if ( objMM->arrMMU.PageMMU.Memory ) {
		printf("\tarrMMU.PageMMU.Memory : %p\t\tpass! √\n", objMM->arrMMU.PageMMU.Memory);
	} else {
		printf("\tarrMMU.PageMMU.Memory : %p\t\tfail! ×\n", objMM->arrMMU.PageMMU.Memory);
	}
	
	printf("\nMM256 LList (pLLI, pMMU, Flag, Count, FreeCount, FreeOffset) : \n");
	if ( objMM->LL_Idle ) {
		printf("\tLL_Idle :\n");
		MMU256_LLNode* pNode = objMM->LL_Idle;
		while ( pNode ) {
			printf("\t\t%p\t%p\t%x\t%d\t%d\t%d\n", pNode, pNode->objMMU, pNode->objMMU->Flag, pNode->objMMU->Count, pNode->objMMU->FreeCount, pNode->objMMU->FreeOffset);
			pNode = pNode->Next;
		}
	} else {
		printf("\tLL_Idle : null\n");
	}
	if ( objMM->LL_Full ) {
		printf("\tLL_Full :\n");
		MMU256_LLNode* pNode = objMM->LL_Full;
		while ( pNode ) {
			printf("\t\t%p\t%p\t%x\t%d\t%d\t%d\n", pNode, pNode->objMMU, pNode->objMMU->Flag, pNode->objMMU->Count, pNode->objMMU->FreeCount, pNode->objMMU->FreeOffset);
			pNode = pNode->Next;
		}
	} else {
		printf("\tLL_Full : null\n");
	}
	if ( objMM->LL_Free ) {
		printf("\tLL_Free :\n");
		MMU256_LLNode* pNode = objMM->LL_Free;
		while ( pNode ) {
			printf("\t\t%p\t%p\t%x\n", pNode, pNode->objMMU, pNode->Flag);
			pNode = pNode->Next;
		}
	} else {
		printf("\tLL_Free : null\n");
	}
	if ( objMM->LL_Null ) {
		printf("\tLL_Null :\n");
		MMU256_LLNode* pNode = objMM->LL_Null;
		printf("\t\t%p\t%p\t%x\t%d\t%d\t%d\n", pNode, pNode->objMMU, pNode->objMMU->Flag, pNode->objMMU->Count, pNode->objMMU->FreeCount, pNode->objMMU->FreeOffset);
	} else {
		printf("\tLL_Null : null\n");
	}
	
	printf("\nMM256 values : \n");
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
	printf("MM256 test subject 4 : free struct memory\n\n");
	MM256_Free(objMM, ptr[14]);
	MM256_Free(objMM, ptr[16]);
	MM256_Free(objMM, ptr[18]);
	printf("free struct memory : %p\n", ptr[14]);
	printf("free struct memory : %p\n", ptr[16]);
	printf("free struct memory : %p\n", ptr[18]);
	
	printf("\nMM256 state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objMM->ItemLength, sizeof(MM256_Test_Struct));
	printf("\tarrMMU.ItemLength : %d\t\t\t=> %d\n", objMM->arrMMU.ItemLength, sizeof(MMU256_LLNode));
	printf("\tarrMMU.Count : %d\t\t\t=> 1\n", objMM->arrMMU.Count);
	printf("\tarrMMU.PageMMU.Count : %d\t\t=> 1\n", objMM->arrMMU.PageMMU.Count);
	printf("\tarrMMU.PageMMU.AllocCount : %d\t\t=> 64\n", objMM->arrMMU.PageMMU.AllocCount);
	printf("\tarrMMU.PageMMU.AllocStep : %d\t\t=> 64\n", objMM->arrMMU.PageMMU.AllocStep);
	if ( objMM->arrMMU.PageMMU.Memory ) {
		printf("\tarrMMU.PageMMU.Memory : %p\t\tpass! √\n", objMM->arrMMU.PageMMU.Memory);
	} else {
		printf("\tarrMMU.PageMMU.Memory : %p\t\tfail! ×\n", objMM->arrMMU.PageMMU.Memory);
	}
	
	printf("\nMM256 LList (pLLI, pMMU, Flag, Count, FreeCount, FreeOffset) : \n");
	if ( objMM->LL_Idle ) {
		printf("\tLL_Idle :\n");
		MMU256_LLNode* pNode = objMM->LL_Idle;
		while ( pNode ) {
			printf("\t\t%p\t%p\t%x\t%d\t%d\t%d\n", pNode, pNode->objMMU, pNode->objMMU->Flag, pNode->objMMU->Count, pNode->objMMU->FreeCount, pNode->objMMU->FreeOffset);
			pNode = pNode->Next;
		}
	} else {
		printf("\tLL_Idle : null\n");
	}
	if ( objMM->LL_Full ) {
		printf("\tLL_Full :\n");
		MMU256_LLNode* pNode = objMM->LL_Full;
		while ( pNode ) {
			printf("\t\t%p\t%p\t%x\t%d\t%d\t%d\n", pNode, pNode->objMMU, pNode->objMMU->Flag, pNode->objMMU->Count, pNode->objMMU->FreeCount, pNode->objMMU->FreeOffset);
			pNode = pNode->Next;
		}
	} else {
		printf("\tLL_Full : null\n");
	}
	if ( objMM->LL_Free ) {
		printf("\tLL_Free :\n");
		MMU256_LLNode* pNode = objMM->LL_Free;
		while ( pNode ) {
			printf("\t\t%p\t%p\t%x\n", pNode, pNode->objMMU, pNode->Flag);
			pNode = pNode->Next;
		}
	} else {
		printf("\tLL_Free : null\n");
	}
	if ( objMM->LL_Null ) {
		printf("\tLL_Null :\n");
		MMU256_LLNode* pNode = objMM->LL_Null;
		printf("\t\t%p\t%p\t%x\t%d\t%d\t%d\n", pNode, pNode->objMMU, pNode->objMMU->Flag, pNode->objMMU->Count, pNode->objMMU->FreeCount, pNode->objMMU->FreeOffset);
	} else {
		printf("\tLL_Null : null\n");
	}
	
	printf("\nMM256 values : \n");
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
	
	
	
	// subject 5 : alloc all (max 512)
	printf("MM256 test subject 5 : alloc all (max 512)\n\n");
	for ( int i = 0; i < 498; i++ ) {
		MM256_Test_Object mto = MM256_Alloc(objMM);
		if ( mto ) {
			mto->iVal = -i;
			sprintf(mto->sVal, "test alloc idx : %d", i);
		} else {
			printf("MM256_Alloc Fail (Should be succ) pos : %d\t\t\tfail! ×\n", i);
		}
	}
	void* failptr = MM256_Alloc(objMM);
	if ( failptr == NULL ) {
		printf("MM256_Alloc Fail (Should be succ)\t\t\t\tfail! ×\n");
	}
	
	printf("\nMM256 state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objMM->ItemLength, sizeof(MM256_Test_Struct));
	printf("\tarrMMU.ItemLength : %d\t\t\t=> %d\n", objMM->arrMMU.ItemLength, sizeof(MMU256_LLNode));
	printf("\tarrMMU.Count : %d\t\t\t=> 3\n", objMM->arrMMU.Count);
	printf("\tarrMMU.PageMMU.Count : %d\t\t=> 1\n", objMM->arrMMU.PageMMU.Count);
	printf("\tarrMMU.PageMMU.AllocCount : %d\t\t=> 64\n", objMM->arrMMU.PageMMU.AllocCount);
	printf("\tarrMMU.PageMMU.AllocStep : %d\t\t=> 64\n", objMM->arrMMU.PageMMU.AllocStep);
	if ( objMM->arrMMU.PageMMU.Memory ) {
		printf("\tarrMMU.PageMMU.Memory : %p\t\tpass! √\n", objMM->arrMMU.PageMMU.Memory);
	} else {
		printf("\tarrMMU.PageMMU.Memory : %p\t\tfail! ×\n", objMM->arrMMU.PageMMU.Memory);
	}
	
	printf("\nMM256 LList (pLLI, pMMU, Flag, Count, FreeCount, FreeOffset) : \n");
	if ( objMM->LL_Idle ) {
		printf("\tLL_Idle :\n");
		MMU256_LLNode* pNode = objMM->LL_Idle;
		while ( pNode ) {
			printf("\t\t%p\t%p\t%x\t%d\t%d\t%d\n", pNode, pNode->objMMU, pNode->objMMU->Flag, pNode->objMMU->Count, pNode->objMMU->FreeCount, pNode->objMMU->FreeOffset);
			pNode = pNode->Next;
		}
	} else {
		printf("\tLL_Idle : null\n");
	}
	if ( objMM->LL_Full ) {
		printf("\tLL_Full :\n");
		MMU256_LLNode* pNode = objMM->LL_Full;
		while ( pNode ) {
			printf("\t\t%p\t%p\t%x\t%d\t%d\t%d\n", pNode, pNode->objMMU, pNode->objMMU->Flag, pNode->objMMU->Count, pNode->objMMU->FreeCount, pNode->objMMU->FreeOffset);
			pNode = pNode->Next;
		}
	} else {
		printf("\tLL_Full : null\n");
	}
	if ( objMM->LL_Free ) {
		printf("\tLL_Free :\n");
		MMU256_LLNode* pNode = objMM->LL_Free;
		while ( pNode ) {
			printf("\t\t%p\t%p\t%x\n", pNode, pNode->objMMU, pNode->Flag);
			pNode = pNode->Next;
		}
	} else {
		printf("\tLL_Free : null\n");
	}
	if ( objMM->LL_Null ) {
		printf("\tLL_Null :\n");
		MMU256_LLNode* pNode = objMM->LL_Null;
		printf("\t\t%p\t%p\t%x\t%d\t%d\t%d\n", pNode, pNode->objMMU, pNode->objMMU->Flag, pNode->objMMU->Count, pNode->objMMU->FreeCount, pNode->objMMU->FreeOffset);
	} else {
		printf("\tLL_Null : null\n");
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 6 : free (max 256)
	printf("MM256 test subject 6 : free (max 256)\n\n");
	MM256_Free(objMM, failptr);
	MM256_Free(objMM, ptr[19]);
	
	printf("\nMM256 state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objMM->ItemLength, sizeof(MM256_Test_Struct));
	printf("\tarrMMU.ItemLength : %d\t\t\t=> %d\n", objMM->arrMMU.ItemLength, sizeof(MMU256_LLNode));
	printf("\tarrMMU.Count : %d\t\t\t=> 3\n", objMM->arrMMU.Count);
	printf("\tarrMMU.PageMMU.Count : %d\t\t=> 1\n", objMM->arrMMU.PageMMU.Count);
	printf("\tarrMMU.PageMMU.AllocCount : %d\t\t=> 64\n", objMM->arrMMU.PageMMU.AllocCount);
	printf("\tarrMMU.PageMMU.AllocStep : %d\t\t=> 64\n", objMM->arrMMU.PageMMU.AllocStep);
	if ( objMM->arrMMU.PageMMU.Memory ) {
		printf("\tarrMMU.PageMMU.Memory : %p\t\tpass! √\n", objMM->arrMMU.PageMMU.Memory);
	} else {
		printf("\tarrMMU.PageMMU.Memory : %p\t\tfail! ×\n", objMM->arrMMU.PageMMU.Memory);
	}
	
	printf("\nMM256 LList (pLLI, pMMU, Flag, Count, FreeCount, FreeOffset) : \n");
	if ( objMM->LL_Idle ) {
		printf("\tLL_Idle :\n");
		MMU256_LLNode* pNode = objMM->LL_Idle;
		while ( pNode ) {
			printf("\t\t%p\t%p\t%x\t%d\t%d\t%d\n", pNode, pNode->objMMU, pNode->objMMU->Flag, pNode->objMMU->Count, pNode->objMMU->FreeCount, pNode->objMMU->FreeOffset);
			pNode = pNode->Next;
		}
	} else {
		printf("\tLL_Idle : null\n");
	}
	if ( objMM->LL_Full ) {
		printf("\tLL_Full :\n");
		MMU256_LLNode* pNode = objMM->LL_Full;
		while ( pNode ) {
			printf("\t\t%p\t%p\t%x\t%d\t%d\t%d\n", pNode, pNode->objMMU, pNode->objMMU->Flag, pNode->objMMU->Count, pNode->objMMU->FreeCount, pNode->objMMU->FreeOffset);
			pNode = pNode->Next;
		}
	} else {
		printf("\tLL_Full : null\n");
	}
	if ( objMM->LL_Free ) {
		printf("\tLL_Free :\n");
		MMU256_LLNode* pNode = objMM->LL_Free;
		while ( pNode ) {
			printf("\t\t%p\t%p\t%x\n", pNode, pNode->objMMU, pNode->Flag);
			pNode = pNode->Next;
		}
	} else {
		printf("\tLL_Free : null\n");
	}
	if ( objMM->LL_Null ) {
		printf("\tLL_Null :\n");
		MMU256_LLNode* pNode = objMM->LL_Null;
		printf("\t\t%p\t%p\t%x\t%d\t%d\t%d\n", pNode, pNode->objMMU, pNode->objMMU->Flag, pNode->objMMU->Count, pNode->objMMU->FreeCount, pNode->objMMU->FreeOffset);
	} else {
		printf("\tLL_Null : null\n");
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 7 : 10m alloc stress testing
	printf("MM256 test subject 7 : 10m alloc stress testing\n\n");
	clock_t tStart = clock();
	for ( int i = 0; i < 10000000; i++ ) {
		MM256_Test_Struct* pVal = MM256_Alloc(objMM);
		if ( pVal ) {
			pVal->iVal = i;
		} else {
			printf("MM256 alloc (%d) : null\t\t\t\t\t\tfail! ×\n", i);
		}
	}
	clock_t tStop = clock();
	printf("MM256 alloc 10000000 struct time interval is: %f seconds\n", (double)(tStop - tStart) / CLOCKS_PER_SEC);
	
	printf("\nMM256 state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objMM->ItemLength, sizeof(MM256_Test_Struct));
	printf("\tarrMMU.ItemLength : %d\t\t\t=> %d\n", objMM->arrMMU.ItemLength, sizeof(MMU256_LLNode));
	printf("\tarrMMU.Count : %d\t\t\t=> 39065\n", objMM->arrMMU.Count);
	printf("\tarrMMU.PageMMU.Count : %d\t\t=> 153\n", objMM->arrMMU.PageMMU.Count);
	printf("\tarrMMU.PageMMU.AllocCount : %d\t\t=> 192\n", objMM->arrMMU.PageMMU.AllocCount);
	printf("\tarrMMU.PageMMU.AllocStep : %d\t\t=> 64\n", objMM->arrMMU.PageMMU.AllocStep);
	if ( objMM->arrMMU.PageMMU.Memory ) {
		printf("\tarrMMU.PageMMU.Memory : %p\t\tpass! √\n", objMM->arrMMU.PageMMU.Memory);
	} else {
		printf("\tarrMMU.PageMMU.Memory : %p\t\tfail! ×\n", objMM->arrMMU.PageMMU.Memory);
	}
	
	printf("\nMM256 LList (pLLI, pMMU, Flag, Count, FreeCount, FreeOffset) : \n");
	if ( objMM->LL_Idle ) {
		printf("\tLL_Idle :\n");
		MMU256_LLNode* pNode = objMM->LL_Idle;
		while ( pNode ) {
			printf("\t\t%p\t%p\t%x\t%d\t%d\t%d\n", pNode, pNode->objMMU, pNode->objMMU->Flag, pNode->objMMU->Count, pNode->objMMU->FreeCount, pNode->objMMU->FreeOffset);
			pNode = pNode->Next;
		}
	} else {
		printf("\tLL_Idle : null\n");
	}
	iViewCount = 0;
	if ( objMM->LL_Full ) {
		printf("\tLL_Full :\n");
		MMU256_LLNode* pNode = objMM->LL_Full;
		while ( pNode ) {
			printf("\t\t%p\t%p\t%x\t%d\t%d\t%d\n", pNode, pNode->objMMU, pNode->objMMU->Flag, pNode->objMMU->Count, pNode->objMMU->FreeCount, pNode->objMMU->FreeOffset);
			pNode = pNode->Next;
			iViewCount++;
			if ( iViewCount > 100 ) {
				printf("\t\tOmit subsequent display of array units (max 100) ...\n");
				break;
			}
		}
	} else {
		printf("\tLL_Full : null\n");
	}
	iViewCount = 0;
	if ( objMM->LL_Free ) {
		printf("\tLL_Free :\n");
		MMU256_LLNode* pNode = objMM->LL_Free;
		while ( pNode ) {
			printf("\t\t%p\t%p\t%x\n", pNode, pNode->objMMU, pNode->Flag);
			pNode = pNode->Next;
			iViewCount++;
			if ( iViewCount > 100 ) {
				printf("\t\tOmit subsequent display of array units (max 100) ...\n");
				break;
			}
		}
	} else {
		printf("\tLL_Free : null\n");
	}
	if ( objMM->LL_Null ) {
		printf("\tLL_Null :\n");
		MMU256_LLNode* pNode = objMM->LL_Null;
		printf("\t\t%p\t%p\t%x\t%d\t%d\t%d\n", pNode, pNode->objMMU, pNode->objMMU->Flag, pNode->objMMU->Count, pNode->objMMU->FreeCount, pNode->objMMU->FreeOffset);
	} else {
		printf("\tLL_Null : null\n");
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 8 : garbage collection (marked free)
	printf("MM256 test subject 8 : garbage collection (marked free)\n\n");
	MM256_GC_Mark(ptr[2]);
	MM256_GC_Mark(ptr[4]);
	MM256_GC_Mark(ptr[6]);
	MM256_GC(objMM, TRUE);
	printf("garbage collection mark : ptr[2]、ptr[4]、ptr[6]\n");
	
	printf("\nMM256 state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objMM->ItemLength, sizeof(MM256_Test_Struct));
	printf("\tarrMMU.ItemLength : %d\t\t\t=> %d\n", objMM->arrMMU.ItemLength, sizeof(MMU256_LLNode));
	printf("\tarrMMU.Count : %d\t\t\t=> 39065\n", objMM->arrMMU.Count);
	printf("\tarrMMU.PageMMU.Count : %d\t\t=> 153\n", objMM->arrMMU.PageMMU.Count);
	printf("\tarrMMU.PageMMU.AllocCount : %d\t\t=> 192\n", objMM->arrMMU.PageMMU.AllocCount);
	printf("\tarrMMU.PageMMU.AllocStep : %d\t\t=> 64\n", objMM->arrMMU.PageMMU.AllocStep);
	if ( objMM->arrMMU.PageMMU.Memory ) {
		printf("\tarrMMU.PageMMU.Memory : %p\t\tpass! √\n", objMM->arrMMU.PageMMU.Memory);
	} else {
		printf("\tarrMMU.PageMMU.Memory : %p\t\tfail! ×\n", objMM->arrMMU.PageMMU.Memory);
	}
	
	printf("\nMM256 LList (pLLI, pMMU, Flag, Count, FreeCount, FreeOffset) : \n");
	if ( objMM->LL_Idle ) {
		printf("\tLL_Idle :\n");
		MMU256_LLNode* pNode = objMM->LL_Idle;
		while ( pNode ) {
			printf("\t\t%p\t%p\t%x\t%d\t%d\t%d\n", pNode, pNode->objMMU, pNode->objMMU->Flag, pNode->objMMU->Count, pNode->objMMU->FreeCount, pNode->objMMU->FreeOffset);
			pNode = pNode->Next;
		}
	} else {
		printf("\tLL_Idle : null\n");
	}
	iViewCount = 0;
	if ( objMM->LL_Full ) {
		printf("\tLL_Full :\n");
		MMU256_LLNode* pNode = objMM->LL_Full;
		while ( pNode ) {
			printf("\t\t%p\t%p\t%x\t%d\t%d\t%d\n", pNode, pNode->objMMU, pNode->objMMU->Flag, pNode->objMMU->Count, pNode->objMMU->FreeCount, pNode->objMMU->FreeOffset);
			pNode = pNode->Next;
			iViewCount++;
			if ( iViewCount > 100 ) {
				printf("\t\tOmit subsequent display of array units (max 100) ...\n");
				break;
			}
		}
	} else {
		printf("\tLL_Full : null\n");
	}
	iViewCount = 0;
	if ( objMM->LL_Free ) {
		printf("\tLL_Free :\n");
		MMU256_LLNode* pNode = objMM->LL_Free;
		while ( pNode ) {
			printf("\t\t%p\t%p\t%x\n", pNode, pNode->objMMU, pNode->Flag);
			pNode = pNode->Next;
			iViewCount++;
			if ( iViewCount > 100 ) {
				printf("\t\tOmit subsequent display of array units (max 100) ...\n");
				break;
			}
		}
	} else {
		printf("\tLL_Free : null\n");
	}
	if ( objMM->LL_Null ) {
		printf("\tLL_Null :\n");
		MMU256_LLNode* pNode = objMM->LL_Null;
		printf("\t\t%p\t%p\t%x\t%d\t%d\t%d\n", pNode, pNode->objMMU, pNode->objMMU->Flag, pNode->objMMU->Count, pNode->objMMU->FreeCount, pNode->objMMU->FreeOffset);
	} else {
		printf("\tLL_Null : null\n");
	}
	
	printf("\nMM256 values : \n");
	printf("\tidx\tptr\t\t\tflag\t\tiVal\tsVal\n");
	for ( int i = 0; i < 20; i++ ) {
		MMU_ValuePtr mvp = (void*)(ptr[i]) - 4;
		printf("\t%d\t%p\t% 8x\t%d\t%s\n", i, ptr[i], mvp->ItemFlag, ptr[i]->iVal, ptr[i]->sVal);
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 9 : garbage collection (not marked free)
	printf("MM256 test subject 9 : garbage collection (not marked free)\n\n");
	MM256_GC_Mark(ptr[13]);
	MM256_GC_Mark(ptr[15]);
	MM256_GC_Mark(ptr[17]);
	MM256_GC(objMM, FALSE);
	printf("garbage collection mark : ptr[13]、ptr[15]、ptr[17]\n");
	
	printf("\nMM256 state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objMM->ItemLength, sizeof(MM256_Test_Struct));
	printf("\tarrMMU.ItemLength : %d\t\t\t=> %d\n", objMM->arrMMU.ItemLength, sizeof(MMU256_LLNode));
	printf("\tarrMMU.Count : %d\t\t\t=> 39065\n", objMM->arrMMU.Count);
	printf("\tarrMMU.PageMMU.Count : %d\t\t=> 153\n", objMM->arrMMU.PageMMU.Count);
	printf("\tarrMMU.PageMMU.AllocCount : %d\t\t=> 192\n", objMM->arrMMU.PageMMU.AllocCount);
	printf("\tarrMMU.PageMMU.AllocStep : %d\t\t=> 64\n", objMM->arrMMU.PageMMU.AllocStep);
	if ( objMM->arrMMU.PageMMU.Memory ) {
		printf("\tarrMMU.PageMMU.Memory : %p\t\tpass! √\n", objMM->arrMMU.PageMMU.Memory);
	} else {
		printf("\tarrMMU.PageMMU.Memory : %p\t\tfail! ×\n", objMM->arrMMU.PageMMU.Memory);
	}
	
	printf("\nMM256 LList (pLLI, pMMU, Flag, Count, FreeCount, FreeOffset) : \n");
	if ( objMM->LL_Idle ) {
		printf("\tLL_Idle :\n");
		MMU256_LLNode* pNode = objMM->LL_Idle;
		while ( pNode ) {
			printf("\t\t%p\t%p\t%x\t%d\t%d\t%d\n", pNode, pNode->objMMU, pNode->objMMU->Flag, pNode->objMMU->Count, pNode->objMMU->FreeCount, pNode->objMMU->FreeOffset);
			pNode = pNode->Next;
		}
	} else {
		printf("\tLL_Idle : null\n");
	}
	iViewCount = 0;
	if ( objMM->LL_Full ) {
		printf("\tLL_Full :\n");
		MMU256_LLNode* pNode = objMM->LL_Full;
		while ( pNode ) {
			printf("\t\t%p\t%p\t%x\t%d\t%d\t%d\n", pNode, pNode->objMMU, pNode->objMMU->Flag, pNode->objMMU->Count, pNode->objMMU->FreeCount, pNode->objMMU->FreeOffset);
			pNode = pNode->Next;
			iViewCount++;
			if ( iViewCount > 100 ) {
				printf("\t\tOmit subsequent display of array units (max 100) ...\n");
				break;
			}
		}
	} else {
		printf("\tLL_Full : null\n");
	}
	iViewCount = 0;
	if ( objMM->LL_Free ) {
		printf("\tLL_Free :\n");
		MMU256_LLNode* pNode = objMM->LL_Free;
		while ( pNode ) {
			printf("\t\t%p\t%p\t%x\n", pNode, pNode->objMMU, pNode->Flag);
			pNode = pNode->Next;
			iViewCount++;
			if ( iViewCount > 100 ) {
				printf("\t\tOmit subsequent display of array units (max 100) ...\n");
				break;
			}
		}
	} else {
		printf("\tLL_Free : null\n");
	}
	if ( objMM->LL_Null ) {
		printf("\tLL_Null :\n");
		MMU256_LLNode* pNode = objMM->LL_Null;
		printf("\t\t%p\t%p\t%x\t%d\t%d\t%d\n", pNode, pNode->objMMU, pNode->objMMU->Flag, pNode->objMMU->Count, pNode->objMMU->FreeCount, pNode->objMMU->FreeOffset);
	} else {
		printf("\tLL_Null : null\n");
	}
	
	printf("\nMM256 values : \n");
	printf("\tidx\tptr\t\t\tflag\t\tiVal\tsVal\n");
	for ( int i = 0; i < 20; i++ ) {
		MMU_ValuePtr mvp = (void*)(ptr[i]) - 4;
		printf("\t%d\t%p\t% 8x\t%d\t%s\n", i, ptr[i], mvp->ItemFlag, ptr[i]->iVal, ptr[i]->sVal);
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject X : struct unit & destroy
	printf("MM256 test subject X : struct unit & destroy\n\n");
	MM256_Unit(objMM);
	printf("MM256 object (%p) already unit!\n\n", objMM);
	
	printf("\nMM256 state : \n");
	printf("\tItemLength : %d\t\t\t\t=> %d\n", objMM->ItemLength, sizeof(MM256_Test_Struct));
	printf("\tarrMMU.ItemLength : %d\t\t\t=> %d\n", objMM->arrMMU.ItemLength, sizeof(MMU256_LLNode));
	printf("\tarrMMU.Count : %d\t\t\t=> 0\n", objMM->arrMMU.Count);
	printf("\tarrMMU.PageMMU.Count : %d\t\t=> 0\n", objMM->arrMMU.PageMMU.Count);
	printf("\tarrMMU.PageMMU.AllocCount : %d\t\t=> 0\n", objMM->arrMMU.PageMMU.AllocCount);
	printf("\tarrMMU.PageMMU.AllocStep : %d\t\t=> 64\n", objMM->arrMMU.PageMMU.AllocStep);
	if ( objMM->arrMMU.PageMMU.Memory ) {
		printf("\tarrMMU.PageMMU.Memory : %p\t\tfail! ×\n", objMM->arrMMU.PageMMU.Memory);
	} else {
		printf("\tarrMMU.PageMMU.Memory : %p\t\tpass! √\n", objMM->arrMMU.PageMMU.Memory);
	}
	
	printf("\nMM256 LList : \n");
	if ( objMM->LL_Idle ) {
		printf("\tLL_Idle : %p\t\t\t\tfail! ×\n", objMM->LL_Idle);
	} else {
		printf("\tLL_Idle : null\n");
	}
	if ( objMM->LL_Full ) {
		printf("\tLL_Full : %p\t\t\t\tfail! ×\n", objMM->LL_Full);
	} else {
		printf("\tLL_Full : null\n");
	}
	if ( objMM->LL_Free ) {
		printf("\tLL_Free : %p\t\t\t\tfail! ×\n", objMM->LL_Free);
	} else {
		printf("\tLL_Free : null\n");
	}
	if ( objMM->LL_Null ) {
		printf("\tLL_Null : %p\t\t\t\tfail! ×\n", objMM->LL_Null);
	} else {
		printf("\tLL_Null : null\n");
	}
	
	MM256_Destroy(objMM);
	printf("\nMM256 object (%p) already destroyed!\n", objMM);
	
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	MMU_Unit();
}


