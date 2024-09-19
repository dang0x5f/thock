# Design

![IPO Diagram](images/ipo.jpg "Input-Process-Output Diagram")

The IPO diagram is based on three prototype implementations:
* 06test.c  (UI Layout)
* u4test.c  (Keyboard IO)
* main.c    (Random Text)

Each implementation is shown below in their respective order.

<!--- ![UI Layout Design](images/ui.gif) -->
<img src="images/ui.gif" alt="UI Layout Design" width=600>
>Dynamic resizing of UI elements and scrollable text region when greater than 10 rows.

<!--- ![Keyboard IPO Design](images/kbd.gif) -->
<img src="images/kbd.gif" alt="Keyboard IPO Design" width=600>
>Prompt text input is compared one character at a time with cursor character in text region.

<!--- ![Random Text Input Design](images/data.gif) -->
<img src="images/data.gif" alt="Random Text Input Design" width=600>
>Random words generated from FreeBSD dictionary word files.
