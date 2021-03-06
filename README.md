# pwclip
Lightweight local password vault for Windows and OSX
![preview](https://github.com/cadouthat/pwclip/raw/screenshots/pwclip-sample.png)

Overview
--------
A pwclip vault contains password entries referenced by user-defined nicknames. Each vault has a master password for encryption, which is needed to open the vault and view/manage entries. After opening a vault, the decryption key is stored in memory until pwclip is closed, so the master password is only needed once.

The system clipboard is used to move passwords around. While pwclip is running, an icon is added to the system tray for quick access to load and manage passwords, switch vaults, generate random passwords, and more.

The raw export feature provides a plaintext backup of all entries (to be stored in a secure location) in case of emergencies, such as data loss or forgotten master passwords.

Usage Tips
----------
* Colons in entry names are treated as sub-menus. For example, an entry named "finance:banks:mybank" will appear as "mybank" nested under "banks" nested under "finance".
* pwclip does not have a separate mechanism for storing usernames. In cases where usernames are hard to remember, creating two entries is recommended, such as "account-user" and "account-pass".

Password Storage
----------------
The password database is stored as an sqlite file. Passwords are encrypted using AES-128 with a 256 bit key derived from the master password using salted PBKDF2-SHA256. Each entry is encrypted independently with a random IV. Entry names are not encrypted.

Building on Windows
-------------------
If they do not exist, create 'bin' and 'local' directories in the pwclip directory. Before building, you will need OpenSSL and SQLite3. Compile SQLite3 if it is not already compiled. Copy path.mk.sample to local/path.mk, and populate the appropriate directories for your system. Finally, run "make" from the pwclip directory.

Building on OSX
---------------
The Xcode project is located in src/osx.

---

Copyright 2016 Connor Douthat
