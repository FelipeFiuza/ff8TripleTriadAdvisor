# Final Fantasy 8 Triple Triad Advisor


## Introduction
The objective of this project is to emulate the mini-game triple triad from final fantasy 8 (PC, PS1) and then develop a function to return which is the best play in a given scenario, by brute forcing every possible combination of plays.

Also i expect to generate optimal games, where each side always makes the best possible play, and use these to feed machine learning softwares. I'm planning to start my studies in this area, and having a solid and meaningful example to work with may help me to better absorb the concepts.

To minimize the overhead, and for being the first language I learnt, I chose c language for this project.

The Triple Triad game consists in a card game for two players, each with five cards, playing them one at a time, switching turns, on a board with 9 slots disposed on a grid 3x3.

A simple game: https://www.youtube.com/watch?v=fXADMoL8wbU

A complete explanation about the rules: https://www.youtube.com/watch?v=LBYtH97AvyA

A C online compiler if you want to try by yourself: https://www.onlinegdb.com/online_c_compiler

The cards are rectangular and have 4 numeric values, each value associated with each side. When a card is placed on a slot of the board, the numbers representing the adjacent sides of adjacents cards are used in a series of calculation, which depends on wich rules are turned on.
From these calculations the game determines which adversary card's you capture. Wins the game the player who ends up with most cards. There are several ways to capture the adversary card's.
Although simple in the beginning, this game add some complexity later on, and became really challenging. 


## To-Do
- Modify variables used to iterate over the board slots to stop relate to axis x and y, and relate to lines and columns
- Pass Slots by ID instead of numbers literals
- Function to iterate all possible plays in a given scenario


## Issues
#### - Does "Same" event enable use of cards values insted field values?
On this play https://www.youtube.com/watch?v=264wi-_Yxmw&t=64s Krysta card wouldn't be turned on my first undestanding. The '2-1 = 1' Phoenix right value wouldn't be able to turn the '1' value of Krysta card on slot 5. Apparently, the Phoenix card captured Krysta Card using its '2' card's right value ignoring the '2 - 1 = 1' value derived from the elemental mismatch. Further investigation needed.


## Calculation of possible scenarios
```
    Round	Cards On Hand	Spots Available	Possible Plays	Aggregated Possibilities	Possible Scenarios
        1	            5	              9	            45	                      45	        5225472000
        2	            5	              8	            40	                    1800	         116121600
        3	            4	              7	            28	                   50400	           2903040
        4	            4	              6	            24	                 1209600	            103680
        5	            3	              5	            15	                18144000	              4320
        6	            3	              4	            12	               217728000	               288
        7	            2	              3	             6	              1306368000	                24
        8	            2	              2	             4	              5225472000	                 4
        9	            1	              1	             1	              5225472000	                 1
```

