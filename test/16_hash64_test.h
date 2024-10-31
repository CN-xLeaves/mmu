


void test_hash64()
{
	MMU_Init();
	
	unsigned long long iHash = Hash64_WithSeed("hashtablekey", 12, HASH64_SEED);
	printf("hash value : hashtablekey = %llx\n", iHash);
	iHash = Hash64_WithSeed("xywhsoft", 8, HASH64_SEED);
	printf("hash value : xywhsoft = %llx\n", iHash);
	iHash = Hash64_WithSeed("abcdefghijklmnopqrstuvwxyz", 8, HASH64_SEED);
	printf("hash value : abcdefghijklmnopqrstuvwxyz = %llx\n", iHash);
	system("pause");
	
	MMU_Unit();
}


