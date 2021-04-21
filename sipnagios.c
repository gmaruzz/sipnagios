/* $Id$ */
/* 
 * Copyright (C) 2008-2011 Teluu Inc. (http://www.teluu.com)
 * Copyright (C) 2003-2008 Benny Prijono <benny@prijono.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 */

/* sipnagios.c (this file) is brought to you by
 * Giovanni Maruzzelli
 * gmaruzz at OpenTelecom.IT
 *
 * sipnagios.c is a modification of the original
 * sample siprtp.c in pjproject distribution
 *
 * enjoy!
 */

/******************************************
  HOW TO COMPILE SIPNAGIOS:

apt install python-dev gcc make gcc binutils build-essential git wget unzip uuid-dev

cd /usr/src
git clone https://github.com/gmaruzz/sipnagios.git

wget https://github.com/pjsip/pjproject/archive/refs/tags/2.11.zip
unzip 2.11.zip
cd pjproject-2.11/
./configure
CFLAGS="$CFLAGS -fPIC -DPJ_LOG_MAX_LEVEL=3" make dep

cp ../sipnagios/sipnagios.c pjsip-apps/src/samples/
cp ../sipnagios/Samples.mak pjsip-apps/build/

CFLAGS="$CFLAGS -fPIC -DPJ_LOG_MAX_LEVEL=3" make 
./pjsip-apps/bin/samples/x86_64-unknown-linux-gnu/sipnagios --help

******************************************/


/* Usage */
static const char *USAGE = 
" PURPOSE:								    \n"
"   sipnagios is a modification of the original siprtp sample in pjproject  \n"
"   \n"
"   siprtp original blurb:  \n"
"   This program establishes SIP INVITE session and media, and calculate    \n"
"   the media quality (packet lost, jitter, rtt, etc.). Unlike normal	    \n"
"   pjmedia applications, this program bypasses all pjmedia stream	    \n"
"   framework and transmit encoded RTP packets manually using own thread.   \n"
"   \n"
"   sipnagios blurb:  \n"
"   sipnagios implements the Nagios plugin API for monitoring and \n"
"   performance data. It makes a call, checks all the various resulting \n"
"   values (mos, rtt, pdd, tta, jitter, packet loss, bytes and packets \n"
"   transferred, and so on). It verifies these values are included into \n"
"   acceptable, warning, or critical ranges. If the call has gone well,\n"
"   sipnagios print performance data for Nagios graphs, and returns 0.\n"
"   If the call fails, or if its measured values are not inside acceptable \n"
"   ranges, it exits with Nagios conventional WARNING or CRITICAL values.\n"
"   \n"
"\n"
" USAGE:\n"
"   sipnagios [options] SIPURL\n"
"\n"
" Call options:\n"
"   --duration=SEC,   -d    Set maximum call duration in seconds (default:40) \n"
"\n"
" Address and ports options:\n"
"   --local-port=PORT,-p    Set local SIP port (default: 5060)\n"
"   --rtp-port=PORT,  -r    Set start of RTP port (default: 4000)\n"
"   --ip-addr=IP,     -i    Set local IP address to use (otherwise it will\n"
"                           try to determine local IP address from hostname)\n"
"\n"
" Local user options:\n"
"   --local-user=USER,       -u    Set local SIP user (default: alice)\n"
"   --local-password=PASSWD, -w    Set local SIP user password (default: 1234)\n"
"   --local-siprealm=REALM,  -s    Set local SIP user SIP realm (default: atlanta.example.com)\n"
"\n"
" Logging Options:\n"
"   --log-level=N,    -l    Set log verbosity level (default=1)\n"
"   --app-log-level=N       Set app log verbosity (default=1)\n"
"   --log-file=FILE         Write log to file FILE\n"
"\n"
"NAGIOS RANGES OPTIONS DEFAULTS:\n"
"\n"
"-c --rxmosWARN = 4.0\n"
"-g --txmosWARN = 4.0\n"
"-t --rxmosCRIT = 3.6\n"
"-m --txmosCRIT = 3.6\n"
"\n"
"-A --durationmsWARN = 40000\n"
"-B --pddmsWARN = 8000\n"
"-C --ttamsWARN = 11000\n"
"-D --rxpktsWARN = 1000\n" 
"-E --rxbytesWARN = 150000\n" 
"-F --rxlosspctWARN = 1.5\n"
"-G --rxduppctWARN =  1.5\n"
"-H --rxreorderpctWARN =1.5\n"
"-I --rxjitavgWARN =   15.1\n"
"-L --txpktsWARN =  1000\n" 
"-M --txbytesWARN = 150000\n" 
"-N --txlosspctWARN = 1.5\n"
"-O --txduppctWARN =  1.5\n"
"-P --txreorderpctWARN =1.5\n"
"-Q --txjitavgWARN =  15.1\n" 
"-R --rttmeanWARN =   150.1\n" 
"\n"
"-S --durationmsCRIT = 20000\n"
"-T --pddmsCRIT = 16000\n"
"-U --ttamsCRIT = 20000\n"
"-V --rxpktsCRIT = 500\n" 
"-Z --rxbytesCRIT = 75000\n" 
"-K --rxlosspctCRIT = 3.0\n"
"-J --rxduppctCRIT =  3.0\n"
"-X --rxreorderpctCRIT =3.0\n"
"-Y --rxjitavgCRIT =   30.1\n"
"-v --txpktsCRIT =  500\n" 
"-z --txbytesCRIT = 75000\n" 
"-k --txlosspctCRIT = 3.0\n"
"-j --txduppctCRIT =  3.0\n"
"-x --txreorderpctCRIT =3.0\n"
"-y --txjitavgCRIT =  30.1\n" 
"-b --rttmeanCRIT =   300.1\n" 
"\n"
"==> rx and tx mos, durationms, rx and tx pkts and bytes becomes WARNING and CRITICAL if they're measured as LOWER than threshold\n"
"==> all other values becomes not OK if they are measured as HIGHER than threshold\n"
"\n"
"Eg:\n"
"sipnagios --rtp-port=4000 --local-port=8090 --ip-addr=188.166.74.47 --local-user=9599 --local-siprealm=acme.cloudpbx.opentelecom.it --local-password=cAcyAgaC46AKuRk sip:0749941093@acme.cloudpbx.opentelecom.it:5030\n"
"(sipnagios is brought to you by Giovanni Maruzzelli. Enjoy!)\n"
;


/* Include all headers. */
#include <pjsip.h>
#include <pjmedia.h>
#include <pjmedia-codec.h>
#include <pjsip_ua.h>
#include <pjsip_simple.h>
#include <pjlib-util.h>
#include <pjlib.h>

#include <stdlib.h>
#include <uuid/uuid.h>

/* Uncomment these to disable threads.
 * NOTE:
 *   when threading is disabled, sipnagios won't transmit any
 *   RTP packets.
 */
/*
#undef PJ_HAS_THREADS
#define PJ_HAS_THREADS 0
 */


#if PJ_HAS_HIGH_RES_TIMER==0
#   error "High resolution timer is needed for this sample"
#endif

#define THIS_FILE	"sipnagios.c"
#define MAX_CALLS	1024
#define RTP_START_PORT	4000


/* Codec descriptor: */
struct codec
{
	unsigned	pt;
	char*	name;
	unsigned	clock_rate;
	unsigned	bit_rate;
	unsigned	ptime;
	char*	description;
};


/* A bidirectional media stream created when the call is active. */
struct media_stream
{
	/* Static: */
	unsigned		 call_index;	    /* Call owner.		*/
	unsigned		 media_index;	    /* Media index in call.	*/
	pjmedia_transport   *transport;	    /* To send/recv RTP/RTCP	*/

	/* Active? */
	pj_bool_t		 active;	    /* Non-zero if is in call.	*/

	/* Current stream info: */
	pjmedia_stream_info	 si;		    /* Current stream info.	*/

	/* More info: */
	unsigned		 clock_rate;	    /* clock rate		*/
	unsigned		 samples_per_frame; /* samples per frame	*/
	unsigned		 bytes_per_frame;   /* frame size.		*/

	/* RTP session: */
	pjmedia_rtp_session	 out_sess;	    /* outgoing RTP session	*/
	pjmedia_rtp_session	 in_sess;	    /* incoming RTP session	*/

	/* RTCP stats: */
	pjmedia_rtcp_session rtcp;		    /* incoming RTCP session.	*/

	/* Thread: */
	pj_bool_t		 thread_quit_flag;  /* Stop media thread.	*/
	pj_thread_t		*thread;	    /* Media thread.		*/
};


/* This is a call structure that is created when the application starts
 * and only destroyed when the application quits.
 */
struct call
{
	unsigned		 index;
	pjsip_inv_session	*inv;
	unsigned		 media_count;
	struct media_stream	 media[1];
	pj_time_val		 start_time;
	pj_time_val		 response_time;
	pj_time_val		 connect_time;

	pj_timer_entry	 d_timer;	    /**< Disconnect timer.	*/
};


/* Application's global variables */
static struct app
{
	unsigned		 max_calls;
	unsigned		 call_gap;
	pj_bool_t		 call_report;
	pj_bool_t		 call_monitoring;
	unsigned		 uac_calls;
	unsigned		 duration;
	pj_bool_t		 auto_quit;
	unsigned		 thread_count;
	int			 sip_port;
	int			 rtp_start_port;
	pj_str_t		 local_addr;
	pj_str_t		 local_uri;
	pj_str_t		 local_contact;
	pj_str_t		 local_user;
	pj_str_t		 local_password;
	pj_str_t		 local_siprealm;

	int			 app_log_level;
	int			 log_level;
	char		*log_filename;
	char		*report_filename;

	struct codec	 audio_codec;

	pj_str_t		 uri_to_call;

	pj_caching_pool	 cp;
	pj_pool_t		*pool;

	pjsip_endpoint	*sip_endpt;
	pj_bool_t		 thread_quit;
	pj_thread_t		*sip_thread[1];

	pjmedia_endpt	*med_endpt;
	struct call		 call[MAX_CALLS];

	long durationmsWARN ; // A
	long pddmsWARN ; //B
	long ttamsWARN ; //C
	long rxpktsWARN ; //D
	long rxbytesWARN ; //E
	float rxlosspctWARN ; //F
	float rxduppctWARN ; //G
	float rxreorderpctWARN ; //H
	float rxjitavgWARN ; //I
	long txpktsWARN ; //L
	long txbytesWARN ; //M
	float txlosspctWARN ; //N
	float txduppctWARN ; //O
	float txreorderpctWARN ; //P
	float txjitavgWARN ; //Q
	float rttmeanWARN ; //R

	long durationmsCRIT ; //S
	long pddmsCRIT ; //T
	long ttamsCRIT ; //U
	long rxpktsCRIT ; //V
	long rxbytesCRIT ; //Z
	float rxlosspctCRIT ; //K
	float rxduppctCRIT ; //J
	float rxreorderpctCRIT ; //X
	float rxjitavgCRIT ; //Y
	long txpktsCRIT ; //v
	long txbytesCRIT ; //z
	float txlosspctCRIT ; //k
	float txduppctCRIT ; //j
	float txreorderpctCRIT ; //x
	float txjitavgCRIT ; //y
	float rttmeanCRIT ; //b

	float rxmosWARN ; //c
	float txmosWARN ; //g
	float rxmosCRIT ; //t
	float txmosCRIT ; //m

} app;



/*
 * Prototypes:
 */

/* Callback to be called when SDP negotiation is done in the call: */
static void call_on_media_update( pjsip_inv_session *inv,
		pj_status_t status);

/* Callback to be called when invite session's state has changed: */
static void call_on_state_changed( pjsip_inv_session *inv, 
		pjsip_event *e);

/* Callback to be called when dialog has forked: */
static void call_on_forked(pjsip_inv_session *inv, pjsip_event *e);

/* Callback to be called to handle incoming requests outside dialogs: */
static pj_bool_t on_rx_request( pjsip_rx_data *rdata );

/* Worker thread prototype */
static int sip_worker_thread(void *arg);

/* Create SDP for call */
static pj_status_t create_sdp( pj_pool_t *pool,
		struct call *call,
		pjmedia_sdp_session **p_sdp);

/* Hangup call */
static void hangup_call(unsigned index);

/* Destroy the call's media */
static void destroy_call_media(unsigned call_index);

/* Destroy media. */
static void destroy_media();

/* This callback is called by media transport on receipt of RTP packet. */
static void on_rx_rtp(void *user_data, void *pkt, pj_ssize_t size);

/* This callback is called by media transport on receipt of RTCP packet. */
static void on_rx_rtcp(void *user_data, void *pkt, pj_ssize_t size);

/* Display error */
static void app_perror(const char *sender, const char *title, 
		pj_status_t status);

/* Convenient function to convert transmission factor to MOS */
static float rfactor_to_mos(float rfactor) {
	float mos;
	if (rfactor <= 0) {
		mos = 0.0;
	} else if (rfactor > 100) {
		mos = 4.5;
	} else {
		mos = rfactor*4.5/100;
	}
	return mos;
}
static void print_call2(int call_index, struct app app)
{
	struct call *call = &app.call[call_index];
	int len;
	pjsip_inv_session *inv = call->inv;
	pjsip_dialog *dlg = inv->dlg;
	struct media_stream *audio = &call->media[0];
	char userinfo[PJSIP_MAX_URL_SIZE];
	char duration[80], last_update[80];
	char bps[16], ipbps[16], packets[16], bytes[16], ipbytes[16];
	unsigned decor;
	pj_time_val now;
	long durationms;


	decor = pj_log_get_decor();
	pj_log_set_decor(0);

	pj_gettimeofday(&now);

	if (app.report_filename)
		puts(app.report_filename);

	/* Print duration */
	if (inv->state >= PJSIP_INV_STATE_CONFIRMED && call->connect_time.sec) {

		PJ_TIME_VAL_SUB(now, call->connect_time);

		durationms = PJ_TIME_VAL_MSEC(now);

	} else {
		durationms = 0;
	}



	/* Call identification */
	len = pjsip_hdr_print_on(dlg->remote.info, userinfo, sizeof(userinfo));
	if (len < 0)
		pj_ansi_strcpy(userinfo, "<--uri too long-->");
	else
		userinfo[len] = '\0';

	if (call->inv == NULL || call->inv->state < PJSIP_INV_STATE_CONFIRMED ||
			call->connect_time.sec == 0) 
	{
		PJ_LOG(1, (THIS_FILE, "%s MOS[rx=%1.2f tx=%1.2f] Call %s TIME[pddms=%ldms ttams=%ldms durationms=%ldms] RX[rxmos=%1.2f rxpkts=%ld rxbytes=%ldB rxloss=%d rxlosspct=%3.1f%% rxdup=%d rxduppct=%3.1f%% rxreorder=%d rxreorderpct=%3.1f%% rxjitmin=%3.1fms rxjitavg=%3.1fms rxjitmax=%3.1fms] TX[txmos=%1.2f txpkts=%ld txbytes=%ldB txloss=%d txlosspct=%3.1f%% txdup=%d txduppct=%3.1f%% txreorder=%d txreorderpct=%3.1f%% txjitmin=%3.1fms txjitavg=%3.1fms txjitmax=%3.1fms] RTT[rttmin=%3.1fms rttmean=%3.1fms rttmax=%3.1fms] |durationms=%ldms pddms=%ldms ttams=%ldms rxmos=%1.2f rxpkts=%ld rxbytes=%ldB rxloss=%d rxlosspct=%3.1f%% rxdup=%d rxduppct=%3.1f%% rxreorder=%d rxreorderpct=%3.1f%% rxjitmin=%3.1fms rxjitavg=%3.1fms rxjitmax=%3.1fms txmos=%1.2f txpkts=%ld txbytes=%ldB txloss=%d txlosspct=%3.1f%% txdup=%d txduppct=%3.1f%% txreorder=%d txreorderpct=%3.1f%% txjitmin=%3.1fms txjitavg=%3.1fms txjitmax=%3.1fms rttmin=%3.1fms rttmean=%3.1fms rttmax=%3.1fms ", 
					"KO",

					0.0,
					0.0,

					userinfo,

					0, 
					0, 
					0,
					0.0,
					0 ,    
					0 ,   
					0 ,    
					0.0 , 
					0 ,     
					0.0 ,  
					0 , 
					0.0,
					0.0 ,  
					0.0 ,  
					0.0 ,  
					0.0,
					0 ,    
					0 ,   
					0 ,    
					0.0 , 
					0 ,     
					0.0 ,  
					0 , 
					0.0,
					0.0 ,  
					0.0 ,  
					0.0 ,  
					0.0 ,    
					0.0 ,   
					0.0 ,

					0, 
					0, 
					0,
					0.0,
					0 ,    
					0 ,   
					0 ,    
					0.0 , 
					0 ,     
					0.0 ,  
					0 , 
					0.0,
					0.0 ,  
					0.0 ,  
					0.0 ,  
					0.0,
					0 ,    
					0 ,   
					0 ,    
					0.0 , 
					0 ,     
					0.0 ,  
					0 , 
					0.0,
					0.0 ,  
					0.0 ,  
					0.0 ,  
					0.0 ,    
					0.0 ,   
					0.0 
						));



		pj_log_set_decor(decor);
		exit(2); //NAGIOS CRITICAL
	}

	pj_time_val t;
	long pddms;
	long ttams;

	long rxpkts =      audio->rtcp.stat.rx.pkt;
	long rxbytes =     audio->rtcp.stat.rx.bytes;
	int rxloss =      audio->rtcp.stat.rx.loss;
	float rxlosspct =   audio->rtcp.stat.rx.loss * 100.0 / (audio->rtcp.stat.rx.pkt + audio->rtcp.stat.rx.loss);
	int rxdup =       audio->rtcp.stat.rx.dup; 
	float rxduppct =    audio->rtcp.stat.rx.dup * 100.0 / (audio->rtcp.stat.rx.pkt + audio->rtcp.stat.rx.loss);
	int rxreorder =   audio->rtcp.stat.rx.reorder; 
	float rxreorderpct =audio->rtcp.stat.rx.reorder * 100.0 / (audio->rtcp.stat.rx.pkt + audio->rtcp.stat.rx.loss);
	float rxjitmin =    audio->rtcp.stat.rx.jitter.min / 1000.0;
	float rxjitavg =    audio->rtcp.stat.rx.jitter.mean / 1000.0;
	float rxjitmax =    audio->rtcp.stat.rx.jitter.max / 1000.0;

	long txpkts =      audio->rtcp.stat.tx.pkt;
	long txbytes =     audio->rtcp.stat.tx.bytes;
	int txloss =      audio->rtcp.stat.tx.loss;
	float txlosspct =   audio->rtcp.stat.tx.loss * 100.0 / (audio->rtcp.stat.tx.pkt + audio->rtcp.stat.tx.loss);
	int txdup =       audio->rtcp.stat.tx.dup; 
	float txduppct =    audio->rtcp.stat.tx.dup * 100.0 / (audio->rtcp.stat.tx.pkt + audio->rtcp.stat.tx.loss);
	int txreorder =   audio->rtcp.stat.tx.reorder; 
	float txreorderpct =audio->rtcp.stat.tx.reorder * 100.0 / (audio->rtcp.stat.tx.pkt + audio->rtcp.stat.tx.loss);
	float txjitmin =    audio->rtcp.stat.tx.jitter.min / 1000.0;
	float txjitavg =    audio->rtcp.stat.tx.jitter.mean / 1000.0;
	float txjitmax =    audio->rtcp.stat.tx.jitter.max / 1000.0;

	float rttmin =      audio->rtcp.stat.rtt.min / 1000.0;
	float rttmean =     audio->rtcp.stat.rtt.mean / 1000.0;
	float rttmax =      audio->rtcp.stat.rtt.max / 1000.0;

	char alarm[512];

	alarm[0] = '\0'; // empty string

	if (call->response_time.sec) {
		t = call->response_time;
		PJ_TIME_VAL_SUB(t, call->start_time);
		pddms = PJ_TIME_VAL_MSEC(t);
	} else {
		pddms = 0;
	}

	if (call->connect_time.sec) {
		t = call->connect_time;
		PJ_TIME_VAL_SUB(t, call->start_time);
		ttams = PJ_TIME_VAL_MSEC(t);
	} else {
		ttams = 0;
	}

	/*
https://medium.com/obkio/measuring-voip-quality-with-mos-score-mean-opinion-score-4de9c7541304
# VoIP Codec Processing Delay in milliseconds (ms)

codec_delay = 10.0

# Effective Latency is the Average Latency plus the doubled
# Average Jitter plus the VoIP Codec Delays

effective_latency = average_latency + 2* average_jitter + codec_delay

# Calculate the R-Value (Transmission Rating Factor R) based on
# Effective Latency. The voice quality drops more significantly
# over 160ms so the R-Value is penalized more.

if effective_latency < 160:
r = 93.2 - (effective_latency / 40)

else:
r = 93.2 - ((effective_latency - 120)/ 10)


# Reduce R-Value 2.5 times the Packet Loss percentage

r = r - 2.5 * packet_loss

# Based on ITU-T G.107

if r < 0:
mos = 1.0

else:
mos = 1 + 0.035*r + 0.000007*r*(r-60)*(100-r)
	 */

	/*****************************/
	// merci Julien Chavanton, VoIP Patrol, https://github.com/jchavanton/voip_patrol/blob/master/src/voip_patrol/voip_patrol.cc
	// mos calculation here is adapted from your work I can't even understand :)

	/* represent loss dependent effective equipment impairment factor and percentage loss probability */
	int Bpl = 25; /* packet-loss robustness factor Bpl is defined as a codec-specific value. */
	float Ie_eff_rx, Ppl_rx, Ppl_cut_rx, Ie_eff_tx, Ppl_tx, Ppl_cut_tx;
	int Ie = 0; /* Not used : Refer to Appendix I of [ITU-T G.113] for the currently recommended values of Ie.*/
	float Ta = 0.0; /* Absolute Delay */

	Ppl_rx = (rxloss+rxdup) * 100.0 / (rxpkts + rxloss);
	Ppl_tx = (txloss+txdup) * 100.0 / (txpkts + txloss);

	float BurstR_rx = 1.0;
	Ie_eff_rx = (Ie + (95 - Ie) * Ppl_rx / (Ppl_rx/BurstR_rx + Bpl));
	int rfactor_rx = 100 - Ie_eff_rx;
	float rxmos = rfactor_to_mos(rfactor_rx);
	float BurstR_tx = 1.0;
	Ie_eff_tx = (Ie + (95 - Ie) * Ppl_tx / (Ppl_rx/BurstR_tx + Bpl));
	int rfactor_tx = 100 - Ie_eff_tx;
	float txmos = rfactor_to_mos(rfactor_tx);

	/*****************************/

	int retval = 0;

	if( (100000 - durationms) > app.durationmsWARN ) {sprintf(alarm, "WARN durationms=%ld", durationms); retval = 1;}
	if(pddms > app.pddmsWARN) {sprintf(alarm, "WARN pddms=%ld", pddms); retval = 1;}
	if(ttams > app.ttamsWARN) {sprintf(alarm, "WARN ttams=%ld", ttams); retval = 1;}

	if( (10000 - rxpkts) > app.rxpktsWARN ) {sprintf(alarm, "WARN rxpkts=%ld", rxpkts); retval = 1;}
	if( (400000 - rxbytes) > app.rxbytesWARN ) {sprintf(alarm, "WARN rxbytes=%ld", rxbytes); retval = 1;}
	if(rxlosspct > app.rxlosspctWARN) {sprintf(alarm, "WARN rxlosspct=%3.1f%%", rxlosspct); retval = 1;}
	if(rxduppct > app.rxduppctWARN) {sprintf(alarm, "WARN rxduppct=%3.1f%%", rxduppct); retval = 1;}
	if(rxreorderpct > app.rxreorderpctWARN) {sprintf(alarm, "WARN rxreorderpct=%3.1f%%", rxreorderpct); retval = 1;}
	if(rxjitavg > app.rxjitavgWARN) {sprintf(alarm, "WARN rxjitavg=%3.1f", rxjitavg); retval = 1;}
	if( (5.0 - rxmos) > app.rxmosWARN ) {sprintf(alarm, "WARN rxmos=%1.2f", rxmos); retval = 1;}

	if( (10000 - txpkts) > app.txpktsWARN ) {sprintf(alarm, "WARN txpkts=%ld", txpkts); retval = 1;}
	if( (400000 - txbytes) > app.txbytesWARN ) {sprintf(alarm, "WARN txbytes=%ld", txbytes); retval = 1;}
	if(txlosspct > app.txlosspctWARN) {sprintf(alarm, "WARN txlosspct=%3.1f%%", txlosspct); retval = 1;}
	if(txduppct > app.txduppctWARN) {sprintf(alarm, "WARN txduppct=%3.1f%%", txduppct); retval = 1;}
	if(txreorderpct > app.txreorderpctWARN) {sprintf(alarm, "WARN txreorderpct=%3.1f%%", txreorderpct); retval = 1;}
	if(txjitavg > app.txjitavgWARN) {sprintf(alarm, "WARN txjitavg=%3.1f", txjitavg); retval = 1;}
	if( (5.0 - txmos) > app.txmosWARN ) {sprintf(alarm, "WARN txmos=%1.2f", txmos); retval = 1;}

	if(rttmean > app.rttmeanWARN) {sprintf(alarm, "WARN rttmean=%3.1f", rttmean); retval = 1;}

	if( (100000 - durationms) > app.durationmsCRIT ) {sprintf(alarm, "CRIT durationms=%ld", durationms); retval = 2;}
	if(pddms > app.pddmsCRIT) {sprintf(alarm, "CRIT pddms=%ld", pddms); retval = 2;}
	if(ttams > app.ttamsCRIT) {sprintf(alarm, "CRIT ttams=%ld", ttams); retval = 2;}

	if( (10000 - rxpkts) > app.rxpktsCRIT ) {sprintf(alarm, "CRIT rxpkts=%ld", rxpkts); retval = 2;}
	if( (400000 - rxbytes) > app.rxbytesCRIT ) {sprintf(alarm, "CRIT rxbytes=%ld", rxbytes); retval = 2;}
	if(rxlosspct > app.rxlosspctCRIT) {sprintf(alarm, "CRIT rxlosspct=%3.1f%%", rxlosspct); retval = 2;}
	if(rxduppct > app.rxduppctCRIT) {sprintf(alarm, "CRIT rxduppct=%3.1f%%", rxduppct); retval = 2;}
	if(rxreorderpct > app.rxreorderpctCRIT) {sprintf(alarm, "CRIT rxreorderpct=%3.1f%%", rxreorderpct); retval = 2;}
	if(rxjitavg > app.rxjitavgCRIT) {sprintf(alarm, "CRIT rxjitavg=%3.1f", rxjitavg); retval = 2;}
	if( (5.0 - rxmos) > app.rxmosCRIT ) {sprintf(alarm, "CRIT rxmos=%1.2f", rxmos); retval = 2;}

	if( (10000 - txpkts) > app.txpktsCRIT ) {sprintf(alarm, "CRIT txpkts=%ld", txpkts); retval = 2;}
	if( (400000 - txbytes) > app.txbytesCRIT ) {sprintf(alarm, "CRIT txbytes=%ld", txbytes); retval = 2;}
	if(txlosspct > app.txlosspctCRIT) {sprintf(alarm, "CRIT txlosspct=%3.1f%%", txlosspct); retval = 2;}
	if(txduppct > app.txduppctCRIT) {sprintf(alarm, "CRIT txduppct=%3.1f%%", txduppct); retval = 2;}
	if(txreorderpct > app.txreorderpctCRIT) {sprintf(alarm, "CRIT txreorderpct=%3.1f%%", txreorderpct); retval = 2;}
	if(txjitavg > app.txjitavgCRIT) {sprintf(alarm, "CRIT txjitavg=%3.1f", txjitavg); retval = 2;}
	if( (5.0 - txmos) > app.txmosCRIT ) {sprintf(alarm, "CRIT txmos=%1.2f", txmos); retval = 2;}

	if(rttmean > app.rttmeanCRIT) {sprintf(alarm, "CRIT rttmean=%3.1f", rttmean); retval = 2;}

	if(retval == 0) {sprintf(alarm, "OK");}

	PJ_LOG(1, (THIS_FILE, "%s MOS[rx=%1.2f tx=%1.2f] Call %s TIME[pddms=%ldms ttams=%ldms durationms=%ldms] RX[rxmos=%1.2f rxpkts=%ld rxbytes=%ldB rxloss=%d rxlosspct=%3.1f%% rxdup=%d rxduppct=%3.1f%% rxreorder=%d rxreorderpct=%3.1f%% rxjitmin=%3.1fms rxjitavg=%3.1fms rxjitmax=%3.1fms] TX[txmos=%1.2f txpkts=%ld txbytes=%ldB txloss=%d txlosspct=%3.1f%% txdup=%d txduppct=%3.1f%% txreorder=%d txreorderpct=%3.1f%% txjitmin=%3.1fms txjitavg=%3.1fms txjitmax=%3.1fms] RTT[rttmin=%3.1fms rttmean=%3.1fms rttmax=%3.1fms] |durationms=%ldms pddms=%ldms ttams=%ldms rxmos=%1.2f rxpkts=%ld rxbytes=%ldB rxloss=%d rxlosspct=%3.1f%% rxdup=%d rxduppct=%3.1f%% rxreorder=%d rxreorderpct=%3.1f%% rxjitmin=%3.1fms rxjitavg=%3.1fms rxjitmax=%3.1fms txmos=%1.2f txpkts=%ld txbytes=%ldB txloss=%d txlosspct=%3.1f%% txdup=%d txduppct=%3.1f%% txreorder=%d txreorderpct=%3.1f%% txjitmin=%3.1fms txjitavg=%3.1fms txjitmax=%3.1fms rttmin=%3.1fms rttmean=%3.1fms rttmax=%3.1fms ", 
				alarm,

				rxmos ,    
				txmos ,    

				userinfo,
				pddms, 
				ttams,
				durationms, 

				rxmos ,    
				rxpkts ,    
				rxbytes ,   
				rxloss ,    
				rxlosspct , 
				rxdup ,     
				rxduppct ,  
				rxreorder , 
				rxreorderpct,
				rxjitmin ,  
				rxjitavg ,  
				rxjitmax ,  
				txmos ,    
				txpkts ,    
				txbytes ,   
				txloss ,    
				txlosspct , 
				txdup ,     
				txduppct ,  
				txreorder , 
				txreorderpct,
				txjitmin ,  
				txjitavg ,  
				txjitmax ,  
				rttmin ,    
				rttmean ,   
				rttmax     ,

				durationms, 
				pddms, 
				ttams,
				rxmos ,    
				rxpkts ,    
				rxbytes ,   
				rxloss ,    
				rxlosspct , 
				rxdup ,     
				rxduppct ,  
				rxreorder , 
				rxreorderpct,
				rxjitmin ,  
				rxjitavg ,  
				rxjitmax ,  
				txmos ,    
				txpkts ,    
				txbytes ,   
				txloss ,    
				txlosspct , 
				txdup ,     
				txduppct ,  
				txreorder , 
				txreorderpct,
				txjitmin ,  
				txjitavg ,  
				txjitmax ,  
				rttmin ,    
				rttmean ,   
				rttmax     

					));

	PJ_LOG(1, (THIS_FILE, "\n"));
	pj_log_set_decor(decor);

	if(retval != 0) {
		exit(retval);
	}

}


/* This is a PJSIP module to be registered by application to handle
 * incoming requests outside any dialogs/transactions. The main purpose
 * here is to handle incoming INVITE request message, where we will
 * create a dialog and INVITE session for it.
 */
static pjsip_module mod_sipnagios =
{
	NULL, NULL,			    /* prev, next.		*/
	{ "mod-sipnagiosapp", 13 },	    /* Name.			*/
	-1,				    /* Id			*/
	PJSIP_MOD_PRIORITY_APPLICATION, /* Priority			*/
	NULL,			    /* load()			*/
	NULL,			    /* start()			*/
	NULL,			    /* stop()			*/
	NULL,			    /* unload()			*/
	&on_rx_request,		    /* on_rx_request()		*/
	NULL,			    /* on_rx_response()		*/
	NULL,			    /* on_tx_request.		*/
	NULL,			    /* on_tx_response()		*/
	NULL,			    /* on_tsx_state()		*/
};


/* Codec constants */
struct codec audio_codecs[] = 
{
	{ 0,  "PCMU", 8000, 64000, 20, "G.711 ULaw" },
	{ 3,  "GSM",  8000, 13200, 20, "GSM" },
	{ 4,  "G723", 8000, 6400,  30, "G.723.1" },
	{ 8,  "PCMA", 8000, 64000, 20, "G.711 ALaw" },
	{ 18, "G729", 8000, 8000,  20, "G.729" },
};


/*
 * Init SIP stack
 */
static pj_status_t init_sip()
{
	unsigned i;
	pj_status_t status;

	/* init PJLIB-UTIL: */
	status = pjlib_util_init();
	PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

	/* Must create a pool factory before we can allocate any memory. */
	pj_caching_pool_init(&app.cp, &pj_pool_factory_default_policy, 0);

	/* Create application pool for misc. */
	app.pool = pj_pool_create(&app.cp.factory, "app", 1000, 1000, NULL);

	/* Create the endpoint: */
	status = pjsip_endpt_create(&app.cp.factory, pj_gethostname()->ptr, 
			&app.sip_endpt);
	PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);


	/* Add UDP transport. */
	{
		pj_sockaddr_in addr;
		pjsip_host_port addrname;
		pjsip_transport *tp;

		pj_bzero(&addr, sizeof(addr));
		addr.sin_family = pj_AF_INET();
		addr.sin_addr.s_addr = 0;
		addr.sin_port = pj_htons((pj_uint16_t)app.sip_port);

		if (app.local_addr.slen) {

			addrname.host = app.local_addr;
			addrname.port = app.sip_port;

			status = pj_sockaddr_in_init(&addr, &app.local_addr, 
					(pj_uint16_t)app.sip_port);
			if (status != PJ_SUCCESS) {
				app_perror(THIS_FILE, "Unable to resolve IP interface", status);
				return status;
			}
		}

		status = pjsip_udp_transport_start( app.sip_endpt, &addr, 
				(app.local_addr.slen ? &addrname:NULL),
				1, &tp);
		if (status != PJ_SUCCESS) {
			app_perror(THIS_FILE, "Unable to start UDP transport", status);
			return status;
		}

		PJ_LOG(3,(THIS_FILE, "SIP UDP listening on %.*s:%d",
					(int)tp->local_name.host.slen, tp->local_name.host.ptr,
					tp->local_name.port));
	}

	/* 
	 * Init transaction layer.
	 * This will create/initialize transaction hash tables etc.
	 */
	status = pjsip_tsx_layer_init_module(app.sip_endpt);
	PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

	/*  Initialize UA layer. */
	status = pjsip_ua_init_module( app.sip_endpt, NULL );
	PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

	/* Initialize 100rel support */
	status = pjsip_100rel_init_module(app.sip_endpt);
	PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

	/*  Init invite session module. */
	{
		pjsip_inv_callback inv_cb;

		/* Init the callback for INVITE session: */
		pj_bzero(&inv_cb, sizeof(inv_cb));
		inv_cb.on_state_changed = &call_on_state_changed;
		inv_cb.on_new_session = &call_on_forked;
		inv_cb.on_media_update = &call_on_media_update;

		/* Initialize invite session module:  */
		status = pjsip_inv_usage_init(app.sip_endpt, &inv_cb);
		PJ_ASSERT_RETURN(status == PJ_SUCCESS, 1);
	}

	/* Register our module to receive incoming requests. */
	status = pjsip_endpt_register_module( app.sip_endpt, &mod_sipnagios);
	PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);

	/* Init calls */
	for (i=0; i<app.max_calls; ++i)
		app.call[i].index = i;

	/* Done */
	return PJ_SUCCESS;
}


/*
 * Destroy SIP
 */
static void destroy_sip()
{
	unsigned i;

	app.thread_quit = 1;
	for (i=0; i<app.thread_count; ++i) {
		if (app.sip_thread[i]) {
			pj_thread_join(app.sip_thread[i]);
			pj_thread_destroy(app.sip_thread[i]);
			app.sip_thread[i] = NULL;
		}
	}

	if (app.sip_endpt) {
		pjsip_endpt_destroy(app.sip_endpt);
		app.sip_endpt = NULL;
	}

}


/*
 * Init media stack.
 */
static pj_status_t init_media()
{
	unsigned	i, count;
	pj_uint16_t	rtp_port;
	pj_status_t	status;


	/* Initialize media endpoint so that at least error subsystem is properly
	 * initialized.
	 */
#if PJ_HAS_THREADS
	status = pjmedia_endpt_create(&app.cp.factory, NULL, 1, &app.med_endpt);
#else
	status = pjmedia_endpt_create(&app.cp.factory, 
			pjsip_endpt_get_ioqueue(app.sip_endpt),
			0, &app.med_endpt);
#endif
	PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);


	/* Must register codecs to be supported */
#if defined(PJMEDIA_HAS_G711_CODEC) && PJMEDIA_HAS_G711_CODEC!=0
	pjmedia_codec_g711_init(app.med_endpt);
#endif

	/* RTP port counter */
	rtp_port = (pj_uint16_t)(app.rtp_start_port & 0xFFFE);

	/* Init media transport for all calls. */
	for (i=0, count=0; i<app.max_calls; ++i, ++count) {

		unsigned j;

		/* Create transport for each media in the call */
		for (j=0; j<PJ_ARRAY_SIZE(app.call[0].media); ++j) {
			/* Repeat binding media socket to next port when fails to bind
			 * to current port number.
			 */
			int retry;

			app.call[i].media[j].call_index = i;
			app.call[i].media[j].media_index = j;

			status = -1;
			for (retry=0; retry<100; ++retry,rtp_port+=2)  {
				struct media_stream *m = &app.call[i].media[j];

				status = pjmedia_transport_udp_create2(app.med_endpt, 
						"sipnagios",
						&app.local_addr,
						rtp_port, 0, 
						&m->transport);
				if (status == PJ_SUCCESS) {
					rtp_port += 2;
					break;
				}
			}
		}

		if (status != PJ_SUCCESS)
			goto on_error;
	}

	/* Done */
	return PJ_SUCCESS;

on_error:
	destroy_media();
	return status;
}


/*
 * Destroy media.
 */
static void destroy_media()
{
	unsigned i;

	for (i=0; i<app.max_calls; ++i) {
		unsigned j;
		for (j=0; j<PJ_ARRAY_SIZE(app.call[0].media); ++j) {
			struct media_stream *m = &app.call[i].media[j];

			if (m->transport) {
				pjmedia_transport_close(m->transport);
				m->transport = NULL;
			}
		}
	}

	if (app.med_endpt) {
		pjmedia_endpt_destroy(app.med_endpt);
		app.med_endpt = NULL;
	}
}


/*
 * Make outgoing call.
 */
static pj_status_t make_call(const pj_str_t *dst_uri)
{
	unsigned i;
	struct call *call;
	pjsip_dialog *dlg;
	pjmedia_sdp_session *sdp;
	pjsip_tx_data *tdata;
	pj_status_t status;


	/* Find unused call slot */
	for (i=0; i<app.max_calls; ++i) {
		if (app.call[i].inv == NULL)
			break;
	}

	if (i == app.max_calls)
		return PJ_ETOOMANY;

	call = &app.call[i];

	/* Create UAC dialog */
	status = pjsip_dlg_create_uac( pjsip_ua_instance(), 
			&app.local_uri,	/* local URI	    */
			&app.local_contact,	/* local Contact    */
			dst_uri,		/* remote URI	    */
			dst_uri,		/* remote target    */
			&dlg);		/* dialog	    */
	if (status != PJ_SUCCESS) {
		++app.uac_calls;
		return status;
	}


	/* Give a unique Call-ID to dialog */
	enum {GUID_LEN = 36};
	char sguid[GUID_LEN + 1];
	uuid_t uuid;

	uuid_generate(uuid);
	uuid_unparse_lower(uuid, sguid);
	dlg->call_id->id = pj_str(sguid);

	/* If we expect the outgoing INVITE to be challenged, then we should
	 * put the credentials in the dialog here.
	 */

	pjsip_cred_info cred[1];

	cred[0].realm     = app.local_siprealm;
	cred[0].scheme    = pj_str("digest");
	cred[0].username  = app.local_user;
	cred[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
	cred[0].data      = app.local_password;

	pjsip_auth_clt_set_credentials( &dlg->auth_sess, 1, cred);

	/* Create SDP */
	create_sdp( dlg->pool, call, &sdp);

	/* Create the INVITE session. */
	status = pjsip_inv_create_uac( dlg, sdp, 0, &call->inv);
	if (status != PJ_SUCCESS) {
		pjsip_dlg_terminate(dlg);
		++app.uac_calls;
		return status;
	}


	/* Attach call data to invite session */
	call->inv->mod_data[mod_sipnagios.id] = call;

	/* Mark start of call */
	pj_gettimeofday(&call->start_time);


	/* Create initial INVITE request.
	 * This INVITE request will contain a perfectly good request and 
	 * an SDP body as well.
	 */
	status = pjsip_inv_invite(call->inv, &tdata);
	PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);


	/* Send initial INVITE request. 
	 * From now on, the invite session's state will be reported to us
	 * via the invite session callbacks.
	 */
	status = pjsip_inv_send_msg(call->inv, tdata);
	PJ_ASSERT_RETURN(status == PJ_SUCCESS, status);


	return PJ_SUCCESS;
}


/* Callback to be called when dialog has forked: */
static void call_on_forked(pjsip_inv_session *inv, pjsip_event *e)
{
	PJ_UNUSED_ARG(inv);
	PJ_UNUSED_ARG(e);

	PJ_TODO( HANDLE_FORKING );
}


/* Callback to be called to handle incoming requests outside dialogs: */
static pj_bool_t on_rx_request( pjsip_rx_data *rdata )
{
	/* Ignore strandled ACKs (must not send respone */
	if (rdata->msg_info.msg->line.req.method.id == PJSIP_ACK_METHOD)
		return PJ_FALSE;

	/* Respond (statelessly) any non-INVITE requests with 500  */
	if (rdata->msg_info.msg->line.req.method.id != PJSIP_INVITE_METHOD) {
		pj_str_t reason = pj_str("Unsupported Operation");
		pjsip_endpt_respond_stateless( app.sip_endpt, rdata, 
				500, &reason,
				NULL, NULL);
		return PJ_TRUE;
	}

	/* Handle incoming INVITE */
	//process_incoming_call(rdata);

	/* Done */
	return PJ_TRUE;
}


/* Callback timer to disconnect call (limiting call duration) */
static void timer_disconnect_call( pj_timer_heap_t *timer_heap,
		struct pj_timer_entry *entry)
{
	struct call *call = entry->user_data;

	PJ_UNUSED_ARG(timer_heap);

	entry->id = 0;
	hangup_call(call->index);
}


/* Callback to be called when invite session's state has changed: */
static void call_on_state_changed( pjsip_inv_session *inv, 
		pjsip_event *e)
{
	struct call *call = inv->mod_data[mod_sipnagios.id];

	PJ_UNUSED_ARG(e);

	if (!call)
		return;

	if (inv->state == PJSIP_INV_STATE_DISCONNECTED) {

		pj_time_val null_time = {0, 0};

		if (call->d_timer.id != 0) {
			pjsip_endpt_cancel_timer(app.sip_endpt, &call->d_timer);
			call->d_timer.id = 0;
		}

		PJ_LOG(3,(THIS_FILE, "Call #%d disconnected. Reason=%d (%.*s)",
					call->index,
					inv->cause,
					(int)inv->cause_text.slen,
					inv->cause_text.ptr));

		if (app.call_report) {
			PJ_LOG(3,(THIS_FILE, "Call #%d statistics:", call->index));
			//print_call(call->index);
		}

		if (app.call_monitoring) {
			PJ_LOG(3,(THIS_FILE, "Call #%d monitoring:", call->index));
			print_call2(call->index, app);
		}

		call->inv = NULL;
		inv->mod_data[mod_sipnagios.id] = NULL;

		destroy_call_media(call->index);

		call->start_time = null_time;
		call->response_time = null_time;
		call->connect_time = null_time;

		++app.uac_calls;

	} else if (inv->state == PJSIP_INV_STATE_CONFIRMED) {

		pj_time_val t;

		pj_gettimeofday(&call->connect_time);
		if (call->response_time.sec == 0)
			call->response_time = call->connect_time;

		t = call->connect_time;
		PJ_TIME_VAL_SUB(t, call->start_time);

		PJ_LOG(3,(THIS_FILE, "Call #%d connected in %d ms", call->index,
					PJ_TIME_VAL_MSEC(t)));

		if (app.duration != 0) {
			call->d_timer.id = 1;
			call->d_timer.user_data = call;
			call->d_timer.cb = &timer_disconnect_call;

			t.sec = app.duration;
			t.msec = 0;

			pjsip_endpt_schedule_timer(app.sip_endpt, &call->d_timer, &t);
		}

	} else if (	inv->state == PJSIP_INV_STATE_EARLY ||
			inv->state == PJSIP_INV_STATE_CONNECTING) {

		if (call->response_time.sec == 0)
			pj_gettimeofday(&call->response_time);

	}
}


/* Utility */
static void app_perror(const char *sender, const char *title, 
		pj_status_t status)
{
	char errmsg[PJ_ERR_MSG_SIZE];

	pj_strerror(status, errmsg, sizeof(errmsg));
	PJ_LOG(3,(sender, "%s: %s [status=%d]", title, errmsg, status));
}


/* Worker thread for SIP */
static int sip_worker_thread(void *arg)
{
	PJ_UNUSED_ARG(arg);

	while (!app.thread_quit) {
		pj_time_val timeout = {0, 10};
		pjsip_endpt_handle_events(app.sip_endpt, &timeout);
	}

	return 0;
}


/* Init application options */
static pj_status_t init_options(int argc, char *argv[])
{
	static char ip_addr[PJ_INET_ADDRSTRLEN];
	static char local_uri[64];

	enum { OPT_START,
		OPT_APP_LOG_LEVEL, OPT_LOG_FILE, 
		OPT_A_PT, OPT_A_NAME, OPT_A_CLOCK, OPT_A_BITRATE, OPT_A_PTIME,
		OPT_REPORT_FILE };

	struct pj_getopt_option long_options[] = {
		//{ "count",	    1, 0, 'c' },
		//{ "gap",            1, 0, 'g' },
		//{ "call-report",    0, 0, 't' },
		//{ "call-monitoring",0, 0, 'm' },
		{ "duration",	    1, 0, 'd' },
		//{ "auto-quit",	    0, 0, 'q' },
		{ "local-port",	    1, 0, 'p' },
		{ "rtp-port",	    1, 0, 'r' },
		{ "ip-addr",	    1, 0, 'i' },
		{ "local-user",	    1, 0, 'u' },
		{ "local-password", 1, 0, 'w' },
		{ "local-siprealm", 1, 0, 's' },

		{ "warn-durationms", 1, 0, 'A' },
		{ "warn-pddms", 1, 0, 'B' },
		{ "warn-ttams", 1, 0, 'C' },
		{ "warn-rxpkts", 1, 0, 'D' },
		{ "warn-rxbytes", 1, 0, 'E' },
		{ "warn-rxlosspct", 1, 0, 'F' },
		{ "warn-rxduppct", 1, 0, 'G' },
		{ "warn-rxreorderpct", 1, 0, 'H' },
		{ "warn-rxjitavg", 1, 0, 'I' },
		{ "warn-txpkts", 1, 0, 'L' },
		{ "warn-txbytes", 1, 0, 'M' },
		{ "warn-txlosspct", 1, 0, 'N' },
		{ "warn-txduppct", 1, 0, 'O' },
		{ "warn-txreorderpct", 1, 0, 'P' },
		{ "warn-txjitavg", 1, 0, 'Q' },
		{ "warn-rttmean", 1, 0, 'R' },

		{ "crit-durationms", 1, 0, 'S' },
		{ "crit-pddms", 1, 0, 'T' },
		{ "crit-ttams", 1, 0, 'U' },
		{ "crit-rxpkts", 1, 0, 'V' },
		{ "crit-rxbytes", 1, 0, 'Z' },
		{ "crit-rxlosspct", 1, 0, 'K' },
		{ "crit-rxduppct", 1, 0, 'J' },
		{ "crit-rxreorderpct", 1, 0, 'X' },
		{ "crit-rxjitavg", 1, 0, 'Y' },
		{ "crit-txpkts", 1, 0, 'v' },
		{ "crit-txbytes", 1, 0, 'z' },
		{ "crit-txlosspct", 1, 0, 'k' },
		{ "crit-txduppct", 1, 0, 'j' },
		{ "crit-txreorderpct", 1, 0, 'x' },
		{ "crit-txjitavg", 1, 0, 'y' },
		{ "crit-rttmean", 1, 0, 'b' },


		{ "log-level",	    1, 0, 'l' },
		{ "app-log-level",  1, 0, OPT_APP_LOG_LEVEL },
		{ "log-file",	    1, 0, OPT_LOG_FILE },

		//{ "report-file",    1, 0, OPT_REPORT_FILE },

		/* Don't support this anymore, see comments in USAGE above.
		   { "a-pt",	    1, 0, OPT_A_PT },
		   { "a-name",	    1, 0, OPT_A_NAME },
		   { "a-clock",	    1, 0, OPT_A_CLOCK },
		   { "a-bitrate",	    1, 0, OPT_A_BITRATE },
		   { "a-ptime",	    1, 0, OPT_A_PTIME },
		 */

		{ NULL, 0, 0, 0 },
	};
	int c;
	int option_index;

	/* Get local IP address for the default IP address */
	{
		const pj_str_t *hostname;
		pj_sockaddr_in tmp_addr;

		hostname = pj_gethostname();
		pj_sockaddr_in_init(&tmp_addr, hostname, 0);
		pj_inet_ntop(pj_AF_INET(), &tmp_addr.sin_addr, ip_addr,
				sizeof(ip_addr));
	}

	/* Init defaults */
	app.duration = 40;
	app.max_calls = 1;
	app.thread_count = 1;
	app.sip_port = 5060;
	app.rtp_start_port = RTP_START_PORT;
	app.local_addr = pj_str(ip_addr);
	app.log_level = 1;
	app.app_log_level = 1;
	app.log_filename = NULL;
	app.local_user = pj_str("alice");
	app.local_password = pj_str("1234");
	app.local_siprealm = pj_str("atlanta.example.com");
	app.call_report = PJ_FALSE;
	app.call_monitoring = PJ_FALSE;

	app.durationmsWARN = 100000 - 40000;
	app.pddmsWARN = 8000;
	app.ttamsWARN = 11000;

	app.rxpktsWARN =    10000 - 1000; 
	app.rxbytesWARN =   400000 - 150000; 
	app.rxlosspctWARN = 1.5;
	app.rxduppctWARN =  1.5;
	app.rxreorderpctWARN =1.5;
	app.rxjitavgWARN =   15.1;
	app.rxmosWARN =    5.0 - 4.0; 

	app.txpktsWARN =    10000 - 1000; 
	app.txbytesWARN =   400000 - 150000; 
	app.txlosspctWARN = 1.5;
	app.txduppctWARN =  1.5;
	app.txreorderpctWARN =1.5;
	app.txjitavgWARN =  15.1; 
	app.txmosWARN =    5.0 - 4.0; 

	app.rttmeanWARN =   150.1; 

	app.durationmsCRIT = 100000 - 20000;
	app.pddmsCRIT = 16000;
	app.ttamsCRIT = 20000;

	app.rxpktsCRIT =    10000 - 500; 
	app.rxbytesCRIT =   400000 - 75000; 
	app.rxlosspctCRIT = 3.0;
	app.rxduppctCRIT =  3.0;
	app.rxreorderpctCRIT =3.0;
	app.rxjitavgCRIT =   30.1;
	app.rxmosCRIT =    5.0 - 3.6; 

	app.txpktsCRIT =    10000 - 500; 
	app.txbytesCRIT =   400000 - 75000; 
	app.txlosspctCRIT = 3.0;
	app.txduppctCRIT =  3.0;
	app.txreorderpctCRIT =3.0;
	app.txjitavgCRIT =  30.1; 
	app.txmosCRIT =    5.0 - 3.6; 

	app.rttmeanCRIT =   300.1; 

	app.call_monitoring = PJ_TRUE;
	app.auto_quit = 1;
	/* Default codecs: */
	app.audio_codec = audio_codecs[0];

	/* Parse options */
	pj_optind = 0;
	while((c=pj_getopt_long(argc,argv, "A:B:C:D:E:F:G:H:I:L:M:N:O:P:Q:R:S:T:U:V:Z:K:J:X:Y:v:z:k:j:x:y:b:u:w:s:d:p:r:i:l:", 
					long_options, &option_index))!=-1) 
	{
		switch (c) {
			case 'c':
				app.rxmosWARN = atof(pj_optarg);
				break;
			case 'g':
				app.txmosWARN = atof(pj_optarg);
				break;
			case 't':
				app.rxmosCRIT = atof(pj_optarg);
				break;
			case 'm':
				app.txmosCRIT = atof(pj_optarg);
				break;
			case 'd':
				app.duration = atoi(pj_optarg);
				break;
			case 'p':
				app.sip_port = atoi(pj_optarg);
				break;
			case 'r':
				app.rtp_start_port = atoi(pj_optarg);
				break;
			case 'i':
				app.local_addr = pj_str(pj_optarg);
				break;
			case 'u':
				app.local_user = pj_str(pj_optarg);
				break;
			case 'w':
				app.local_password = pj_str(pj_optarg);
				break;
			case 's':
				app.local_siprealm = pj_str(pj_optarg);
				break;

			case 'l':
				app.log_level = atoi(pj_optarg);
				break;

			case 'A':
				app.durationmsWARN = 100000 - atol(pj_optarg);
				break;
			case 'B':
				app.pddmsWARN = atol(pj_optarg);
				break;
			case 'C':
				app.ttamsWARN = atol(pj_optarg);
				break;
			case 'D':
				app.rxpktsWARN = 10000 - atol(pj_optarg);
				break;
			case 'E':
				app.rxbytesWARN = 400000 - atol(pj_optarg);
				break;
			case 'F':
				app.rxlosspctWARN = atof(pj_optarg);
				break;
			case 'G':
				app.rxduppctWARN = atof(pj_optarg);
				break;
			case 'H':
				app.rxreorderpctWARN = atof(pj_optarg);
				break;
			case 'I':
				app.rxjitavgWARN = atof(pj_optarg);
				break;
			case 'L':
				app.txpktsWARN = 10000 - atol(pj_optarg);
				break;
			case 'M':
				app.txbytesWARN = 400000 - atol(pj_optarg);
				break;
			case 'N':
				app.txlosspctWARN = atof(pj_optarg);
				break;
			case 'O':
				app.txduppctWARN = atof(pj_optarg);
				break;
			case 'P':
				app.txreorderpctWARN = atof(pj_optarg);
				break;
			case 'Q':
				app.txjitavgWARN = atof(pj_optarg);
				break;
			case 'R':
				app.rttmeanWARN = atof(pj_optarg);
				break;

			case 'S':
				app.durationmsCRIT = 100000 - atol(pj_optarg);
				break;
			case 'T':
				app.pddmsCRIT = atol(pj_optarg);
				break;
			case 'U':
				app.ttamsCRIT = atol(pj_optarg);
				break;
			case 'V':
				app.rxpktsCRIT = 10000 - atol(pj_optarg);
				break;
			case 'Z':
				app.rxbytesCRIT = 400000 - atol(pj_optarg);
				break;
			case 'K':
				app.rxlosspctCRIT = atof(pj_optarg);
				break;
			case 'J':
				app.rxduppctCRIT = atof(pj_optarg);
				break;
			case 'X':
				app.rxreorderpctCRIT = atof(pj_optarg);
				break;
			case 'Y':
				app.rxjitavgCRIT = atof(pj_optarg);
				break;
			case 'v':
				app.txpktsCRIT = 10000 - atol(pj_optarg);
				break;
			case 'z':
				app.txbytesCRIT = 400000 - atol(pj_optarg);
				break;
			case 'k':
				app.txlosspctCRIT = atof(pj_optarg);
				break;
			case 'j':
				app.txduppctCRIT = atof(pj_optarg);
				break;
			case 'x':
				app.txreorderpctCRIT = atof(pj_optarg);
				break;
			case 'y':
				app.txjitavgCRIT = atof(pj_optarg);
				break;
			case 'b':
				app.rttmeanCRIT = atof(pj_optarg);
				break;


			case OPT_APP_LOG_LEVEL:
				app.app_log_level = atoi(pj_optarg);
				break;
			case OPT_LOG_FILE:
				app.log_filename = pj_optarg;
				break;

			case OPT_A_PT:
				app.audio_codec.pt = atoi(pj_optarg);
				break;
			case OPT_A_NAME:
				app.audio_codec.name = pj_optarg;
				break;
			case OPT_A_CLOCK:
				app.audio_codec.clock_rate = atoi(pj_optarg);
				break;
			case OPT_A_BITRATE:
				app.audio_codec.bit_rate = atoi(pj_optarg);
				break;
			case OPT_A_PTIME:
				app.audio_codec.ptime = atoi(pj_optarg);
				break;

			default:
				puts(USAGE);
				return 1;
		}
	}

	/* Check if URL is specified */
	if (pj_optind < argc)
		app.uri_to_call = pj_str(argv[pj_optind]);

	/* Build local URI and contact */
	pj_ansi_sprintf( local_uri, "sip:%s@%s", app.local_user.ptr, app.local_siprealm.ptr);
	app.local_uri = pj_str(local_uri);
	app.local_contact = app.local_uri;


	return PJ_SUCCESS;
}


/*****************************************************************************
 * MEDIA STUFFS
 */

/*
 * Create SDP session for a call.
 */
static pj_status_t create_sdp( pj_pool_t *pool,
		struct call *call,
		pjmedia_sdp_session **p_sdp)
{
	pj_time_val tv;
	pjmedia_sdp_session *sdp;
	pjmedia_sdp_media *m;
	pjmedia_sdp_attr *attr;
	pjmedia_transport_info tpinfo;
	struct media_stream *audio = &call->media[0];

	PJ_ASSERT_RETURN(pool && p_sdp, PJ_EINVAL);


	/* Get transport info */
	pjmedia_transport_info_init(&tpinfo);
	pjmedia_transport_get_info(audio->transport, &tpinfo);

	/* Create and initialize basic SDP session */
	sdp = pj_pool_zalloc (pool, sizeof(pjmedia_sdp_session));

	pj_gettimeofday(&tv);
	sdp->origin.user = pj_str("pjsip-sipnagios");
	sdp->origin.version = sdp->origin.id = tv.sec + 2208988800UL;
	sdp->origin.net_type = pj_str("IN");
	sdp->origin.addr_type = pj_str("IP4");
	sdp->origin.addr = *pj_gethostname();
	sdp->name = pj_str("pjsip");

	/* Since we only support one media stream at present, put the
	 * SDP connection line in the session level.
	 */
	sdp->conn = pj_pool_zalloc (pool, sizeof(pjmedia_sdp_conn));
	sdp->conn->net_type = pj_str("IN");
	sdp->conn->addr_type = pj_str("IP4");
	sdp->conn->addr = app.local_addr;


	/* SDP time and attributes. */
	sdp->time.start = sdp->time.stop = 0;
	sdp->attr_count = 0;

	/* Create media stream 0: */

	sdp->media_count = 1;
	m = pj_pool_zalloc (pool, sizeof(pjmedia_sdp_media));
	sdp->media[0] = m;

	/* Standard media info: */
	m->desc.media = pj_str("audio");
	m->desc.port = pj_ntohs(tpinfo.sock_info.rtp_addr_name.ipv4.sin_port);
	m->desc.port_count = 1;
	m->desc.transport = pj_str("RTP/AVP");

	/* Add format and rtpmap for each codec. */
	m->desc.fmt_count = 1;
	m->attr_count = 0;

	{
		pjmedia_sdp_rtpmap rtpmap;
		char ptstr[10];

		sprintf(ptstr, "%d", app.audio_codec.pt);
		pj_strdup2(pool, &m->desc.fmt[0], ptstr);
		rtpmap.pt = m->desc.fmt[0];
		rtpmap.clock_rate = app.audio_codec.clock_rate;
		rtpmap.enc_name = pj_str(app.audio_codec.name);
		rtpmap.param.slen = 0;

		pjmedia_sdp_rtpmap_to_attr(pool, &rtpmap, &attr);
		m->attr[m->attr_count++] = attr;
	}

	/* Add sendrecv attribute. */
	attr = pj_pool_zalloc(pool, sizeof(pjmedia_sdp_attr));
	attr->name = pj_str("sendrecv");
	m->attr[m->attr_count++] = attr;

	/*
	 * Add support telephony event
	 */
	m->desc.fmt[m->desc.fmt_count++] = pj_str("121");
	/* Add rtpmap. */
	attr = pj_pool_zalloc(pool, sizeof(pjmedia_sdp_attr));
	attr->name = pj_str("rtpmap");
	attr->value = pj_str("121 telephone-event/8000");
	m->attr[m->attr_count++] = attr;
	/* Add fmtp */
	attr = pj_pool_zalloc(pool, sizeof(pjmedia_sdp_attr));
	attr->name = pj_str("fmtp");
	attr->value = pj_str("121 0-15");
	m->attr[m->attr_count++] = attr;

	/* Done */
	*p_sdp = sdp;

	return PJ_SUCCESS;
}


#if (defined(PJ_WIN32) && PJ_WIN32 != 0) || (defined(PJ_WIN64) && PJ_WIN64 != 0)
#include <windows.h>
static void boost_priority(void)
{
	SetPriorityClass( GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
}

#elif defined(PJ_LINUX) && PJ_LINUX != 0
#include <pthread.h>
static void boost_priority(void)
{
#define POLICY	SCHED_FIFO
	struct sched_param tp;
	int max_prio;
	int policy;
	int rc;

	if (sched_get_priority_min(POLICY) < sched_get_priority_max(POLICY))
		max_prio = sched_get_priority_max(POLICY)-1;
	else
		max_prio = sched_get_priority_max(POLICY)+1;

	/*
	 * Adjust process scheduling algorithm and priority
	 */
	rc = sched_getparam(0, &tp);
	if (rc != 0) {
		app_perror( THIS_FILE, "sched_getparam error",
				PJ_RETURN_OS_ERROR(rc));
		return;
	}
	tp.sched_priority = max_prio;

	rc = sched_setscheduler(0, POLICY, &tp);
	if (rc != 0) {
		app_perror( THIS_FILE, "sched_setscheduler error",
				PJ_RETURN_OS_ERROR(rc));
	}

	PJ_LOG(4, (THIS_FILE, "New process policy=%d, priority=%d",
				policy, tp.sched_priority));

	/*
	 * Adjust thread scheduling algorithm and priority
	 */
	rc = pthread_getschedparam(pthread_self(), &policy, &tp);
	if (rc != 0) {
		app_perror( THIS_FILE, "pthread_getschedparam error",
				PJ_RETURN_OS_ERROR(rc));
		return;
	}

	PJ_LOG(4, (THIS_FILE, "Old thread policy=%d, priority=%d",
				policy, tp.sched_priority));

	policy = POLICY;
	tp.sched_priority = max_prio;

	rc = pthread_setschedparam(pthread_self(), policy, &tp);
	if (rc != 0) {
		app_perror( THIS_FILE, "pthread_setschedparam error",
				PJ_RETURN_OS_ERROR(rc));
		return;
	}

	PJ_LOG(4, (THIS_FILE, "New thread policy=%d, priority=%d",
				policy, tp.sched_priority));
}

#else
#  define boost_priority()
#endif


/*
 * This callback is called by media transport on receipt of RTP packet.
 */
static void on_rx_rtp(void *user_data, void *pkt, pj_ssize_t size)
{
	struct media_stream *strm;
	pj_status_t status;
	const pjmedia_rtp_hdr *hdr;
	const void *payload;
	unsigned payload_len;

	strm = user_data;

	/* Discard packet if media is inactive */
	if (!strm->active)
		return;

	/* Check for errors */
	if (size < 0) {
		app_perror(THIS_FILE, "RTP recv() error", (pj_status_t)-size);
		return;
	}

	/* Decode RTP packet. */
	status = pjmedia_rtp_decode_rtp(&strm->in_sess, 
			pkt, (int)size, 
			&hdr, &payload, &payload_len);
	if (status != PJ_SUCCESS) {
		app_perror(THIS_FILE, "RTP decode error", status);
		return;
	}

	//PJ_LOG(4,(THIS_FILE, "Rx seq=%d", pj_ntohs(hdr->seq)));

	/* Update the RTCP session. */
	pjmedia_rtcp_rx_rtp(&strm->rtcp, pj_ntohs(hdr->seq),
			pj_ntohl(hdr->ts), payload_len);

	/* Update RTP session */
	pjmedia_rtp_session_update(&strm->in_sess, hdr, NULL);

}

/*
 * This callback is called by media transport on receipt of RTCP packet.
 */
static void on_rx_rtcp(void *user_data, void *pkt, pj_ssize_t size)
{
	struct media_stream *strm;

	strm = user_data;

	/* Discard packet if media is inactive */
	if (!strm->active)
		return;

	/* Check for errors */
	if (size < 0) {
		app_perror(THIS_FILE, "Error receiving RTCP packet",(pj_status_t)-size);
		return;
	}

	/* Update RTCP session */
	pjmedia_rtcp_rx_rtcp(&strm->rtcp, pkt, size);
}


/* 
 * Media thread 
 *
 * This is the thread to send and receive both RTP and RTCP packets.
 */
static int media_thread(void *arg)
{
	enum { RTCP_INTERVAL = 5000, RTCP_RAND = 2000 };
	struct media_stream *strm = arg;
	char packet[1500];
	unsigned msec_interval;
	pj_timestamp freq, next_rtp, next_rtcp;


	/* Boost thread priority if necessary */
	boost_priority();

	/* Let things settle */
	pj_thread_sleep(100);

	msec_interval = strm->samples_per_frame * 1000 / strm->clock_rate;
	pj_get_timestamp_freq(&freq);

	pj_get_timestamp(&next_rtp);
	next_rtp.u64 += (freq.u64 * msec_interval / 1000);

	next_rtcp = next_rtp;
	next_rtcp.u64 += (freq.u64 * (RTCP_INTERVAL+(pj_rand()%RTCP_RAND)) / 1000);


	while (!strm->thread_quit_flag) {
		pj_timestamp now, lesser;
		pj_time_val timeout;
		pj_bool_t send_rtp, send_rtcp;

		send_rtp = send_rtcp = PJ_FALSE;

		/* Determine how long to sleep */
		if (next_rtp.u64 < next_rtcp.u64) {
			lesser = next_rtp;
			send_rtp = PJ_TRUE;
		} else {
			lesser = next_rtcp;
			send_rtcp = PJ_TRUE;
		}

		pj_get_timestamp(&now);
		if (lesser.u64 <= now.u64) {
			timeout.sec = timeout.msec = 0;
		} else {
			pj_uint64_t tick_delay;
			tick_delay = lesser.u64 - now.u64;
			timeout.sec = 0;
			timeout.msec = (pj_uint32_t)(tick_delay * 1000 / freq.u64);
			pj_time_val_normalize(&timeout);
		}

		/* Wait for next interval */
		pj_thread_sleep(PJ_TIME_VAL_MSEC(timeout));
		if (strm->thread_quit_flag)
			break;

		pj_get_timestamp(&now);

		if (send_rtp || next_rtp.u64 <= now.u64) {
			/*
			 * Time to send RTP packet.
			 */
			pj_status_t status;
			const void *p_hdr;
			const pjmedia_rtp_hdr *hdr;
			pj_ssize_t size;
			int hdrlen;

			/* Format RTP header */
			status = pjmedia_rtp_encode_rtp( &strm->out_sess, strm->si.tx_pt,
					0, /* marker bit */
					strm->bytes_per_frame, 
					strm->samples_per_frame,
					&p_hdr, &hdrlen);
			if (status == PJ_SUCCESS) {

				hdr = (const pjmedia_rtp_hdr*) p_hdr;

				/* Copy RTP header to packet */
				pj_memcpy(packet, hdr, hdrlen);

				/* Zero the payload */
				pj_bzero(packet+hdrlen, strm->bytes_per_frame);

				/* Send RTP packet */
				size = hdrlen + strm->bytes_per_frame;
				status = pjmedia_transport_send_rtp(strm->transport, 
						packet, size);
				if (status != PJ_SUCCESS)
					app_perror(THIS_FILE, "Error sending RTP packet", status);

			} else {
				pj_assert(!"RTP encode() error");
			}

			/* Update RTCP SR */
			pjmedia_rtcp_tx_rtp( &strm->rtcp, (pj_uint16_t)strm->bytes_per_frame);

			/* Schedule next send */
			next_rtp.u64 += (msec_interval * freq.u64 / 1000);
		}

		if (send_rtcp || next_rtcp.u64 <= now.u64) {
			/*
			 * Time to send RTCP packet.
			 */
			void *rtcp_pkt;
			int rtcp_len;
			pj_ssize_t size;
			pj_status_t status;

			/* Build RTCP packet */
			pjmedia_rtcp_build_rtcp(&strm->rtcp, &rtcp_pkt, &rtcp_len);


			/* Send packet */
			size = rtcp_len;
			status = pjmedia_transport_send_rtcp(strm->transport,
					rtcp_pkt, size);
			if (status != PJ_SUCCESS) {
				app_perror(THIS_FILE, "Error sending RTCP packet", status);
			}

			/* Schedule next send */
			next_rtcp.u64 += (freq.u64 * (RTCP_INTERVAL+(pj_rand()%RTCP_RAND)) /
					1000);
		}
	}

	return 0;
}


/* Callback to be called when SDP negotiation is done in the call: */
static void call_on_media_update( pjsip_inv_session *inv,
		pj_status_t status)
{
	struct call *call;
	struct media_stream *audio;
	const pjmedia_sdp_session *local_sdp, *remote_sdp;
	struct codec *codec_desc = NULL;
	unsigned i;

	call = inv->mod_data[mod_sipnagios.id];
	audio = &call->media[0];

	/* If this is a mid-call media update, then destroy existing media */
	if (audio->thread != NULL)
		destroy_call_media(call->index);


	/* Do nothing if media negotiation has failed */
	if (status != PJ_SUCCESS) {
		app_perror(THIS_FILE, "SDP negotiation failed", status);
		return;
	}


	/* Capture stream definition from the SDP */
	pjmedia_sdp_neg_get_active_local(inv->neg, &local_sdp);
	pjmedia_sdp_neg_get_active_remote(inv->neg, &remote_sdp);

	status = pjmedia_stream_info_from_sdp(&audio->si, inv->pool, app.med_endpt,
			local_sdp, remote_sdp, 0);
	if (status != PJ_SUCCESS) {
		app_perror(THIS_FILE, "Error creating stream info from SDP", status);
		return;
	}

	/* Get the remainder of codec information from codec descriptor */
	if (audio->si.fmt.pt == app.audio_codec.pt)
		codec_desc = &app.audio_codec;
	else {
		/* Find the codec description in codec array */
		for (i=0; i<PJ_ARRAY_SIZE(audio_codecs); ++i) {
			if (audio_codecs[i].pt == audio->si.fmt.pt) {
				codec_desc = &audio_codecs[i];
				break;
			}
		}

		if (codec_desc == NULL) {
			PJ_LOG(3, (THIS_FILE, "Error: Invalid codec payload type"));
			return;
		}
	}

	audio->clock_rate = audio->si.fmt.clock_rate;
	audio->samples_per_frame = audio->clock_rate * codec_desc->ptime / 1000;
	audio->bytes_per_frame = codec_desc->bit_rate * codec_desc->ptime / 1000 / 8;


	pjmedia_rtp_session_init(&audio->out_sess, audio->si.tx_pt, 
			pj_rand());
	pjmedia_rtp_session_init(&audio->in_sess, audio->si.fmt.pt, 0);
	pjmedia_rtcp_init(&audio->rtcp, "rtcp", audio->clock_rate, 
			audio->samples_per_frame, 0);


	/* Attach media to transport */
	status = pjmedia_transport_attach(audio->transport, audio, 
			&audio->si.rem_addr, 
			&audio->si.rem_rtcp, 
			sizeof(pj_sockaddr_in),
			&on_rx_rtp,
			&on_rx_rtcp);
	if (status != PJ_SUCCESS) {
		app_perror(THIS_FILE, "Error on pjmedia_transport_attach()", status);
		return;
	}

	/* Start media transport */
	pjmedia_transport_media_start(audio->transport, 0, 0, 0, 0);

	/* Start media thread. */
	audio->thread_quit_flag = 0;
#if PJ_HAS_THREADS
	status = pj_thread_create( inv->pool, "media", &media_thread, audio,
			0, 0, &audio->thread);
	if (status != PJ_SUCCESS) {
		app_perror(THIS_FILE, "Error creating media thread", status);
		return;
	}
#endif

	/* Set the media as active */
	audio->active = PJ_TRUE;
}



/* Destroy call's media */
static void destroy_call_media(unsigned call_index)
{
	struct media_stream *audio = &app.call[call_index].media[0];

	if (audio) {
		audio->active = PJ_FALSE;

		if (audio->thread) {
			audio->thread_quit_flag = 1;
			pj_thread_join(audio->thread);
			pj_thread_destroy(audio->thread);
			audio->thread = NULL;
			audio->thread_quit_flag = 0;
		}

		pjmedia_transport_detach(audio->transport, audio);
	}
}


/*****************************************************************************
 * USER INTERFACE STUFF
 */

static void call_get_duration(int call_index, pj_time_val *dur)
{
	struct call *call = &app.call[call_index];
	pjsip_inv_session *inv;

	dur->sec = dur->msec = 0;

	if (!call)
		return;

	inv = call->inv;
	if (!inv)
		return;

	if (inv->state >= PJSIP_INV_STATE_CONFIRMED && call->connect_time.sec) {

		pj_gettimeofday(dur);
		PJ_TIME_VAL_SUB((*dur), call->connect_time);
	}
}


static const char *good_number(char *buf, pj_int32_t val)
{
	if (val < 1000) {
		pj_ansi_sprintf(buf, "%d", val);
	} else if (val < 1000000) {
		pj_ansi_sprintf(buf, "%d.%02dK", 
				val / 1000,
				(val % 1000) / 100);
	} else {
		pj_ansi_sprintf(buf, "%d.%02dM", 
				val / 1000000,
				(val % 1000000) / 10000);
	}

	return buf;
}



static void hangup_call(unsigned index)
{
	pjsip_tx_data *tdata;
	pj_status_t status;

	if (app.call[index].inv == NULL)
		return;

	status = pjsip_inv_end_session(app.call[index].inv, 603, NULL, &tdata);
	if (status==PJ_SUCCESS && tdata!=NULL)
		pjsip_inv_send_msg(app.call[index].inv, tdata);
}


/*****************************************************************************
 * Below is a simple module to log all incoming and outgoing SIP messages
 */


/* Notification on incoming messages */
static pj_bool_t logger_on_rx_msg(pjsip_rx_data *rdata)
{
	PJ_LOG(4,(THIS_FILE, "RX %d bytes %s from %s:%d:\n"
				"%s\n"
				"--end msg--",
				rdata->msg_info.len,
				pjsip_rx_data_get_info(rdata),
				rdata->pkt_info.src_name,
				rdata->pkt_info.src_port,
				rdata->msg_info.msg_buf));


	//giovanni TODO
	pj_str_t event_hdr_name = pj_str("X-RTP-AVG-TMOS");
	pjsip_generic_string_hdr *event_hdr = (pjsip_generic_string_hdr*)pjsip_msg_find_hdr_by_name(rdata->msg_info.msg, &event_hdr_name, NULL);

	if (event_hdr){
		pj_str_t event_value = event_hdr->hvalue;

		char stats[1024];
		strncpy(stats, event_value.ptr, event_value.slen);
		stats[event_value.slen] = '\0';

		printf("X-RTP-AVG-TMOS:\n||%s||\n", stats);
	}
	/* Always return false, otherwise messages will not get processed! */
	return PJ_FALSE;
}

/* Notification on outgoing messages */
static pj_status_t logger_on_tx_msg(pjsip_tx_data *tdata)
{

	/* Important note:
	 *	tp_info field is only valid after outgoing messages has passed
	 *	transport layer. So don't try to access tp_info when the module
	 *	has lower priority than transport layer.
	 */

	PJ_LOG(4,(THIS_FILE, "TX %d bytes %s to %s:%d:\n"
				"%s\n"
				"--end msg--",
				(tdata->buf.cur - tdata->buf.start),
				pjsip_tx_data_get_info(tdata),
				tdata->tp_info.dst_name,
				tdata->tp_info.dst_port,
				tdata->buf.start));

	/* Always return success, otherwise message will not get sent! */
	return PJ_SUCCESS;
}

/* The module instance. */
static pjsip_module msg_logger = 
{
	NULL, NULL,				/* prev, next.		*/
	{ "mod-sipnagios-log", 14 },		/* Name.		*/
	-1,					/* Id			*/
	PJSIP_MOD_PRIORITY_TRANSPORT_LAYER-1,/* Priority	        */
	NULL,				/* load()		*/
	NULL,				/* start()		*/
	NULL,				/* stop()		*/
	NULL,				/* unload()		*/
	&logger_on_rx_msg,			/* on_rx_request()	*/
	&logger_on_rx_msg,			/* on_rx_response()	*/
	&logger_on_tx_msg,			/* on_tx_request.	*/
	&logger_on_tx_msg,			/* on_tx_response()	*/
	NULL,				/* on_tsx_state()	*/

};



/*****************************************************************************
 * Console application custom logging:
 */


static FILE *log_file;


static void app_log_writer(int level, const char *buffer, int len)
{
	/* Write to both stdout and file. */

	if (level <= app.app_log_level)
		pj_log_write(level, buffer, len);

	if (log_file) {
		pj_size_t count = fwrite(buffer, len, 1, log_file);
		PJ_UNUSED_ARG(count);
		fflush(log_file);
	}
}


pj_status_t app_logging_init(void)
{
	/* Redirect log function to ours */

	pj_log_set_log_func( &app_log_writer );

	/* If output log file is desired, create the file: */

	if (app.log_filename) {
		log_file = fopen(app.log_filename, "wt");
		if (log_file == NULL) {
			PJ_LOG(1,(THIS_FILE, "Unable to open log file %s", 
						app.log_filename));   
			return -1;
		}
	}

	return PJ_SUCCESS;
}


void app_logging_shutdown(void)
{
	/* Close logging file, if any: */

	if (log_file) {
		fclose(log_file);
		log_file = NULL;
	}
}


/*
 * main()
 */
int main(int argc, char *argv[])
{
	unsigned i;
	pj_status_t status;

	/* Must init PJLIB first */
	status = pj_init();
	if (status != PJ_SUCCESS)
		return 1;

	/* Get command line options */
	status = init_options(argc, argv);
	if (status != PJ_SUCCESS)
		return 1;

	/* Init logging */
	status = app_logging_init();
	if (status != PJ_SUCCESS)
		return 1;

	/* Init SIP etc */
	status = init_sip();
	if (status != PJ_SUCCESS) {
		app_perror(THIS_FILE, "Initialization has failed", status);
		destroy_sip();
		return 1;
	}

	/* Register module to log incoming/outgoing messages */
	pjsip_endpt_register_module(app.sip_endpt, &msg_logger);

	/* Init media */
	status = init_media();
	if (status != PJ_SUCCESS) {
		app_perror(THIS_FILE, "Media initialization failed", status);
		destroy_sip();
		return 1;
	}

	/* Start worker threads */
#if PJ_HAS_THREADS
	for (i=0; i<app.thread_count; ++i) {
		pj_thread_create( app.pool, "app", &sip_worker_thread, NULL,
				0, 0, &app.sip_thread[i]);
	}
#endif

	/* If URL is specified, then make call immediately */
	if (app.uri_to_call.slen) {
		PJ_LOG(3,(THIS_FILE, "Making %d calls to %s..", app.max_calls,
					app.uri_to_call.ptr));

		for (i=0; i<app.max_calls; ++i) {
			status = make_call(&app.uri_to_call);
			if (status != PJ_SUCCESS) {
				app_perror(THIS_FILE, "Error making call", status);
				break;
			}
			pj_thread_sleep(app.call_gap);
		}

		if (app.auto_quit) {
			/* Wait for calls to complete */
			while (app.uac_calls < app.max_calls)
				pj_thread_sleep(100);
			pj_thread_sleep(200);
		} else {
#if PJ_HAS_THREADS
			/* Start user interface loop */
			//console_main();
#endif
		}

	} else {
		puts(USAGE);
		return 1;

		PJ_LOG(3,(THIS_FILE, "Ready for incoming calls (max=%d)", 
					app.max_calls));

#if PJ_HAS_THREADS
		/* Start user interface loop */
		//console_main();
#endif
	}

#if !PJ_HAS_THREADS
	PJ_LOG(3,(THIS_FILE, "Press Ctrl-C to quit"));
	for (;;) {
		pj_time_val t = {0, 10};
		pjsip_endpt_handle_events(app.sip_endpt, &t);
	}
#endif

	/* Shutting down... */
	destroy_sip();
	destroy_media();

	if (app.pool) {
		pj_pool_release(app.pool);
		app.pool = NULL;
		pj_caching_pool_destroy(&app.cp);
	}

	app_logging_shutdown();

	/* Shutdown PJLIB */
	pj_shutdown();

	return 0;
}
