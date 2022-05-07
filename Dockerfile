FROM alpine AS libs-build
ADD https://github.com/cesanta/mongoose/archive/refs/tags/7.6.tar.gz ./mongoose.tar.gz
ADD https://github.com/cesanta/mjson/archive/refs/tags/1.2.7.tar.gz ./mjson.tar.gz
RUN apk add --no-cache build-base \
&& mkdir -p /lib/src/mongoose /lib/src/mjson /lib/build/lib /lib/build/include \
&& tar xzf mongoose.tar.gz -C /lib/src/mongoose --strip-components=1 \
&& tar xzf mjson.tar.gz -C /lib/src/mjson --strip-components=1 \
&& cd /lib/src/mongoose && make mongoose.c mongoose.h \
&& mv /lib/src/mongoose/mongoose.h /lib/build/include/mongoose.h \
&& mv /lib/src/mongoose/mongoose.c /lib/build/lib/mongoose.c \
&& mv /lib/src/mjson/src/mjson.h /lib/build/include/mjson.h \
&& mv /lib/src/mjson/src/mjson.c /lib/build/lib/mjson.c

FROM alpine AS app-build
RUN apk add --no-cache build-base
WORKDIR /src
COPY ./src src
COPY ./Makefile Makefile
COPY --from=libs-build /lib/build .
RUN make server-static && strip build/server-static

FROM scratch
COPY --from=app-build /src/build/server-static /server
EXPOSE 8080/tcp
ENTRYPOINT ["./server"]
