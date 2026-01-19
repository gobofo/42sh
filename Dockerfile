# Use a minimal base image
FROM debian:bookworm-slim

# Install only the strictly necessary tools mentioned in the subject
RUN apt-get update && apt-get install -y \
    gcc \
    make \
    autoconf \
    automake \
    libtool \
    pkg-config \
    && rm -rf /var/lib/apt/lists/*

# Set the working directory
WORKDIR /app
