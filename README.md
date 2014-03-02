zmqcat
======
Usage speaks more or less for itself.

    zmqcat 1.0

      usage: zmqcat [-b] -e endpoint [-l 20] [-r 1] [-s channel] [-t type] [-v]
        -b  --bind      : bind instead of connect to endpoint
        -e  --endpoint  : endpoint, e.g. "tcp://127.0.0.1:5000"
        -h  --help      : display this usage information
        -l  --linger    : linger period for socket shutdown in ms
        -r  --repeat    : repeat send and receive cycle X times (-1 = forever)
        -s  --subscribe : subscribe to channel for SUB type socket
        -t  --type      : PUSH | PULL | REQ | REP | PUB | SUB
        -v  --verbose   : verbose output to stderr

### License ###
    Copyright 2014 Florian Schäfer <florian.schaefer@gmail.com>
    Copyright 2012 Emiel Mols <emiel@paiq.nl>

    Redistribution and use in source and binary forms, with or without modification, are
    permitted provided that the following conditions are met:

       1. Redistributions of source code must retain the above copyright notice, this list of
          conditions and the following disclaimer.

       2. Redistributions in binary form must reproduce the above copyright notice, this list
          of conditions and the following disclaimer in the documentation and/or other materials
          provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND ANY EXPRESS OR IMPLIED
    WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
    FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> OR
    CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
    ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    The views and conclusions contained in the software and documentation are those of the
    authors and should not be interpreted as representing official policies, either expressed
    or implied, of the copyright holders.
