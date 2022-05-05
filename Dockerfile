# Build environment
FROM alpine AS build
RUN apk add --no-cache build-base
COPY ./server.c /src/server.c
COPY ./lib /src/lib
COPY ./Makefile /src/Makefile
WORKDIR /src
RUN make server-static && mv server-static server

# Just the static binary
FROM scratch
COPY --from=build /src/server /server
EXPOSE 8080/tcp
CMD ["./server"]
