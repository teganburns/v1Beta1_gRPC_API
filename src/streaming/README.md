# StreamingRecognition

This is still pretty messy and has no guarantee of working on your system!!

1) as stated in /README.md you will need gRPC and protobuf3. Make sure to read /README.md for full details!!

2) use make to compile


## Info
This program does nothing spcific, it is simply meant as example for building other C++ programs based off of Google's speech API.
Please note that Google's speech API is still in Beta so this is likely to become void (until updated) in the future.

## Operation
 Be warned that there is a "timeout" varriable that will also cause the program to exit if the programs runs for more than 45 seconds.

## Building
```
    $ make          // Build
    $ ./main_cpp    // Run
    $ make clean    // Clean up
```

