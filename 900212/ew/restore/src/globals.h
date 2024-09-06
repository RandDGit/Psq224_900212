
extern unsigned int ui_axis_set;	// Axis Set Point
extern unsigned int ui_axis_pos;	// Axis Position
extern unsigned int ui_axis_rup;	// Axis Ramp-up end point
extern unsigned int ui_axis_rdn;	// Axis Ramp-dn end point
extern unsigned int ui_axis_tmm;	// Axis Set Value in 1/10th mm units

extern unsigned char uc_tprim;		// Primary timer base (TMR0 ovf)
extern unsigned char uc_tprev;		// Prev. value
extern unsigned char uc_tm0preset;	// Timer 0 preset (controls base period)

extern unsigned char uc_axis_vss;	// Motor State
extern unsigned char uc_axis_dir;	// Motor Direction
extern unsigned char uc_axis_pwm;	// Motor PWM output
extern unsigned char uc_axis_rud;	// Motor Ramp Up,Down state

extern unsigned char uc_led_value;	// Status LED (reload value = frequency)
extern unsigned char uc_led_count;	// Status LED (counter)
extern unsigned char uc_led_scale;	// Scale factor to LED timebase

extern unsigned char uc_rx_stata;	// comms rx status (a) used by USART_RXC_interrupt
extern unsigned char uc_rx_statb;	// comms rx status (b) used by USART_RXC_interrupt
extern unsigned char uc_rx_chbyte;	// comms rx byte       used by USART_RXC_interrupt


extern struct wdef_rxbuff uc_rxbuff[];
extern unsigned char uc_rxrdptr;	// read pointer (rxbuff)
extern unsigned char uc_rxwrptr;	// write pointer (rxbuff)
extern unsigned char uc_rx_chrecv;	// comms rx received, read from buffer to function
extern unsigned char uc_rx_chksum;	// comms rx check sum
extern unsigned char uc_rx_process;	// comms rx byte processed

extern unsigned char uc_tx_chout;	// character to be TX'd out
extern unsigned char uc_tx_chksum;	// comms tx check sum
extern unsigned char uc_tx_chsent;	// TX char out complete

extern unsigned char uc_comm_adid;	// comm address id
extern unsigned char uc_comm_idle;	// comm RS485 bus idle test
extern unsigned char uc_comm_flag;	// comms bit flags

extern unsigned char uc_comm_9set;	// 9th bit set (derived from statb)
extern unsigned char uc_comm_hilo;	// Comms hi-speed (9bit) or lo-speed (8bit)
extern unsigned char uc_comm_comp;	// Comms compare Tx out with Rx in (TX integrity)

extern unsigned char uca_msgrx[];      	// rx buffer dst:src:cmd:dhi:dlo:cks
extern unsigned char uca_msgtx[];      	// tx buffer
extern unsigned char uc_msgrxptr;	// index to rx message = 6 if hi-speed = 5 if lo-speed
extern unsigned char uc_msgtxprt;	// index to tx message

extern unsigned char dblead_ip_enc;	// Debounce Lead edge encoder
extern unsigned char dbtail_ip_enc;
extern unsigned char val_ip_enc;

extern unsigned char dblead_ip_trg;	// Debounce Lead edge trigger input
extern unsigned char dbtail_ip_trg;
extern unsigned char val_ip_trg;

extern unsigned char dblead_ip_hom;	// Debounce Lead edge home input
extern unsigned char dbtail_ip_hom;
extern unsigned char val_ip_hom;

extern unsigned char dblead_ip_lim;	// Debounce Lead edge limit input
extern unsigned char dbtail_ip_lim;
extern unsigned char val_ip_lim;

extern unsigned char dblead_ip_sw1;	// Debounce Lead edge button input
extern unsigned char dbtail_ip_sw1;
extern unsigned char val_ip_sw1;
extern unsigned char pin_ip_sw1;

extern unsigned char uc_read_portd;            // PIND
extern unsigned char uc_read_portb;            // PINB

extern struct wdef_ctimer ws_ctimer[];	// application timer function(s)
extern struct wdef_ctimer *wsp_tc;	// timer counter implementation


