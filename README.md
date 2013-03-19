jnxlibc
=======
[![Build Status](https://travis-ci.org/[AlexsJones]/[jnxlibc].png)](https://travis-ci.org/[AlexsJones]/[jnxlibc])

C based common code library based on libjnx philosophy of general purpose & lightweight development.

Key features:
- Simple listen/send over IP/port functions
- Easy to use file read/write functions
- Simple logger system
- Terminal loading spinners and fancy colors done easily.
- Several useful data structures (hash, list, vector, btree)


#Installation

Grab this repo.
````
./configure
make
sudo make install
link against -ljnxc -ldl
````
