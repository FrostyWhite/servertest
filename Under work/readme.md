## INDEX:

__5.3.2018: DataFromFileUnit: (deprecated)__<br>
Contains code for creating data packages for the HTML renderer to work with. Creates packages from the lines of a given file by formatting them and collecting them into a single structure. Biggest issue is dealing with the formatting options.
#### This unit will be replaced by ImprovedDatastructUnit

__5.3.2018: ImprovedDatastructUnit: (completed)__<br>
New version of the DataFromFileUnit. Big chunks of code were rewritten to simplify the code somewhat. New formats for creating new datapackets can now be easily created as the format used for line rendering was "stadardised". There are still a few optimisation steps to be done but that will follow later.
#### ++<br>
One more datastruct function created for easier autorendering. Added pseudocode to clarify what the functions do. Cleaned the .c file of useless functions

__28.3.2018: ImprovedBaseUnit:__<br>
New version of the basic structure of the server. After implementation of this, the server is more expandable and supports multiple users. This version already works on a basic level (receives connections and responds with a simpple hello page) on a local area network and could technically be deployed on a server.
#### ++<br>
The unit now works as expected, however, some issues were found in rendering long strings. These problems will be corrected before the main project is updated
<br>
<br>
### Plans:<br>
- A function for sending values to correct function based on the HTTP request resource (Under work)
- Multithreading and living connection support for giving service to multiple clients simultaneously (Under work)
- Major changes to make creating new pages easier [In next version]
- Loading Css and other additional files
- Rendering pictures [In next version]
