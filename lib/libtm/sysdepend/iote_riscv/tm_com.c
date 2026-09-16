#include <tk/tkernel.h>

#if USE_TMONITOR
#include "../../libtm.h"

/* CFU-PG simulation console: write one character to the low eight bits. */
#define CFU_CONSOLE_TX	(*(volatile UW *)0x80000000U)

EXPORT	void	tm_snd_dat( const UB* buf, INT size )
{
	INT i;
	for (i = 0; i < size; i++) {
		CFU_CONSOLE_TX = buf[i];
	}
}

EXPORT	void	tm_rcv_dat( UB* buf, INT size )
{
	INT i;

	/* The current CFU-PG console is output-only. */
	for (i = 0; i < size; i++) {
		buf[i] = 0;
	}
}

EXPORT	void	tm_com_init(void)
{
	/* The simulation console does not require initialization. */
}

#endif /* USE_TMONITOR */
