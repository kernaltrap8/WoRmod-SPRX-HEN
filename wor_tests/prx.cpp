#include <cellstatus.h>
#include <sys/prx.h>
#include "main.h"
#include "printf.h"
#include "syscalls.h"
#include "scripting/script.h"
#define VERSION "v1.2r2 alpha-hen"

SYS_MODULE_INFO( wor_tests, 0, 1, 1);
SYS_MODULE_START( _wor_tests_prx_entry );
SYS_MODULE_STOP(_wor_tests_prx_stop);

SYS_LIB_DECLARE_WITH_STUB( LIBNAME, SYS_LIB_AUTO_EXPORT, STUBNAME );
SYS_LIB_EXPORT( _wor_tests_export_function, LIBNAME );

sys_ppu_thread_t gWORTthreadID = SYS_PPU_THREAD_ID_INVALID;

// An exported function is needed to generate the project's PRX stub export library
extern "C" int _wor_tests_export_function(void)
{
    return CELL_OK;
}

extern "C" uint64_t strlen(const char *s) {
	uint64_t r = 0;
	while (*s) {
		r++;
		s++;
	}
	return r;
}

namespace QSymbol {
	void InsertSymbol(uint32_t symbol, int symbolData, int g_EnablePrintf) {
		Script::CSymbolTableEntry* gSymbol = Script::Resolve(symbol);
		printf("%s symbol: %p\n", gSymbol);
		if (g_EnablePrintf = 1) {
			printf("%s symbol data: %p %d %d\n", gSymbol->union_type, gSymbol->type, gSymbol->sourceFileNameChecksum);
		}
		if (gSymbol) {
			gSymbol->union_type = symbolData;
		}
	}
}

void wor_test_main_thread(uint64_t args) {
	//Sleep 30sec before patches
	printf("WoRmod %s loaded.\nSleeping for 30sec before applying patches.\n", VERSION);
	_sys_timer_sleep(30);
	// enable_button_cheats | for debug menu
	QSymbol::InsertSymbol(720971780, 1, 0);
	// debug_use_screen_noise | post processing FX disable
	QSymbol::InsertSymbol(3786639802, 0, 0)
	// debug_use_motion_blur | post processing FX disable
	QSymbol::InsertSymbol(42529484, 0, 0);
	// allow_controller_for_all_instruments
	QSymbol::InsertSymbol(2590800659, 1, 0);
	// g_career_skip_naration
	QSymbol::InsertSymbol(2634030452, 1, 0);
	printf("FINISHED EDITING SYMBOLS!!!\n");
}

extern "C" int _wor_tests_prx_entry(void)
{	
	sys_ppu_thread_create(&gWORTthreadID, wor_test_main_thread, 0, 3000, 4096 * 16, SYS_PPU_THREAD_CREATE_JOINABLE, "WoRmodMainThread");
    return SYS_PRX_RESIDENT;
}


extern "C" int _wor_tests_prx_stop(void)
{
	uint64_t retVal;
	sys_ppu_thread_join(gWORTthreadID, &retVal);
	return SYS_PRX_RESIDENT;
}
