# Stage 1
from alpine:3.20.3 as compiler
WORKDIR /app
RUN apk add cmake make git binutils
COPY toolchain.cmake .
COPY CMakeLists.txt .
COPY src src
RUN cmake -Bbuild -H. -DTARGET=$(uname -m)-linux
RUN make -C build
RUN strip -s /app/build/bin/game

# Stage 2
FROM scratch
COPY --from=compiler /app/build/bin/game /run
ENTRYPOINT ["/run"]
