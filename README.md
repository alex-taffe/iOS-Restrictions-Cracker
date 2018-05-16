# iOS-Restrictions-Cracker-Linux
This program is designed to crack iOS 7-11 restrictions passcode as quickly as possible. It can use an unlimited number of threads to complete this process.

## Install
apt package coming soon

## Compile from source
Make sure that gcc (4.7 or newer), make, openssl and openssl-dev are installed on the machine (Linux only, macOS see [Docker](#Usage-with-Docker). This will be fixed in future builds)

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

Alternatively, to run the program using the hash and salt listed above in the Usage section (will yield a code of 3956):  
`docker run -e hash="J94ZcXHm1J/F9Vye8GwNh1HNclA=" -e salt="/RHN4A==" restrictions`

## To Find Your Restrictions Hash and Salt
1) Create a non encrypted iPhone backup in iTunes
2) On macOS navigate to `~/Library/Application Support/MobileSync/Backup/YOUR_DEVICE_UDID` or on Windows `%AppData%\Apple Computer\MobileSync\Backup\YOUR_DEVICE_UDID`.
3) Search the folder for a file called `398bc9c2aeeab4cb0c12ada0f52eea12cf14f40b`
4) Open the folder in a text editor. In the file you will find a section that looks like:

 ```
  <dict>
       <key>RestrictionsPasswordKey</key>
       <data>
       M/p4734c8/SOXZnGgZot+BciAW0=
       </data>
       <key>RestrictionsPasswordSalt</key>
       <data>
       aSbUXg==
       </data>
  </dict>
  ```
  In this example, the hash would be `M/p4734c8/SOXZnGgZot+BciAW0=` and the salt `aSbUXg==`
  
5) Plug the retrieved values into the program

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
- Add better support for macOS (OpenSSL libraries not preinstalled, use Apple's Crypto library)
- Add support on macOS to auto extract data from iOS backups
- Add the ability to read codes in from a file
- <s>Add a</s> Improve the premade list of codes for benchmarking purposes
- Add unit tests
- Compile a list of benchmarks on various systems
