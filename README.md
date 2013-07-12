=Description=

/!\ This is a poc, be indulgent, is 1 hour of work. /!\

Simple commande line for manage your dedibox.
For anyone who having fun. The concept is there. 
After that, there are more than to complete the structs in dedicli.h ;)

=Exemple=

	ja@x220:~/codaz/dedicli$ ./dedicli -t xxxxxxxxxxxxxxxxxxxxxxxxxxxxx -s 28xxx -i
	--- SRV Information ---
	id: 28xxx
	hostname: awaks
	power: ON
	os: debian
	version: v6.0.0
	--- DC location ---
	room:103 zone:PRO line:x rack:xx block:x position:xx

=Compilation=

You need libcurl for rest requests and jansson for json.

	apt-get install libcurl4-openssl-dev 
	apt-get install libjansson-dev

For compilation (make && gcc) :

	apt-get install build-essential

or

	apt-get install make gcc
