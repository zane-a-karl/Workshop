Project List:

Note: a lack of a checked-box means incomplete.

  - [ ] Deutsch Proj
    - Uses beeg guide to have a client communicate with a server which communicates with two other servers
    - Upgrade to fork and debug child processes with lldb here: https://stackoverflow.com/questions/14746315/how-to-attach-to-child-process-in-lldb
    - Upgrade shortest path search with Fibonacci heap here: https://en.wikipedia.org/wiki/Fibonacci_heap
  - [ ] Encrypt and Authenticate a client to server message
    - Refer to the following sites:
      - https://wiki.openssl.org/index.php/Libcrypto_API
      - https://wiki.openssl.org/index.php/AES
      - https://wiki.openssl.org/index.php/EVP_Authenticated_Encryption_and_Decryption
      - https://stackoverflow.com/questions/42797594/can-aes-gcm-tag-be-set-after-calling-evp-decryptupdate?noredirect=1&lq=1
  - Modes of Operation
    - ECB
      - [x] 1x threaded
      - [x] multi-threaded
      - [ ] cleanup with makefile for tests


Personal Style Guidelines:
  - Most functions should return void, if not detail the reason.
  - Any given line is capped at 70 characters, roughly the width of an emacs session split in half.
  - Function input is ordered like so:
    - Outputs come first in alphabetical order by variable TYPE and then by NAME.
    - Inputs come second in alphabetical order by variable TYPE and then by NAME.
