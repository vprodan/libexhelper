FROM ubuntu:24.04

ARG TARGETARCH

RUN apt-get update && apt-get install -y \
    clang \
    llvm \
    lld \
    libc++-dev \
    libc++abi-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build
COPY . .

# Default: build and test
CMD ["bash", "-c", "MODE=release TEST=1 ./build.sh"]
