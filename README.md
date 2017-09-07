# iOS-Restrictions-Cracker-Linux
This program is designed to crack iOS 7-11 restrictions passcode as quickly as possible. It can use an unlimited number of threads to complete this process.

## Install
apt package coming soon

## Compile from source
Make sure that gcc (4.7 or newer), make, openssl and openssl-dev are installed on the machine

  `sudo apt install gcc make openssl openssl-dev`
  
To compile run 
  `make`

## Usage
To crack, simply enter

`restrictions-crack <hash> <salt>`

Ex (will yield a code of 1):

`./restrictions-crack "M/p4734c8/SOXZnGgZot+BciAW0=" "aSbUXg=="`

## ToDo
- Clean up memory when it is no longer needed
- Add error checking and input validation
- Create threads more quickly to improve performance (multi-thread thread creation?)
- Add timer to program to measure performance
- End all threads immediately 
- Improve algorithm efficiency
- Improve code documentation and commenting
