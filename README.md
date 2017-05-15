## PHP-logger
phplogger is an PHP extension that allows a user to override the normal behaviour 
of a PHP internal function.


## Overview
This extension stores the original handler of the function (fopen in this case) 
in a new global HashTable (my_functions), then it modifies the handler to log 
the arguments of the function when it is called.

In order to activate this extension the method `sqreenOn()` must be called from 
the PHP script. This function calls `activate_sqreen` function which modify the handler.

After call `sqreenOn()` the extension will start to log in the syslog all the 
arguments passed to the `fopen` function.

The function `sqreenOff` deactivates the extension, replacing the execution of the handler
for the original handler call.


## Usage
To get the code, first clone this repository, then phpize it and compile and install
using make. The binary is installed in the ext folder depending on your php version. 

### Get The Code
```bash
$ git clone https://github.com/mcornejo/phplogger.git
$ cd phplogger
$ phpize
$ ./configure
$ make clean && make && make install
```
### Usage
Create a test.php script as follows:

```php
<?php

sqreenOn();
fopen("test.text", "r");
sqreenOff();

?>
```

To execute in the terminal, run the following commands:
```bash
$ php -dextension=instrumentation.so test.php
```
