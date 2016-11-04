# StreamingRecognition
---
## Info
This is the source for the Streaming recognition implimantation of Google's Speech API in C++.
This program does nothing spcific, it is simply meant as example for building other C++ programs baised off of Google's speech API.
Please note that Google's speech API is still in beta so this is likely to become void (until updated) in the future.


## Operation:
    If the word "exit" or "quit" is returned in any of the results the program will exit.
    Additionally be warned that there is a "timeout" varriable that will also cause the program to exit if the programs runs for more than 45 seconds.

## Building
    Follow the directions in the [root](/ "root dir") of the repo before building this.
```
    $ make          // Build
    $ ./main_cpp    // Run
    $ make clean    // Clean up
```

