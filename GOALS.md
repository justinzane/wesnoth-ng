# GOALS

Goals, plans, thought, analysis and general silliness regarding the justinzane fork of 
Battle for Wesnoth.

**Note**: These are written by an amateur programmer as a hobby project. Nothing here is to be 
taken as a disrecpectful criticism of the many much more skilled and experience coders who have 
contributed their effort and creativity to Wesnoth. If you disagree, *please* contact the author
on #wesnoth or at <justin at justinzane dot com>.  

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
    
            :::c++
            /** header comment */
            #ifndef WESNOTH_UNIT_WRAPPER_GUARD
            #include "unit/unit-blah.hpp"
            #include "unit/unit-foo.hpp"
            #include "unit/unit-bar.hpp"
            #endif

## Serization

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

`Total: 201472, Exact: 65876 [32.7%], < 5: 130446 [64.7%]`

From a total of over **350000** lines from **one** scenario, over **100000** -- over a third --
are **exact** duplicates. Similarly, two thirds of the lines are within **4** characters of a base.
In my not-so-humble opinion, that alone is a huge cry for helpful refactoring. 
While modern x86_64 systems can handle that volume of text seemingly instantly, that is a huge 
burden on the resources of low-power, mobile devices. With the increasing popularity of tablets, 
chromebooks, monster-phones and whatnot, as well as the availability of FOSS or similar stacks 
on these devices, it seems silly to exclude them for want of efficient serialization.

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

My perferred way to handle in-game serialization is to have the game state serialized to JSON 
and persisted either as files, in an in-memory store or in a NoSQL store like MongoDB. One of 
the things that I see as a major advantage of using json and MongoDB as a serialization and 
persistence methodology is that it makes it possible for quick and easy network updates of 
campaigns, sharing of private -- not wesnoth.org -- campaigns and scenarios, persisting from 
storage constrained devices to network stores, etc.

By using JSON and a JSON friendly DB, the responsibility for the performance and security of 
the serialization and persistence layers move from the Wesnoth devs to the library devs. And, 
as several professors and authors have repeatedly beaten into my brain, *it is good to use 
libraries, bad to rewrite the same functionality*.



## Notes

The python script mentioned above:

        :::python
        import Levenshtein as lv
        import sys
        
        DIST = 5
        current = 0
        count_exact = 0
        count_dist = 0
        out = 0
        uniq_lines = []
        
        for line in open(
            '/home/justin/.local/share/wesnoth/1.11/saves/LotI1-The_Battle_for_Ogira_Turn_6',
            'r').readlines():
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
                