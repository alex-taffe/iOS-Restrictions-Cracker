# iOS-Restrictions-Cracker-Linux
This program is designed to crack iOS 7-11 restrictions passcode as quickly as possible. It can use an unlimited number of threads to complete this process.

## Install
apt package coming soon

## Compile from source
Make sure that gcc (4.7 or newer), make, openssl and openssl-dev are installed on the machine

  `sudo apt install gcc make openssl libssl-dev`
  
To compile run 
  `make`

## Usage
To crack, simply enter

`restrictions-crack <hash> <salt>`

Ex (will yield a code of 3956):

`./restrictions-crack "J94ZcXHm1J/F9Vye8GwNh1HNclA=" "/RHN4A=="`

## Usage with Docker
1) Install Docker from docker.com
2) Clone this repository
3) `cd` into the repository
4) Run `docker build -t restrictions .`
5) Run `docker run -e hash=HASH -e salt=SALT restrictions` replacing HASH and SALT with your input  
6) To run the program using the hash and salt listed above (will yield a code of 3956):  
`docker run -e hash="J94ZcXHm1J/F9Vye8GwNh1HNclA=" -e salt="/RHN4A==" restrictions`


## Todo
- Clean up memory when it is no longer needed
- Add apt package
- Add error checking and input validation
- Create threads more quickly to improve performance (multi-thread thread creation?)
- <s>Add timer to program to measure performance</s>
- <s>End all threads immediately</s>
- Improve algorithm efficiency
- Improve code documentation and commenting
- <s>Remove debug flags from production build files</s>
- <s>Add -O3 flag to gcc for production builds to speed up execution</s>
- Add make install action
- Add configure script
- Add automake support
