## README:

__Installation:__<br>
Install via “make”, clean executables via “make clean”

__Usage:__<br>
The program should be run via command line

Run the server program and navigate to the address 127.0.0.1:8001
(or localhost:8001) on your browser. You will receive some test data
rendered into the html template at _files/index.html_.

__Notes:__<br>
At the current stage only one connection is accepted at a time (and only
within the computer). Stylesheets or other files can currently not be loaded.
String formatting is currently ignored in browsers. Only reading data from files is currently supported.

Forementioned issues will be corrected in the near future. This project is mainly a personal experiment and will
improve little by little as I come up with new ways to expand it.

__Plans:__<br>
Rewriting datastruct formating to better support varying sourcetypes
Adding multithreading support to accept multiple clients
Making program accept external connections instead of internal loopback

__Changelog:__<br>
28.2. Added function for handling HTTP requests. Functionality is not yet used and functions for removing and actually using generated header struct are still under work.
6.3. An html document can now be rendered from data contained in a file
