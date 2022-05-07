# Autor: Łukasz Burzak

# ---------- Etap 1 - budowanie bibliotek ----------
FROM alpine AS libs-build

# Pozyskuje kod źródłowy bibliotek z repozytoriów
ADD https://github.com/cesanta/mongoose/archive/refs/tags/7.6.tar.gz ./mongoose.tar.gz
ADD https://github.com/cesanta/mjson/archive/refs/tags/1.2.7.tar.gz ./mjson.tar.gz

# Instaluje pakiety niezbędne do budowania bibliotek
RUN apk add --no-cache build-base \

# Tworzy katalogi w których podejmie działanie
&& mkdir -p /lib/src/mongoose /lib/src/mjson /lib/build/lib /lib/build/include \

# Wypakowuje archiwa z kodem źródłowym
&& tar xzf mongoose.tar.gz -C /lib/src/mongoose --strip-components=1 \
&& tar xzf mjson.tar.gz -C /lib/src/mjson --strip-components=1 \

# Przechodzi do katalogu biblioteki `mongoose` i buduje ją
&& cd /lib/src/mongoose && make mongoose.c mongoose.h \

# Umieszcza pliki wynikowe w wymaganej strukturze
&& mv /lib/src/mongoose/mongoose.h /lib/build/include/mongoose.h \
&& mv /lib/src/mongoose/mongoose.c /lib/build/lib/mongoose.c \
&& mv /lib/src/mjson/src/mjson.h /lib/build/include/mjson.h \
&& mv /lib/src/mjson/src/mjson.c /lib/build/lib/mjson.c


# ---------- Etap 2 - budowanie aplikacji ----------
FROM alpine AS app-build

# Instaluje pakiety niezbędne do budowania bibliotek
RUN apk add --no-cache build-base

# Przenosi pliki źródłowe aplikacji do katalogu roboczego
WORKDIR /src
COPY ./src src
COPY ./Makefile Makefile

# Pozyskuje pliki wynikowe poprzedniego etapu
COPY --from=libs-build /lib/build .

# Buduje plik wykonywalny aplikacji i redukuje jego rozmiar
RUN make server-static && strip build/server-static


# ---------- Etap 3 - wystawianie usługi ----------
FROM scratch

# Pozyskuje plik wynikowy poprzedniego etapu
COPY --from=app-build /src/build/server-static /server

# Otwiera port do komunikacji z aplikacją
EXPOSE 8080/tcp

# Uruchamia aplikację
ENTRYPOINT ["./server"]
