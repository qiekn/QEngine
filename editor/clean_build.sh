
#!/bin/bash

rm -r bin && rm -r build && premake5 gmake && cd build && make && cd ..
