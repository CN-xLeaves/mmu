


void test_hash32()
{
	MMU_Init();
	
	unsigned int iHash = Hash32_WithSeed("hashtablekey", 12, HASH32_SEED);
	printf("hash value : hashtablekey = %x\n", iHash);
	iHash = Hash32_WithSeed("xywhsoft", 8, HASH32_SEED);
	printf("hash value : xywhsoft = %x\n", iHash);
	iHash = Hash32_WithSeed("abcdefghijklmnopqrstuvwxyz", 8, HASH32_SEED);
	printf("hash value : abcdefghijklmnopqrstuvwxyz = %x\n", iHash);
	system("pause");
	
	MMU_Unit();
}


