# Design

![IPO Diagram](images/ipo.jpg "Input-Process-Output Diagram")

The IPO diagram is based on three prototype implementations:
* 06test.c  (UI Layout)
* u4test.c  (Keyboard IO)
* main.c    (Random Text)

Each implementation is shown below in their respective order.

![UI Layout Design](images/ui.gif)
Dynamic resizing of UI elements and scrollable text region when greater than 10 rows.

![Keyboard IPO Design](images/kbd.gif)
Prompt text input is compared one character at a time with cursor character in text region.

![Random Text Input Design](images/data.gif)
Random words generated from FreeBSD dictionary word files.
