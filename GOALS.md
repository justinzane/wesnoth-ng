Title: justinzane/wesnoth-old GOALS
Category: foss
Tags: Wesnoth, c++, development, planning

# GOALS

Goals, plans, thought, analysis and general silliness regarding the justinzane fork of 
Battle for Wesnoth.

**Note**: These are written by an amateur programmer as a hobby project. Nothing here is to be 
taken as a disrespectful criticism of the many much more skilled and experience coders who have 
contributed their effort and creativity to Wesnoth. If you disagree, *please* contact the author
on #wesnoth or by [email](mailto:justin@justinzane.com).  

## General Coding

### WRITE COMMENTS

A very large part of the codebase is completely without documentation or 
inline comments. This makes it very challenging for newcomers to learn and inhibits contributions
to the project. My suggestion is that first, comments be written; and that **all** doc and 
header comments be formatted in doxygen style.

### ORGANIZATION

Currently much of the codebase is mingled together in the `src/` directory 
which makes navigation frustrating since there are ~350 entries in that directory. My suggestion 
is to move related source files into their own directories, i.e. `mv unit*.[ch]pp unit/`.

- When not too problematic, create wrapper headers in `src/` so that API clients can use one 
simple `#include` instead of several. For example `unit.hpp`:
    
            '''c++
            /** header comment */
            #ifndef WESNOTH_UNIT_WRAPPER_GUARD
            #include "unit/unit-blah.hpp"
            #include "unit/unit-foo.hpp"
            #include "unit/unit-bar.hpp"
            #endif
            '''
### Naming Clarifications

Though this is certainly not a priority for performance or functionality, I would prefer to see
object names that are potentially ambiguous like "map" refactored to reduce confusion. In this 
particular example, a map is both a c++ standard container `std::map<key_t, value_t>` and 
a game concept -- the playing space. By renaming `map.hpp`, `map_utils.hpp`, etc. to `board.hpp`, 
`board_utils.hpp`, etc. it becomes clear that that class is not a key-value container type, but 
a core game type.

## Parallelization

As just about every device capable of playing Wesnoth now has, at minimum, multiple CPU cores 
and many have multiple compute devices, like OpenCL capable GPUs, maintaining the code to be 
thread and coprocessor friendly seems like an ideal practice. Of course, countless PhDs have 
written their theses on this subject; and, it is not at all clear and simple. Not, am I in any 
way expert in these matters.

My initial suggestions in this area are twofold. First, those who **do** have expertise in the 
various levels of parallelism are requested to provide guidelines about writing or refactoring 
to enable clean and easy parallelization in the future.

One thing that I believe is important in this respect, as well as in others, is to keep Wesnoth's 
codebase as "library-like" as possible. That is to say, to make sure that users of a class do 
not leverage their knowledge of the internals of the class, but just use its clearly defined 
public API. Another is that all the functionality relating to a Wesnoth conceptual topic, for example units 
or time_of_days, be organized together and separated from other conceptual topics as much as 
possible.    

### First Places to Parallelize

There seem to be two functional areas that are very time-consuming and look to be ideal 
candidates for parallelization. The first is serialization, which I will discuss later. The 
second is situational analysis -- the part of an AI's mechanism for determining the optimal 
actions for all its units of responsibility. Since many AI strategies examine this from the 
perspective of each individual unit, the ability to create *tasks* that can be allocated to 
processing pools (thread pools, opencl kernels, __future_tool__, etc.) and run in parallel 
would have great benefits. 

Either simpler AI algorithms could run at improved speed on low-power devices or complex 
algorithms could run within the limits of player annoyance on higher performance systems. 

## Separation of Logic from Presentation

In the publishing world, there is a clear separation of design creation from content writing. 
In Wesnoth, I believe that it would be good to clearly separate the visual/audio presentation 
code from the game logic code. While display obviously is wholely dependent on logic, the 
logic is almost entirely independent on the display. 

*TODO* define what this means in detail.

## Serialization

### ONCE AND ONLY ONCE

Though I am not cognizant of all the thought and planning and changes 
that went into Wesnoth's Markup Language, WML, I can appreciate how thoroughly it is integrated 
into the codebase and into the minds of Wesnoth users and developers. So, I know that changes to
or replacement of WML for serialization will be difficult and inflammatory.

That said, though I am strongly suspicious that an existing human readable markup/serialization 
language like XML, json, yaml, etc. would provide all the capabilities of WML and shift the 
basics of parsing and writing to common and well-optimized and supported libraries; I am not so 
sure that there is any benefit to be had from forcing the porting of **every** map, scenarios, 
unit definition, etc. from WML into json or whatever.

Where I see clear and overwhelming benefits from replacing WML is in general the in-game ser-des
routines. For complex campaigns like LotI, playing one scenario all the way through can lead to 
almost **100GB** of memory allocations and deallocations, most of which seem to come from the 
ser-des process at end-of-turn as well as significant contributions from the constant 
manipulation of attributes that are stored as `std::string`.

Though I am, as yet, thoroughly unclear as to the full details and complications, I envision 
adding the ability to serialize to either a simple bitstream or to json. Though the bitstream 
has the advantage of being able to be persisted to any preferred location, it does little to 
reduce the enormous redundancy in the current serialization methodology. 

As an example, I examined a save file: `LotI1-The_Battle_for_Ogira_Turn_6`. Running each line 
through a simple Python script to check for exact duplicates as well as lines with an edit 
distance (Levenshtein edit distance) of less than five. Here are the details:

`Total: 00340057, Exact: 116282 [34.2%], < 5: 217820 [64.1%]`

From a total of over **350000** lines from **one** scenario, over **100000** -- over a third --
are **exact** duplicates. Similarly, two thirds of the lines are within **4** characters of a base.
In my not-so-humble opinion, that alone is a huge cry for helpful refactoring. 

While modern x86_64 systems can handle that volume of text seemingly instantly, that is a huge 
burden on the resources of low-power, mobile devices. With the increasing popularity of tablets, 
chromebooks, monster-phones and whatnot, as well as the availability of FOSS or similar stacks 
on these devices, it seems silly to exclude them for want of efficient serialization.

To beat on this deceased ungulate some more, I looked at how many tags in that save file had 
exactly the same contents. (The script code is below.) The results are mind-boggling:

| TAG                         | Unique   | Uniq Chars | Duplicate| Dup Chars  | Waste Pct|
| ---------------------------:| --------:| ----------:| --------:| ----------:| --------:|
|                   abilities |      127 |      80361 |      837 |     466712 |    759.1%|
|                     advance |       47 |      24175 |      181 |      43814 |    485.1%|
|                   advanced2 |        2 |      25752 |        2 |      25752 |    200.0%|
|                 advancefrom |       12 |        556 |       37 |       1675 |    408.3%|
|                 advancement |      116 |     171817 |     1048 |    1122540 |   1003.4%|
|                    advances |       14 |        669 |       61 |        244 |    535.7%|
|             affect_adjacent |       15 |       1169 |      663 |      40565 |   4520.0%|
|                          ai |        5 |      44129 |       11 |      84589 |    320.0%|
|                      ai_log |        1 |       6319 |        0 |          0 |    100.0%|
|                  allow_undo |        2 |         10 |        2 |         10 |    200.0%|
|                      amulet |        1 |         16 |        1 |         16 |    200.0%|
|                         and |      125 |      13077 |      593 |      53977 |    574.4%|
|                animate_unit |        2 |        369 |        2 |        369 |    200.0%|
|                   animation |       12 |      13266 |       94 |      83903 |    883.3%|
|                      armour |        3 |         49 |        5 |         81 |    266.7%|
|                      aspect |       54 |      14614 |      314 |      73602 |    681.5%|
|                      attack |      314 |     333843 |      712 |     767612 |    326.8%|
|                 attack_anim |       19 |      26178 |      178 |     267079 |   1036.8%|
|                     attacks |       73 |      27376 |       13 |       2250 |    117.8%|
|                     berserk |        7 |       1754 |      433 |      87568 |   6285.7%|
|            candidate_action |       21 |       2859 |      139 |      18839 |    761.9%|
|             carryover_sides |        1 |     962483 |        0 |          0 |    100.0%|
|       carryover_sides_start |        1 |    1636795 |        0 |          0 |    100.0%|
|                        case |      101 |      82686 |      215 |     117322 |    312.9%|
|               chance_to_hit |       11 |       2638 |      588 |     142529 |   5445.5%|
|                    checksum |      443 |      70304 |        8 |       1269 |    101.8%|
|                      choose |        2 |         28 |        6 |         84 |    400.0%|
|              clear_variable |      203 |       8335 |      865 |      26638 |    526.1%|
|                     command |     1218 |     506783 |      564 |     276216 |    146.3%|
|                      damage |       72 |      29187 |     1126 |     283901 |   1663.9%|
|                      deaths |       52 |       3825 |       23 |        176 |    144.2%|
|                     default |       15 |       1129 |      353 |      24807 |   2453.3%|
|                      defend |        3 |       1493 |       37 |      18267 |   1333.3%|
|                     defends |       69 |      26229 |        6 |         24 |    108.7%|
|                     defense |       29 |       5789 |      479 |      94306 |   1751.7%|
|                       delay |        2 |         38 |        4 |         76 |    300.0%|
|                 description |        9 |        715 |        9 |        715 |    200.0%|
|                 destination |       67 |       1464 |       74 |       1618 |    210.4%|
|            disallow_recruit |        1 |        104 |        1 |        104 |    200.0%|
|                     display |        1 |         86 |        1 |         86 |    200.0%|
|                          do |      136 |     318644 |      229 |     466195 |    268.4%|
|                      drains |        7 |       1350 |      449 |      80674 |   6514.3%|
|                       dummy |       60 |      11871 |      326 |      61457 |    643.3%|
|                      effect |      387 |     193739 |     6346 |    1255568 |   1739.8%|
|                        else |      156 |     160031 |      729 |     385524 |    567.3%|
|                    endlevel |        3 |        176 |        9 |        308 |    400.0%|
|              end_level_data |        1 |        101 |        1 |        101 |    200.0%|
|                    end_turn |        1 |          3 |       39 |        117 |   4000.0%|
|                      engine |        2 |        107 |       19 |        894 |   1050.0%|
|                       event |      151 |     947132 |      139 |     932558 |    192.1%|
|                       facet |        8 |       1310 |        0 |          0 |    100.0%|
|                      facing |        1 |         56 |        1 |         56 |    200.0%|
|                      filter |      140 |      11518 |      784 |      39575 |    660.0%|
|             filter_adjacent |       30 |       1627 |     3561 |     230576 |  11970.0%|
|    filter_adjacent_location |        3 |        210 |        5 |        324 |    266.7%|
|               filter_attack |       61 |       1761 |      324 |      10637 |    631.1%|
|           filter_base_value |        5 |        235 |       21 |       1014 |    520.0%|
|             filter_location |       18 |       1457 |      151 |       6567 |    938.9%|
|             filter_opponent |        9 |        810 |       50 |       4022 |    655.6%|
|               filter_radius |        1 |         43 |        5 |        215 |    600.0%|
|               filter_recall |        1 |          4 |      476 |       1904 |  47700.0%|
|               filter_second |       29 |       1134 |       83 |       2966 |    386.2%|
|        filter_second_attack |       16 |        522 |       52 |       1606 |    425.0%|
|                 filter_self |       14 |       1504 |     3540 |     407081 |  25385.7%|
|                 filter_side |        2 |         66 |       12 |        390 |    700.0%|
|                  filter_wml |       14 |        789 |       42 |       2299 |    400.0%|
|                  fire_event |       48 |       4000 |      470 |      28368 |   1079.2%|
|                 firststrike |        3 |        522 |      799 |     150641 |  26733.3%|
|               floating_text |       11 |        927 |       49 |       3131 |    545.5%|
|                  formula_ai |        1 |          5 |        6 |         30 |    700.0%|
|                       frame |       91 |       8959 |     1354 |     134400 |   1587.9%|
|                        from |        9 |        193 |      161 |       3449 |   1888.9%|
|                        gold |        5 |        296 |        5 |        296 |    200.0%|
|                   harm_unit |       49 |      14311 |       51 |      15151 |    204.1%|
|                   have_unit |       11 |       1288 |       15 |       1684 |    236.4%|
|                       heals |       13 |       7149 |       73 |      39752 |    661.5%|
|                   heal_unit |       22 |       2920 |       24 |       3206 |    209.1%|
|                        helm |        1 |         16 |        3 |         48 |    400.0%|
|                       hides |       11 |       9918 |       73 |      64185 |    763.6%|
|             hit_sound_frame |        2 |         70 |       78 |       2730 |   4000.0%|
|                          if |      928 |     482987 |     2392 |     993946 |    357.8%|
|                 illuminates |       14 |       6548 |      136 |      62573 |   1071.4%|
|                   init_side |        1 |          3 |       40 |        120 |   4100.0%|
|                       input |        9 |        127 |       24 |        336 |    366.7%|
|                  insert_tag |       32 |       2440 |       84 |       6248 |    362.5%|
|                   inventory |        4 |         57 |       12 |        171 |    400.0%|
|                        item |        5 |       3135 |       15 |       9351 |    400.0%|
|                   item_list |        2 |     397453 |        2 |     397478 |    200.0%|
|                       items |        7 |        254 |       21 |        762 |    400.0%|
|                item_storage |        2 |        859 |        2 |        859 |    200.0%|
|                        join |        9 |        982 |       23 |       2554 |    355.6%|
|                        kill |        6 |        638 |       74 |       6344 |   1333.3%|
|                      killed |       50 |       3688 |       25 |        134 |    150.0%|
|                       knife |        1 |         17 |        1 |         17 |    200.0%|
|                      latent |       62 |      16426 |      215 |      55364 |    446.8%|
|                  leadership |       43 |      15208 |      451 |     158467 |   1148.8%|
|                     limited |        1 |         16 |        3 |         48 |    400.0%|
|                     literal |        5 |       2215 |       17 |       6337 |    440.0%|
|                        mace |        2 |         33 |        6 |         99 |    400.0%|
|                   menu_item |        4 |     235222 |        8 |     248951 |    300.0%|
|                     message |      181 |     202113 |      185 |     345303 |    202.2%|
|               missile_frame |       24 |       3554 |      530 |      82654 |   2308.3%|
|               modifications |      101 |     181599 |      377 |     339044 |    473.3%|
|                 modify_unit |        9 |       1249 |       13 |       1663 |    244.4%|
|                        move |      479 |      24334 |       22 |       1093 |    104.6%|
|              movement_costs |       28 |       4384 |      560 |      89737 |   2100.0%|
|                       music |        2 |        113 |        2 |        113 |    200.0%|
|                         not |       46 |       3232 |      126 |       7114 |    373.9%|
|                      object |      310 |     255331 |      958 |     728410 |    409.0%|
|          object_description |        4 |        263 |       12 |        789 |    400.0%|
|                   objective |        8 |        762 |        6 |        589 |    175.0%|
|                  objectives |        3 |       1253 |        1 |        583 |    133.3%|
|              object_removed |        1 |         23 |        3 |         69 |    400.0%|
|                      option |      270 |     253110 |      380 |     406707 |    240.7%|
|                          or |      279 |      31838 |      405 |      42718 |    245.2%|
|                      plague |        5 |       1531 |       17 |       5273 |    440.0%|
|                      poison |        6 |       1330 |      883 |     165819 |  14816.7%|
|                    portrait |       71 |       7326 |      845 |      87886 |   1290.1%|
|                      potion |        3 |         49 |        7 |        115 |    333.3%|
|              primary_attack |        1 |         22 |        1 |         22 |    200.0%|
|                primary_unit |       19 |        803 |      138 |       4645 |    826.3%|
|                      random |     2427 |     108402 |       49 |       1172 |    102.0%|
|                      recall |       17 |       1407 |        0 |          0 |    100.0%|
|                     recalls |       13 |       1673 |       62 |        248 |    576.9%|
|                     recruit |      108 |       8733 |       45 |       3589 |    141.7%|
|                    recruits |       44 |       3009 |       31 |        124 |    170.5%|
|                  regenerate |        7 |       2167 |       55 |      16637 |    885.7%|
|                 remove_item |        5 |        485 |       15 |       1617 |    400.0%|
|         remove_unit_overlay |        6 |        352 |        6 |        352 |    200.0%|
|                      replay |        1 |     371641 |        0 |          0 |    100.0%|
|                replay_start |        1 |    2283731 |        0 |          0 |    100.0%|
|                  resistance |       76 |      10464 |      718 |      90931 |   1044.7%|
|                     results |      100 |       8072 |      540 |      43078 |    640.0%|
|                        ring |        2 |         32 |        6 |         96 |    400.0%|
|                    scenario |       16 |     101886 |        0 |          0 |    100.0%|
|                    sequence |      497 |      35803 |       54 |       1513 |    110.9%|
|                set_specials |       37 |      26125 |      163 |      62323 |    540.5%|
|                set_variable |     1507 |     135197 |     5767 |     429079 |    482.7%|
|               set_variables |      101 |      50729 |      205 |      76083 |    303.0%|
|                     show_if |       64 |      14476 |      174 |      33064 |    371.9%|
|                        side |       20 |    3719228 |        2 |     622596 |    110.0%|
|                  skirmisher |        3 |        728 |       36 |       8578 |   1300.0%|
|                        slow |        5 |       1578 |      107 |      35018 |   2240.0%|
|                    snapshot |        1 |    2743727 |        0 |          0 |    100.0%|
|                      source |      129 |       3553 |       45 |       3967 |    134.9%|
|                    specials |      109 |     126670 |      983 |     931923 |   1001.8%|
|              specials_melee |        6 |       1533 |      113 |       6497 |   1983.3%|
|             specials_ranged |        1 |          6 |       94 |        564 |   9500.0%|
|                       split |        2 |        228 |        2 |        228 |    200.0%|
|                       staff |        1 |         15 |        1 |         15 |    200.0%|
|                       stage |        3 |       5655 |       13 |      24189 |    533.3%|
|                       start |        1 |          3 |        0 |          0 |    100.0%|
|                  statistics |        1 |     102273 |        0 |          0 |    100.0%|
|                      status |       20 |        651 |      519 |       9081 |   2695.0%|
|             store_locations |        4 |        560 |        8 |        992 |    300.0%|
|                  store_unit |       49 |       7874 |      174 |      21938 |    455.1%|
|             store_unit_type |        2 |        149 |        4 |        281 |    300.0%|
|                       swarm |        1 |        313 |        2 |        626 |    300.0%|
|                      switch |       10 |      84137 |       24 |     118905 |    340.0%|
|                      target |        4 |        561 |       16 |       2261 |    500.0%|
|                        team |       74 |      99737 |        1 |        457 |    101.4%|
|                    teleport |        5 |       3715 |       13 |       9798 |    360.0%|
|    teleport_sparkle_1_frame |        4 |       1384 |       22 |       7597 |    650.0%|
|    teleport_sparkle_2_frame |        4 |       1378 |       22 |       7564 |    650.0%|
|    teleport_sparkle_3_frame |        4 |       1384 |       22 |       7597 |    650.0%|
|                        then |      658 |     217506 |     2239 |     479214 |    440.3%|
|                        time |        7 |       1071 |        5 |        736 |    171.4%|
|                       trait |       21 |       5015 |      957 |     222354 |   4657.1%|
|                      tunnel |        4 |       1997 |       16 |       8026 |    500.0%|
|                   turn_info |       41 |       4464 |        3 |        330 |    107.3%|
|                  undo_stack |        2 |         53 |        0 |          0 |    100.0%|
|                        unit |      365 |    2651804 |      145 |    1559248 |    139.7%|
|                unit_overlay |        6 |        420 |        6 |        420 |    200.0%|
|                unstore_unit |       44 |       3296 |      155 |       9212 |    452.3%|
|                  upload_log |        1 |       6341 |        0 |          0 |    100.0%|
|                       value |       67 |      34201 |      151 |      48705 |    325.4%|
|                    variable |     1261 |      92729 |     4118 |     274182 |    426.6%|
|                   variables |       21 |     622658 |      474 |       7003 |   2357.1%|
|                     village |       74 |       1389 |       13 |        247 |    117.6%|
|                       while |      136 |     334464 |      229 |     492431 |    268.4%|
|                        MINS |        1 |          3 |        0 |          0 |    100.0%|
|                        MAXS |     2427 |    3719228 |     6346 |    1559248 |  47700.0%|
|                        SUMS |          |   22026602 |          |   18412225 |          |

*Note that the minimum possible waste is 100%, not 0%, since it should be called percentage of 
minimum possible data size.* :)

### A Basic Fix

Though not directly related to serialization methodology, a key component of fixing this issue 
is to refactor some of Wesnoth's core classes to limit the use of strings and other complex 
objects when either simple datatypes or pointers/references to single base objects will do. One 
obvious advantage to reducing the use of strings as attributes is that comparisons are vastly 
quicker and cheaper.

My observation is that almost all systems, from phones to gaming behemoths, tend towards greater 
CPU performance than IO performance. With that in mind, it makes sense to reduce object 
redundancy if the cost is calculation not IO. This seems to be another advantage of reducing 
reliance on strings and large objects that require large reads and writes during ser-des.



### The JSON Fix

My preferred way to handle in-game serialization is to have the game state serialized to JSON 
and persisted either as files, in an in-memory store or in a NoSQL store like MongoDB. One of 
the things that I see as a major advantage of using JSON and MongoDB as a serialization and 
persistence methodology is that it makes it possible for quick and easy network updates of 
campaigns, sharing of private -- not wesnoth.org -- campaigns and scenarios, persisting from 
storage constrained devices to network stores, etc.

By using JSON and a JSON friendly DB, the responsibility for the performance and security of 
the serialization and persistence layers move from the Wesnoth devs to the library devs. And, 
as several professors and authors have repeatedly beaten into my brain, *it is good to use 
libraries, bad to rewrite the same functionality*.

While I understand that the lead developers of Wesnoth strongly advise against adding external 
dependencies; and, while I have experience for myself the hassle that adding dependencies can 
create; I believe that adding the following dependencies will be worth the trouble:
- boost::uuid : For creating unique ids for units, maps, abilities, etc.
- libjson/json++/jsoncpp
- MongoDB/CouchDB [only with wesnothd]

## Notes

The python script mentioned above:

        '''python
        import Levenshtein as lv
        import sys
        import re

        PATH = '/home/justin/.local/share/wesnoth/1.11/saves/LotI1-The_Battle_for_Ogira_Turn_6'
        TAGS = [r"abilities", r"advance", r"advanced2", r"advancefrom", r"advancement", r"advances", r"affect_adjacent", r"ai", r"ai_log", r"allow_undo", r"amulet", r"and", r"animate_unit", r"animation", r"armour", r"aspect", r"attack", r"attack_anim", r"attacks", r"berserk", r"candidate_action", r"carryover_sides", r"carryover_sides_start", r"case", r"chance_to_hit", r"checksum", r"choose", r"clear_variable", r"command", r"damage", r"deaths", r"default", r"defend", r"defends", r"defense", r"delay", r"description", r"destination", r"disallow_recruit", r"display", r"do", r"drains", r"dummy", r"effect", r"else", r"endlevel", r"end_level_data", r"end_turn", r"engine", r"event", r"facet", r"facing", r"filter", r"filter_adjacent", r"filter_adjacent_location", r"filter_attack", r"filter_base_value", r"filter_location", r"filter_opponent", r"filter_radius", r"filter_recall", r"filter_second", r"filter_second_attack", r"filter_self", r"filter_side", r"filter_wml", r"fire_event", r"firststrike", r"floating_text", r"formula_ai", r"frame", r"from", r"gold", r"harm_unit", r"have_unit", r"heals", r"heal_unit", r"helm", r"hides", r"hit_sound_frame", r"if", r"illuminates", r"init_side", r"input", r"insert_tag", r"inventory", r"item", r"item_list", r"items", r"item_storage", r"join", r"k$", r"kill", r"killed", r"knife", r"latent", r"leadership", r"limited", r"literal", r"mace", r"menu_item", r"message", r"missile_frame", r"modifications", r"modify_unit", r"move", r"movement_costs", r"music", r"not",r"object", r"object_description", r"objective", r"objectives", r"object_removed", r"option", r"or", r"plague", r"poison", r"portrait", r"potion", r"primary_attack", r"primary_unit", r"random", r"recall", r"recalls", r"recruit", r"recruits", r"regenerate", r"remove_item", r"remove_unit_overlay", r"replay", r"replay_start", r"resistance", r"results", r"ring", r"scenario", r"sequence", r"set_specials", r"set_variable", r"set_variables", r"show_if", r"side", r"skirmisher", r"slow", r"snapshot", r"source", r"specials", r"specials_melee", r"specials_ranged", r"split", r"staff", r"stage", r"start", r"statistics", r"status", r"store_locations", r"store_unit", r"store_unit_type", r"swarm", r"switch", r"target", r"team", r"teleport", r"teleport_sparkle_1_frame", r"teleport_sparkle_2_frame", r"teleport_sparkle_3_frame", r"then", r"time", r"trait", r"tunnel", r"turn_info", r"undo_stack", r"unit", r"unit_overlay", r"unstore_unit", r"upload_log", r"value", r"variable", r"variables", r"village", r"while",
        ]

        def common_lines():
            DIST = 5
            current = 0
            count_exact = 0
            count_dist = 0
            out = 0
            uniq_lines = []

            for line in open(PATH, 'r').readlines():
                current += 1
                if line in uniq_lines:
                    count_exact += 1
                else:
                    cd = False
                    for ul in uniq_lines:
                        if lv.distance(line, ul) < DIST:
                            count_dist += 1
                            cd = True
                            break
                    if not cd:
                        uniq_lines.append(line)

                if (current % 256) == 0:
                    sys.stdout.write("% 8d, " % current)
                    out += 1
                    if (out % 8) == 0:
                        sys.stdout.write("\n")

            print("Total: %08d, Exact: %d [%4.1f%%], < %d: %d [%4.1f%%]" %
                    (current,
                    count_exact,
                    100.0*float(count_exact)/current,
                    DIST,
                    count_dist,
                    100.0*float(count_dist)/current,))

        def tag_contents(tag):
            contents = []
            uniqchars = 0
            dupchars = 0
            uniq = 0
            dups = 0
            tagstring = r'\[' + tag + r'\](?P<c>.*?)\[/' + tag + r'\]'
            tagre = re.compile(tagstring, flags=re.DOTALL)
            data = open(PATH, 'r').read()

            for tc in tagre.finditer(data):
                if tc.group('c') in contents:
                    dups += 1
                    dupchars += len(tc.group('c'))
                else:
                    uniq += 1
                    uniqchars += len(tc.group('c'))
                    contents.append(tc.group('c'))

            if not uniq == 0:
                print("|% 28s | % 8d | % 10d | % 8d | % 10d | % 8.1f%%|" % (tag,
                                                                            uniq,
                                                                            uniqchars,
                                                                            dups,
                                                                            dupchars,
                                                                            100.0 * (dups+uniq)/uniq))

        if __name__ == "__main__":
            for t in TAGS:
                tag_contents(t)
        '''
