# Techonologie Chmurowe - zadanie 1
Autor: Łukasz Burzak

Aby zbudować obraz kontenera:
```shell
docker buildx build -t polydome/z1 .
```

Aby uruchomić kontener:
```shell
docker run -p 8080:8080 --rm -d --name lb-z1 polydome/z1
```

Aby odczytać logi serwera:
```shell
docker logs lb-z1
```

Aby sprawdzić ile warstw posiada zbudowany obraz:
```shell
docker history polydome/z1
```
