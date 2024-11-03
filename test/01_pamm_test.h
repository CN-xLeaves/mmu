


int PAMM_TestSortProc(void** p1, void** p2)
{
	return (intptr_t)*p1 - (intptr_t)*p2;
}



void test_pamm()
{
	system("cls");
	MMU_Init();
	
	
	
	// subject 1 : create object
	printf("PAMM test subject 1 : create object\n\n");
	PAMM_Object objPAMM = PAMM_Create();
	if ( objPAMM ) {
		printf("PAMM object : %p\t\t\t\t\tpass! √\n", objPAMM);
		printf("\tMemory : %p\t\t=> 0\n", objPAMM->Memory);
		printf("\tCount : %d\t\t\t\t=> 0\n", objPAMM->Count);
		printf("\tAllocCount : %d\t\t\t\t=> 0\n", objPAMM->AllocCount);
	} else {
		printf("PAMM object : %p\t\t\t\t\tfail! ×\n", objPAMM);
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 2 : append item
	printf("PAMM test subject 2 : append item\n\n");
	for ( int i = 0; i < 10; i++ ) {
		unsigned int idx = i + 1;
		idx = PAMM_Append(objPAMM, (void*)(intptr_t)(idx * 1000 + idx));
		if ( idx ) {
			printf("PAMM_Append idx : %d\t\t\t\t\t\tpass! √\n", idx);
		} else {
			printf("PAMM_Append idx : %d\t\t\t\t\t\tfail! ×\n", idx);
		}
	}
	
	printf("\nPAMM state : \n");
	if ( objPAMM->Memory ) {
		printf("\tMemory : %p\t\t\t\tpass! √\n", objPAMM->Memory);
	} else {
		printf("\tMemory : %p\t\t\t\tfail! ×\n", objPAMM->Memory);
	}
	printf("\tCount : %d\t\t\t\t=> 10\n", objPAMM->Count);
	printf("\tAllocCount : %d\t\t\t=> 256\n", objPAMM->AllocCount);
	
	printf("\nPAMM values : \n");
	printf("\tidx\tptr\t\t\tval\texpected val\n");
	void* t1val[10] = { (void*)1001, (void*)2002, (void*)3003, (void*)4004, (void*)5005, (void*)6006, (void*)7007, (void*)8008, (void*)9009, (void*)10010 };
	for ( int i = 1; i <= objPAMM->Count; i++ ) {
		void* val = PAMM_GetVal_Unsafe(objPAMM, i);
		void* ptr = PAMM_GetPtr_Unsafe(objPAMM, i);
		if ( val == t1val[i - 1] ) {
			printf("\t%d\t%p\t%d\t%d\t\tsucc! √\n", i, ptr, val, t1val[i - 1]);
		} else {
			printf("\t%d\t%p\t%d\t%d\t\tfail! ×\n", i, ptr, val, t1val[i - 1]);
		}
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 3 : insert item
	printf("PAMM test subject 3 : insert item\n\n");
	for ( int i = 0; i < 10; i++ ) {
		unsigned int idx = i + 1;
		idx = PAMM_Insert(objPAMM, i * 2, (void*)(intptr_t)(idx * 10000 + idx));
		if ( idx ) {
			printf("PAMM_Insert idx : %d\t\t\t\t\t\tpass! √\n", idx);
		} else {
			printf("PAMM_Insert idx : %d\t\t\t\t\t\tfail! ×\n", idx);
		}
	}
	
	printf("\nPAMM state : \n");
	if ( objPAMM->Memory ) {
		printf("\tMemory : %p\t\t\t\tpass! √\n", objPAMM->Memory);
	} else {
		printf("\tMemory : %p\t\t\t\tfail! ×\n", objPAMM->Memory);
	}
	printf("\tCount : %d\t\t\t\t=> 20\n", objPAMM->Count);
	printf("\tAllocCount : %d\t\t\t=> 256\n", objPAMM->AllocCount);
	
	printf("\nPAMM values : \n");
	printf("\tidx\tptr\t\t\tval\texpected val\n");
	void* t2val[20] = { (void*)10001, (void*)1001, (void*)20002, (void*)2002, (void*)30003, (void*)3003, (void*)40004, (void*)4004, (void*)50005, (void*)5005, (void*)60006, (void*)6006, (void*)70007, (void*)7007, (void*)80008, (void*)8008, (void*)90009, (void*)9009, (void*)100010, (void*)10010 };
	for ( int i = 1; i <= objPAMM->Count; i++ ) {
		void* val = PAMM_GetVal_Unsafe(objPAMM, i);
		void* ptr = PAMM_GetPtr_Unsafe(objPAMM, i);
		if ( val == t2val[i - 1] ) {
			printf("\t%d\t%p\t%d\t%d\t\tsucc! √\n", i, ptr, val, t2val[i - 1]);
		} else {
			printf("\t%d\t%p\t%d\t%d\t\tfail! ×\n", i, ptr, val, t2val[i - 1]);
		}
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 4 : remove item
	printf("PAMM test subject 4 : remove item\n\n");
	if ( PAMM_Remove(objPAMM, 17, 1) ) {
		printf("PAMM_Remove idx : 17 (90009)\t\t\t\t\tpass! √\n");
	} else {
		printf("PAMM_Remove idx : 17 (90009)\t\t\t\t\tfail! ×\n");
	}
	if ( PAMM_Remove(objPAMM, 14, 1) ) {
		printf("PAMM_Remove idx : 14 (7007)\t\t\t\t\tpass! √\n");
	} else {
		printf("PAMM_Remove idx : 14 (7007)\t\t\t\t\tfail! ×\n");
	}
	if ( PAMM_Remove(objPAMM, 11, 1) ) {
		printf("PAMM_Remove idx : 11 (60006)\t\t\t\t\tpass! √\n");
	} else {
		printf("PAMM_Remove idx : 11 (60006)\t\t\t\t\tfail! ×\n");
	}
	if ( PAMM_Remove(objPAMM, 8, 1) ) {
		printf("PAMM_Remove idx : 8 (4004)\t\t\t\t\tpass! √\n");
	} else {
		printf("PAMM_Remove idx : 8 (4004)\t\t\t\t\tfail! ×\n");
	}
	if ( PAMM_Remove(objPAMM, 5, 1) ) {
		printf("PAMM_Remove idx : 5 (30003)\t\t\t\t\tpass! √\n");
	} else {
		printf("PAMM_Remove idx : 5 (30003)\t\t\t\t\tfail! ×\n");
	}
	for ( int i = 0; i < 10; i++ ) {
		unsigned int idx = i + 1;
		idx = PAMM_Append(objPAMM, (void*)(intptr_t)(idx * 100 + idx));
		if ( idx ) {
			printf("PAMM_Append idx : %d\t\t\t\t\t\tpass! √\n", idx);
		} else {
			printf("PAMM_Append idx : %d\t\t\t\t\t\tfail! ×\n", idx);
		}
	}
	
	printf("\nPAMM state : \n");
	if ( objPAMM->Memory ) {
		printf("\tMemory : %p\t\t\t\tpass! √\n", objPAMM->Memory);
	} else {
		printf("\tMemory : %p\t\t\t\tfail! ×\n", objPAMM->Memory);
	}
	printf("\tCount : %d\t\t\t\t=> 25\n", objPAMM->Count);
	printf("\tAllocCount : %d\t\t\t=> 256\n", objPAMM->AllocCount);
	
	printf("\nPAMM values : \n");
	printf("\tidx\tptr\t\t\tval\texpected val\n");
	void* t3val[25] = { (void*)10001, (void*)1001, (void*)20002, (void*)2002, (void*)3003, (void*)40004, (void*)50005, (void*)5005, (void*)6006, (void*)70007, (void*)80008, (void*)8008, (void*)9009, (void*)100010, (void*)10010, (void*)101, (void*)202, (void*)303, (void*)404, (void*)505, (void*)606, (void*)707, (void*)808, (void*)909, (void*)1010 };
	for ( int i = 1; i <= objPAMM->Count; i++ ) {
		void* val = PAMM_GetVal_Unsafe(objPAMM, i);
		void* ptr = PAMM_GetPtr_Unsafe(objPAMM, i);
		if ( val == t3val[i - 1] ) {
			printf("\t%d\t%p\t%d\t%d\t\tsucc! √\n", i, ptr, val, t3val[i - 1]);
		} else {
			printf("\t%d\t%p\t%d\t%d\t\tfail! ×\n", i, ptr, val, t3val[i - 1]);
		}
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 5 : swap item
	printf("PAMM test subject 5 : swap item\n\n");
	if ( PAMM_Swap(objPAMM, 3, 13) ) {
		printf("PAMM_Swap 3 - 13 (20002 - 9009)\t\t\t\t\tpass! √\n");
	} else {
		printf("PAMM_Swap 3 - 13 (20002 - 9009)\t\t\t\t\tfail! ×\n");
	}
	if ( PAMM_Swap(objPAMM, 5, 15) ) {
		printf("PAMM_Swap 5 - 15 (3003 - 10010)\t\t\t\t\tpass! √\n");
	} else {
		printf("PAMM_Swap 5 - 15 (3003 - 10010)\t\t\t\t\tfail! ×\n");
	}
	if ( PAMM_Swap(objPAMM, 7, 17) ) {
		printf("PAMM_Swap 7 - 17 (50005 - 202)\t\t\t\t\tpass! √\n");
	} else {
		printf("PAMM_Swap 7 - 17 (50005 - 202)\t\t\t\t\tfail! ×\n");
	}
	
	printf("\nPAMM state : \n");
	if ( objPAMM->Memory ) {
		printf("\tMemory : %p\t\t\t\tpass! √\n", objPAMM->Memory);
	} else {
		printf("\tMemory : %p\t\t\t\tfail! ×\n", objPAMM->Memory);
	}
	printf("\tCount : %d\t\t\t\t=> 25\n", objPAMM->Count);
	printf("\tAllocCount : %d\t\t\t=> 256\n", objPAMM->AllocCount);
	
	printf("\nPAMM values : \n");
	printf("\tidx\tptr\t\t\tval\texpected val\n");
	void* t4val[25] = { (void*)10001, (void*)1001, (void*)9009, (void*)2002, (void*)10010, (void*)40004, (void*)202, (void*)5005, (void*)6006, (void*)70007, (void*)80008, (void*)8008, (void*)20002, (void*)100010, (void*)3003, (void*)101, (void*)50005, (void*)303, (void*)404, (void*)505, (void*)606, (void*)707, (void*)808, (void*)909, (void*)1010 };
	for ( int i = 1; i <= objPAMM->Count; i++ ) {
		void* val = PAMM_GetVal_Unsafe(objPAMM, i);
		void* ptr = PAMM_GetPtr_Unsafe(objPAMM, i);
		if ( val == t4val[i - 1] ) {
			printf("\t%d\t%p\t%d\t%d\t\tsucc! √\n", i, ptr, val, t4val[i - 1]);
		} else {
			printf("\t%d\t%p\t%d\t%d\t\tfail! ×\n", i, ptr, val, t4val[i - 1]);
		}
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 6 : malloc
	printf("PAMM test subject 6 : malloc\n\n");
	for ( int i = 0; i < 260; i++ ) {
		unsigned int idx = PAMM_Append(objPAMM, (void*)(intptr_t)i);
		if ( idx ) {
			printf("PAMM_Append idx : %d\t\t\t\t\t\tpass! √\n", idx);
		} else {
			printf("PAMM_Append idx : %d\t\t\t\t\t\tfail! ×\n", idx);
		}
	}
	
	printf("\nPAMM state : \n");
	if ( objPAMM->Memory ) {
		printf("\tMemory : %p\t\t\t\tpass! √\n", objPAMM->Memory);
	} else {
		printf("\tMemory : %p\t\t\t\tfail! ×\n", objPAMM->Memory);
	}
	printf("\tCount : %d\t\t\t\t=> 285\n", objPAMM->Count);
	printf("\tAllocCount : %d\t\t\t=> 512\n", objPAMM->AllocCount);
	
	printf("\nPAMM values : \n");
	printf("\tidx\tptr\t\t\tval\texpected val\n");
	void* t5val[29] = { (void*)10001, (void*)80008, (void*)606, (void*)5, (void*)15, (void*)25, (void*)35, (void*)45, (void*)55, (void*)65, (void*)75, (void*)85, (void*)95, (void*)105, (void*)115, (void*)125, (void*)135, (void*)145, (void*)155, (void*)165, (void*)175, (void*)185, (void*)195, (void*)205, (void*)215, (void*)225, (void*)235, (void*)245, (void*)255 };
	int pos = 0;
	for ( int i = 1; i <= objPAMM->Count; i+=10 ) {
		void* val = PAMM_GetVal_Unsafe(objPAMM, i);
		void* ptr = PAMM_GetPtr_Unsafe(objPAMM, i);
		if ( val == t5val[pos] ) {
			printf("\t%d\t%p\t%d\t%d\t\tsucc! √\n", i, ptr, val, t5val[pos]);
		} else {
			printf("\t%d\t%p\t%d\t%d\t\tfail! ×\n", i, ptr, val, t5val[pos]);
		}
		pos++;
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 7 : sort
	printf("PAMM test subject 7 : sort\n\n");
	PAMM_Sort(objPAMM, PAMM_TestSortProc);
	
	printf("\nPAMM state : \n");
	if ( objPAMM->Memory ) {
		printf("\tMemory : %p\t\t\t\tpass! √\n", objPAMM->Memory);
	} else {
		printf("\tMemory : %p\t\t\t\tfail! ×\n", objPAMM->Memory);
	}
	printf("\tCount : %d\t\t\t\t=> 285\n", objPAMM->Count);
	printf("\tAllocCount : %d\t\t\t=> 512\n", objPAMM->AllocCount);
	
	printf("\nPAMM values : \n");
	printf("\tidx\tptr\t\t\tval\texpected val\n");
	void* t6val[29] = { (void*)0, (void*)10, (void*)20, (void*)30, (void*)40, (void*)50, (void*)60, (void*)70, (void*)80, (void*)90, (void*)100, (void*)109, (void*)119, (void*)129, (void*)139, (void*)149, (void*)159, (void*)169, (void*)179, (void*)189, (void*)199, (void*)208, (void*)218, (void*)228, (void*)238, (void*)248, (void*)258, (void*)1010, (void*)40004 };
	pos = 0;
	for ( int i = 1; i <= objPAMM->Count; i+=10 ) {
		void* val = PAMM_GetVal_Unsafe(objPAMM, i);
		void* ptr = PAMM_GetPtr_Unsafe(objPAMM, i);
		if ( val == t6val[pos] ) {
			printf("\t%d\t%p\t%d\t%d\t\tsucc! √\n", i, ptr, val, t6val[pos]);
		} else {
			printf("\t%d\t%p\t%d\t%d\t\tfail! ×\n", i, ptr, val, t6val[pos]);
		}
		pos++;
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject X : struct unit & destroy
	printf("PAMM test subject X : struct unit & destroy\n\n");
	PAMM_Unit(objPAMM);
	printf("PAMM object (%p) already unit!\n\n", objPAMM);
	
	printf("PAMM state (object) : \n");
	if ( objPAMM->Memory ) {
		printf("\tMemory : %p\t\t\t\tfail! ×\n", objPAMM->Memory);
	} else {
		printf("\tMemory : %p\t\t\t\tpass! √\n", objPAMM->Memory);
	}
	printf("\tCount : %d\t\t\t\t=> 0\n", objPAMM->Count);
	printf("\tAllocCount : %d\t\t\t\t=> 0\n", objPAMM->AllocCount);
	
	PAMM_Destroy(objPAMM);
	printf("\nPAMM object (%p) already destroyed!\n", objPAMM);
	
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	MMU_Unit();
}


