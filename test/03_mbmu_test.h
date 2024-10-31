


void test_mbmu()
{
	system("cls");
	MMU_Init();
	
	
	
	// subject 1 : create object
	printf("MBMU test subject 1 : create object\n\n");
	MBMU_Object objMBMU = MBMU_Create(5, 10);
	if ( objMBMU ) {
		printf("MBMU object : %p\t\t\t\t\tpass! √\n", objMBMU);
		if ( objMBMU->Buffer ) {
			printf("\tBuffer : %p\t\t\t\tpass! √\n", objMBMU->Buffer);
		} else {
			printf("\tBuffer : %p\t\t\t\tfail! ×\n", objMBMU->Buffer);
		}
		printf("\tLength : %d\t\t\t\t=> 0\n", objMBMU->Length);
		printf("\tAllocLength : %d\t\t\t\t=> 5\n", objMBMU->AllocLength);
		printf("\tAllocStep : %d\t\t\t\t=> 10\n", objMBMU->AllocStep);
	} else {
		printf("MBMU object : %p\t\t\t\t\tfail! ×\n", objMBMU);
	}
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 2 : append string
	printf("MBMU test subject 2 : append string\n\n");
	MBMU_Append(objMBMU, "012345", 4, MBMU_UTF8);
	
	printf("\nMBMU state : \n");
	if ( objMBMU->Buffer ) {
		printf("\tBuffer : %p\t\t\t\tpass! √\n", objMBMU->Buffer);
	} else {
		printf("\tBuffer : %p\t\t\t\tfail! ×\n", objMBMU->Buffer);
	}
	printf("\tLength : %d\t\t\t\t=> 4\n", objMBMU->Length);
	printf("\tAllocLength : %d\t\t\t\t=> 5\n", objMBMU->AllocLength);
	printf("\tAllocStep : %d\t\t\t\t=> 10\n", objMBMU->AllocStep);
	
	printf("\nMBMU value : \n");
	printf("\t%s\n", objMBMU->Buffer);
	
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 3 : append string (auto length) & malloc
	printf("MBMU test subject 3 : append string (auto length) & malloc\n\n");
	MBMU_Append(objMBMU, "456789", 0, MBMU_UTF8);
	
	printf("\nMBMU state : \n");
	if ( objMBMU->Buffer ) {
		printf("\tBuffer : %p\t\t\t\tpass! √\n", objMBMU->Buffer);
	} else {
		printf("\tBuffer : %p\t\t\t\tfail! ×\n", objMBMU->Buffer);
	}
	printf("\tLength : %d\t\t\t\t=> 10\n", objMBMU->Length);
	printf("\tAllocLength : %d\t\t\t=> 21 (10 + 10 + 1)\n", objMBMU->AllocLength);
	printf("\tAllocStep : %d\t\t\t\t=> 10\n", objMBMU->AllocStep);
	
	printf("\nMBMU value : \n");
	printf("\t%s\n", objMBMU->Buffer);
	
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 4 : insert string (rewrite)
	printf("MBMU test subject 4 : insert string (rewrite)\n\n");
	MBMU_Insert(objMBMU, 0, "9876543210", 0, MBMU_UTF8);
	
	printf("\nMBMU state : \n");
	if ( objMBMU->Buffer ) {
		printf("\tBuffer : %p\t\t\t\tpass! √\n", objMBMU->Buffer);
	} else {
		printf("\tBuffer : %p\t\t\t\tfail! ×\n", objMBMU->Buffer);
	}
	printf("\tLength : %d\t\t\t\t=> 10\n", objMBMU->Length);
	printf("\tAllocLength : %d\t\t\t=> 21 (10 + 10 + 1)\n", objMBMU->AllocLength);
	printf("\tAllocStep : %d\t\t\t\t=> 10\n", objMBMU->AllocStep);
	
	printf("\nMBMU value : \n");
	printf("\t%s\n", objMBMU->Buffer);
	
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 5 : Generate webpage
	printf("MBMU test subject 5 : Generate webpage\n\n");
	MBMU_Unit(objMBMU);
	MBMU_Append(objMBMU, "<!DOCTYPE html>\n", 0, MBMU_UTF8);
	MBMU_Append(objMBMU, "<html>\n", 0, MBMU_UTF8);
	MBMU_Append(objMBMU, "<head>\n", 0, MBMU_UTF8);
	MBMU_Append(objMBMU, "<meta charset=\"utf-8\">\n", 0, MBMU_UTF8);
	MBMU_Append(objMBMU, "<title>The page was generated on MBMU</title>\n", 0, MBMU_UTF8);
	MBMU_Append(objMBMU, "</head>\n", 0, MBMU_UTF8);
	MBMU_Append(objMBMU, "<body>\n", 0, MBMU_UTF8);
	MBMU_Append(objMBMU, "Hello World!\n", 0, MBMU_UTF8);
	MBMU_Append(objMBMU, "</body>\n", 0, MBMU_UTF8);
	MBMU_Append(objMBMU, "</html>", 0, MBMU_UTF8);
	
	printf("\nMBMU state : \n");
	if ( objMBMU->Buffer ) {
		printf("\tBuffer : %p\t\t\t\tpass! √\n", objMBMU->Buffer);
	} else {
		printf("\tBuffer : %p\t\t\t\tfail! ×\n", objMBMU->Buffer);
	}
	printf("\tLength : %d\t\t\t\t=> 142\n", objMBMU->Length);
	printf("\tAllocLength : %d\t\t\t=> 153\n", objMBMU->AllocLength);
	printf("\tAllocStep : %d\t\t\t\t=> 10\n", objMBMU->AllocStep);
	
	printf("\nMBMU value : \n");
	printf("\t%s\n", objMBMU->Buffer);
	
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	// subject 6 : struct unit & destroy
	printf("MBMU test subject 6 : struct unit & destroy\n\n");
	MBMU_Unit(objMBMU);
	printf("MBMU object (%p) already unit!\n\n", objMBMU);
	
	printf("MBMU state (object) : \n");
	if ( objMBMU->Buffer ) {
		printf("\tBuffer : %p\t\t\t\tfail! ×\n", objMBMU->Buffer);
	} else {
		printf("\tBuffer : %p\t\t\t\tpass! √\n", objMBMU->Buffer);
	}
	printf("\tLength : %d\t\t\t\t=> 0\n", objMBMU->Length);
	printf("\tAllocLength : %d\t\t\t\t=> 0\n", objMBMU->AllocLength);
	printf("\tAllocStep : %d\t\t\t\t=> 10\n", objMBMU->AllocStep);
	
	MBMU_Destroy(objMBMU);
	printf("\nMBMU object (%p) already destroyed!\n", objMBMU);
	
	printf("\n\n\n");
	system("pause");
	system("cls");
	
	
	
	MMU_Unit();
}


