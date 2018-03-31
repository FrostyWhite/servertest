## INDEX:

__5.3.2018: DataFromFileUnit: (deprecated)__<br>
Contains code for creating data packages for the HTML renderer to work with. Creates packages from the lines of a given file by formatting them and collecting them into a single structure. Biggest issue is dealing with the formatting options.
#### This unit will be replaced by ImprovedDatastructUnit

__5.3.2018: ImprovedDatastructUnit: (completed)__<br>
New version of the DataFromFileUnit. Big chunks of code were rewritten to simplify the code somewhat. New formats for creating new datapackets can now be easily created as the format used for line rendering was "stadardised". There are still a few optimisation steps to be done but that will follow later.
#### ++<br>
One more datastruct function created for easier autorendering. Added pseudocode to clarify what the functions do. Cleaned the .c file of useless functions

__28.3.2018: ImprovedBaseUnit: (completed)__<br>
New version of the basic structure of the server. After implementation of this, the server is more expandable and supports multiple users. This version already works on a basic level (receives connections and responds with a simpple hello page) on a local area network and could technically be deployed on a server.
#### ++<br>
The unit now works as expected, however, some issues were found in rendering long strings. These problems will be corrected before the main project is updated

__31.3.2018: Version2Unit:__<br>
New version of the structure of the entire program. Structure will be changed to a more comprehensive and logical direction. Changes will be made with ease of use in mind. This means that pages and content can be created easily with minimal programming. Data can be loaded from a definition file along with page names and URLs, page-specific backend can be written easily. Everything required to run the system is to write content in the user folder. Source code glues everything together.
<br>
<br>
### Plans:<br>
- Easier to use page creation
- More logical software structure
- Major changes to make creating new pages easier [Later in version 2]
- Loading Css and other additional files [Later in version 2]
- Rendering pictures [Later in version 2]
- Better database system [Later in version 2]
