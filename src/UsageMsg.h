#define UsageMessage "List of Run Time Options (all optional): [hmquvVg:p:s:x:C:D:M:O:P:R:T:N:E:S:W:X:U:0:1:2:3:4:5:6:7:8:9:]\n" \
"h=Help u=UC_toggle v={Verbose, toggle EOJ stats} m={progress Meter} q={PBN Quiet mode} V={show Version info and exit}\n" \
"\nThese next switches all require values either integers or strings \n" \
"g={override Inputfile Generate} p={override Inputfile Produce}\n" \
"s={override Inputfile starting Seed for RNG} {x=eXchangeMode:2|3}\n" \
"C={Filename for CSV Report. (Precede with w: to truncate, else opened for append)}\n" \
"N:E:S:W={Compass predeal holding} O={OPC evaluation Opener(NSEW) Default=[W|S]}\n" \
"M={dds_Mode: 1=single solution, 2=20x solutions} R={Resources/Threads(1..9)}\n" \
"P={vulnerability for Par computation: 0=NoneVul, 1=NS, 2=EW, 3=Both}\n"  \
"T={Title in quotes} X={Filename to open for eXporting predeal holdings} U={DealerServer pathname}\n" \
"D={Debug verbosity level 0-9; (minimal effect in production version)}\n" \
"-0 to -9={set $0 thru $9 script parms in Inputfile one word or many in quotes}\n"
