#include <tk/tkernel.h>

#if USE_TMONITOR
#include "../../libtm.h"

#define UART0_THR		((volatile UW *)0x80000000U)
#define UART0_LSR		((volatile UW *)0x80000004U)
#define UART0_TX_READY		0x00000001U

EXPORT	void	tm_snd_dat( const UB* buf, INT size )
{
	INT i;
	for (i = 0; i < size; i++) {
		/* Wait for transmitter ready (bit 0) */
		while ((*UART0_LSR & UART0_TX_READY) == 0U);
		*UART0_THR = buf[i];
	}
}

EXPORT	void	tm_rcv_dat( UB* buf, INT size )
{
	/* Stub receive */
}

EXPORT	void	tm_com_init(void)
{
	/* No init needed for default UART */
}

#endif /* USE_TMONITOR */
