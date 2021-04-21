# sipnagios
Nagios Plugin to check Call Quality in SIP VoIP (compatible with checkmk, etc)

sipnagios implements the Nagios plugin API for monitoring and performance data.

sipnagios.c is a modification of the original siprtp.c sample in pjproject distribution. Supposedly, it works on Linux, Windows, and anywhere you can compile pjproject on.

It makes a call, checks all the various resulting values (mos, rtt, pdd, tta, jitter, packet loss, bytes and packets transferred, and so on). It verifies these values are included into acceptable, warning, or critical ranges.

If the call has gone well, sipnagios print performance data for Nagios graphs, and returns 0.

If the call fails, or if its measured values are not inside acceptable ranges, it exits with Nagios conventional WARNING or CRITICAL values.

# USAGE:
<pre>
 sipnagios [options] SIPURL

 Call options:
   --duration=SEC,   -d    Set maximum call duration in seconds (default:40)

 Address and ports options:
   --local-port=PORT,-p    Set local SIP port (default: 5060)
   --rtp-port=PORT,  -r    Set start of RTP port (default: 4000)
   --ip-addr=IP,     -i    Set local IP address to use (otherwise it will
                           try to determine local IP address from hostname)

 Local user options:
   --local-user=USER,       -u    Set local SIP user (default: alice)
   --local-password=PASSWD, -w    Set local SIP user password (default: 1234)
   --local-siprealm=REALM,  -s    Set local SIP user SIP realm (default: atlanta.example.com)

 Logging Options:
   --log-level=N,    -l    Set log verbosity level (default=1)
   --app-log-level=N       Set app log verbosity (default=1)
   --log-file=FILE         Write log to file FILE

NAGIOS RANGES OPTIONS DEFAULTS:

-c --rxmosWARN = 4.0
-g --txmosWARN = 4.0
-t --rxmosCRIT = 3.6
-m --txmosCRIT = 3.6

-A --durationmsWARN = 40000
-B --pddmsWARN = 8000
-C --ttamsWARN = 11000
-D --rxpktsWARN = 1000
-E --rxbytesWARN = 150000
-F --rxlosspctWARN = 1.5
-G --rxduppctWARN =  1.5
-H --rxreorderpctWARN =1.5
-I --rxjitavgWARN =   15.1
-L --txpktsWARN =  1000
-M --txbytesWARN = 150000
-N --txlosspctWARN = 1.5
-O --txduppctWARN =  1.5
-P --txreorderpctWARN =1.5
-Q --txjitavgWARN =  15.1
-R --rttmeanWARN =   150.1

-S --durationmsCRIT = 20000
-T --pddmsCRIT = 16000
-U --ttamsCRIT = 20000
-V --rxpktsCRIT = 500
-Z --rxbytesCRIT = 75000
-K --rxlosspctCRIT = 3.0
-J --rxduppctCRIT =  3.0
-X --rxreorderpctCRIT =3.0
-Y --rxjitavgCRIT =   30.1
-v --txpktsCRIT =  500
-z --txbytesCRIT = 75000
-k --txlosspctCRIT = 3.0
-j --txduppctCRIT =  3.0
-x --txreorderpctCRIT =3.0
-y --txjitavgCRIT =  30.1
-b --rttmeanCRIT =   300.1

==> rx and tx mos, durationms, rx and tx pkts and bytes becomes WARNING and CRITICAL if they're measured as LOWER than threshold
==> all other values becomes not OK if they are measured as HIGHER than threshold

Eg:
sipnagios --rtp-port=4000 --local-port=8090 --ip-addr=188.166.74.47 --local-user=9599 --local-siprealm=acme.cloudpbx.opentelecom.it --local-password=cAcyAgaC46AKuRk sip:0749941093@acme.cloudpbx.opentelecom.it:5030
(sipnagios is brought to you by Giovanni Maruzzelli. Enjoy!)
</pre>

#  HOW TO COMPILE SIPNAGIOS on DEBIAN:
(NB: To compile on Windows you'll need to come out with some other way to generate
a uuid for the Call-ID. In Linux, uuid_generate/uuid_unparse_lower are used. Patch welcome!)

<pre>
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
</pre>

# Install in CheckMk
<pre>
cp sipnagios /omd/sites/YOURSITENAME/local/lib/nagios/plugins/
chmod +x /omd/sites/YOURSITENAME/local/lib/nagios/plugins/sipnagios
</pre>

WATO -> Host & Service Parameters -> Active Checks (HTTP, TCP, etc)

# TODO
- Enhance Zabbix compatibility (if possible/needed)
- Let's use pjproject media interface for streaming an audio file and recording into a file, so we'll reuse all RTCP pjsip's goodies

# CREDITS
mos calculation is scraped from Julien Chavanton work (VoIP Patrol, on GitHub too) I can't even understand :) (merci Julien!)
https://github.com/jchavanton/voip_patrol/

pjproject: https://github.com/pjsip/pjproject
