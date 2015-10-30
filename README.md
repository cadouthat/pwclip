# pwclip
Lightweight local password vault

Overview
--------
Passwords are transported to and from the vault via the clipboard. Each entry is referenced with a user-defined nickname, and encrypted with a "master" password. Users may choose to use the same master password for most or all entries.

While pwclip is running, an icon is added to the system tray for convenient access. Decryption keys are stored in memory until pwclip is closed, so users don't need to enter their master password(s) every time they load an entry.

The export feature provides a plaintext backup of all entries (to be stored in a secure location) in case of emergencies, such as data loss or forgotten master passwords.

Usage Tips
----------
* Colons in entry names are treated as sub-menus. For example, an entry named "finance:banks:mybank" will appear as "mybank" nested under "banks" nested under "finance".
* pwclip does not have a separate mechanism for usernames. In cases where usernames are hard to remember, creating two entries is recommended, such as "account-user" and "account-pass".

Password Storage
----------------
The password database is stored as an sqlite file, in the current user's Local Application Data directory by default. Entry nicknames are stored as plaintext in the database. Stored passwords are encrypted using AES-128 with a 256 bit key derived from the encryption password using PBKDF2-SHA256. Each password is encrypted independently with a random IV.

Building
--------
If they do not exist, create 'bin' and 'local' directories in the pwclip directory. The default makefile target will build all binaries (output to the bin directory). Before building, you will need OpenSSL and SQLite3. Compile SQLite3 if it is not already compiled. Copy path.mk.sample to local/path.mk, and populate  the appropriate directories for your system. Finally, run make from the pwclip directory.

Operating Systems
-----------------
The application is under active development on Windows (XP through 10). OSX development is planned to begin soon.