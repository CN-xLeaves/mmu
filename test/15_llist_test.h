


// test LLIST struct
typedef struct {
	LList_NodeBase Base;
	int iVal;
	char sVal[32];
} LList_Test_Struct, *LList_Test_Object;



void test_llist()
{
	system("cls");
	MMU_Init();
	
	
	
	// subject 1 : create object
	printf("LList test subject 1 : create object\n\n");
	LList_Object objLL = LList_Create(sizeof(LList_Test_Struct));
	if ( objLL ) {
		printf("LList object : %p\t\t\t\t\tpass! √\n", objLL);
		printf("\tCount : %d\t\t\t\t=> 0\n", objLL->Count);
		printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objLL->objMM.ItemLength, sizeof(LList_Test_Struct));
		printf("\tMM.arrMMU.ItemLength : %d\t\t=> %d\n", objLL->objMM.arrMMU.ItemLength, sizeof(MMU256_LLNode));
		printf("\tMM.arrMMU.Count : %d\t\t\t=> 0\n", objLL->objMM.arrMMU.Count);
		printf("\tMM.arrMMU.PageMMU.Count : %d\t\t=> 0\n", objLL->objMM.arrMMU.PageMMU.Count);
		printf("\tMM.arrMMU.PageMMU.AllocCount : %d\t=> 0\n", objLL->objMM.arrMMU.PageMMU.AllocCount);
		printf("\tMM.arrMMU.PageMMU.AllocStep : %d\t=> 64\n", objLL->objMM.arrMMU.PageMMU.AllocStep);
		if ( objLL->objMM.arrMMU.PageMMU.Memory ) {
			printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tfail! ×\n", objLL->objMM.arrMMU.PageMMU.Memory);
		} else {
			printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tpass! √\n", objLL->objMM.arrMMU.PageMMU.Memory);
		}
	} else {
		printf("LList object : %p\t\t\t\t\tfail! ×\n", objLL);
	}
	
	printf("\nLList values (first to last) : \n");
	LList_Test_Object objItem = (LList_Test_Object)objLL->FirstNode;
	int idx = 1;
	printf("\tidx\tptr\t\t\tiVal\tsVal\n");
	while ( objItem ) {
		printf("\t%d\t%p\t%d\t%s\n", idx, objItem, objItem->iVal, objItem->sVal);
		objItem = (LList_Test_Object)objItem->Base.Next;
		idx++;
	}
	
	printf("\nLList values (last to first) : \n");
	objItem = (LList_Test_Object)objLL->LastNode;
	idx = 1;
	printf("\tidx\tptr\t\t\tiVal\tsVal\n");
	while ( objItem ) {
		printf("\t%d\t%p\t%d\t%s\n", idx, objItem, objItem->iVal, objItem->sVal);
		objItem = (LList_Test_Object)objItem->Base.Prev;
		idx++;
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 2 : insert last item
	printf("LList test subject 2 : insert last item\n\n");
	for ( int i = 0; i < 5; i++ ) {
		objItem = (LList_Test_Object)LList_InsertNext(objLL, NULL);
		objItem->iVal = i + 1;
		sprintf(objItem->sVal, "insert last idx : %d", objItem->iVal);
		printf("LList insert last item : %d (%s)\n", objItem->iVal, objItem->sVal);
	}
	
	printf("\nLList state : \n");
	printf("\tCount : %d\t\t\t\t=> 5\n", objLL->Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objLL->objMM.ItemLength, sizeof(LList_Test_Struct));
	printf("\tMM.arrMMU.ItemLength : %d\t\t=> %d\n", objLL->objMM.arrMMU.ItemLength, sizeof(MMU256_LLNode));
	printf("\tMM.arrMMU.Count : %d\t\t\t=> 1\n", objLL->objMM.arrMMU.Count);
	printf("\tMM.arrMMU.PageMMU.Count : %d\t\t=> 1\n", objLL->objMM.arrMMU.PageMMU.Count);
	printf("\tMM.arrMMU.PageMMU.AllocCount : %d\t=> 64\n", objLL->objMM.arrMMU.PageMMU.AllocCount);
	printf("\tMM.arrMMU.PageMMU.AllocStep : %d\t=> 64\n", objLL->objMM.arrMMU.PageMMU.AllocStep);
	if ( objLL->objMM.arrMMU.PageMMU.Memory ) {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tpass! √\n", objLL->objMM.arrMMU.PageMMU.Memory);
	} else {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tfail! ×\n", objLL->objMM.arrMMU.PageMMU.Memory);
	}
	
	printf("\nLList values (first to last) : \n");
	objItem = (LList_Test_Object)objLL->FirstNode;
	idx = 1;
	printf("\tidx\tptr\t\t\tiVal\tsVal\n");
	while ( objItem ) {
		printf("\t%d\t%p\t%d\t%s\n", idx, objItem, objItem->iVal, objItem->sVal);
		objItem = (LList_Test_Object)objItem->Base.Next;
		idx++;
	}
	
	printf("\nLList values (last to first) : \n");
	objItem = (LList_Test_Object)objLL->LastNode;
	idx = 1;
	printf("\tidx\tptr\t\t\tiVal\tsVal\n");
	while ( objItem ) {
		printf("\t%d\t%p\t%d\t%s\n", idx, objItem, objItem->iVal, objItem->sVal);
		objItem = (LList_Test_Object)objItem->Base.Prev;
		idx++;
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 3 : insert first item
	printf("LList test subject 3 : insert first item\n\n");
	for ( int i = 0; i < 5; i++ ) {
		objItem = (LList_Test_Object)LList_InsertPrev(objLL, NULL);
		objItem->iVal = (i + 1) * 10;
		sprintf(objItem->sVal, "insert first idx : %d", objItem->iVal);
		printf("LList insert first item : %d (%s)\n", objItem->iVal, objItem->sVal);
	}
	
	printf("\nLList state : \n");
	printf("\tCount : %d\t\t\t\t=> 10\n", objLL->Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objLL->objMM.ItemLength, sizeof(LList_Test_Struct));
	printf("\tMM.arrMMU.ItemLength : %d\t\t=> %d\n", objLL->objMM.arrMMU.ItemLength, sizeof(MMU256_LLNode));
	printf("\tMM.arrMMU.Count : %d\t\t\t=> 1\n", objLL->objMM.arrMMU.Count);
	printf("\tMM.arrMMU.PageMMU.Count : %d\t\t=> 1\n", objLL->objMM.arrMMU.PageMMU.Count);
	printf("\tMM.arrMMU.PageMMU.AllocCount : %d\t=> 64\n", objLL->objMM.arrMMU.PageMMU.AllocCount);
	printf("\tMM.arrMMU.PageMMU.AllocStep : %d\t=> 64\n", objLL->objMM.arrMMU.PageMMU.AllocStep);
	if ( objLL->objMM.arrMMU.PageMMU.Memory ) {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tpass! √\n", objLL->objMM.arrMMU.PageMMU.Memory);
	} else {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tfail! ×\n", objLL->objMM.arrMMU.PageMMU.Memory);
	}
	
	printf("\nLList values (first to last) : \n");
	objItem = (LList_Test_Object)objLL->FirstNode;
	idx = 1;
	printf("\tidx\tptr\t\t\tiVal\tsVal\n");
	while ( objItem ) {
		printf("\t%d\t%p\t%d\t%s\n", idx, objItem, objItem->iVal, objItem->sVal);
		objItem = (LList_Test_Object)objItem->Base.Next;
		idx++;
	}
	
	printf("\nLList values (last to first) : \n");
	objItem = (LList_Test_Object)objLL->LastNode;
	idx = 1;
	printf("\tidx\tptr\t\t\tiVal\tsVal\n");
	while ( objItem ) {
		printf("\t%d\t%p\t%d\t%s\n", idx, objItem, objItem->iVal, objItem->sVal);
		objItem = (LList_Test_Object)objItem->Base.Prev;
		idx++;
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 4 : insert prev item
	printf("LList test subject 4 : insert prev item\n\n");
	objItem = (LList_Test_Object)objLL->FirstNode;
	for ( int i = 0; i < 3; i++ ) {
		objItem = (LList_Test_Object)objItem->Base.Next;
	}
	for ( int i = 0; i < 5; i++ ) {
		objItem = (LList_Test_Object)LList_InsertPrev(objLL, (void*)objItem);
		objItem->iVal = (i + 1) * 100;
		sprintf(objItem->sVal, "insert prev idx : %d", objItem->iVal);
		printf("LList insert prev item : %d (%s)\n", objItem->iVal, objItem->sVal);
	}
	
	printf("\nLList state : \n");
	printf("\tCount : %d\t\t\t\t=> 15\n", objLL->Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objLL->objMM.ItemLength, sizeof(LList_Test_Struct));
	printf("\tMM.arrMMU.ItemLength : %d\t\t=> %d\n", objLL->objMM.arrMMU.ItemLength, sizeof(MMU256_LLNode));
	printf("\tMM.arrMMU.Count : %d\t\t\t=> 1\n", objLL->objMM.arrMMU.Count);
	printf("\tMM.arrMMU.PageMMU.Count : %d\t\t=> 1\n", objLL->objMM.arrMMU.PageMMU.Count);
	printf("\tMM.arrMMU.PageMMU.AllocCount : %d\t=> 64\n", objLL->objMM.arrMMU.PageMMU.AllocCount);
	printf("\tMM.arrMMU.PageMMU.AllocStep : %d\t=> 64\n", objLL->objMM.arrMMU.PageMMU.AllocStep);
	if ( objLL->objMM.arrMMU.PageMMU.Memory ) {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tpass! √\n", objLL->objMM.arrMMU.PageMMU.Memory);
	} else {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tfail! ×\n", objLL->objMM.arrMMU.PageMMU.Memory);
	}
	
	printf("\nLList values (first to last) : \n");
	objItem = (LList_Test_Object)objLL->FirstNode;
	idx = 1;
	printf("\tidx\tptr\t\t\tiVal\tsVal\n");
	while ( objItem ) {
		printf("\t%d\t%p\t%d\t%s\n", idx, objItem, objItem->iVal, objItem->sVal);
		objItem = (LList_Test_Object)objItem->Base.Next;
		idx++;
	}
	
	printf("\nLList values (last to first) : \n");
	objItem = (LList_Test_Object)objLL->LastNode;
	idx = 1;
	printf("\tidx\tptr\t\t\tiVal\tsVal\n");
	while ( objItem ) {
		printf("\t%d\t%p\t%d\t%s\n", idx, objItem, objItem->iVal, objItem->sVal);
		objItem = (LList_Test_Object)objItem->Base.Prev;
		idx++;
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 5 : insert next item
	printf("LList test subject 5 : insert next item\n\n");
	objItem = (LList_Test_Object)objLL->LastNode;
	for ( int i = 0; i < 3; i++ ) {
		objItem = (LList_Test_Object)objItem->Base.Prev;
	}
	for ( int i = 0; i < 5; i++ ) {
		objItem = (LList_Test_Object)LList_InsertNext(objLL, (void*)objItem);
		objItem->iVal = (i + 1) * 1000;
		sprintf(objItem->sVal, "insert next idx : %d", objItem->iVal);
		printf("LList insert next item : %d (%s)\n", objItem->iVal, objItem->sVal);
	}
	
	printf("\nLList state : \n");
	printf("\tCount : %d\t\t\t\t=> 20\n", objLL->Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objLL->objMM.ItemLength, sizeof(LList_Test_Struct));
	printf("\tMM.arrMMU.ItemLength : %d\t\t=> %d\n", objLL->objMM.arrMMU.ItemLength, sizeof(MMU256_LLNode));
	printf("\tMM.arrMMU.Count : %d\t\t\t=> 1\n", objLL->objMM.arrMMU.Count);
	printf("\tMM.arrMMU.PageMMU.Count : %d\t\t=> 1\n", objLL->objMM.arrMMU.PageMMU.Count);
	printf("\tMM.arrMMU.PageMMU.AllocCount : %d\t=> 64\n", objLL->objMM.arrMMU.PageMMU.AllocCount);
	printf("\tMM.arrMMU.PageMMU.AllocStep : %d\t=> 64\n", objLL->objMM.arrMMU.PageMMU.AllocStep);
	if ( objLL->objMM.arrMMU.PageMMU.Memory ) {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tpass! √\n", objLL->objMM.arrMMU.PageMMU.Memory);
	} else {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tfail! ×\n", objLL->objMM.arrMMU.PageMMU.Memory);
	}
	
	printf("\nLList values (first to last) : \n");
	objItem = (LList_Test_Object)objLL->FirstNode;
	idx = 1;
	printf("\tidx\tptr\t\t\tiVal\tsVal\n");
	while ( objItem ) {
		printf("\t%d\t%p\t%d\t%s\n", idx, objItem, objItem->iVal, objItem->sVal);
		objItem = (LList_Test_Object)objItem->Base.Next;
		idx++;
	}
	
	printf("\nLList values (last to first) : \n");
	objItem = (LList_Test_Object)objLL->LastNode;
	idx = 1;
	printf("\tidx\tptr\t\t\tiVal\tsVal\n");
	while ( objItem ) {
		printf("\t%d\t%p\t%d\t%s\n", idx, objItem, objItem->iVal, objItem->sVal);
		objItem = (LList_Test_Object)objItem->Base.Prev;
		idx++;
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 6 : remove last item
	printf("LList test subject 6 : remove last item\n\n");
	for ( int i = 0; i < 2; i++ ) {
		LList_Remove(objLL, objLL->LastNode, FALSE);
		printf("LList remove last node.\n");
	}
	
	printf("\nLList state : \n");
	printf("\tCount : %d\t\t\t\t=> 18\n", objLL->Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objLL->objMM.ItemLength, sizeof(LList_Test_Struct));
	printf("\tMM.arrMMU.ItemLength : %d\t\t=> %d\n", objLL->objMM.arrMMU.ItemLength, sizeof(MMU256_LLNode));
	printf("\tMM.arrMMU.Count : %d\t\t\t=> 1\n", objLL->objMM.arrMMU.Count);
	printf("\tMM.arrMMU.PageMMU.Count : %d\t\t=> 1\n", objLL->objMM.arrMMU.PageMMU.Count);
	printf("\tMM.arrMMU.PageMMU.AllocCount : %d\t=> 64\n", objLL->objMM.arrMMU.PageMMU.AllocCount);
	printf("\tMM.arrMMU.PageMMU.AllocStep : %d\t=> 64\n", objLL->objMM.arrMMU.PageMMU.AllocStep);
	if ( objLL->objMM.arrMMU.PageMMU.Memory ) {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tpass! √\n", objLL->objMM.arrMMU.PageMMU.Memory);
	} else {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tfail! ×\n", objLL->objMM.arrMMU.PageMMU.Memory);
	}
	
	printf("\nLList values (first to last) : \n");
	objItem = (LList_Test_Object)objLL->FirstNode;
	idx = 1;
	printf("\tidx\tptr\t\t\tiVal\tsVal\n");
	while ( objItem ) {
		printf("\t%d\t%p\t%d\t%s\n", idx, objItem, objItem->iVal, objItem->sVal);
		objItem = (LList_Test_Object)objItem->Base.Next;
		idx++;
	}
	
	printf("\nLList values (last to first) : \n");
	objItem = (LList_Test_Object)objLL->LastNode;
	idx = 1;
	printf("\tidx\tptr\t\t\tiVal\tsVal\n");
	while ( objItem ) {
		printf("\t%d\t%p\t%d\t%s\n", idx, objItem, objItem->iVal, objItem->sVal);
		objItem = (LList_Test_Object)objItem->Base.Prev;
		idx++;
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 7 : remove first item
	printf("LList test subject 7 : remove first item\n\n");
	for ( int i = 0; i < 2; i++ ) {
		LList_Remove(objLL, objLL->FirstNode, FALSE);
		printf("LList remove first node.\n");
	}
	
	printf("\nLList state : \n");
	printf("\tCount : %d\t\t\t\t=> 16\n", objLL->Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objLL->objMM.ItemLength, sizeof(LList_Test_Struct));
	printf("\tMM.arrMMU.ItemLength : %d\t\t=> %d\n", objLL->objMM.arrMMU.ItemLength, sizeof(MMU256_LLNode));
	printf("\tMM.arrMMU.Count : %d\t\t\t=> 1\n", objLL->objMM.arrMMU.Count);
	printf("\tMM.arrMMU.PageMMU.Count : %d\t\t=> 1\n", objLL->objMM.arrMMU.PageMMU.Count);
	printf("\tMM.arrMMU.PageMMU.AllocCount : %d\t=> 64\n", objLL->objMM.arrMMU.PageMMU.AllocCount);
	printf("\tMM.arrMMU.PageMMU.AllocStep : %d\t=> 64\n", objLL->objMM.arrMMU.PageMMU.AllocStep);
	if ( objLL->objMM.arrMMU.PageMMU.Memory ) {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tpass! √\n", objLL->objMM.arrMMU.PageMMU.Memory);
	} else {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tfail! ×\n", objLL->objMM.arrMMU.PageMMU.Memory);
	}
	
	printf("\nLList values (first to last) : \n");
	objItem = (LList_Test_Object)objLL->FirstNode;
	idx = 1;
	printf("\tidx\tptr\t\t\tiVal\tsVal\n");
	while ( objItem ) {
		printf("\t%d\t%p\t%d\t%s\n", idx, objItem, objItem->iVal, objItem->sVal);
		objItem = (LList_Test_Object)objItem->Base.Next;
		idx++;
	}
	
	printf("\nLList values (last to first) : \n");
	objItem = (LList_Test_Object)objLL->LastNode;
	idx = 1;
	printf("\tidx\tptr\t\t\tiVal\tsVal\n");
	while ( objItem ) {
		printf("\t%d\t%p\t%d\t%s\n", idx, objItem, objItem->iVal, objItem->sVal);
		objItem = (LList_Test_Object)objItem->Base.Prev;
		idx++;
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 8 : remove item
	printf("LList test subject 8 : remove item\n\n");
	objItem = (LList_Test_Object)objLL->FirstNode;
	for ( int i = 0; i < 6; i++ ) {
		objItem = (LList_Test_Object)objItem->Base.Next;
	}
	for ( int i = 0; i < 4; i++ ) {
		objItem = (LList_Test_Object)LList_Remove(objLL, (void*)objItem, FALSE);
		printf("LList remove node.\n");
	}
	
	printf("\nLList state : \n");
	printf("\tCount : %d\t\t\t\t=> 12\n", objLL->Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objLL->objMM.ItemLength, sizeof(LList_Test_Struct));
	printf("\tMM.arrMMU.ItemLength : %d\t\t=> %d\n", objLL->objMM.arrMMU.ItemLength, sizeof(MMU256_LLNode));
	printf("\tMM.arrMMU.Count : %d\t\t\t=> 1\n", objLL->objMM.arrMMU.Count);
	printf("\tMM.arrMMU.PageMMU.Count : %d\t\t=> 1\n", objLL->objMM.arrMMU.PageMMU.Count);
	printf("\tMM.arrMMU.PageMMU.AllocCount : %d\t=> 64\n", objLL->objMM.arrMMU.PageMMU.AllocCount);
	printf("\tMM.arrMMU.PageMMU.AllocStep : %d\t=> 64\n", objLL->objMM.arrMMU.PageMMU.AllocStep);
	if ( objLL->objMM.arrMMU.PageMMU.Memory ) {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tpass! √\n", objLL->objMM.arrMMU.PageMMU.Memory);
	} else {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tfail! ×\n", objLL->objMM.arrMMU.PageMMU.Memory);
	}
	
	printf("\nLList values (first to last) : \n");
	objItem = (LList_Test_Object)objLL->FirstNode;
	idx = 1;
	printf("\tidx\tptr\t\t\tiVal\tsVal\n");
	while ( objItem ) {
		printf("\t%d\t%p\t%d\t%s\n", idx, objItem, objItem->iVal, objItem->sVal);
		objItem = (LList_Test_Object)objItem->Base.Next;
		idx++;
	}
	
	printf("\nLList values (last to first) : \n");
	objItem = (LList_Test_Object)objLL->LastNode;
	idx = 1;
	printf("\tidx\tptr\t\t\tiVal\tsVal\n");
	while ( objItem ) {
		printf("\t%d\t%p\t%d\t%s\n", idx, objItem, objItem->iVal, objItem->sVal);
		objItem = (LList_Test_Object)objItem->Base.Prev;
		idx++;
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject X : struct unit & destroy
	printf("LList test subject X : struct unit & destroy\n\n");
	LList_Unit(objLL);
	printf("LList object (%p) already unit!\n\n", objLL);
	
	printf("\nLList state : \n");
	printf("\tCount : %d\t\t\t\t=> 0\n", objLL->Count);
	printf("\tMM.ItemLength : %d\t\t\t=> %d\n", objLL->objMM.ItemLength, sizeof(LList_Test_Struct));
	printf("\tMM.arrMMU.ItemLength : %d\t\t=> %d\n", objLL->objMM.arrMMU.ItemLength, sizeof(MMU256_LLNode));
	printf("\tMM.arrMMU.Count : %d\t\t\t=> 0\n", objLL->objMM.arrMMU.Count);
	printf("\tMM.arrMMU.PageMMU.Count : %d\t\t=> 0\n", objLL->objMM.arrMMU.PageMMU.Count);
	printf("\tMM.arrMMU.PageMMU.AllocCount : %d\t=> 0\n", objLL->objMM.arrMMU.PageMMU.AllocCount);
	printf("\tMM.arrMMU.PageMMU.AllocStep : %d\t=> 64\n", objLL->objMM.arrMMU.PageMMU.AllocStep);
	if ( objLL->objMM.arrMMU.PageMMU.Memory ) {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tfail! ×\n", objLL->objMM.arrMMU.PageMMU.Memory);
	} else {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tpass! √\n", objLL->objMM.arrMMU.PageMMU.Memory);
	}
	
	printf("\nLList values (first to last) : \n");
	objItem = (LList_Test_Object)objLL->FirstNode;
	idx = 1;
	printf("\tidx\tptr\t\t\tiVal\tsVal\n");
	while ( objItem ) {
		printf("\t%d\t%p\t%d\t%s\n", idx, objItem, objItem->iVal, objItem->sVal);
		objItem = (LList_Test_Object)objItem->Base.Next;
		idx++;
	}
	
	printf("\nLList values (last to first) : \n");
	objItem = (LList_Test_Object)objLL->LastNode;
	idx = 1;
	printf("\tidx\tptr\t\t\tiVal\tsVal\n");
	while ( objItem ) {
		printf("\t%d\t%p\t%d\t%s\n", idx, objItem, objItem->iVal, objItem->sVal);
		objItem = (LList_Test_Object)objItem->Base.Prev;
		idx++;
	}
	
	LList_Destroy(objLL);
	printf("\nLList object (%p) already destroyed!\n", objLL);
	
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	MMU_Unit();
}


