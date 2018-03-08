## INDEX:

__5.3.2018: DataFromFileUnit: (deprecated)__<br>
Contains code for creating data packages for the HTML renderer to work with. Creates packages from the lines of a given file by formatting them and collecting them into a single structure. Biggest issue is dealing with the formatting options.
### This unit will be replaced by ImprovedDatastructUnit

__5.3.2018: ImprovedDatastructUnit:__<br>
New version of the DataFromFileUnit. Big chunks of code were rewritten to simplify the code somewhat. New formats for creating new datapackets can now be easily created as the format used for line rendering was "stadardised". There are still a few optimisation steps to be done but that will follow later.

<br>
<br>
### Plans:<br>
- A function for sending values to correct function based on the HTTP request resource
- Multithreading and living connection support for giving service to multiple clients simultaneously
