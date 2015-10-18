# pwclip
Lightweight local password vault

Overview
--------
At its core, pwclip is a password manager designed to be small, flexible, and minimally intrusive. Passwords are saved and recalled with user-defined nicknames. Each entry is encrypted independently, but users will find it most convenient to use the same master password for most or all entries.

The tray application (pwcliptray), while entirely optional, provides a convenient graphical interface and remembers master password(s) until closed.

The export tool (pwclipdump) provides a plaintext backup of your passwords (to be stored in a secure location) in case of emergencies, such as data loss or forgotten master passwords.

Password Storage
----------------
The password database is stored as an sqlite file, in the current user's Local Application Data directory by default. Entry nicknames are stored as plaintext in the database. Stored passwords are encrypted using AES-128 with a 256 bit key derived from the encryption password using PBKDF2-SHA256. Each password is encrypted independently with a random IV.

Building
--------
If they do not exist, create 'bin' and 'local' directories in the pwclip directory. The default makefile target will build all binaries (output to the bin directory). Before building, you will need OpenSSL and SQLite3. Compile SQLite3 if it is not already compiled. Copy path.mk.sample to local/path.mk, and populate  the appropriate directories for your system. Finally, run make from the pwclip directory.

Operating Systems
-----------------
The application will be limited to Windows (XP through 10) for the forseeable future. However, OSX and Linux support are long term goals.