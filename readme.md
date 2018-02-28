## README:

__Installation:__<br>
Install via “make”, clean executables via “make clean”

__Usage:__<br>
The program should be run via commandline

Run the server program and the navigate to the address 127.0.0.1:8001
(or localhost:8001) on your browser. You will receive some test data
rendered into the html template at _files/index.html_.

__Notes:__<br>
At the current stage only one connection is accepted at a time (and only
within the computer). The code doesn't, for now, support rendering html from file.
This version is currently at the 'proof of concept' stage.

Forementioned issues will be corrected in the near future.

__Changelog:__<br>
28.2. Added function for handling HTTP requests. Functionality is not yet used and functions for removing and actually using generated header struct is still under work.
