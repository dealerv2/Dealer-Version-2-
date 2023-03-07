### Note from JGM on 2022/03/13
### This is François Dellacherie's orginal (1997) Readme file for his pre-processor which he called DPP.
### JGM has renamed from 'dpp' to 'fdp' to give credit where it is due.
### JGM has added a couple of extra explanations at the end of FD's original text.
###  --- This file is included to give credit to Francois by preserving his original documentation ---
###########################################################################################################

### DPP README ###

1/ What is dpp ?
----------------
dpp stands for dealer-pre-processor. Its main purpose it to preprocess
input files for Hans van Staveren's great dealer program.

2/ How is it written ?
----------------------
dpp is written in Perl-object. In fact, it was a small part of a
package that I will probably never finish to write (a package that
should do automatic checking of bidding conventions...).

It contains two files:
o the executable file 'dpp',
o the package 'Dist.pm'.

3/ What do I need to make it work ?
-----------------------------------
a/ You need Perl (and anyway, even if you don't plan to use dpp, you
   probably need perl, with or without knowing it !),

b/ You may need to change the first line of dpp, depending on where
   your perl interpreter is located, and where you plan to install the
   package file 'Dist.pm'. 

c/ You need to read this...

4/ What can I do with it ?
--------------------------
The greatest idea of Hans van Staveren was his 'shape' function
(please reread his manual). The 'shape' function speed is the same for
any number of args in it. Unfortunately, there's no convenient way to
express ideas like "a balanced shape with 4 in a major". With the
regular dealer, you have to write :

shape(north,2443 + 2434 + 3424 + 3442 + 3433 + 4243 + 4423 + 4234 +
            4432 + 4324 + 4342 + 4333)

or, if you are clever :

shape(north, any 4432 + any 4333 - 3343 - 3334 - 2344 - 3244)

With dpp, you will just write :

shape{north, 4M(3+3+2+)}

nice isn't it ??


5/ Differences with the original syntax of shape :
--------------------------------------------------
Perl is great. (Repeat after me...)
The only thing it misses regarding text-parsing is the ability to
handle sexp (in a more general case, I don't recommend having sex-p with a
computer ;-).
The bad news is that I had to change the parenthesis () into 
braces {}, because I wanted to use () for other purposes inside the
shape-func. The only other difference is that shape doesn't recognize
the word 'any' anymore. 
(I confess I may have kept the old syntax and invent a new word
 like dist{}. If people complain about this, I may change it...
 I just wanted to keep HVS-soul alive in my files...)

The new general syntax is :

shape{COMPASS, shp1 + shp2 + ... + shpM - shm1 - ... - shmN}  ### JGM Note: The '+' shapes all come before any '-' shape.

See below for the atomic (shp or shm) shapes...


6/ Features :
-------------
when you see a shape after a '-->' signs, it's the result of a shape
after being dp-processed...

(a) regular shapes :
    7xxx, 4432, 4xx5 have the same meaning as in original dealer.

(b) the 'at least' operator :
    whenever you put a '+' after a suit-length, it means 'at least'.
    Example:
    shape{north, 5+xx5+} : at least 5/5 in the blacks,
 -->shape(north,5xx5 + 5xx6 + 6xx5 + 5xx7 + 6xx6 + 7xx5 + 5008 + 6007
                + 7006 + 8005)

(c) the 'at most' operator :
    same as the 'at least', just use a '-' :
    Example:
    shape{north, 2-xxx} : at most 2 spades,
 -->shape(north,0xxx + 1xxx + 2xxx)

(d) the 'range' operator :
    the range operator operates on a single suit. It comes with [], like in Perl.
    Example:
    shape{north, x[3-5]x[13]} : (3 to 5) hearts, and one or 3 clubs 
 -->shape(north,x3x1 + x4x1 + x3x3 + x5x1 + x4x3 + x5x3)
 
    Note that shape{north, 3-xxx} is equivalent to
              shape{north,[0-3]xxx}
    You can write compound ranges if you want :
    shape{north, [013-68]xxx} means: 0,1,3,4,5,6 or 8 spades.

(e) the 'permutation' operator :
    the permutation operator will appears as () like in the WBF rules
    of specifying shapes.
    Example :
    shape{north, (4432)} : any 4432 hand.
 -->shape(north,any 4432)

    what's best is that the () operator can operate on restricted groups
    of cards :
    Example: 
    shape{north, 5s(431)} : 5 spades, the others suits being (431).
 -->shape(north,5134 + 5314 + 5143 + 5341 + 5413 + 5431)      

    What you have to do is :
    o specify the suit names (s,h,d,c in english) in the left part of
      the shape, in any order, 
    o and enclose the rest of the permutation into () in the right part.
    
    A (complicated) example :
    shape{north, 4+c3+d(2+2+)} : at least 5 clubs, at least 3 diamonds, 
                               and no short major.
 -->shape(north,3334 + 4234 + 2434 + 3244 + 2344 + 2254 + 3235 + 2335 + 2245 + 2236)

(f) the 'Major' operator :
    has to be used together with the () operator. 'M' stands for 'a major'
    The 'M' operator can only be used in the left part (never put a M
    inside the parens)
    Examples :
    shape{north, 5M(xxx)}
 -->shape(north,x5xx + 5xxx)

    shape{north, 5+M3+c(31)}
 -->shape(north,1534 + 3514 + 1633 + 3613 + 5134 + 5314 + 6133 + 6313)

(g) the 'minor' operator :
    same as the 'Major' operator, but in lowercase 'm'.
    Example :
    shape{north, 5M5m(xx)}
 -->shape(north,x5x5 + 5xx5 + x55x + 5x5x)

(h) the 'conditional' operator :
    you've got the possibility of specifying complex conditions to be
    verified that cannot be simply expressed by other operators. In
    order  to do that, you use the ':' operator, just after the
    atomic-shape specification :
    shape{north, shp1:condition1,condition2,...}
    
    The conditions use the letters (s,h,d,c) as variables for the suit
    lengths, and the usual C-operators (*,+,-,/,>,<,==,!,?, and so on)
    Note that the ',' stands for the 'and-logical' operator. You can
    use 'or' for the 'or-logical' operator.

    Example:
    shape{north, 4+s4+h(xx):d>c,h+s==10}
 -->shape(north,6421 + 6430 + 5521 + 5530 + 4621 + 4630)


7/ Some other examples :
------------------------
a balanced hand          : (3+3+3+2+)
a french 1H opening      : x5+xx:h>s,h>=d,h>=c
a michaels (spade+minor) : 5+s5+m(xx)
my No_trump opening      :
   hn = hcp(north)
   n1NT = (hn > 14 and hn < 18 and shape{north, 5m(332) + 5m2s(42)} )  or
          (hn > 13 and hn < 17 and shape{north, 6m(322)}            )  or
          (hn > 15 and hn < 18 and shape{north, 5M(332)}            )  or
          (hn > 15 and hn < 19 and shape{north, (4432) + (4333)}    )


8/ How to use dpp :
-------------------
a/ Unix world    : dpp my_input_file | dealer
b/ Windows/DOS world :
   I don't know. I've never used windows/DOS in my life :-).
   I guess 2 possibilities :
   (i) 'dpp my_input_file > dppfile' followed by 'dealer dppfile'
  (ii) Include the dpp code inside the dealer code.
       Maybe somebody can do that on the next dealer distribution, and
       add a '-dpp' option to dealer. I won't do it, since I cannot
       check if that works properly.

c/ you can also try some shapes by typing 'dpp' on the command line, ### JGM Note: use 'fdpi' not dpp or fdp .... 
   and enter shape{} one by one to see how it transforms them.


9/ Languages :
--------------
The suit specifications can be made in french or english.
The default mode is english.
To change the mode, you can do 2 things:
(i) add a 
    Dist->language('French');
    in dpp (after the Dist->new)

(ii) Change the DefaultLanguage value in Dist.pm

If somebody wants to add other languages, it's possible...

10/ Bugs and further developments :
----------------------------------
If you :
- notice bugs or,
- don't understand a peculiar behaviour of dpp (but please reread this
  README first),
- have suggestions for further developments,

you can e-mail me at sr007-2@dial.oleane.com

On the other hand, if you :
- don't understand the *code*,
- cannot install perl on your machine,

there is unfortunately nothing that I can do for you. 

Quoting Hans van Staveren :

<< This program is hereby put in the public domain. Do with it whatever
you want, but I would like you not to redistribute it in modified form
without mentioning the fact of modification. I will accept bug reports
and modification requests, without any obligation of course, but fixing
bugs someone else put in is beyond me.

When you report bugs please mention the version number in the source
files, and preferably send context diffs if you changed anything.  I
might put in your fixes, and ask Henk to distribute a new version
someday. >>


Have Fun !
François DELLACHERIE
 < email deleted by JGM because it does not work anymore >
Paris, FRANCE
###############################################################################
################# Further comments by JGM 2022/03/13 ##########################
1) Several bugs in the original DPP code have been fixed in 2022.
2) François' comment about not handling signs (sexp) properly has been fixed
   provided that you enter all your plus shapes before any minus shape. Dealer itself seems to 
   work that way anyway so it should not be a big burden.
3) The dpp program has been replaced by two programs: 'fdp' for use by Dealer itself, and 
   'fdpi' for use by users who want to enter shapes interactively from the command line.
4) you need to specify the complete shape specification ( all on one line)  not just the distribution
   Ex: do NOT enter:             4+M(431) - 2-m(xxx) - 5+mxxx:h>s
       do enter    : shape{west, 4+M(431) - 2-m(xxx) - 5+mxxx:h>s }
   and you will see as a result: 
Parsing:: shape{west, 4+M(431) - 2-mxxx - 5+mxxx:h>s }
[Length:969, patterns: 137] shape(west,1534 + 1543 + 3514 + 3541 + 4513 + 4531 + 5134 + 5143 + 5314 + 5341 + 5413 + 5431 - xx0x - 
xx1x - xx2x - xxx0 - xxx1 - xxx2 - 0139 - 0148 - 0157 - 0166 - 0175 - 0184 - 0193 - 0229 - 0238 - 0247 - 0256 - 0265 - 0274 - 0283 - 
0292 - 0319 - 0328 - 0337 - 0346 - 0355 - 0364 - 0373 - 0382 - 0391 - 0409 - 0418 - 0427 - 0436 - 0445 - 0454 - 0463 - 0472 - 0481 - 
0490 - 0508 - 0517 - 0526 - 0535 - 0553 - 0562 - 0571 - 0580 - 0607 - 0616 - 0625 - 0652 - 0661 - 0670 - 0706 - 0715 - 0751 - 0760 - 
0805 - 0850 - 1219 - 1228 - 1237 - 1246 - 1255 - 1264 - 1273 - 1282 - 1291 - 1309 - 1318 - 1327 - 1336 - 1345 - 1354 - 1363 - 1372 - 
1381 - 1390 - 1408 - 1417 - 1426 - 1435 - 1453 - 1462 - 1471 - 1480 - 1507 - 1516 - 1525 - 1552 - 1561 - 1570 - 1606 - 1615 - 1651 - 
1660 - 1705 - 1750 - 2308 - 2317 - 2326 - 2335 - 2353 - 2362 - 2371 - 2380 - 2407 - 2416 - 2425 - 2452 - 2461 - 2470 - 2506 - 2515 - 
2551 - 2560 - 2605 - 2650 - 3406 - 3415 - 3451 - 3460 - 3505 - 3550)







