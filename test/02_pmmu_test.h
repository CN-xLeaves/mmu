


int PMMU_TestSortProc(void** p1, void** p2)
{
	return (intptr_t)*p1 - (intptr_t)*p2;
}



void test_pmmu()
{
	system("cls");
	MMU_Init();
	
	
	
	// subject 1 : create object
	printf("PMMU test subject 1 : create object\n\n");
	PMMU_Object objPMMU = PMMU_Create();
	if ( objPMMU ) {
		printf("PMMU object : %p\t\t\t\t\tpass! √\n", objPMMU);
		printf("\tMemory : %p\t\t=> 0\n", objPMMU->Memory);
		printf("\tCount : %d\t\t\t\t=> 0\n", objPMMU->Count);
		printf("\tAllocCount : %d\t\t\t\t=> 0\n", objPMMU->AllocCount);
	} else {
		printf("PMMU object : %p\t\t\t\t\tfail! ×\n", objPMMU);
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 2 : append item
	printf("PMMU test subject 2 : append item\n\n");
	for ( int i = 0; i < 10; i++ ) {
		unsigned int idx = i + 1;
		idx = PMMU_Append(objPMMU, (void*)(intptr_t)(idx * 1000 + idx));
		if ( idx ) {
			printf("PMMU_Append idx : %d\t\t\t\t\t\tpass! √\n", idx);
		} else {
			printf("PMMU_Append idx : %d\t\t\t\t\t\tfail! ×\n", idx);
		}
	}
	
	printf("\nPMMU state : \n");
	if ( objPMMU->Memory ) {
		printf("\tMemory : %p\t\t\t\tpass! √\n", objPMMU->Memory);
	} else {
		printf("\tMemory : %p\t\t\t\tfail! ×\n", objPMMU->Memory);
	}
	printf("\tCount : %d\t\t\t\t=> 10\n", objPMMU->Count);
	printf("\tAllocCount : %d\t\t\t=> 256\n", objPMMU->AllocCount);
	
	printf("\nPMMU values : \n");
	printf("\tidx\tptr\t\t\tval\texpected val\n");
	void* t1val[10] = { (void*)1001, (void*)2002, (void*)3003, (void*)4004, (void*)5005, (void*)6006, (void*)7007, (void*)8008, (void*)9009, (void*)10010 };
	for ( int i = 1; i <= objPMMU->Count; i++ ) {
		void* val = PMMU_GetVal_Unsafe(objPMMU, i);
		void* ptr = PMMU_GetPtr_Unsafe(objPMMU, i);
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
	printf("PMMU test subject 3 : insert item\n\n");
	for ( int i = 0; i < 10; i++ ) {
		unsigned int idx = i + 1;
		idx = PMMU_Insert(objPMMU, i * 2, (void*)(intptr_t)(idx * 10000 + idx));
		if ( idx ) {
			printf("PMMU_Insert idx : %d\t\t\t\t\t\tpass! √\n", idx);
		} else {
			printf("PMMU_Insert idx : %d\t\t\t\t\t\tfail! ×\n", idx);
		}
	}
	
	printf("\nPMMU state : \n");
	if ( objPMMU->Memory ) {
		printf("\tMemory : %p\t\t\t\tpass! √\n", objPMMU->Memory);
	} else {
		printf("\tMemory : %p\t\t\t\tfail! ×\n", objPMMU->Memory);
	}
	printf("\tCount : %d\t\t\t\t=> 20\n", objPMMU->Count);
	printf("\tAllocCount : %d\t\t\t=> 256\n", objPMMU->AllocCount);
	
	printf("\nPMMU values : \n");
	printf("\tidx\tptr\t\t\tval\texpected val\n");
	void* t2val[20] = { (void*)10001, (void*)1001, (void*)20002, (void*)2002, (void*)30003, (void*)3003, (void*)40004, (void*)4004, (void*)50005, (void*)5005, (void*)60006, (void*)6006, (void*)70007, (void*)7007, (void*)80008, (void*)8008, (void*)90009, (void*)9009, (void*)100010, (void*)10010 };
	for ( int i = 1; i <= objPMMU->Count; i++ ) {
		void* val = PMMU_GetVal_Unsafe(objPMMU, i);
		void* ptr = PMMU_GetPtr_Unsafe(objPMMU, i);
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
	printf("PMMU test subject 4 : remove item\n\n");
	if ( PMMU_Remove(objPMMU, 17, 1) ) {
		printf("PMMU_Remove idx : 17 (90009)\t\t\t\t\tpass! √\n");
	} else {
		printf("PMMU_Remove idx : 17 (90009)\t\t\t\t\tfail! ×\n");
	}
	if ( PMMU_Remove(objPMMU, 14, 1) ) {
		printf("PMMU_Remove idx : 14 (7007)\t\t\t\t\tpass! √\n");
	} else {
		printf("PMMU_Remove idx : 14 (7007)\t\t\t\t\tfail! ×\n");
	}
	if ( PMMU_Remove(objPMMU, 11, 1) ) {
		printf("PMMU_Remove idx : 11 (60006)\t\t\t\t\tpass! √\n");
	} else {
		printf("PMMU_Remove idx : 11 (60006)\t\t\t\t\tfail! ×\n");
	}
	if ( PMMU_Remove(objPMMU, 8, 1) ) {
		printf("PMMU_Remove idx : 8 (4004)\t\t\t\t\tpass! √\n");
	} else {
		printf("PMMU_Remove idx : 8 (4004)\t\t\t\t\tfail! ×\n");
	}
	if ( PMMU_Remove(objPMMU, 5, 1) ) {
		printf("PMMU_Remove idx : 5 (30003)\t\t\t\t\tpass! √\n");
	} else {
		printf("PMMU_Remove idx : 5 (30003)\t\t\t\t\tfail! ×\n");
	}
	for ( int i = 0; i < 10; i++ ) {
		unsigned int idx = i + 1;
		idx = PMMU_Append(objPMMU, (void*)(intptr_t)(idx * 100 + idx));
		if ( idx ) {
			printf("PMMU_Append idx : %d\t\t\t\t\t\tpass! √\n", idx);
		} else {
			printf("PMMU_Append idx : %d\t\t\t\t\t\tfail! ×\n", idx);
		}
	}
	
	printf("\nPMMU state : \n");
	if ( objPMMU->Memory ) {
		printf("\tMemory : %p\t\t\t\tpass! √\n", objPMMU->Memory);
	} else {
		printf("\tMemory : %p\t\t\t\tfail! ×\n", objPMMU->Memory);
	}
	printf("\tCount : %d\t\t\t\t=> 25\n", objPMMU->Count);
	printf("\tAllocCount : %d\t\t\t=> 256\n", objPMMU->AllocCount);
	
	printf("\nPMMU values : \n");
	printf("\tidx\tptr\t\t\tval\texpected val\n");
	void* t3val[25] = { (void*)10001, (void*)1001, (void*)20002, (void*)2002, (void*)3003, (void*)40004, (void*)50005, (void*)5005, (void*)6006, (void*)70007, (void*)80008, (void*)8008, (void*)9009, (void*)100010, (void*)10010, (void*)101, (void*)202, (void*)303, (void*)404, (void*)505, (void*)606, (void*)707, (void*)808, (void*)909, (void*)1010 };
	for ( int i = 1; i <= objPMMU->Count; i++ ) {
		void* val = PMMU_GetVal_Unsafe(objPMMU, i);
		void* ptr = PMMU_GetPtr_Unsafe(objPMMU, i);
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
	printf("PMMU test subject 5 : swap item\n\n");
	if ( PMMU_Swap(objPMMU, 3, 13) ) {
		printf("PMMU_Swap 3 - 13 (20002 - 9009)\t\t\t\t\tpass! √\n");
	} else {
		printf("PMMU_Swap 3 - 13 (20002 - 9009)\t\t\t\t\tfail! ×\n");
	}
	if ( PMMU_Swap(objPMMU, 5, 15) ) {
		printf("PMMU_Swap 5 - 15 (3003 - 10010)\t\t\t\t\tpass! √\n");
	} else {
		printf("PMMU_Swap 5 - 15 (3003 - 10010)\t\t\t\t\tfail! ×\n");
	}
	if ( PMMU_Swap(objPMMU, 7, 17) ) {
		printf("PMMU_Swap 7 - 17 (50005 - 202)\t\t\t\t\tpass! √\n");
	} else {
		printf("PMMU_Swap 7 - 17 (50005 - 202)\t\t\t\t\tfail! ×\n");
	}
	
	printf("\nPMMU state : \n");
	if ( objPMMU->Memory ) {
		printf("\tMemory : %p\t\t\t\tpass! √\n", objPMMU->Memory);
	} else {
		printf("\tMemory : %p\t\t\t\tfail! ×\n", objPMMU->Memory);
	}
	printf("\tCount : %d\t\t\t\t=> 25\n", objPMMU->Count);
	printf("\tAllocCount : %d\t\t\t=> 256\n", objPMMU->AllocCount);
	
	printf("\nPMMU values : \n");
	printf("\tidx\tptr\t\t\tval\texpected val\n");
	void* t4val[25] = { (void*)10001, (void*)1001, (void*)9009, (void*)2002, (void*)10010, (void*)40004, (void*)202, (void*)5005, (void*)6006, (void*)70007, (void*)80008, (void*)8008, (void*)20002, (void*)100010, (void*)3003, (void*)101, (void*)50005, (void*)303, (void*)404, (void*)505, (void*)606, (void*)707, (void*)808, (void*)909, (void*)1010 };
	for ( int i = 1; i <= objPMMU->Count; i++ ) {
		void* val = PMMU_GetVal_Unsafe(objPMMU, i);
		void* ptr = PMMU_GetPtr_Unsafe(objPMMU, i);
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
	printf("PMMU test subject 6 : malloc\n\n");
	for ( int i = 0; i < 260; i++ ) {
		unsigned int idx = PMMU_Append(objPMMU, (void*)(intptr_t)i);
		if ( idx ) {
			printf("PMMU_Append idx : %d\t\t\t\t\t\tpass! √\n", idx);
		} else {
			printf("PMMU_Append idx : %d\t\t\t\t\t\tfail! ×\n", idx);
		}
	}
	
	printf("\nPMMU state : \n");
	if ( objPMMU->Memory ) {
		printf("\tMemory : %p\t\t\t\tpass! √\n", objPMMU->Memory);
	} else {
		printf("\tMemory : %p\t\t\t\tfail! ×\n", objPMMU->Memory);
	}
	printf("\tCount : %d\t\t\t\t=> 285\n", objPMMU->Count);
	printf("\tAllocCount : %d\t\t\t=> 512\n", objPMMU->AllocCount);
	
	printf("\nPMMU values : \n");
	printf("\tidx\tptr\t\t\tval\texpected val\n");
	void* t5val[29] = { (void*)10001, (void*)80008, (void*)606, (void*)5, (void*)15, (void*)25, (void*)35, (void*)45, (void*)55, (void*)65, (void*)75, (void*)85, (void*)95, (void*)105, (void*)115, (void*)125, (void*)135, (void*)145, (void*)155, (void*)165, (void*)175, (void*)185, (void*)195, (void*)205, (void*)215, (void*)225, (void*)235, (void*)245, (void*)255 };
	int pos = 0;
	for ( int i = 1; i <= objPMMU->Count; i+=10 ) {
		void* val = PMMU_GetVal_Unsafe(objPMMU, i);
		void* ptr = PMMU_GetPtr_Unsafe(objPMMU, i);
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
	printf("PMMU test subject 7 : sort\n\n");
	PMMU_Sort(objPMMU, PMMU_TestSortProc);
	
	printf("\nPMMU state : \n");
	if ( objPMMU->Memory ) {
		printf("\tMemory : %p\t\t\t\tpass! √\n", objPMMU->Memory);
	} else {
		printf("\tMemory : %p\t\t\t\tfail! ×\n", objPMMU->Memory);
	}
	printf("\tCount : %d\t\t\t\t=> 285\n", objPMMU->Count);
	printf("\tAllocCount : %d\t\t\t=> 512\n", objPMMU->AllocCount);
	
	printf("\nPMMU values : \n");
	printf("\tidx\tptr\t\t\tval\texpected val\n");
	void* t6val[29] = { (void*)0, (void*)10, (void*)20, (void*)30, (void*)40, (void*)50, (void*)60, (void*)70, (void*)80, (void*)90, (void*)100, (void*)109, (void*)119, (void*)129, (void*)139, (void*)149, (void*)159, (void*)169, (void*)179, (void*)189, (void*)199, (void*)208, (void*)218, (void*)228, (void*)238, (void*)248, (void*)258, (void*)1010, (void*)40004 };
	pos = 0;
	for ( int i = 1; i <= objPMMU->Count; i+=10 ) {
		void* val = PMMU_GetVal_Unsafe(objPMMU, i);
		void* ptr = PMMU_GetPtr_Unsafe(objPMMU, i);
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
	
	
	
	// subject 8 : struct unit & destroy
	printf("PMMU test subject 8 : struct unit & destroy\n\n");
	PMMU_Unit(objPMMU);
	printf("PMMU object (%p) already unit!\n\n", objPMMU);
	
	printf("PMMU state (object) : \n");
	if ( objPMMU->Memory ) {
		printf("\tMemory : %p\t\t\t\tfail! ×\n", objPMMU->Memory);
	} else {
		printf("\tMemory : %p\t\t\t\tpass! √\n", objPMMU->Memory);
	}
	printf("\tCount : %d\t\t\t\t=> 0\n", objPMMU->Count);
	printf("\tAllocCount : %d\t\t\t\t=> 0\n", objPMMU->AllocCount);
	
	PMMU_Destroy(objPMMU);
	printf("\nPMMU object (%p) already destroyed!\n", objPMMU);
	
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	MMU_Unit();
}


