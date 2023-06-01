#define UsageMessage "List of Run Time Options (all optional): [hmquvVg:p:s:x:C:D:L:M:O:P:R:T:N:E:S:W:X:U:0:1:2:3:4:5:6:7:8:9:]\n" \
"h=Help                 u=UC_toggle            v={Verbose, toggle EOJ stats}\n"         \
"m={progress Meter}     q={PBN Quiet mode}     V={show Version info and exit}\n" \
"\nThese next switches all require values either integers or strings \n" \
"g={override Inputfile Generate}                \np={override Inputfile Produce}\n" \
"s={override Inputfile starting Seed for RNG} \nx={eXchangeMode:2|3}\n" \
"C={Filename for CSV Report. (Precede with w: to truncate, else opened for append)}\n" \
"N:E:S:W={Compass predeal holding}    \n" \
"O={OPC evaluation Opener(NSEW) Default=[W|S]}\n" \
"L={Use Path as RP Library source. Default Path = ../rpdd.zrd \n"\
"M={dds_Mode: 1=single solution, 2=20x solutions}  \nR={Resources/Threads(1..9)}\n" \
"P={vulnerability for Par computation: 0=NoneVul, 1=NS, 2=EW, 3=Both}\n"  \
"T={Title in quotes}    \nU={DealerServer pathname}\n" \
"X={Filename to open for eXporting predeal holdings}\n" \
"D={Debug verbosity level 0-9[.0-9]; (minimal effect in production version)}\n" \
"-0 to -9={set $0 thru $9 script parms in Inputfile one word or many in quotes}\n"
