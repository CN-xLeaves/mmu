


void mp64k_error_proc(MP256_Object objMM, int ErrorID)
{
	printf("!!! Error !!! error code : %d\n", ErrorID);
}



// print Tree
int iHeigthMP64k = 1;
void mp64k_treeprint(FSB64K_Item* pRoot)
{
	for ( int i = 0; i < iHeigthMP64k; i++ ) {
		printf("    ");
	}
	if ( pRoot ) {
		printf("Range : %d - %d\n", pRoot->MinLength, pRoot->MaxLength);
		iHeigthMP64k++;
		mp64k_treeprint(pRoot->left);
		mp64k_treeprint(pRoot->right);
		iHeigthMP64k--;
	} else {
		printf("null\n");
	}
}



void test_mp64k()
{
	system("cls");
	MMU_Init();
	
	
	
	// subject 1 : print memory pool small block plan tree
	printf("MP64K test subject 1 : print memory pool small block plan tree\n\n");
	MP64K_Object objMP = MP64K_Create(1);
	
	printf("\nMP64K state : \n");
	printf("\tRootNode : %p\t\t=> not 0\n", objMP->FSB_RootNode);
	if ( objMP->FSB_Memory ) {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tpass! √\n", objMP->FSB_Memory);
	} else {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tfail! ×\n", objMP->FSB_Memory);
	}
	
	printf("\nTree print : \n");
	mp64k_treeprint(objMP->FSB_RootNode);
	MP64K_Destroy(objMP);
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 2 : print memory pool normal block plan tree
	printf("MP64K test subject 2 : print memory pool normal block plan tree\n\n");
	objMP = MP64K_Create(2);
	
	printf("\nMP64K state : \n");
	printf("\tRootNode : %p\t\t=> not 0\n", objMP->FSB_RootNode);
	if ( objMP->FSB_Memory ) {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tpass! √\n", objMP->FSB_Memory);
	} else {
		printf("\tMM.arrMMU.PageMMU.Memory : %p\t\tfail! ×\n", objMP->FSB_Memory);
	}
	
	printf("\nTree print : \n");
	mp64k_treeprint(objMP->FSB_RootNode);
	MP64K_Destroy(objMP);
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 3 : create object
	printf("MP64K test subject 3 : create object\n\n");
	objMP = MP64K_Create(2);
	if ( objMP ) {
		objMP->OnError = (void*)mp64k_error_proc;
		printf("MP64K object : %p\t\t\t\t\tpass! √\n", objMP);
		printf("\tarrMMU.Count : %d\t\t\t=> 0\n", objMP->arrMMU.Count);
		printf("\tBigMM.Count : %d\t\t\t\t=> 0\n", objMP->BigMM.Count);
	} else {
		printf("MP64K object : %p\t\t\t\t\tfail! ×\n", objMP);
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 4 : alloc mempry
	printf("MP64K test subject 4 : alloc mempry\n\n");
	void* ptr[64];
	int arrLen[64] = { 1, 16, 17, 32, 33, 48, 49, 64, 65, 80, 81, 96, 97, 128, 129, 160, 161, 192, 193, 224, 225, 256, 257, 320, 321, 384, 385, 448, 449, 512, 513, 640, 641, 768, 769, 896, 897, 1024, 1025, 1280, 1281, 1536, 1537, 1792, 1793, 2048, 2049, 2304, 2305, 2560, 2561, 2816, 2817, 3072, 3073, 3328, 3329, 3584, 3585, 3840, 3841, 4096, 4097, 8192 };
	for ( int i = 0; i < 64; i++ ) {
		ptr[i] = MP64K_Alloc(objMP, arrLen[i]);
		printf("malloc : %p (len : %d, idx : %d)\n", ptr[i], arrLen[i], i);
	}
	
	printf("\nMP64K state : \n");
	printf("\tarrMMU.Count : %d\t\t\t=> 31\n", objMP->arrMMU.Count);
	printf("\tBigMM.Count : %d\t\t\t\t=> 2\n", objMP->BigMM.Count);
	
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 5 : free mempry
	printf("MP64K test subject 5 : free mempry\n\n");
	for ( int i = 0; i < 64; i++ ) {
		printf("free : %p (len : %d, idx : %d)\n", ptr[i], arrLen[i], i);
		MP64K_Free(objMP, ptr[i]);
	}
	
	printf("\nMP64K state : \n");
	printf("\tarrMMU.Count : %d\t\t\t=> 31\n", objMP->arrMMU.Count);
	printf("\tBigMM.Count : %d\t\t\t\t=> 2\n", objMP->BigMM.Count);
	
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject X : struct unit & destroy
	printf("MP64K test subject X : struct unit & destroy\n\n");
	MP64K_Unit(objMP);
	printf("MP64K object (%p) already unit!\n\n", objMP);
	
	printf("\nMP64K state : \n");
	printf("\tarrMMU.Count : %d\t\t\t=> 31\n", objMP->arrMMU.Count);
	printf("\tBigMM.Count : %d\t\t\t\t=> 2\n", objMP->BigMM.Count);
	
	MP64K_Destroy(objMP);
	printf("\nMP64K object (%p) already destroyed!\n", objMP);
	
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	MMU_Unit();
}


