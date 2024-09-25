# Development

> summary

## Design (Pt1)

![IPO Diagram](images/ipo.jpg "Input-Process-Output Diagram")

The IPO diagram is a result of three prototype implementations:
1. main.c
2. u4test.c
3. 06test.c

Each file demonstrates key features the application should have (see below).

### main.c (Generate Random Text)
* retrieve random wordset
* print wordset within column constraints
<img src="images/data.gif" alt="Random Text Input Design" width=600>

### u4test.c (Keyboard I/O)
* visual char comparison (green=correct, red=incorrect)
* prompt clearing on segment completion
* segment locking after completion (cannot backspace into completed segments)
<img src="images/kbd.gif" alt="Keyboard IPO Design" width=600>

### 06test.c (UI Layout)
* dynamic window resizing
* scrollable text
<img src="images/ui.gif" alt="UI Layout Design" width=600>

## Design (Pt2)

Header files outline

| thock.h            | ui.h            | xdata.h         |
|--------------------|-----------------|-----------------|
| request ext data   | win properties  | parse data      |
| request kbd input  | win functions   | allocate memory |
| track state        | key definitions |                 |
| track status       | i/o processes   |                 |
| track data sources |                 |                 |

Essentially, thock.h is main and overlooks program data + progress. Any printing of the data is handled by the ui.h, as well as ui functions like resizing, refreshing, scrolling, etc. xdata.h where [ x == type_of_data ] is any type of external data source. For example, in main.c, above, 20 random words are retrieved through FreeBSD dictionary files, thus the xdata.h file could be dictdata.h. General cases could include: txtdata.h, jsondata.h, xmldata.h. etc.

<!---

thock.h/c ui.h/c 

dictdata.h/c xmldata.h/c koexdata.h/c txtdata.h/c jsondata.h/c csvdata.h/c

TODO

-->
