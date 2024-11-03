


#include <stdlib.h>
#include <stdio.h>
#include <time.h>



#ifdef _WIN32
	#include <Windows.h>
#endif



#include "mmu_config.h"
#include "mmu.h"



#include "test/01_pamm_test.h"
#include "test/02_samm_test.h"
#include "test/03_mbmu_test.h"
#include "test/04_bsmm_test.h"
#include "test/05_mmu256_test.h"
#include "test/06_mmu64k_test.h"
#include "test/07_mm256_test.h"
#include "test/08_mm64k_test.h"
#include "test/09_mp256_test.h"
#include "test/10_mp64k_test.h"
#include "test/11_ssstk_test.h"
#include "test/12_psstk_test.h"
#include "test/13_sdstk_test.h"
#include "test/14_pdstk_test.h"
#include "test/15_llist_test.h"
#include "test/16_avltree_test.h"
#include "test/17_rbtree_test.h"
#include "test/18_hash32_test.h"
#include "test/19_hash64_test.h"
#include "test/20_avlht32_test.h"
#include "test/21_avlht64_test.h"
#include "test/22_rbht32_test.h"
#include "test/23_rbht64_test.h"



void test_other()
{
	/* 溢出机制测试
	unsigned char u8 = 0;
	for ( int i = 0; i < 514; i++ ) {
		printf("%d\n", u8);
		u8++;
	}
	//*/
	//* 位移机制测试
	unsigned int u32 = 0x1AE;
	printf("%d\n", u32 >> 8);
	//*/
}



int main(int argc, char** argv)
{
	#ifdef _WIN32
		SetConsoleOutputCP(65001);
	#endif
	
	//test_other();
	//test_pamm();
	//test_samm();
	//test_mbmu();
	//test_bsmm();
	//test_mmu256();
	//test_mmu64k();
	//test_mm256();
	test_mm64k();
	//test_ssstk();
	//test_psstk();
	//test_sdstk();
	//test_pdstk();
	//test_llist();
	//test_avltree();
	//test_rbtree();
	//test_hash32();
	//test_hash64();
	//test_avlht32();
	//test_avlht64();
	//test_rbht32();
	//test_rbht64();
	
	return 0;
}


