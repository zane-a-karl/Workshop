On the serialization buffer
Your two-array idea is actually a clean solution and essentially equivalent. The inline approach just collapses those two arrays into one by interleaving the metadata with the data itself. Here's how traversal works:
buffer: [4][1 0 0 0][1][a][8][1.2 as double]...
         ^  ^        ^  ^  ^  ^
         |  |        |  |  |  data (8 bytes)
         |  |        |  |  data_n for next element
         |  data (4 bytes)
         data_n for this element
You start at byte 0, read data_n (say 4 bytes), now you know the next data_n bytes are the value, so you skip ahead sizeof(data_n) + 4 bytes to reach the next element. Each element tells you how far to jump to reach the next one.