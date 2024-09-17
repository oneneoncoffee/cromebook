#!/bin/bash
echo Installing [Included dependencies]  [OK!]
sudo apt-get install libncurses5-dev libncursesw5-dev -y
echo Makeing gcc project... 
gcc -o blackjack blackjack.c  
gcc -o magpiecpu magpiecpu.c    
gcc -o magpielife magpielife.c  
gcc -o magpiesim magpiesim.c	   
gcc -o sums sums.c
gcc -o magpied magpied.c
gcc -o magpiell magpiell.c	  
gcc -o mus mus.c	
gcc magpiemem.c -o magpiemem -lncurses	   
gcc -o tarot tarot.c
gcc luxmus.c -o luxmus 
gcc magpiedir.c -o magpiedir    
gcc -o magpieman magpieman.c   
gcc -o numagie numagpie.c 
gcc -o tarotGold tarotGold.c
gcc -o magpie16 magpie16.c   
gcc -o magpiefrack magpiefrack.c  
gcc -o magpieproc magpieproc.c  
gcc -o search-magpie search-magpie.c
gcc -o magpieavg magpieavg.c  
gcc -o magpiefs magpiefs.c     
gcc -o magpies magpies.c	  
gcc -o square square.c -lm
gcc -o magpie magpie.c     
gcc -o magpiehelp magpiehelp.c   
gcc -o magpiescr magpiescr.c   
gcc -o statos statos.c
echo done.. [OK!]

