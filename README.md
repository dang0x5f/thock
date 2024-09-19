# Design

![IPO Diagram](images/ipo.jpg "Input-Process-Output Diagram")

The IPO diagram is based on three prototype implementations:
* 06test.c  (UI Layout)
* u4test.c  (Keyboard IO)
* main.c    (Random Text)

Each implementation is shown below in their respective order.

<!--- ![UI Layout Design](images/ui.gif) -->
<figure>
    <img src="images/ui.gif" alt="UI Layout Design" width=400>
    <figcaption>
    Dynamic resizing of UI elements and scrollable text region when greater than 10 rows.
    </figcaption>
</figure>

<!--- ![Keyboard IPO Design](images/kbd.gif) -->
<figure>
    <img src="images/kbd.gif" alt="Keyboard IPO Design" width=400>
    <figcaption>
    Prompt text input is compared one character at a time with cursor character in text region.
    </figcaption>
</figure>

<!--- ![Random Text Input Design](images/data.gif) -->
<figure>
    <img src="images/data.gif" alt="Random Text Input Design" width=400>
    <figcaption>
    Random words generated from FreeBSD dictionary word files.
    </figcaption>
</figure>
